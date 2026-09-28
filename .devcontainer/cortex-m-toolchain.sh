#!/bin/bash
# -----------------------------------------------------------------------------
# cortex-m-toolchain.sh: install toolchain for CircuitPython
#
# Author: Bernhard Bablok
#
# -----------------------------------------------------------------------------

echo -e "[cortex-m-toolchain.sh] starting install"

# --- tooling   --------------------------------------------------------------

echo -e "[cortex-m-toolchain.sh] downloading and installing gcc-arm-non-eabi toolchain"
cd /workspaces

wget -qO gcc-arm-none-eabi.tar.xz \
  https://gitlab.arm.com/api/v4/projects/tooling%2Fgnu-toolchains-for-arm/packages/generic/gnu-toolchain/15.3.rel1/arm-gnu-toolchain-15.3.rel1-x86_64-arm-none-eabi.tar.xz

tar -xJf gcc-arm-none-eabi.tar.xz
ln -s arm-gnu-toolchain-15.3.rel1-x86_64-arm-none-eabi gcc-arm-none-eabi
rm -f gcc-arm-none-eabi.tar.xz

echo -e "[cortex-m-toolchain.sh] update PATH in environment"
echo -e "\nexport PATH=/workspaces/gcc-arm-none-eabi/bin:$PATH"  >> $HOME/.bashrc
