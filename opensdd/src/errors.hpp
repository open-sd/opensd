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
#ifndef __ERRORS_HPP__
#define __ERRORS_HPP__

#include <string>
#include <string.h>
#include <errno.h>


namespace Err
{
    enum
    {
        OK = 0,
        UNKNOWN,
        INVALID_PARAMETER,
        OUT_OF_RANGE,
        INIT_FAILED,
        NOT_INITIALIZED,
        NOT_FOUND,
        NOT_OPEN,
        NO_DEVICE,
        ALREADY_OPEN,
        CANNOT_OPEN,
        CANNOT_CREATE,
        CANNOT_SET_PROP,
        READ_FAILED,
        WRITE_FAILED,
        WRONG_SIZE,
        NO_PERMISSION,      
    };

    std::string GetErrnoString( int e );
}


#endif // __ERRORS_HPP__
