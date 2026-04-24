#ifndef LIBGTE_H
#define LIBGTE_H

#include "common.h"

#define ONE 4096 /**< GTE fixed-point 1.0 (12-bit fractional). */

/** @brief 3x3 rotation matrix with translation vector (32 bytes). */
typedef struct {
    s16 m[3][3];    /**< 3x3 rotation matrix (fixed point). */
    s32 t[3];       /**< Translation vector (tx, ty, tz). */
} MATRIX;

/** @brief Long word 3D vector. */
typedef struct {
    s32 vx, vy;
    s32 vz, pad;
} VECTOR;

/** @brief Short word 3D vector. */
typedef struct {
    s16 vx, vy;
    s16 vz, pad;
} SVECTOR;

/** @brief Color vector. */
typedef struct {
    u8 r, g, b, cd;
} CVECTOR;

/** @brief 2D short vector. */
typedef struct {
    s16 vx, vy;
} DVECTOR;

/* --- GTE matrix operations --- */

void SetRotMatrix(MATRIX *m);
void SetTransMatrix(MATRIX *m);
void ScaleMatrix(MATRIX *m, VECTOR *scale);
void ScaleMatrixL(MATRIX *m, VECTOR *scale);

/* --- GTE transform operations --- */

s32 RotTransPers(SVECTOR *v0, s32 *sxy, s32 *p, s32 *flag);

/* --- GTE initialization --- */

void InitGeom(void);

/* --- GTE math --- */

s32 SquareRoot0(s32 a);
s32 rsin(s32 a);
s32 rcos(s32 a);

/* --- Inline GTE (COP2) opcode macros ---
 *
 * These expand to @c __asm__ volatile sequences that emit raw COP2 opcodes,
 * so the caller can pipeline multiple GTE ops with the matching delay slots
 * that @c cc1 schedules. Equivalent to PsyQ's @c inline_c.h but re-encoded
 * for GAS: where the PsyQ SDK used assembler-side rewrites of bogus
 * @c 0x000013bf -style constants, we emit the real @c 0x4A... COP2 bytes
 * (same convention as sotn-decomp's @c gte.inc).
 */

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

#define gte_ldtr( x, y, z ) __asm__ volatile (           \
    "ctc2   %0, $5;"                                     \
    "ctc2   %1, $6;"                                     \
    "ctc2   %2, $7"                                      \
    :                                                    \
    : "r"( x ), "r"( y ), "r"( z ) )

#define gte_stlvnl( r0 ) __asm__ volatile (              \
    "swc2   $25, 0( %0 );"                               \
    "swc2   $26, 4( %0 );"                               \
    "swc2   $27, 8( %0 )"                                \
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

#endif /* LIBGTE_H */
