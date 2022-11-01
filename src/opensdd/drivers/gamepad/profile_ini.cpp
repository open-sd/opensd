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
#include "../../../common/log.hpp"
#include "../../../common/input_event_names.hpp"
#include "../../../common/string_funcs.hpp"
// C++
#include <fstream>


void Drivers::Gamepad::ProfileIni::Reset()
{
    // Default values
    mProf = Drivers::Gamepad::Presets::TEMPLATE;
    mBindIdCounter = 1;
    
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



void Drivers::Gamepad::ProfileIni::AddKeyEvent( Drivers::Gamepad::BindType bindType, uint16_t code )
{
    bool    found = false;
    
    switch (bindType)
    {
        case BindType::GAME:
            for (auto& i : mProf.dev.gamepad.key_list)
                if (i == code)
                    found = true;
            if (!found)
                mProf.dev.gamepad.key_list.push_back( code );
        break;
        
        case BindType::MOTION:
            gLog.Write( Log::DEBUG, FUNC_NAME, "Key events are not supported on Motion device." );
            return;
        break;
        
        case BindType::MOUSE:
            for (auto& i : mProf.dev.mouse.key_list)
                if (i == code)
                    found = true;
            if (!found)
                mProf.dev.mouse.key_list.push_back( code );
        break;
        
        case BindType::NONE:
        case BindType::COMMAND:
        case BindType::PROFILE:
            gLog.Write( Log::DEBUG, FUNC_NAME, "Invalid binding type." );
            return;
        break;

        default:
            gLog.Write( Log::DEBUG, FUNC_NAME, "An unhandled binding type occurred." );
        break;
    }
}



void Drivers::Gamepad::ProfileIni::AddAbsEvent( Drivers::Gamepad::BindType bindType, uint16_t code, int32_t min, int32_t max )
{
    bool    found = false;
    
    switch (bindType)
    {
        case BindType::GAME:
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
        
        case BindType::MOTION:
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
        
        case BindType::MOUSE:
            gLog.Write( Log::DEBUG, FUNC_NAME, "Absolute axis events are not supported on Mouse device." );
            return;
        break;
        
        case BindType::NONE:
        case BindType::COMMAND:
        case BindType::PROFILE:
            gLog.Write( Log::DEBUG, FUNC_NAME, "Invalid binding type." );
            return;
        break;
        
        default:
            gLog.Write( Log::DEBUG, FUNC_NAME, "An unhandled binding type occurred." );
        break;
    }
}



void Drivers::Gamepad::ProfileIni::AddRelEvent( Drivers::Gamepad::BindType bindType, uint16_t code )
{
    bool    found = false;
    
    switch (bindType)
    {
        case BindType::GAME:
            for (auto& i : mProf.dev.gamepad.rel_list)
                if (i == code)
                    found = true;
            if (!found)
                mProf.dev.gamepad.rel_list.push_back( code );
        break;
        
        case BindType::MOTION:
            for (auto& i : mProf.dev.motion.rel_list)
                if (i == code)
                    found = true;
            if (!found)
                mProf.dev.motion.rel_list.push_back( code );
        break;
        
        case BindType::MOUSE:
            for (auto& i : mProf.dev.mouse.rel_list)
                if (i == code)
                    found = true;
            if (!found)
                mProf.dev.mouse.rel_list.push_back( code );
        break;
        
        case BindType::NONE:
        case BindType::COMMAND:
        case BindType::PROFILE:
            gLog.Write( Log::DEBUG, FUNC_NAME, "Invalid binding type." );
            return;
        break;

        default:
            gLog.Write( Log::DEBUG, FUNC_NAME, "An unhandled binding type occurred." );
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
    
    val = mIni.GetVal( "Deadzones", key );

    if (!val.Count())
    {
        // rValue is unaltered (i.e. uses default) if key is not found
        gLog.Write( Log::DEBUG, FUNC_NAME, "Deadzone for '" + key + "' is missing value." );
        return;
    }
    
    double v = val.Double();
    
    // Clamp range
    if (v > 0.9)
        v = 0.9;
    if (v < 0)
        v = 0;
        
    rValue = v;
    
    gLog.Write( Log::VERB, "Setting deadzone for '" + key + "' to " + std::to_string(v) );
}



void Drivers::Gamepad::ProfileIni::GetAxisRange( std::string section, std::string key, int32_t& rMin, int32_t& rMax )
{
    Ini::ValVec         val;
    
    // rMin and rMax are unaltered if key is not found or invalid
    
    val = mIni.GetVal( section, key );
    if (val.Count() < 2)
        gLog.Write( Log::DEBUG, FUNC_NAME, "Format error: " + key + " expects 2 integer values.  Ignoring." );
    else
    {
        int     min = val.Int(0);
        int     max = val.Int(1);
        
        if (min == max)
        {
            gLog.Write( Log::DEBUG, FUNC_NAME, "Format error: " + key + " has an invalid range.  Ignoring." );
        }
        else
        {
            rMin = min;
            rMax = max;
            gLog.Write( Log::VERB, "Setting axis range for " + key + ": " + std::to_string(min) + " to " + std::to_string(max) );
        }
    }
}



void Drivers::Gamepad::ProfileIni::GetEventBinding( std::string key, Drivers::Gamepad::Binding& rBind )
{
    Drivers::Gamepad::Binding   bind;
    std::string                 dev_str;
    std::string                 ev_str;
    Ini::ValVec                 val;
    int                         ev_type = 0;
    int                         result;
    
    val = mIni.GetVal( "Bindings", key );

    // Need at least 2 params for event bindings
    if (val.Count() < 2)
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, "Error in binding " + key + ": Event bindings must have at least two parameters." );
        return;
    }

    // Get device type
    dev_str = Str::Uppercase(val.String(0));
    if (dev_str == "GAMEPAD")
    {
        bind.type = BindType::GAME;
        dev_str = "Gamepad";
    }
    else
        if (dev_str == "MOTION")
        {
            bind.type = BindType::MOTION;
            dev_str = "Motion";
        }
        else
            if (dev_str == "MOUSE")
            {
                bind.type = BindType::MOUSE;
                dev_str = "Mouse";
            }
            else
                bind.type = BindType::NONE;

    // Get 2nd param, which should be the event code, from which we extract the event type
    ev_str = Str::Uppercase( val.String(1) );
    result = EvName::GetEvType( ev_str );
    if (result < 0)
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, "Failed to get event type. " );
        return;
    }
    ev_type = result;
    
    // Handle the rest based on the event type
    switch (ev_type)
    {
        case EV_KEY: // Button / key
            bind.ev_type  = EV_KEY;

            result = EvName::GetEvCode( ev_str );
            if (result < 0)
            {
                gLog.Write( Log::DEBUG, FUNC_NAME, "Error in binding " + key + ": Unrecognized KEY / BTN event code.  Ignoring." );
                return;
            }
            bind.ev_code  = (uint16_t)result;
            bind.dir   = false;
            
            // Enable key event to the specified device
            AddKeyEvent( bind.type, bind.ev_code );

            gLog.Write( Log::VERB, "Added binding: " + key + " = " + dev_str + " " + ev_str );
        break;
        
        case EV_ABS: // Absolute axis event
            // Need 3 params to continue
            if (val.Count() < 3)
            {
                gLog.Write( Log::DEBUG, FUNC_NAME, "Error in binding " + key +
                            ": Axis requires a direction indicator (+ or -) as a third parameter.  Ignoring." );
                return;
            }
            bind.ev_type  = EV_ABS;
            
            // Get the event code + any offset
            result = EvName::GetEvCode( ev_str );
            if (result < 0)
            {
                gLog.Write( Log::DEBUG, FUNC_NAME, "Error in binding " + key + ": Unrecognized ABS event code.  Ignoring." );
                return;
            }
            bind.ev_code  = (uint16_t)result;

            // Get the direction of the axis even to bind
            if (val.String(2) == "+")
                bind.dir = true;
            else
                if (val.String(2) == "-")
                    bind.dir = false;
                else
                {
                    gLog.Write( Log::DEBUG, FUNC_NAME, "Error in binding " + key + ": Unhandled device type.  Ignoring." );
                    return;
                }
            
            // !! The axis is not enabled enabled here, but from the GamepadAxes / MotionAxes section
            gLog.Write( Log::VERB, "Added binding: " + key + " = " + dev_str + " " + ev_str + (bind.dir ? "+" : "-") );
        break;
        
        case EV_REL:
            bind.ev_type  = EV_REL;

            result = EvName::GetEvCode( ev_str );
            if (result < 0)
            {
                gLog.Write( Log::DEBUG, FUNC_NAME, "Error in binding " + key + ": Unrecognized REL event code.  Ignoring." );
                return;
            }
            bind.ev_code  = (uint16_t)result;
            bind.dir   = false;
            
            // Enable relative axis event
            AddRelEvent( bind.type, bind.ev_code );
            
            gLog.Write( Log::VERB, "Added binding: " + key + " = " + dev_str + " " + ev_str );
        break;
        
        default:
            gLog.Write( Log::DEBUG, FUNC_NAME, "Error in binding " + key + ": Unrecognized event type.  Ignoring." );
            return;
        break;
    }

    // Assign new binding to referenced parameter
    rBind = bind;
}



