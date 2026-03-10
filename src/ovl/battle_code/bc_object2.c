/**
 * @file bc_object2.c
 * @brief Battle entity status effects and damage calculation.
 *
 * Handles status effect application, hit probability calculations,
 * damage computation, and entity flag management for the battle system.
 */
#include "common.h"
#include "battle.h"

extern u8 D_800ED148[];
extern u8 D_800EEBC4[];
extern u8 D_800EEBBB[];
extern u8 D_800EE456[];
extern u8 D_800EE4C0[];
extern u8 D_80078E00[];
extern u8 D_800EE476[];
extern u8 D_800ED70C[];
extern u8 D_800EEBBC[];
extern u8 D_800EEBB9[];
extern u8 D_800EEBB8[];
extern u8 D_800EEBBA[];
extern u8 D_800EEBC2[];
extern u8 D_800ED160[];
extern u8 D_80078720[];
extern u8 D_80077378[];
extern u8 D_800786D9[];

void func_8009B79C(s32, s32);
s32 func_8009B7BC(s32);
void func_8009B924(s32, s32, s32);
s32 func_800A09D0(s32);
void func_800A4FC4(s32, u8 *);
void func_800A5210(s32);
s32 func_800B0574(s32, s32);
s32 func_800B0600(s32, s32);
s32 func_800B0F7C(s32);
s32 func_800B0F9C(s32);
s32 func_800A1760(s32);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009BAC4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009BB3C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009BB98);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009BBD0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009BC28);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009BCE4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009BD60);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009BDD0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009BE24);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009BEA4);

/**
 * @brief Conditionally XOR masked flag bits.
 *
 * Compares masked versions of newFlags and *flagsPtr. If they differ,
 * returns the masked current flags XORed with masked new flags.
 *
 * @param newFlags New flag value to compare.
 * @param flagsPtr Pointer to current flags word.
 * @param mask Bit mask for comparison.
 * @return Masked current value, XORed with masked newFlags if different.
 */
s32 func_8009BF50(s32 newFlags, s32 *flagsPtr, s32 mask) {
    s32 current = *flagsPtr;
    newFlags &= mask;
    current &= mask;
    if (current != newFlags) {
        current ^= newFlags;
    }
    return current;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009BF70);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009BFE0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009C090);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009C104);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009C300);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009C390);

/**
 * @brief Look up entity type byte from a battle table.
 * @param id Entity identifier (masked to 16 bits).
 * @return First byte of the resolved table entry.
 */
s32 func_8009C570(s32 id) {
    u8 buf[8];
    func_800A4FC4(id & 0xFFFF, buf);
    return buf[0];
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009C598);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009C610);

/**
 * @brief Clamp a damage value to a maximum of 9999.
 * @param val Value to clamp.
 * @return val if < 10000, otherwise 9999.
 */
s32 func_8009C6CC(s32 val) {
    if (val >= 10000) {
        val = 9999;
    }
    return val;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009C6E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009C798);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009C8B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009CA14);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009CAD8);

/**
 * @brief Conditionally queue a status effect clear command.
 *
 * If D_800EE456 bits 0-1 are clear, queues a battle command
 * to clear status flag 0x4001 on the given entity.
 *
 * @param a0 Entity slot index.
 */
void func_8009CD10(s32 a0) {
    if ((*(u8 *)D_800EE456 & 3) == 0) {
        func_8009B924(a0, 0, 0x4001);
    }
}

/**
 * @brief Conditionally queue a high-bit status clear command.
 *
 * If D_800EE456 bits 0-1 are clear, queues a battle command
 * to clear status flag 0x800000 on the given entity.
 *
 * @param a0 Entity slot index.
 */
void func_8009CD44(s32 a0) {
    if ((*(u8 *)D_800EE456 & 3) == 0) {
        func_8009B924(a0, 0, 0x800000);
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009CD78);

/**
 * @brief Query battle state 0x21 and add offset 0xEF to the result.
 *
 * @return func_8009B7BC(0x21) + 0xEF.
 */
s32 func_8009CF18(void) {
    return func_8009B7BC(0x21) + 0xEF;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009CF38);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009D174);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009D228);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009D420);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009D474);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009D508);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009D594);

/**
 * @brief Conditionally set control flag bit 0x10 in D_800EE4C0.
 *
 * If bit 2 of D_800EE4C0[6] is clear and the u16 at D_800EE4C0+0x1C
 * equals 0x49, sets bit 0x10 in D_800EE4C0[6].
 */
void func_8009D68C(void) {
    s32 base = (s32)D_800EE4C0;
    u8 val = *(u8 *)(base + 6);
    if (!(val & 4)) {
        if (*(u16 *)(base + 0x1C) == 0x49) {
            *(u8 *)(base + 6) = val | 0x10;
        }
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009D6C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009D7D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009DCCC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009DD2C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009DEF0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009E110);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009E33C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009E418);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009E528);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009E5C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009E684);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009E7B0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009E95C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009EA08);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009EAEC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009ED2C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009EE44);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009EF64);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009F040);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009F168);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009F23C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009F350);

/**
 * @brief Read and clear an entity's pending damage value.
 *
 * Reads a u16 at entity offset 0xDC, clears it, and returns the
 * value right-shifted by 2.
 *
 * @param entityIdx Entity index.
 * @return Pending damage value divided by 4.
 */
