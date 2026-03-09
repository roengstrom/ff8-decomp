#include "common.h"

extern u8 D_800ED148[];
extern u8 D_800EE490[];
extern u8 D_80078E00[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF358);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF4BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF5E0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF654);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF6BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF740);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF7C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF8A4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF918);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF988);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF9C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF9E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AFA64);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AFB5C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AFD0C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AFF30);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AFF70);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AFFB4);

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

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0248);

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

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0754);

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

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0C08);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0C68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0CC4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0D8C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0DDC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0E30);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0F3C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0F7C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0F9C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B1050);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B1104);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B115C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B13A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B1438);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B1564);
