#include "common.h"

extern u8 D_80102E40[];
extern u8 D_80102E70[];
extern u8 D_80102E78[];
extern u8 D_80103240[];
extern u8 D_80103308[];
extern u8 D_80103420[];
extern u8 D_80077E5C[];
extern u8 D_80078842[];
extern u8 D_8007873E[];
void func_800D18B0(void);
void func_800D3044(s32, s32);
void func_800D3164(void);
void func_800D1A20(void);
void func_800D2F14(void);
void func_800D5C28(s32, s32, s32, s32);
void func_800D5D08(s32, s32);
void func_800D33F8(void);
void func_800D3A00(void);
void func_800A5F24(s32, s32);
void func_800A2360(s32);

/**
 * @brief Initialize 3 entries in D_80103420 table and register task handlers.
 *
 * Calls func_800D18B0 to prepare, then iterates 3 entries in D_80103420
 * (stride 0x6C each) calling func_800D3044 for each. Registers func_800D1A20
 * and func_800D2F14 as task handlers for slot 7, sets slot 7 mode to 2,
 * then calls func_800D1A20.
 */
void func_800D325C(void) {
    s32 ptr = (s32)D_80103420;
    s32 i;
    func_800D18B0();
    i = 0;
    do {
        func_800D3044(ptr, i);
        i++;
        ptr += 0x6C;
    } while (i < 3);
    func_800D3164();
    func_800D5C28(7, (s32)func_800D1A20, (s32)func_800D2F14, 0);
    func_800D5D08(7, 2);
    func_800D1A20();
}

/**
 * @brief Store battle command parameters to D_80103240.
 *
 * Always stores a0 at offset 7. If a0 is zero, clears the
 * D_80103240 halfword. If a0 is non-zero, stores a1 at offset 6.
 *
 * @param a0 Command type (stored at offset 7).
 * @param a1 Command parameter (stored at offset 6 if a0 != 0).
 */
void func_800D32E8(s32 a0, s32 a1) {
    s32 base = (s32)D_80103240;
    *(u8 *)(base + 7) = a0;
    if (a0 == 0) {
        *(s16 *)D_80103240 = 0;
    } else {
        *(u8 *)(base + 6) = a1;
    }
}

/**
 * @brief Test if a specific bit is set in a 64-bit value.
 *
 * The low 32 bits are at a0+0xC, high 32 bits at a0+0x10.
 *
 * @param a0 Pointer to data containing 64-bit bitfield.
 * @param bit Bit position to test (0-63, masked to 6 bits).
 * @return 1 if bit is set, 0 otherwise.
 */
s32 func_800D330C(u8 *a0, s32 bit) {
    s32 word;
    s32 mask;
    s32 result;
    bit &= 0x3F;
    if (bit >= 32) {
        word = *(s32 *)(a0 + 0x10);
        bit &= 0x1F;
    } else {
        word = *(s32 *)(a0 + 0xC);
        bit &= 0x1F;
    }
    mask = 1 << bit;
    result = word & mask;
    return result != 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3344);

/**
 * @brief Count the number of set bits in a 32-bit value.
 *
 * @param val Bitmask to count.
 * @return Number of set bits (popcount).
 */
s32 func_800D33C0(s32 val) {
    s32 count = 0;
    s32 i = 0;
    s32 mask = 1;
    do {
        if (val & (mask << i)) {
            count++;
        }
        i++;
    } while (i < 32);
    return count;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D33F8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D365C);

/**
 * @brief Clamp a0 and call func_80020F84 with offset.
 *
 * If a0 >= 0x20, subtracts 0x10 to wrap it back. Then calls
 * func_80020F84 with the adjusted value plus 0x39.
 *
 * @param a0 Input index, clamped if >= 0x20.
 */
void func_800D3708(s32 a0) {
    if (a0 >= 0x20) {
        a0 -= 0x10;
    }
    func_80020F84(a0 + 0x39);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3734);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D37D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3A00);

/**
 * @brief Initialize D_80103240 struct and register task handler.
 *
 * Zeros all fields of the D_80103240 state struct, then registers
 * func_800D33F8 and func_800D3A00 as task handlers via func_800D5C28,
 * and calls func_800D33F8 to start processing.
 */
