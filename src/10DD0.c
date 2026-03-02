#include "common.h"
#include "psxsdk/libgpu.h"

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800205D0);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020608);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020644);


/** @brief Sets bit 0x1 in byte at D_80077378[a0 * 68 + 0x61].
 *  @param a0 Table index (stride 68 bytes).
 */
void func_80020670(s32 a0) {
    extern u8 D_80077378[];
    s32 base = (s32)D_80077378;
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


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020FBC);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020FEC);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021034);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021108);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800211B4);


/**
 * @brief Check if a character's ability list contains a specific ability ID.
 * @param a0 Character index used to look up a slot ID in D_80077378 at offset 0xAF4.
 * @param a1 Ability ID to search for; returns 0 immediately if a1 is 0.
 * @return 1 if the ability is found in the character's 20-entry ability list, 0 otherwise.
 */
s32 func_80021290(a0, a1)

s32 a0;
s32 a1;
{
    extern u8 D_80077378[];
    s32 base;
    u8 slot_id;
    s32 i;
    s32 ptr;

    if (a1 == 0) return 0;

    base = (s32)D_80077378;
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
 * @param a0 Character index used to resolve a slot ID via D_80077378.
 * @param a1 Amount to add to the stat.
 * @note The stat at ptr+2 (likely HP or experience) is read as u16, added to a1, then clamped by func_800231C8.
 */
void func_800214E0(s32 a0, s32 a1) {
    extern u8 D_80077378[];
    u8 *base = D_80077378 + a0;
    u8 idx = base[0xAF4];
    u8 *ptr = D_80077378 + 0x490 + idx * 152;
    *(s16 *)(ptr + 2) = func_800231C8(*(u16 *)(ptr + 2) + a1);
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002153C);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021628);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002166C);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800216B0);


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


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800217F4);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021894);


/**
 * @brief Search a character's junction list for a specific junction ID and return its value.
 * @param a0 Character slot index (stride 152 in D_80077378).
 * @param a1 Junction ID to search for; returns 0 immediately if a1 is 0.
 * @return The junction value (byte at offset +1 from matching entry), or 0 if not found.
 * @note Scans up to 32 entries (2 bytes each: ID at +0x4A0, value at +0x4A1).
 */
s32 func_80021944(a0, a1)

s32 a0;
s32 a1;
{
    extern u8 D_80077378[];
    s32 i = 0;
    s32 base;

    if (a1 == 0) return 0;

    base = (s32)D_80077378;
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


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800219B8);


/**
 * @brief Compute an elemental or status modifier percentage from a character's equipped abilities.
 * @param a0 Character slot index (stride 152 in D_80077378).
 * @param a1 Element or status type to match against.
 * @return Base value of 100 plus any matching ability bonuses from D_80078E00 table.
 * @note Checks 4 ability slots (offsets 0x4E4..0x4E7). Abilities in range 0x27..0x39 are looked up
 *       in D_80078E00 (stride 8, offset 0x421D for type, 0x421E for bonus).
 */
s32 func_800219E0(s32 a0, s32 a1) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 result = 100;
    s32 i = 0;
    s32 base1 = (s32)D_80077378;
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


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021A64);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021B58);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021C10);


/**
 * @brief Compute a derived stat (likely magic power) for a character, clamped to 0-255.
 * @param a0 Character slot index (stride 152 in D_80077378).
 * @param a1 Modifier or stat type passed to func_80021B58.
 * @return Clamped u8 result combining a base value, a junction bonus, and a level-based lookup.
 * @note Reads GF/junction index from offset 0x4F3, looks up base value from D_80078E00 (stride 60),
 *       then adds a bonus from func_800219B8 scaled by junction quantity.
 */
s32 func_80022028(s32 a0, s32 a1) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80077378;
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
 * @param a0 Character slot index (stride 152 in D_80077378).
 * @param a1 Base value that is divided by 4 before adding junction bonus.
 * @return Clamped u8 result combining (a1/4) with a junction-scaled bonus from D_80078E00.
 * @note Reads GF/junction index from offset 0x4F2, looks up multiplier from D_80078E00 (stride 60, offset 0x239).
 */
s32 func_800220E4(s32 a0, s32 a1) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80077378;
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
 * @param a0 Character slot index (stride 152 in D_80077378).
 * @return u8 value from D_80078E00 at stride 60, offset 0x23C for the character's GF index at slot 0x4F5.
 * @note Purpose uncertain -- appears to retrieve a GF compatibility or stat modifier base value.
 */
s32 func_8002216C(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80077378;
    u8 idx = *(u8 *)(base1 + a0 * 152 + 0x4F5);
    s32 base2 = (s32)D_80078E00;
    return *(u8 *)(base2 + idx * 60 + 0x23C);
}


/**
 * @brief Compute a junction-scaled stat bonus for a character.
 * @param a0 Character slot index (stride 152 in D_80077378).
 * @return Product of a GF multiplier (from D_80078E00 offset 0x23D) and the junction quantity.
 * @note Reads GF index from offset 0x4F5, then computes multiplier * func_80021944(a0, idx).
 */
s32 func_800221B4(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80077378;
    s32 entry = base1 + a0 * 152;
    s32 base2;
    u8 idx;
    u8 val;

    base2 = (s32)D_80078E00;
    idx = *(u8 *)(entry + 0x4F5);
    val = *(u8 *)(base2 + idx * 60 + 0x23D);

    return func_800219B8(val, func_80021944(a0, idx));
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80022228);


/**
 * @brief Get the lower 7 bits of a GF's flags field for a character.
 * @param a0 Character slot index (stride 152 in D_80077378).
 * @return Lower 7 bits (0x7F mask) of a u16 flags field from D_80078E00 at offset 0x242.
 * @note Reads GF index from character slot offset 0x4F6, then looks up flags in GF data (stride 60).
 */
s32 func_80022328(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80077378;
    u8 idx = *(u8 *)(base1 + a0 * 152 + 0x4F6);
    s32 base2 = (s32)D_80078E00;
    return *(u16 *)(base2 + idx * 60 + 0x242) & 0x7F;
}


/**
 * @brief Decode a GF's status immunity/attribute flags into a game-engine bitmask.
 * @param a0 Character slot index (stride 152 in D_80077378).
 * @return Bitmask with remapped flag bits from the GF's u16 flags field at D_80078E00 offset 0x242.
 * @note Maps source bits to result bits: 0x80->0x1, 0x100->0x4, 0x200->0x8,
 *       0x400->0x200, 0x800->0x4000, 0x1000->0x8000.
 */
