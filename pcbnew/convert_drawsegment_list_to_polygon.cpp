/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 2015 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 1992-2019 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @file convert_drawsegment_list_to_polygon.cpp
 * @brief functions to convert a shape built with DRAWSEGMENTS to a polygon.
 * expecting the shape describes shape similar to a polygon
 */

#include <trigo.h>
#include <macros.h>

#include <math/vector2d.h>
#include <class_drawsegment.h>
#include <class_module.h>
#include <base_units.h>
#include <convert_basic_shapes_to_polygon.h>
#include <geometry/shape_poly_set.h>
#include <geometry/geometry_utils.h>


/**
 * Function close_ness
 * is a non-exact distance (also called Manhattan distance) used to approximate
 * the distance between two points.
 * The distance is very in-exact, but can be helpful when used
 * to pick between alternative neighboring points.
 * @param aLeft is the first point
 * @param aRight is the second point
 * @return unsigned - a measure of proximity that the caller knows about, in BIU,
 *  but remember it is only an approximation.
 */

static unsigned close_ness(  const wxPoint& aLeft, const wxPoint& aRight )
{
    // Don't need an accurate distance calculation, just something
    // approximating it, for relative ordering.
    return unsigned( std::abs( aLeft.x - aRight.x ) + abs( aLeft.y - aRight.y ) );
}

/**
 * Function close_enough
 * is a local and tunable method of qualifying the proximity of two points.
 *
 * @param aLeft is the first point
 * @param aRight is the second point
 * @param aLimit is a measure of proximity that the caller knows about.
 * @return bool - true if the two points are close enough, else false.
 */
inline bool close_enough( const wxPoint& aLeft, const wxPoint& aRight, unsigned aLimit )
{
    // We don't use an accurate distance calculation, just something
    // approximating it, since aLimit is non-exact anyway except when zero.
    return close_ness( aLeft, aRight ) <= aLimit;
}

/**
 * Function close_st
 * is a local method of qualifying if either the start of end point of a segment is closest to a point.
 *
 * @param aReference is the reference point
 * @param aFirst is the first point
 * @param aSecond is the second point
 * @return bool - true if the the first point is closest to the reference, otherwise false.
 */
inline bool close_st( const wxPoint& aReference, const wxPoint& aFirst, const wxPoint& aSecond )
{
    // We don't use an accurate distance calculation, just something
    // approximating to find the closest to the reference.
    return close_ness( aReference, aFirst ) <= close_ness( aReference, aSecond );
}


/**
 * Searches for a DRAWSEGMENT matching a given end point or start point in a list, and
 * if found, removes it from the TYPE_COLLECTOR and returns it, else returns NULL.
 * @param aPoint The starting or ending point to search for.
 * @param aList The list to remove from.
 * @param aLimit is the distance from \a aPoint that still constitutes a valid find.
 * @return DRAWSEGMENT* - The first DRAWSEGMENT that has a start or end point matching
 *   aPoint, otherwise NULL if none.
 */
static DRAWSEGMENT* findPoint( const wxPoint& aPoint, std::vector< DRAWSEGMENT* >& aList, unsigned aLimit )
{
    unsigned min_d = INT_MAX;
    int      ndx_min = 0;

    // find the point closest to aPoint and perhaps exactly matching aPoint.
    for( size_t i = 0; i < aList.size(); ++i )
    {
        DRAWSEGMENT*    graphic = aList[i];
        unsigned        d;

        switch( graphic->GetShape() )
        {
        case S_ARC:
            if( aPoint == graphic->GetArcStart() || aPoint == graphic->GetArcEnd() )
            {
                aList.erase( aList.begin() + i );
                return graphic;
            }

            d = close_ness( aPoint, graphic->GetArcStart() );
            if( d < min_d )
            {
                min_d = d;
                ndx_min = i;
            }

            d = close_ness( aPoint, graphic->GetArcEnd() );
            if( d < min_d )
            {
                min_d = d;
                ndx_min = i;
            }
            break;

        default:
            if( aPoint == graphic->GetStart() || aPoint == graphic->GetEnd() )
            {
                aList.erase( aList.begin() + i );
                return graphic;
            }

            d = close_ness( aPoint, graphic->GetStart() );
            if( d < min_d )
            {
                min_d = d;
                ndx_min = i;
            }

            d = close_ness( aPoint, graphic->GetEnd() );
            if( d < min_d )
            {
                min_d = d;
                ndx_min = i;
            }
        }
    }

    if( min_d <= aLimit )
    {
        DRAWSEGMENT* graphic = aList[ndx_min];
        aList.erase( aList.begin() + ndx_min );
        return graphic;
    }

    return NULL;
}