void func_800D3BB8(void) {
    s32 base = (s32)D_80103240;
    *(u8 *)(base + 7) = 0;
    *(u8 *)(base + 6) = 0;
    *(u16 *)(base) = 0;
    *(u16 *)(base + 2) = 0;
    *(u8 *)(base + 6) = 0;
    *(u16 *)(base + 4) = 0;
    func_800D5C28(5, (s32)func_800D33F8, (s32)func_800D3A00, 0);
    func_800D33F8();
}

/**
 * @brief Compute pointer into entity table from D_80103308 fields.
 *
 * Reads entity index from D_80103308+0x14 and sub-index from D_80103308+0x13,
 * then returns D_8007873E + entity_index * 464 + sub_index * 4.
 *
 * @return Computed entity field pointer.
 */
/**
 * @brief Get entity field pointer from D_80103308 config.
 *
 * Reads the signed entity index from D_80103308+0x14 and the unsigned
 * sub-field index from D_80103308+0x13. Returns pointer into
 * D_8007873E at index * 464 + sub * 4.
 *
 * @return Pointer to entity field (as integer).
 *
 * @note Non-matching: CC1PSX scheduler moves `sll a0, a0, 2` (sub*4)
 * before `lui v1, %hi(D_8007873E)`, reordering the addition as
 * (sub*4 + base) + idx*464 vs original (base + idx*464) + sub*4.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3C14);

/**
 * @brief Return the address of D_80102E40.
 *
 * @return Address of D_80102E40 as an integer.
 */
s32 func_800D3C50(void) {
    return (s32)D_80102E40;
}

/**
 * @brief Read word at offset 4 from entry in D_80102E40 table.
 *
 * @param index Entry index (stride 12).
 * @return Word at offset 4 of the entry.
 */
s32 func_800D3C5C(s32 index) {
    u8 *base = (u8 *)func_800D3C50();
    return *(s32 *)(base + index * 12 + 4);
}

/**
 * @brief Read halfword at offset 2 from entry in D_80102E40 table.
 *
 * @param index Entry index (stride 12).
 * @return Halfword at offset 2 of the entry.
 */
u16 func_800D3C98(s32 index) {
    u8 *base = (u8 *)func_800D3C50();
    return *(u16 *)(base + index * 12 + 2);
}

/**
 * @brief Read halfword at offset 8 from entry in D_80102E40 table.
 *
 * @param index Entry index (stride 12).
 * @return Halfword at offset 8 of the entry.
 */
u16 func_800D3CD4(s32 index) {
    u8 *base = (u8 *)func_800D3C50();
    return *(u16 *)(base + index * 12 + 8);
}

/**
 * @brief Clear D_80102E70 to zero.
 */
void func_800D3D10(void) {
    *(s32 *)D_80102E70 = 0;
}

/**
 * @brief Decrement D_80102E70 if it is positive.
 */
void func_800D3D1C(void) {
    s32 val = *(s32 *)D_80102E70;
    if (val > 0) {
        *(s32 *)D_80102E70 = val - 1;
    }
}

/**
 * @brief Return the word value at D_80102E70.
 *
 * @return Current value of D_80102E70.
 */
s32 func_800D3D3C(void) {
    return *(s32 *)D_80102E70;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3D4C);

/**
 * @brief Call func_800D3D4C with a1 = 1.
 *
 * @param a0 First argument passed through.
 */
void func_800D3E18(s32 a0) {
    func_800D3D4C(a0, 1);
}

/**
 * @brief Call func_800D3D4C with a1 = 0.
 *
 * @param a0 First argument passed through.
 */
void func_800D3E38(s32 a0) {
    func_800D3D4C(a0, 0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3E58);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3EE8);

void func_800D3F70(void) {
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3F78);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4038);

/**
 * @brief Read battle state value or return -1.
 *
 * If the halfword at D_80103308+0x10 equals 0x1000, returns the
 * signed byte at D_80103308+0x14. Otherwise returns -1.
 *
 * @return Battle state byte or -1.
 */
s32 func_800D40EC(void) {
    s32 base = (s32)D_80103308;
    if (*(s16 *)(base + 0x10) == 0x1000) {
        return *(s8 *)(base + 0x14);
    }
    return -1;
}

