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
#ifndef __GAMEPAD__PRESET_DEFAULT_HPP__
#define __GAMEPAD__PRESET_DEFAULT_HPP__

#include "profile.hpp"
#include <linux/input.h>


namespace Drivers::Gamepad::Presets
{
    // The OpenSD default profile for testing and general use.
    // Follows a PS5 button/axis layout from the hid_playstation driver.
    // Uses the right pad for a trackpad mouse.  Left trackpad press is
    // left click, right trackpad press is right click.
    const Drivers::Gamepad::Profile DEFAULT =
    {
        .profile_name               = "Default OpenSD Profile",
        .profile_desc               = "A basic configuration that should work for most games and provide a few extras.",
        .features
        {
            .ff                     = true,
            .motion                 = true,
            .mouse                  = true,
            .lizard                 = false,
            .filter_sticks          = true,
            .filter_pads            = true
        },
        .dz
        {
            .stick
            {
                .l                  = 0.04,
                .r                  = 0.04
            },
            .pad
            {
                .l                  = 0.0,
                .r                  = 0.0
            },
            .trigg
            {
                .l                  = 0.0,
                .r                  = 0.0
            }
        },
        .dev
        {
            .gamepad
            {
                .name_override      = "",
                .key_list
                {
                    BTN_SOUTH,          // A
                    BTN_EAST,           // B
                    BTN_WEST,           // X
                    BTN_NORTH,          // Y
                    BTN_TL,             // L1
                    BTN_TR,             // R1
                    BTN_TL2,            // L2
                    BTN_TR2,            // R2
                    BTN_THUMBL,         // L3
                    BTN_THUMBR,         // R3
                    BTN_START,          // Menu
                    BTN_SELECT,         // Options
                    BTN_MODE,           // Steam
                    KEY_MENU            // Quick Access (...)
                },
                .abs_list
                {
                    // Dpad axes
                    { .code = ABS_HAT0X, .min = -1, .max = 1 }, 
                    { .code = ABS_HAT0Y, .min = -1, .max = 1 },
                    // Left stick
                    { .code = ABS_X, .min = -32767, .max = 32767 },
                    { .code = ABS_Y, .min = -32767, .max = 32767 },
                    // Right stick
                    { .code = ABS_RX, .min = -32767, .max = 32767 },
                    { .code = ABS_RY, .min = -32767, .max = 32767 },
                    // Left trigger
                    { .code = ABS_Z, .min = 0, .max = 32767 },
                    // Right trigger
                    { .code = ABS_RZ, .min = 0, .max = 32767 }
                },
                .rel_list
                {
                    // No relative axes for gamepad device
                }
            },
            .motion
            {
                .name_override      = "",
                .key_list
                {
                    // No keys for motion device
                },
                .abs_list
                {
                    // Accelerometers
                    { .code = ABS_X, .min = -32767, .max = 32767 }, 
                    { .code = ABS_Y, .min = -32767, .max = 32767 },
                    { .code = ABS_Z, .min = -32767, .max = 32767 },
                    // Gyro
                    { .code = ABS_RX, .min = -32767, .max = 32767 },
                    { .code = ABS_RY, .min = -32767, .max = 32767 },
                    { .code = ABS_RZ, .min = -32767, .max = 32767 },
                },
                .rel_list
                {
                    // No relative axes for gamepad device
                }
            },
            .mouse
            {
                .name_override      = "",
                .key_list
                {
                    BTN_LEFT,
                    BTN_RIGHT
                },
                .abs_list
                {
                    // No absolute axes for touchpad / mouse
                },
                .rel_list
                {
                    REL_X,
                    REL_Y
                }
            }
        },
        .map
        {
            .dpad
            {
                .up                 = { .dev = GAME, .type = ABS,   .code = ABS_HAT0Y,    .dir = 0 },
                .down               = { .dev = GAME, .type = ABS,   .code = ABS_HAT0Y,    .dir = 1 },
                .left               = { .dev = GAME, .type = ABS,   .code = ABS_HAT0X,    .dir = 0 },
                .right              = { .dev = GAME, .type = ABS,   .code = ABS_HAT0X,    .dir = 1 },
            },
            .btn
            {
                .a                  = { .dev = GAME,    .type = BTN,    .code = BTN_SOUTH,  .dir = 0 },
                .b                  = { .dev = GAME,    .type = BTN,    .code = BTN_EAST,   .dir = 0 },
                .x                  = { .dev = GAME,    .type = BTN,    .code = BTN_WEST,   .dir = 0 },
                .y                  = { .dev = GAME,    .type = BTN,    .code = BTN_NORTH,  .dir = 0 },
                .l1                 = { .dev = GAME,    .type = BTN,    .code = BTN_TL,     .dir = 0 },
                .l2                 = { .dev = GAME,    .type = BTN,    .code = BTN_TL2,    .dir = 0 },
                .l3                 = { .dev = GAME,    .type = BTN,    .code = BTN_THUMBL, .dir = 0 },
                .l4                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .l5                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .r1                 = { .dev = GAME,    .type = BTN,    .code = BTN_TR,     .dir = 0 },
                .r2                 = { .dev = GAME,    .type = BTN,    .code = BTN_TR2,    .dir = 0 },
                .r3                 = { .dev = GAME,    .type = BTN,    .code = BTN_THUMBR, .dir = 0 },
                .r4                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .r5                 = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                .menu               = { .dev = GAME,    .type = BTN,    .code = BTN_START,  .dir = 0 },
                .options            = { .dev = GAME,    .type = BTN,    .code = BTN_SELECT, .dir = 0 },
                .steam              = { .dev = GAME,    .type = BTN,    .code = BTN_MODE,   .dir = 0 },
                .quick_access       = { .dev = GAME,    .type = KEY,    .code = KEY_MENU,   .dir = 0 }
            },
            .trigg
            {
                .l                  = { .dev = GAME,    .type = ABS,    .code = ABS_Z,      .dir = 1 },
                .r                  = { .dev = GAME,    .type = ABS,    .code = ABS_RZ,     .dir = 1 },
            },
            .stick
            {
                .l
                {
                    .up             = { .dev = GAME,    .type = ABS,    .code = ABS_Y,      .dir = 0 },
                    .down           = { .dev = GAME,    .type = ABS,    .code = ABS_Y,      .dir = 1 },
                    .left           = { .dev = GAME,    .type = ABS,    .code = ABS_X,      .dir = 0 },
                    .right          = { .dev = GAME,    .type = ABS,    .code = ABS_X,      .dir = 1 },
                    .touch          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .force          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 }
                },
                .r
                {
                    .up             = { .dev = GAME,    .type = ABS,    .code = ABS_RY,     .dir = 0 },
                    .down           = { .dev = GAME,    .type = ABS,    .code = ABS_RY,     .dir = 1 },
                    .left           = { .dev = GAME,    .type = ABS,    .code = ABS_RX,     .dir = 0 },
                    .right          = { .dev = GAME,    .type = ABS,    .code = ABS_RX,     .dir = 1 },
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
                    .press          = { .dev = MOUSE,   .type = BTN,    .code = BTN_LEFT,   .dir = 0 },
                    .force          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 }
                },
                .r
                {
                    .up             = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .down           = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .left           = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .right          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .rel_x          = { .dev = MOUSE,   .type = REL,    .code = REL_X,      .dir = 0 },
                    .rel_y          = { .dev = MOUSE,   .type = REL,    .code = REL_Y,      .dir = 0 },
                    .touch          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 },
                    .press          = { .dev = MOUSE,   .type = BTN,    .code = BTN_RIGHT,  .dir = 0 },
                    .force          = { .dev = NONE,    .type = NONE,   .code = NONE,       .dir = 0 }
                },
            },
            .accel
            {
                .x_plus             = { .dev = MOTION,  .type = ABS,    .code = ABS_RX,     .dir = 1 },
                .x_minus            = { .dev = MOTION,  .type = ABS,    .code = ABS_RX,     .dir = 0 },
                .y_plus             = { .dev = MOTION,  .type = ABS,    .code = ABS_RY,     .dir = 1 },
                .y_minus            = { .dev = MOTION,  .type = ABS,    .code = ABS_RY,     .dir = 0 },
                .z_plus             = { .dev = MOTION,  .type = ABS,    .code = ABS_RZ,     .dir = 1 },
                .z_minus            = { .dev = MOTION,  .type = ABS,    .code = ABS_RZ,     .dir = 0 },
            },
            .att
            {
                .roll_plus          = { .dev = MOTION,  .type = ABS,    .code = ABS_X,      .dir = 1 },
                .roll_minus         = { .dev = MOTION,  .type = ABS,    .code = ABS_X,      .dir = 0 },
                .pitch_plus         = { .dev = MOTION,  .type = ABS,    .code = ABS_Y,      .dir = 1 },
                .pitch_minus        = { .dev = MOTION,  .type = ABS,    .code = ABS_Y,      .dir = 0 },
                .yaw_plus           = { .dev = MOTION,  .type = ABS,    .code = ABS_Z,      .dir = 1 },
                .yaw_minus          = { .dev = MOTION,  .type = ABS,    .code = ABS_Z,      .dir = 0 },
            }
        }
    }; // end profile

} // namespace Drivers::Gamepad::Presets


#endif // __GAMEPAD__PRESET_DEFAULT_HPP__
