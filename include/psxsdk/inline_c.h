#ifndef INLINE_C_H
#define INLINE_C_H

/**
 * @file
 * @brief GTE inline macros from PsyQ INLINE_C.H that libgte.h does not carry.
 *
 * Spelled as the SDK spells them. The bare op uses the GAS encoding of the
 * instruction word, as libgte.h does for AVSZ3/AVSZ4; the SDK's own
 * ".word 0x0000117f" is for Sony's assembler.
 */

/* Load the 3x3 of a MATRIX into the light matrix (L11..L33). */
#define gte_SetLightMatrix(r0) __asm__ volatile (        \
    "lw     $12, 0( %0 );"                               \
    "lw     $13, 4( %0 );"                               \
    "ctc2   $12, $8;"                                    \
    "ctc2   $13, $9;"                                    \
    "lw     $12, 8( %0 );"                               \
    "lw     $13, 12( %0 );"                              \
    "lw     $14, 16( %0 );"                              \
    "ctc2   $12, $10;"                                   \
    "ctc2   $13, $11;"                                   \
    "ctc2   $14, $12"                                    \
    :                                                    \
    : "r"(r0)                                            \
    : "$12", "$13", "$14")

/* Load the background colour (RBK, GBK, BBK). */
#define gte_ldbkdir(r0, r1, r2) __asm__ volatile (       \
    "ctc2   %0, $13;"                                    \
    "ctc2   %1, $14;"                                    \
    "ctc2   %2, $15"                                     \
    :                                                    \
    : "r"(r0), "r"(r1), "r"(r2))

/* Load the far colour (RFC, GFC, BFC) from three 8-bit components, scaled into
 * the GTE's 1/16 units. gte_ldfc takes a VECTOR already in those units. */
#define gte_ldfcb(r0, r1, r2) __asm__ volatile (          \
    "sll    $12, %0, 4;"                                 \
    "sll    $13, %1, 4;"                                 \
    "sll    $14, %2, 4;"                                 \
    "ctc2   $12, $21;"                                   \
    "ctc2   $13, $22;"                                   \
    "ctc2   $14, $23"                                    \
    :                                                    \
    : "r"(r0), "r"(r1), "r"(r2)                          \
    : "$12", "$13", "$14")

/* NCLIP -- outer product of the three screen points, sign gives the winding. */
#define gte_nclip() __asm__ volatile (                   \
    "nop;"                                               \
    "nop;"                                               \
    ".word  0x4B400006"                                  \
    : : )

/* Store MAC0 (the NCLIP result). */
#define gte_stopz(r0) __asm__ volatile (                 \
    "swc2   $24, 0( %0 )"                                \
    :                                                    \
    : "r"(r0)                                            \
    : "memory")

/* Store SXY0..SXY2 straight into a POLY_GT3's three vertices. */
#define gte_stsxy3_gt3(r0) __asm__ volatile (            \
    "swc2   $12, 8( %0 );"                               \
    "swc2   $13, 20( %0 );"                              \
    "swc2   $14, 32( %0 )"                               \
    :                                                    \
    : "r"(r0)                                            \
    : "memory")

/* Store SXY0..SXY2 straight into a POLY_FT3's three vertices. */
#define gte_stsxy3_ft3(r0) __asm__ volatile (            \
    "swc2   $12, 8( %0 );"                               \
    "swc2   $13, 16( %0 );"                              \
    "swc2   $14, 24( %0 )"                               \
    :                                                    \
    : "r"(r0)                                            \
    : "memory")

/* RT -- rotation matrix times V0, plus the translation vector, unscaled. */
#define gte_rt() gte_mvmva(1, 0, 0, 0, 0)

/* Load the packed RGB source DPCS reads (GTE data register 6). */
#define gte_ldrgb(r0) __asm__ volatile (                 \
    "lwc2   $6, 0( %0 )"                                 \
    :                                                    \
    : "r"(r0))

/* DPCS -- fade the loaded colour toward the far colour by IR0. */
#define gte_dpcs() __asm__ volatile (                    \
    "nop;"                                               \
    "nop;"                                               \
    ".word  0x4A780010"                                  \
    : : )

/* Load the three colour FIFO slots, and RGBC from the third. */
#define gte_ldrgb3(r0, r1, r2) __asm__ volatile (        \
    "lwc2   $20, 0( %0 );"                               \
    "lwc2   $21, 0( %1 );"                               \
    "lwc2   $22, 0( %2 );"                               \
    "lwc2   $6,  0( %2 )"                                \
    :                                                    \
    : "r"(r0), "r"(r1), "r"(r2))

/* DPCT -- run DPCS over all three colours in the FIFO. */
#define gte_dpct() __asm__ volatile (                    \
    "nop;"                                               \
    "nop;"                                               \
    ".word  0x4AF8002A"                                  \
    : : )

/* Store the three colour FIFO slots. */
#define gte_strgb3(r0, r1, r2) __asm__ volatile (        \
    "swc2   $20, 0( %0 );"                               \
    "swc2   $21, 0( %1 );"                               \
    "swc2   $22, 0( %2 )"                                \
    :                                                    \
    : "r"(r0), "r"(r1), "r"(r2)                          \
    : "memory")

/* GPF -- IR1..IR3 = IR0 * IR1..IR3, shifted 12. Two nops cover the stall
 * between the mtc2 that loads IR and the op that reads it. */
#define gte_gpf1() __asm__ volatile (                    \
    "nop;"                                               \
    "nop;"                                               \
    ".word  0x4B98003D"                                  \
    : : )

/* GPL -- IR1..IR3 += IR0 * IR1..IR3, shifted 12. */
#define gte_gpl1() __asm__ volatile (                    \
    "nop;"                                               \
    "nop;"                                               \
    ".word  0x4BA8003E"                                  \
    : : )

/* Load the 3x3 of a MATRIX into the colour matrix (LR1..LB3). */
#define gte_SetColorMatrix(r0) __asm__ volatile (         \
    "lw     $12, 0( %0 );"                                \
    "lw     $13, 4( %0 );"                                \
    "ctc2   $12, $16;"                                    \
    "ctc2   $13, $17;"                                    \
    "lw     $12, 8( %0 );"                                \
    "lw     $13, 12( %0 );"                               \
    "lw     $14, 16( %0 );"                               \
    "ctc2   $12, $18;"                                    \
    "ctc2   $13, $19;"                                    \
    "ctc2   $14, $20"                                     \
    :                                                     \
    : "r"(r0)                                             \
    : "$12", "$13", "$14")

/* Load packed screen coordinates for the NCLIP winding test. */
#define gte_ldsxy3(r0, r1, r2) __asm__ volatile (         \
    "mtc2 %0, $12;"                                       \
    "mtc2 %2, $14;"                                       \
    "mtc2 %1, $13"                                        \
    : : "r"(r0), "r"(r1), "r"(r2))

#endif /* INLINE_C_H */
