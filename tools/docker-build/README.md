# Building CircuitPython with Docker

On a Linux machine with `docker` installed, an executable for any supported CircuitPython board can be built without setup by invoking the command `tools/docker-build/build <port> <board-name> <option>` from the CircuitPython root directory.  For example, if the CircuitPython repo is located at `~/circuitpython` the commands

```bash
    cd ~/circuitpython
    tools/docker-build/build espressif adafruit_feather_esp32s2_reverse_tft
    tools/docker-build/build raspberrypi waveshare_rp2040_zero
    tools/docker-build/build atmel-samd grandcentral_m4_express DEBUG=1 TRANSLATION=es
```

will build executables for a Feather ESP32S2 board, a Waveshare RP2040 board and an Adafruit Grand Central M4 Express board, and the Grand Central build will include debugging information and Spanish localization.

## Building a new Docker image

The `build` script uses tools built into a container image pulled from a public Docker Hub repository.  If you like, you can build your own image from the `Dockerfile` in this directory with the command

```bash
    docker build -t <name> .
```

Where <name> is the name that will be applied to the image.  If you then change the value of `$LOCAL_IMAGE` in the `build` script to match <name>, `build` will use the new image for building CircuitPython.
