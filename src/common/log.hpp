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
#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <string>
#include <mutex>


// Simple thread-safe global logger
class Log
{
private:
    int             mFilter;
    int             mMethod;
    std::mutex      mMutex;

public:
    enum            Level { VERB, DEBUG, INFO, WARN, ERROR };
    enum            Method { NONE, STDOUT, STDERR, SYSLOG };    

    void            SetFilterLevel( Log::Level logLevel );
    void            SetOutputMethod( Log::Method  method );
    void            Write( Log::Level logLevel, std::string msg );
    void            Write( Log::Level logLevel, std::wstring msg );

    Log();
    ~Log();
};

// Global instance
extern Log     gLog;

#endif // __LOG_HPP__
