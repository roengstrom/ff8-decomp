/* Jump tables for INCLUDE_ASM functions in 1D2C.c.
 * These must be in the same compilation unit as the code that defines
 * the .L local labels they reference. Persistent file — not wiped by make split.
 * Corresponding entries in asm/data/800.rodata.s must be removed after each split.
 */

.section .rodata, "a"

.align 3
nonmatching jtbl_80010000

dlabel jtbl_80010000
    .word .L800116F4
    .word .L800116BC
    .word .L800116CC
    .word .L800116DC
    .word .L800116EC
    .word 0x00000000
enddlabel jtbl_80010000

.align 3
nonmatching jtbl_80010018

dlabel jtbl_80010018
    .word .L8001204C
    .word .L80012064
    .word .L80012284
    .word .L80012284
    .word .L8001278C
    .word .L80012444
    .word .L8001278C
    .word .L80012264
    .word .L80012714
    .word .L80012444
enddlabel jtbl_80010018
