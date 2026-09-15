#include "common.h"
#include "psxsdk/libgte.h"
#include "battle/bc_object8.h"
#include "battle/bc_object14.h"
#include "battle/bc_object9.h"
#include "battle.h"

extern s32 D_800FA4FC;
extern u8 D_800E662C[];
extern u8 D_800F1B90[];
extern u8 D_800FA4F8;
extern s16 D_800FA504[2];
extern s16 D_800FA500;
extern u8 D_800EEC54[];
extern u8 D_800F02F4[];

extern u8 D_800E3F78;
extern s32 D_800E3F7C;
extern s32 D_800E3F80;
extern DRAWENV D_800FA508[];
extern DISPENV D_800FA5C0[];
extern u8 D_800F1B98[];
extern u8 D_800F1BE8[];
extern u8 D_800F606C[];
extern s32 D_24000;
extern s32 D_1C000;
extern s32 D_80106000;
extern s32 func_801A0000;

void func_800D0970(void *a0, DRAWENV *env, s32 a2);
void func_800D09F0(s32 a0, DISPENV *disp, DRAWENV *draw1, DRAWENV *draw2);
void func_80048C50(s32 a);
void func_80048DD4(DRAWENV *env, s32 r, s32 g, s32 b);
void func_80048BB8(s32 a);
void func_800B3330(void);
void func_800C45FC(void);
void func_800BAEE8(void);
void func_80049AFC(u8 *a0, s32 a1);
void func_800491E8(u8 *a0);
void func_80049244(u8 *a0);
void func_8004913C(u8 *a0, s32 a1);
s32 func_800C5B1C(u8 *a0);
s32 func_800C5A94(s32, s32);
void func_800C5338(s32);
void func_800472E4(void);
void func_800472F4(void);
void sndEnableReverb(s32);
void sndDisableReverb(s32);
void func_8009B6B0(void);
s32 func_80042634(s32 mode);
static void func_800C6DB4(s32 level, s32 colour);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C4A64);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C4B78);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C4D70);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C5188);

/**
 * @brief Store a word value to D_800F1B90.
 *
 * @param a0 Value to store.
 */
void func_800C52C4(s32 a0) {
    *(s32 *)D_800F1B90 = a0;
}

/**
 * @brief Set D_800F1B90 to -1 and register callback with mode 0.
 *
 * Stores -1 to D_800F1B90, then calls func_8009B5C4 with a2=0
 * and func_800C52C4 as the callback.
 *
 * @param a0 First argument passed through.
 * @param a1 Second argument passed through.
 */
void func_800C52D0(s32 a0, s32 a1) {
    *(s32 *)D_800F1B90 = -1;
    func_8009B5C4(a0, a1, 0, func_800C52C4);
}

/**
 * @brief Set D_800F1B90 to -1 and register callback with mode 1.
 *
 * Stores -1 to D_800F1B90, then calls func_8009B5C4 with a2=1
 * and func_800C52C4 as the callback.
 *
 * @param a0 First argument passed through.
 * @param a1 Second argument passed through.
 */
void func_800C5304(s32 a0, s32 a1) {
    *(s32 *)D_800F1B90 = -1;
    func_8009B5C4(a0, a1, 1, func_800C52C4);
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C5338);

/**
 * @brief Wait for a sound request to complete, disabling/re-enabling channels.
 *
 * If D_800F1B90 is negative (request pending), disables sound channels 2 and 3
 * via sndEnableReverb (within a critical section), then polls func_8009B6B0 in a
 * loop until D_800F1B90 becomes non-negative. Finally re-enables channels 2
 * and 3 via sndDisableReverb (also within a critical section).
 *
 * @return Current value of D_800F1B90 after completion.
 */
/**
 * @brief Wait for CD load completion and manage audio channel state.
 *
 * If D_800F1B90 is negative, disables audio channels 2 and 3 in a
 * critical section, then polls func_8009B6B0 until D_800F1B90 becomes
 * non-negative. Once ready, re-enables channels 2 and 3. Returns the
 * final value of D_800F1B90.
 *
 * @return Current value of D_800F1B90.
 */
s32 func_800C53F0(void) {
    if (*(s32 *)D_800F1B90 < 0) {
        func_800472E4();
        sndEnableReverb(2);
        sndEnableReverb(3);
        func_800472F4();
        while (*(s32 *)D_800F1B90 < 0) {
            func_8009B6B0();
        }
        func_800472E4();
        sndDisableReverb(2);
        sndDisableReverb(3);
        func_800472F4();
    }
    return *(s32 *)D_800F1B90;
}