void Drivers::Gamepad::ProfileIni::GetCommandBinding( std::string key, Drivers::Gamepad::Binding& rBind )
{
    Drivers::Gamepad::Binding   bind;
    Ini::ValVec                 val;
    std::string                 temp_str;
    int                         result;

    val = mIni.GetVal( "Bindings", key );

    // Need at least 4 params for Command bindings
    if (val.Count() < 4)
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, "Error in binding " + key + ": Command bindings must have at least four parameters." );
        return;
    }
    
    bind.type = BindType::COMMAND;
    
    // wait_for_exit field
    if (val.Bool(1))
    {
        // Wait for exit, set unique binding ID
        if (mBindIdCounter == 0)
            ++mBindIdCounter;
        bind.id = mBindIdCounter;
    }
    else
    {
        // Do not wait for exit
        bind.id = 0;
    }
    
    // repeat_delay_ms field 
    result = val.Int(2);
    if (result < 0)
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, "Error in binding " + key + ": repeat_delay_ms must be 0 or greater." );
        result = 0;
    }
        
    bind.delay = result;
    
    // Command string
    temp_str = val.FullString(3);
    if (temp_str.empty())
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, "Error in binding" + key + ": command_to_execute string is empty." );
        return;
    }
    bind.str = temp_str;
    
    gLog.Write( Log::VERB, "Added binding: " + key + " = Command " + (bind.id ? "true" : "false") + " " + 
                std::to_string(bind.delay) + " " + bind.str );

    // Assign new binding to referenced parameter
    rBind = bind;
}



