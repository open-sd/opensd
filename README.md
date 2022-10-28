# OpenSD
An open-source Linux userspace driver for Valve's Steam Deck hardware.

[![](https://gitlab.com/open-sd/opensd/-/wikis/uploads/64793cc72ad0159b16a770067a431a2a/License-GPLv3-blueviolet.png)](https://choosealicense.com/licenses/gpl-3.0/) [![](https://gitlab.com/open-sd/opensd/-/wikis/uploads/f508e5f9a3cbe321873df25a251c3df5/Written_in-C++-f34b7d.png)]() [![](https://gitlab.com/open-sd/opensd/-/wikis/uploads/541826803dd5f8272489f44cce10fd5b/PayPal-Donate-blue.png)](https://paypal.me/seekdev)


<br>

## About
OpenSD is a highly-configurable userspace driver for the Steam Deck written in modern C++.  It aims to be lighweight, very fast and provide a way to fully utilize the hardware without running any closed-source, proprietary software/spyware like Steam.

If you're like me, you want to tinker with your hardware devices and build neat projects on them without needing to run any bloat, spyware, or closed-source drivers.  At the time of writing, there is no way to utilize the gamepad portion (buttons, thumbsticks, gyros, etc.) of the Steam Deck without also having to run Steam, since Steam implements an unpublished, undocumented, closed-source userspace driver to make it all work.

As good as Valve has been about Linux support and contributing to open-source initiatives, they're not so good about keeping their hardware open, nor do they respect privacy as a human right.

This goal of this project is to reverse engineer, document the hardware, and provide a driver along with the tools to configure it.  Ultimately enabling the hardware to be used freely and unencumbered by proprietary requirements.

<br>

## Features
Development is still early, so many of the features and support is incomplete.  What's listed is the current scope of the project.  See the Roadmap section for feature implmentation.

- Small and fast driver daemon
- Fully configurable and mappable
- Default device profiles for common controllers
- Customizable device profiles
- Configurable by ini file
- CLI tool to configure live driver
- GUI tool to configure live driver
- Support for gamepad input (buttons, axes, etc.)
- Support for motion controls (accelerometers, gyros)
- Support for Force-Feedback / haptic vibration.
- Support for non-gamepad input (volume)
- Manual backlight control
- Automatic backlight control (ambient light sensor)
- Volume control
- Configurable mouse emulation

<br>

## Current state
The OpenSD main branch is currently in a usable state for very basic driver functionality.  It should be enough to play many games and emulators if you know your way around a terminal.

<br>

## Requirements
Hardware support for the Steam Deck is pretty recent, so using the most recent kernel is recommended.  Presently the target / development system is Arch Linux with kernel 5.19.  Older distros/kernels may not support the necessary features.

The code has very few dependencies other than the kernel.  It does, however use C++17 and a couple C++20 features, so a fairly recent version of G++ is needed.  I'm currently building with G++ 12.2.

- Kernel 5.18+  (older kernels will work, but other hardware support may be missing, i.e. audio and wifi)
- GCC 8.0+ (for c++20 designated initializers)
- cmake 3.10+

<br>

## Building
OpenSD uses cmake so building is nice and simple.
```
mkdir build
cd build
cmake ..
make
```

To configure build/install options, run this from the build directory
```
ccmake ..
```

<br>

## Installation
Once you've successfully built OpenSD, just run
```
sudo make install
```

By default, this will
 - Install a systemd user service file.
 - Install a udev rule to give hardware access to the 'opensd' group
 - Reload udev rules
 - Create the 'opensd' group and make the current user a member of that group.
 
Note:  Group changes will not take effect until after logout.

<br>

## Usage
Make sure that whichever user is running OpenSD is a member of the 'opensd' group (or otherwise has access to the Steam Deck hidraw devices).

If you've installed OpenSD, you can run the daemon from a terminal with
```
opensdd
```
or using the systemd user service with
```
systemctl --user start opensd
```

This means OpenSD can also be enabled on login by using
```
systemctl --user enable opensd
```

CLI options can be shown using
```
opensdd --help
```

Once the daemon is running, you should be able to read joystick/gamepad the event devices from other software.
You can use tools like `evtest` and `jstest-gtk` to test it.

## Configuration
OpenSD is highly configurable and easy to use.  
Currently, OpenSD can be configured using ini files, and eventually, there will be a GUI configuration tool as well.

Configuration is broken into two parts, the daemon configuration and the gamepad profile.  By default (XDG standards are correctly followed), you can find them in `~/.config/opensd/`.
The `config.ini` file contains settings for the daemon (not much implemented so far), including which profile file to load (from the user profile directory)

The ini files in the `~/.config/opensd/profiles/` directory are used to configure the gamepad features and mappings.  If you want to create a new profile, it's recommended to copy the `default.ini` to `yourprofilename.ini` instead of editing it directly.  
The `default.ini` profile is documented to explain how to configure your device.  Don't forget to set it to be loaded in the 'config.ini'.  There's no limit to the number of device profiles you can make.


<br>

## Roadmap
Feature progress:

- [x]   Driver daemon
    - [x]   Gamepad driver
        - [x]   Hidraw interface
        - [x]   Gamepad uinput interface
        - [x]   Motion uinput interface
        - [x]   Mouse uinput interface
        - [x]   Completely mappable
        - [x]   Universal binding system
        - [x]   Profile support
        - [x]   Profile ini loading
        - [x]   Configuration directories
        - [x]   Multithreaded
        - [x]   Buttons
        - [x]   Sticks
        - [x]   Trackpads
        - [x]   Trackpad button emulation
        - [x]   Run commands / scripts with bindings
        - [ ]   Accelerometers
        - [ ]   Gyros
        - [x]   "Lizard Mode" control
        - [x]   Mouse emuation
        - [ ]   Force-feedback
        - [x]   Deadzones
    - [ ]   Backlight driver
        - [ ]   Backend interface
        - [ ]   Manual control
        - [ ]   Automatic control
    - [ ]   Volume control
    - [ ]   IPC
    - [x]   Config file loading
        - [ ]   Global config file
        - [x]   User config files
        - [x]   User profile files
- [ ]   Battery reporting
- [ ]   CLI config tool
- [ ]   GUI config tool
- [x]   Install script
- [ ]   Packaging
- [ ]   Documentation
- [ ]   Man files
- [ ]   Wiki


<br>

## Contributing
There are a lot of open questions about how the HID reports work.  Most of the the work has come from reverse engineering input reports and function documenetation from the kernel Steam Controller driver.  Anyone who can fill in blanks in the code would be appreciated, just open an issue or submit a PR.

As always, feel free to buy me a coffee if you appreciate this software ;)

<br>

## License
OpenSD is licensed under THE GNU GPLv3+.  See LICENSE for details.

<br>

## Disclaimer
I have no affiliation with Valve or any of their properties.

Valve, Steam, Steam Deck, Steam Controller or other Valve trademarks, are the property of Valve LLC.  Any reference to these, or other tradmarks, are in fair use.

All hardware documentation and implementation is derived from legal reverse-engineering and referencing other GPL-licensed open-source code published in the Linux kernel.  No code or documentation in this project has been obtained from any method that could be considered a trade secret.
