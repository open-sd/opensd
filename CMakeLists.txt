cmake_minimum_required( VERSION 3.1 )

project( opensd )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED True )
list( APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/" )

set( CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall" )
set( CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O2" )

set( CMAKE_BUILD_TYPE Release )

# Executable names
set( OPENSD_DAEMON_BIN "opensdd" )
set( OPENSD_CLI_BIN "opensd-cli" )
set( OPENSD_GUI_BIN "opensd-gui" )

# OpenSD daemon source files
file( GLOB OPENSD_DAEMON_SRC "opensdd/src/*.cpp" "opensdd/src/drivers/*.cpp" "opensdd/src/drivers/gamepad/*.cpp" "opensdd/src/drivers/backlight/*.cpp" )

# OpenSD CLI tool source files
file( GLOB OPENSD_CLI_SRC "opensd-cli/src/*.cpp" )

# OpenSD GUI tool source files
file( GLOB OPENSD_GUI_SRC "opensd-gui/src/*.cpp" )


# Package / library checks


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
