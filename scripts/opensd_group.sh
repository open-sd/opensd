#!/bin/bash

curr_user=${SUDO_USER:-$USER}

printf "%s" "Adding 'opensd' group..."
groupadd -f opensd
if [ $? -eq 0 ]; then
    printf "%s\n" "OK"
else
    printf "%s\n" "ERROR"
fi

printf "%s" "Adding user ${curr_user} to 'opensd' group..."
usermod -aG opensd "${curr_user}"
if [ $? -eq 0 ]; then
    printf "%s\n" "OK"
else
    printf "%s\n" "ERROR"
fi

printf "\n%s\n" "You may need to logout for these changes to take effect."