s32 func_80022370(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80077378;
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
 * @param a0 Character slot index (stride 152 in D_80077378).
 * @return 100 + junction-scaled bonus from GF index at offset 0x4F6, D_80078E00 offset 0x240.
 * @note Calls func_800219B8 to scale the GF multiplier by junction quantity.
 */
s32 func_80022404(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80077378;
    s32 entry = base1 + a0 * 152;
    s32 base2;
    u8 idx;
    u8 val;

    base2 = (s32)D_80078E00;
    idx = *(u8 *)(entry + 0x4F6);
    val = *(u8 *)(base2 + idx * 60 + 0x240);

    return func_800219B8(val, func_80021944(a0, idx)) + 100;
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002247C);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002257C);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002274C);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800227F4);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800228F4);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002293C);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800229FC);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80022B04);


/**
 * @brief Accumulate status immunity flags as an RGB-packed bitmask from a character's equipped abilities.
 * @param a0 Character slot index (stride 152 in D_80077378).
 * @return Packed bitmask: (B << 16) | (G << 8) | R, OR'd across up to 4 matching ability slots.
 * @note Checks 4 ability slots (offsets 0x4E4..0x4E7). Abilities in range 0x3A..0x4D are looked up
 *       in D_80078E00 (stride 8, offsets 0x42B5/B6/B7 for R/G/B status immunity bytes).
 */
s32 func_80022B48(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 result = 0;
    s32 i = 0;
    s32 base1 = (s32)D_80077378;
    s32 base2;
    s32 off = base1 + a0 * 152;
    base2 = (s32)D_80078E00;
    do {
        s32 val = *(u8 *)(off + i + 0x4E4);
        s32 idx = val - 0x3A;
        if ((u32)idx < 0x14) {
            s32 entry = idx * 8 + base2;
            u8 b = *(u8 *)(entry + 0x42B7);
            u8 g = *(u8 *)(entry + 0x42B6);
            u8 r = *(u8 *)(entry + 0x42B5);
            result |= (b << 16) | (g << 8) | r;
        }
        i++;
    } while (i < 4);
    return result;
}


/**
 * @brief Check if any of 4 command slots has command type 6 (likely "Magic" or a special command).
 * @param a0 Pointer to an array of command entries (4 bytes each, type field at offset 0x1E).
 * @return 1 if any slot has type == 6, 0 otherwise.
 */
s32 func_80022BD0(u8 *a0) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (a0[0x1E] == 6) return 1;
        a0 += 4;
    }
    return 0;
}


/**
 * @brief Determine a character's magic availability flags based on status and command slots.
 * @param a0 Pointer to a character data structure.
 * @return Flags: bit 0 set if status bit 0x20000 is active; bit 1 set if magic commands present
 *         (unless D_80082C10 bit 3 is set, which suppresses the magic flag).
 */
s32 func_80022C04(s32 a0) {
    extern u8 D_80082C10;
    s32 val = *(s32 *)(a0 + 0x190);
    s32 masked = val & 0x20000;
    s32 flag = masked != 0;
    if (func_80022BD0(a0)) {
        if (D_80082C10 & 8) {
            return flag;
        }
        flag |= 2;
    }
    return flag;
}


/**
 * @brief Apply party ability flags from a character's equipped abilities to D_80078720.
 * @param a0 Character slot index (stride 152 in D_80077378).
 * @note Checks 4 ability slots (offsets 0x4E4..0x4E7). Abilities in range 0x4E..0x52 are looked up
 *       in D_80078E00 (stride 8, offset 0x4355) and OR'd into D_80078720 offset 0x6D8.
 *       Likely enables field/world abilities (e.g., encounter-none, rare-item).
 */
void func_80022C5C(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    extern u8 D_80078720[];
    s32 i = 0;
    s32 base1 = (s32)D_80077378;
    s32 base3;
    s32 base2;
    s32 off = base1 + a0 * 152;
    base3 = (s32)D_80078720;
    base2 = (s32)D_80078E00;
    do {
        s32 val = *(u8 *)(off + i + 0x4E4);
        s32 idx = val - 0x4E;
        if ((u32)idx < 5) {
            *(u8 *)(base3 + 0x6D8) |= *(u8 *)(idx * 8 + base2 + 0x4355);
        }
        i++;
    } while (i < 4);
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80022CDC);


/**
 * @brief Clear a character's magic, item, and command slot data.
 * @param a0 Pointer to a character data structure.
 * @note Zeroes out three arrays: 32 entries of 5 bytes at offset 0x82 (magic inventory),
 *       16 entries of 5 bytes at offset 0x122 (item inventory), 4 entries of 4 bytes at
 *       offset 0x1E (command slots), plus fields at 0x1C, 0x1D, and a u16 at 0x14.
 */
void func_80022D78(s32 a0) {
    s32 i;

    for (i = 0; i < 0x20; i++) {
        *(u8 *)(a0 + i * 5 + 0x84) = 0;
        *(u8 *)(a0 + i * 5 + 0x85) = 0;
        *(u8 *)(a0 + i * 5 + 0x86) = 0;
        *(u8 *)(a0 + i * 5 + 0x83) = 0;
        *(u8 *)(a0 + i * 5 + 0x82) = 0;
    }

    for (i = 0; i < 0x10; i++) {
        *(u8 *)(a0 + i * 5 + 0x124) = 0;
        *(u8 *)(a0 + i * 5 + 0x125) = 0;
        *(u8 *)(a0 + i * 5 + 0x126) = 0;
        *(u8 *)(a0 + i * 5 + 0x123) = 0;
        *(u8 *)(a0 + i * 5 + 0x122) = 0;
    }

    for (i = 0; i < 4; i++) {
        *(u8 *)(a0 + i * 4 + 0x20) = 0;
        *(u8 *)(a0 + i * 4 + 0x21) = 0;
        *(u8 *)(a0 + i * 4 + 0x1F) = 0;
        *(u8 *)(a0 + i * 4 + 0x1E) = 0;
    }

    *(u8 *)(a0 + 0x1C) = 0;
    *(u8 *)(a0 + 0x1D) = 0;
    *(u16 *)(a0 + 0x14) = 0;
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80022E08);


