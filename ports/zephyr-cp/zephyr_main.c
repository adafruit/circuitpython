#include <stdio.h>

extern int circuitpython_main(void);

#if defined(__SANITIZE_ADDRESS__) || (defined(__has_feature) && __has_feature(address_sanitizer))
// ASAN's stack-use-after-return detection (on by default in recent runtimes)
// moves C locals into "fake stack" frames on the heap. The GC scans the real
// machine stack for live object pointers, so with fake stacks enabled it misses
// pointers held in C frames and collects still-referenced objects, corrupting
// the heap. GCC's instrumentation consults this global at every instrumented
// function entry, so clearing it from a constructor disables only that check;
// all other ASAN checks stay enabled.
extern char __asan_option_detect_stack_use_after_return;
__attribute__((constructor)) static void cp_disable_asan_fake_stack(void) {
    __asan_option_detect_stack_use_after_return = 0;
}
#endif

int main(void) {
    // Use a unique name for CP main so that the linker needs to look in libcircuitpython.a
    return circuitpython_main();
}
