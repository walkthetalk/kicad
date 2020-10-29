/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015-2016 Mario Luzeiro <mrluzeiro@ua.pt>
 * Copyright (C) 1992-2016 KiCad Developers, see AUTHORS.txt for contributors.
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

/**
 * @file  citemlayercsg2d.cpp
 * @brief
 */

#include "citemlayercsg2d.h"
#include "3d_fastmath.h"
#include <wx/debug.h>

CITEMLAYERCSG2D::CITEMLAYERCSG2D( const COBJECT2D* aObjectA,
        std::vector<const COBJECT2D*>* aObjectB, const COBJECT2D* aObjectC,
        const BOARD_ITEM& aBoardItem )
        : COBJECT2D( OBJECT2D_TYPE::CSG, aBoardItem ),
          m_objectA( aObjectA ),
          m_objectB( aObjectB ),
          m_objectC( aObjectC )
{
    wxASSERT( aObjectA );

    m_bbox.Reset();
    m_bbox.Set( aObjectA->GetBBox() );
    m_bbox.ScaleNextUp();
    m_centroid = m_bbox.GetCenter();

    wxASSERT( m_bbox.IsInitialized() );
}


CITEMLAYERCSG2D::~CITEMLAYERCSG2D()
{
    if( ((void*)m_objectB != CSGITEM_EMPTY) &&
        ((void*)m_objectB != CSGITEM_FULL) )
    {
        delete m_objectB;
        m_objectB = NULL;
    }
}


bool CITEMLAYERCSG2D::Intersects( const CBBOX2D &aBBox ) const
{
    return m_bbox.Intersects( aBBox );
    // !TODO: improove this implementation
    //return false;
}


bool CITEMLAYERCSG2D::Overlaps( const CBBOX2D &aBBox ) const
{
    // NOT IMPLEMENTED
    return false;
}

// Based on ideas and implementation by Nick Chapman
// http://homepages.paradise.net.nz/nickamy/raytracer/raytracer.htm
bool CITEMLAYERCSG2D::Intersect( const RAYSEG2D &aSegRay,
                                 float *aOutT,
                                 SFVEC2F *aNormalOut ) const
{
    wxASSERT( aOutT );
    wxASSERT( aNormalOut );

    if( m_objectA->GetObjectType() == OBJECT2D_TYPE::DUMMYBLOCK )
        return false;

    float currentRayDist;
    SFVEC2F currentRayPos;
    SFVEC2F currentNormal;
    RAYSEG2D currentRay = aSegRay;

    if( m_objectA->IsPointInside( aSegRay.m_Start ) )
    {
        // start ray point off where it is now (at the origin)
        currentRayDist = 0.0f;
        currentRayPos = aSegRay.m_Start;
    }
    else
    {
        //move ray point to start of main object
        if( !m_objectA->Intersect( aSegRay, &currentRayDist, &currentNormal ) )
            return false;

        currentRayPos = aSegRay.atNormalized( currentRayDist + 0.01f );

        currentRay = RAYSEG2D( currentRayPos, aSegRay.m_End );
    }

    currentRayDist = 0.0f;

    //wxASSERT( (currentRayDist >= 0.0f) && (currentRayDist <= 1.0f) );


    // move through the union of subtracted regions
    if( m_objectB )
    {
        for( unsigned int l = 0; l < 4; ++l )
        {
            bool wasInsideSubVol = false;

            //check against all subbed objects
            for( unsigned int i = 0; i < m_objectB->size(); ++i )
            {
                if( ((const COBJECT2D *)(*m_objectB)[i])->IsPointInside( currentRayPos ) )
                {
                    // ray point is inside a subtracted region,  so move it to the end of the
                    // subtracted region
                    float hitDist;
                    SFVEC2F tmpNormal;
                    if( !((const COBJECT2D *)(*m_objectB)[i])->Intersect( currentRay,
                                                                          &hitDist,
                                                                          &tmpNormal ) )
                        return false; // ray hit main object but did not leave subtracted volume

                    wxASSERT( hitDist <= 1.0f );

                    if( hitDist > currentRayDist )
                    {
                        wasInsideSubVol = true;

                        currentRayPos = currentRay.atNormalized( glm::min( hitDist + 0.0001f, 1.0f ) );

                        currentRayDist = 0.0001f;

                        currentRay = RAYSEG2D( currentRayPos, aSegRay.m_End );

                        currentNormal = tmpNormal * -1.0f;
                    }
                }
            }

            if( !wasInsideSubVol )
            {
                break;
            }
        }
    }

    *aNormalOut = currentNormal;
    *aOutT      = glm::min( glm::max( 1.0f - glm::length( currentRayPos - aSegRay.m_End ) / aSegRay.m_Length, 0.0f ), 1.0f );
    return true;
}


INTERSECTION_RESULT CITEMLAYERCSG2D::IsBBoxInside( const CBBOX2D &aBBox ) const
{

    // !TODO:

    return INTERSECTION_RESULT::MISSES;
}


bool CITEMLAYERCSG2D::IsPointInside( const SFVEC2F &aPoint ) const
{
    // Perform the operation (A - B) /\ C

    if( m_objectA->IsPointInside( aPoint ) )
    {

        if( m_objectB != CSGITEM_EMPTY)
            for( unsigned int i = 0; i< m_objectB->size(); i++ )
            {
                if( (*m_objectB)[i]->IsPointInside( aPoint ) )
                    return false;
            }

        // !TODO: not yet implemented
        //if( m_objectC && m_objectC != CSGITEM_FULL )
        //    return m_objectC->IsPointInside( aPoint );

        return true;
    }

    return false;
}
