#include "common.h"

typedef struct {
    u8 pad[0x12E0];
    s16 unk12E0;
    u8 pad2[0x2D];
    s8 unk130F;
} D_800ED148_Type;

extern D_800ED148_Type D_800ED148;
extern u8 D_80078E00[];
extern u8 D_800EE424[];
extern u8 D_800EE43C[];
extern u8 D_800EE462[];
s32 func_800A4798(s32, s32);
void func_8009B320(s32, u8 *, u8 *);
void func_800A5948(s32, s32);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A18E0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1940);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A19BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1AB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1C98);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1CFC);

/**
 * @brief Subtract delta from a halfword value, clamping to zero.
 *
 * @param unused0 Unused parameter.
 * @param unused1 Unused parameter.
 * @param delta Amount to subtract.
 * @param ptr Pointer to structure; halfword at offset 0x18 is modified.
 */
void func_800A1D78(s32 unused0, s32 unused1, s32 delta, u8 *ptr) {
    s16 val = *(u16 *)(ptr + 0x18) - delta;
    *(u16 *)(ptr + 0x18) = val;
    if (val <= 0) {
        *(u16 *)(ptr + 0x18) = 0;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1DA0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1E04);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1EC8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1F74);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2008);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A20AC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2150);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A21B0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2310);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2360);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A240C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2480);

/**
 * @brief Call func_8009B924 with constants 0x7A and 0x030E77FF.
 *
 * @param a0 First argument passed through.
 */
void func_800A2520(s32 a0) {
    func_8009B924(a0, 0x7A, 0x030E77FF);
}

/**
 * @brief Call func_8009B924 with constants 0x7E and 0x0180560D.
 *
 * @param a0 First argument passed through.
 */
void func_800A2548(s32 a0) {
    func_8009B924(a0, 0x7E, 0x0180560D);
}

/**
 * @brief Call func_8009B924 with constants 0x37E and 0x038E7FFF.
 *
 * @param a0 First argument passed through.
 */
void func_800A2570(s32 a0) {
    func_8009B924(a0, 0x37E, 0x038E7FFF);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2598);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2638);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A26A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2724);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2CE4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2D24);

/**
 * @brief Look up a byte from a two-level table.
 *
 * Indexes into D_800ED148 by a0*208, reads a byte at offset 0xDA,
 * then uses (byte-1)*2 to index into D_80078E00 at offset 0x4CFD.
 *
 * @param a0 Entity index (stride 208).
 * @return Byte value from the second-level table.
 */
s32 func_800A2E04(s32 a0) {
    u8 *table = D_80078E00;
    u8 *base = (u8 *)&D_800ED148;
    s32 val = *(u8 *)(base + a0 * 208 + 0xDA);
    val = (val - 1) * 2;
    return *(u8 *)(table + val + 0x4CFD);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2E48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2EB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2EF8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2F54);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2FC8);

/**
 * @brief Call func_800DEA58 with D_800EE462 byte value.
 */
void func_800A302C(void) {
    func_800DEA58(*(u8 *)D_800EE462);
}

/**
 * @brief Add an entry to the command table at D_800ED148+0xEFC.
 *
 * Reads the current table index from D_800ED148[0x1302], computes the
 * entry pointer (stride 24), and stores the provided fields.
 *
 * @param a0 First byte field (offset 0).
 * @param a1 Second byte field (offset 1).
 * @param a2 Halfword field (offset 4).
 * @param a3 Third byte field (offset 2).
 * @param arg5 Halfword field (offset 6).
 */
void func_800A3054(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5) {
    u8 *base = (u8 *)&D_800ED148;
    s32 idx;
    u8 *entry;
    idx = base[0x1302];
    base += 0xEFC;
    entry = base + idx * 24;
    entry[0] = a0;
    entry[1] = a1;
    *(u16 *)(entry + 4) = a2;
    entry[2] = a3;
    entry[3] = 0;
    *(u16 *)(entry + 6) = arg5;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A3094);

/**
 * @brief Clear two battle state bytes at D_800ED148 offsets 0x5C0 and 0x5C1.
 */
void func_800A30E4(void) {
    u8 *base = (u8 *)&D_800ED148;
    base[0x5C1] = 0;
    base[0x5C0] = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A30F8);

/**
 * @brief Initialize battle state: set flag, clear params, and store constants.
 *
 * @param a Byte pointer to clear.
 * @param b Byte pointer to set to 0xA.
 * @param c Halfword pointer to clear.
 * @param d Halfword pointer to set to 9.
 */
void func_800A42B4(u8 *a, u8 *b, u16 *c, u16 *d) {
    *(u8 *)D_800EE43C = 1;
    *a = 0;
    *d = 9;
    *b = 0xA;
    *c = 0;
}

/**
 * @brief Initialize battle state and call func_800A432C.
 *
 * Rearranges parameters: a1-a3+stack become a0-a3 for func_800A42B4,
 * then original a0 is passed to func_800A432C.
 *
 * @param a0 Argument for func_800A432C.
 * @param a1 Byte pointer (becomes a0 for func_800A42B4).
 * @param a2 Byte pointer (becomes a1 for func_800A42B4).
 * @param a3 Halfword pointer (becomes a2 for func_800A42B4).
 * @param stack0 Halfword pointer (becomes a3 for func_800A42B4).
 */
