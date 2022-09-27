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
#include "profile.hpp"
#include "../../../common/errors.hpp"
#include "../../../common/log.hpp"
#include "../../../common/ini.hpp"
#include "../../../common/input_event_names.hpp"
// C++
#include <fstream>


int Drivers::Gamepad::LoadGamepadProfileFromFile( std::filesystem::path filePath, Drivers::Gamepad::Profile& rProf )
{
    Profile             temp_prof = {};
    std::ifstream       file;
    Ini::IniFile        ini;
    int                 result;
    
    rProf = temp_prof;
    namespace fs = std::filesystem;
    
    if (!fs::exists(filePath))
    {
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::LoadGpProfileFromFile(): File '" + filePath.string() + "' not found." );
        return Err::CANNOT_OPEN;
    }
    
    // Test open to make sure we have permissions, etc.
    file.open( filePath );
    if (!file.is_open())
    {
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::LoadGpProfileFromFile(): Failed to open file '" + filePath.string() + "'" );
        return Err::CANNOT_OPEN;
    }
    file.close();
    
    result = ini.LoadFile( filePath );
    if (result != Err::OK)
    {
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::LoadGpProfileFromFile(): Failed to parse file '" + filePath.string() + "'" );
        return result;
    }
    
    // TODO:  Read ini values 
    
    
    return Err::OK;
    
}