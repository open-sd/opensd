#!/bin/bash

printf "%s" "Reloading udev rules..."
udevadm control --reload-rules
if [ $? -eq 0 ]; then
    printf "%s\n" "OK"
else
    printf "%s\n" "ERROR"
fi

printf "%s" "Triggering udev rules..."
udevadm trigger
if [ $? -eq 0 ]; then
    printf "%s\n" "OK"
else
    printf "%s\n" "ERROR"
fi
