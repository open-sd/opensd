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
#include "daemon.hpp"
#include "log.hpp"
#include "errors.hpp"
#include "drivers/gamepad/presets.hpp"
// Linux
#include <signal.h>


// Global flag to stop daemon
bool gDaemonRunning = true;


void sig_handler( int sig )
{
    switch (sig)
    {
        case SIGINT:
            gDaemonRunning = false;
        break;
        
        default:
            // no other handlers
        break;
    }
}



int Daemon::Run()
{
    gLog.Write( Log::INFO, "Starting up..." );


    // Set up signal handlers
    signal( SIGINT, sig_handler );
    
    if (mGpDrv != nullptr)
    {
        gLog.Write( Log::ERROR, "Gamepad driver object already exists." );
        return Err::INIT_FAILED;
    }
    
    // Create gamepad driver object
    try 
    {
        mGpDrv = new Drivers::Gamepad::Driver;
    }
    catch (...)
    {
        gLog.Write( Log::ERROR, "Failed to create gamepad driver object." );
        return Err::CANNOT_CREATE;
    }
    
    // Load default profile
    mGpDrv->SetProfile( Drivers::Gamepad::DEFAULT_PROFILE );
    
    // Start threaded gamepad driver
    mGpDrv->Start();

    // TODO: proper loop with IPC
    
    // Loop until kepress just for testing
    while (gDaemonRunning)
    {
        // ZzZzZzzz...
        usleep( 100000 );
    }


    gLog.Write( Log::INFO, "Shutting down..." );

    // Stop gamepad driver thread
    mGpDrv->Stop();
    
    
    // Done
    return Err::OK;
}



void Daemon::Stop()
{
    gDaemonRunning  = false;
}


Daemon::Daemon()
{
    mGpDrv          = nullptr;
    gDaemonRunning  = true;
}



Daemon::~Daemon()
{
    if (mGpDrv != nullptr)
        delete mGpDrv;
        
    mGpDrv          = nullptr;
    gDaemonRunning  = false;
}