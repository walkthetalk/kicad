/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 1992-2021 KiCad Developers, see AUTHORS.txt for contributors.
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

#ifndef _SCH_SHEEET_PIN_H_
#define _SCH_SHEEET_PIN_H_

#include <sch_text.h>

class KIID;
class LINE_READER;
class SCH_SHEET;


/**
 * Define the edge of the sheet that the sheet pin is positioned.
 *
 * SHEET_LEFT_SIDE = 0: pin on left side
 * SHEET_RIGHT_SIDE = 1: pin on right side
 * SHEET_TOP_SIDE = 2: pin on top side
 * SHEET_BOTTOM_SIDE =3: pin on bottom side
 *
 * For compatibility reasons, this does not follow same values as text orientation.
 */
enum class SHEET_SIDE
{
    LEFT = 0,
    RIGHT,
    TOP,
    BOTTOM,
    UNDEFINED
};


/**
 * Define a sheet pin (label) used in sheets to create hierarchical schematics.
 *
 * A SCH_SHEET_PIN is used to create a hierarchical sheet in the same way a
 * pin is used in a symbol.  It connects the objects in the sheet object
 * to the objects in the schematic page to the objects in the page that is
 * represented by the sheet.  In a sheet object, a SCH_SHEET_PIN must be
 * connected to a wire, bus, or label.  In the schematic page represented by
 * the sheet, it corresponds to a hierarchical label.
 */
class SCH_SHEET_PIN : public SCH_HIERLABEL
{
public:
    SCH_SHEET_PIN( SCH_SHEET* parent, const wxPoint& pos = wxPoint( 0, 0 ),
                   const wxString& text = wxEmptyString );

    // Do not create a copy constructor.  The one generated by the compiler is adequate.

    ~SCH_SHEET_PIN() { }

    static inline bool ClassOf( const EDA_ITEM* aItem )
    {
        return aItem && SCH_SHEET_PIN_T == aItem->Type();
    }

    wxString GetClass() const override
    {
        return wxT( "SCH_SHEET_PIN" );
    }

    bool operator ==( const SCH_SHEET_PIN* aPin ) const;

    /**
     * Return true for items which are moved with the anchor point at mouse cursor
     * and false for items moved with no reference to anchor (usually large items).
     *
     * @return true for a hierarchical sheet pin.
     */
    bool IsMovableFromAnchorPoint() const override { return true; }

    void Print( const RENDER_SETTINGS* aSettings, const wxPoint& aOffset ) override;

    /**
     * Calculate the graphic shape (a polygon) associated to the text.
     *
     * @param aPoints is a buffer to fill with polygon corners coordinates.
     * @param aPos is the position of the shape.
     */
    void CreateGraphicShape( const RENDER_SETTINGS* aSettings,
                             std::vector <wxPoint>& aPoints, const wxPoint& aPos ) const override;

    void SwapData( SCH_ITEM* aItem ) override;

    int GetPenWidth() const override;

    /**
     * Get the sheet label number.
     *
     * @return Number of the sheet label.
     */
    int GetNumber() const { return m_number; }

    /**
     * Set the sheet label number.
     *
     * @param aNumber New sheet number label.
     */
    void SetNumber( int aNumber );

    void SetEdge( SHEET_SIDE aEdge );

    SHEET_SIDE GetEdge() const;

    /**
     * Adjust label position to edge based on proximity to vertical or horizontal edge
     * of the parent sheet.
     */
    void ConstrainOnEdge( wxPoint Pos );

    /**
     * Get the parent sheet object of this sheet pin.
     *
     * @return The sheet that is the parent of this sheet pin or NULL if it does
     *         not have a parent.
     */
    SCH_SHEET* GetParent() const { return (SCH_SHEET*) m_parent; }

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override;
#endif

    // Geometric transforms (used in block operations):

    void Move( const wxPoint& aMoveVector ) override
    {
        Offset( aMoveVector );
    }

    void MirrorVertically( int aCenter ) override;
    void MirrorHorizontally( int aCenter ) override;
    void Rotate( wxPoint aCenter ) override;

    bool Matches( const wxFindReplaceData& aSearchData, void* aAuxData ) const override
    {
        return SCH_ITEM::Matches( GetText(), aSearchData );
    }

    bool Replace( const wxFindReplaceData& aSearchData, void* aAuxData = NULL ) override
    {
        return EDA_TEXT::Replace( aSearchData );
    }

    bool IsReplaceable() const override { return true; }

    void GetEndPoints( std::vector< DANGLING_END_ITEM >& aItemList ) override;

    bool IsConnectable() const override { return true; }

    wxString GetSelectMenuText( EDA_UNITS aUnits ) const override;

    BITMAPS GetMenuImage() const override;

    void SetPosition( const wxPoint& aPosition ) override { ConstrainOnEdge( aPosition ); }

    bool IsPointClickableAnchor( const wxPoint& aPos ) const override
    {
        return m_isDangling && GetPosition() == aPos;
    }

    bool HitTest( const wxPoint& aPosition, int aAccuracy = 0 ) const override;

    EDA_ITEM* Clone() const override;

private:
    int m_number;       ///< Label number use for saving sheet label to file.
                        ///< Sheet label numbering begins at 2.
                        ///< 0 is reserved for the sheet name.
                        ///< 1 is reserve for the sheet file name.

    SHEET_SIDE m_edge;
};

#endif // _SCH_SHEEET_PIN_H_
