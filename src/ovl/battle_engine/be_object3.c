#include "common.h"

/**
 * @brief Script-action entry in the D_801D3EC0 2x5 table.
 *
 * Initialized in func_800A00EC. Used by func_8009FC90's state-2 sweep
 * to mark queued actions complete and by func_8009EF68 to scan for
 * pending actions.
 */
typedef struct {
    /* 0x00 */ u8 marker;       /**< Sentinel; set to 0xFF on init. */
    /* 0x01 */ u8 status;       /**< Action status (set to 3 to mark "complete"). */
    /* 0x02 */ u8 field02;      /**< Cleared when action marked complete. */
    /* 0x03 */ u8 row;          /**< Row index (cached on init). */
    /* 0x04 */ u8 col;          /**< Column index (cached on init). */
    /* 0x05 */ u8 pad05;
    /* 0x06 */ u16 field06;
    /* 0x08 */ s16 actionId;    /**< Pending action ID; non-zero means action queued. */
    /* 0x0A */ u16 field0A;
    /* 0x0C */ u8 pad0C[10];
} ScriptEntry; /* 0x16 = 22 bytes */

/**
 * @brief Callback context for state-machine handlers (e.g. func_8009FC90).
 *
 * Allocated via func_80098C44 / func_80098CC0 with state byte at +0x10.
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x0C];
    /* 0x0C */ s32 cachedResult;
    /* 0x10 */ u8 state;
    /* 0x11 */ u8 subState;
    /* 0x12 */ u8 counter;
    /* 0x13 */ u8 pad13;
} ScriptCtx;

extern u8 D_801D3C58[];
extern u8 D_801D3C68[];
extern u8 D_801D3C78[];
extern ScriptEntry D_801D3EC0[2][5];
extern u8 D_801D4308[];
extern s32 D_801D3D08;
extern s32 D_80182E4C[];
extern u8 D_801C2DCA;
extern u8 D_801C2DD0[];
extern u8 D_8012E66C[];
extern u8 D_80158680[];
extern u8 D_801A2CE6;
extern s32 D_801A2C54;

/** @brief Call func_80098C44 with D_801D3C58 and a0. */
void func_8009E248(s32 a0) {
    func_80098C44(D_801D3C58, a0);
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009E270);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009E464);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009E640);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009E904);

/**
 * @brief Allocate a node in D_801D3C68 list with a callback from D_80182E4C.
 *
 * Looks up a callback pointer from the D_80182E4C table at index @p a0,
 * allocates a node via func_80098CC0, and initializes several fields.
 *
 * @param a0 Index into the D_80182E4C callback table.
 */
void func_8009EB30(s32 a0) {
    u8 *node = (u8 *)func_80098CC0(D_801D3C68, D_80182E4C[a0]);
    if (node != 0) {
        node[0xE] = a0;
        *(s16 *)(node + 0x20) = 0;
        node[0x22] = 0;
        node[0xC] = 0;
        node[0xD] = 0;
    }
}

/**
 * @brief Store a byte value at offset 0x22 of a structure.
 *
 * @param a0 Pointer to structure base.
 * @param a1 Byte value to store at offset 0x22.
 */
void func_8009EB90(u8 *a0, s32 a1) {
    a0[0x22] = a1;
}

/**
 * @brief Initialize the D_801D3C68 list with node pool D_801D3C78.
 *
 * Sets up a linked list with node size 0x24 and capacity 0x4.
 */
void func_8009EB98(void) {
    func_80098BC0(D_801D3C68, D_801D3C78, 0x24, 0x4);
}

/**
 * @brief Call func_80098D28 with D_801D3C68 and store result in D_801D3D08.
 */
void func_8009EBCC(void) {
    D_801D3D08 = func_80098D28(D_801D3C68);
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009EBF4);

/**
 * Checks if any active slot has a pending action.
 *
 * Searches through a 2D table (2 rows of 5 entries, each 22 bytes apart,
 * rows 110 bytes apart) for an entry where byte 0 is not 0xFF and byte 1
 * is non-zero.
 *
 * @return 1 if a pending action was found, 0 otherwise.
 */
s32 func_8009EF68(void) {
    s32 row = 0;
    u8 *base = (u8 *)D_801D3EC0;
    s32 marker = 0xFF;
    s32 rowOff = row;
    s32 col;
    s32 colOff;
    do {
        col = 0;
        colOff = rowOff;
        do {
            u8 *entry = (u8 *)(colOff + (s32)base);
            if (entry[0] != marker) {
                if (entry[1] != 0) {
                    return 1;
                }
            }
            col++;
            colOff += 0x16;
        } while (col < 5);
        row++;
        rowOff += 0x6E;
    } while (row < 2);
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009EFD4);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009F17C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009F5F0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009F844);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009F908);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009FAF8);

