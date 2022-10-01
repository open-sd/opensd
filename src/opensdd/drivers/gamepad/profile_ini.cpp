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
#include "profile_ini.hpp"
#include "preset_template.hpp"
#include "../../../common/errors.hpp"
#include "../../../common/log.hpp"
#include "../../../common/input_event_names.hpp"
#include "../../../common/string_funcs.hpp"
// C++
#include <fstream>


void Drivers::Gamepad::ProfileIni::AddKeyEvent( uint16_t device, uint16_t code )
{
    bool    found = false;
    
    switch (device)
    {
        case GAME:
            for (auto& i : mProf.dev.gamepad.key_list)
                if (i == code)
                    found = true;
            if (!found)
                mProf.dev.gamepad.key_list.push_back( code );
        break;
        
        case MOTION:
            gLog.Write( Log::DEBUG, "Driver::Gamepad::ProfileIni::AddKeyEvent(): Key events are not supported on Motion device." );
            return;
        break;
        
        case MOUSE:
            for (auto& i : mProf.dev.mouse.key_list)
                if (i == code)
                    found = true;
            if (!found)
                mProf.dev.mouse.key_list.push_back( code );
        break;
        
        case NONE:
        default:
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::AddKeyEvent(): Invalid device parameter." );
            return;
        break;
    }
}



void Drivers::Gamepad::ProfileIni::AddAbsEvent( uint16_t device, uint16_t code, int32_t min, int32_t max )
{
    bool    found = false;
    
    switch (device)
    {
        case GAME:
            for (auto& i : mProf.dev.gamepad.abs_list)
                if (i.code == code)
                    found = true;
                    
            if (!found)
            {
                Uinput::AbsAxisInfo     axis;
                axis.code = code;
                axis.min = min;
                axis.max = max;
                mProf.dev.gamepad.abs_list.push_back( axis );
            }
        break;
        
        case MOTION:
            for (auto& i : mProf.dev.motion.abs_list)
                if (i.code == code)
                    found = true;
                    
            if (!found)
            {
                Uinput::AbsAxisInfo     axis;
                axis.code = code;
                axis.min = min;
                axis.max = max;
                mProf.dev.motion.abs_list.push_back( axis );
            }
        break;
        
        case MOUSE:
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::AddKeyEvent(): Absolute axis events are not supported on Mouse device." );
            return;
        break;
        
        case NONE:
        default:
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::AddKeyEvent(): Invalid device parameter." );
            return;
        break;
    }
}



void Drivers::Gamepad::ProfileIni::AddRelEvent( uint16_t device, uint16_t code )
{
    bool    found = false;
    
    switch (device)
    {
        case GAME:
            for (auto& i : mProf.dev.gamepad.rel_list)
                if (i == code)
                    found = true;
            if (!found)
                mProf.dev.gamepad.rel_list.push_back( code );
        break;
        
        case MOTION:
            for (auto& i : mProf.dev.motion.rel_list)
                if (i == code)
                    found = true;
            if (!found)
                mProf.dev.motion.rel_list.push_back( code );
        break;
        
        case MOUSE:
            for (auto& i : mProf.dev.mouse.rel_list)
                if (i == code)
                    found = true;
            if (!found)
                mProf.dev.mouse.rel_list.push_back( code );
        break;
        
        case NONE:
        default:
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::AddRelEvent(): Invalid device parameter." );
            return;
        break;
    }
}



void Drivers::Gamepad::ProfileIni::GetFeatEnable( std::string key, bool& rValue )
{
    Ini::ValVec         val;
    
    // rValue is unaltered (i.e. uses default) if key is not found

    val = mIni.GetVal( "Features", key );
    if (val.Count())
    {
        if (Str::Lowercase(val.String()) == "true")
            rValue = true;
        else
            rValue = false;
    }
}



void Drivers::Gamepad::ProfileIni::GetDeadzone( std::string key, double& rValue )
{
    Ini::ValVec         val;
    
    // rValue is unaltered (i.e. uses default) if key is not found

    val = mIni.GetVal( "Deadzone", key );
    if (val.Count())
    {
        double v = val.Double();
        
        // Clamp range
        if (v > 0.9)
            v = 0.9;
        if (v < 0)
            v = 0;
            
        rValue = v;
    }
}