/**
 * Function ConvertOutlineToPolygon
 * build a polygon (with holes) from a DRAWSEGMENT list, which is expected to be
 * a outline, therefore a closed main outline with perhaps closed inner outlines.
 * These closed inner outlines are considered as holes in the main outline
 * @param aSegList the initial list of drawsegments (only lines, circles and arcs).
 * @param aPolygons will contain the complex polygon.
 * @param aTolerance is the max distance between points that is still accepted as connected
 *                   (internal units)
 * @param aErrorText is a wxString to return error message.
 * @param aErrorLocation is the optional position of the error in the outline
 */
bool ConvertOutlineToPolygon( std::vector<DRAWSEGMENT*>& aSegList, SHAPE_POLY_SET& aPolygons,
                              wxString* aErrorText, unsigned int aTolerance,
                              wxPoint* aErrorLocation )
{
    if( aSegList.size() == 0 )
        return true;

    // Return value
    bool polygonComplete = true;

    wxString msg;

    // Make a working copy of aSegList, because the list is modified during calculations
    std::vector< DRAWSEGMENT* > segList = aSegList;

    DRAWSEGMENT* graphic;
    wxPoint prevPt;

    // Find edge point with minimum x, this should be in the outer polygon
    // which will define the perimeter polygon polygon.
    wxPoint xmin    = wxPoint( INT_MAX, 0 );
    int     xmini   = 0;

    for( size_t i = 0; i < segList.size(); i++ )
    {
        graphic = (DRAWSEGMENT*) segList[i];

        switch( graphic->GetShape() )
        {
        case S_RECT:
        case S_SEGMENT:
            {
                if( graphic->GetStart().x < xmin.x )
                {
                    xmin    = graphic->GetStart();
                    xmini   = i;
                }

                if( graphic->GetEnd().x < xmin.x )
                {
                    xmin    = graphic->GetEnd();
                    xmini   = i;
                }
            }
            break;

        case S_ARC:
            {
                wxPoint  pstart = graphic->GetArcStart();
                wxPoint  center = graphic->GetCenter();
                double   angle  = -graphic->GetAngle();
                double   radius = graphic->GetRadius();
                int      steps  = GetArcToSegmentCount( radius, aTolerance, angle / 10.0 );
                wxPoint  pt;

                for( int step = 1; step<=steps; ++step )
                {
                    double rotation = ( angle * step ) / steps;

                    pt = pstart;

                    RotatePoint( &pt, center, rotation );

                    if( pt.x < xmin.x )
                    {
                        xmin  = pt;
                        xmini = i;
                    }
                }
            }
            break;

        case S_CIRCLE:
            {
                wxPoint pt = graphic->GetCenter();

                // pt has minimum x point
                pt.x -= graphic->GetRadius();

                // when the radius <= 0, this is a mal-formed circle. Skip it
                if( graphic->GetRadius() > 0 && pt.x < xmin.x )
                {
                    xmin  = pt;
                    xmini = i;
                }
            }
            break;

        case S_CURVE:
            {
                graphic->RebuildBezierToSegmentsPointsList( graphic->GetWidth() );

                for( const wxPoint& pt : graphic->GetBezierPoints())
                {
                    if( pt.x < xmin.x )
                    {
                        xmin  = pt;
                        xmini = i;
                    }
                }
            }
            break;

        case S_POLYGON:
            {
                const SHAPE_POLY_SET poly = graphic->GetPolyShape();
                MODULE*              module = aSegList[0]->GetParentModule();
                double               orientation = module ? module->GetOrientation() : 0.0;
                VECTOR2I             offset = module ? module->GetPosition() : VECTOR2I( 0, 0 );

                for( auto iter = poly.CIterate(); iter; iter++ )
                {
                    VECTOR2I pt = *iter;
                    RotatePoint( pt, orientation );
                    pt += offset;

                    if( pt.x < xmin.x )
                    {
                        xmin.x = pt.x;
                        xmin.y = pt.y;
                        xmini = i;
                    }
                }
            }
            break;

        default:
            break;
        }
    }

    // Grab the left most point, assume its on the board's perimeter, and see if we
    // can put enough graphics together by matching endpoints to formulate a cohesive
    // polygon.

    graphic = (DRAWSEGMENT*) segList[xmini];

    // The first DRAWSEGMENT is in 'graphic', ok to remove it from 'items'
    segList.erase( segList.begin() + xmini );

    // Output the outline perimeter as polygon.
    if( graphic->GetShape() == S_CIRCLE )
    {
        TransformCircleToPolygon( aPolygons, graphic->GetCenter(), graphic->GetRadius(), aTolerance );
    }
    else if( graphic->GetShape() == S_RECT )
    {
        std::vector<wxPoint> pts = graphic->GetRectCorners();

        aPolygons.NewOutline();

        for( const wxPoint& pt : pts )
            aPolygons.Append( pt );
    }
    else if( graphic->GetShape() == S_POLYGON )
    {
        MODULE* module = graphic->GetParentModule();     // NULL for items not in footprints
        double orientation = module ? module->GetOrientation() : 0.0;
        VECTOR2I offset = module ? module->GetPosition() : VECTOR2I( 0, 0 );

        aPolygons.NewOutline();

        for( auto it = graphic->GetPolyShape().CIterate( 0 ); it; it++ )
        {
            auto pt = *it;
            RotatePoint( pt, orientation );
            pt += offset;
            aPolygons.Append( pt );
        }
    }
    else
    {
        // Polygon start point. Arbitrarily chosen end of the
        // segment and build the poly from here.

        wxPoint startPt = graphic->GetShape() == S_ARC ? graphic->GetArcEnd()
                                                       : graphic->GetEnd();

        prevPt = startPt;
        aPolygons.NewOutline();
        aPolygons.Append( prevPt );

        // Do not append the other end point yet of this 'graphic', this first
        // 'graphic' might be an arc or a curve.

        for(;;)
        {
            switch( graphic->GetShape() )
            {
            case S_SEGMENT:
                {
                    wxPoint  nextPt;

                    // Use the line segment end point furthest away from prevPt as we assume
                    // the other end to be ON prevPt or very close to it.

                    if( close_st( prevPt, graphic->GetStart(), graphic->GetEnd() ) )
                        nextPt = graphic->GetEnd();
                    else
                        nextPt = graphic->GetStart();

                    aPolygons.Append( nextPt );
                    prevPt = nextPt;
                }
                break;

            case S_ARC:
                // We do not support arcs in polygons, so approximate an arc with a series of
                // short lines and put those line segments into the !same! PATH.
                {
                    wxPoint pstart  = graphic->GetArcStart();
                    wxPoint pend    = graphic->GetArcEnd();
                    wxPoint pcenter = graphic->GetCenter();
                    double  angle   = -graphic->GetAngle();
                    double  radius  = graphic->GetRadius();
                    int     steps   = GetArcToSegmentCount( radius, aTolerance, angle / 10.0 );

                    if( !close_enough( prevPt, pstart, aTolerance ) )
                    {
                        wxASSERT( close_enough( prevPt, graphic->GetArcEnd(), aTolerance ) );

                        angle = -angle;
                        std::swap( pstart, pend );
                    }

                    wxPoint nextPt;

                    for( int step = 1; step<=steps; ++step )
                    {
                        double rotation = ( angle * step ) / steps;
                        nextPt = pstart;
                        RotatePoint( &nextPt, pcenter, rotation );

                        aPolygons.Append( nextPt );
                    }

                    prevPt = nextPt;
                }
                break;

            case S_CURVE:
                // We do not support Bezier curves in polygons, so approximate with a series
                // of short lines and put those line segments into the !same! PATH.
                {
                    wxPoint nextPt;
                    bool    reverse = false;

                    // Use the end point furthest away from
                    // prevPt as we assume the other end to be ON prevPt or
                    // very close to it.

                    if( close_st( prevPt, graphic->GetStart(), graphic->GetEnd() ) )
                        nextPt = graphic->GetEnd();
                    else
                    {
                        nextPt = graphic->GetStart();
                        reverse = true;
                    }

                    if( reverse )
                    {
                        for( int jj = graphic->GetBezierPoints().size()-1; jj >= 0; jj-- )
                            aPolygons.Append( graphic->GetBezierPoints()[jj] );
                    }
                    else
                    {
                        for( size_t jj = 0; jj < graphic->GetBezierPoints().size(); jj++ )
                            aPolygons.Append( graphic->GetBezierPoints()[jj] );
                    }

                    prevPt = nextPt;
                }
                break;

            default:
                if( aErrorText )
                {
                    msg.Printf( "Unsupported DRAWSEGMENT type %s.",
                                BOARD_ITEM::ShowShape( graphic->GetShape() ) );

                    *aErrorText << msg << "\n";
                }

                if( aErrorLocation )
                    *aErrorLocation = graphic->GetPosition();

                return false;
            }

            // Get next closest segment.

            graphic = findPoint( prevPt, segList, aTolerance );

            // If there are no more close segments, check if the board
            // outline polygon can be closed.

            if( !graphic )
            {
                if( close_enough( startPt, prevPt, aTolerance ) )
                {
                    // Close the polygon back to start point
                    // aPolygons.Append( startPt ); // not needed
                }
                else
                {
                    if( aErrorText )
                    {
                        msg.Printf( _( "Unable to find edge with an endpoint of (%s, %s)." ),
                                    StringFromValue( EDA_UNITS::MILLIMETRES, prevPt.x, true ),
                                    StringFromValue( EDA_UNITS::MILLIMETRES, prevPt.y, true ) );

                        *aErrorText << msg << "\n";
                    }

                    if( aErrorLocation )
                        *aErrorLocation = prevPt;

                    polygonComplete = false;
                    break;
                }
                break;
            }
        }
    }

    int holeNum = -1;

    while( segList.size() )
    {
        // emit a signal layers keepout for every interior polygon left...
        int hole = aPolygons.NewHole();
        holeNum++;

        graphic = (DRAWSEGMENT*) segList[0];
        segList.erase( segList.begin() );

        // Both circles and polygons on the edge cuts layer are closed items that
        // do not connect to other elements, so we process them independently
        if( graphic->GetShape() == S_POLYGON )
        {
            MODULE* module = graphic->GetParentModule();     // NULL for items not in footprints
            double orientation = module ? module->GetOrientation() : 0.0;
            VECTOR2I offset = module ? module->GetPosition() : VECTOR2I( 0, 0 );

            for( auto it = graphic->GetPolyShape().CIterate(); it; it++ )
            {
                auto val = *it;
                RotatePoint( val, orientation );
                val += offset;

                aPolygons.Append( val, -1, hole );
            }
        }
        else if( graphic->GetShape() == S_CIRCLE )
        {
            // make a circle by segments;
            wxPoint  center  = graphic->GetCenter();
            double   angle   = 3600.0;
            wxPoint  start   = center;
            int      radius  = graphic->GetRadius();
            int      steps   = GetArcToSegmentCount( radius, aTolerance, 360.0 );
            wxPoint  nextPt;

            start.x += radius;

            for( int step = 0; step < steps; ++step )
            {
                double rotation = ( angle * step ) / steps;
                nextPt = start;
                RotatePoint( &nextPt.x, &nextPt.y, center.x, center.y, rotation );
                aPolygons.Append( nextPt, -1, hole );
            }
        }
        else if( graphic->GetShape() == S_RECT )
        {
            std::vector<wxPoint> pts = graphic->GetRectCorners();

            for( const wxPoint& pt : pts )
                aPolygons.Append( pt, -1, hole );
        }
        else
        {
            // Polygon start point. Arbitrarily chosen end of the
            // segment and build the poly from here.

            wxPoint startPt( graphic->GetEnd() );
            prevPt = graphic->GetEnd();
            aPolygons.Append( prevPt, -1, hole );

            // do not append the other end point yet, this first 'graphic' might be an arc
            for(;;)
            {
                switch( graphic->GetShape() )
                {
                case S_SEGMENT:
                    {
                        wxPoint nextPt;

                        // Use the line segment end point furthest away from
                        // prevPt as we assume the other end to be ON prevPt or
                        // very close to it.

                        if( close_st( prevPt, graphic->GetStart(), graphic->GetEnd() ) )
                            nextPt = graphic->GetEnd();
                        else
                            nextPt = graphic->GetStart();

                        prevPt = nextPt;
                        aPolygons.Append( prevPt, -1, hole );
                    }
                    break;

                case S_ARC:
                    // Freerouter does not yet understand arcs, so approximate
                    // an arc with a series of short lines and put those
                    // line segments into the !same! PATH.
                    {
                        wxPoint pstart  = graphic->GetArcStart();
                        wxPoint pend    = graphic->GetArcEnd();
                        wxPoint pcenter = graphic->GetCenter();
                        double  angle   = -graphic->GetAngle();
                        int     radius  = graphic->GetRadius();
                        int     steps = GetArcToSegmentCount( radius, aTolerance, angle / 10.0 );

                        if( !close_enough( prevPt, pstart, aTolerance ) )
                        {
                            wxASSERT( close_enough( prevPt, graphic->GetArcEnd(), aTolerance ) );

                            angle = -angle;
                            std::swap( pstart, pend );
                        }

                        wxPoint nextPt;

                        for( int step = 1; step <= steps; ++step )
                        {
                            double rotation = ( angle * step ) / steps;

                            nextPt = pstart;
                            RotatePoint( &nextPt, pcenter, rotation );

                            aPolygons.Append( nextPt, -1, hole );
                        }

                        prevPt = nextPt;
                    }
                    break;

                case S_CURVE:
                    // We do not support Bezier curves in polygons, so approximate
                    // with a series of short lines and put those
                    // line segments into the !same! PATH.
                    {
                        wxPoint  nextPt;
                        bool reverse = false;

                        // Use the end point furthest away from
                        // prevPt as we assume the other end to be ON prevPt or
                        // very close to it.

                        if( close_st( prevPt, graphic->GetStart(), graphic->GetEnd() ) )
                            nextPt = graphic->GetEnd();
                        else
                        {
                            nextPt = graphic->GetStart();
                            reverse = true;
                        }

                        if( reverse )
                        {
                            for( int jj = graphic->GetBezierPoints().size()-1; jj >= 0; jj-- )
                                aPolygons.Append( graphic->GetBezierPoints()[jj], -1, hole );
                        }
                        else
                        {
                            for( const wxPoint& pt : graphic->GetBezierPoints())
                                aPolygons.Append( pt, -1, hole );
                        }

                        prevPt = nextPt;
                    }
                    break;

                default:
                    if( aErrorText )
                    {
                        msg.Printf( "Unsupported DRAWSEGMENT type %s.",
                                    BOARD_ITEM::ShowShape( graphic->GetShape() ) );

                        *aErrorText << msg << "\n";
                    }

                    if( aErrorLocation )
                        *aErrorLocation = graphic->GetPosition();

                    return false;
                }

                // Get next closest segment.

                graphic = findPoint( prevPt, segList, aTolerance );

                // If there are no more close segments, check if polygon
                // can be closed.

                if( !graphic )
                {
                    if( close_enough( startPt, prevPt, aTolerance ) )
                    {
                        // Close the polygon back to start point
                        // aPolygons.Append( startPt, -1, hole );   // not needed
                    }
                    else
                    {
                        if( aErrorText )
                        {
                            msg.Printf( _( "Unable to find edge with an endpoint of (%s, %s)." ),
                                        StringFromValue( EDA_UNITS::MILLIMETRES, prevPt.x, true ),
                                        StringFromValue( EDA_UNITS::MILLIMETRES, prevPt.y, true ) );

                            *aErrorText << msg << "\n";
                        }

                        if( aErrorLocation )
                            *aErrorLocation = prevPt;

                        aPolygons.Hole( 0, holeNum ).SetClosed( false );
                        polygonComplete = false;
                    }
                    break;
                }
            }
        }
    }

    // All of the silliness that follows is to work around the segment iterator
    // while checking for collisions.
    // TODO: Implement proper segment and point iterators that follow std
    for( auto seg1 = aPolygons.IterateSegmentsWithHoles(); seg1; seg1++ )
    {
        auto seg2 = seg1;

        for( ++seg2; seg2; seg2++ )
        {
            // Check for exact overlapping segments.  This is not viewed
            // as an intersection below
            if( *seg1 == *seg2 ||
                    ( ( *seg1 ).A == ( *seg2 ).B && ( *seg1 ).B == ( *seg2 ).A ) )
            {
                if( aErrorLocation )
                {
                    aErrorLocation->x = ( *seg1 ).A.x;
                    aErrorLocation->y = ( *seg1 ).A.y;
                }

                return false;
            }

            if( boost::optional<VECTOR2I> pt = seg1.Get().Intersect( seg2.Get(), true ) )
            {
                if( aErrorLocation )
                {
                    aErrorLocation->x = pt->x;
                    aErrorLocation->y = pt->y;
                }

                return false;
            }
        }
    }

    return polygonComplete;
}