/**
 * @brief Find the index of a command slot matching a given command type.
 * @param a0 Pointer to an array of command entries (4 bytes each, type field at offset 0x1E).
 * @param a1 Command type to search for.
 * @return Index (0-3) of the matching slot, or 0xFF if not found.
 */
s32 func_80023180(u8 *a0, s32 a1) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (a0[0x1E] == a1) return i;
        a0 += 4;
    }
    return 0xFF;
}


/**
 * @brief Clamp a value to the 0-255 (u8) range.
 * @param a0 Value to clamp.
 * @return a0 if < 256, otherwise 255.
 */
s32 func_800231B0(s32 a0) {
    if (a0 >= 0x100) {
        return 0xFF;
    }
    return a0;
}


/**
 * @brief Clamp a value to the 0-9999 range.
 * @param a0 Value to clamp.
 * @return a0 as u16 if < 10000, otherwise 9999.
 * @note Commonly used to cap HP or similar stats at the FF8 maximum of 9999.
 */
s32 func_800231C8(s32 a0) {
    if (a0 >= 0x2710) {
        return 0x270F;
    }
    return (u16)a0;
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800231E0);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002363C);


/**
 * @brief Iterate over 16 GF entries and recalculate stats for each active one.
 * @note Checks D_80077378 entries (stride 0x44) for bit 0 of field 0x61 (active flag).
 *       Calls func_8002363C for each active GF to recalculate its derived stats.
 */
void func_80023828(void) {
    extern u8 D_80077378[];
    s32 i = 0;
    u8 *ptr = D_80077378;
    do {
        if (ptr[0x61] & 1) {
            func_8002363C(i);
        }
        i++;
        ptr += 0x44;
    } while (i < 16);
}


/**
 * @brief Recalculate stats for all 3 party members and their GFs.
 * @note Resets D_80078DF8, then for each of the 3 party slots calls func_80022E08 and
 *       func_800231E0 to recalculate character stats. Finally calls func_80023828 for GFs.
 */
void func_80023888(void) {
    extern u8 D_80078DF8;
    extern u8 D_80077378[];
    s32 i;
    s32 base;
    s32 ptr;

    D_80078DF8 = 0;
    i = 0;
    base = (s32)D_80077378;
    do {
        ptr = i + base;
        func_80022E08(*(u8 *)(ptr + 0xAF4), i);
        func_800231E0(*(u8 *)(ptr + 0xAF4), i);
        i++;
    } while (i < 3);
    func_80023828();
}


extern u8 D_80078658;

/** @brief Return a pointer to the global item/key item inventory array D_80078658. */
u8 *func_80023900(void) {
    return &D_80078658;
}

/**
 * @brief Modify an inventory item quantity or set a key item flag.
 * @param a0 Item slot index. Slots < 0x4D are consumable items; >= 0x4D are key items.
 * @param a1 For consumable items: 0xF0 to increment, anything else to decrement.
 *           For key items: value to set directly.
 * @return 0 on success, -1 if increment/decrement would overflow (max 100) or underflow (min 0).
 * @note Item quantities stored in lower 7 bits (0x7F mask). Upper bit may be a flag.
 */
s32 func_8002390C(a0, a1)

s32 a0;
s32 a1;
{
    u8 *base = func_80023900();
    u8 *ptr;
    u8 val;

    if (a0 < 0x4D) {
        ptr = &base[a0];
        if (a1 == 0xF0) {
            val = *ptr;
            if ((val & 0x7F) < 100) {
                *ptr = val + 1;
            } else {
                return -1;
            }
        } else {
            val = *ptr;
            if (val & 0x7F) {
                *ptr = val - 1;
            } else {
                return -1;
            }
        }
    } else {
        base[a0] = (u8)a1;
    }
    return 0;
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800239A8);


/** @brief Wrapper that calls func_8002390C with a1=0. */
s32 func_80023A34(s32 a0) {
    return func_8002390C(a0, 0);
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023A54);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023A88);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023B14);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023BB4);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023C08);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023C48);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023D04);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023D60);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80024064);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800242C8);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80025920);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800259C0);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80025D80);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80025E4C);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80025F58);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800262B0);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800263DC);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026ADC);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026CA0);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026CF0);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026D10);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026D8C);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026E20);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026E70);


/**
 * @brief Open a thread with interrupt protection.
 * @param a0 Thread program counter / entry point address.
 * @param a1 Thread stack pointer.
 * @return Thread handle from OpenTh.
 * @note Wraps PsyQ OpenTh with func_800472E4/func_800472F4 (likely interrupt disable/enable).
 */
s32 func_80026EC4(s32 a0, s32 a1) {
    s32 result;
    func_800472E4(a0);
    result = OpenTh(a0, a1, 0);
    func_800472F4();
    return result;
}


/**
 * @brief Close a thread with interrupt protection.
 * @param a0 Thread handle to close.
 * @note Wraps PsyQ CloseTh with func_800472E4/func_800472F4 (likely interrupt disable/enable).
 */
void func_80026F14(s32 a0) {
    func_800472E4(a0);
    CloseTh(a0);
    func_800472F4();
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026F4C);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026F90);


/** @brief Wrapper that calls func_80047384 (likely returns interrupt/thread status). */
void func_80026FB4(void) { func_80047384(); }


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026FD4);


/**
 * @brief Switch to a thread, using a fallback address if a0 is 0.
 * @param a0 Thread handle to switch to; 0 defaults to 0xFF000000.
 * @note If func_80047384 returns bit 2 set, uses func_80026F4C instead of PsyQ ChangeTh.
 */
void func_80026FE0(s32 a0) {
    if (a0 == 0) {
        a0 = (s32)0xFF000000;
    }
    if (func_80047384() & 4) {
        func_80026F4C(a0);
    } else {
        ChangeTh(a0);
    }
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027038);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800270B0);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027220);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027360);


extern u8 D_80082DD0[];

/**
 * @brief Mark a D_80082DD0 entry as active by setting fields 0x19 and 0x0A to 1.
 * @param idx Index into D_80082DD0 array (stride 196 bytes).
 * @note Purpose uncertain -- appears to enable or activate an entity entry.
 */
void func_800273D8(s32 idx) {
    u8 *entry = D_80082DD0 + idx * 196;
    entry[0x19] = 1;
    entry[0x0A] = 1;
}


