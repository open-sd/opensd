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
#include "driver.hpp"
#include "compat.hpp"
#include "../../../common/log.hpp"
// Linux
#include <unistd.h>
// C++
#include <bit>
#include <bitset>
#include <cmath>
#include <iostream>
#include <ctime>


void FilterStickCoords( double& rX, double& rY, double deadzone, double scale )
{
    double mag = sqrt( rX * rX + rY * rY );
    double ang = atan2( rY, rX );

    // Check if vector magnitude is inside deadzone
    if (mag < deadzone)
    {
        // Clip low input inside deadzone
        rX = 0;
        rY = 0;
    }
    else
    {
        // Rescale stick outside deadzone
        mag = (mag - deadzone) * scale;
        // Clip magnitude to unit vector
        mag = (mag > 1.0) ? 1.0 : mag;
        // Translate polar coordinates back to cartesian
        rX = mag * cos(ang);
        rY = mag * sin(ang);
    }
}



void FilterPadCoords( double& rX, double& rY, double deadzone, double scale )
{
    double mag = sqrt( rX * rX + rY * rY );
    double ang = atan2( rY, rX );

    // Check if vector magnitude is inside deadzone
    if (mag < deadzone)
    {
        // Clip low input inside deadzone
        rX = 0;
        rY = 0;
    }
    else
    {
        // Rescale stick outside deadzone
        mag = (mag - deadzone) * scale;
        // Translate polar coordinates back to cartesian
        rX = mag * cos(ang);
        rY = mag * sin(ang);
    }
}



int Drivers::Gamepad::Driver::OpenHid()
{
    int                 result;
    std::string         path;
    

    // Loop throught known gamepad device list and break on first one found
    for (auto&& i : KNOWN_DEVICES)
    {
        path = mHid.FindDevNode( i.vid, i.pid, i.ifacenum );
        if (!path.empty())
        {
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::OpenHid(): Found hidraw device on '" + path + "'." );
            result = mHid.Open( path );
            if (result != Err::OK)
            {
                gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::OpenHid(): Error opening hidraw device on '" + path + "'." );
                gLog.Write( Log::ERROR, "Failed to open gamepad hidraw device." );
                return Err::CANNOT_OPEN;
            }
            else
            {
                gLog.Write( Log::INFO, "Successfully opened Steam Deck gamepad device." );
                return Err::OK;
            }
        }
    }

    return Err::UNKNOWN;
}



int Drivers::Gamepad::Driver::SetHidRegister( uint8_t reg, uint16_t value )
{
    std::vector<uint8_t>    buff;
    uint8_t                 length = 3;  // Function writes fixed nuber of bytes
    int                     result;
    
    if (!mHid.IsOpen())
    {
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::SetHidRegister(): Device is not open." );
        return Err::NOT_OPEN;
    }

    using namespace v1;
    
    
    // Set the first byte of the report to the write register command
    buff.push_back( ReportId::WRITE_REGISTER );
    // Second byte is the number of bytes for registers and values
    buff.push_back( length );
    // Register is 8 bits
    buff.push_back( reg );
    // Value is 16 bits, with the low bits first
    buff.push_back( value & 0xff );
    buff.push_back( value >> 8 );

    // Fix report size at 64 bytes
    buff.resize(64);
    
    result = mHid.Write( buff );
    if (result != Err::OK)
    {
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::SetHidRegister(): Failed to write register to gamepad device. " );
        return Err::WRITE_FAILED;
    }
    
    return Err::OK;
}
   


void Drivers::Gamepad::Driver::DestroyUinputDevs()
{
    if (mpGamepad != nullptr)
    {
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::DestroyUinputDevs(): Destroying gamepad uinput object." );
        delete mpGamepad;
        mpGamepad = nullptr;
    }
    
    if (mpMotion != nullptr)
    {
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::DestroyUinputDevs(): Destroying motion uinput object." );
        delete mpMotion;
        mpMotion = nullptr;
    }
    
    if (mpMouse != nullptr)
    {
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::DestroyUinputDevs(): Destroying mouse uinput object." );
        delete mpMouse;
        mpMouse = nullptr;
    }
}