void Drivers::Gamepad::ProfileIni::GetAxisRange( std::string section, std::string key, int32_t& rMin, int32_t& rMax )
{
    Ini::ValVec         val;
    
    // rMin and rMax are unaltered if key is not found or invalid
    
    val = mIni.GetVal( section, key );
    if (val.Count() < 2)
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::GetAxisRange(): Format error: " + key + 
                    " expects 2 integer values.  Ignoring." );
    else
    {
        int     min = val.Int(0);
        int     max = val.Int(1);
        
        if (min == max)
        {
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::GetAxisRange(): Format error: " + key + 
                        " has an invalid range.  Ignoring." );
        }
        else
        {
            rMin = min;
            rMax = max;
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::GetAxisRange(): " + key + " is " + std::to_string(min) + " to " + std::to_string(max) );
        }
    }
}



void Drivers::Gamepad::ProfileIni::GetBinding( std::string key, Drivers::Gamepad::Binding& rBind )
{
    Ini::ValVec                 val;
    Drivers::Gamepad::Binding   temp_bind;
    std::string                 temp_str;
    std::string                 sub_str;
    int                         ev_type = 0;
    int                         result;
    
    // Find the key in the Bindings section
    val = mIni.GetVal( "Bindings", key );
    gLog.Write( Log::DEBUG, ">>> "+key+" = "+val.String(0)+" "+val.String(1)+" "+val.String(2) );
    if (!val.Count())
    {
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::GetBinding(): Error in binding " + key + 
                    ": No parameters.  Ignoring." );
        return;
    }

    // Get the the input device to bind to
    temp_str = Str::Uppercase(val.String(0));
    if (temp_str == "NONE")
        return;
    else
    {
        if (temp_str == "GAMEPAD")
            temp_bind.dev = GAME;
        else
            if (temp_str == "MOTION")
                temp_bind.dev = MOTION;
            else
                if (temp_str == "MOUSE")
                    temp_bind.dev = MOUSE;
                else
                {                        
                    gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::GetBinding(): Error in binding " + key + 
                                ": Unknown device type '" + temp_str + "'" );
                    return;
                }
    }
    
    // Need 2 params to continue
    if (val.Count() < 2)
    {
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::GetBinding(): Error in binding " + key + 
                    ": Bindings must have at least two parameters." );
        return;
    }
    
    // Get 2nd param, which should be the event code, from which we extract the event type
    temp_str = val.String(1);
    result = EvName::GetEvType( temp_str );
    if (result < 0)
    {
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::GetBinding(): Failed to get event type. " );
        return;
    }
    ev_type = result;
    
    // Handle the rest based on the event type
    switch (ev_type)
    {
        case EV_KEY: // Button / key
            temp_bind.type  = EV_KEY;

            result = EvName::GetEvCode( temp_str );
            if (result < 0)
            {
                gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::GetBinding(): Error in binding " + key +
                            ": Unrecognized KEY / BTN event code.  Ignoring." );
                return;
            }
            temp_bind.code  = (uint16_t)result;
            temp_bind.dir   = false;
            // Return the binding as a reference parameter
            rBind           = temp_bind;
            
            // Enable key event to the specified device
            AddKeyEvent( temp_bind.dev, temp_bind.code );
        break;
        
        case EV_ABS: // Absolute axis event
            // Need 3 params to continue
            if (val.Count() < 3)
            {
                gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::GetBinding(): Error in binding " + key +
                            ": Axis requires a direction indicator (+ or -) as a third parameter.  Ignoring." );
                return;
            }
            temp_bind.type  = EV_ABS;
            
            // Get the event code + any offset
            result = EvName::GetEvCode( temp_str );
            if (result < 0)
            {
                gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::GetBinding(): Error in binding " + key +
                            ": Unrecognized ABS event code.  Ignoring." );
                return;
            }
            temp_bind.code  = (uint16_t)result;

            // Get the direction of the axis even to bind
            if (val.String(2) == "+")
                temp_bind.dir = true;
            else
                if (val.String(2) == "-")
                    temp_bind.dir = false;
                else
                {
                    gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::GetBinding(): Error in binding " + key +
                                ": Unhandled device type.  Ignoring." );
                    return;
                }
            // Return the binding as a reference parameter
            rBind = temp_bind;
            
            // !! The axis is not enabled enabled here, but from the GamepadAxes / MotionAxes section
        break;
        
        case EV_REL:
            temp_bind.type  = EV_REL;

            result = EvName::GetEvCode( temp_str );
            if (result < 0)
            {
                gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::GetBinding(): Error in binding " + key +
                            ": Unrecognized REL event code.  Ignoring." );
                return;
            }
            temp_bind.code  = (uint16_t)result;
            temp_bind.dir   = false;
            // Return the binding as a reference parameter
            rBind           = temp_bind;
            
            // Enable relative axis event
            AddRelEvent( temp_bind.dev, temp_bind.code );
        break;
        
        default:
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::GetBinding(): Error in binding " + key +
                        ": Unrecognized event type.  Ignoring." );
            return;
        break;
    }
}