/**
 * @brief Read battle state value or return -1 (zero check).
 *
 * If the halfword at D_80103308+0x10 is non-zero, returns the
 * signed byte at D_80103308+0x14. Otherwise returns -1.
 *
 * @return Battle state byte or -1.
 */
s32 func_800D4110(void) {
    s32 base = (s32)D_80103308;
    if (*(s16 *)(base + 0x10) != 0) {
        return *(s8 *)(base + 0x14);
    }
    return -1;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4134);

/**
 * @brief Clear 21 bytes in a structure (offsets 0x54-0x68).
 *
 * Zeroes 16 bytes at a0+0x54, 4 bytes at a0+0x64, and 1 byte at a0+0x68
 * using two countdown loops and a final store.
 *
 * @param a0 Base pointer to structure.
 */
void func_800D422C(s32 a0) {
    register s32 i asm("$2") = 0xF;
    register s32 ptr asm("$3") = a0 + 0xF;
    do {
        *(u8 *)(ptr + 0x54) = 0;
        i--;
        ptr--;
    } while (i >= 0);
    i = 3;
    ptr = a0 + 3;
    do {
        *(u8 *)(ptr + 0x64) = 0;
        i--;
        ptr--;
    } while (i >= 0);
    *(u8 *)(a0 + 0x68) = 0;
}

/**
 * @brief Conditionally call func_800D422C based on D_80077E5C flag.
 *
 * If bit 2 of the halfword at D_80077E5C is clear, calls func_800D422C
 * with the passed pointer.
 *
 * @param a0 Base pointer passed through to func_800D422C.
 */
void func_800D4264(s32 a0) {
    if ((*(u16 *)D_80077E5C & 4) == 0) {
        func_800D422C(a0);
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4294);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4630);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D475C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4804);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D49B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4B04);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4BC4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4D84);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4E44);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4F24);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5064);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5580);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D568C);

/**
 * @brief Call func_800D568C with a2 = D_80103308.
 *
 * @param a0 First argument passed through.
 * @param a1 Second argument passed through.
 */
void func_800D581C(s32 a0, s32 a1) {
    func_800D568C(a0, a1, D_80103308);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5840);

/**
 * @brief Initialize default state fields in D_80103308.
 *
 * Sets display dimensions, clears status flags, and resets counters
 * in the battle state structure.
 */
void func_800D5ADC(void) {
    s32 base = (s32)D_80103308;
    *(u8 *)(base + 0x21) = 0x40;
    *(u8 *)(base + 0x22) = 0x38;
    *(u16 *)(base + 0x26) = 0x40;
    *(u16 *)(base + 0x10) = 0;
    *(u8 *)(base + 0x12) = 0;
    *(u8 *)(base + 0x13) = 0;
    *(u8 *)(base + 0x16) = 0;
    *(u8 *)(base + 0x17) = 0;
    *(u8 *)(base + 0x1A) = 0;
    *(u16 *)(base + 0x24) = 0;
    *(u16 *)(base + 0x28) = 0;
    *(u16 *)(base + 0x18) = 0;
    *(u8 *)(base + 0x1B) = 0;
    *(s8 *)(base + 0x14) = -1;
    *(s8 *)(base + 0x15) = -1;
    *(u16 *)(base + 0x10) = 0;
    *(u8 *)(base + 0x2B) = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5B3C);

/**
 * @brief Return the address of D_80102E78.
 *
 * @return Address of D_80102E78 as an integer.
 */
s32 func_800D5C00(void) {
    return (s32)D_80102E78;
}

/**
 * @brief Compute a pointer into the D_80102E78 array with stride 20.
 *
 * @param a0 Array index.
 * @return Pointer to the entry at a0 * 20 + D_80102E78.
 */
s32 func_800D5C0C(s32 a0) {
    return a0 * 20 + (s32)D_80102E78;
}

