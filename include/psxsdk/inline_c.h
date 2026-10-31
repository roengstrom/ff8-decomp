#ifndef INLINE_C_H
#define INLINE_C_H

/**
 * @file
 * @brief GTE inline macros from PsyQ INLINE_C.H.
 *
 * Spelled as the SDK spells them. The bare ops use the GAS encoding of the
 * instruction word; the SDK's own ".word 0x0000117f" style is for Sony's
 * assembler.
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
#define gte_SetFarColor( r0, r1, r2 ) __asm__ volatile ( \
    "sll    $12, %0, 4;"                                 \
    "sll    $13, %1, 4;"                                 \
    "sll    $14, %2, 4;"                                 \
    "ctc2   $12, $21;"                                   \
    "ctc2   $13, $22;"                                   \
    "ctc2   $14, $23"                                    \
    :                                                    \
    : "r"( r0 ), "r"( r1 ), "r"( r2 )                    \
    : "$12", "$13", "$14" )

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
#define gte_gpf12() __asm__ volatile (                    \
    "nop;"                                               \
    "nop;"                                               \
    ".word  0x4B98003D"                                  \
    : : )

/* GPL -- IR1..IR3 += IR0 * IR1..IR3, shifted 12. */
#define gte_gpl12() __asm__ volatile (                    \
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

#define gte_SetRotMatrix( r0 ) __asm__ volatile (        \
    "lw     $12, 0( %0 );"                               \
    "lw     $13, 4( %0 );"                               \
    "ctc2   $12, $0;"                                    \
    "ctc2   $13, $1;"                                    \
    "lw     $12, 8( %0 );"                               \
    "lw     $13, 12( %0 );"                              \
    "lw     $14, 16( %0 );"                              \
    "ctc2   $12, $2;"                                    \
    "ctc2   $13, $3;"                                    \
    "ctc2   $14, $4"                                     \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "$13", "$14" )

#define gte_SetTransMatrix( r0 ) __asm__ volatile (      \
    "lw     $12, 20( %0 );"                              \
    "lw     $13, 24( %0 );"                              \
    "ctc2   $12, $5;"                                    \
    "lw     $14, 28( %0 );"                              \
    "ctc2   $13, $6;"                                    \
    "ctc2   $14, $7"                                     \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "$13", "$14" )

/* Read the rotation matrix back out of the GTE into the MATRIX @p r0 --
 * the 3x3 at +0 and the translation vector at +20, so it is the inverse of
 * gte_SetRotMatrix plus gte_SetTransMatrix in one go. */
#define gte_ReadRotMatrix( r0 ) __asm__ volatile (       \
    "cfc2   $12, $0;"                                    \
    "cfc2   $13, $1;"                                    \
    "sw     $12, 0( %0 );"                               \
    "sw     $13, 4( %0 );"                               \
    "cfc2   $12, $2;"                                    \
    "cfc2   $13, $3;"                                    \
    "cfc2   $14, $4;"                                    \
    "sw     $12, 8( %0 );"                               \
    "sw     $13, 12( %0 );"                              \
    "sw     $14, 16( %0 );"                              \
    "cfc2   $12, $5;"                                    \
    "cfc2   $13, $6;"                                    \
    "cfc2   $14, $7;"                                    \
    "sw     $12, 20( %0 );"                              \
    "sw     $13, 24( %0 );"                              \
    "sw     $14, 28( %0 )"                               \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "$13", "$14", "memory" )

/* Read the translation vector back out of the GTE into @p r0 (inverse of
 * gte_SetTransVector). */
#define gte_sttr( r0 ) __asm__ volatile (                \
    "cfc2   $12, $5;"                                    \
    "cfc2   $13, $6;"                                    \
    "cfc2   $14, $7;"                                    \
    "sw     $12, 0( %0 );"                               \
    "sw     $13, 4( %0 );"                               \
    "sw     $14, 8( %0 )"                                \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "$13", "$14", "memory" )

#define gte_SetTransVector( r0 ) __asm__ volatile (      \
    "lw     $12, 0( %0 );"                               \
    "lw     $13, 4( %0 );"                               \
    "lw     $14, 8( %0 );"                               \
    "ctc2   $12, $5;"                                    \
    "ctc2   $13, $6;"                                    \
    "ctc2   $14, $7"                                     \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "$13", "$14" )

#define gte_ldv0( r0 ) __asm__ volatile (                \
    "lwc2   $0, 0( %0 );"                                \
    "lwc2   $1, 4( %0 )"                                 \
    :                                                    \
    : "r"( r0 ) )

