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
#ifndef __GAMEPAD__DRIVER_HPP__
#define __GAMEPAD__DRIVER_HPP__

#include "../driver_base.hpp"
#include "../../hidraw.hpp"
#include "../../uinput.hpp"
#include "hid_reports.hpp"
#include "hid_state.hpp"
#include "profile.hpp"


namespace Drivers::Gamepad
{
    // Binding types for translation functions
    enum BindMode
    {
        BUTTON,
        AXIS_PLUS,
        AXIS_MINUS,
        PRESSURE,
        RELATIVE
    };
    
    // Enums for deadzone and calibration functions
    enum AxisEnum
    {
        L_STICK,
        R_STICK,
        L_PAD,
        R_PAD,
        L_TRIGG,
        R_TRIGG
    };

    // Gamepad driver class
    class Driver : public Drivers::DrvBase
    {
    private:
        Hidraw                      mHid;
        DeviceState                 mState;
        Uinput::Device*             mpGamepad;
        Uinput::Device*             mpMotion;
        Uinput::Device*             mpMouse;
        BindMap                     mMap;
        std::atomic<bool>           mLizardMode;
        std::thread                 mLizHandlerThread;
        std::mutex                  mPollMutex;
        uint64_t                    mProfSwitchDelay;       // In milliseconds
        uint64_t                    mProfSwitchTimestamp;   // In milliseconds
        
        // Hid functions
        int                         OpenHid();
        int                         SetHidRegister( uint8_t reg, uint16_t value );
        int                         ClearMapping();
        // Uinput
        int                         CreateUinputDevs();
        void                        DestroyUinputDevs();
        // Update loop functions
        void                        UpdateState( v1::PackedInputReport* pIr );
        void                        TransEvent( Binding& bind, double state, BindMode mode );
        void                        Translate();
        void                        Flush();
        
    protected:
        int                         Poll();
        void                        ThreadedLizardHandler();
        
    public:
        int                         SetProfile( const Drivers::Gamepad::Profile& rProf );
        int                         SetLizardMode( bool enabled );
        void                        SetDeadzone( AxisEnum axis, double dz );
        void                        SetStickFiltering( bool enabled );
        void                        SetPadFiltering( bool enabled );
        void                        Run();

        Driver();
        ~Driver();
    };

} // namespace Drivers::Gamepad


#endif // __GAMEPAD__DRIVER_HPP__
