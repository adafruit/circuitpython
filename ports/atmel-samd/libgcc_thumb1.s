@ Thin replacements for libgcc soft-float and bit helpers on Cortex-M0+. Each builds on a
@ libgcc routine the firmware links anyway, so the larger archive member is never pulled in.
@ Results are bit-identical to libgcc's, including NaN handling.

    .syntax unified
    .cpu cortex-m0plus
    .thumb

@ float __aeabi_fsub(float a, float b): a + (-b). A NaN b keeps its sign, as in libgcc.
    .section .text.__aeabi_fsub,"ax",%progbits
    .global __aeabi_fsub
    .type __aeabi_fsub, %function
    .thumb_func
__aeabi_fsub:
    lsls r2, r1, #1
    ldr r3, =0xff000000
    cmp r2, r3
    bhi 1f
    movs r2, #1
    lsls r2, r2, #31
    eors r1, r2
1:
    ldr r2, =__aeabi_fadd
    bx r2
    .size __aeabi_fsub, . - __aeabi_fsub
    .ltorg

@ int __aeabi_fcmp{eq,lt,le,gt,ge}(float a, float b), all from __lesf2, which returns
@ -1, 0 or 1 when ordered and 2 when either operand is NaN.
    .section .text.__aeabi_fcmp,"ax",%progbits
    .global __aeabi_fcmpgt
    .type __aeabi_fcmpgt, %function
    .thumb_func
__aeabi_fcmpgt:
    mov r2, r0
    mov r0, r1
    mov r1, r2
    .global __aeabi_fcmplt
    .type __aeabi_fcmplt, %function
    .thumb_func
__aeabi_fcmplt:
    push {r4, lr}
    bl __lesf2
    lsrs r0, r0, #31
    pop {r4, pc}

    .global __aeabi_fcmpge
    .type __aeabi_fcmpge, %function
    .thumb_func
__aeabi_fcmpge:
    mov r2, r0
    mov r0, r1
    mov r1, r2
    .global __aeabi_fcmple
    .type __aeabi_fcmple, %function
    .thumb_func
__aeabi_fcmple:
    push {r4, lr}
    bl __lesf2
    subs r0, r0, #1
    lsrs r0, r0, #31
    pop {r4, pc}

    .global __aeabi_fcmpeq
    .type __aeabi_fcmpeq, %function
    .thumb_func
__aeabi_fcmpeq:
    push {r4, lr}
    bl __lesf2
    negs r1, r0
    adcs r0, r1
    pop {r4, pc}
    .size __aeabi_fcmpgt, . - __aeabi_fcmpgt

@ float __aeabi_i2f(int x): the unsigned conversion of |x|, with the sign set for x < 0.
@ -INT_MIN is 0x80000000 as unsigned, so INT_MIN converts exactly.
    .section .text.__aeabi_i2f,"ax",%progbits
    .global __aeabi_i2f
    .type __aeabi_i2f, %function
    .thumb_func
__aeabi_i2f:
    cmp r0, #0
    bge 1f
    push {r4, lr}
    negs r0, r0
    bl __aeabi_ui2f
    movs r1, #1
    lsls r1, r1, #31
    orrs r0, r1
    pop {r4, pc}
1:
    ldr r1, =__aeabi_ui2f
    bx r1
    .size __aeabi_i2f, . - __aeabi_i2f
    .ltorg

@ int __ffssi2(int x): 1 + index of the lowest set bit, 0 for x == 0.
    .section .text.__ffssi2,"ax",%progbits
    .global __ffssi2
    .type __ffssi2, %function
    .thumb_func
__ffssi2:
    negs r1, r0
    ands r0, r1
    beq 1f
    push {r4, lr}
    bl __clzsi2
    movs r1, #32
    subs r0, r1, r0
    pop {r4, pc}
1:
    bx lr
    .size __ffssi2, . - __ffssi2
