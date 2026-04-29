#include "common.h"
#include "psxsdk/libetc.h"

/** @brief Battle render command block (linked-list node, 0x10 bytes). */
typedef struct CmdBlk_ {
    struct CmdBlk_ *next;   /**< 0x00: next command in queue. */
    u16 unk04;              /**< 0x04: command param (often x coordinate). */
    u16 cmdId;              /**< 0x06: command ID/opcode. */
    u16 unk08;              /**< 0x08: flags or count. */
    u16 unk0A;              /**< 0x0A: padding/unused. */
    void *dataPtr;          /**< 0x0C: command-specific data pointer. */
} CmdBlk;

/** @brief Battle render context (D_800D3C58 target), command queue at 0x4070. */
typedef struct {
    u8 padHeader[0x4070];
    CmdBlk cmdQueueHead;
} BattleRenderCtx;

extern BattleRenderCtx *D_800D3C58;
extern s32 D_8009B54C[];
extern s32 D_800AB9F8[];
extern s32 D_800ABA00[];
extern u8 D_80098000[];
extern u8 D_8009800C[];
extern u8 D_80098018[];
extern s32 D_800ABA08;
extern void func_800275D4(s32 a0);
extern s32 getAnimFrameParam(s32 idx, s32 offset);
extern void func_8009AF64(void *cmd, ...);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_80098238);

/**
 * @brief Emit three debug-string render commands for the build banner.
 *
 * Stages three CmdBlk entries through the scratchpad and dispatches each
 * via func_8009AF64. The commands draw the build date string ("Jul  4
 * 1999"), the build time ("21:03:43"), and a formatted version field
 * (" %x" with D_800ABA08) into the battle render queue at
 * D_800D3C58->cmdQueueHead.
 */
void func_80098688(void) {
    CmdBlk *cmd = (CmdBlk *)getScratchAddr(0);

    cmd->unk04 = 0x1A;
    cmd->cmdId = 0x16;
    cmd->unk08 = 1;
    cmd->next = &D_800D3C58->cmdQueueHead;
    cmd->dataPtr = &D_80098000;
    func_8009AF64(cmd);

    cmd->unk04 = 0x1D;
    cmd->cmdId = 0x17;
    cmd->unk08 = 1;
    cmd->next = &D_800D3C58->cmdQueueHead;
    cmd->dataPtr = &D_8009800C;
    func_8009AF64(cmd);

    cmd->unk04 = 0x1A;
    cmd->cmdId = 0x1A;
    cmd->next = &D_800D3C58->cmdQueueHead;
    cmd->unk08 = 1;
    cmd->dataPtr = &D_80098018;
    func_8009AF64(cmd, D_800ABA08);
}

/**
 * @brief Initialize render state flags.
 *
 * Sets flag at offset 0x18 to 1 and clears flags at 0x19-0x1B.
 *
 * @param a0 Pointer to render state structure.
 */
void func_80098768(u8 *a0) {
    a0[0x18] = 1;
    a0[0x19] = 0;
    a0[0x1A] = 0;
    a0[0x1B] = 0;
}

void func_80098780(void) {
}

void func_80098788(void) {
}

void func_80098790(void) {
}

/**
 * @brief Update animation frame state with timer-based debounce.
 *
 * Calls func_800275D4 then samples getAnimFrameParam for slot @p a0.
 * If the frame param is unchanged from the previous tick, decrements
 * D_800AB9F8 (timer1) or D_800ABA00 (timer2) until both expire, at
 * which point the new state is committed (D_800ABA00 = 0 if bit 0x40
 * set, else 2). On a change, timer1 is reset to 8 and timer2 set per
 * the bit 0x40 flag.
 *
 * @param a0 Animation slot index (0 or 1).
 * @return Last-committed previous-state value (caller may ignore).
 */