int Drivers::Gamepad::ProfileIni::Load( std::filesystem::path filePath, Drivers::Gamepad::Profile& rProf )
{
    Ini::ValVec         val;
    int                 result;
    
    namespace fs = std::filesystem;
    
    if (!fs::exists(filePath))
    {
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::Load(): File '" + filePath.string() + "' not found." );
        return Err::CANNOT_OPEN;
    }
    
    mIni.Clear();
    result = mIni.LoadFile( filePath );
    if (result != Err::OK)
    {
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::Load(): Failed to parse file '" + filePath.string() + "'" );
        return result;
    }
    
        
    // ----------------------------- [Profile] section -----------------------------
    // Name =
    val = mIni.GetVal( "Profile", "Name" );
    if (val.Count())
        mProf.profile_name = val.FullString();
    
    // Description = 
    val = mIni.GetVal( "Profile", "Description" );
    if (val.Count())
        mProf.profile_desc = val.FullString();
    
    // ----------------------------- [Features] section -----------------------------
    GetFeatEnable( "ForceFeedback", mProf.features.ff );
    GetFeatEnable( "MotionDevice",  mProf.features.motion );
    GetFeatEnable( "MouseDevice",   mProf.features.mouse );
    GetFeatEnable( "LizardMode",    mProf.features.lizard );

    // ----------------------------- [Deadzone] section -----------------------------
    GetDeadzone( "LStick",  mProf.dz.stick.l );
    GetDeadzone( "RStick",  mProf.dz.stick.r );
    GetDeadzone( "LPad",    mProf.dz.pad.l );
    GetDeadzone( "RPad",    mProf.dz.pad.r );
    GetDeadzone( "LTrigg",  mProf.dz.trigg.l );
    GetDeadzone( "RTrigg",  mProf.dz.trigg.r );

    // ----------------------------- [GamepadAxes] section -----------------------------
    // Iterate through list of keys and define / enable axes
    val = mIni.GetKeyList( "GamepadAxes" );
    for (auto& key : val.mData)
    {
        int32_t         min;
        int32_t         max;
        uint16_t        code;
        int             result;
        
        result = EvName::GetEvCode( key );
        if (result < 0)
        {
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::Load(): Failed to enable gamepad axis '" + key +
                        ": Event name not found." );;
        }
        code = (uint16_t)result;
        
        GetAxisRange( "GamepadAxes", key, min, max );
        AddAbsEvent( GAME, code, min, max );
    }

    // ----------------------------- [MotionAxes] section -----------------------------
    // Iterate through list of keys and define / enable axes
    val = mIni.GetKeyList( "MotionAxes" );
    for (auto& key : val.mData)
    {
        int32_t         min;
        int32_t         max;
        uint16_t        code;
        int             result;
        
        result = EvName::GetEvCode( key );
        if (result < 0)
        {
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::ProfileIni::Load(): Failed to enable gamepad axis '" + key +
                        ": Event name not found." );;
        }
        code = (uint16_t)result;
        
        GetAxisRange( "MotionAxes", key, min, max );
        AddAbsEvent( MOTION, code, min, max );
    }
    
    // ----------------------------- [Bindings] section -----------------------------
    // Dpad
    GetBinding( "DpadUp",       mProf.map.dpad.up );
    GetBinding( "DpadDown",     mProf.map.dpad.down );
    GetBinding( "DpadLeft",     mProf.map.dpad.left );
    GetBinding( "DpadRight",    mProf.map.dpad.right );
    // Buttons
    GetBinding( "A",            mProf.map.btn.a );
    GetBinding( "B",            mProf.map.btn.b );
    GetBinding( "X",            mProf.map.btn.x );
    GetBinding( "Y",            mProf.map.btn.y );
    GetBinding( "L1",           mProf.map.btn.l1 );
    GetBinding( "L2",           mProf.map.btn.l2 );
    GetBinding( "L3",           mProf.map.btn.l3 );
    GetBinding( "L4",           mProf.map.btn.l4 );
    GetBinding( "L5",           mProf.map.btn.l5 );
    GetBinding( "R1",           mProf.map.btn.r1 );
    GetBinding( "R2",           mProf.map.btn.r2 );
    GetBinding( "R3",           mProf.map.btn.r3 );
    GetBinding( "R4",           mProf.map.btn.r4 );
    GetBinding( "R5",           mProf.map.btn.r5 );
    GetBinding( "Menu",         mProf.map.btn.menu );
    GetBinding( "Options",      mProf.map.btn.options );
    GetBinding( "Steam",        mProf.map.btn.steam );
    GetBinding( "QuickAccess",  mProf.map.btn.quick_access );
    // Triggers
    GetBinding( "LTrigg",       mProf.map.trigg.l );
    GetBinding( "RTrigg",       mProf.map.trigg.r );
    // Left Stick
    GetBinding( "LStickUp",     mProf.map.stick.l.up );
    GetBinding( "LStickDown",   mProf.map.stick.l.down );
    GetBinding( "LStickLeft",   mProf.map.stick.l.left );
    GetBinding( "LStickRight",  mProf.map.stick.l.right );
    GetBinding( "LStickTouch",  mProf.map.stick.l.touch );
    GetBinding( "LStickForce",  mProf.map.stick.l.force );
    // Right Stick
    GetBinding( "RStickUp",     mProf.map.stick.r.up );
    GetBinding( "RStickDown",   mProf.map.stick.r.down );
    GetBinding( "RStickLeft",   mProf.map.stick.r.left );
    GetBinding( "RStickRight",  mProf.map.stick.r.right );
    GetBinding( "RStickTouch",  mProf.map.stick.r.touch );
    GetBinding( "RStickForce",  mProf.map.stick.r.force );
    // Left Touchpad
    GetBinding( "LPadUp",       mProf.map.pad.l.up );
    GetBinding( "LPadDown",     mProf.map.pad.l.down );
    GetBinding( "LPadLeft",     mProf.map.pad.l.left );
    GetBinding( "LPadRight",    mProf.map.pad.l.right );
    GetBinding( "LPadRelX",     mProf.map.pad.l.rel_x );
    GetBinding( "LPadRelY",     mProf.map.pad.l.rel_y );
    GetBinding( "LPadTouch",    mProf.map.pad.l.touch );
    GetBinding( "LPadPress",    mProf.map.pad.l.press );
    GetBinding( "LPadForce",    mProf.map.pad.l.force );
    // Right Touchpad
    GetBinding( "RPadUp",       mProf.map.pad.r.up );
    GetBinding( "RPadDown",     mProf.map.pad.r.down );
    GetBinding( "RPadLeft",     mProf.map.pad.r.left );
    GetBinding( "RPadRight",    mProf.map.pad.r.right );
    GetBinding( "RPadRelX",     mProf.map.pad.r.rel_x );
    GetBinding( "RPadRelY",     mProf.map.pad.r.rel_y );
    GetBinding( "RPadTouch",    mProf.map.pad.r.touch );
    GetBinding( "RPadPress",    mProf.map.pad.r.press );
    GetBinding( "RPadForce",    mProf.map.pad.r.force );
    // Acclerometers
    GetBinding( "AccelXPlus",   mProf.map.accel.x_plus );
    GetBinding( "AccelXMinus",  mProf.map.accel.x_minus );
    GetBinding( "AccelYPlus",   mProf.map.accel.y_plus );
    GetBinding( "AccelYMinus",  mProf.map.accel.y_minus );
    GetBinding( "AccelZPlus",   mProf.map.accel.z_plus );
    GetBinding( "AccelZMinus",  mProf.map.accel.z_minus );
    // Gyro / Attitude
    GetBinding( "RollPlus",     mProf.map.att.roll_plus );
    GetBinding( "RollMinus",    mProf.map.att.roll_minus );
    GetBinding( "PitchPlus",    mProf.map.att.pitch_plus );
    GetBinding( "PitchMinus",   mProf.map.att.pitch_minus );
    GetBinding( "YawPlus",      mProf.map.att.yaw_plus );
    GetBinding( "YawMinus",     mProf.map.att.yaw_minus );

    // Assign loaded profile to reference parameter
    rProf = mProf;
    
    return Err::OK;
}



Drivers::Gamepad::ProfileIni::ProfileIni()
{
    // Define default values
    mProf = Drivers::Gamepad::Presets::TEMPLATE;

    // TODO: Move these to constants somewhere more obvious
    mAxisRange.hat.min      = -1;
    mAxisRange.hat.max      = 1;
    mAxisRange.stick.min    = -32767;
    mAxisRange.stick.max    = 32767;
    mAxisRange.trigg.min    = 0;
    mAxisRange.trigg.max    = 32767;
    mAxisRange.pad.min      = -32767;
    mAxisRange.pad.max      = 32767;
    mAxisRange.accel.min    = -32767;
    mAxisRange.accel.max    = 32767;
    mAxisRange.gyro.min     = -32767;
    mAxisRange.gyro.max     = 32767;
}



Drivers::Gamepad::ProfileIni::~ProfileIni()
{
    mIni.Clear();
}