void Drivers::Gamepad::Driver::UpdateState( v1::PackedInputReport* pIr )
{
    using namespace     v1;
    DeviceState         old = mState;
    
    // Buttons
    mState.dpad.up              = pIr->up;
    mState.dpad.down            = pIr->down;
    mState.dpad.left            = pIr->left;
    mState.dpad.right           = pIr->right;
    mState.btn.a                = pIr->a;
    mState.btn.b                = pIr->b;
    mState.btn.x                = pIr->x;
    mState.btn.y                = pIr->y;
    mState.btn.l1               = pIr->l1;
    mState.btn.l2               = pIr->l2;
    mState.btn.l3               = pIr->l3;
    mState.btn.l4               = pIr->l4;
    mState.btn.l5               = pIr->l5;
    mState.btn.r1               = pIr->r1;
    mState.btn.r2               = pIr->r2;
    mState.btn.r3               = pIr->r3;
    mState.btn.r4               = pIr->r4;
    mState.btn.r5               = pIr->r5;
    mState.btn.menu             = pIr->menu;
    mState.btn.options          = pIr->options;
    mState.btn.steam            = pIr->steam;
    mState.btn.quick_access     = pIr->quick_access;
    // Triggers
    mState.trigg.l.z            = (double)pIr->l_trigg * TRIGG_AXIS_MULT;
    mState.trigg.r.z            = (double)pIr->r_trigg * TRIGG_AXIS_MULT;
    // Trigger deadzones
    if (mState.trigg.l.deadzone > 0)
        mState.trigg.l.z = (mState.trigg.l.z < mState.trigg.l.deadzone) ? 0 : (mState.trigg.l.z - mState.trigg.l.deadzone) * mState.trigg.l.scale;
    if (mState.trigg.r.deadzone > 0)
        mState.trigg.r.z = (mState.trigg.r.z < mState.trigg.r.deadzone) ? 0 : (mState.trigg.r.z - mState.trigg.r.deadzone) * mState.trigg.r.scale;
    // Sticks
    mState.stick.l.x            = (double)pIr->l_stick_x * STICK_X_AXIS_MULT;
    mState.stick.l.y            = (double)pIr->l_stick_y * STICK_Y_AXIS_MULT;
    mState.stick.l.touch        = pIr->l_stick_touch;
    mState.stick.l.force        = (((double)pIr->l_stick_force > STICK_FORCE_MAX) ? STICK_FORCE_MAX : (double)pIr->l_stick_force) * STICK_FORCE_MULT;
    mState.stick.r.x            = (double)pIr->r_stick_x * STICK_X_AXIS_MULT;
    mState.stick.r.y            = (double)pIr->r_stick_y * STICK_Y_AXIS_MULT;
    mState.stick.r.touch        = pIr->r_stick_touch;
    mState.stick.r.force        = (((double)pIr->r_stick_force > STICK_FORCE_MAX) ? STICK_FORCE_MAX : (double)pIr->r_stick_force) * STICK_FORCE_MULT;
    // Stick vectorization & deadzones
    if (mState.stick.filtered)
    {
        FilterStickCoords( mState.stick.l.x, mState.stick.l.y, mState.stick.l.deadzone, mState.stick.l.scale );
        FilterStickCoords( mState.stick.r.x, mState.stick.r.y, mState.stick.r.deadzone, mState.stick.r.scale );
    }
    // Pads
    mState.pad.l.x              = (double)pIr->l_pad_x * PAD_X_AXIS_MULT;
    mState.pad.l.y              = (double)pIr->l_pad_y * PAD_Y_AXIS_MULT;
    mState.pad.l.sx             = ((double)pIr->l_pad_x + PAD_X_MAX) * PAD_X_SENS_MULT;
    mState.pad.l.sy             = ((double)pIr->l_pad_y * -1.0 + PAD_Y_MIN) * PAD_Y_SENS_MULT;
    mState.pad.l.touch          = pIr->l_pad_touch;
    mState.pad.l.press          = pIr->l_pad_press;
    mState.pad.l.force          = (double)pIr->l_pad_force * PAD_FORCE_MULT;
    mState.pad.r.x              = (double)pIr->r_pad_x * PAD_X_AXIS_MULT;
    mState.pad.r.y              = (double)pIr->r_pad_y * PAD_Y_AXIS_MULT;
    mState.pad.r.sx             = ((double)pIr->r_pad_x + PAD_X_MAX) * PAD_X_SENS_MULT;
    mState.pad.r.sy             = ((double)pIr->r_pad_y * -1.0 + PAD_Y_MIN) * PAD_Y_SENS_MULT;
    mState.pad.r.touch          = pIr->r_pad_touch;
    mState.pad.r.press          = pIr->r_pad_press;
    mState.pad.r.force          = (double)pIr->r_pad_force * PAD_FORCE_MULT;
    // Left trackpad deltas
    if ((mState.pad.l.touch) && (old.pad.l.touch))
    {
        mState.pad.l.dx = ((mState.pad.l.sx - old.pad.l.sx) + old.pad.l.dx) / 2.0;
        mState.pad.l.dy = ((mState.pad.l.sy - old.pad.l.sy) + old.pad.l.dy) / 2.0;
    }
    else
    {
        // Delta decay / inertia
        // Rate of decay here is fixed to hardware polling interval, which
        // seems to be 250Hz.  If the polling rate changes, the decay will need
        // to reflect that.  For now, 5% feels pretty good.
        mState.pad.l.dx *= 0.95;
        mState.pad.l.dy *= 0.95;
    }
    // Right trackpad deltas
    if ((mState.pad.r.touch) && (old.pad.r.touch))
    {
        mState.pad.r.dx = ((mState.pad.r.sx - old.pad.r.sx) + old.pad.r.dx) / 2.0;
        mState.pad.r.dy = ((mState.pad.r.sy - old.pad.r.sy) + old.pad.r.dy) / 2.0;
    }
    else
    {
        // Delta decay / inertia
        mState.pad.r.dx *= .95;
        mState.pad.r.dy *= .95;
    }
    // Trackpad deadzones
    if (mState.pad.filtered)
    {
        FilterPadCoords( mState.pad.l.x, mState.pad.l.y, mState.pad.l.deadzone, mState.pad.l.scale );
        FilterPadCoords( mState.pad.r.x, mState.pad.r.y, mState.pad.r.deadzone, mState.pad.r.scale );
    }
    
    // Accelerometers
    // TODO
    
    // Gyro
    // TODO
}