/* Load the three s16 components of the SVECTOR @p r0 into IR1/IR2/IR3
 * (mtc2 to GTE data regs $9/$10/$11). */
#define gte_ldsv( r0 ) __asm__ volatile (                \
    "lhu    $12, 0( %0 );"                               \
    "lhu    $13, 2( %0 );"                               \
    "lhu    $14, 4( %0 );"                               \
    "mtc2   $12, $9;"                                    \
    "mtc2   $13, $10;"                                   \
    "mtc2   $14, $11"                                    \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "$13", "$14" )

/* SQR at full precision (sf 0): square IR1/IR2/IR3 into MAC1/MAC2/MAC3,
 * and back to IR via the limiter. The instruction is COP2 function 0x28 with
 * sf in bit 19. Two nops cover the GTE data-reg load stall before SQR can read the IR
 * registers. */
#define gte_sqr0() __asm__ volatile ( \
    "nop;" \
    "nop;" \
    ".word 0x4AA00428" \
    : : )

#define gte_ldtr( x, y, z ) __asm__ volatile (           \
    "ctc2   %0, $5;"                                     \
    "ctc2   %1, $6;"                                     \
    "ctc2   %2, $7"                                      \
    :                                                    \
    : "r"( x ), "r"( y ), "r"( z ) )

/* Load the first OP (outer/cross product) operand. The three words of the
 * VECTOR @p r0 go into the rotation-matrix diagonal R11/R22/R33, which OP
 * reads as its D1/D2/D3 vector. */
#define gte_ldopv1( r0 ) __asm__ volatile (              \
    "lw     $12, 0( %0 );"                               \
    "lw     $13, 4( %0 );"                               \
    "ctc2   $12, $0;"                                    \
    "lw     $14, 8( %0 );"                               \
    "ctc2   $13, $2;"                                    \
    "ctc2   $14, $4"                                     \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "$13", "$14" )

/* Load the second OP operand. The three words of the VECTOR @p r0 go
 * straight into IR1/IR2/IR3. */
#define gte_ldopv2( r0 ) __asm__ volatile (              \
    "lwc2   $11, 8( %0 );"                               \
    "lwc2   $9, 0( %0 );"                                \
    "lwc2   $10, 4( %0 )"                                \
    :                                                    \
    : "r"( r0 ) )

#define gte_stlvnl( r0 ) __asm__ volatile (              \
    "swc2   $25, 0( %0 );"                               \
    "swc2   $26, 4( %0 );"                               \
    "swc2   $27, 8( %0 )"                                \
    :                                                    \
    : "r"( r0 )                                          \
    : "memory" )

/* Load IR1/IR2/IR3 from the three words of the VECTOR @p r0. */
#define gte_ldlvl( r0 ) __asm__ volatile (               \
    "lwc2   $9, 0( %0 );"                                \
    "lwc2   $10, 4( %0 );"                               \
    "lwc2   $11, 8( %0 )"                                \
    :                                                    \
    : "r"( r0 ) )

/* Load the far colour (RFC/GFC/BFC) from the VECTOR @p r0 -- the colour
 * INTPL fades towards. */
#define gte_ldfc( r0 ) __asm__ volatile (                \
    "lw     $12, 0( %0 );"                               \
    "lw     $13, 4( %0 );"                               \
    "lw     $14, 8( %0 );"                               \
    "ctc2   $12, $21;"                                   \
    "ctc2   $13, $22;"                                   \
    "ctc2   $14, $23"                                    \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "$13", "$14" )

/* Load one column of the 3x3 matrix @p r0 (row stride 6) into
 * IR1/IR2/IR3. PSY-Q INLINE_C.H gte_ldclmv. */
#define gte_ldclmv( r0 ) __asm__ volatile (              \
    "lhu    $12, 0( %0 );"                               \
    "lhu    $13, 6( %0 );"                               \
    "lhu    $14, 12( %0 );"                              \
    "mtc2   $12, $9;"                                    \
    "mtc2   $13, $10;"                                   \
    "mtc2   $14, $11"                                    \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "$13", "$14" )

/* Store IR1/IR2/IR3 back to one column of the 3x3 matrix @p r0 (row
 * stride 6). PSY-Q INLINE_C.H gte_stclmv. */
#define gte_stclmv( r0 ) __asm__ volatile (              \
    "mfc2   $12, $9;"                                    \
    "mfc2   $13, $10;"                                   \
    "mfc2   $14, $11;"                                   \
    "sh     $12, 0( %0 );"                               \
    "sh     $13, 6( %0 );"                               \
    "sh     $14, 12( %0 )"                               \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "$13", "$14", "memory" )