void Drivers::Gamepad::ProfileIni::GetProfileBinding( std::string key, Drivers::Gamepad::Binding& rBind )
{
    Drivers::Gamepad::Binding   bind;
    Ini::ValVec                 val;

    val = mIni.GetVal( "Bindings", key );

    // Need 2 params for Profile bindings
    if (val.Count() < 2)
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, "Error in binding " + key + ": Profile bindings must have at least two parameters." );
        return;
    }
    
    bind.type = BindType::PROFILE;
    
    bind.str = val.FullString(1);
    if (bind.str.empty())
        return;
    
    gLog.Write( Log::VERB, "Added binding: " + key + " = Profile " + bind.str );
    
    // Assign new binding to referenced parameter
    rBind = bind;
}



void Drivers::Gamepad::ProfileIni::GetBinding( std::string key, Drivers::Gamepad::Binding& rBind )
{
    Ini::ValVec                 val;
    std::string                 temp_str;
    std::string                 sub_str;
    
    // Find the key in the Bindings section
    val = mIni.GetVal( "Bindings", key );
    if (!val.Count())
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, "Error in binding " + key + ": No parameters.  Ignoring." );
        return;
    }

    // Get the the binding type
    temp_str = Str::Uppercase(val.String(0));
    if (temp_str == "NONE")
        return;
    else
    {
        if ((temp_str == "GAMEPAD") || (temp_str == "MOTION") || (temp_str == "MOUSE"))
            GetEventBinding( key, rBind );
        else
            if (temp_str == "COMMAND")
                GetCommandBinding( key, rBind );
            else
                if (temp_str == "PROFILE")
                    GetProfileBinding( key, rBind );
                else
                {                        
                    gLog.Write( Log::DEBUG, FUNC_NAME, "Error in binding " + key + ": Unknown bind type '" + temp_str + "'" );
                    return;
                }
    }
}