/**
 * @brief Initialize two data blocks for a structure at address a0 via func_80027220.
 * @param a0 Base address of the structure to initialize.
 * @note Initializes two regions: one at the base with mode 0, and one at offset 0xC4 with mode 0x10.
 */
void func_80027408(s32 a0) {
    func_80027220(a0, a0, 0);
    func_80027220(a0, a0 + 0xC4, 0x10);
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027448);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027558);


/** @brief Returns byte at D_80082DD0[a0 * 196 + 0x0B].
 *  @param a0 Entity index (stride 196 bytes).
 */
s32 func_800275A8(s32 a0) {
    extern u8 D_80082DD0[];
    u8 *entry = D_80082DD0 + a0 * 196;
    return entry[0x0B];
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800275D4);


/**
 * @brief Get an animation frame value from a D_80082DD0 entry's sub-table.
 * @param a0 Entity index (masked to 0 or 1).
 * @param a1 Frame offset subtracted from the current frame counter.
 * @return u16 value from a 20-byte-stride sub-table at offset 0x1E within the linked entry.
 * @note Resolves a secondary entry via field 0xC2, then indexes into its sub-table using
 *       (current_frame - a1) & 7 as a circular buffer index.
 */
u16 func_8002795C(s32 a0, s32 a1) {
    u8 *base;
    u8 *entry;
    u8 *entry2;
    s32 sub_idx;
    a0 &= 1;
    base = D_80082DD0;
    entry = base + a0 * 196;
    entry2 = base + entry[0xC2] * 196;
    sub_idx = (entry2[0x18] - a1) & 7;
    return *(u16 *)(entry2 + sub_idx * 20 + 0x1E);
}


/**
 * @brief Get a combined flags/status value by OR'ing 4 u16 fields from a D_80082DD0 sub-table entry.
 * @param a0 Entity index (masked to 0 or 1).
 * @param a1 Frame offset subtracted from the current frame counter.
 * @return Bitwise OR of u16 fields at offsets +8, +0xA, +0xC, +0xE within the sub-table entry.
 * @note Same sub-table lookup as func_8002795C but reads 4 adjacent u16 values instead of one.
 */
u16 func_800279CC(s32 a0, s32 a1) {
    u8 *base;
    u8 *entry;
    u8 *entry2;
    s32 sub_idx;
    s32 off;
    u8 *p;
    a0 &= 1;
    base = D_80082DD0;
    entry = base + a0 * 196;
    entry2 = base + entry[0xC2] * 196;
    sub_idx = (entry2[0x18] - a1) & 7;
    off = sub_idx * 20 + 0x1C;
    p = entry2 + off;
    return *(u16 *)(p + 8) | *(u16 *)(p + 0xA) | *(u16 *)(p + 0xC) | *(u16 *)(p + 0xE);
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027A58);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027AC8);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027B38);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027B7C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027BA8);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027C00);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027C90);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027CF8);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027DB4);


/** @brief Returns byte at D_80082DD0[(a0 & 1) * 196 + 0x1B].
 *  @param a0 Entity index, masked to 0 or 1.
 */
s32 func_80027EC8(s32 a0) {
    extern u8 D_80082DD0[];
    u8 *entry = D_80082DD0 + (a0 & 1) * 196;
    return entry[0x1B];
}


/**
 * @brief Set or clear the opacity/visibility field (0x1B) of a D_80082DD0 entry.
 * @param a0 Entity index (masked to 0 or 1).
 * @param a1 If nonzero, set field 0x1B to 0xFF (fully visible); otherwise set to 0 (hidden).
 */
void func_80027EF8(s32 a0, s32 a1) {
    extern u8 D_80082DD0[];
    u8 *entry = D_80082DD0 + (a0 & 1) * 196;
    if (a1 != 0) {
        entry[0x1B] = 0xFF;
    } else {
        entry[0x1B] = 0;
    }
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027F38);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027F78);


/** @brief Stores two u16 values into D_80082DD0 at (a0 & 1) * 4.
 *  @param a0 Entity index, masked to 0 or 1.
 *  @param a1 Value stored at offset 0x1D0.
 *  @param a2 Value stored at offset 0x1D2.
 */
void func_80027FBC(s32 a0, s16 a1, s16 a2) {
    extern u8 D_80082DD0[];
    s32 base;
    a0 &= 1;
    base = (s32)D_80082DD0;
    *(u16 *)(base + a0 * 4 + 0x1D0) = a1;
    *(u16 *)(base + a0 * 4 + 0x1D2) = a2;
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027FDC);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80028098);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800280C0);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800281A4);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800281C4);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002828C);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800282F4);


extern u16 D_80083794;
// get D_80083794 (u16)

/** @brief Initializes D_80082FB2 to 0, calls func_80039764(0), then loops twice calling func_800281C4(i, 0). */
void func_8002837C(void) {
    extern u8 D_80082FB2;
    s32 i;
    D_80082FB2 = 0;
    func_80039764(0);
    for (i = 0; i < 2; i++) {
        func_800281C4(i, 0);
    }
}


/** @brief Get the current value of D_80083794 (global u16 state variable). */
u16 func_800283CC(void) {
    return D_80083794;
}


extern u8 D_80082FB3;

/**
 * @brief Set D_80083794 to a new value.
 * @param a0 Value to store.
 * @return The value that was set.
 */
s32 func_800283DC(s32 a0) {
    D_80083794 = a0;
    return a0;
}


/**
 * @brief Set the global flag D_80082FB3.
 * @param a0 Value to store in D_80082FB3.
 */
void func_800283EC(s32 a0) {
    D_80082FB3 = a0;
}


/**
 * @brief Initialize or reset the CD audio/streaming subsystem state.
 * @note Calls several initialization functions and resets a counter at D_80082DD0 + 0x9C4 to 0.
 *       Passes two D_80082DD0 buffer pointers (offsets 0x188 and 0x1AC) to func_8003BC24.
 */
void func_800283F8(void) {
    extern u8 D_80082DD0[];
    u8 *base = D_80082DD0;
    func_800982B8();
    func_8003BC24(base + 0x188, base + 0x1AC);
    func_8003AB84();
    func_800282F4();
    *(s16 *)(base + 0x9C4) = 0;
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80028444);


