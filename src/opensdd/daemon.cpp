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
#include "daemon.hpp"
#include "filemgr.hpp"
#include "../common/log.hpp"
#include "../common/errors.hpp"
#include "drivers/gamepad/presets.hpp"
#include "drivers/gamepad/profile_ini.hpp"
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



int Daemon::LoadProfile( std::string fileName )
{
    namespace       fs = std::filesystem;
    fs::path        path;
    int             result;
    
    // Make sure we have a gamepad object first
    if (mpGpDrv == nullptr)
    {
        gLog.Write( Log::DEBUG, FUNC_NAME, "Gamepad driver object does not exist." );
        gLog.Write( Log::ERROR, "Failed to load profile: Initialization error." );
        return Err::NOT_INITIALIZED;
    }

    // Get the full file path for the profile
    path = mFileMgr.GetProfileFilePath( fileName );
    if (path.empty())
    {
        gLog.Write( Log::ERROR, "Failed to find profile '" + fileName + "'" );
        return Err::FILE_NOT_FOUND;
    }

    Drivers::Gamepad::ProfileIni    profile_ini;
    Drivers::Gamepad::Profile       profile;
    
    // Parse profile file into profile object
    result = profile_ini.Load( path, profile );
    if (result != Err::OK)
    {
        gLog.Write( Log::ERROR, "Failed to load gamepad profile." );
        return Err::NOT_INITIALIZED;
    }
    mpGpDrv->SetProfile( profile );
    
    return Err::OK;
}



int Daemon::Startup()
{
    int             result;
    
    // Set up signal handlers
    signal( SIGINT, sig_handler );

    // Initialize file manager
    gLog.Write( Log::INFO, "Initializing file manager..." );
    result = mFileMgr.Init();
    if (result != Err::OK)
        return Err::INIT_FAILED;
    
    // Load config.ini
    gLog.Write( Log::INFO, "Loading config file..." );
    result = mConfig.Load( mFileMgr.GetConfigFilePath() );
    if (result != Err::OK)
        return Err::INIT_FAILED;
    
    // Create gamepad driver object
    gLog.Write( Log::INFO, "Creating gamepad driver object..." );
    if (mpGpDrv != nullptr)
    {
        gLog.Write( Log::ERROR, "Gamepad driver object already exists." );
        return Err::INIT_FAILED;
    }
    
    try 
    {
        mpGpDrv = new Drivers::Gamepad::Driver;
    }
    catch (...)
    {
        gLog.Write( Log::ERROR, "Failed to create gamepad driver object." );
        return Err::CANNOT_CREATE;
    }
    
    // Load gamepad driver profile
    gLog.Write( Log::INFO, "Loading gamepad profile..." );
    result = LoadProfile( mConfig.mProfileName );
    if (result != Err::OK)
        return Err::CANNOT_OPEN;
 
    // Start threaded drivers
    gLog.Write( Log::INFO, "Starting gamepad driver..." );
    mpGpDrv->Start();
   
    return Err::OK;
}



void Daemon::Shutdown()
{
    // Stop gamepad driver thread
    if (mpGpDrv != nullptr)
        mpGpDrv->Stop();
    
    if (mpGpDrv != nullptr)
        delete mpGpDrv;
        
    mpGpDrv         = nullptr;
    gDaemonRunning  = false;
}






int Daemon::Run()
{
    int         result;
    
    gLog.Write( Log::INFO, "Starting up..." );
    result = Startup();
    if (result != Err::OK)
    {
        gLog.Write( Log::ERROR, "Initialization failure.  Aborting." );
        return result;
    }

    // Loop until interrupt signal
    while (gDaemonRunning)
    {
        // ZzZzZzzz...
        usleep( 100000 );
    }

    gLog.Write( Log::INFO, "Shutting down..." );
    Shutdown();
    
    // Done
    return Err::OK;
}



void Daemon::Stop()
{
    gDaemonRunning  = false;
}



Daemon::Daemon()
{
    mpGpDrv         = nullptr;
    gDaemonRunning  = true;
}



Daemon::~Daemon()
{
    Shutdown();
}