void Drivers::Gamepad::Driver::TransEvent( const Binding& bind, double state, BindMode mode )
{
    Uinput::Device*     device = nullptr;
    
    
    // Select which uinput device we need to write to
    switch (bind.dev)
    {
        case NONE:
            // No binding, do nothing
            return;
        break;
        
        case GAME:
            // Abort if there is no gamepad uinput device to write to
            if (mpGamepad == nullptr)
                return;
            else
                device = mpGamepad;
        break;
        
        case MOTION:
            if (mpMotion == nullptr)
                return;
            else
                device = mpMotion;
        break;
        
        case MOUSE:
            if (mpMouse == nullptr)
                return;
            else
                device = mpMouse;
        break;
        
        default:
            // Unhandled device type
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::TransEvent(): An unhandled device type occurred." );
            return;
        break;
    }
    
    // Switch on input trigger mode
    switch (mode)
    {
        // State is a button
        case BindMode::BUTTON:
            switch (bind.type)
            {
                // Button press emits a key/button event
                case EV_KEY:
                    if (state)
                        device->UpdateKey( bind.code, true );
                break;
                
                // Button press emits an absolute axis value
                case EV_ABS:
                    // If triggered, emit an maximum absolute axis value in the direction specified by
                    // the binding
                    if (state)
                        device->UpdateAbs( bind.code, (bind.dir) ? 1.0 : -1.0 );
                break;
                
                // Button press emits a relative axis value
                case EV_REL:
                    // If triggered, emit a relative value in the direction specified in the binding
                    if (state) 
                        device->UpdateRel( bind.code, (bind.dir) ? 1 : -1 );  // TODO: Some kind of scaling / multiplier
                break;
                
                default:
                    // Unsupported input event type
                    gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::TransEvent(): An unsupported input event type occurred." );
                    return;
                break;
            }
        break;

        // State is an normalized absolute axis with a negative value
        case BindMode::AXIS_MINUS:
            switch (bind.type)
            {
                // Axis UP/LEFT emits a key/button event
                case EV_KEY:
                    if (state < 0)
                        device->UpdateKey( bind.code, true );
                break;
                
                // Axis UP/LEFT emits an absolute axis event
                case EV_ABS:
                    // If triggered, emit the state as a positive or negive absolute axis
                    // value depending on the direction specified in the binding.
                    if (state < 0)
                        device->UpdateAbs( bind.code, (bind.dir) ? fabs(state) : state );
                        
                break;

                // Axis UP/LEFT emits an absolute axis event
                case EV_REL:
                    // If triggered, emit the state as a positive or negative relative axis 
                    // value depending on the direction specified in the binding.
                    if (state < 0)
                        device->UpdateRel( bind.code, (bind.dir) ? fabs(state) : state);  // TODO Some kind of axis scaling / multiplier
                break;
                
                default:
                    // Unsupported input event type
                    gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::TransEvent(): An unsupported input event type occurred." );
                    return;
                break;
            }
        break;
        
        // State is a normalized absolute axis with a positive value
        case BindMode::PRESSURE:
        case BindMode::AXIS_PLUS:
            switch (bind.type)
            {
                // Axis DOWN/RIGHT emits a key/button event
                case EV_KEY:
                    if (state > 0)
                        device->UpdateKey( bind.code, true );
                break;
                
                // Axis DOWN/RIGHT emits an absolute axis event
                case EV_ABS:
                    // If triggered, emit the state as a positive or negive absolute axis
                    // value depending on the direction specified in the binding.
                    if (state > 0)
                        device->UpdateAbs( bind.code, (bind.dir) ? state : state * -1.0 );
                break;

                // Axis UP/LEFT emits an absolute axis event
                case EV_REL:
                    // If triggered, emit the state as a positive or negative relative axis 
                    // value depending on the direction specified in the binding.
                    if (state > 0)
                        device->UpdateRel( bind.code, (bind.dir) ? state : state * -1.0 );  // TODO Some kind of axis scaling / multiplier
                break;
                
                default:
                    // Unsupported input event type
                    gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::TransEvent(): An unsupported input event type occurred." );
                    return;
                break;
            }
        break;
        
        case BindMode::RELATIVE:
            switch (bind.type)
            {
                // TODO: handle other bind types?  Is it practical?
                
                case EV_REL:
                    device->UpdateRel( bind.code, state );
                break;
                
                default:
                    // Unsupported input event type
                    gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::TransEvent(): An unsupported input event type occurred." );
                    return;
                break;
            }
        break;
        
        default:
            // Unhandled state trigger mode
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::TransEvent(): An unhandled state trigger occurred." );
            return;
        break;
    }
}



