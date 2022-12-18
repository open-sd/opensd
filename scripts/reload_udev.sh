#!/bin/sh

# Make sure we're root/sudo
if [ "$(id -u)" -ne 0 ]; then 
    printf "%s\n" "This script needs to be run with sudo."
    exit 1
fi

# Reload and trigger udev rules after being installed or updated
printf "%s" "Reloading udev rules..."
if udevadm control --reload-rules; then
    printf "%s\n" "OK"
else
    printf "%s\n" "ERROR"
fi

printf "%s" "Triggering udev rules..."
if udevadm trigger; then
    printf "%s\n" "OK"
else
    printf "%s\n" "ERROR"
fi

# Done