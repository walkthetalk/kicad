/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2021 Roberto Fernandez Bautista <roberto.fer.bau@gmail.com>
 * Copyright (C) 2021 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <tuple>
#include <unit_test_utils/unit_test_utils.h>

#include <geometry/shape_line_chain.h>
#include <geometry/shape_poly_set.h>

#include "fixtures_geometry.h"


BOOST_AUTO_TEST_SUITE( CurvedPolys )


/**
 * Simplify the polygon a large number of times and check that the area
 * does not change and also that the arcs are the same before and after
 */
BOOST_AUTO_TEST_CASE( TestSimplify )
{
    KI_TEST::CommonTestData               testData;

    std::map<std::string, SHAPE_POLY_SET> polysToTest =
    {
        { "Case 1: Single polygon", testData.holeyCurvedPolySingle },
        { "Case 2: Multi polygon", testData.holeyCurvedPolyMulti }
    };

    for( std::pair<std::string, SHAPE_POLY_SET> testCase : polysToTest )
    {
        BOOST_TEST_CONTEXT( testCase.first )
        {
            SHAPE_POLY_SET testPoly = testCase.second;

            double originalArea = testPoly.Area();

            std::vector<SHAPE_ARC> originalArcs;
            testPoly.GetArcs( originalArcs );

            for( int i = 1; i <= 3; i++ )
            {
                BOOST_TEST_CONTEXT( "Simplify Iteration " << i )
                {
                    testPoly.Simplify( SHAPE_POLY_SET::POLYGON_MODE::PM_FAST );

                    std::vector<SHAPE_ARC> foundArcs;
                    testPoly.GetArcs( foundArcs );

                    BOOST_CHECK_EQUAL( testPoly.Area(), originalArea );
                    BOOST_CHECK_EQUAL( originalArcs.size(), foundArcs.size() );
                    KI_TEST::CheckUnorderedMatches( originalArcs, foundArcs,
                                                    []( const SHAPE_ARC& aA, const SHAPE_ARC& aB ) -> bool
                                                    {
                                                        // We accept that the arcs could be reversed after Simplify
                                                        return aA == aB || aA.Reversed() == aB;
                                                    } );

                }
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