void Drivers::Gamepad::Driver::Translate()
{
    //return;

    // Map normalized event values using the pregenerated map and write them to
    // our uinput event buffer
    TransEvent( mMap.dpad.up,           mState.dpad.up,             BindMode::BUTTON );
    TransEvent( mMap.dpad.down,         mState.dpad.down,           BindMode::BUTTON );
    TransEvent( mMap.dpad.left,         mState.dpad.left,           BindMode::BUTTON );
    TransEvent( mMap.dpad.right,        mState.dpad.right,          BindMode::BUTTON );
    TransEvent( mMap.btn.a,             mState.btn.a,               BindMode::BUTTON );
    TransEvent( mMap.btn.b,             mState.btn.b,               BindMode::BUTTON );
    TransEvent( mMap.btn.x,             mState.btn.x,               BindMode::BUTTON );
    TransEvent( mMap.btn.y,             mState.btn.y,               BindMode::BUTTON );
    TransEvent( mMap.btn.l1,            mState.btn.l1,              BindMode::BUTTON );
    TransEvent( mMap.btn.l2,            mState.btn.l2,              BindMode::BUTTON );
    TransEvent( mMap.btn.l3,            mState.btn.l3,              BindMode::BUTTON );
    TransEvent( mMap.btn.l4,            mState.btn.l4,              BindMode::BUTTON );
    TransEvent( mMap.btn.l5,            mState.btn.l5,              BindMode::BUTTON );
    TransEvent( mMap.btn.r1,            mState.btn.r1,              BindMode::BUTTON );
    TransEvent( mMap.btn.r2,            mState.btn.r2,              BindMode::BUTTON );
    TransEvent( mMap.btn.r3,            mState.btn.r3,              BindMode::BUTTON );
    TransEvent( mMap.btn.r4,            mState.btn.r4,              BindMode::BUTTON );
    TransEvent( mMap.btn.r5,            mState.btn.r5,              BindMode::BUTTON );
    TransEvent( mMap.btn.menu,          mState.btn.menu,            BindMode::BUTTON );
    TransEvent( mMap.btn.options,       mState.btn.options,         BindMode::BUTTON );
    TransEvent( mMap.btn.steam,         mState.btn.steam,           BindMode::BUTTON );
    TransEvent( mMap.btn.quick_access,  mState.btn.quick_access,    BindMode::BUTTON );
    TransEvent( mMap.trigg.l,           mState.trigg.l.z,           BindMode::PRESSURE );
    TransEvent( mMap.trigg.r,           mState.trigg.r.z,           BindMode::PRESSURE );
    TransEvent( mMap.stick.l.up,        mState.stick.l.y,           BindMode::AXIS_MINUS );
    TransEvent( mMap.stick.l.down,      mState.stick.l.y,           BindMode::AXIS_PLUS );
    TransEvent( mMap.stick.l.left,      mState.stick.l.x,           BindMode::AXIS_MINUS );
    TransEvent( mMap.stick.l.right,     mState.stick.l.x,           BindMode::AXIS_PLUS );
    TransEvent( mMap.stick.l.touch,     mState.stick.l.touch,       BindMode::BUTTON );
    TransEvent( mMap.stick.l.force,     mState.stick.l.force,       BindMode::PRESSURE );
    TransEvent( mMap.stick.r.up,        mState.stick.r.y,           BindMode::AXIS_MINUS );
    TransEvent( mMap.stick.r.down,      mState.stick.r.y,           BindMode::AXIS_PLUS );
    TransEvent( mMap.stick.r.left,      mState.stick.r.x,           BindMode::AXIS_MINUS );
    TransEvent( mMap.stick.r.right,     mState.stick.r.x,           BindMode::AXIS_PLUS );
    TransEvent( mMap.stick.r.touch,     mState.stick.r.touch,       BindMode::BUTTON );
    TransEvent( mMap.stick.r.force,     mState.stick.r.force,       BindMode::PRESSURE );
    TransEvent( mMap.pad.l.up,          mState.pad.l.y,             BindMode::AXIS_MINUS );
    TransEvent( mMap.pad.l.down,        mState.pad.l.y,             BindMode::AXIS_PLUS );
    TransEvent( mMap.pad.l.left,        mState.pad.l.x,             BindMode::AXIS_MINUS );
    TransEvent( mMap.pad.l.right,       mState.pad.l.x,             BindMode::AXIS_PLUS );
    TransEvent( mMap.pad.l.rel_x,       mState.pad.l.dx,            BindMode::RELATIVE );
    TransEvent( mMap.pad.l.rel_y,       mState.pad.l.dy,            BindMode::RELATIVE );
    TransEvent( mMap.pad.l.touch,       mState.pad.l.touch,         BindMode::BUTTON );
    TransEvent( mMap.pad.l.press,       mState.pad.l.press,         BindMode::BUTTON );
    TransEvent( mMap.pad.l.force,       mState.pad.l.force,         BindMode::PRESSURE );
    TransEvent( mMap.pad.r.up,          mState.pad.r.y,             BindMode::AXIS_MINUS );
    TransEvent( mMap.pad.r.down,        mState.pad.r.y,             BindMode::AXIS_PLUS );
    TransEvent( mMap.pad.r.left,        mState.pad.r.x,             BindMode::AXIS_MINUS );
    TransEvent( mMap.pad.r.right,       mState.pad.r.x,             BindMode::AXIS_PLUS );
    TransEvent( mMap.pad.r.rel_x,       mState.pad.r.dx,            BindMode::RELATIVE );
    TransEvent( mMap.pad.r.rel_y,       mState.pad.r.dy,            BindMode::RELATIVE );
    TransEvent( mMap.pad.r.touch,       mState.pad.r.touch,         BindMode::BUTTON );
    TransEvent( mMap.pad.r.press,       mState.pad.r.press,         BindMode::BUTTON );
    TransEvent( mMap.pad.r.force,       mState.pad.r.force,         BindMode::PRESSURE );
    TransEvent( mMap.accel.x_plus,      mState.accel.x,             BindMode::AXIS_PLUS );
    TransEvent( mMap.accel.x_minus,     mState.accel.x,             BindMode::AXIS_MINUS );
    TransEvent( mMap.accel.y_plus,      mState.accel.y,             BindMode::AXIS_PLUS );
    TransEvent( mMap.accel.y_minus,     mState.accel.y,             BindMode::AXIS_MINUS );
    TransEvent( mMap.accel.z_plus,      mState.accel.z,             BindMode::AXIS_PLUS );
    TransEvent( mMap.accel.z_minus,     mState.accel.z,             BindMode::AXIS_MINUS );
    TransEvent( mMap.att.roll_plus,     mState.att.roll,            BindMode::AXIS_PLUS );
    TransEvent( mMap.att.roll_minus,    mState.att.roll,            BindMode::AXIS_MINUS );
    TransEvent( mMap.att.pitch_plus,    mState.att.pitch,           BindMode::AXIS_PLUS );
    TransEvent( mMap.att.pitch_minus,   mState.att.pitch,           BindMode::AXIS_MINUS );
    TransEvent( mMap.att.yaw_plus,      mState.att.yaw,             BindMode::AXIS_PLUS );
    TransEvent( mMap.att.yaw_minus,     mState.att.yaw,             BindMode::AXIS_MINUS );
}