/**
 * @brief Check D_800F1B90 sign and D_800EEC5C bit 0x200, conditionally call func_800C5338.
 *
 * If D_800F1B90 is negative: if bit 0x200 of D_800EEC5C is set, returns -1;
 * otherwise calls func_800C5338(1) and returns -1.
 * If D_800F1B90 is non-negative: if bit 0x200 of D_800EEC5C is set, calls
 * func_800C5338(0). Returns the (re-read) value of D_800F1B90.
 *
 * @return -1 if D_800F1B90 < 0, otherwise current value of D_800F1B90.
 */
s32 func_800C5490(void) {
    volatile s32 *pVal = (volatile s32 *)D_800F1B90;
    s32 val = *pVal;
    if (val < 0) {
        if (D_800EEC5C & BATTLE_STATE_UNK200) {
            return -1;
        }
        func_800C5338(1);
        return -1;
    } else {
        if (D_800EEC5C & BATTLE_STATE_UNK200) {
            func_800C5338(0);
        }
        return *pVal;
    }
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C550C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C561C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C5758);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C5848);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C58D8);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C599C);

/**
 * @brief Check if an entity passes multiple flag checks.
 *
 * Checks halfword[0] bit 3, word[8] against mask a1, byte[5],
 * and indirect halfword at ptr[0x74]->0x2C against mask a2.
 *
 * @param a0 Pointer to entity data.
 * @param a1 Bitmask for word at offset 0x8.
 * @param a2 Bitmask for indirect halfword check.
 * @return 0 if blocked, 1 if entity passes all checks.
 */
INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C5A34);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C5A94);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C5B1C);

/**
 * @brief Allocate a handler with func_800C5B1C callback and initialize fields.
 *
 * Calls func_800B853C to allocate a handler with func_800C5B1C as callback.
 * Clears byte at offset 0xD, stores a0 at offset 0x10, returns 8.
 *
 * @param a0 Entity pointer to store in handler.
 * @return Always 8.
 */
s32 func_800C5B88(s32 a0) {
    u8 *handler = (u8 *)func_800B853C(func_800C5B1C);
    handler[0xD] = 0;
    *(s32 *)(handler + 0x10) = a0;
    return 8;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C5BC8);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C5D28);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C5E68);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C5F98);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C60A0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C6198);

/**
 * @brief Update animation frame counter with optional palette reset.
 *
 * If the frame counter (byte at +6) is less than 2, resets the palette
 * via SetFarColor(0xFF, 0xFF, 0xFF), then sets up a palette entry via
 * DpqColor with the counter shifted left by 11, and clears byte +0x2B.
 * Then, if the counter is non-zero, decrements it. If the counter is zero,
 * generates a new random value in the range [1, 15] using func_80023D04.
 *
 * @param a0 Pointer to entity/animation state structure.
 */
void func_800C624C(u8 *a0) {
    if (a0[6] < 2) {
        SetFarColor(0xFF, 0xFF, 0xFF);
        DpqColor((CVECTOR *)((s32)a0 + 0x28), (a0[6] + 1) << 11, (CVECTOR *)((s32)a0 + 0x28));
        a0[0x2B] = 0;
    }
    if (a0[6] != 0) {
        a0[6] = a0[6] - 1;
    } else {
        a0[6] = (u32)func_80023D04() % 15 + 1;
    }
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C62EC);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C63D4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C6480);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C65A8);

/**
 * @brief Initialize palette and display state for battle effect.
 *
 * Calls InitGeom to reset state, clears D_800FA504 (2 halfwords),
 * calls SetGeomOffset with dimensions 0xA0 x 0x6C, stores 0x200 to
 * D_800FA500, calls SetGeomScreen with 0x200, sets D_800FA4F8 to 0x11,
 * and sets bit 2 of D_800EEC5C.
 */
void func_800C66E4(void) {
    s32 flags;

    InitGeom();
    D_800FA504[1] = 0;
    D_800FA504[0] = 0;
    SetGeomOffset(0xA0, 0x6C);
    D_800FA500 = 0x200;
    SetGeomScreen(0x200);
    flags = D_800EEC5C;
    D_800FA4F8 = 0x11;
    D_800EEC5C = flags | 4;
}

/**
 * @brief Store constant 2 to D_800FA4F8.
 */
void func_800C674C(void) {
    D_800FA4F8 = 2;
}

/**
 * @brief Advance the battle frame: swap buffers, update geometry, and refresh OT pointers.
 */