s32 func_80098798(s32 a0) {
    s32 prevArr[2];
    s32 currArr[2];
    s32 prevVal;
    s32 v;

    func_800275D4(a0);
    v = getAnimFrameParam(a0, 0);
    currArr[a0] = v;
    prevVal = D_8009B54C[a0];

    if (prevVal == v) {
        if (D_800AB9F8[a0] == 0) {
            if (D_800ABA00[a0] == 0) {
                prevArr[a0] = prevVal;
                if (currArr[a0] & 0x40) {
                    D_800ABA00[a0] = 0;
                } else {
                    D_800ABA00[a0] = 2;
                }
            } else {
                D_800ABA00[a0] = D_800ABA00[a0] - 1;
                prevArr[a0] = 0;
            }
        } else {
            D_800AB9F8[a0] = D_800AB9F8[a0] - 1;
            prevArr[a0] = 0;
        }
    } else {
        D_800AB9F8[a0] = 8;
        if (currArr[a0] & 0x40) {
            D_800ABA00[a0] = 0;
        } else {
            D_800ABA00[a0] = 2;
        }
        prevArr[a0] = currArr[a0];
    }
    D_8009B54C[a0] = currArr[a0];
    return prevArr[a0];
}

/**
 * @brief Copy a null-terminated string into the name table.
 *
 * Copies bytes from src to D_800ABA10[idx * 10] until a null byte
 * is encountered (the null byte is also copied).
 *
 * @param idx Entry index in the name table (stride 10).
 * @param src Pointer to the source string.
 */
INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_800988EC);

/**
 * @brief Set a name table entry from one of two default strings.
 *
 * If flag is zero, copies the string at D_8009801C into the name
 * table at index idx; otherwise copies D_80098020.
 *
 * @param idx Entry index in the name table.
 * @param flag Selects which default string to use.
 */
void func_80098920(s32 idx, s32 flag) {
    extern u8 D_8009801C[];
    extern u8 D_80098020[];
    if (flag == 0) {
        func_800988EC(idx, D_8009801C);
    } else {
        func_800988EC(idx, D_80098020);
    }
}

/**
 * @brief Decode input flags into battle render configuration bytes.
 *
 * Reads bits from D_8007809A, D_80077E5F, and D_80077E92 and writes
 * corresponding flag bytes to the D_800D3C30 region. Then iterates
 * through 0x1D name table slots calling func_80098920 with values
 * from D_800D3C30.
 */
void func_80098958(void) {
    extern u8 D_8007809A;
    extern u8 D_80077E5F;
    extern u8 D_80077E92;
    extern u8 D_800D3C35;
    extern u8 D_800D3C36;
    extern u8 D_800D3C38;
    extern u8 D_800D3C39;
    extern u8 D_800D3C3A;
    extern u8 D_800D3C3B;
    extern u8 D_800D3C3C;
    extern u8 D_800D3C3D;
    extern u8 D_800D3C3E;
    extern u8 D_800D3C3F;
    extern u8 D_800D3C44;
    extern u8 D_800D3C45;
    extern u8 D_800D3C46;
    extern u8 D_800D3C47;
    extern u8 D_800D3C48;
    extern u8 D_800D3C49;
    extern u8 D_800D3C30[];
    s32 i;

    if (D_8007809A & 0x10) {
        D_800D3C35 = 1;
    } else {
        D_800D3C35 = 0;
    }

    if (D_8007809A & 0x2) {
        D_800D3C36 = 1;
    } else {
        D_800D3C36 = 0;
    }

    if (D_8007809A & 0x20) {
        D_800D3C38 = 1;
    } else {
        D_800D3C38 = 0;
    }

    if (D_80077E5F & 0x1) {
        D_800D3C39 = 1;
    } else {
        D_800D3C39 = 0;
    }

    if (D_80077E5F & 0x2) {
        D_800D3C3A = 1;
    } else {
        D_800D3C3A = 0;
    }

    if (D_80077E5F & 0x4) {
        D_800D3C3B = 1;
    } else {
        D_800D3C3B = 0;
    }

    if (D_80077E5F & 0x8) {
        D_800D3C3C = 1;
    } else {
        D_800D3C3C = 0;
    }

    if (D_80077E5F & 0x10) {
        D_800D3C3D = 1;
    } else {
        D_800D3C3D = 0;
    }

    if (D_80077E5F & 0x20) {
        D_800D3C3E = 1;
    } else {
        D_800D3C3E = 0;
    }

    if (D_80077E5F & 0x40) {
        D_800D3C3F = 1;
    } else {
        D_800D3C3F = 0;
    }

    if (D_80077E92 & 0x2) {
        D_800D3C44 = 1;
    } else {
        D_800D3C44 = 0;
    }

    if (D_80077E92 & 0x4) {
        D_800D3C45 = 1;
    } else {
        D_800D3C45 = 0;
    }

    if (D_80077E92 & 0x8) {
        D_800D3C46 = 1;
    } else {
        D_800D3C46 = 0;
    }

    if (D_80077E92 & 0x20) {
        D_800D3C47 = 1;
    } else {
        D_800D3C47 = 0;
    }

    if (D_80077E92 & 0x40) {
        D_800D3C48 = 1;
    } else {
        D_800D3C48 = 0;
    }

    if (D_80077E92 & 0x80) {
        D_800D3C49 = 1;
    } else {
        D_800D3C49 = 0;
    }

    for (i = 0; i < 0x1D; i++) {
        func_80098920(i, D_800D3C30[i]);
    }
}

