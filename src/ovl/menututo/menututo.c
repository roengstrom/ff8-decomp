#include "common.h"

/**
 * @brief Read tutorial column index 1.
 *
 * @return Byte value of D_801E4EC1.
 */
s32 func_801E2800(void) {
    extern u8 D_801E4EC1;
    return D_801E4EC1;
}

/**
 * @brief Read tutorial column index 2.
 *
 * @return Byte value of D_801E4EC2.
 */
s32 func_801E2810(void) {
    extern u8 D_801E4EC2;
    return D_801E4EC2;
}

/**
 * @brief Load a sub-overlay at an offset from the base address.
 *
 * Dispatches to loadSubOverlay with a base pointer adjusted by a
 * fixed offset determined by the tutorial section index.
 *
 * @param index Tutorial section index (0-6).
 * @param base Base address of the overlay data.
 */
void func_801E2820(s32 index, u8 *base) {
    switch (index) {
    case 0:
        base += 0x1C;
        break;
    case 1:
        base += 0x14;
        break;
    case 2:
        base += 0x18;
        break;
    case 3:
        base += 0x2C;
        break;
    case 4:
        base += 0x30;
        break;
    case 5:
        base += 0x47;
        break;
    case 6:
        base += 0xB4;
        break;
    }

    loadSubOverlay(base, 0x801D1000);
}

/**
 * @brief Load tutorial page indices from table.
 *
 * Reads two bytes from D_801E4EB4[a0] (4-byte stride) and stores
 * them into D_801E4EC1 and D_801E4EC2 respectively.
 *
 * @param a0 Tutorial page table index.
 */
void func_801E28A8(s32 a0) {
    extern u8 D_801E4EB4[];
    extern u8 D_801E4EC1;
    extern u8 D_801E4EC2;
    u8 *entry;

    entry = D_801E4EB4;
    entry = entry + a0 * 4;
    D_801E4EC1 = entry[0];
    D_801E4EC2 = entry[1];
}

/**
 * @brief Read tutorial page index.
 *
 * @return Byte value of D_801E4EC0.
 */
s32 func_801E28D4(void) {
    extern u8 D_801E4EC0;
    return D_801E4EC0;
}

/** @brief Draw inner panel with section id 0xD and clear flag. */
s32 func_801E28E4(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 0);
}

/** @brief Draw inner panel with section id 0xD and set flag. */
s32 func_801E2910(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 1);
}

/** @brief Call func_801F0A34 with a0, 0, 0x22, and a1 * 15 + 0x3E. */
void func_801E293C(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0x22, a1 * 15 + 0x3E);
}

/**
 * @brief Draw tutorial text with computed CLUT and Y position.
 *
 * Computes a CLUT index from 0x1000 minus field 0x2C via lookup table
 * D_801FA3C8, then calls func_801F0A34 with the computed x/y coordinates.
 *
 * @param a0 OT pointer
 * @param a1 Pointer to tutorial state structure
 */
/** @brief Tutorial state structure for rendering callbacks. */
typedef struct {
    u8 pad00[0x2A];
    s16 field_2A;       /**< Scroll/fade position. */
    s16 field_2C;       /**< Scroll/fade progress value. */
    u8 pad2E[4];
    s8 field_32;        /**< Tutorial entry index (signed). */
    u8 pad33[2];
    u8 field_35;        /**< Tutorial page parameter index. */
} TutoState;

void func_801E296C(s32 a0, TutoState *data) {
    extern u16 D_801FA3C8[];

    s32 index;
    s32 tableVal;
    s32 scaled;
    s32 val = data->field_2C;
    u8 param = data->field_35;

    scaled = val;
    index = (tableVal = 0x1000 - val);
    index /= 64;

    tableVal = D_801FA3C8[index];
    scaled = (D_801FA3C8[index] * 190) / 4096;
    func_801F0A34(a0, 0, scaled + 0xA8, (param * 15) + 0x3E);
}

/**
 * @brief Draw tutorial text with table-based X scaling and entry-based Y.
 *
 * Computes Y from the entry index modulo 10, and X from a CLUT lookup
 * table indexed by the scroll position divided by 64.
 *
 * @param a0 OT pointer.
 * @param data Pointer to tutorial state structure.
 */
void func_801E29F8(s32 a0, TutoState *data) {
    extern u16 D_801FA3C8[];

    s32 tableVal;
    u32 scaled;
    u32 height;
    u32 mod = 10;
    s16 pos = data->field_2A;
    s8 val = (s8)data->field_32;

    height = ((val % (tableVal = 10)) * 15) + 0x3E;
    tableVal = D_801FA3C8[(tableVal = pos) / 64];
    scaled = (tableVal * 190) / 4096;

    func_801F0A34(a0, 0, scaled + 0xA8, height);
}

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E2ABC);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E2D3C);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E2EF0);

/**
 * @brief Scan tutorial entry table and build list of available entries.
 *
 * Iterates over 9 entries in D_801E4E3C (12-byte stride), calls
 * testFieldFlag to check if each entry's item (byte at offset 8) is
 * available. Available entries' indices are stored sequentially at
 * a0+0x39, and the total count is stored at a0+0x36.
 *
 * @param a0 Pointer to tutorial state structure
 */
void func_801E30C4(u8 *a0) {
    extern u8 D_801E4E3C[];
    s32 i = 0;
    s32 count = 0;
    u8 *table = D_801E4E3C;

    do {
        if (testFieldFlag(table[8]) != 0) {
            *(u8 *)(a0 + count + 0x39) = i;
            count++;
        }
        i++;
        table += 0xC;
    } while (i < 9);

    *(u8 *)(a0 + 0x36) = count;
}

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E3140);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E3EC0);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E3F8C);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4080);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4214);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E431C);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E43D4);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4598);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E46DC);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E47F8);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E48C0);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4BD0);

/**
 * @brief Conditionally call func_801E4BD0 if field 0x20 is non-zero.
 * @param a0 Pointer to structure with s16 field at offset 0x20
 * @param a1 First argument passed to func_801E4BD0
 * @param a2 Second argument passed to func_801E4BD0
 */
INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4CB0);

/**
 * @brief Initialize tutorial menu: register callbacks, clear state, and enter.
 *
 * Registers func_801E48C0 and func_801E4CB0 as callbacks via func_801F179C.
 * If registration succeeds, clears the 16-bit fields at offsets 0x20 and 0x22,
 * calls func_801F010C(0x140) for display setup, then enters via func_801E48C0.
 */
void func_801E4CE4(void) {
    extern void func_801E48C0();
    extern void func_801E4CB0();
    s32 result = func_801F179C((s32)func_801E48C0, (s32)func_801E4CB0);

    if (result != 0) {
        *(s16 *)(result + 0x20) = 0;
        *(s16 *)(result + 0x22) = 0;
        func_801F010C(0x140);
        func_801E48C0(result);
    }
}
