#!/bin/sh

# OpenSD group for udev rules
group="opensd"
# Get actual current user, ignoring sudo
curr_user="${SUDO_USER:-$USER}"


# Make sure we're root/sudo
if [ "$(id -u)" -ne 0 ]; then 
    printf "%s\n" "This script needs to be run with sudo."
    exit 1
fi

# If opensd group does not exist, create it
if ! getent group opensd 2>&1 > /dev/null; then
    printf "%s" "Adding '${group}' group..."
    if groupadd "${group}"; then
        printf "%s\n" "OK"
    else
        printf "%s\n" "ERROR"
    fi
else 
    printf "%s\n" "${group} group already exists."
fi

# Check if user is already a member of opensd group
if ! groups "${curr_user}" | grep "${group}" 2>&1 > /dev/null; then
    printf "%s" "Adding user ${curr_user} to '${group}' group..."
    if usermod -aG opensd "${curr_user}"; then
        printf "%s\n" "OK"
    else
        printf "%s\n" "ERROR"
    fi
    
    printf "\n%s\n" "You may need to logout for these changes to take effect."
else
    printf "%s\n" "${curr_user} is already a member of the ${group} group."
fi

# Done