int Drivers::Gamepad::ProfileIni::Load( std::filesystem::path filePath, Drivers::Gamepad::Profile& rProf )
{
    Ini::ValVec         val;
    int                 result;
    
    namespace fs = std::filesystem;

    // Reset to default values
    Reset();
    
    if (!fs::exists(filePath))
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, "File '" + filePath.string() + "' not found." );
        return Err::CANNOT_OPEN;
    }
    
    mIni.Clear();
    result = mIni.LoadFile( filePath );
    if (result != Err::OK)
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, "Failed to parse file '" + filePath.string() + "'" );
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
    GetFeatEnable( "ForceFeedback",     mProf.features.ff );
    GetFeatEnable( "MotionDevice",      mProf.features.motion );
    GetFeatEnable( "MouseDevice",       mProf.features.mouse );
    GetFeatEnable( "LizardMode",        mProf.features.lizard );
    GetFeatEnable( "StickFiltering",    mProf.features.filter_sticks );
    GetFeatEnable( "TrackpadFiltering", mProf.features.filter_pads );

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
            gLog.Write( Log::DEBUG, FUNC_NAME, "Failed to enable gamepad axis '" + key + ": Event name not found." );
        }
        code = (uint16_t)result;
        
        GetAxisRange( "GamepadAxes", key, min, max );
        AddAbsEvent( BindType::GAME, code, min, max );
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
            gLog.Write( Log::DEBUG, FUNC_NAME, "Failed to enable gamepad axis '" + key + ": Event name not found." );
        }
        code = (uint16_t)result;
        
        GetAxisRange( "MotionAxes", key, min, max );
        AddAbsEvent( BindType::MOTION, code, min, max );
    }
    
    // ----------------------------- [Bindings] section -----------------------------
    // Dpad
    GetBinding( "DpadUp",               mProf.map.dpad.up );
    GetBinding( "DpadDown",             mProf.map.dpad.down );
    GetBinding( "DpadLeft",             mProf.map.dpad.left );
    GetBinding( "DpadRight",            mProf.map.dpad.right );
    // Buttons
    GetBinding( "A",                    mProf.map.btn.a );
    GetBinding( "B",                    mProf.map.btn.b );
    GetBinding( "X",                    mProf.map.btn.x );
    GetBinding( "Y",                    mProf.map.btn.y );
    GetBinding( "L1",                   mProf.map.btn.l1 );
    GetBinding( "L2",                   mProf.map.btn.l2 );
    GetBinding( "L3",                   mProf.map.btn.l3 );
    GetBinding( "L4",                   mProf.map.btn.l4 );
    GetBinding( "L5",                   mProf.map.btn.l5 );
    GetBinding( "R1",                   mProf.map.btn.r1 );
    GetBinding( "R2",                   mProf.map.btn.r2 );
    GetBinding( "R3",                   mProf.map.btn.r3 );
    GetBinding( "R4",                   mProf.map.btn.r4 );
    GetBinding( "R5",                   mProf.map.btn.r5 );
    GetBinding( "Menu",                 mProf.map.btn.menu );
    GetBinding( "Options",              mProf.map.btn.options );
    GetBinding( "Steam",                mProf.map.btn.steam );
    GetBinding( "QuickAccess",          mProf.map.btn.quick_access );
    // Triggers
    GetBinding( "LTrigg",               mProf.map.trigg.l );
    GetBinding( "RTrigg",               mProf.map.trigg.r );
    // Left Stick
    GetBinding( "LStickUp",             mProf.map.stick.l.up );
    GetBinding( "LStickDown",           mProf.map.stick.l.down );
    GetBinding( "LStickLeft",           mProf.map.stick.l.left );
    GetBinding( "LStickRight",          mProf.map.stick.l.right );
    GetBinding( "LStickTouch",          mProf.map.stick.l.touch );
    GetBinding( "LStickForce",          mProf.map.stick.l.force );
    // Right Stick
    GetBinding( "RStickUp",             mProf.map.stick.r.up );
    GetBinding( "RStickDown",           mProf.map.stick.r.down );
    GetBinding( "RStickLeft",           mProf.map.stick.r.left );
    GetBinding( "RStickRight",          mProf.map.stick.r.right );
    GetBinding( "RStickTouch",          mProf.map.stick.r.touch );
    GetBinding( "RStickForce",          mProf.map.stick.r.force );
    // Left Touchpad
    GetBinding( "LPadUp",               mProf.map.pad.l.up );
    GetBinding( "LPadDown",             mProf.map.pad.l.down );
    GetBinding( "LPadLeft",             mProf.map.pad.l.left );
    GetBinding( "LPadRight",            mProf.map.pad.l.right );
    GetBinding( "LPadRelX",             mProf.map.pad.l.rel_x );
    GetBinding( "LPadRelY",             mProf.map.pad.l.rel_y );
    GetBinding( "LPadTouch",            mProf.map.pad.l.touch );
    GetBinding( "LPadPress",            mProf.map.pad.l.press );
    GetBinding( "LPadForce",            mProf.map.pad.l.force );
    GetBinding( "LPadPressQuadUp",      mProf.map.pad.l.btn_quad_up );
    GetBinding( "LPadPressQuadDown",    mProf.map.pad.l.btn_quad_down );
    GetBinding( "LPadPressQuadLeft",    mProf.map.pad.l.btn_quad_left );
    GetBinding( "LPadPressQuadRight",   mProf.map.pad.l.btn_quad_right );
    GetBinding( "LPadPressOrthUp",      mProf.map.pad.l.btn_orth_up );
    GetBinding( "LPadPressOrthDown",    mProf.map.pad.l.btn_orth_down );
    GetBinding( "LPadPressOrthLeft",    mProf.map.pad.l.btn_orth_left );
    GetBinding( "LPadPressOrthRight",   mProf.map.pad.l.btn_orth_right );
    GetBinding( "LPadPressGrid2x2_1",   mProf.map.pad.l.btn_2x2_1 );
    GetBinding( "LPadPressGrid2x2_2",   mProf.map.pad.l.btn_2x2_2 );
    GetBinding( "LPadPressGrid2x2_3",   mProf.map.pad.l.btn_2x2_3 );
    GetBinding( "LPadPressGrid2x2_4",   mProf.map.pad.l.btn_2x2_4 );
    GetBinding( "LPadPressGrid3x3_1",   mProf.map.pad.l.btn_3x3_1 );
    GetBinding( "LPadPressGrid3x3_2",   mProf.map.pad.l.btn_3x3_2 );
    GetBinding( "LPadPressGrid3x3_3",   mProf.map.pad.l.btn_3x3_3 );
    GetBinding( "LPadPressGrid3x3_4",   mProf.map.pad.l.btn_3x3_4 );
    GetBinding( "LPadPressGrid3x3_5",   mProf.map.pad.l.btn_3x3_5 );
    GetBinding( "LPadPressGrid3x3_6",   mProf.map.pad.l.btn_3x3_6 );
    GetBinding( "LPadPressGrid3x3_7",   mProf.map.pad.l.btn_3x3_7 );
    GetBinding( "LPadPressGrid3x3_8",   mProf.map.pad.l.btn_3x3_8 );
    GetBinding( "LPadPressGrid3x3_9",   mProf.map.pad.l.btn_3x3_9 );
    // Right Touchpad
    GetBinding( "RPadUp",               mProf.map.pad.r.up );
    GetBinding( "RPadDown",             mProf.map.pad.r.down );
    GetBinding( "RPadLeft",             mProf.map.pad.r.left );
    GetBinding( "RPadRight",            mProf.map.pad.r.right );
    GetBinding( "RPadRelX",             mProf.map.pad.r.rel_x );
    GetBinding( "RPadRelY",             mProf.map.pad.r.rel_y );
    GetBinding( "RPadTouch",            mProf.map.pad.r.touch );
    GetBinding( "RPadPress",            mProf.map.pad.r.press );
    GetBinding( "RPadForce",            mProf.map.pad.r.force );
    GetBinding( "RPadPressQuadUp",      mProf.map.pad.l.btn_quad_up );
    GetBinding( "RPadPressQuadDown",    mProf.map.pad.l.btn_quad_down );
    GetBinding( "RPadPressQuadLeft",    mProf.map.pad.l.btn_quad_left );
    GetBinding( "RPadPressQuadRight",   mProf.map.pad.l.btn_quad_right );
    GetBinding( "RPadPressOrthUp",      mProf.map.pad.l.btn_orth_up );
    GetBinding( "RPadPressOrthDown",    mProf.map.pad.l.btn_orth_down );
    GetBinding( "RPadPressOrthLeft",    mProf.map.pad.l.btn_orth_left );
    GetBinding( "RPadPressOrthRight",   mProf.map.pad.l.btn_orth_right );
    GetBinding( "RPadPressGrid2x2_1",   mProf.map.pad.l.btn_2x2_1 );
    GetBinding( "RPadPressGrid2x2_2",   mProf.map.pad.l.btn_2x2_2 );
    GetBinding( "RPadPressGrid2x2_3",   mProf.map.pad.l.btn_2x2_3 );
    GetBinding( "RPadPressGrid2x2_4",   mProf.map.pad.l.btn_2x2_4 );
    GetBinding( "RPadPressGrid3x3_1",   mProf.map.pad.l.btn_3x3_1 );
    GetBinding( "RPadPressGrid3x3_2",   mProf.map.pad.l.btn_3x3_2 );
    GetBinding( "RPadPressGrid3x3_3",   mProf.map.pad.l.btn_3x3_3 );
    GetBinding( "RPadPressGrid3x3_4",   mProf.map.pad.l.btn_3x3_4 );
    GetBinding( "RPadPressGrid3x3_5",   mProf.map.pad.l.btn_3x3_5 );
    GetBinding( "RPadPressGrid3x3_6",   mProf.map.pad.l.btn_3x3_6 );
    GetBinding( "RPadPressGrid3x3_7",   mProf.map.pad.l.btn_3x3_7 );
    GetBinding( "RPadPressGrid3x3_8",   mProf.map.pad.l.btn_3x3_8 );
    GetBinding( "RPadPressGrid3x3_9",   mProf.map.pad.l.btn_3x3_9 );
    // Acclerometers
    GetBinding( "AccelXPlus",           mProf.map.accel.x_plus );
    GetBinding( "AccelXMinus",          mProf.map.accel.x_minus );
    GetBinding( "AccelYPlus",           mProf.map.accel.y_plus );
    GetBinding( "AccelYMinus",          mProf.map.accel.y_minus );
    GetBinding( "AccelZPlus",           mProf.map.accel.z_plus );
    GetBinding( "AccelZMinus",          mProf.map.accel.z_minus );
    // Gyro / Attitude
    GetBinding( "RollPlus",             mProf.map.att.roll_plus );
    GetBinding( "RollMinus",            mProf.map.att.roll_minus );
    GetBinding( "PitchPlus",            mProf.map.att.pitch_plus );
    GetBinding( "PitchMinus",           mProf.map.att.pitch_minus );
    GetBinding( "YawPlus",              mProf.map.att.yaw_plus );
    GetBinding( "YawMinus",             mProf.map.att.yaw_minus );

    // Assign loaded profile to reference parameter
    rProf = mProf;
    
    return Err::OK;
}



Drivers::Gamepad::ProfileIni::ProfileIni()
{
    Reset();
}



Drivers::Gamepad::ProfileIni::~ProfileIni()
{
    mIni.Clear();
}
