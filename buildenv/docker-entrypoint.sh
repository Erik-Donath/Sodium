#!/usr/bin/env bash

# Convert line endings to Unix format (for all shell scripts in /root/env/script)
echo "--> Converting line endings to Unix format"
find /root/env/script -type f -name "*.sh" -exec dos2unix {} +

# Ensure all scripts are executable
echo "--> Ensuring scripts are executable"
chmod +x /root/env/script/*.sh

# Run make with the arguments provided
echo "--> Running make"
exec make clean info all $@
