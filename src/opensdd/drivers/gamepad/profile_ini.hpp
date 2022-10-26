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
#ifndef __GAMEPAD__PROFILE_INI_HPP__
#define __GAMEPAD__PROFILE_INI_HPP__

#include "profile.hpp"
#include "../../../common/ini.hpp"
#include "../../../common/errors.hpp"
// C++
#include <filesystem>


namespace Drivers::Gamepad
{
    
    // Free function to load (and test) external gamepad profiles
    class ProfileIni
    {
    private:
        struct _axisranges
        {
            struct _minmax
            {
                int32_t             min;
                int32_t             max;
            };
            
            _minmax                 hat;
            _minmax                 stick;
            _minmax                 trigg;
            _minmax                 pad;
            _minmax                 accel;
            _minmax                 gyro;
        };
        
        Drivers::Gamepad::Profile   mProf;
        _axisranges                 mAxisRange;
        Ini::IniFile                mIni;
        uint32_t                    mBindIdCounter;
        
        void                        Reset();
        void                        AddKeyEvent( uint16_t device, uint16_t code );
        void                        AddAbsEvent( uint16_t device, uint16_t code, int32_t min, int32_t max );
        void                        AddRelEvent( uint16_t device, uint16_t code );
        
        void                        GetFeatEnable( std::string key, bool& rValue );
        void                        GetDeadzone( std::string key, double& rValue );
        void                        GetAxisRange( std::string section, std::string key, int32_t& rMin, int32_t& rMax );
        Drivers::Gamepad::Binding   GetEventBinding( std::string key );
        Drivers::Gamepad::Binding   GetCommandBinding( std::string key );
        void                        GetBinding( std::string key, Drivers::Gamepad::Binding& rBind );
        
    public:
        int                         Load( std::filesystem::path filePath, Drivers::Gamepad::Profile& rProf );
        
        ProfileIni();
        ~ProfileIni();
    };
    
} // namespace Drivers::Gamepad


#endif // __GAMEPAD__PROFILE_INI_HPP__
