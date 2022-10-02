# OpenSD
An open-source Linux userspace driver for Valve's Steam Deck hardware.

[![](https://img.shields.io/badge/License-GPLv3-blueviolet?style=for-the-badge)]() [![](https://img.shields.io/badge/Written%20in-C%2B%2B-f34b7d?style=for-the-badge)]() [![](https://img.shields.io/badge/PayPal-Donate-blue?style=for-the-badge&logo=paypal)](https://paypal.me/SeeKntr0py)


<br>

## About
OpenSD is a highly-configurable userspace driver for the Steam Deck written in modern C++.  It aims to be lighweight, very fast and provide a way to fully utilize the hardware without running any closed-source, proprietary software/spyware like Steam.

If you're like me, you want to tinker with your hardware devices and build neat projects on them without needing to run any bloat, spyware, or closed-source drivers.  At the time of writing, there is no way to utilize the gamepad portion (buttons, thumbsticks, gyros, etc.) of the Steam Deck without also having to run Steam, since Steam implements an unpublished, undocumented, closed-source userspace driver to make it all work.

As good as Valve has been about Linux support and contributing to open-source initiatives, they're not so good about keeping their hardware open, nor do they respect privacy as a human right.

This goal of this project is to reverse engineer, document the hardware, and provide a driver along with the tools to configure it.  Ultimately enabling the hardware to be used freely and unencumbered by proprietary requirements.

<br>

## Features
Development is still early, so many of the features and support is incomplete.  What's listed is the current scope of the project.  See the Roadmap section for feature implmentation.

- Fast and tiny driver daemon
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
TODO:  Better instructions + install script

There's no install script yet, so it must be manually built and executed.

The OpenSD daemon `opensdd` requires access to the underlying hardware via the hidraw kernel subsystem, it then wites the events to the uinput subsystem.  This means the user running the daemon must have access to those devices.  

A udev rule is provided to make setting this up easier -- or you can configure it your own way, but it's NOT recommended to run `opensdd` as root.

once the uder rule is loaded and the user has been made a member of the relevant group(s), it should be able to run.  You will need to logout for group changes to take effect on your current user.

<br>

## Usage
TODO

Build the binaries, then run the daemon.  Again, make sure the user that runs this has access to the Steam Deck gamepad hidraw nodes AND the uinput device, or it it will fail.

If your permissions are correct, run from a terminal:
```
opensdd
```

Once the daemon finished starting up and the program is running you should be able to read joystick/gamepad the event devices from other software.

You can use tools `evtest` and `jstest-gtk` to test if it's working.

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
        - [ ]   Configuration directories
        - [x]   Multithreaded
        - [x]   Buttons
        - [x]   Sticks
        - [x]   Trackpads
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
    - [ ]   Config file loading
        - [ ]   Global config file
        - [ ]   User config files
        - [ ]   User profile files
- [ ]   Battery reporting
- [ ]   CLI config tool
- [ ]   GUI config tool
- [ ]   Install script
- [ ]   Packaging

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