/** @brief Calls func_8002A128, func_800286FC, and func_8002A090 in sequence. */
void func_8002848C(void) {
    func_8002A128();
    func_800286FC();
    func_8002A090();
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800284BC);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80028564);


/** @brief Wrapper that calls func_80050BC4. */
void func_800286BC(void) {
    func_80050BC4();
}


/** @brief Wrapper that calls func_8004E720. */
void func_800286DC(void) {
    func_8004E720();
}


/** @brief Wrapper that calls func_8004DFF4 (likely a CD subsystem tick or finalization). */
void func_800286FC(void) { func_8004DFF4(); }


/**
 * @brief Copy a null-terminated string from src to dst (strcpy implementation).
 * @param dst Destination buffer.
 * @param src Source null-terminated string.
 */
void func_8002871C(u8 *dst, u8 *src) {
    s32 c;
    do {
        c = *src++;
        *dst++ = c;
    } while (c != 0);
}


/**
 * @brief Append a null-terminated string to dst (strcat implementation).
 * @param dst Destination buffer containing an existing null-terminated string.
 * @param src Source null-terminated string to append.
 */
void func_80028738(u8 *dst, u8 *src) {
    do { } while (*dst++);
    func_8002871C(dst - 1, src);
}


/**
 * @brief Copy a block of bytes from src to dst (memcpy implementation).
 * @param src Source buffer.
 * @param dst Destination buffer.
 * @param numBytes Number of bytes to copy.
 */
void func_80028768(u8 *src, u8 *dst, s32 numBytes) {
    while (numBytes > 0) {
        *dst++ = *src++;
        numBytes--;
    }
}


/**
 * @brief Wait for vertical sync (VSync wrapper).
 * @param a0 Unused parameter.
 */
void func_80028790(s32 a0) { VSync(); }


extern s8 D_80082FD4;
/**
 * @brief Set the global flag D_80082FD4.
 * @param val Value to store in D_80082FD4.
 */
void func_800287B0(s8 val) {
    D_80082FD4 = val;
}


/**
 * @brief Encode a memory card port and slot into a single byte.
 * @param a0 Memory card port number (reduced mod 2: 0 or 1).
 * @param a1 Memory card slot number (reduced mod 4: 0-3).
 * @return Packed value: (port << 4) | slot.
 */
s32 func_800287BC(s32 a0, s32 a1) {
    a1 %= 4;
    a0 %= 2;
    return (a0 << 4) | a1;
}


/**
 * @brief Extract the memory card port number from a packed card identifier.
 * @param a0 Packed card identifier (port in bit 4).
 * @return Port number (0 or 1).
 */
s32 func_800287F4(s32 a0) {
    return (a0 >> 4) % 2;
}


/**
 * @brief Extract the memory card slot number from a packed card identifier.
 * @param a0 Packed card identifier (slot in lower 2 bits).
 * @return Slot number (0-3).
 */
s32 func_80028810(s32 a0) {
    return a0 % 4;
}


/**
 * @brief Wait for a memory card operation to complete on the specified port.
 * @param a0 Packed card identifier; port is extracted via func_800287F4.
 */
void func_8002882C(s32 a0) {
    _card_wait(func_800287F4(a0));
}


/**
 * @brief Test a BIOS event (wrapper for PsyQ TestEvent).
 * @param a0 Event descriptor to test.
 * @return Nonzero if the event has been delivered.
 */
s32 func_80028854(s32 a0) { TestEvent(a0); }


/**
 * @brief Poll the first 4 memory card events and return which one fired.
 * @return Event index 0-3 if an event was delivered, or -1 if none fired.
 * @note Tests events from D_80082FB4 array entries [0..3] in order.
 */
s32 func_80028874(void) {
    extern u8 D_80082FB4[];
    s32 *s0 = (s32 *)((s32)D_80082FB4);
    if (func_80028854(s0[0])) return 0;
    if (func_80028854(s0[1])) return 1;
    if (func_80028854(s0[2])) return 2;
    if (func_80028854(s0[3])) return 3;
    return -1;
}


/**
 * @brief Wait up to 180 frames (0xB4) for a memory card event, polling each VSync.
 * @return Event index 0-3 if an event fires, or 2 (timeout) if none fires within the limit.
 */
s32 func_800288F8(void) {
    s32 i;
    s32 result;
    for (i = 0; i < 0xB4; i++) {
        result = func_80028874();
        if (result != -1) return result;
        func_80028790(0);
    }
    return 2;
}


/** @brief Poll memory card events once (discards result). */
void func_80028950(void) { func_80028874(); }


/**
 * @brief Poll the second set of 4 memory card events and return which one fired.
 * @return Event index 0-3 if an event was delivered, or -1 if none fired.
 * @note Tests events from D_80082FB4 array entries [4..7] in order.
 */
s32 func_80028970(void) {
    extern u8 D_80082FB4[];
    s32 *s0 = (s32 *)((s32)D_80082FB4);
    if (TestEvent(s0[4])) return 0;
    if (TestEvent(s0[5])) return 1;
    if (TestEvent(s0[6])) return 2;
    if (TestEvent(s0[7])) return 3;
    return -1;
}


/**
 * @brief Busy-wait up to 16384 iterations for a secondary memory card event.
 * @return Event index 0-3 if an event fires, or 2 (timeout) if none fires within the limit.
 * @note Unlike func_800288F8, this does not VSync between polls (tight busy-wait).
 */
s32 func_800289F8(void) {
    s32 i;
    s32 result;
    for (i = 0; i < 0x4000; i++) {
        result = func_80028970();
        if (result != -1) return result;
    }
    return 2;
}


/**
 * @brief Read a memory card status byte for a given port and slot.
 * @param a0 Packed card identifier (port in bit 4, slot in lower bits).
 * @return Status byte from D_80082FB4 at offset 0x2C + port*4 + slot.
 */
u8 func_80028A44(a0)

s32 a0;
{
    extern u8 D_80082FB4[];
    s32 base = (s32)D_80082FB4;
    s32 r1 = func_800287F4(a0);
    s32 r2 = func_80028810(a0);
    return *(u8 *)(r1 * 4 + base + r2 + 0x2C);
}


/**
 * @brief Write a memory card status byte for a given port and slot.
 * @param a0 Packed card identifier (port in bit 4, slot in lower bits).
 * @param a1 Status value to write.
 */