#include <class_board.h>
#include <collectors.h>

/* This function is used to extract a board outlines (3D view, automatic zones build ...)
 * Any closed outline inside the main outline is a hole
 * All contours should be closed, i.e. valid closed polygon vertices
 */
bool BuildBoardPolygonOutlines( BOARD* aBoard, SHAPE_POLY_SET& aOutlines, wxString* aErrorText,
                                unsigned int aTolerance, wxPoint* aErrorLocation )
{
    PCB_TYPE_COLLECTOR  items;
    bool                success = false;

    // Get all the DRAWSEGMENTS and module graphics into 'items',
    // then keep only those on layer == Edge_Cuts.
    static const KICAD_T  scan_graphics[] = { PCB_LINE_T, PCB_MODULE_EDGE_T, EOT };
    items.Collect( aBoard, scan_graphics );

    // Make a working copy of aSegList, because the list is modified during calculations
    std::vector< DRAWSEGMENT* > segList;

    for( int ii = 0; ii < items.GetCount(); ii++ )
    {
        if( items[ii]->GetLayer() == Edge_Cuts )
            segList.push_back( static_cast< DRAWSEGMENT* >( items[ii] ) );
    }

    if( segList.size() )
    {
        success = ConvertOutlineToPolygon( segList, aOutlines, aErrorText, aTolerance,
                                           aErrorLocation );
    }
    else if( aErrorText )
    {
        *aErrorText = _( "No edges found on Edge.Cuts layer." );
    }

    if( !success || !aOutlines.OutlineCount() )
    {
        // Couldn't create a valid polygon outline.  Use the board edge cuts bounding box to
        // create a rectangular outline, or, failing that, the bounding box of the items on
        // the board.

        EDA_RECT bbbox = aBoard->GetBoardEdgesBoundingBox();

        // If null area, uses the global bounding box.
        if( ( bbbox.GetWidth() ) == 0 || ( bbbox.GetHeight() == 0 ) )
            bbbox = aBoard->ComputeBoundingBox();

        // Ensure non null area. If happen, gives a minimal size.
        if( ( bbbox.GetWidth() ) == 0 || ( bbbox.GetHeight() == 0 ) )
            bbbox.Inflate( Millimeter2iu( 1.0 ) );

        aOutlines.RemoveAllContours();
        aOutlines.NewOutline();

        wxPoint corner;
        aOutlines.Append( bbbox.GetOrigin() );

        corner.x = bbbox.GetOrigin().x;
        corner.y = bbbox.GetEnd().y;
        aOutlines.Append( corner );

        aOutlines.Append( bbbox.GetEnd() );

        corner.x = bbbox.GetEnd().x;
        corner.y = bbbox.GetOrigin().y;
        aOutlines.Append( corner );
    }

    return success;
}


