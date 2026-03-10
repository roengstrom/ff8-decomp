#include "common.h"

extern u8 D_800ED148[];
extern u8 D_800EE490[];
extern u8 D_80078E00[];
extern u8 D_80082C10[];
extern u8 D_80077EBC[];
extern u8 D_800EE9E8[];
extern u8 D_800EEBE8[];
s32 func_800B0204(u8 *, s32, s32, s32);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF358);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF4BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF5E0);

/**
 * @brief Initialize 32 entity entries by calling func_800AF5E0.
 *
 * Iterates over 32 entries at stride 5 in D_800EE9E8, passing the
 * unsigned byte at offset 0 and signed byte at offset 1 along with
 * D_80077EBC to func_800AF5E0.
 */
void func_800AF654(void) {
    u8 *constPtr = D_80077EBC;
    s32 i = 0;
    u8 *base = D_800EE9E8;
    do {
        func_800AF5E0(base[0], *(s8 *)(base + 1), constPtr);
        base += 5;
        i++;
    } while (i < 0x20);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF6BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF740);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF7C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF8A4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF918);

/**
 * @brief Get a byte field from an entity's nested pointer chain.
 *
 * Looks up D_800ED148[a0] (stride 208), follows the pointer at +0x10,
 * dereferences it, then returns the byte at offset +0x14F.
 *
 * @param a0 Entity index.
 * @return Byte value at the end of the pointer chain.
 */
s32 func_800AF988(s32 a0) {
    s32 base = (s32)D_800ED148;
    s32 entry = base + a0 * 208;
    s32 ptr = *(s32 *)(entry + 0x10);
    ptr = *(s32 *)ptr;
    return *(u8 *)(ptr + 0x14F);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF9C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF9E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AFA64);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AFB5C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AFD0C);

/**
 * @brief Look up entity field and call func_80020FBC.
 *
 * Computes D_80078E00 + a0 * 20 to find the entity entry, reads
 * the halfword at offset 0x3EE0 and the word at D_80078E00 + 0xA4,
 * then calls func_80020FBC with them.
 *
 * @param a0 Entity index (stride 20).
 */
void func_800AFF30(s32 a0) {
    s32 base = (s32)D_80078E00;
    u8 *entry = (u8 *)(base + a0 * 20);
    func_80020FBC(*(u16 *)(entry + 0x3EE0), *(s32 *)(base + 0xA4));
}

/**
 * @brief Look up entity field and call func_80020FBC (stride 132).
 *
 * Subtracts 0x40 from a0, computes D_80078E00 + adjusted * 132,
 * reads halfword at offset 0xF78 and word at D_80078E00 + 0x88.
 *
 * @param a0 Entity index (offset by 0x40, stride 132).
 */
void func_800AFF70(s32 a0) {
    s32 base = (s32)D_80078E00;
    u8 *entry;
    a0 -= 0x40;
    entry = (u8 *)(base + a0 * 132);
    func_80020FBC(*(u16 *)(entry + 0xF78), *(s32 *)(base + 0x88));
}

/**
 * @brief Look up entity field and call func_80020FBC (alternate offsets).
 *
 * Same pattern as func_800AFF30 but reads halfword at offset 0x17B8
 * and word at D_80078E00 + 0x8C.
 *
 * @param a0 Entity index (stride 20).
 */
void func_800AFFB4(s32 a0) {
    s32 base = (s32)D_80078E00;
    u8 *entry = (u8 *)(base + a0 * 20);
    func_80020FBC(*(u16 *)(entry + 0x17B8), *(s32 *)(base + 0x8C));
}

/**
 * @brief Call func_80020F84 with argument 0xA.
 */
void func_800AFFF4(void) {
    func_80020F84(0xA);
}

/**
 * @brief Call func_80020F84 with argument 0xC.
 */
void func_800B0014(void) {
    func_80020F84(0xC);
}

/**
 * @brief Call func_80020F84 with argument 0xD.
 */
void func_800B0034(void) {
    func_80020F84(0xD);
}

/**
 * @brief Call func_80020F84 with argument 0xE.
 */
void func_800B0054(void) {
    func_80020F84(0xE);
}

/**
 * @brief Read a pointer from entity data and dereference it.
 *
 * @param idx Entity index (stride 0xD0).
 * @return First word of the data pointed to by entity[0x10].
 */
s32 func_800B0074(s32 idx) {
    u8 *base = D_800ED148;
    u8 *entity;
    asm("");
    entity = base + idx * 0xD0;
    return **(s32 **)(entity + 0x10);
}

/**
 * @brief Call func_80020F84 with argument 0xF.
 */
void func_800B00A8(void) {
    func_80020F84(0xF);
}

/**
 * @brief Call func_80020F84 with argument 0x10.
 */
void func_800B00C8(void) {
    func_80020F84(0x10);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B00E8);

/**
 * @brief Copy a null-terminated string from src to dst.
 *
 * @param dst Destination buffer.
 * @param src Source string.
 */