void func_80028A98(s32 a0, u8 a1) {
    extern u8 D_80082FB4[];
    s32 base = (s32)D_80082FB4;
    s32 r1 = func_800287F4(a0);
    s32 r2 = func_80028810(a0);
    *(u8 *)(r1 * 4 + base + r2 + 0x2C) = a1;
}


/**
 * @brief Write a secondary memory card status byte for a given port and slot.
 * @param a0 Packed card identifier (port in bit 4, slot in lower bits).
 * @param a1 Status value to write at offset 0x34 (secondary status table).
 */
void func_80028AF8(s32 a0, u8 a1) {
    extern u8 D_80082FB4[];
    s32 base = (s32)D_80082FB4;
    s32 r1 = func_800287F4(a0);
    s32 r2 = func_80028810(a0);
    *(u8 *)(r1 * 4 + base + r2 + 0x34) = a1;
}


/** @brief Mark a memory card slot as busy/active (set status byte to 1). */
void func_80028B58(s32 a0) { func_80028A98(a0, 1); }


/** @brief Read memory card status (wrapper, discards result). */
void func_80028B78(void) { func_80028A44(); }


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80028B98);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80028CB4);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80028D20);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80028D80);


/**
 * @brief Attempt to load a memory card save, retrying up to 180 times.
 * @param a0 Packed card identifier.
 * @return 0 on success, 2 if new card detected, 3 on timeout, 4 on other failure.
 * @note Waits for the card, issues _card_load, then polls events. Updates card status
 *       bytes based on the result.
 */
s32 func_80028F3C(s32 a0) {
    s32 counter;

    counter = 0;
    do {
        func_8002882C(a0);
        if (_card_load(a0) != 0) {
            s32 result = func_800288F8();
            switch (result) {
            case 0:
                func_80028AF8(a0, 0);
                func_80028A98(a0, 0);
                return 0;
            case 3:
                func_80028AF8(a0, 1);
                func_80028B58(a0);
                return 2;
            case 2:
                func_80028B58(a0);
                return 3;
            default:
                func_80028B58(a0);
                return 4;
            }
        }
        counter++;
    } while (counter < 0xB4);
    return 4;
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029028);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800290C0);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029150);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800291FC);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800292AC);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029360);


/** @brief Returns the signed byte value at D_80082FD4. */
s32 func_800293F0(void) {
    extern s8 D_80082FD4;
    return D_80082FD4;
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029400);


/** @brief Wrapper that calls func_80029400. */
void func_8002947C(void) {
    func_80029400();
}


/**
 * @brief Close a file descriptor if it is valid (not -1).
 * @param a0 File descriptor to close.
 */
void func_8002949C(s32 a0) {
    if (a0 != -1) {
        close(a0);
    }
}


/** @brief Wrapper for func_8002949C (close a file descriptor if valid). */
void func_800294C4(s32 a0) {
    func_8002949C(a0);
}


/**
 * @brief Open the first file on a memory card matching the given parameters.
 * @param a0 Packed card identifier.
 * @param a1 File index or name parameter.
 * @param a2 Directory entry buffer for firstfile result.
 * @return 0 if card initialization failed, otherwise the result of firstfile.
 * @note Calls func_80028CB4 for setup, checks card status via func_80029150, builds
 *       the filename via func_80029360, then calls PsyQ firstfile.
 */
s32 func_800294E4(s32 a0, s32 a1, s32 a2) {
    s32 buf[8];
    func_80028CB4();
    if (func_80029150(a0) != 0) return 0;
    func_80029360(a0, a1, (s32)buf);
    firstfile((s32)buf, a2);
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029550);


/** @brief Advance to the next file in the memory card directory listing (PsyQ nextfile wrapper). */
void func_800295D0(void) { nextfile(); }


INCLUDE_ASM("asm/nonmatchings/10DD0", func_800295F0);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029660);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029680);


/**
 * @brief Seek to an offset in a file and write data.
 * @param a0 File descriptor.
 * @param a1 Pointer to data buffer to write.
 * @param a2 Number of bytes to write.
 * @param a3 File offset to seek to before writing.
 * @return Number of bytes written on success, -1 if the seek failed.
 */
s32 func_80029778(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result = lseek(a0, a3, 0);
    if (result == -1) return -1;
    return write(a0, a1, a2);
}


/**
 * @brief Seek to an offset in a file and read data.
 * @param a0 File descriptor.
 * @param a1 Pointer to destination buffer.
 * @param a2 Number of bytes to read.
 * @param a3 File offset to seek to before reading.
 * @return Number of bytes read on success, -1 if the seek failed.
 */
s32 func_800297E4(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result = lseek(a0, a3, 0);
    if (result == -1) return -1;
    return read(a0, a1, a2);
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029850);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029A20);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029BA0);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029C44);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029CB8);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029D38);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029DAC);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029E40);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029EE4);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029F5C);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029FDC);


/**
 * @brief Shut down the memory card subsystem by closing all 8 card events.
 * @note Disables interrupts via func_800472E4, closes all events in D_80082FB4[0..7],
 *       re-enables interrupts, then calls func_8004D968 for final cleanup.
 */
void func_8002A090(void) {
    extern s32 D_80082FB4[];
    func_800472E4();
    CloseEvent(D_80082FB4[0]);
    CloseEvent(D_80082FB4[1]);
    CloseEvent(D_80082FB4[2]);
    CloseEvent(D_80082FB4[3]);
    CloseEvent(D_80082FB4[4]);
    CloseEvent(D_80082FB4[5]);
    CloseEvent(D_80082FB4[6]);
    CloseEvent(D_80082FB4[7]);
    func_800472F4();
    func_8004D968();
}


/** @brief Calls func_80027448 and func_8003ABA4 in sequence. */
void func_8002A128(void) {
    func_80027448();
    func_8003ABA4();
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A150);


/**
 * @brief Apply a transformation or encoding to a value if a global flag is set.
 * @param a0 First parameter (context or key).
 * @param a1 Value to potentially transform; returned unchanged if D_80052958 is 0.
 * @return Transformed a1 if D_80052958 is nonzero, otherwise the original a1.
 * @note When active, calls func_8002E8DC with parameters from D_800837A0/AC/AE globals,
 *       then applies func_8002A45C to the result.
 */
