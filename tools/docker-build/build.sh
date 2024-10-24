#!/bin/bash

git config --global --add safe.directory /circuitpython

git config --global --add safe.directory /circuitpython/ports/espressif/esp-idf

cd /circuitpython

make fetch-tags

. /tools/esp/esp-idf/export.sh

cd /circuitpython/ports/$1

make BOARD=${@:2}
