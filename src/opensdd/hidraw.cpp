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
#include "hidraw.hpp"
#include "../common/log.hpp"
#include "string_conv.hpp"
// Linux
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/hidraw.h>


std::filesystem::path Hidraw::FindDevNode( uint16_t vid, uint16_t pid, uint16_t iFaceNum )
{
    namespace fs = std::filesystem;
    
    fs::path            sysfs = "/sys/devices";
    fs::path            device_sysfs_path;
    fs::path            hidraw_node;
    std::string         search_string;
    
    search_string = Uint16ToHex( vid ) + ":" + Uint16ToHex( pid ) + "." + Uint16ToHex( iFaceNum + 1 ) + "/hidraw";
    
    gLog.Write( Log::VERB, "Hidraw::FindHidrawNode(): Searching for device string '" + search_string + "'..." );

    // Check if sysfs exists
    if (fs::exists( sysfs ))
    {
        gLog.Write( Log::VERB, "Hidraw::FindHidrawNode(): Found sysfs path" );
        
        // scan through /sys/devices recursively looking for a matching 
        // identifier at the end of the path
        for (const auto& i : fs::recursive_directory_iterator( sysfs ))
        {
            if (i.path().string().length() > search_string.length())
                if (i.path().string().substr( i.path().string().length() - search_string.length() ) == search_string)
                {
                    gLog.Write( Log::VERB, "Hidraw::FindHidrawNode(): Found matching device at '" + i.path().string() + "'" );
                    // Found the device, make sure it's a directory
                    if (fs::is_directory(i))
                    {
                        // Get list of subdirectories, which should also be the
                        // filenames of hidraw char devices in /dev/
                        for (const auto& ii : fs::directory_iterator(i))
                            device_sysfs_path = ii.path();
                        
                        // If a directory is returned that's our device's hidraw* name
                        if (!device_sysfs_path.string().empty())
                            if (fs::is_directory(device_sysfs_path))
                            {
                                // Make sure /dev/hidraw* exists and is a char device
                                hidraw_node = "/dev/" + device_sysfs_path.filename().string();
                                if (fs::is_character_file(hidraw_node))
                                {
                                    // Got it, cool.
                                    gLog.Write( Log::VERB, "Hidraw::FindHidrawNode(): Found matching hidraw device at '" + hidraw_node.string() + "'." );
                                    return hidraw_node;
                                }
                                else
                                {
                                    // Not a character device
                                    gLog.Write( Log::DEBUG, "Hidraw::FindHidrawNode(): '" + hidraw_node.string() + "' is not a character device." );
                                    return std::filesystem::path();
                                }
                            }
                    }
                }
        }
    }
    
    // No luck.  Return empty string on failure.
    gLog.Write( Log::DEBUG, "Hidraw::FindHidrawNode(): Failed to find any hidraw device matching '" + search_string + "'." );
    return std::filesystem::path();
}



int Hidraw::Open( std::filesystem::path hidrawPath )
{
    namespace fs = std::filesystem;

    
    if (!fs::exists( hidrawPath ))
    {
        gLog.Write( Log::DEBUG, "Hidraw::Open(): hidraw path '" + hidrawPath.string() + " does not exist." );
        return Err::INVALID_PARAMETER;
    }

    if (!fs::is_character_file( hidrawPath ))
    {
        gLog.Write( Log::DEBUG, "Hidraw::Open(): hidraw path '" + hidrawPath.string() + " is not a character file." );
        return Err::INVALID_PARAMETER;
    }

    if (IsOpen())
    {
        gLog.Write( Log::DEBUG, "Hidraw::Open(): Hidraw object already has an open fd." );
        return Err::ALREADY_OPEN;
    }
    
    gLog.Write( Log::VERB, "Hidraw::Open(): Opening hidraw device on '" + hidrawPath.string() + "'." );

    // Multithreaded access guard
    std::lock_guard<std::mutex>     lock( mMutex );
    
    mFd = open( hidrawPath.c_str(), O_RDWR );
    if (mFd < 0)
    {
        int e = errno;
        gLog.Write( Log::DEBUG, "Hidraw::Open(): Failed to open device on '" + hidrawPath.string() + "' with error " + std::to_string(e) + ": " + Err::GetErrnoString(e) );
        Close();
        return Err::CANNOT_OPEN;
    }

    gLog.Write( Log::VERB, "Hidraw::Open(): Successfully opened hidraw device on '" + hidrawPath.string() + "'." );
    mPath = hidrawPath;
    
    return Err::OK;
}



bool Hidraw::IsOpen()
{
    if (mFd > 0)
        if (fcntl( mFd, F_GETFD) >= 0)
            return true;
            
    return false;
}



