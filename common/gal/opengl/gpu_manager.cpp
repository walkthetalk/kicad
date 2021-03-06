/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright 2013-2017 CERN
 * Copyright (C) 2021 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * @author Maciej Suminski <maciej.suminski@cern.ch>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <gal/opengl/gpu_manager.h>
#include <gal/opengl/cached_container_gpu.h>
#include <gal/opengl/cached_container_ram.h>
#include <gal/opengl/noncached_container.h>
#include <gal/opengl/shader.h>
#include <gal/opengl/utils.h>

#include <typeinfo>
#include <confirm.h>
#include <trace_helpers.h>

#ifdef KICAD_GAL_PROFILE
#include <profile.h>
#include <wx/log.h>
#endif /* KICAD_GAL_PROFILE */

using namespace KIGFX;

GPU_MANAGER* GPU_MANAGER::MakeManager( VERTEX_CONTAINER* aContainer )
{
    if( aContainer->IsCached() )
        return new GPU_CACHED_MANAGER( aContainer );
    else
        return new GPU_NONCACHED_MANAGER( aContainer );
}


GPU_MANAGER::GPU_MANAGER( VERTEX_CONTAINER* aContainer ) :
        m_isDrawing( false ),
        m_container( aContainer ),
        m_shader( nullptr ),
        m_shaderAttrib( 0 ),
        m_enableDepthTest( true )
{
}


GPU_MANAGER::~GPU_MANAGER()
{
}


void GPU_MANAGER::SetShader( SHADER& aShader )
{
    m_shader = &aShader;
    m_shaderAttrib = m_shader->GetAttribute( "attrShaderParams" );

    if( m_shaderAttrib == -1 )
    {
        DisplayError( nullptr, wxT( "Could not get the shader attribute location" ) );
    }
}


// Cached manager
GPU_CACHED_MANAGER::GPU_CACHED_MANAGER( VERTEX_CONTAINER* aContainer ) :
        GPU_MANAGER( aContainer ),
        m_buffersInitialized( false ),
        m_indicesPtr( nullptr ),
        m_indicesBuffer( 0 ),
        m_indicesSize( 0 ),
        m_indicesCapacity( 0 )
{
    // Allocate the biggest possible buffer for indices
    resizeIndices( aContainer->GetSize() );
}


GPU_CACHED_MANAGER::~GPU_CACHED_MANAGER()
{
    if( m_buffersInitialized )
    {
        if( glBindBuffer )
            glBindBuffer( GL_ARRAY_BUFFER, 0 );

        if( glDeleteBuffers )
            glDeleteBuffers( 1, &m_indicesBuffer );
    }
}


void GPU_CACHED_MANAGER::BeginDrawing()
{
    wxASSERT( !m_isDrawing );

    if( !m_buffersInitialized )
    {
        glGenBuffers( 1, &m_indicesBuffer );
        checkGlError( "generating vertices buffer", __FILE__, __LINE__ );
        m_buffersInitialized = true;
    }

    if( m_container->IsDirty() )
        resizeIndices( m_container->GetSize() );

    // Number of vertices to be drawn in the EndDrawing()
    m_indicesSize = 0;
    // Set the indices pointer to the beginning of the indices-to-draw buffer
    m_indicesPtr = m_indices.get();

    m_isDrawing = true;
}


void GPU_CACHED_MANAGER::DrawIndices( unsigned int aOffset, unsigned int aSize )
{
    wxASSERT( m_isDrawing );

    // Copy indices of items that should be drawn to GPU memory
    for( unsigned int i = aOffset; i < aOffset + aSize; *m_indicesPtr++ = i++ )
        ;

    m_indicesSize += aSize;
}


void GPU_CACHED_MANAGER::DrawAll()
{
    wxASSERT( m_isDrawing );

    for( unsigned int i = 0; i < m_indicesSize; *m_indicesPtr++ = i++ )
        ;

    m_indicesSize = m_container->GetSize();
}


void GPU_CACHED_MANAGER::EndDrawing()
{
#ifdef KICAD_GAL_PROFILE
    PROF_COUNTER totalRealTime;
#endif /* KICAD_GAL_PROFILE */

    wxASSERT( m_isDrawing );

    CACHED_CONTAINER* cached = static_cast<CACHED_CONTAINER*>( m_container );

    if( cached->IsMapped() )
        cached->Unmap();

    if( m_indicesSize == 0 )
    {
        m_isDrawing = false;
        return;
    }

    if( m_enableDepthTest )
        glEnable( GL_DEPTH_TEST );
    else
        glDisable( GL_DEPTH_TEST );

    // Prepare buffers
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );

    // Bind vertices data buffers
    glBindBuffer( GL_ARRAY_BUFFER, cached->GetBufferHandle() );
    glVertexPointer( COORD_STRIDE, GL_FLOAT, VERTEX_SIZE, (GLvoid*) COORD_OFFSET );
    glColorPointer( COLOR_STRIDE, GL_UNSIGNED_BYTE, VERTEX_SIZE, (GLvoid*) COLOR_OFFSET );

    if( m_shader != nullptr ) // Use shader if applicable
    {
        m_shader->Use();
        glEnableVertexAttribArray( m_shaderAttrib );
        glVertexAttribPointer( m_shaderAttrib, SHADER_STRIDE, GL_FLOAT, GL_FALSE, VERTEX_SIZE,
                               (GLvoid*) SHADER_OFFSET );
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_indicesSize * sizeof( int ), (GLvoid*) m_indices.get(),
                  GL_DYNAMIC_DRAW );

    glDrawElements( GL_TRIANGLES, m_indicesSize, GL_UNSIGNED_INT, nullptr );