/**
 * @brief Set battle render mode to active.
 *
 * Sets D_800ABA0C to 1 and D_800D3C70 to -0x18.
 */
void func_80098C68(void) {
    extern u8 D_800ABA0C;
    extern s16 D_800D3C70;
    D_800ABA0C = 1;
    D_800D3C70 = -0x18;
}

/**
 * @brief Clear battle render mode.
 *
 * Clears D_800ABA0C and sets D_800D3C70 to 2.
 */
void func_80098C84(void) {
    extern u8 D_800ABA0C;
    extern s16 D_800D3C70;
    D_800ABA0C = 0;
    D_800D3C70 = 2;
}

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_80098C9C);

/**
 * @brief Initialize battle entity rendering tables.
 *
 * Sets GPU attribute flags on 8 entity slots, configures entity
 * visibility flags at offsets 0x61/0xA5/0xE9, and fills the
 * animation tile table at offset 0xB44 with incrementing indices.
 */
INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009A368);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009A40C);

/** @brief Call func_8003646C with display mode 0x16 and layer 7. */
void func_8009A60C(void) {
    func_8003646C(0x16, 7);
}

/**
 * @brief Compute checksum of D_80078E00 buffer.
 *
 * Sums 0x9E08 bytes from D_80078E00 and returns the total.
 *
 * @return Byte sum of the buffer.
 */
INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009A630);

void func_8009A664(void) {
}

/**
 * @brief Process render object and dispatch collision checks.
 *
 * Calls func_8004B8F4 to initialize, then func_8004B904 to populate
 * a local buffer. Dispatches func_80048EFC with the third/fourth
 * words, and if bit 3 of the first word is set, also dispatches
 * with the second/third words.
 *
 * @param a0 Render object pointer.
 */
void func_8009A66C(u8 *a0) {
    s32 buf[6];
    func_8004B8F4(a0);
    func_8004B904(&buf);
    func_80048EFC(buf[3], buf[4]);
    if (((u32)buf[0] >> 3) & 1) {
        func_80048EFC(buf[1], buf[2]);
    }
}

/** @brief Call func_8009A66C with D_800A37D4 then D_8009B5B4. */
void func_8009A6CC(void) {
    extern u8 D_800A37D4[];
    extern u8 D_8009B5B4[];
    func_8009A66C(D_800A37D4);
    func_8009A66C(D_8009B5B4);
}

/**
 * @brief Initialize render object with two display functions.
 *
 * Calls func_8004D604 with mode 0, then func_8004D634 with mode 1,
 * both using the same object pointer.
 *
 * @param a0 Render object pointer.
 */
void func_8009A6FC(s32 a0) {
    func_8004D604(a0, 0);
    func_8004D634(a0, 1);
}

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009A734);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009A8B4);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009AA18);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009AC18);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009AE1C);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009AF64);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009B170);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009B394);
