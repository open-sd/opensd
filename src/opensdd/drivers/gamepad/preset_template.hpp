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
#ifndef __GAMEPAD__PRESET_TEMPLATE_HPP__
#define __GAMEPAD__PRESET_TEMPLATE_HPP__

#include "profile.hpp"
#include <linux/input.h>


namespace Drivers::Gamepad::Presets
{
    // This is the template that the profile ini get loaded on top of.
    // We do this because there are things we want to default to if not
    // specified in the config.  There are also some things we want to 
    // pass on to the initialization, but aren't worth the time or errors
    // to leave to the user to configure.
    const Drivers::Gamepad::Profile TEMPLATE =
    {
        .profile_name               = "Default OpenSD Profile",
        .features
        {
            .ff                     = false,
            .motion                 = false,
            .mouse                  = false,
            .lizard                 = false
        },
        .dz
        {
            .stick
            {
                .l                  = 0.00,
                .r                  = 0.00
            },
            .pad
            {
                .l                  = 0.00,
                .r                  = 0.00
            },
        },
        .dev
        {
            .gamepad
            {
                .name_override      = "OpenSD Gamepad Device",
                .key_list
                {
                    // All buttons will be enabled just by binding them
                    // in the profile ini.  This section will be filled
                    // in by the LoadGamepadProfileFromFile() function.
                },
                .abs_list
                {
                    // Axes ranges need to be defined in the profile ini
                    // or binding them and using the defaults?
                },
                .rel_list
                {
                    // All relative axes will be enabled just by binding
                    // them in the profile ini.  This section will be
                    // filled in the by the LoadGamepadProfileFromFile()
                    // function.
                }
            },
            .motion
            {
                .name_override      = "OpenSD Motion Control Device",
                .key_list
                {
                    // No keys for motion device.  Doesn't seem useful
                    // or a good idea.
                },
                .abs_list
                {
                    // All axes ranges for motion controle need to be 
                    // defined in the profile ini.
                },
                .rel_list
                {
                    // For now only absolute axes are supported for 
                    // motion control.  Relative axes probably arent
                    // too useful, but I expect to eventually allow it.
                }
            },
            .mouse
            {
                .name_override      = "OpenSD Mouse Device",
                .key_list
                {
                    BTN_LEFT,
                    BTN_RIGHT,
                    BTN_MIDDLE,
                    BTN_SIDE,
                    BTN_EXTRA,
                    BTN_FORWARD,
                    BTN_BACK,
                    BTN_TASK,
                },
                .abs_list
                {
                    // No absolute axes for touchpad / mouse
                },
                .rel_list
                {
                    REL_X,
                    REL_Y,
                    REL_WHEEL,
                    REL_HWHEEL
                }
            }
        },
        .map
        {
            // The price of designated initialization is using designated initialization
            .dpad
            {
                .up                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .down               = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .left               = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .right              = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
            },
            .NONE
            {
                .a                  = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .b                  = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .x                  = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .y                  = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .l1                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .l2                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .l3                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .l4                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .l5                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .r1                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .r2                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .r3                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .r4                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .r5                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .menu               = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .options            = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .steam              = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .quick_access       = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 }
            },
            .trigg
            {
                .l                  = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .r                  = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
            },
            .stick
            {
                .l
                {
                    .up             = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .down           = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .left           = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .right          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .touch          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .force          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 }
                },
                .r
                {
                    .up             = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .down           = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .left           = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .right          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .touch          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .force          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 }
                },
            },
            .pad
            {
                .l
                {
                    .up             = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .down           = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .left           = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .right          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .rel_x          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .rel_y          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .touch          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .press          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .force          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 }
                },
                .r
                {
                    .up             = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .down           = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .left           = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .right          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .rel_x          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .rel_y          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .touch          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .press          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .force          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 }
                },
            },
            .accel
            {
                .x_plus             = { .dev = NONE,    .type = NONE,    .code = NONE,      .dir = 0 },
                .x_minus            = { .dev = NONE,    .type = NONE,    .code = NONE,      .dir = 0 },
                .y_plus             = { .dev = NONE,    .type = NONE,    .code = NONE,      .dir = 0 },
                .y_minus            = { .dev = NONE,    .type = NONE,    .code = NONE,      .dir = 0 },
                .z_plus             = { .dev = NONE,    .type = NONE,    .code = NONE,      .dir = 0 },
                .z_minus            = { .dev = NONE,    .type = NONE,    .code = NONE,      .dir = 0 },
            },
            .att
            {
                .roll_plus          = { .dev = NONE,    .type = NONE,    .code = NONE,      .dir = 0 },
                .roll_minus         = { .dev = NONE,    .type = NONE,    .code = NONE,      .dir = 0 },
                .pitch_plus         = { .dev = NONE,    .type = NONE,    .code = NONE,      .dir = 0 },
                .pitch_minus        = { .dev = NONE,    .type = NONE,    .code = NONE,      .dir = 0 },
                .yaw_plus           = { .dev = NONE,    .type = NONE,    .code = NONE,      .dir = 0 },
                .yaw_minus          = { .dev = NONE,    .type = NONE,    .code = NONE,      .dir = 0 },
            }
        }
    }; // end profile

} // namespace Drivers::Gamepad::Presets


#endif // __GAMEPAD__PRESET_TEMPLATE_HPP__
