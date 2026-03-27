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

#endif /* LIBGTE_H */
