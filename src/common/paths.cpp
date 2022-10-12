////////////////////////////////////////////////////////////////////////////////////////////////////
//  OpenSD
//  An open-source userspace driver for Valve's Steam Deck hardware
//
//  Copyright 2022 seek
//  https://gitlab.com/open-sd/opensd
//  Licensed under the GNU GPLv3+
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the 
//  GNU General Public License as published by the Free Software Foundation, either version 3 of 
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this program. 
//  If not, see <https://www.gnu.org/licenses/>.             
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "paths.hpp"
#include "xdg.hpp"
#include "log.hpp"


bool FilePaths::IsInstalled()
{
    namespace fs = std::filesystem;
    
    fs::path            dir;
    fs::path            file;
    

    dir = Xdg::SysDataDir();
    if (dir.empty())
    {
        gLog.Write( Log::DEBUG, "FilePaths::IsInstalled(): XDG system data dir is not defined." );
        return false;
    }
    
    dir += "/opensd/";
    if (!fs::exists(dir))
    {
        gLog.Write( Log::DEBUG, "FilePaths::IsInstalled(): Path '" + dir.string() + "' does not exist." );
        return false;
    }

    if (!fs::is_directory(dir))
    {
        gLog.Write( Log::DEBUG, "FilePaths::IsInstalled(): Path '" + dir.string() + "' is not a directory." );
        return false;
    }
    
    file = dir + "
    
    return true;
}