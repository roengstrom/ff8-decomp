#include "common.h"
#include "gamestate.h"
#include "battle.h"

extern void func_80043578(s32);
extern void func_8004358C(s32);
extern void func_800435A0(s32, s32, s32);

/** Initializes audio/display subsystem. */
void func_800C40F8(void) {
    func_80043578(0);
    func_8004358C(0);
    func_800435A0(9, 0, 0);
}

/**
 * @brief Multi-sector CD streaming controller.
 *
 * Drives the block-based read pipeline used by the world engine for streaming
 * data off disc. @c buffers points at a NULL-terminated array of destination
 * buffers (one per buffer slot); @c blocksPerBuf is the per-buffer block
 * count (reloaded into @c remaining when advancing to the next buffer). Each
 * tick decrements @c remaining while reading the @c blockIdx-th block of the
 * current buffer.
 */
typedef struct StreamState {
    u8 **buffers;                /* 0x00: NULL-terminated array of destination buffers */
    u32 pad_04;
    volatile s32 blocksPerBuf;   /* 0x08: blocks to read into each buffer (reload for remaining) */
    u32 pad_0C;
    volatile s32 remaining;      /* 0x10: blocks remaining in current buffer */
    u32 pad_14[2];
    volatile s32 expectedSeq;    /* 0x1C: expected sequence number */
    volatile s32 blockIdx;       /* 0x20: current block index within buffer */
    u8  bufIdx;                  /* 0x24: current buffer index */
    volatile u8 status;          /* 0x25: status flag (0 idle, 2 error/done) */
} StreamState;

extern StreamState D_800E3E70;
extern void (*D_800E3E60)(s32, void *);

extern void func_8004397C(void *dst, s32 size);
extern s32  func_80043B04(void *buf);
extern void func_80047C3C(u8 *msg);
extern u8   D_800987C0;

/**
 * @brief Per-frame tick of the multi-sector CD streaming engine.
 *
 * On @p event == 1: services the in-flight read pipeline. While
 * @c D_800E3E70.remaining is positive, pulls the just-finished sector's
 * header via @c func_8004397C and validates the decoded sequence number
 * (from @c func_80043B04) against @c expectedSeq. On mismatch, logs
 * "CdRead: sector error" and aborts the buffer by setting @c remaining to
 * -1. After verify, if @c remaining is still positive, queues the next
 * block read at @c buffers[bufIdx] + blockIdx*0x800 (0x200 words = one
 * sector), decrements @c remaining, and advances the sequence/block indices.
 *
 * On any other @p event: aborts the in-flight buffer by setting
 * @c remaining to -1.
 *
 * Trailing bookkeeping: if @c remaining went negative, transitions
 * @c status to 2 (error/done). If @c remaining hit zero, advances
 * @c bufIdx; if the next slot is @c NULL the stream is exhausted so it
 * resets the reader via @c func_800C4450 and fires the registered
 * completion callback with @c (2, ctx). Otherwise reloads @c remaining from
 * @c blocksPerBuf and starts the next buffer.
 */
void func_800C4130(u8 event, void *ctx) {
    if (event == 1) {
        StreamState *s = &D_800E3E70;
        u8 header[0x10];

        if (s->remaining > 0) {
            func_8004397C(header, 3);
            if (func_80043B04(header) != s->expectedSeq) {
                func_80047C3C(&D_800987C0);
                s->remaining = -1;
            }
            if (s->remaining > 0) {
                func_8004397C(s->buffers[s->bufIdx] + s->blockIdx * 0x800, 0x200);
                s->remaining--;
                s->expectedSeq++;
                s->blockIdx++;
            }
        }
    } else {
        D_800E3E70.remaining = -1;
    }

    if (D_800E3E70.remaining < 0) {
        D_800E3E70.status = 2;
    } else if (D_800E3E70.remaining == 0) {
        D_800E3E70.bufIdx++;
        if (D_800E3E70.buffers[D_800E3E70.bufIdx] == NULL) {
            func_800C4450();
            if (D_800E3E60 != NULL) {
                D_800E3E60(2, ctx);
            }
        } else {
            D_800E3E70.blockIdx = 0;
            D_800E3E70.remaining = D_800E3E70.blocksPerBuf;
        }
    }
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C42D8);

/** Resets playback state and reinitializes subsystem.
 * @return Always returns 1.
 */
s32 func_800C4450(void) {
    D_800E3E70.remaining = 0;
    D_800E3E70.status = 0;
    func_800C40F8();
    return 1;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4480);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4558);

/** Exchanges the callback pointer, returning the previous value.
 * @param newVal New callback value to store.
 * @return Previous callback value.
 */
s32 func_800C4634(s32 newVal) {
    s32 old = (s32)D_800E3E60;
    D_800E3E60 = (void (*)(s32, void *))newVal;
    return old;
}