/* Rotate IR by the current rotation matrix. PSY-Q INLINE_C.H spells this
 * ".word 0x000001ff", but that constant is for Sony's own assembler; under GAS
 * the same operation is MVMVA(1,0,3,3,0), which is what the ROM contains. */
#define gte_rtir() gte_mvmva(1, 0, 3, 3, 0)

/* Load OP operand 1 from an SVECTOR into the rotation-matrix diagonal.
 * PSY-Q INLINE_C.H gte_ldopv1SV. */
#define gte_ldopv1SV( r0 ) __asm__ volatile (            \
    "lh     $12, 0( %0 );"                               \
    "lh     $13, 2( %0 );"                               \
    "ctc2   $12, $0;"                                    \
    "lh     $14, 4( %0 );"                               \
    "ctc2   $13, $2;"                                    \
    "ctc2   $14, $4"                                     \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "$13", "$14" )

/* Load OP operand 2 from an SVECTOR into IR1/IR2/IR3.
 * PSY-Q INLINE_C.H gte_ldopv2SV. */
#define gte_ldopv2SV( r0 ) __asm__ volatile (            \
    "lh     $12, 0( %0 );"                               \
    "lh     $13, 2( %0 );"                               \
    "lh     $14, 4( %0 );"                               \
    "mtc2   $12, $9;"                                    \
    "mtc2   $13, $10;"                                   \
    "mtc2   $14, $11"                                    \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "$13", "$14" )

/* Load a VECTOR into V0, packing X/Y into one word. PSY-Q INLINE_C.H gte_ldlv0. */
#define gte_ldlv0( r0 ) __asm__ volatile (               \
    "lhu    $13, 4( %0 );"                               \
    "lhu    $12, 0( %0 );"                               \
    "sll    $13, $13, 16;"                               \
    "or     $12, $12, $13;"                              \
    "mtc2   $12, $0;"                                    \
    "lwc2   $1, 8( %0 )"                                 \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "$13" )

/* Store the GTE flag register. PSY-Q INLINE_C.H gte_stflg. */
#define gte_stflg( r0 ) __asm__ volatile (               \
    "cfc2   $12, $31;"                                   \
    "nop    ;"                                           \
    "sw     $12, 0( %0 )"                                \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "memory" )

// Load IR0, the interpolation fraction used by INTPL and DPCS.
#define gte_lddp( r0 ) __asm__ volatile (                \
    "mtc2   %0, $8"                                      \
    :                                                    \
    : "r"( r0 ) )

/* Store the packed RGB result (GTE data register 22). */
#define gte_strgb( r0 ) __asm__ volatile (               \
    "swc2   $22, 0( %0 )"                                \
    :                                                    \
    : "r"( r0 )                                          \
    : "memory" )

#define gte_mvmva_core( r0 ) __asm__ volatile (          \
    "nop;"                                               \
    "nop;"                                               \
    ".word  %0"                                          \
    :                                                    \
    : "g"( r0 ) )

/* MVMVA: opcode COP2 (0x12), CO bit set, funct 0x12. Base 0x4A400012 is
 * (sf=0, mx=0, v=0, cv=0, lm=0); shift factor at bit 19, matrix at 17,
 * vector at 15, translation vector at 13, limit at 10. Matches the encoding
 * used by sotn-decomp's @c MVMVA macro in @c include/gte.inc. */
#define gte_mvmva( sf, mx, v, cv, lm ) gte_mvmva_core(   \
    0x4A400012 | ((sf) << 19) | ((mx) << 17)             \
               | ((v)  << 15) | ((cv) << 13)             \
               | ((lm) << 10) )

/* Load V0/V1/V2 simultaneously from 3 separate SVECTORs. */
#define gte_ldv3( r0, r1, r2 ) __asm__ volatile (        \
    "lwc2   $0,  0( %0 );"                               \
    "lwc2   $1,  4( %0 );"                               \
    "lwc2   $2,  0( %1 );"                               \
    "lwc2   $3,  4( %1 );"                               \
    "lwc2   $4,  0( %2 );"                               \
    "lwc2   $5,  4( %2 )"                                \
    :                                                    \
    : "r"( r0 ), "r"( r1 ), "r"( r2 ) )

/* Load V0/V1/V2 simultaneously from 3 consecutive SVECTORs (24 bytes). */
#define gte_ldv3c( r0 ) __asm__ volatile (               \
    "lwc2   $0,  0( %0 );"                               \
    "lwc2   $1,  4( %0 );"                               \
    "lwc2   $2,  8( %0 );"                               \
    "lwc2   $3, 12( %0 );"                               \
    "lwc2   $4, 16( %0 );"                               \
    "lwc2   $5, 20( %0 )"                                \
    :                                                    \
    : "r"( r0 ) )

