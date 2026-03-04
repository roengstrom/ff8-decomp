#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

INCLUDE_ASM("asm/nonmatchings/11D3C", func_8002153C);


INCLUDE_ASM("asm/nonmatchings/11D3C", func_80021628);


INCLUDE_ASM("asm/nonmatchings/11D3C", func_8002166C);


INCLUDE_ASM("asm/nonmatchings/11D3C", func_800216B0);


/**
 * @brief Compute an experience or stat curve value using a quadratic formula.
 * @param a0 Level or input value for the formula.
 * @param a1 Entry index into the D_80078E00 data table (stride 132 bytes).
 * @return Computed value as s16: a0*field0 + a0*a0*10/field1 + field2.
 * @note Uses fields at offsets 0xF8C (linear coeff), 0xF8D (quadratic divisor), 0xF8E (constant).
 */
s32 func_8002172C(s32 a0, s32 a1) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    s32 entry = base + a1 * 132;
    u8 field1 = *(u8 *)(entry + 0xF8D);
    u8 field0 = *(u8 *)(entry + 0xF8C);
    u8 field2 = *(u8 *)(entry + 0xF8E);
    return (s16)(a0 * field0 + a0 * a0 * 10 / field1 + field2);
}


/**
 * @brief Compute a stat value for an entity by looking up parameters from two data tables.
 * @param a0 Entity index into D_80078720 (stride 464 bytes).
 * @param a1 Level or modifier value passed to func_80021628.
 * @return Result of func_80021628 using two u8 fields from D_80078E00 indexed by a secondary ID.
 * @note Reads a sub-index from D_80078720 offset 0x1C3, then uses it to look up a D_80078E00 entry (stride 36).
 */