void func_800B01E8(u8 *dst, u8 *src) {
    u8 ch;
    do {
        ch = *src++;
        *dst++ = ch;
    } while (ch != 0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0204);

/**
 * @brief Build a string in D_800EEBE8 from two parts using func_800B0204.
 *
 * Writes the first part with a1 as length byte, then appends the
 * second part starting at the returned offset.
 *
 * @param a0 First part data.
 * @param a1 Length/type byte for first part (masked to 8 bits).
 * @param a2 Second part data.
 * @return Pointer to D_800EEBE8 buffer.
 */
u8 *func_800B0248(s32 a0, s32 a1, s32 a2) {
    u8 *buf = D_800EEBE8;
    s32 offset = func_800B0204(buf, a0, 0, (u8)a1);
    func_800B0204(buf + offset, a2, offset, 0);
    return buf;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B02AC);

/**
 * @brief Copy a string to D_800EE490 and return the buffer pointer.
 *
 * @param src Source string to copy.
 * @return Pointer to D_800EE490.
 */
u8 *func_800B0328(u8 *src) {
    u8 *dst = D_800EE490;
    func_800B01E8(dst, src);
    return dst;
}

/**
 * @brief Look up a halfword from entity table and call func_80020FBC.
 *
 * @param a0 Entity index (stride 8 in table).
 */
void func_800B0360(s32 a0) {
    u8 *base = D_80078E00;
    u16 val = *(u16 *)(base + a0 * 8 + 0x4A5E);
    s32 arg = *(s32 *)(base + 0xD4);
    func_80020FBC(val, arg);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0398);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0414);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B04A0);

/**
 * @brief Find the bit position of the lowest set bit.
 *
 * @param a0 Value to scan (should have exactly one bit set).
 * @return Bit position (0-31), or 32 if no single bit found.
 */
s32 func_800B054C(s32 a0) {
    s32 i = 0;
    do {
        if (a0 == 1) {
            return i;
        }
        i++;
        a0 = (u32)a0 >> 1;
    } while (i < 0x20);
    return i;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0574);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0600);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0668);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B06DC);

/**
 * @brief Set up extended parameters and call two processing functions.
 *
 * Saves the 16-bit truncation of a3, calls func_800A30F8 with 7 args
 * (a0, a1, a2 passed through, a3 zeroed, plus a0, truncated a3, and 0
 * on the stack), then calls func_800B06DC with the truncated a3 value.
 *
 * @param a0 First parameter (also passed as 5th arg).
 * @param a1 Second parameter passed through.
 * @param a2 Third parameter passed through.
 * @param a3 Fourth parameter (16-bit truncated, passed as 6th arg).
 */
void func_800B0754(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 val = (u16)a3;
    func_800A30F8(a0, a1, a2, 0, a0, val, 0);
    func_800B06DC(val);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0794);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0808);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B08AC);

/**
 * @brief Call func_800A59AC with a1=6 and a2=0.
 *
 * @param a0 First argument passed through.
 */
void func_800B095C(s32 a0) {
    func_800A59AC(a0, 6, 0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0980);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B09F0);

/**
 * @brief Process entities that are not busy or disabled.
 *
 * Iterates over 7 entities at stride 0xD0 from D_800ED148.
 * For each entity, checks if bits 0 and 2 of the halfword at
 * offset 0x90 are clear. If so, calls func_800B09F0 with the index.
 */
void func_800B0C08(void) {
    s32 i = 0;
    u8 *base = D_800ED148;
    do {
        if ((*(u16 *)(base + 0x90) & 5) == 0) {
            func_800B09F0(i);
        }
        i++;
        base += 0xD0;
    } while (i < 7);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0C68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0CC4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0D8C);

/**
 * @brief Dispatch call based on D_80082C10 flag bit 1.
 *
 * If bit 1 of D_80082C10 is set, passes 0xFF to func_800B0CC4.
 * Otherwise calls func_800B0D8C with a0 and mode 2, then passes
 * the result to func_800B0CC4.
 *
 * @param a0 Entity parameter for func_800B0D8C and func_800B0CC4.
 */
void func_800B0DDC(s32 a0) {
    s32 val;
    if (*(u8 *)D_80082C10 & 2) {
        val = 0xFF;
    } else {
        val = func_800B0D8C(a0, 2);
    }
    func_800B0CC4(a0, val);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0E30);

/**
 * @brief Dispatch call based on D_80082C10 flag bit 0.
 *
 * If bit 0 of D_80082C10 is set, passes 0xFF to func_800B0E30.
 * Otherwise calls func_800B0D8C with a0 and mode 4, then passes
 * the result to func_800B0E30.
 *
 * @param a0 Entity parameter for func_800B0D8C.
 */
void func_800B0F3C(s32 a0) {
    s32 val;
    if (*(u8 *)D_80082C10 & 1) {
        val = 0xFF;
    } else {
        val = func_800B0D8C(a0, 4);
    }
    func_800B0E30(val);
}

/**
 * @brief Convert ability flag bits to GF compatibility bitmask.
 *
 * Bit 0 of the input maps to bit 14 (0x4000) of the result,
 * and bit 1 maps to bit 13 (0x2000).
 *
 * @param arg0 Ability flags.
 * @return Bitmask with bits 14 and/or 13 set.
 */
s32 func_800B0F7C(s32 arg0) {
    s32 temp_v1;
    int new_var;
    s32 var_v0;

    temp_v1 = (arg0 & 1) << 0xE;
    new_var = arg0 & 2;
    var_v0 = temp_v1;
    if (new_var) {
        var_v0 = temp_v1 | 0x2000;
        var_v0 = temp_v1;
        var_v0 = var_v0 | 0x2000;
    }
    return var_v0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0F9C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B1050);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B1104);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B115C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B13A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B1438);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B1564);
