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
#include "input_event_names.hpp"
#include "log.hpp"


int EvName::GetCode( uint16_t evType, std::string codeName )
{
    switch (evType)
    {
        case EV_KEY:
            // Make sure key name exists in map
            if (!KEY_MAP.count( codeName ))
            {
                gLog.Write( Log::DEBUG, "EvName::GetCode(): Invalid or unknown KEY name specified." );
                return -1;
            }
            
            // Return with mapped event code
            return KEY_MAP.at( codeName );
        break;
        
        case EV_ABS:
            // Make sure absolute axis name exists in map
            if (!ABS_MAP.count( codeName ))
            {
                gLog.Write( Log::DEBUG, "EvName::GetCode(): Invalid or unknown ABS name specified." );
                return -1;
            }
            
            // Return with mapped event code
            return ABS_MAP.at( codeName );
        break;
        
        case EV_REL:
            // Make sure relative axis name exists in map
            if (!REL_MAP.count( codeName ))
                return -1;
            
            // Return with mapped event code
            return REL_MAP.at( codeName );
        break;
        
        default:
            gLog.Write( Log::DEBUG, "EvName::GetCode(): Invalid type specified." );
            return -1;
        break;
    }
}
