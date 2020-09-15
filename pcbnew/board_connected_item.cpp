/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 2012 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 1992-2020 KiCad Developers, see AUTHORS.txt for contributors.
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

#include <fctsys.h>
#include <class_board.h>
#include <class_board_item.h>
#include <connectivity/connectivity_data.h>
#include <drc/drc_engine.h>

using namespace std::placeholders;

BOARD_CONNECTED_ITEM::BOARD_CONNECTED_ITEM( BOARD_ITEM* aParent, KICAD_T idtype ) :
    BOARD_ITEM( aParent, idtype ), m_netinfo( NETINFO_LIST::OrphanedItem() )
{
    m_localRatsnestVisible = true;
}


bool BOARD_CONNECTED_ITEM::SetNetCode( int aNetCode, bool aNoAssert )
{
    if( !IsOnCopperLayer() )
        aNetCode = 0;

    // if aNetCode < 0 ( typically NETINFO_LIST::FORCE_ORPHANED )
    // or no parent board,
    // set the m_netinfo to the dummy NETINFO_LIST::ORPHANED

    BOARD* board = GetBoard();

    if( ( aNetCode >= 0 ) && board )
        m_netinfo = board->FindNet( aNetCode );
    else
        m_netinfo = NETINFO_LIST::OrphanedItem();

    if( !aNoAssert )
        wxASSERT( m_netinfo );

    return ( m_netinfo != NULL );
}


// This method returns the Default netclass for nets which don't have their own.
NETCLASS* BOARD_CONNECTED_ITEM::GetEffectiveNetclass() const
{
    // NB: we must check the net first, as when it is 0 GetNetClass() will return the
    // orphaned net netclass, not the default netclass.
    if( m_netinfo->GetNet() == 0 )
        return GetBoard()->GetDesignSettings().GetDefault();
    else
        return GetNetClass();
}


/*
 * Clearances exist in a hiearchy.  If a given level is specified then the remaining levels
 * are NOT consulted.
 *
 * LEVEL 1: (highest priority) local overrides (pad, footprint, etc.)
 * LEVEL 2: Rules
 * LEVEL 3: Accumulated local settings, netclass settings, & board design settings
 */
int BOARD_CONNECTED_ITEM::GetClearance( PCB_LAYER_ID aLayer, BOARD_ITEM* aItem,
                                        wxString* aSource ) const
{
    // No clearance if "this" is not (yet) linked to a board therefore no available netclass
    if( !GetBoard() )
        return 0;

    std::shared_ptr<DRC_ENGINE> drcEngine = GetBoard()->GetDesignSettings().m_DRCEngine;

    DRC_CONSTRAINT constraint = drcEngine->EvalRulesForItems( DRC_CONSTRAINT_TYPE_CLEARANCE,
                                                              this, aItem, aLayer );

    if( constraint.Value().HasMin() )
    {
        if( aSource )
            *aSource = constraint.GetName();

        return constraint.Value().Min();
    }

    return 0;
}


// Note: do NOT return a std::shared_ptr from this.  It is used heavily in DRC, and the
// std::shared_ptr stuff shows up large in performance profiling.
NETCLASS* BOARD_CONNECTED_ITEM::GetNetClass() const
{
    NETCLASS* netclass = m_netinfo->GetNetClass();

    if( netclass )
        return netclass;
    else
        return GetBoard()->GetDesignSettings().GetDefault();
}


wxString BOARD_CONNECTED_ITEM::GetNetClassName() const
{
    return m_netinfo->GetClassName();
}


static struct BOARD_CONNECTED_ITEM_DESC
{
    BOARD_CONNECTED_ITEM_DESC()
    {
        ENUM_MAP<PCB_LAYER_ID>& layerEnum = ENUM_MAP<PCB_LAYER_ID>::Instance();

        if( layerEnum.Choices().GetCount() == 0 )
        {
            layerEnum.Undefined( UNDEFINED_LAYER );

            for( LSEQ seq = LSET::AllLayersMask().Seq(); seq; ++seq )
                layerEnum.Map( *seq, LSET::Name( *seq ) );
        }

        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        REGISTER_TYPE( BOARD_CONNECTED_ITEM );
        propMgr.InheritsAfter( TYPE_HASH( BOARD_CONNECTED_ITEM ), TYPE_HASH( BOARD_ITEM ) );

        propMgr.ReplaceProperty( TYPE_HASH( BOARD_ITEM ), _( "Layer" ),
                new PROPERTY_ENUM<BOARD_CONNECTED_ITEM, PCB_LAYER_ID, BOARD_ITEM>( _( "Layer" ),
                    &BOARD_CONNECTED_ITEM::SetLayer, &BOARD_CONNECTED_ITEM::GetLayer ) );

        propMgr.AddProperty( new PROPERTY_ENUM<BOARD_CONNECTED_ITEM, int>( _( "Net" ),
                    &BOARD_CONNECTED_ITEM::SetNetCode, &BOARD_CONNECTED_ITEM::GetNetCode ) );
        propMgr.AddProperty( new PROPERTY<BOARD_CONNECTED_ITEM, wxString>( _( "NetName" ),
                    NO_SETTER( BOARD_CONNECTED_ITEM, wxString ), &BOARD_CONNECTED_ITEM::GetNetname ) );
        propMgr.AddProperty( new PROPERTY<BOARD_CONNECTED_ITEM, wxString>( _( "NetClass" ),
                    NO_SETTER( BOARD_CONNECTED_ITEM, wxString ), &BOARD_CONNECTED_ITEM::GetNetClassName ) );
    }
} _BOARD_CONNECTED_ITEM_DESC;