s32 func_8002A238(s32 a0, s32 a1) {
    extern u8 D_80052958;
    extern u8 D_800837A0[];
    extern s16 D_800837AC;
    extern u8 D_800837AE;
    if (D_80052958 != 0) {
        s32 result = func_8002E8DC(a0, a1, D_800837AC, D_800837AE, (s32)D_800837A0, 7);
        a1 = func_8002A45C(a0, result);
    }
    return a1;
}


/**
 * @brief Copy a null-terminated string from src to dst (strcpy implementation, duplicate).
 * @param dst Destination buffer.
 * @param src Source null-terminated string.
 * @note This is a second copy of the strcpy function (see also func_8002871C).
 */
void func_8002A2A8(u8 *dst, u8 *src) {
    s32 c;
    do {
        c = *src++;
        *dst++ = c;
    } while (c != 0);
}


/**
 * @brief Append a null-terminated string to dst (strcat implementation, duplicate).
 * @param dst Destination buffer containing an existing null-terminated string.
 * @param src Source null-terminated string to append.
 * @note This is a second copy of the strcat function (see also func_80028738).
 */
void func_8002A2C4(u8 *dst, u8 *src) {
    do { } while (*dst++);
    func_8002A2A8(dst - 1, src);
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A2F4);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A318);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A340);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A36C);


/** @brief Wrapper for func_80020E4C. */
void func_8002A3A8(void) { func_80020E4C(); }


/** @brief Wrapper for func_80020E4C (duplicate of func_8002A3A8). */
void func_8002A3C8(void) { func_80020E4C(); }


/** @brief Wrapper for func_80020DB8. */
void func_8002A3E8(void) { func_80020DB8(); }


/**
 * @brief Copy 8-byte display field from *D_8005F134 to destination.
 *
 * @param dst Destination for the 8-byte unaligned copy (RECT-sized).
 */
void func_8002A408(RECT *dst) {
    extern u8 *D_8005F134;
    RECT *src = (RECT *)D_8005F134;
    *dst = *src;
}


extern u8 *D_8005F134;
/**
 * @brief Copy display coordinates (x, y) from the global display struct D_8005F134.
 * @param a0 Destination buffer; receives x at offset 0 and y at offset 2 (both u16).
 */
void func_8002A438(u8 *a0) {
    u8 *p = D_8005F134;
    *(u16 *)(a0 + 0) = *(u16 *)(p + 8);
    *(u16 *)(a0 + 2) = *(u16 *)(p + 0xA);
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A45C);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A528);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A588);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A5E8);


extern s32 D_800834C0;

/** @brief Calls func_8002A5E8(a0, 0) then func_80030CB0(a0).
 *  @param a0 Parameter passed to both calls.
 */
void func_8002A834(s32 a0) {
    func_8002A5E8(a0, 0);
    func_80030CB0(a0);
}


/** @brief Calls func_8002A5E8(a0, 1).
 *  @param a0 First parameter.
 */
void func_8002A868(s32 a0) {
    func_8002A5E8(a0, 1);
}


/**
 * @brief Read the first s32 word from the structure pointed to by D_800834C0.
 * @return Value at *D_800834C0.
 */
s32 func_8002A888(void) {
    return *(s32 *)D_800834C0;
}


/**
 * @brief Read an s32 value at offset 0x54 from the structure pointed to by D_800834C0.
 * @return Value at D_800834C0 + 0x54.
 */
s32 func_8002A8A0(void) {
    return *(s32 *)(D_800834C0 + 0x54);
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A8B8);


/** @brief Returns the value of D_800834C0 plus 8. */
s32 func_8002A91C(void) {
    extern s32 D_800834C0;
    return D_800834C0 + 8;
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A92C);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002AA18);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002AAC0);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002AB5C);


/* D_80083210 is an array of some datastructure that is 64 bytes in size.
   Below are some functions that fetch entries given an index and some property from that datastructure. */
extern u8 D_80083210[];

/**
 * @brief Get a pointer to a battle entity by index.
 * @param idx Entity index into D_80083210 (stride 64 bytes).
 * @return Pointer to the start of the entity's 64-byte record.
 */
u8 *func_8002AC74(s32 idx) {
    return D_80083210 + idx * 64;
}

/**
 * @brief Set a battle entity's field 0x37 (likely animation speed/priority), clamped to [3, 11].
 * @param idx Entity index into D_80083210 (stride 64 bytes).
 * @param val Value to set; clamped to minimum 3 and maximum 11.
 */
void func_8002AC88(s32 idx, s32 val) {
    u8 *p = D_80083210 + idx * 64;
    s32 v;
    if (val >= 3) {
        if (val < 12) {
            v = val;
        } else {
            v = 11;
        }
    } else {
        v = 3;
    }
    p[0x37] = v;
}


/**
 * @brief Get a battle entity's field 0x37 value.
 * @param idx Entity index into D_80083210 (stride 64 bytes).
 * @return Value of field 0x37 for the entity.
 */
s32 func_8002ACBC(s32 idx) {
    u8 *p = D_80083210 + idx * 64;
    return p[0x37];
}


/** @brief Stores a byte value into the battle entity array at D_80083210.
 *
 *  @param idx Entity index (stride 64 bytes).
 *  @param offset Additional byte offset within the entity.
 *  @param val Byte value to store at offset 0x3A.
 */
void func_8002ACD8(s32 idx, s32 offset, s32 val) {
    extern u8 D_80083210[];
    u8 *entry = D_80083210 + idx * 64;
    *(u8 *)(entry + offset + 0x3A) = val;
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002ACF4);


/**
 * @brief Set a battle entity's 8-byte field at offset 8 (bounding rect).
 *
 * @param a0 Entity index into D_80083210 (stride 64 bytes).
 * @param src Source RECT to copy (8 bytes).
 */
void func_8002AD04(s32 a0, RECT *src) {
    extern u8 D_80083210[];
    u8 *entity = D_80083210 + a0 * 64;
    *(RECT *)(entity + 8) = *src;
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002AD3C);


// battle_entity_get_field_38 - D_80083210 stride 64

/**
 * @brief Get a battle entity's 8-byte field at offset 8 (bounding rect).
 *
 * @param a0 Entity index into D_80083210 (stride 64 bytes).
 * @param dst Destination for the 8-byte copy.
 */
void func_8002ADA4(s32 a0, RECT *dst) {
    extern u8 D_80083210[];
    u8 *entity = D_80083210 + a0 * 64;
    *dst = *(RECT *)(entity + 8);
}