void Drivers::Gamepad::Driver::Flush()
{
    if (mpGamepad != nullptr)
        mpGamepad->Flush();
    if (mpMotion != nullptr)
        mpMotion->Flush();
    if (mpMouse != nullptr)
        mpMouse->Flush();
}



int Drivers::Gamepad::Driver::SetProfile( const Drivers::Gamepad::Profile& rProf )
{
    Uinput::DeviceConfig        cfg;


    gLog.Write( Log::INFO, "Setting gamepad profile..." );

    // Lock driver so we can make changes
    // Wait 50ms for threads to hit the mutex just to be safe
    usleep( 50000 );
    
    // Destroy any uinput objects since we need to create new ones
    DestroyUinputDevs();
    
    // Create Gamepad device
    if (!rProf.dev.gamepad.name_override.empty())
        cfg.deviceinfo.name = rProf.dev.gamepad.name_override;
    else
        cfg.deviceinfo.name     = "OpenSD Gamepad Device";
    cfg.deviceinfo.vid          = 0xDEAD;
    cfg.deviceinfo.pid          = 0xBEEF;
    cfg.deviceinfo.ver          = 0x0001;
    cfg.features.enable_keys    = true;
    cfg.features.enable_abs     = true;
    cfg.features.enable_rel     = true;
    cfg.features.enable_ff      = rProf.features.ff;
    cfg.key_list                = rProf.dev.gamepad.key_list;
    cfg.abs_list                = rProf.dev.gamepad.abs_list;
    cfg.rel_list.clear();
    try { mpGamepad = new Uinput::Device( cfg ); } catch (...)
    {
        gLog.Write( Log::ERROR, "Failed to create gamepad uinput device." );
        DestroyUinputDevs();
        return Err::CANNOT_CREATE;
    }
    
    // Create motion device
    if (rProf.features.motion)
    {
        if (!rProf.dev.motion.name_override.empty())
            cfg.deviceinfo.name     = rProf.dev.motion.name_override;
        else
            cfg.deviceinfo.name     = "OpenSD Motion Control Device";
        cfg.deviceinfo.vid          = 0xDEAD;
        cfg.deviceinfo.pid          = 0xBEEF;
        cfg.deviceinfo.ver          = 0x0001;
        cfg.features.enable_keys    = false;
        cfg.features.enable_abs     = true;
        cfg.features.enable_rel     = false;
        cfg.features.enable_ff      = false;
        cfg.key_list.clear();
        cfg.abs_list                = rProf.dev.motion.abs_list;
        cfg.rel_list.clear();
        try { mpMotion = new Uinput::Device( cfg ); } catch (...)
        {
            gLog.Write( Log::ERROR, "Failed to create motion control uinput device." );
            DestroyUinputDevs();
            return Err::CANNOT_CREATE;
        }
    }

    // Create mouse device
    if (rProf.features.mouse)
    {
        if (!rProf.dev.mouse.name_override.empty())
            cfg.deviceinfo.name     = rProf.dev.mouse.name_override;
        else
            cfg.deviceinfo.name     = "OpenSD Trackpad/Mouse Device";
        cfg.deviceinfo.vid          = 0xDEAD;
        cfg.deviceinfo.pid          = 0xBEEF;
        cfg.deviceinfo.ver          = 0x0001;
        cfg.features.enable_keys    = true;
        cfg.features.enable_abs     = false;
        cfg.features.enable_rel     = true;
        cfg.features.enable_ff      = false;
        cfg.key_list                = rProf.dev.mouse.key_list;
        cfg.abs_list.clear();
        cfg.rel_list                = rProf.dev.mouse.rel_list;
        try { mpMouse = new Uinput::Device( cfg ); } catch (...)
        {
            gLog.Write( Log::ERROR, "Failed to create trackpad/mouse uinput device." );
            DestroyUinputDevs();
            return Err::CANNOT_CREATE;
        }
    }
      
    // Set bindings
    mMap = rProf.map;
    
    // Set Deadzones
    SetStickFiltering( rProf.features.filter_sticks );
    SetPadFiltering( rProf.features.filter_pads );
    SetDeadzone( AxisEnum::L_STICK, rProf.dz.stick.l );
    SetDeadzone( AxisEnum::R_STICK, rProf.dz.stick.r );
    SetDeadzone( AxisEnum::L_PAD,   rProf.dz.pad.l );
    SetDeadzone( AxisEnum::R_PAD,   rProf.dz.pad.r );
    SetDeadzone( AxisEnum::L_TRIGG, rProf.dz.trigg.l );
    SetDeadzone( AxisEnum::R_TRIGG, rProf.dz.trigg.r );
    
    // Done
    return Err::OK;
}



