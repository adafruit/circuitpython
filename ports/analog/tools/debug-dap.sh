#!/bin/bash

# Primer on taking cmd line args in Linux shell scripts
# $0 is the script itself
# $1 $2 $3 are arg1, arg2, arg3, etc

# Export OCD Path
### USER ACTION: Replace with your path to OpenOCD ###
OCD_PATH="~/MaximSDK/Tools/OpenOCD"

# Call openocd to setup the debug server, storing the PID for OpenOCD
sh -c "openocd -s $OCD_PATH/scripts -f interface/cmsis-dap.cfg -f target/$1.cfg -c \"init; reset halt\" " &

# Allow enough time for OCD server to set up
# + wait for the sleep to finish
sleep 3 &
wait $!

# spawn the gdb process and store the gdb_pid
gdb-multiarch build-apard32690/firmware.elf -x "tools/debug-dap.gdb"

# when gdb exits, kill all openocd processes
killall openocd