/**
 * Get the complete bounding box of the board (including all items)
 */
void buildBoardBoundingBoxPoly( const BOARD* aBoard, SHAPE_POLY_SET& aOutline )
{
    EDA_RECT bbbox = aBoard->GetBoundingBox();

    // If null area, uses the global bounding box.
    if( ( bbbox.GetWidth() ) == 0 || ( bbbox.GetHeight() == 0 ) )
        bbbox = aBoard->ComputeBoundingBox();

    // Ensure non null area. If happen, gives a minimal size.
    if( ( bbbox.GetWidth() ) == 0 || ( bbbox.GetHeight() == 0 ) )
        bbbox.Inflate( Millimeter2iu( 1.0 ) );

    aOutline.RemoveAllContours();
    aOutline.NewOutline();

    wxPoint corner;
    aOutline.Append( bbbox.GetOrigin() );

    corner.x = bbbox.GetOrigin().x;
    corner.y = bbbox.GetEnd().y;
    aOutline.Append( corner );

    aOutline.Append( bbbox.GetEnd() );

    corner.x = bbbox.GetEnd().x;
    corner.y = bbbox.GetOrigin().y;
    aOutline.Append( corner );
}


bool isCopperOutside( const MODULE* aMod, SHAPE_POLY_SET& aShape )
{
    bool padOutside = false;

    for( auto pad : aMod->Pads() )
    {
        SHAPE_POLY_SET padPoly;
        pad->BuildPadShapePolygon( padPoly, wxSize( 0, 0 ) );

        bool padOutsidePad = false;
        for( auto it = padPoly.CIterateSegments( 0 ); it; it++ )
        {
            auto seg = it.Get();
            padOutsidePad |= !aShape.Collide( seg );
        }

        wxPoint padPos = pad->GetPosition();
        wxLogDebug( "Tested pad (%d, %d): %s", padPos.x, padPos.y, padOutsidePad ? "true" : "false" );

        padOutside |= padOutsidePad;
    }

    return padOutside;
}


