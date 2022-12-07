# OpenSD changelog

## [v0.44]  2022/12/06
### Added 
  - Created new online documentation at https://open-sd.gitlab.io/opensd-docs
  
### Changed
  - Moved all documentation source to separate repo (https://gitlab.com/open-sd/opensd-docs)
  - Changed the way offline documentation is built
  - Changed the way manpages are built
  - Updated offline documentation 
  - Updated the manpage
  - Updated README with new documentation links

### Removed
  - Removed asciidoc source from the main repo
  - Removed hardware notes (doc/hw.html) from this project

## [v0.43]  2022/11/19
### Added
  - Added asciidoc user manual
  - Added html user manual (from asciidoc)
  - Added asciidoc manpage
  - Added manpage for opensdd (from asciidoc)
  - Added documentation and manpage installation to CMakeLists.txt
  - Documentation is now installed to ${CMAKE_INSTALL_PREFIX}/share/doc/opensd/


## [v0.42]  2022/11/01
### Added
  - Added a CHANGELOG to the project

### Changed
  - Changed file extension for profile files from `.ini` to `.profile` in order to be less confusing.
  - Missing default / standard profiles are copied to user profile directory on startup (old and modified files are not overwritten)
  - Cleaned up profile loading code a bit.


## [v0.41]  2022/10/31
### Added
  - Added new "Profile" binding.  This allows the user to create a binding that loads a new gamepad profile.
  - Added driver messaging system
  - Added driver locking
  - Added a 250 microsecond sleep to main driver thread

### Changed
  - Updated documentation in default profile
  - Gamepad driver code cleanup


## [v0.40]  2022/10/27
### Added
  - Added touchpad button emulation.  This allows binding "press" actions for specific regions of each touchpad.
    - Added quadrant touchpad "buttons" (triangular, no overlap)
    - Added orthogonal touchpad "buttons" (dpad-like, overlapping)
    - Added 2x2 grid touchpad "buttons" (no overlap)
    - Added 3x3 grid touchpad "buttons" (no overlap)

### Changed
  - Updated default profile file with documentation on new bindings


## [v0.38]  2022/10/26
### Added
  - Added new "Command" binding.  This allows the user to create a binding that executes a command or script.
  - Added new Ini::ValVec::FullString() index parameter
  - Added multi-threaded process runner

### Changed
  - Updated documentation in default profile
  - Improved logging when loading bindings

### Removed
  - Removed old built-in profile preset


## [0.37]  2022/10/21
### Fixed
  - Fixed a new bug where the daemon was opening the wrong hidraw device.
  - Fixed a bug where the driver thread would not exit until an input report was received.

### Changed
  - The scanning method for hidraw devices has been re-written.
  - Added a timeout to input device polling.


## Older versions are not documented here.