/* Store IR1/IR2/IR3 as a 3-component s16 vector (mfc2 + sh). */
#define gte_stsv( r0 ) __asm__ volatile (                \
    "mfc2   $12, $9;"                                    \
    "mfc2   $13, $10;"                                   \
    "mfc2   $14, $11;"                                   \
    "sh     $12, 0( %0 );"                               \
    "sh     $13, 2( %0 );"                               \
    "sh     $14, 4( %0 )"                                \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "$13", "$14", "memory" )

/* Store SXY0/SXY1/SXY2 to 3 separate destinations. */
#define gte_stsxy3( r0, r1, r2 ) __asm__ volatile (      \
    "swc2   $12, 0( %0 );"                               \
    "swc2   $13, 0( %1 );"                               \
    "swc2   $14, 0( %2 )"                                \
    :                                                    \
    : "r"( r0 ), "r"( r1 ), "r"( r2 )                    \
    : "memory" )

/* Store SXY0/SXY1/SXY2 to 3 consecutive words. */
#define gte_stsxy3c( r0 ) __asm__ volatile (             \
    "swc2   $12, 0( %0 );"                               \
    "swc2   $13, 4( %0 );"                               \
    "swc2   $14, 8( %0 )"                                \
    :                                                    \
    : "r"( r0 )                                          \
    : "memory" )

/* Store the single projected SXY from $14. */
#define gte_stsxy( r0 ) __asm__ volatile (               \
    "swc2   $14, 0( %0 )"                                \
    :                                                    \
    : "r"( r0 )                                          \
    : "memory" )

/* Store OTZ: SZ2 (GTE $19) scaled down by 4, the usual ordering-table index. */
#define gte_stszotz( r0 ) __asm__ volatile (             \
    "mfc2   $12, $19;"                                   \
    "nop;"                                               \
    "sra    $12, $12, 2;"                                \
    "sw     $12, 0( %0 )"                                \
    :                                                    \
    : "r"( r0 )                                          \
    : "$12", "memory" )

#define gte_stsz( r0 ) __asm__ volatile (                \
    "swc2   $19, 0( %0 )"                                \
    :                                                    \
    : "r"( r0 )                                          \
    : "memory" )

// Store OTZ, the depth in GTE register $7.
#define gte_stotz( r0 ) __asm__ volatile (               \
    "swc2   $7, 0( %0 )"                                 \
    :                                                    \
    : "r"( r0 )                                          \
    : "memory" )

/* Bare GTE ops (each emits 2 nops + the encoded instruction word). */
#define gte_rtpt() __asm__ volatile (                    \
    "nop;"                                               \
    "nop;"                                               \
    ".word  0x4A280030"                                  \
    : : )

#define gte_rtps() __asm__ volatile (                    \
    "nop;"                                               \
    "nop;"                                               \
    ".word  0x4A180001"                                  \
    : : )

/* INTPL -- fade IR1/IR2/IR3 towards the far colour by the fraction in IR0,
 * leaving the packed result in RGB2. */
#define gte_intpl() __asm__ volatile (                   \
    "nop;"                                               \
    "nop;"                                               \
    ".word  0x4A980011"                                  \
    : : )

/* Average of the three Z values in SZ1..SZ3, scaled by ZSF3, into OTZ.
 * PSY-Q INLINE_C.H spells this ".word 0x000011bf", but that constant is for
 * Sony's own assembler; under GAS the same operation is the word below. */
#define gte_avsz3() __asm__ volatile (                   \
    "nop;"                                               \
    "nop;"                                               \
    ".word  0x4B58002D"                                  \
    : : )

#define gte_avsz4() __asm__ volatile (                   \
    "nop;"                                               \
    "nop;"                                               \
    ".word  0x4B68002E"                                  \
    : : )

/* Outer product with sf=1. PSY-Q INLINE_C.H spells this ".word 0x0000123f";
 * in the GAS encoding that is gte_op0's word with the sf bit (19) set. */
#define gte_op12() __asm__ volatile (                    \
    "nop;"                                               \
    "nop;"                                               \
    ".word  0x4B78000C"                                  \
    : : )

/* OP with sf 0: the cross product of the vector loaded by @ref gte_ldopv1
 * with the one loaded by @ref gte_ldopv2, at full precision (no >>12), left
 * in MAC1..MAC3 for @ref gte_stlvnl. */
#define gte_op0() __asm__ volatile (                     \
    "nop;"                                               \
    "nop;"                                               \
    ".word  0x4B70000C"                                  \
    : : )

#endif /* INLINE_C_H */