int Drivers::Gamepad::Driver::Poll()
{
    // Use static to avoid construction costs since reports should usually
    // be the same size.  The underlying memory will stay allocated between
    // calls.
    static std::vector<uint8_t>     buff;
    int                             result;
    

    using namespace v1;
    
    result = mHid.Read( buff );
    if (result != Err::OK)
    {
        switch (result)
        {
            case Err::NOT_OPEN:
                gLog.Write( Log::ERROR, "Failed to read gamepad input:  Device is not open." );
                return Err::NO_DEVICE;
            break;

            case Err::READ_FAILED:
                gLog.Write( Log::ERROR, "Failed to read input from gamepad device." );
                return Err::READ_FAILED;
            break;

            default:
                gLog.Write( Log::ERROR, "An unhandled error while occurred reading gamepad device." );
                return Err::UNKNOWN;
            break;
        }
    }
    
    if (buff.size())
    {
        if (buff.at(0) == ReportId::INPUT)
        {
            if (buff.size() != 64)
            {
                gLog.Write( Log::WARN, "Invalid input report size was recieved from gamepad device." );
                return Err::WRONG_SIZE;
            }

            // Cast input report vector into packed report struct
            PackedInputReport*      ir = (PackedInputReport*)buff.data();

            // Update internal gamepad state
            UpdateState( ir );
            
            // Translate gamepad state into mapped events
            Translate();
            
            Flush();
        }
        else
            gLog.Write( Log::VERB, "An unhandled report type was received from gamepad device." );
    }
    else
        gLog.Write( Log::VERB, "Received zero-length report from gamepad device." );
    
    return Err::OK;
}



