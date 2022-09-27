####################################################################################################
##  OpenSD
##  An open-source userspace driver for Valve's Steam Deck hardware
##
##  Copyright 2022 seek
##  https://gitlab.com/open-sd/opensd
##  Licensed under the GNU GPLv3+
##
##  This program is free software: you can redistribute it and/or modify it under the terms of the 
##  GNU General Public License as published by the Free Software Foundation, either version 3 of 
##  the License, or (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
##  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
##  See the GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License along with this program. 
##  If not, see <https://www.gnu.org/licenses/>.             
####################################################################################################
cmake_minimum_required( VERSION 3.10 )

project( OpenSD )

# Compiler settings
set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED True )
set( CMAKE_CC_COMPILER gcc )
set( CMAKE_CXX_COMPILER g++ )
set( CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall" )
set( CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O2" )
set( CMAKE_BUILD_TYPE Release )

# Path for extra cmake mocules
list( APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/" )

# Executable names
set( OPENSD_DAEMON_BIN "opensdd" )
set( OPENSD_CLI_BIN "opensd-cli" )
set( OPENSD_GUI_BIN "opensd-gui" )

# OpenSD daemon source files
file( GLOB OPENSD_DAEMON_SRC 
        "src/common/errors.cpp"
        "src/common/log.cpp"
        "src/common/ini.cpp"
        "src/common/prog_args.cpp"
        "src/common/input_event_names.cpp"
        "src/common/string_funcs.cpp"
        "src/opensdd/*.cpp" 
        "src/opensdd/drivers/*.cpp" 
        "src/opensdd/drivers/gamepad/*.cpp" 
        "src/opensdd/drivers/backlight/*.cpp"
    )

# OpenSD CLI tool source files
file( GLOB OPENSD_CLI_SRC 
        "src/opensd-cli/*.cpp" 
    )

# OpenSD GUI tool source files
file( GLOB OPENSD_GUI_SRC 
        "src/opensd-gui/*.cpp" 
    )

# Build options
option( BUILD_DAEMON "Build OpenSD daemon (opensdd)" ON )
option( BUILD_CLI "Build OpenSD CLI tool (opensd-cli)" OFF )
option( BUILD_GUI "Build OpenSD GUI tool (opensd-gui)" OFF )

# Build driver daemon binary
if( BUILD_DAEMON )
    add_executable( "${OPENSD_DAEMON_BIN}" "${OPENSD_DAEMON_SRC}" )
    target_compile_options( "${OPENSD_DAEMON_BIN}" PUBLIC -Wall -Wextra )
    target_link_libraries( "${OPENSD_DAEMON_BIN}" )
endif( BUILD_DAEMON )

# Build CLI tool binary
if( BUILD_CLI )
    # TODO
endif( BUILD_CLI )

# Build GUI tool binary
if( BUILD_GUI )
    # TODO
endif( BUILD_GUI )

install( TARGETS "${OPENSD_DAEMON_BIN}" CONFIGURATIONS Release DESTINATION bin )
