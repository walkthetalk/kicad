/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2019-2020 KiCad Developers, see AUTHORS.txt for contributors.
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

#include <string>

#include <common.h>
#include <profile.h>

#include <wx/cmdline.h>

#include <pcbnew_utils/board_file_utils.h>
#include <drc_proto/drc_engine.h>

#include <reporter.h>
#include <widgets/progress_reporter.h>

int main( int argc, char *argv[] )
{
    PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
    propMgr.Rebuild();

    STDOUT_REPORTER msgReporter;

    auto brd =  KI_TEST::ReadBoardFromFileOrStream(argv[1]);

    test::DRC_ENGINE drcEngine( brd.get(), &brd->GetDesignSettings() );

    drcEngine.SetLogReporter( &msgReporter );

    try
    {
        drcEngine.LoadRules( wxString( argv[2] ) );
    }
    catch( PARSE_ERROR& err )
    {
        return -1;
    }

    drcEngine.RunTests();

    return 0;
}