void Drivers::Gamepad::Driver::ThreadedLizardHandler()
{
    std::vector<uint8_t>    buff;
    int                     result;

    // Strangely, the only known method to disable keyboard emulation only does
    // so for a few seconds, whereas disabling the mouse is permanent until
    // re-enabled.  This means we have to run a separate thread which wakes up
    // every couple seconds and disabled the keyboard again using the 
    // CLEAR_MAPPINGS report.  If there's a better way to do this, I'd love to
    // know about it.  Looking at you, Valve.
        
    using namespace v1;
    
    // Initialize report
    buff.resize( 64, 0 );
    buff.at(0) = ReportId::CLEAR_MAPPINGS;

    // Loop thread while driver is running
    while (mRunning)
    {
        // Sleep for a bit
        usleep( LIZARD_SLEEP_SEC * 1000000 );   // in microseconds
        
        // If lizard mode is still false, send another CLEAR_MAPPINGS report
        if (!mLizardMode)
        {
            if (!mHid.IsOpen())
                gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::ThreadedLizardHandler(): Device is not open." );
            else
            {
                result = mHid.Write( buff );
                if (result != Err::OK)
                    gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::ThreadedLizardHander(): Failed to write gamepad device." );
            }
        }
    }
}



void Drivers::Gamepad::Driver::Run()
{
    // Init
    mRunning    = true;
    mLizardMode = false;
    
    // Run this function as a separate thread
    mLizHandlerThread = std::thread( &Drivers::Gamepad::Driver::ThreadedLizardHandler, this );
    
    // Loop while driver is running
    while (mRunning)
    {
        Poll();
    }
    
    // Rejoin threads after driver exits
    mLizHandlerThread.join();
}