/**
 * @brief Add a rendering command entry based on the alternate screen index.
 *
 * Reads D_801C2DCA, XORs with 1 to get the alternate index, computes
 * an offset of index * 92 into D_801C2DD0, and calls func_80098A1C
 * with the resulting pointer and D_8012E66C.
 *
 * @return Always 0.
 */
s32 func_8009FC40(void) {
    s32 idx = D_801C2DCA ^ 1;
    func_80098A1C(D_801C2DD0 + idx * 92, D_8012E66C);
    return 0;
}

extern void func_800A030C(s32 a0);
extern s32 func_8009FAF8(s32 a0);

/**
 * @brief Battle-script callback: 5-state machine driving an intro/setup sequence.
 *
 * Registered via func_800A00EC. Each invocation advances the state machine
 * one tick; returns 0 while running, returns from any case.
 *
 * State 0: warmup. Calls func_800A030C(0xF) once, ticks 15 frames.
 * State 1: setup. Calls func_8009FAF8(counter) per counter, polls
 *          func_80098D28 until ready; tries counter 0..1, then branches
 *          to state 2 (if D_801A2C54 & 1) or state 4.
 * State 2: clear sweep. Every 5 ticks, marks queued actions in
 *          D_801D3EC0[row][col] complete for column = 4 down to 0;
 *          transitions to state 3 once column 0 is processed.
 * State 3: wait. Calls func_8009EF68 once, idles 0x1E frames.
 * State 4: done. Sets D_801A2CE6 = 3 and exits.
 *
 * @param ctx Callback context (state at +0x10, subState at +0x11).
 * @return 0 while progressing, 0 on completion.
 */
s32 func_8009FC90(ScriptCtx *ctx) {
    while (1) {
        switch (ctx->state) {
        case 0:
            if (ctx->subState == 0) {
                func_800A030C(0xF);
            }
            ctx->subState++;
            if (ctx->subState < 0xF) {
                return 0;
            }
            ctx->counter = 0;
            ctx->state = 1;
            ctx->subState = 0;
            break;

        case 1:
            if (ctx->subState == 0) {
                ctx->cachedResult = func_8009FAF8(ctx->counter);
                ctx->subState++;
            }
            if (func_80098D28(ctx->cachedResult) != 0) {
                return 0;
            }
            ctx->counter++;
            if (ctx->counter < 2) {
                ctx->state = 1;
                ctx->subState = 0;
                break;
            }
            if (D_801A2C54 & 1) {
                ctx->state = 2;
                ctx->subState = 0;
                break;
            }
            ctx->state = 4;
            ctx->subState = 0;
            break;

        case 2: {
            s32 row;
            s32 col;
            if ((u8)(ctx->subState % 5) == 0) {
                col = 4 - (u8)(ctx->subState / 5);
                for (row = 0; row < 2; row++) {
                    if (D_801D3EC0[row][col].actionId != 0) {
                        D_801D3EC0[row][col].status = 3;
                        D_801D3EC0[row][col].field02 = 0;
                    }
                }
                if (col == 0) {
                    ctx->state = 3;
                    ctx->subState = 0;
                }
            }
            ctx->subState++;
            return 0;
        }

        case 3:
            if (ctx->subState == 0) {
                if (func_8009EF68() != 0) {
                    return 0;
                }
            }
            ctx->subState++;
            if (ctx->subState < 0x1E) {
                return 0;
            }
            ctx->state = 4;
            ctx->subState = 0;
            break;

        case 4:
            D_801A2CE6 = 3;
            return 0;
        }
    }
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009FED0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A00EC);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A01DC);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A030C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A0370);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A03DC);

/**
 * @brief Check if any active battle object has a pending action.
 *
 * Iterates through 10 entries in D_801D4308 (stride 0x20). For each entry
 * where bit 0 of the word at offset +4 is set and the byte at offset +8
 * is non-zero, returns 1. Returns 0 if no such entry is found.
 *
 * @return 1 if any active object has a pending action, 0 otherwise.
 */
s32 func_800A0A88(void) {
    s32 i = 0;
    u8 *entry = D_801D4308;

    top:
    if ((*(s32 *)(entry + 4) & 1) && *(u8 *)(entry + 8) != 0) {
        return 1;
    }
    i++;
    entry += 0x20;
    if (i < 10) goto top;

    return 0;
}

/**
 * @brief Add a rendering command for the alternate screen buffer.
 *
 * Reads D_801C2DCA, XORs with 1 to get the alternate index, computes
 * an offset of index * 92 into D_801C2DD0, and calls func_80098A1C
 * with the resulting pointer and D_80158680.
 *
 * @return Always 0.
 */
s32 func_800A0AD4(void) {
    s32 idx = D_801C2DCA ^ 1;
    func_80098A1C(D_801C2DD0 + idx * 92, D_80158680);
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A0B24);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A0F0C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A1080);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A1260);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A1374);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A15C8);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A18D0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A1BC4);
