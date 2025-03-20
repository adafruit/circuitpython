#include <stdio.h>

extern int circuitpython_main(void);

int main(void) {
    // Use a unique name for CP main so that the linker needs to look in libcircuitpython.a
    return circuitpython_main();
}