int Drivers::Gamepad::Driver::SetLizardMode( bool enabled )
{
    int                     result;
    std::vector<uint8_t>    buff;

    
    using namespace v1;
        
    if (!mHid.IsOpen())
    {
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::SetLizardMode(): Device is not open." );
        return Err::NOT_OPEN;
    }
    
    // Wait 50ms for drivers to hit the mutex just to be safe
    usleep( 50000 );
    
    // Initialize report
    buff.resize( 64, 0 );
    
    if (!enabled)
    {
        buff.at(0) = ReportId::CLEAR_MAPPINGS;                      // Disable keyboard emulation (for a few seconds)

        result = mHid.Write( buff );
        if (result != Err::OK)
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::SetLizardMode(): Failed to disable keyboard emulation." );

        result = SetHidRegister( Register::RPAD_MODE, 0x07 );       // Disable mouse emulation on right pad
        if (result != Err::OK)
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::SetLizardMode(): Failed to disable mouse emulation." );

        result = SetHidRegister( Register::RPAD_MARGIN, 0x00 );     // Disable margins on the right pad
        if (result != Err::OK)
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::SetLizardMode(): Failed to disable trackpad margins." );

        mLizardMode = false;
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::SetLizardMode(): 'Lizard Mode' disabled." );
    }
    else
    {
        buff.at(0) = ReportId::DEFAULT_MAPPINGS;                    // Enable keyboard emulation
        result = mHid.Write( buff );
        if (result != Err::OK)
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::SetLizardMode(): Failed to enable keyboard emulation." );
        
        buff.at(0) = ReportId::DEFAULT_MOUSE;                       // Enable mouse emulation
        result = mHid.Write( buff );
        if (result != Err::OK)
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::SetLizardMode(): Failed to enable mouse emulation." );

        result = SetHidRegister( Register::RPAD_MARGIN, 0x01 );     // Enable margins on the right pad
        if (result != Err::OK)
            gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::SetLizardMode(): Failed to enable trackpad margins." );

        mLizardMode = true;
        gLog.Write( Log::DEBUG, "Drivers::Gamepad::Driver::SetLizardMode(): 'Lizard Mode' enabled." );
    }
    
    return Err::OK;
}



void Drivers::Gamepad::Driver::SetDeadzone( AxisEnum ax, double dz )
{
    dz = (dz < 0) ? 0 : dz;
    dz = (dz > 0.9) ? 0.9 : dz;
    
    switch (ax)
    {
        case AxisEnum::L_STICK:
            mState.stick.l.deadzone = dz;
            mState.stick.l.scale = (1.0 / (1.0 - dz));
        break;
        
        case AxisEnum::R_STICK:
            mState.stick.r.deadzone = dz;
            mState.stick.r.scale = (1.0 / (1.0 - dz));
        break;

        case AxisEnum::L_PAD:
            mState.pad.l.deadzone = dz;
            mState.pad.l.scale = (1.0 / (1.0 - dz));
        break;
        
        case AxisEnum::R_PAD:
            mState.pad.r.deadzone = dz;
            mState.pad.r.scale = (1.0 / (1.0 - dz));
        break;

        case AxisEnum::L_TRIGG:
            mState.trigg.l.deadzone = dz;
            mState.trigg.l.scale = (1.0 / (1.0 - dz));
        break;
        
        case AxisEnum::R_TRIGG:
            mState.trigg.r.deadzone = dz;
            mState.trigg.r.scale = (1.0 / (1.0 - dz));
        break;
    }
}



void Drivers::Gamepad::Driver::SetStickFiltering( bool enabled )
{
    mState.stick.filtered = enabled;
}



void Drivers::Gamepad::Driver::SetPadFiltering( bool enabled )
{
    mState.pad.filtered = enabled;
}



Drivers::Gamepad::Driver::Driver()
{
    int             result;
    DeviceState     initstate = {};
   
    mpGamepad           = nullptr;
    mpMotion            = nullptr;
    mpMouse             = nullptr;
    mState              = initstate;
    
    result = OpenHid();
    if (result != Err::OK)
        throw result;
        
    SetLizardMode( false );
}



Drivers::Gamepad::Driver::~Driver()
{
    SetLizardMode( true );
    
    DestroyUinputDevs();
        
    mHid.Close();
}
