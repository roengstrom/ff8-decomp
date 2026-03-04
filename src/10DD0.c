#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

/** @brief Empty stub at the end of the sound engine region.
 *  Followed by ~4KB of zero padding before the game code resumes.
 */
INCLUDE_ASM("asm/nonmatchings/10DD0", func_8001F5C8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800205D0);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020608);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020644);


/** @brief Sets bit 0x1 in byte at g_gameState[a0 * 68 + 0x61].
 *  @param a0 Table index (stride 68 bytes).
 */
void func_80020670(s32 a0) {
    extern u8 g_gameState[];
    s32 base = (s32)g_gameState;
    *(u8 *)(base + a0 * 68 + 0x61) |= 1;
}


/** @brief Returns a pointer to global D_800773B4. */
u8 *func_8002069C(void) {
    extern u8 D_800773B4;
    return &D_800773B4;
}


/** @brief Returns a pointer to global D_800773A8. */
u8 *func_800206A8(void) {
    extern u8 D_800773A8;
    return &D_800773A8;
}


/** @brief Resolves a halfword from D_80078E00 table (stride 20, offset 0x4A6C) via func_80020FBC. */
s32 func_800206B4(s32 a0) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 20 + 0x4A6C), *(s32 *)(base + 0xD8));
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800206F4);


/** @brief Resolves a halfword from D_80078E00 table (stride 32, offset 0x48B8) via func_80020FBC. */
s32 func_80020740(s32 a0) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 32 + 0x48B8), *(s32 *)(base + 0xD0));
}


/** @brief Resolves a halfword from D_80078E00 table (stride 32, offset 0x48BA) via func_80020FBC. */
s32 func_80020778(s32 a0) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 32 + 0x48BA), *(s32 *)(base + 0xD0));
}


/** @brief Resolves a halfword from D_80078E00 table (stride 24, offset 0x47F8) via func_80020FBC. */
s32 func_800207B0(s32 a0) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 24 + 0x47F8), *(s32 *)(base + 0xCC));
}


/** @brief Resolves a halfword from D_80078E00 table (stride 24, offset 0x47FA) via func_80020FBC. */
s32 func_800207F0(s32 a0) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 24 + 0x47FA), *(s32 *)(base + 0xCC));
}


/** @brief Resolves a halfword from D_80078E00 table (stride 24, offset 0x3744) via func_80020FBC. */
s32 func_80020830(s32 a0) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 24 + 0x3744), *(s32 *)(base + 0x94));
}


/** @brief Resolves a halfword from D_80078E00 table (stride 24, offset 0x3746) via func_80020FBC. */
s32 func_80020870(s32 a0) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 24 + 0x3746), *(s32 *)(base + 0x94));
}


/** @brief Resolves a halfword from D_80078E00 table (stride 16, offset 0x44F8) via func_80020FBC. */
s32 func_800208B0(s32 a0) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 16 + 0x44F8), *(s32 *)(base + 0xC8));
}


/** @brief Resolves a halfword from D_80078E00 table (stride 16, offset 0x44FA) via func_80020FBC. */
s32 func_800208E8(s32 a0) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 16 + 0x44FA), *(s32 *)(base + 0xC8));
}


/** @brief Resolves a halfword from D_80078E00 table (stride 24, offset 0x4480) via func_80020FBC. */
s32 func_80020920(s32 a0) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 24 + 0x4480), *(s32 *)(base + 0xC4));
}


/** @brief Resolves a halfword from D_80078E00 table (stride 24, offset 0x4482) via func_80020FBC. */
s32 func_80020960(s32 a0) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 24 + 0x4482), *(s32 *)(base + 0xC4));
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800209A0);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020AD4);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020C08);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020C6C);


/**
 * @brief Look up a parameter from D_80078E00 tables and pass it to func_80020FBC.
 * @param a0 Index into the lookup table; values >= 0x21 use a secondary table at offset 0x3C48.
 * @return Result of func_80020FBC with the looked-up parameter and a base pointer.
 * @note Purpose uncertain -- appears to resolve a stat or attribute using two different table layouts.
 */
s32 func_80020CE0(s32 a0) {
    extern u8 D_80078E00[];
    u16 param;
    s32 arg2;

    if (a0 >= 0x21) {
        s32 base = (s32)D_80078E00;
        s32 idx = a0 - 0x21;
        param = *(u16 *)(base + idx * 4 + 0x3C48);
        arg2 = *(s32 *)(base + 0xA0);
    } else {
        s32 base = (s32)D_80078E00;
        param = *(u16 *)(base + a0 * 24 + 0x3930);
        arg2 = *(s32 *)(base + 0x9C);
    }
    return func_80020FBC(param, arg2);
}