s32 func_8002178C(s32 a0, s32 a1) {
    extern u8 D_80078720[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80078720;
    s32 entry = base1 + a0 * 464;
    s32 base2;
    u8 idx;
    base2 = (s32)D_80078E00;
    idx = *(u8 *)(entry + 0x1C3);
    return func_80021628(a1, *(u8 *)(base2 + idx * 36 + 0x37AA), *(u8 *)(base2 + idx * 36 + 0x37AB));
}


INCLUDE_ASM("asm/nonmatchings/11D3C", func_800217F4);


INCLUDE_ASM("asm/nonmatchings/11D3C", func_80021894);


/**
 * @brief Search a character's junction list for a specific junction ID and return its value.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @param a1 Junction ID to search for; returns 0 immediately if a1 is 0.
 * @return The junction value (byte at offset +1 from matching entry), or 0 if not found.
 * @note Scans up to 32 entries (2 bytes each: ID at +0x4A0, value at +0x4A1).
 */
s32 func_80021944(a0, a1)

s32 a0;
s32 a1;
{
    extern u8 g_gameState[];
    s32 i = 0;
    s32 base;

    if (a1 == 0) return 0;

    base = (s32)g_gameState;
    a0 = a0 * 152;
    while (i < 32) {
        u8 *ptr = (u8 *)(a0 + base);
        if (ptr[0x4A0] == a1) {
            return ptr[0x4A1];
        }
        a0 += 2;
        i++;
    }
    return 0;
}


/**
 * @brief Multiply two signed 32-bit integers.
 * @param a0 First operand.
 * @param a1 Second operand.
 * @return Product of a0 and a1.
 */
s32 func_800219A8(s32 a0, s32 a1) {
    return a0 * a1;
}


INCLUDE_ASM("asm/nonmatchings/11D3C", func_800219B8);


/**
 * @brief Compute an elemental or status modifier percentage from a character's equipped abilities.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @param a1 Element or status type to match against.
 * @return Base value of 100 plus any matching ability bonuses from D_80078E00 table.
 * @note Checks 4 ability slots (offsets 0x4E4..0x4E7). Abilities in range 0x27..0x39 are looked up
 *       in D_80078E00 (stride 8, offset 0x421D for type, 0x421E for bonus).
 */
s32 func_800219E0(s32 a0, s32 a1) {
    extern u8 g_gameState[];
    extern u8 D_80078E00[];
    s32 result = 100;
    s32 i = 0;
    s32 base1 = (s32)g_gameState;
    s32 base2;
    s32 off = base1 + a0 * 152;
    base2 = (s32)D_80078E00;
    do {
        s32 val = *(u8 *)(off + i + 0x4E4);
        s32 idx = val - 0x27;
        if ((u32)idx < 0x13) {
            s32 entry = idx * 8 + base2;
            if (*(u8 *)(entry + 0x421D) == a1) {
                result += *(u8 *)(entry + 0x421E);
            }
        }
        i++;
    } while (i < 4);
    return result;
}


INCLUDE_ASM("asm/nonmatchings/11D3C", func_80021A64);


INCLUDE_ASM("asm/nonmatchings/11D3C", func_80021B58);


INCLUDE_ASM("asm/nonmatchings/11D3C", func_80021C10);


/**
 * @brief Compute a derived stat (likely magic power) for a character, clamped to 0-255.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @param a1 Modifier or stat type passed to func_80021B58.
 * @return Clamped u8 result combining a base value, a junction bonus, and a level-based lookup.
 * @note Reads GF/junction index from offset 0x4F3, looks up base value from D_80078E00 (stride 60),
 *       then adds a bonus from func_800219B8 scaled by junction quantity.
 */
s32 func_80022028(s32 a0, s32 a1) {
    extern u8 g_gameState[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)g_gameState;
    s32 entry = base1 + a0 * 152;
    s32 base2;
    u8 idx;
    u8 val;
    s32 result1;

    base2 = (s32)D_80078E00;
    idx = *(u8 *)(entry + 0x4F3);
    val = *(u8 *)(base2 + idx * 60 + 0x23A);

    result1 = func_80021B58(a0, a1);
    return func_800231B0(*(u8 *)(base2 + result1 * 12 + 0x35BF) + func_800219B8(val, func_80021944(a0, idx)));
}


/**
 * @brief Compute a derived stat (likely spirit/magic defense) for a character, clamped to 0-255.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @param a1 Base value that is divided by 4 before adding junction bonus.
 * @return Clamped u8 result combining (a1/4) with a junction-scaled bonus from D_80078E00.
 * @note Reads GF/junction index from offset 0x4F2, looks up multiplier from D_80078E00 (stride 60, offset 0x239).
 */
s32 func_800220E4(s32 a0, s32 a1) {
    extern u8 g_gameState[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)g_gameState;
    s32 entry = base1 + a0 * 152;
    s32 base2;
    u8 idx;
    u8 val;

    base2 = (s32)D_80078E00;
    idx = *(u8 *)(entry + 0x4F2);
    val = *(u8 *)(base2 + idx * 60 + 0x239);

    return func_800231B0((a1 >> 2) + func_800219B8(val, func_80021944(a0, idx)));
}


/**
 * @brief Get a base stat value for a character's junctioned GF.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return u8 value from D_80078E00 at stride 60, offset 0x23C for the character's GF index at slot 0x4F5.
 * @note Purpose uncertain -- appears to retrieve a GF compatibility or stat modifier base value.
 */
s32 func_8002216C(s32 a0) {
    extern u8 g_gameState[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)g_gameState;
    u8 idx = *(u8 *)(base1 + a0 * 152 + 0x4F5);
    s32 base2 = (s32)D_80078E00;
    return *(u8 *)(base2 + idx * 60 + 0x23C);
}


/**
 * @brief Compute a junction-scaled stat bonus for a character.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return Product of a GF multiplier (from D_80078E00 offset 0x23D) and the junction quantity.
 * @note Reads GF index from offset 0x4F5, then computes multiplier * func_80021944(a0, idx).
 */
s32 func_800221B4(s32 a0) {
    extern u8 g_gameState[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)g_gameState;
    s32 entry = base1 + a0 * 152;
    s32 base2;
    u8 idx;
    u8 val;

    base2 = (s32)D_80078E00;
    idx = *(u8 *)(entry + 0x4F5);
    val = *(u8 *)(base2 + idx * 60 + 0x23D);

    return func_800219B8(val, func_80021944(a0, idx));
}


INCLUDE_ASM("asm/nonmatchings/11D3C", func_80022228);


/**
 * @brief Get the lower 7 bits of a GF's flags field for a character.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return Lower 7 bits (0x7F mask) of a u16 flags field from D_80078E00 at offset 0x242.
 * @note Reads GF index from character slot offset 0x4F6, then looks up flags in GF data (stride 60).
 */
s32 func_80022328(s32 a0) {
    extern u8 g_gameState[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)g_gameState;
    u8 idx = *(u8 *)(base1 + a0 * 152 + 0x4F6);
    s32 base2 = (s32)D_80078E00;
    return *(u16 *)(base2 + idx * 60 + 0x242) & 0x7F;
}


/**
 * @brief Decode a GF's status immunity/attribute flags into a game-engine bitmask.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return Bitmask with remapped flag bits from the GF's u16 flags field at D_80078E00 offset 0x242.
 * @note Maps source bits to result bits: 0x80->0x1, 0x100->0x4, 0x200->0x8,
 *       0x400->0x200, 0x800->0x4000, 0x1000->0x8000.
 */
s32 func_80022370(s32 a0) {
    extern u8 g_gameState[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)g_gameState;
    s32 base2;
    u8 idx;
    u16 flags;
    s32 val;
    s32 result;

    idx = *(u8 *)(base1 + a0 * 152 + 0x4F6);
    base2 = (s32)D_80078E00;
    flags = *(u16 *)(base2 + idx * 60 + 0x242);
    val = flags & 0x80;
    result = val != 0;
    if (flags & 0x100) result |= 0x4;
    if (flags & 0x200) result |= 0x8;
    if (flags & 0x400) result |= 0x200;
    if (flags & 0x800) result |= 0x4000;
    if (flags & 0x1000) result |= 0x8000;
    return result;
}


/**
 * @brief Compute a hit/accuracy percentage for a character, starting at 100 and adding a junction bonus.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return 100 + junction-scaled bonus from GF index at offset 0x4F6, D_80078E00 offset 0x240.
 * @note Calls func_800219B8 to scale the GF multiplier by junction quantity.
 */
s32 func_80022404(s32 a0) {
    extern u8 g_gameState[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)g_gameState;
    s32 entry = base1 + a0 * 152;
    s32 base2;
    u8 idx;
    u8 val;

    base2 = (s32)D_80078E00;
    idx = *(u8 *)(entry + 0x4F6);
    val = *(u8 *)(base2 + idx * 60 + 0x240);

    return func_800219B8(val, func_80021944(a0, idx)) + 100;
}


INCLUDE_ASM("asm/nonmatchings/11D3C", func_8002247C);


INCLUDE_ASM("asm/nonmatchings/11D3C", func_8002257C);


INCLUDE_ASM("asm/nonmatchings/11D3C", func_8002274C);