#ifdef KICAD_GAL_PROFILE
    wxLogTrace( traceGalProfile, wxT( "Cached manager size: %d" ), m_indicesSize );
#endif /* KICAD_GAL_PROFILE */

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    cached->ClearDirty();

    // Deactivate vertex array
    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );

    if( m_shader != nullptr )
    {
        glDisableVertexAttribArray( m_shaderAttrib );
        m_shader->Deactivate();
    }

    m_isDrawing = false;

#ifdef KICAD_GAL_PROFILE
    totalRealTime.Stop();
    wxLogTrace( traceGalProfile, wxT( "GPU_CACHED_MANAGER::EndDrawing(): %.1f ms" ),
                totalRealTime.msecs() );
#endif /* KICAD_GAL_PROFILE */
}


void GPU_CACHED_MANAGER::resizeIndices( unsigned int aNewSize )
{
    if( aNewSize > m_indicesCapacity )
    {
        m_indicesCapacity = aNewSize;
        m_indices.reset( new GLuint[m_indicesCapacity] );
    }
}


// Noncached manager
GPU_NONCACHED_MANAGER::GPU_NONCACHED_MANAGER( VERTEX_CONTAINER* aContainer ) :
        GPU_MANAGER( aContainer )
{
}


void GPU_NONCACHED_MANAGER::BeginDrawing()
{
    // Nothing has to be prepared
}


void GPU_NONCACHED_MANAGER::DrawIndices( unsigned int aOffset, unsigned int aSize )
{
    wxASSERT_MSG( false, wxT( "Not implemented yet" ) );
}


void GPU_NONCACHED_MANAGER::DrawAll()
{
    // This is the default use case, nothing has to be done
    // The real rendering takes place in the EndDrawing() function
}


void GPU_NONCACHED_MANAGER::EndDrawing()
{
#ifdef KICAD_GAL_PROFILE
    PROF_COUNTER totalRealTime;
#endif /* KICAD_GAL_PROFILE */

    if( m_container->GetSize() == 0 )
        return;

    VERTEX*  vertices = m_container->GetAllVertices();
    GLfloat* coordinates = (GLfloat*) ( vertices );
    GLubyte* colors = (GLubyte*) ( vertices ) + COLOR_OFFSET;

    if( m_enableDepthTest )
        glEnable( GL_DEPTH_TEST );
    else
        glDisable( GL_DEPTH_TEST );

    // Prepare buffers
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );

    glVertexPointer( COORD_STRIDE, GL_FLOAT, VERTEX_SIZE, coordinates );
    glColorPointer( COLOR_STRIDE, GL_UNSIGNED_BYTE, VERTEX_SIZE, colors );

    if( m_shader != nullptr ) // Use shader if applicable
    {
        GLfloat* shaders = (GLfloat*) ( vertices ) + SHADER_OFFSET / sizeof( GLfloat );

        m_shader->Use();
        glEnableVertexAttribArray( m_shaderAttrib );
        glVertexAttribPointer( m_shaderAttrib, SHADER_STRIDE, GL_FLOAT, GL_FALSE, VERTEX_SIZE,
                               shaders );
    }

    glDrawArrays( GL_TRIANGLES, 0, m_container->GetSize() );

#ifdef KICAD_GAL_PROFILE
    wxLogTrace( traceGalProfile, wxT( "Noncached manager size: %d" ), m_container->GetSize() );
#endif /* KICAD_GAL_PROFILE */

    // Deactivate vertex array
    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );

    if( m_shader != nullptr )
    {
        glDisableVertexAttribArray( m_shaderAttrib );
        m_shader->Deactivate();
    }

    m_container->Clear();

#ifdef KICAD_GAL_PROFILE
    totalRealTime.Stop();
    wxLogTrace( traceGalProfile, wxT( "GPU_NONCACHED_MANAGER::EndDrawing(): %.1f ms" ),
                totalRealTime.msecs() );
#endif /* KICAD_GAL_PROFILE */
}

void GPU_MANAGER::EnableDepthTest( bool aEnabled )
{
    m_enableDepthTest = aEnabled;
}