/** Clears all playback state and callback pointer. */
void func_800C4644(void) {
    D_800E3E60 = NULL;
    D_800E3E70.buffers = NULL;
    D_800E3E70.pad_04 = 0;
    D_800E3E70.blocksPerBuf = 0;
    D_800E3E70.pad_0C = 0;
    D_800E3E70.remaining = 0;
    D_800E3E70.pad_14[0] = 0;
    D_800E3E70.pad_14[1] = 0;
    D_800E3E70.expectedSeq = 0;
    D_800E3E70.blockIdx = 0;
    D_800E3E70.bufIdx = 0;
    D_800E3E70.status = 0;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4688);

/**
 * @brief Tick all active GFs' HP up by 1 toward their battle max.
 *
 * Iterates the 16 GF save entries. For each unlocked GF (@c exists bit 0)
 * with non-zero HP, if the current HP is below the GF's battle max (at
 * @c g_battleChars.gfEntries[i].hp), it's incremented by 1. Likely runs
 * once per field step to slowly regenerate GF HP while walking.
 *
 * @note Match requires reading @c g_gameState.gfs[i].hp directly at the
 *       compare AND increment — not through the cached @c hp local.
 */
void func_800C48C0(void) {
    s32 i;

    for (i = 0; i < 16; i++) {
        if (g_gameState.gfs[i].exists & 1) {
            u16 hp = g_gameState.gfs[i].hp;
            if (hp != 0) {
                if (g_gameState.gfs[i].hp < (s16)g_battleChars.gfEntries[i].hp) {
                    g_gameState.gfs[i].hp = g_gameState.gfs[i].hp + 1;
                }
            }
        }
    }
}

/**
 * @brief Tick each active party member's HP up by 1 toward their battle regen cap.
 *
 * Iterates the 3 party slots. For each occupied slot with its field-status
 * bit 0 set, increments the character's @c currentHp by 1 as long as it
 * stays below the slot's @c hpRegenCap (stored in the BattleCharData at
 * offset 0x174). Paired with @c func_800C48C0 which does the same for GF HP.
 *
 * @note Match requires reading @c g_gameState.mainData.party.party[i] and
 *       @c g_gameState.chars[charIdx].currentHp directly at each use site
 *       (not through cached locals) — identical pattern to func_800C48C0.
 */
void func_800C492C(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        u8 charIdx = g_gameState.mainData.party.party[i];
        if (charIdx != 0xFF) {
            if (g_battleChars.chars[i].fieldStatusByte & 1) {
                u16 hp = g_gameState.chars[g_gameState.mainData.party.party[i]].currentHp;
                if (hp != 0) {
                    if ((s32)g_gameState.chars[g_gameState.mainData.party.party[i]].currentHp <
                        (s32)g_battleChars.chars[i].hpRegenCap) {
                        g_gameState.chars[g_gameState.mainData.party.party[i]].currentHp =
                            g_gameState.chars[g_gameState.mainData.party.party[i]].currentHp + 1;
                    }
                }
            }
        }
    }
}

extern void sndPlaySfx(s32 sfxId, s32 a1, s32 a2, s32 a3);

/**
 * @brief Tick the currently-learning Angelo trick's points; unlock + play SFX at 0.
 *
 * When Rinoa (CHAR_RINOA = 4) is in the battle party, decrements the
 * learning-points counter for the currently-learning Angelo trick (index
 * at @c mainData.party.trickLearning). When the counter reaches 0 and the
 * trick isn't already completed, sets the completed bit and plays the
 * learn SFX (0x83). Paired with other field-tick helpers.
 */
void func_800C49CC(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (g_gameState.battleParty[i] == CHAR_RINOA) {
            u8 idx = g_gameState.mainData.party.trickLearning;
            if (g_gameState.mainData.limitBreaks.angeloPoints[idx] != 0) {
                g_gameState.mainData.limitBreaks.angeloPoints[idx]--;
            } else {
                u8 mask = g_gameState.mainData.limitBreaks.angeloCompleted;
                if (!((mask >> idx) & 1)) {
                    g_gameState.mainData.limitBreaks.angeloCompleted = mask | (1 << idx);
                    sndPlaySfx(0x83, 0, 0x80, 0x7F);
                }
            }
            return;
        }
    }
}

extern s32 getPackedField2Bit(s32);
extern s32 fieldRandom(void);
extern void func_800383B8(s32, s32);

/** Scans all 256 entries and processes active ones. */
void func_800C4A74(void) {
    s32 i;
    s32 result;
    for (i = 0; i < 256; i++) {
        result = getPackedField2Bit(i) - 1;
        if ((u32)result < 2) {
            if (fieldRandom() & 1) {
                func_800383B8(i, result);
            }
        }
    }
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4AE4);
