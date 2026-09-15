#ifndef WORLD_WE_OBJECT13_H
#define WORLD_WE_OBJECT13_H

#include "common.h"
#include "world.h"

/**
 * @brief Multi-sector CD streaming controller state.
 *
 * Drives the block-based read pipeline used by the world engine for streaming
 * data off disc. @c buffers points at a NULL-terminated array of destination
 * buffers (one per buffer slot); @c blocksPerBuf is the per-buffer block
 * count (reloaded into @c remaining when advancing to the next buffer). Each
 * tick decrements @c remaining while reading the @c blockIdx-th block of the
 * current buffer.
 */
typedef struct StreamState {
    u8 *volatile *buffers;       /* 0x00: NULL-terminated array of destination buffers (slots are volatile pointers) */
    u32 pad_04;
    volatile s32 blocksPerBuf;   /* 0x08: blocks to read into each buffer (reload for remaining) */
    u32 pad_0C;
    volatile s32 remaining;      /* 0x10: blocks remaining in current buffer */
    volatile s32 field_14;       /* 0x14 */
    volatile s32 field_18;       /* 0x18: VSync() snapshot */
    volatile s32 expectedSeq;    /* 0x1C: expected sequence number */
    volatile s32 blockIdx;       /* 0x20: current block index within buffer */
    volatile u8 bufIdx;          /* 0x24: current buffer index */
    volatile u8 status;          /* 0x25: status flag (0 idle, 2 error/done) */
} StreamState;

extern StreamState D_800E3E70;
extern void (*D_800E3E60)(s32, void *);
extern u8   D_800987C0;
extern u32  D_80082C14;

extern void func_80047C3C(u8 *msg);

/* Advance the streaming pipeline one tick. Returns the controller's status:
   0 while idle, -1 on error, otherwise the pending block count. */
extern s32 func_800C4558(s32 nowait, s32 *out);
/* Initialize multi-sector streaming and start the first buffer. */
extern s32 func_800C4480(s32 startSeq, s32 blocksPerBuf, u8 *volatile *buffers);
/* Reset the streaming controller; returns its status word. */
extern s32 func_800C4450(void);

extern void func_800C4AE4(s32 a);

#endif /* WORLD_WE_OBJECT13_H */