/**
 * @brief Initialize an entry in the D_80102E78 table.
 *
 * Computes the entry at D_80102E78 + a0 * 20, sets bytes at offsets
 * 0x10 and 0x11 to 0xFF, byte at 0x12 to 0, and stores a1, a2, a3
 * as words at offsets 0, 8, and 0xC.
 *
 * @param a0 Entry index (stride 20).
 * @param a1 Value for offset 0.
 * @param a2 Value for offset 8.
 * @param a3 Value for offset 0xC.
 */
void func_800D5C28(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 entry = (s32)D_80102E78;
    entry += a0 * 20;
    *(s8 *)(entry + 0x10) = -1;
    *(s8 *)(entry + 0x11) = -1;
    *(u8 *)(entry + 0x12) = 0;
    *(s32 *)entry = a1;
    *(s32 *)(entry + 8) = a2;
    *(s32 *)(entry + 0xC) = a3;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5C60);

/**
 * @brief Read a signed byte from an array element at stride 20.
 *
 * Reads the signed byte at offset 0x10 within the a0-th entry
 * (stride 20) of the D_80102E78 array.
 *
 * @param a0 Array index.
 * @return Signed byte value.
 */
s32 func_800D5CE4(s32 a0) {
    u8 *base = D_80102E78;
    base = base + a0 * 20;
    return *(s8 *)(base + 0x10);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5D08);

/**
 * @brief Store a byte into an array element at stride 20.
 *
 * Writes a1 as a byte to offset 0x10 within the a0-th entry
 * (stride 20) of the D_80102E78 array.
 *
 * @param a0 Array index.
 * @param a1 Byte value to store.
 */
void func_800D5D64(s32 a0, s32 a1) {
    u8 *base = D_80102E78;
    base = base + a0 * 20;
    base[0x10] = a1;
}

/**
 * @brief Set entry fields to -1 and 1 at stride-20 offsets.
 *
 * Stores -1 at offset 0x11 and 1 at offset 0x12 in the a0-th entry
 * (stride 20) of D_80102E78.
 *
 * @param a0 Array index.
 */
void func_800D5D84(s32 a0) {
    u8 *base = D_80102E78;
    base = base + a0 * 20;
    *(s8 *)(base + 0x11) = -1;
    base[0x12] = 1;
}

/**
 * @brief Store a value and set flag at stride-20 offsets.
 *
 * Stores a1 at offset 0x11 and 1 at offset 0x12 in the a0-th entry
 * (stride 20) of D_80102E78.
 *
 * @param a0 Array index.
 * @param a1 Value to store at offset 0x11.
 */
void func_800D5DB0(s32 a0, s32 a1) {
    s32 base = (s32)D_80102E78;
    base = base + a0 * 20;
    *(u8 *)(base + 0x11) = a1;
    *(u8 *)(base + 0x12) = 1;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5DD8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5E48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5F98);

/**
 * @brief Read bit 0 from scratchpad control register at 0x1F8003AE.
 *
 * @return 1 if bit 0 is set, 0 otherwise.
 */
s32 func_800D6090(void) {
    return *(u16 *)0x1F8003AE & 1;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D60A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D60F4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D6294);

/**
 * @brief Clear all 9 display slots and reset scratchpad bit 5.
 *
 * Calls func_800D5C28 with (i, 0, 0, 0) for i from 8 down to 0,
 * then clears bit 5 of the scratchpad status halfword at 0x1F8003AE.
 */
void func_800D6310(void) {
    s32 i = 8;
top:
    func_800D5C28(i, 0, 0, 0);
    i--;
    if (i >= 0) goto top;
    {
        s32 scratch = 0x1F800390;
        *(u16 *)(scratch + 0x1E) &= 0xFFDF;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D636C);

/**
 * @brief Wrapper for func_800DBCBC.
 */
void func_800D672C(void) {
    func_800DBCBC();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D674C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D67B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D6938);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D69BC);

/**
 * @brief Get pointer to entity data at given index.
 *
 * Computes D_80078842 + index * 464 (0x1D0 stride per entity).
 *
 * @param index Entity index.
 * @return Pointer to entity data.
 */
u8 *func_800D6AC8(s32 index) {
    return D_80078842 + index * 464;
}

/**
 * @brief Return constant 0xA2 (162).
 *
 * @return Always 0xA2.
 */
s32 func_800D6AEC(void) {
    return 0xA2;
}
