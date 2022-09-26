////////////////////////////////////////////////////////////////////////////////////////////////////
//  OpenSD
//  An open-source userspace driver for Valve's Steam Deck hardware
//
//  Copyright 2022 seek
//  https://gitlab.com/seek-dev/opensd
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
#ifndef __GAMEPAD__DRVCONFIG_HPP__
#define __GAMEPAD__DRVCONFIG_HPP__

#include <vector>


namespace Drivers::Gamepad
{
    
    struct DrvConfig
    {
        struct _axisrange
        {
            double                      Min;
            double                      Max;
        };

        // List of absolute axis ranges by index
        std::vector<_axisrange>         AbsRange;
        std::vector<_axisrange>         RelRange;

        struct _deadzone
        {
            struct  _lr
            {
                double                  L;
                double                  R;
            };

            bool                        Enable;
            _lr                         Stick;
            _lr                         Pad;
            _lr                         Trigg;
        } Deadzone;
    };    

} // namespace Drivers::Gamepad


#endif // __GAMEPAD__DRVCONFIG_HPP__
