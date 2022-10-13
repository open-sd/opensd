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
#include "files.hpp"
#include "../common/xdg.hpp"
#include "../common/log.hpp"
// Generated by cmake
#include "config.hpp"
// C++
#include <filesystem>


bool Files::IsInstalled()
{
    namespace fs = std::filesystem;
    
    fs::path            path;
    

    path = Config::INSTALL_DATA_DIR;
    if (!fs::exists(path))
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, "Path '" + path.string() + "' does not exist." );
        return false;
    }

    if (!fs::is_directory(path))
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, ": Path '" + path.string() + "' is not a directory." );
        return false;
    }

    path = Config::INSTALL_DATA_CONFIG_DIR;
    if (!fs::exists(path))
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, ": Path '" + path.string() + "' does not exist." );
        return false;
    }

    if (!fs::is_directory(path))
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, ": Path '" + path.string() + "' is not a directory." );
        return false;
    }
    
    path = Config::INSTALL_DATA_CONFIG_DIR + Config::DEFAULT_CONFIG_FILENAME;
    if (!fs::exists(path))
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, ": File '" + path.string() + "' does not exist." );
        return false;
    }
    
    if (!fs::is_regular_file(path))
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, ": '" + path.string() + "' is not a regular file." );
        return false;
    }

    path = Config::INSTALL_DATA_PROFILE_DIR;
    if (!fs::exists(path))
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, ": Path '" + path.string() + "' does not exist." );
        return false;
    }

    if (!fs::is_directory(path))
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, ": Path '" + path.string() + "' is not a directory." );
        return false;
    }
    
    path = Config::INSTALL_DATA_PROFILE_DIR + Config::DEFAULT_PROFILE_FILENAME;
    if (!fs::exists(path))
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, ": File '" + path.string() + "' does not exist." );
        return false;
    }
    
    if (!fs::is_regular_file(path))
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, ": '" + path.string() + "' is not a regular file." );
        return false;
    }
    
    // Return success if nothing missing
    return true;
}



bool Files::HasSysConfig()
{
    // TODO 
    
    return true;
}