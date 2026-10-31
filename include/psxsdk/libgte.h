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

/* --- GTE matrix operations (signatures match PsyQ 4.6 LIBGTE.H) --- */

MATRIX *RotMatrix(SVECTOR *r, MATRIX *m);
MATRIX *RotMatrixYXZ(SVECTOR *r, MATRIX *m);
MATRIX *RotMatrixZ(s32 r, MATRIX *m);
MATRIX *CompMatrix(MATRIX *m0, MATRIX *m1, MATRIX *m2);
MATRIX *MulMatrix(MATRIX *m0, MATRIX *m1);
MATRIX *MulMatrix0(MATRIX *m0, MATRIX *m1, MATRIX *m2);   /**< @p m2 = @p m0 * @p m1, leaving both operands alone. */
MATRIX *ScaleMatrix(MATRIX *m, VECTOR *scale);
MATRIX *ScaleMatrixL(MATRIX *m, VECTOR *scale);
MATRIX *TransposeMatrix(MATRIX *m0, MATRIX *m1);
void MatrixNormal_2(MATRIX *m, MATRIX *n);   /**< Orthonormalise @p m into @p n via the GTE. */
void SetRotMatrix(MATRIX *m);
void SetTransMatrix(MATRIX *m);
void SetLightMatrix(MATRIX *m);
void SetColorMatrix(MATRIX *m);
void SetBackColor(s32 rbk, s32 gbk, s32 bbk);
void SetFarColor(s32 rfc, s32 gfc, s32 bfc);

/* --- GTE transform operations --- */

VECTOR *ApplyMatrixLV(MATRIX *m, VECTOR *v0, VECTOR *v1);
VECTOR *ApplyTransposeMatrixLV(MATRIX *m, VECTOR *v0, VECTOR *v1);
SVECTOR *ApplyMatrixSV(MATRIX *m, SVECTOR *v0, SVECTOR *v1);
s32 RotTransPers(SVECTOR *v0, s32 *sxy, s32 *p, s32 *flag);
void LoadAverageShort12(SVECTOR *v0, SVECTOR *v1, s32 p0, s32 p1, SVECTOR *v2);
void LoadAverageCol(u8 *v0, u8 *v1, s32 p0, s32 p1, u8 *v2);
MATRIX *RotMatrixX(s32 r, MATRIX *m);
s32 RotTransPers3(SVECTOR *v0, SVECTOR *v1, SVECTOR *v2,
                  s32 *sxy0, s32 *sxy1, s32 *sxy2, s32 *p, s32 *flag);
s32 RotTransPers4(SVECTOR *v0, SVECTOR *v1, SVECTOR *v2, SVECTOR *v3,
                  s32 *sxy0, s32 *sxy1, s32 *sxy2, s32 *sxy3,
                  s32 *p, s32 *flag);
s32 NormalClip(s32 sxy0, s32 sxy1, s32 sxy2);

/* --- GTE color/lighting --- */

void DpqColor(CVECTOR *v0, s32 p, CVECTOR *v1);

/* --- GTE initialization --- */

void InitGeom(void);
void SetGeomOffset(s32 ofx, s32 ofy);   /**< Set GTE screen offset OFX/OFY (signed, scaled <<16). */
void SetGeomScreen(s32 h);   /**< Set GTE projection-plane distance H (screen depth). */

/* --- GTE math --- */

s32 SquareRoot0(s32 a);
s32 SquareRoot12(s32 a);   /**< Square root of a 20.12 fixed-point value. */
s32 VectorNormal(VECTOR *v0, VECTOR *v1);   /**< Normalise @p v0 into @p v1; returns the squared length. */
s32 VectorNormalS(VECTOR *v0, SVECTOR *v1);   /**< Normalise @p v0 into short @p v1; returns the squared length. */
s32 VectorNormalSS(SVECTOR *v0, SVECTOR *v1);   /**< Normalise a short vector; returns the squared length. */
s32 rsin(s32 a);
s32 rcos(s32 a);
s32 ratan2(s32 y, s32 x);   /**< Angle of (x, y); 0x1000 = full circle. */

/* r3 = r1 * r2, a column at a time. PSY-Q GTEMAC.H gte_MulMatrix0. */
#define gte_MulMatrix0(r1, r2, r3)                       \
    {   gte_SetRotMatrix(r1);                            \
        gte_ldclmv(r2);                                  \
        gte_rtir();                                      \
        gte_stclmv(r3);                                  \
        gte_ldclmv((char *)r2 + 2);                      \
        gte_rtir();                                      \
        gte_stclmv((char *)r3 + 2);                      \
        gte_ldclmv((char *)r2 + 4);                      \
        gte_rtir();                                      \
        gte_stclmv((char *)r3 + 4);   }

#endif /* LIBGTE_H */
