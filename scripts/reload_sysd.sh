#!/bin/sh

# Get actual current user, ignoring sudo
curr_user="${SUDO_USER:-$USER}"

# If systemctl does not exist, assume we're not using systemd
if ! command -v systemctl 2>&1 > /dev/null; then
    printf "%s\n" "Cannot find systemctl."
    printf "%s\n" "This system does not appear to use systemd.  Skipping reload."
else
    printf "%s" "Reloading systemd user rules..."
    # Reloads systemd user rules.  Can be run directly as user OR with sudo.
    if systemctl --user -M "${curr_user}@" daemon-reload; then
        printf "%s\n" "OK"
    else
        printf "%s\n" "ERROR"
    fi
fi

# Done