/**
 * @brief Get a battle entity's 8-byte field at offset 0x10.
 *
 * @param a0 Entity index into D_80083210 (stride 64 bytes).
 * @param dst Destination for the 8-byte copy.
 */
void func_8002ADDC(s32 a0, RECT *dst) {
    extern u8 D_80083210[];
    u8 *entity = D_80083210 + a0 * 64;
    *dst = *(RECT *)(entity + 0x10);
}


/**
 * @brief Get a battle entity's field 0x38 (likely entity type or state).
 * @param idx Entity index into D_80083210 (stride 64 bytes).
 * @return Value of field 0x38 for the entity.
 */
s32 func_8002AE14(s32 idx) {
    u8 *p = D_80083210 + idx * 64;
    return p[0x38];
}


/**
 * @brief Set a battle entity's field 0x38 and compute field 0x30 based on bit 0.
 * @param idx Entity index into D_80083210 (stride 64 bytes).
 * @param val Value to store in field 0x38; if bit 0 is set, field 0x30 becomes 0x3A000000,
 *            otherwise 0x38000000.
 */
void func_8002AE30(s32 idx, s32 val) {
    u8 *p = D_80083210 + idx * 64;
    s32 v;
    p[0x38] = val;
    v = 0x38;
    if (val & 1) {
        v = 0x3A;
    }
    *(s32 *)(p + 0x30) = v << 24;
}


/**
 * @brief Set a battle entity's field 0x00 (first s32 word, likely a pointer or ID).
 * @param idx Entity index into D_80083210 (stride 64 bytes).
 * @param val s32 value to store at offset 0x00.
 */
void func_8002AE60(s32 idx, s32 val) {
    u8 *p = D_80083210 + idx * 64;
    *(s32 *)p = val;
}


/**
 * @brief Set a battle entity's field 0x04 (second s32 word).
 * @param idx Entity index into D_80083210 (stride 64 bytes).
 * @param val s32 value to store at offset 0x04.
 */
void func_8002AE78(s32 idx, s32 val) {
    u8 *p = D_80083210 + idx * 64;
    *(s32 *)(p + 4) = val;
}


/**
 * @brief Set a battle entity's field 0x36.
 * @param a0 Entity index into D_80083210 (stride 64 bytes).
 * @param a1 Value to store at offset 0x36.
 */
void func_8002AE90(s32 a0, s32 a1) {
    u8 *p = D_80083210 + a0 * 64;
    p[0x36] = a1;
}


/**
 * @brief Get a battle entity's field 0x36 value.
 * @param a0 Entity index into D_80083210 (stride 64 bytes).
 * @return Value of field 0x36 for the entity.
 */
u32 func_8002AEA8(s32 a0) {
    u8 *p = D_80083210 + a0 * 64;
    return p[0x36];
}


/**
 * @brief Set a battle entity's field 0x35.
 * @param a0 Entity index into D_80083210 (stride 64 bytes).
 * @param a1 Value to store at offset 0x35.
 */
void func_8002AEC4(s32 a0, s32 a1) {
    u8 *p = D_80083210 + a0 * 64;
    p[0x35] = a1;
}


/**
 * @brief Get a battle entity's field 0x35 value.
 * @param a0 Entity index into D_80083210 (stride 64 bytes).
 * @return Value of field 0x35 for the entity.
 */
u32 func_8002AEDC(s32 a0) {
    u8 *p = D_80083210 + a0 * 64;
    return p[0x35];
}


/**
 * @brief Set a battle entity's field 0x34 and clear related fields if value is 0.
 * @param idx Entity index into D_80083210 (stride 64 bytes).
 * @param value Value to store at offset 0x34; if 0, also clears fields 0x36, 0x04, and 0x00.
 * @note Setting to 0 appears to fully deactivate the entity.
 */
void func_8002AEF8(s32 idx, s32 value) {
    u8 *p = (u8 *)D_80083210 + (idx << 6);

    p[0x34] = value;
    if (value == 0) {
        func_8002AE90(idx, 0);
        func_8002AE78(idx, 0);
        func_8002AE60(idx, 0);
    }
}


/**
 * @brief Get a battle entity's field 0x34 (likely active/type indicator).
 * @param idx Entity index into D_80083210 (stride 64 bytes).
 * @return Value of field 0x34 for the entity.
 */
s32 func_8002AF54(s32 idx) {
    u8 *p = D_80083210 + idx * 64;
    return p[0x34];
}


/**
 * @brief Set a battle entity's field 0x3C (s16, likely a scale or distance value).
 * @param a0 Entity index into D_80083210 (stride 64 bytes).
 * @param a1 s16 value to store at offset 0x3C.
 */
void func_8002AF70(s32 a0, s32 a1) {
    u8 *p = D_80083210 + a0 * 64;
    *(s16 *)(p + 0x3C) = a1;
}


/**
 * @brief Get a battle entity's field 0x3C value (s16).
 * @param a0 Entity index into D_80083210 (stride 64 bytes).
 * @return s16 value at offset 0x3C for the entity.
 */
s32 func_8002AF88(s32 a0) {
    u8 *p = D_80083210 + a0 * 64;
    return *(s16 *)(p + 0x3C);
}


/**
 * @brief Initialize a battle entity to default values.
 * @param a0 Entity index into D_80083210 (stride 64 bytes).
 * @note Sets up a default rectangle (64,64,128,128), entity type 6, clears pointers and IDs,
 *       sets field 0x37 to 3, initializes sub-fields, sets field 0x3C to 0x1000, clears field 0x36.
 */
void func_8002AFA4(s32 a0) {
    s16 rect[4];
    s32 i;
    rect[0] = 0x40;
    rect[1] = 0x40;
    rect[2] = 0x80;
    rect[3] = 0x80;
    func_8002AD04(a0, rect);
    func_8002AD3C(a0, rect);
    func_8002AE30(a0, 6);
    func_8002AE78(a0, 0);
    func_8002AE60(a0, 0);
    func_8002AEF8(a0, 0);
    func_8002AC88(a0, 3);
    for (i = 0; i < 2; i++) {
        func_8002ACD8(a0, i, 0);
    }
    func_8002ACD8(a0, 1, a0);
    func_8002AF70(a0, 0x1000);
    func_8002AE90(a0, 0);
}


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002B080);


INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002B16C);
