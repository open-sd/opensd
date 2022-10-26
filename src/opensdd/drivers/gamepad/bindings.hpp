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
#ifndef __GAMEPAD__BINDINGS_HPP__
#define __GAMEPAD__BINDINGS_HPP__

// Linux
#include <linux/input.h>
// C++ 
#include <cstdint>
#include <string>


namespace Drivers::Gamepad
{
    // Shorthand enums
    enum
    {
        // Devices
        NONE    = 0,
        GAME    = 100,
        MOTION  = 200,
        MOUSE   = 300,
        COMMAND = 400,
        // Event types
        KEY     = EV_KEY,
        BTN     = EV_KEY,
        ABS     = EV_ABS,
        REL     = EV_REL
    };

    // Event map element
    struct Binding
    {
        uint16_t                dev;            // Determines which uinput device the event is sent to
                                                // Must be: NONE, GAME, MOTION, MOUSE or COMMAND
        uint16_t                type;           // Input event type
        uint16_t                code;           // Input event code
        bool                    dir;            // Axis direction.  true = Axis+, false = Axis-
        std::string             cmd;            // If dev is COMMAND, this string will be executed in a shell environment
        uint32_t                id;             // Unique binding ID for commands, or zero to disable wait_for_exit
        uint64_t                delay;          // Minimum delay between repeated commands
        uint64_t                timestamp;      // Timestamp of binding execution in ms
        
        Binding():
            dev(NONE), type(NONE), code(NONE), dir(false), cmd(""), id(0), delay(0), timestamp(0) {};
        Binding( uint16_t deviceType, uint16_t eventType, uint16_t eventCode, bool direction ):
            dev(deviceType), type(eventType), code(eventCode), dir(direction), cmd(""), id(0), delay(0), timestamp(0) {};
        Binding( std::string s, uint32_t uniqueId, uint64_t repeatDelay ): 
            dev(COMMAND), type(NONE), code(NONE), cmd(s), id(uniqueId), delay(repeatDelay), timestamp(0) {};
    };

    struct BindMap
    {
        struct _dpad
        {
            Binding             up;
            Binding             down;
            Binding             left;
            Binding             right;
        } dpad;

        struct _btn
        {
            Binding             a;
            Binding             b;
            Binding             x;
            Binding             y;
            Binding             l1;
            Binding             l2;
            Binding             l3;
            Binding             l4;
            Binding             l5;
            Binding             r1;
            Binding             r2;
            Binding             r3;
            Binding             r4;
            Binding             r5;
            Binding             menu;
            Binding             options;
            Binding             steam;
            Binding             quick_access;
        } btn;

        struct _trigg
        {
            Binding             l;
            Binding             r;
        } trigg;

        struct _stick
        {
            Binding             up;
            Binding             down;
            Binding             left;
            Binding             right;
            Binding             touch;
            Binding             force;
        };

        struct _sticks
        {
            _stick              l;
            _stick              r;
        } stick;

        struct _touchpad
        {
            Binding             up;
            Binding             down;
            Binding             left;
            Binding             right;
            Binding             rel_x;
            Binding             rel_y;
            Binding             touch;
            Binding             press;
            Binding             force;
        };

        struct _touchpads
        {
            _touchpad           l;
            _touchpad           r;
        } pad;

        struct _accel
        {
            Binding             x_plus;
            Binding             x_minus;
            Binding             y_plus;
            Binding             y_minus;
            Binding             z_plus;
            Binding             z_minus;
        } accel;

        struct _attitude
        {
            Binding             roll_plus;
            Binding             roll_minus;
            Binding             pitch_plus;
            Binding             pitch_minus;
            Binding             yaw_plus;
            Binding             yaw_minus;
        } att;
    };

} // namespace Drivers::Gamepad


#endif // __GAMEPAD__BINDINGS_HPP__