void Hidraw::Close()
{
    // Multithreaded access guard
    std::lock_guard<std::mutex>     lock( mMutex );

    if (IsOpen())
    {
        gLog.Write( Log::VERB, "Hidraw::Close(): Closing device '" + mPath.string() + "'." );
        close( mFd );
        mFd = -1;
    }
    mPath.clear();
}



int Hidraw::Read( std::vector<uint8_t>& rData )
{
    int             result;
    uint8_t         buff[64];
    

    // Make sure our return vector is empty
    rData.clear();

    if (!IsOpen())
    {
        gLog.Write( Log::DEBUG, "Hidraw::Read(): Device is not open. " );
        return Err::NOT_OPEN;
    }

    // Multithreaded access guard
    std::lock_guard<std::mutex>     lock( mMutex );

    result = read( mFd, buff, sizeof(buff) );
    if (result < 0)
    {
        int e = errno;
        gLog.Write( Log::DEBUG, "Hidraw::Read(): Failed to read '" + mPath.string() + "': error " + std::to_string(e) + ": " + Err::GetErrnoString(e) );
        return Err::READ_FAILED;
    }
    
    if (result != sizeof(buff))
    {
        gLog.Write( Log::DEBUG, "Hidraw::Read(): Read " + std::to_string(result) + " bytes, but expected to read " + std::to_string(sizeof(buff)) + " bytes." );
        return Err::READ_FAILED;
    }
    
    rData.assign( buff, buff + result );
    
    return Err::OK;
}



int Hidraw::Write( const std::vector<uint8_t>& rData )
{
    int                 result;

    
    if (!IsOpen())
    {
        gLog.Write( Log::DEBUG, "Hidraw::Write(): Device is not open." );
        return Err::NOT_OPEN;
    }
    
    // Multithreaded access guard
    std::lock_guard<std::mutex>     lock( mMutex );

    result = write( mFd, rData.data(), rData.size() );
    if (result < 0)
    {
        int e = errno;
        gLog.Write( Log::DEBUG, "Hidraw::Write(): Failed to write '" + mPath.string() + " with error " + std::to_string(e) + ": " + Err::GetErrnoString(e) );
        return Err::WRITE_FAILED;
    }

    //gLog.Write( Log::VERB, "Hidraw::Write(): Successfully wrote " + std::to_string(result) + " bytes to '" + mPath.string() );
    
    return Err::OK;
}



int Hidraw::GetReportDescriptor( hidraw_report_descriptor& rDesc )
{
    hidraw_report_descriptor    temp_desc = {};
    int                         result;
    int                         desc_size = 0;


    // Clear descriptor parameter
    rDesc = temp_desc;
    
    if (!IsOpen())
    {
        gLog.Write( Log::DEBUG, "Hidraw::GetReportDescriptor(): Device is not open." );
        return Err::NOT_OPEN;
    }
    
    // Multithreaded access guard
    std::lock_guard<std::mutex>     lock( mMutex );

    result = ioctl( mFd, HIDIOCGRDESCSIZE, &desc_size );
    if (result < 0)
    {
        int e = errno;
        gLog.Write( Log::DEBUG, "Hidraw::GetReportDescriptor(): Failed to get report descriptor size on '" + mPath.string() + "' with error " + std::to_string(e) + ": " + Err::GetErrnoString(e) );
        return Err::READ_FAILED;
    }
    
    temp_desc.size = desc_size;
    result = ioctl( mFd, HIDIOCGRDESC, &temp_desc);
    if (result < 0)
    {
        int e = errno;
        gLog.Write( Log::DEBUG, "Hidraw::GetReportDescriptor(): Failed to get report descriptor on '" + mPath.string() + "' with error " + std::to_string(e) + ": " + Err::GetErrnoString(e) );
        return Err::READ_FAILED;
    }
    
    // Return filled out descriptor if successfull
    rDesc = temp_desc;
    
    return Err::OK;
}



std::string Hidraw::GetName()
{
    int             result;
    char            buff[256] = {0};
    

    if (!IsOpen())
    {
        gLog.Write( Log::DEBUG, "Hidraw::GetRawName(): Device is not open." );
        return "";
    }
    
    // Multithreaded access guard
    std::lock_guard<std::mutex>     lock( mMutex );

    result = ioctl( mFd, HIDIOCGRAWNAME(sizeof(buff)), buff );
    if (result < 0)
    {
        int e = errno;
        gLog.Write( Log::DEBUG, "Hidraw::GetRawName(): Failed to read '" + mPath.string() + "' with error " + std::to_string(e) + ": " + Err::GetErrnoString(e) );
        return "";
    }
    
    return buff;
}



