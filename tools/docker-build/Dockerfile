FROM ubuntu:24.04 as build

ARG ARM_TOOLCHAIN_URL=https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/

ARG ARM_TOOLCHAIN_FILE=arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.gz

ARG ARM_TOOLCHAIN_DIR=arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi

ARG TOOLS_DIR=/tools

ARG DOWNLOAD_DIR=${TOOLS_DIR}/bin

RUN apt-get update && apt-get install -y build-essential software-properties-common git git-lfs gettext cmake mtools wget curl which

RUN mkdir -p ${DOWNLOAD_DIR} && cd ${DOWNLOAD_DIR} && curl -LO ${ARM_TOOLCHAIN_URL}/${ARM_TOOLCHAIN_FILE}

RUN cd ${DOWNLOAD_DIR} && pwd && tar -xf ${ARM_TOOLCHAIN_FILE} && rm ${ARM_TOOLCHAIN_FILE}

ENV PATH="${PATH}:${DOWNLOAD_DIR}/${ARM_TOOLCHAIN_DIR}/bin"

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs -y | sh

RUN apt-get install -y git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build

RUN apt-get install -y ccache libffi-dev libssl-dev dfu-util libusb-1.0-0 python-is-python3

ENV IDF_TOOLS_PATH=${TOOLS_DIR}/.espressif

ENV IDF_PATH=${TOOLS_DIR}/esp/esp-idf

RUN mkdir -p ${TOOLS_DIR}/esp && cd ${TOOLS_DIR}/esp && git clone -b v5.2.2 --recursive https://github.com/espressif/esp-idf.git

RUN cd ${TOOLS_DIR}/esp/esp-idf && ./install.sh all

RUN curl -LO https://raw.githubusercontent.com/adafruit/circuitpython/main/requirements-dev.txt

RUN curl -LO https://raw.githubusercontent.com/adafruit/circuitpython/main/requirements-doc.txt

RUN . ${TOOLS_DIR}/esp/esp-idf/export.sh && pip3 install --upgrade -r requirements-dev.txt && \
           pip3 install --upgrade -r requirements-doc.txt

COPY build.sh ${TOOLS_DIR}

RUN chmod -R a=u ${TOOLS_DIR}

ENTRYPOINT ["/bin/bash"]