void func_800C675C(void) {
    s32 flip;
    u8 fade;
    s32 arg0;

    if (D_800EEC5C & 4) {
        func_800D0970((u8 *)D_800FA5E8 + 8, &D_800FA508[D_800E3F78 ^ 1], D_800E3F7C);
    }
    if (D_800EEC5C & 8) {
        func_80042634(1);
        func_80048C50(0);
        func_80042634(1);
    }
    if (D_800EEC5C & 4) {
        arg0 = D_800E3F7C;
        flip = D_800E3F78 ^ 1;
        func_800D09F0(arg0, &D_800FA5C0[flip], &D_800FA508[flip], &D_800FA508[D_800E3F78]);
        D_800E3F80 = func_80042634(1);
        fade = D_800FA4F8;
        if (fade != 0) {
            if (fade >= 0x11U) {
                D_800FA4F8 = fade - 0x10;
            } else {
                func_80048BB8(fade & 1);
                D_800FA4F8 = 0;
            }
        }
        func_80048DD4(&D_800FA508[D_800E3F78 ^ 1], 0, 0, 0);
    }
    func_800B3330();
    func_800C45FC();
    if (D_800EEC5C & 8) {
        func_800BAEE8();
        func_80049AFC(D_800F1B98, 1);
        func_800491E8(D_800F1B98);
        func_80048C50(0);
    }
    if (D_800EEC5C & 4) {
        func_80049244(D_800F606C + D_800E3F78 * 0x4488);
        SetGeomScreen(D_800FA500);
        SetGeomOffset(D_800FA504[0] + 0xA0, D_800FA504[1] + 0x6C);
        D_800FA504[1] = 0;
        D_800FA504[0] = 0;
    }
    D_800E3F78 ^= 1;
    func_8004913C(D_800F1BE8 + D_800E3F78 * 0x4488, 0x1122);
    if (D_800EEC5C & 0x80) {
        D_800FA5F0 = D_800E3F78 * (s32)&D_24000 + (s32)&func_801A0000;
        D_800FA4FC = D_800FA5F0 + (s32)&D_24000;
    } else {
        D_800FA5F0 = D_800E3F78 * (s32)&D_1C000 + (s32)&D_80106000;
        D_800FA4FC = D_800FA5F0 + (s32)&D_1C000;
    }
    D_800FA5E8 = (BattleGfx *)(D_800F1BE8 + D_800E3F78 * 0x4488);
}


/**
 * @brief Compute the difference between D_800FA4FC and D_800FA5F0.
 *
 * @return D_800FA4FC - D_800FA5F0.
 */
s32 func_800C6A8C(void) {
    return D_800FA4FC - D_800FA5F0;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C6AA4);

/**
 * @brief Look up a word from the D_800E662C array.
 *
 * @param a0 Array index.
 * @return The word at D_800E662C[a0].
 */
s32 func_800C6B1C(s32 a0) {
    return *(s32 *)(D_800E662C + a0 * 4);
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C6B38);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C6C44);

extern u8 D_800FB408[];
void func_800C6C44(void);

/**
 * @brief Allocate a particle entry and store packed type + parameter.
 *
 * Allocates an entry from D_800FB408 with callback func_800C6C44,
 * packs a0, a1, a2 into a 16-bit value (a0 | a1<<4 | a2<<8),
 * stores it at offset 0xE, clears offset 0xC, and stores a3 at 0x1C.
 *
 * @param a0 Low nibble of packed type.
 * @param a1 Mid nibble of packed type (shifted left 4).
 * @param a2 High byte of packed type (shifted left 8).
 * @param a3 Parameter stored at offset 0x1C.
 */
void func_800C6D3C(s32 a0, s32 a1, s32 a2, s32 a3) {
    u8 *entry = (u8 *)func_800B2A84(D_800FB408, func_800C6C44);
    s32 packed = a0 | (a1 << 4) | (a2 << 8);
    *(u16 *)(entry + 0xC) = 0;
    *(u16 *)(entry + 0xE) = packed;
    *(u16 *)(entry + 0x1C) = a3;
}

/**
 * @brief Set every screen tint to @p colour at @p level.
 *
 * @param level  How far each tint has come up; @ref func_800B9078 steps it.
 * @param colour The three colour bytes, written together as one word.
 */
static void func_800C6DB4(s32 level, s32 colour) {
    s32 i = 0;
    BattleTint *base = D_800EF738;
    u32 *rgb = (u32 *)&base->r;
    BattleTint *tint = base;

    do {
        tint->level = level;
        *rgb = colour;
        rgb += sizeof(BattleTint) / sizeof(u32);
        i++;
        tint++;
    } while (i < 4);
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C6DEC);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C6EAC);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C6F88);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C7134);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object14", func_800C71D8);