VECTOR2I projectPointOnSegment( const VECTOR2I& aEndPoint, const SHAPE_POLY_SET& aOutline,
        int aOutlineNum = 0 )
{
    int      minDistance = -1;
    VECTOR2I projPoint;

    for( auto it = aOutline.CIterateSegments( aOutlineNum ); it; it++ )
    {
        auto seg = it.Get();
        int dis = seg.Distance( aEndPoint );

        if( minDistance < 0 || ( dis < minDistance ) )
        {
            minDistance = dis;
            projPoint   = seg.NearestPoint( aEndPoint );
        }
    }

    return projPoint;
}


/**
 * This function is used to extract a board outline for a footprint view.
 *
 * Notes:
 * * Incomplete outlines will be closed by joining the end of the outline
 *   onto the bounding box (by simply projecting the end points) and then take the
 *   area that contains the copper.
 * * If all copper lies inside a closed outline, than that outline will be treated
 *   as an external board outline.
 * * If copper is located outside a closed outline, then that outline will be treated
 *   as a hole, and the outer edge will be formed using the bounding box.
 */
bool BuildFootprintPolygonOutlines( BOARD* aBoard, SHAPE_POLY_SET& aOutlines,
        wxString* aErrorText, unsigned int aTolerance, wxPoint* aErrorLocation )
{
    PCB_TYPE_COLLECTOR  items;

    SHAPE_POLY_SET outlines;

    // Get all the DRAWSEGMENTS and module graphics into 'items',
    // then keep only those on layer == Edge_Cuts.
    static const KICAD_T  scan_graphics[] = { PCB_LINE_T, PCB_MODULE_EDGE_T, EOT };
    items.Collect( aBoard, scan_graphics );

    // Make a working copy of aSegList, because the list is modified during calculations
    std::vector< DRAWSEGMENT* > segList;

    for( int ii = 0; ii < items.GetCount(); ii++ )
    {
        if( items[ii]->GetLayer() == Edge_Cuts )
            segList.push_back( static_cast< DRAWSEGMENT* >( items[ii] ) );
    }

    bool success = ConvertOutlineToPolygon( segList, outlines, aErrorText, aTolerance, aErrorLocation );

    // A closed outline was found
    if( success )
    {
        // If copper is outside a closed polygon, treat it as a hole
        if( isCopperOutside( aBoard->GetFirstModule(), outlines ) )
        {
            buildBoardBoundingBoxPoly( aBoard, aOutlines );

            // Copy all outlines from the conversion as holes into the new outline
            for( int i = 0; i < outlines.OutlineCount(); i++ )
            {
                aOutlines.AddHole( outlines.Outline( i ), -1 );

                for( int j = 0; j < outlines.HoleCount( i ); j++ )
                {
                    aOutlines.AddHole( outlines.Hole( i, j ), -1 );
                }
            }
        }
        // If all copper is inside, then the computed outline is the board outline
        else
        {
            aOutlines = outlines;
        }
    }
    // No board outlines were found, so use the bounding box
    else if( !outlines.OutlineCount() )
    {
        buildBoardBoundingBoxPoly( aBoard, aOutlines );
    }
    // There is an outline present, but it is not closed
    else
    {
        SHAPE_POLY_SET closedPolys;
        SHAPE_POLY_SET openPolys;

        // Extract all polygons to top-level entities
        for( int i = 0; i < outlines.OutlineCount(); i++ )
        {
            SHAPE_LINE_CHAIN chain = outlines.Outline( i );

            // Separate the open and closed polygons
            if( chain.IsClosed() )
            {
                wxLogDebug( "Outline closed" );
                closedPolys.AddOutline( chain );
            }
            else
            {
                wxLogDebug( "Outline open" );
                chain.SetClosed( true );
                openPolys.AddOutline( chain );
            }

            // The ConvertOutlineToPolygon function returns only one main
            // outline and the rest as holes, so we promote the holes
            for( int j = 0; j < outlines.HoleCount( i ); j++ )
            {
                SHAPE_LINE_CHAIN hole = outlines.Hole( i, j );

                if( hole.IsClosed() )
                {
                    wxLogDebug( "Hole closed" );
                    closedPolys.AddOutline( hole );
                }
                else
                {
                    wxLogDebug( "Hole open" );
                    hole.SetClosed( true );
                    openPolys.AddOutline( hole );
                }
            }
        }

        SHAPE_POLY_SET boardBoundingBox;
        buildBoardBoundingBoxPoly( aBoard, boardBoundingBox );

        for( int i = 0; i < openPolys.OutlineCount(); i++ )
        {
            SHAPE_LINE_CHAIN chain = openPolys.Outline( i );


            VECTOR2I startProj = projectPointOnSegment( chain.CPoint( 0 ), openPolys, i );
            VECTOR2I endProj   = projectPointOnSegment( chain.CLastPoint(), openPolys, i );

            wxLogDebug( "Start project: (%d, %d)", startProj.x, startProj.y );
            wxLogDebug( "end project: (%d, %d)", endProj.x, endProj.y );
        }


    }

    return success;
}