void func_800A42DC(s32 a0, u8 *a1, u8 *a2, u16 *a3, u16 *stack0) {
    func_800A42B4(a1, a2, a3, stack0);
    func_800A432C(a0);
}

/**
 * @brief Store a value to the global D_800EE424.
 *
 * @param value Value to store.
 */
void func_800A4320(s32 value) {
    *(s32 *)D_800EE424 = value;
}

/**
 * @brief Call func_80020F84 and store the result to D_800EE424.
 *
 * @param a0 Argument passed to func_80020F84.
 */
void func_800A432C(s32 a0) {
    *(s32 *)D_800EE424 = func_80020F84(a0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4350);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A43C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4434);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A44FC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4618);

/**
 * @brief Call one of two processing functions based on entity count.
 *
 * If the input (masked to 16 bits) is less than 8, calls func_800A980C;
 * otherwise calls func_800A9888. Returns the lower 16 bits of the result.
 *
 * @param count Entity count (u16).
 * @return Result masked to 16 bits.
 */
u16 func_800A475C(u16 count) {
    s32 result;
    if (count < 8) {
        result = func_800A980C();
    } else {
        result = func_800A9888();
    }
    return (u16)result;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4798);

/**
 * @brief Call func_800A4798 for each of 7 entities, storing result at offset 0xD9.
 *
 * Iterates over 7 entities (stride 0xD0) in D_800ED148, calling func_800A4798
 * with the entity index and a0, storing the result byte at offset 0xD9.
 *
 * @param a0 Parameter passed through to func_800A4798.
 */
void func_800A47E4(s32 a0) {
    s32 i = 0;
    u8 *base = (u8 *)&D_800ED148;
    do {
        base[0xD9] = func_800A4798(i, a0);
        i++;
        base += 0xD0;
    } while (i < 7);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4844);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4898);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A493C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4A74);

/**
 * @brief Unpack a 16-bit value into entity fields.
 *
 * Stores the lower 13 bits as a halfword at D_800ED148.unk12E0,
 * and the upper 3 bits (shifted right 13) as a byte at D_800ED148.unk130F.
 *
 * @param arg0 Packed 16-bit value.
 */
void func_800A4B68(s32 arg0) {
    s16 tmp = arg0;
    s32 tmp2 = arg0 & 0xE000;
    D_800ED148.unk12E0 = (tmp &= 0x1FFF);
    tmp = ((u32)tmp2) >> 0xD;
    D_800ED148.unk130F = (s8)tmp;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4B88);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4C84);

/**
 * @brief Append a value to the battle command queue and set the current command.
 *
 * Increments the queue index at D_800ED148[0x5C0], stores the value at the
 * computed queue slot (stride 20), and also stores it at D_800ED148[0x1305].
 *
 * @param a0 Command value to store.
 */
void func_800A4DD4(s32 a0) {
    u8 *base = (u8 *)&D_800ED148;
    u8 idx = base[0x5C0];
    base[0x5C0] = idx + 1;
    *(u8 *)(base + idx * 20 + 0x5D4) = a0;
    base[0x1305] = a0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4E08);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4EA0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4F28);

/**
 * @brief Extract set bit positions from a 16-bit mask into a buffer.
 *
 * Iterates over bits 0-15. For each set bit, stores the bit position
 * to *dst and increments dst. Returns the count of set bits (masked to 8 bits).
 *
 * @param mask 16-bit bitmask to scan.
 * @param dst Destination buffer for bit positions.
 * @return Number of set bits found (0-16), masked to u8.
 */
s32 func_800A4FC4(s32 mask, u8 *dst) {
    s32 count = 0;
    s32 bit = 1;
    s32 pos = 0;
    do {
        if ((mask & bit) & 0xFFFF) {
            *dst++ = pos;
            count++;
        }
        pos++;
        bit <<= 1;
    } while (pos < 16);
    return count & 0xFF;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5004);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A517C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5210);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A52E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A53C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5454);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A554C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A559C);

/**
 * @brief Clear the word at offset 0x24 of a battle entity.
 *
 * @param idx Entity index (stride 0xD0).
 */
void func_800A565C(s32 idx) {
    u8 *base = (u8 *)&D_800ED148;
    u8 *entity;
    asm("");
    entity = base + idx * 0xD0;
    *(volatile s32 *)(entity + 0x24) = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5688);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5778);

/**
 * @brief Set up entity battle data and process action.
 *
 * Reads the entity index from D_800ED148[0x12F2], computes the entity
 * data pointer (stride 44 at offset 0xD64) and status pointer (offset 0x1100),
 * calls func_8009B320 to initialize them, then func_800A5948 to process.
 *
 * @param a0 Battle action parameter.
 */
void func_800A57E0(s32 a0) {
    u8 *base = (u8 *)&D_800ED148;
    u8 *entityBase = base + 0xD64;
    s32 idx = base[0x12F2];
    base += 0x1100;
    func_8009B320(a0, entityBase + idx * 44, base + idx);
    func_800A5948(a0, idx);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A584C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A589C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5948);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A59AC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5A7C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5AF4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5BC4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5C48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5F24);