s32 func_8009F3F8(s32 entityIdx) {
    u8 *base = D_800ED148;
    u8 *entity;
    s32 val;
    asm("");
    entity = base + entityIdx * 0xD0;
    val = *(u16 *)(entity + 0xDC);
    *(u16 *)(entity + 0xDC) = 0;
    return val >> 2;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009F428);

/**
 * @brief Mark entity for processing and set a control flag.
 *
 * Sets bit 0 in D_800EE4C0[5] and sets bit 0x10000 in the entity's
 * flags word at offset 0x18.
 *
 * @param entityIdx Entity index.
 * @return Always 0.
 */
s32 func_8009F46C(s32 entityIdx) {
    s32 ctrl = (s32)D_800EE4C0;
    u8 *base;
    u8 *entity;
    *(u8 *)(ctrl + 5) |= 1;
    base = D_800ED148;
    asm("");
    entity = base + entityIdx * 0xD0;
    *(s32 *)(entity + 0x18) |= 0x10000;
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009F4BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009F52C);

/**
 * @brief Set a bit in the battle flag array at D_80077378+0xD0C.
 *
 * Computes the word index and bit position from the given bit index,
 * then ORs the corresponding bit into the flag array.
 *
 * @param bitIndex The bit index to set.
 */
void func_8009F570(s32 bitIndex) {
    s32 wordIdx = bitIndex / 32;
    s32 bitPos = bitIndex % 32;
    s32 base = (s32)D_80077378;
    *(s32 *)(base + wordIdx * 4 + 0xD0C) |= (1 << bitPos);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009F5B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009F65C);

/**
 * @brief Look up a byte attribute from D_80078E00 table (stride 0x3C).
 * @param idx Entry index.
 * @return Byte at offset 0x228 within the table entry.
 */
s32 func_8009F6F4(s32 idx) {
    u8 *base = D_80078E00;
    u8 *entry;
    asm("");
    entry = base + idx * 0x3C;
    return entry[0x228];
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009F718);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009F824);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009F930);

/**
 * @brief Store a command byte to entity data.
 *
 * Stores the full command value at D_800ED148[0x130E], then stores the
 * low 2 bits at entity[0x99] where the entity index is read from
 * D_800ED148[0x12F3].
 *
 * @param cmd Command byte value.
 */
void func_8009FCF4(s32 cmd) {
    u8 *base = D_800ED148;
    s32 idx = base[0x12F3];
    base[0x130E] = cmd;
    cmd &= 3;
    *(u8 *)(base + idx * 0xD0 + 0x99) = cmd;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009FD28);

/**
 * @brief Check if a value is at least 20.
 * @param val Value to test.
 * @return 1 if val >= 20, 0 otherwise.
 */
s32 func_8009FDD4(s32 val) {
    return val >= 20;
}

/**
 * @brief Compute an entity table offset from a byte field.
 *
 * Looks up byte at offset 0xDA in entity a0's table entry (stride 208),
 * then returns (a1 * 4 - 1) + that byte value.
 *
 * @param a0 Entity index.
 * @param a1 Multiplier input.
 * @return Computed offset value.
 */
s32 func_8009FDE0(s32 a0, s32 a1) {
    s32 base;
    a1 = a1 * 4;
    base = (s32)D_800ED148;
    a1 -= 1;
    return a1 + *(u8 *)(base + a0 * 208 + 0xDA);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_8009FE14);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_800A085C);

/**
 * @brief Get the active entity count, defaulting to 1 if unset.
 *
 * @return The value of D_800786D9, or 1 if it is zero.
 */
s32 func_800A08C0(void) {
    u8 val = *(u8 *)D_800786D9;
    if (val == 0) {
        return 1;
    }
    return val;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_800A08E0);

/**
 * @brief Apply 1.5x multiplier to battle control value if entity has status bit 0x20.
 *
 * If the entity at entityIdx has status bit 0x20 set, multiplies
 * the word at D_800EE4C0[0xC] by 3/2 (rounds down).
 *
 * @param entityIdx Index into the battle entity array.
 */
void func_800A0978(s32 entityIdx) {
    u8 *base = D_800ED148;
    u8 *entity;
    asm("");
    entity = base + entityIdx * 0xD0;
    if (*(u16 *)(entity + 0x90) & 0x20) {
        s32 ctrl = (s32)D_800EE4C0;
        u32 val = *(u32 *)(ctrl + 0xC);
        *(u32 *)(ctrl + 0xC) = (val * 3) >> 1;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_800A09D0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_800A1760);

/**
 * @brief Set masked attribute values on a D_80078720 table entry (stride 0x1D0).
 *
 * Stores a masked u16 at offset 0x1B2 and a masked u32 at offset 0x188.
 *
 * @param idx Entry index.
 * @param attr Attribute value (masked to 7 bits).
 * @param flags Flag value (masked to 0x30E7FFF).
 */
void func_800A184C(s32 idx, s32 attr, s32 flags) {
    u8 *entry = D_80078720 + idx * 0x1D0;
    *(u16 *)(entry + 0x1B2) = attr & 0x7F;
    *(s32 *)(entry + 0x188) = flags & 0x30E7FFF;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object2", func_800A1888);