std::string Hidraw::GetPhysLocation()
{
    int             result;
    char            buff[256] = {0};
    

    if (!IsOpen())
    {
        gLog.Write( Log::DEBUG, "Hidraw::GetPhysicalLocation(): Device is not open." );
        return "";
    }
    
    // Multithreaded access guard
    std::lock_guard<std::mutex>     lock( mMutex );

    result = ioctl( mFd, HIDIOCGRAWPHYS(sizeof(buff)), buff );
    if (result < 0)
    {
        int e = errno;
        gLog.Write( Log::DEBUG, "Hidraw::GetPhysicalLocation(): Failed to read '" + mPath.string() + "' with error " + std::to_string(e) + ": " + Err::GetErrnoString(e) );
        return "";
    }
    
    return buff;
}



int Hidraw::GetInfo( hidraw_devinfo& rInfo )
{
    hidraw_devinfo              temp_info = {};
    int                         result;


    // Clear info parameter
    rInfo = temp_info;
    
    if (!IsOpen())
    {
        gLog.Write( Log::DEBUG, "Hidraw::GetInfo(): Device is not open." );
        return Err::NOT_OPEN;
    }
    
    // Multithreaded access guard
    std::lock_guard<std::mutex>     lock( mMutex );

    result = ioctl( mFd, HIDIOCGRAWINFO, &temp_info);
    if (result < 0)
    {
        int e = errno;
        gLog.Write( Log::DEBUG, "Hidraw::GetInfo(): Failed to get report descriptor on '" + mPath.string() + "' with error " + std::to_string(e) + ": " + Err::GetErrnoString(e) );
        return Err::READ_FAILED;
    }
    
    // Return filled out info if successfull
    rInfo = temp_info;
    
    return Err::OK;
}



int Hidraw::GetFeatureReport( std::vector<uint8_t>& rData )
{
    int             result;
    

    if (!IsOpen())
    {
        gLog.Write( Log::DEBUG, "Hidraw::GetFeatureReport(): Device is not open." );
        return Err::NOT_OPEN;
    }
    
    if (rData.empty())
    {
        gLog.Write( Log::DEBUG, "Hidraw::GetFeatureReport(): No report number specified in first byte. " );
        return Err::INVALID_PARAMETER;
    }

    // Trim off everything after the report number and resize
    rData.resize( 1 );
    rData.resize( 256, 0 );

    // Multithreaded access guard
    std::lock_guard<std::mutex>     lock( mMutex );
    
    result = ioctl( mFd, HIDIOCGFEATURE(rData.size()), rData.data() );
    if (result < 0)
    {
        int e = errno;
        gLog.Write( Log::DEBUG, "Hidraw::GetFeatureReport(): Failed to read feature report on '" + mPath.string() + " with error " + std::to_string(e) + ": " + Err::GetErrnoString(e) );
        return Err::READ_FAILED;
    }
    
    rData.resize( result );
    
    return Err::OK;
}



int Hidraw::GetFeatureReport( uint8_t reportId, std::vector<uint8_t>& rData )
{
    rData.insert( rData.begin(), reportId );
    
    return GetFeatureReport( rData );
}



int Hidraw::SetFeatureReport( const std::vector<uint8_t>& rData )
{
    int             result;


    if (!IsOpen())
    {
        gLog.Write( Log::DEBUG, "Hidraw::SetFeatureReport(): Device is not open." );
        return Err::NOT_OPEN;
    }
    
    if (rData.empty())
    {
        gLog.Write( Log::DEBUG, "Hidraw::SetFeatureReport(): Report is empty." );
        return Err::INVALID_PARAMETER;
    }
    
    // Multithreaded access guard
    std::lock_guard<std::mutex>     lock( mMutex );

    result = ioctl( mFd, HIDIOCSFEATURE(rData.size()), rData.data() ); 
    if (result < 0)
    {
        int e = errno;
        gLog.Write( Log::DEBUG, "Hidraw::SetFeatureReport(): Failed to get report descriptor on '" + mPath.string() + "' with error " + std::to_string(e) + ": " + Err::GetErrnoString(e) );
        return Err::READ_FAILED;
    }
    
    return Err::OK;
}


int Hidraw::SetFeatureReport( uint8_t reportId, const std::vector<uint8_t>& rData )
{
    std::vector<uint8_t>    vec_copy = rData;
    
    vec_copy.insert( rData.begin(), reportId );
    
    return SetFeatureReport( vec_copy );
}





Hidraw::Hidraw()
{
    mFd = -1;
    mPath.clear();
}



Hidraw::~Hidraw()
{
    Close();
    mPath.clear();
}
