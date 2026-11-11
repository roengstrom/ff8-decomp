#ifndef FE_OBJECT3_H
#define FE_OBJECT3_H

#include "common.h"
#include "field.h"

/** @brief One pending CD-load request queued by fe_object3 / drained by fe_object4. */
typedef struct {
    /* 0x00 */ s32 unk0;    /**< LBA / disc sector. */
    /* 0x04 */ s32 unk4;    /**< Size in bytes (0x800-aligned). */
    /* 0x08 */ s32 unk8;    /**< Stride / advance in 4-byte words. */
} CmdEntry; /* 12 bytes */

/** @brief The pending CD-load command queue at @c D_800DE7B0. */
typedef struct {
    /* 0x00 */ s32 count;
    /* 0x04 */ u8  pad4[4];
    /* 0x08 */ CmdEntry entries[1];
} CmdQueue;

extern CmdQueue D_800DE7B0;

extern void func_800ADAF4(s32 a, u32 size, u32 c);

extern void FieldPause(void (*callback)(void));

#endif
