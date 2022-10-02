////////////////////////////////////////////////////////////////////////////////////////////////////
//  OpenSD
//  An open-source userspace driver for Valve's Steam Deck hardware
//
//  Copyright 2022 seek
//  https://gitlab.com/open-sd/opensd
//  Licensed under the GNU GPLv3+
//
//    This program is free software: you can redistribute it and/or modify it under the terms of 
//    the GNU General Public License as published by the Free Software Foundation, either 
//    version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
//    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//    See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this program. 
//    If not, see <https://www.gnu.org/licenses/>.             
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __APPINFO_HPP__
#define __APPINFO_HPP__

#include <string>


namespace AppInfo
{
    const std::string   PROJECT         = "OpenSD";
    const std::string   PROJECT_PAGE    = "https://gitlab.com/open-sd/opensd";
    const std::string   APP_NAME        = "OpenSD daemon";
    const std::string   APP_SHORT_NAME  = "opensdd";
    const std::string   AUTHOR          = "seek";
    const std::string   COPYRIGHT_YEAR  = "2022";
    const std::string   MAJOR_VER       = "0";
    const std::string   MINOR_VER       = "33";
    const std::string   SPDX_LICENSE    = "GPL-3.0-or-later";
    const std::string   INFO_BLOCK      =
    {
        APP_NAME + "\n" +
        "Copyright " + COPYRIGHT_YEAR + " " + AUTHOR + "\n" +
        "Version " + MAJOR_VER + "." + MINOR_VER + "\n" +
        PROJECT_PAGE + "\n" +
        "Licensed under the GNU GPLv3+\n"
    };
    const std::string   LICENSE_BLOCK   =
    {
        "    This program comes with ABSOLUTELY NO WARRANTY.\n"
        "    This is free software, and you are welcome to redistribute it under certain\n"
        "    conditions.  See https://gitlab.com/open-sd/opensd for license.\n"
    };
    const std::string   HELP_BLOCK      =
    {
        "  Usage:\n"
        "\n"
        "    -h    --help             Show this help message.\n"
        "    -v    --version          Output version number and exit.\n"
        "    -l    --log-level        Set minumum logging level.  Default: 'warn'\n"
        "                             Valid options are:\n"
        "                                 verbose, debug, info, warn, error\n"
    };
} // namespace AppInfo


// Prototype CLI
/*
OpenSD Daemon
Copyright 2022 seek
Version 0.000
Licensed under the GNU GPLv3+
    
    This program comes with ABSOLUTELY NO WARRANTY. 
    This is free software, and you are welcome to redistribute it under certain
    conditions.  See https://gitlab.com/open-sd/opensd for license.

  Usage:
    
    -h      --help              Show this help message.
    -v      --version           Output version number and exit.
    -l      --log-level         Set minumum logging level.  Default: 'warn'
                                Valid options are:
                                    verbose, debug, info, warn, error
*/




#endif // __APPINFO_HPP__