/**
 * @brief Look up a second parameter from D_80078E00 tables and pass it to func_80020FBC.
 * @param a0 Index into the lookup table; values >= 0x21 use a secondary table at offset 0x3C4A.
 * @return Result of func_80020FBC with the looked-up parameter and a base pointer.
 * @note Purpose uncertain -- appears to resolve an alternate stat/attribute, offset +2 from func_80020CE0 tables.
 */
s32 func_80020D4C(s32 a0) {
    extern u8 D_80078E00[];
    u16 param;
    s32 arg2;

    if (a0 >= 0x21) {
        s32 base = (s32)D_80078E00;
        s32 idx = a0 - 0x21;
        param = *(u16 *)(base + idx * 4 + 0x3C4A);
        arg2 = *(s32 *)(base + 0xA0);
    } else {
        s32 base = (s32)D_80078E00;
        param = *(u16 *)(base + a0 * 24 + 0x3932);
        arg2 = *(s32 *)(base + 0x9C);
    }
    return func_80020FBC(param, arg2);
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020DB8);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020E4C);


/** @brief Resolves a halfword from D_80078E00 table (stride 12, offset 0x35B8) via func_80020FBC. */
s32 func_80020EB4(s32 a0) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 12 + 0x35B8), *(s32 *)(base + 0x90));
}


/** @brief Resolves a halfword from D_80078E00 table (stride 8, offset 0xE4) via func_80020FBC. */
s32 func_80020EF4(s32 a0) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 8 + 0xE4), *(s32 *)(base + 0x80));
}


/** @brief Resolves a halfword from D_80078E00 table (stride 8, offset 0xE6) via func_80020FBC. */
s32 func_80020F2C(s32 a0) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 8 + 0xE6), *(s32 *)(base + 0x80));
}


/** @brief Wrapper that calls func_80020F84 with argument 3. */
s32 func_80020F64(void) {
    return func_80020F84(3);
}


/**
 * @brief Look up a u16 value from D_80078E00 table at offset 0x4D08 and pass it to func_80020FBC.
 * @param a0 Index into the lookup table (stride 2 bytes).
 * @return Result of func_80020FBC with the table entry and a pointer from offset 0xE0.
 */
s32 func_80020F84(s32 a0) {
    extern u8 D_80078E00[];
    u8 *base = D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 2 + 0x4D08), *(s32 *)(base + 0xE0));
}


/**
 * @brief Resolve a data pointer for a given entity index.
 * @param a0 Entity index (0xFFFF = no entity, returns default pointer).
 * @param a1 Base offset into the data region.
 * @return Pointer into D_80078E00 at offset a1+a0, or D_80052898 if a0 is 0xFFFF.
 */
u8 *func_80020FBC(u16 a0, s32 a1) {
    extern u8 D_80078E00[];
    extern u8 D_80052898[];
    u8 *result;
    if (a0 != 0xFFFF) {
        result = a1 + (a0 + D_80078E00);
    } else {
        result = D_80052898;
    }
    return result;
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020FEC);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021034);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021108);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800211B4);


/**
 * @brief Check if a character's ability list contains a specific ability ID.
 * @param a0 Character index used to look up a slot ID in g_gameState at offset 0xAF4.
 * @param a1 Ability ID to search for; returns 0 immediately if a1 is 0.
 * @return 1 if the ability is found in the character's 20-entry ability list, 0 otherwise.
 */
s32 func_80021290(a0, a1)

s32 a0;
s32 a1;
{
    extern u8 g_gameState[];
    s32 base;
    u8 slot_id;
    s32 i;
    s32 ptr;

    if (a1 == 0) return 0;

    base = (s32)g_gameState;
    slot_id = *(u8 *)(a0 + base + 0xAF4);
    i = 0;
    ptr = base + slot_id * 152;
    while (i < 20) {
        if (*(u8 *)(ptr + i + 0x4EC) == a1) {
            return 1;
        }
        i++;
    }
    return 0;
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021300);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021358);


/**
 * @brief Add a value to a character's stat at offset 0x02, clamping the result to 9999.
 * @param a0 Character index used to resolve a slot ID via g_gameState.
 * @param a1 Amount to add to the stat.
 * @note The stat at ptr+2 (likely HP or experience) is read as u16, added to a1, then clamped by func_800231C8.
 */
void func_800214E0(s32 a0, s32 a1) {
    extern u8 g_gameState[];
    u8 *base = g_gameState + a0;
    u8 idx = base[0xAF4];
    u8 *ptr = g_gameState + 0x490 + idx * 152;
    *(s16 *)(ptr + 2) = func_800231C8(*(u16 *)(ptr + 2) + a1);
}


