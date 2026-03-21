#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

extern SfxEntry g_sfxEntries[];

INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002C8A4);


/**
 * @brief Decrement byte at offset 0x1E3 in g_sfxEntries and trigger an SFX update.
 *
 * Offset 0x1E3 falls within entry index 8 (8 * 60 = 480 = 0x1E0, +3 = 0x1E3),
 * at field offset 0x03 of that entry. After decrementing, calls func_8002C8A4
 * to process the change.
 */
void func_8002C920(void) {
    s32 base = (s32)g_sfxEntries;
    (*(u8 *)(base + 0x1E3))--;
    func_8002C8A4();
}


/**
 * @brief Build a packed RGBA-like color value from a scalar and store it as an
 *        SFX control word.
 *
 * Divides the input by 32, masks to 8 bits, replicates across the low 24 bits
 * (R=G=B), sets the high byte to 0x64 (alpha/command), and stores the result
 * in g_flashColor before triggering an SFX update.
 *
 * @param a0 Scalar intensity value (divided by 32, clamped to 0-255).
 */
void func_8002C954(s32 a0) {
    extern s32 g_flashColor;
    a0 /= 32;
    a0 &= 0xFF;
    a0 |= (a0 << 16) | (a0 << 8);
    a0 |= 0x64000000;
    g_flashColor = a0;
    func_8002C8A4();
}


/**
 * @brief Read volume from an SFX entry and dispatch to color/effect updates.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 */
void func_8002C9A4(s32 idx) {
    SfxEntry *entry = &g_sfxEntries[idx];
    s32 val = entry->volume;
    func_8002C954(val);
    func_80030058(val);
}


// sfx_entry_swap_field_16 - g_sfxEntries stride 60, returns old value

/**
 * @brief Set the linked battle entity index on an SFX entry.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param val Battle entity index.
 */
void func_8002C9F0(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries[idx];
    entry->entityIdx = val;
}


/**
 * @brief Swap the state of an SFX entry, returning the old value.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param val New state value.
 * @return Previous state value.
 */
s32 func_8002CA10(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries[idx];
    s32 old = entry->state;
    entry->state = val;
    return old;
}


/**
 * @brief Get the state of an SFX entry.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @return State value (0 = inactive, 1 = active).
 */
s32 func_8002CA34(s32 idx) {
    SfxEntry *entry = &g_sfxEntries[idx];
    return entry->state;
}


/**
 * @brief Set pitch and clear field14 on an SFX entry.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param val Pitch value.
 */
void func_8002CA58(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries[idx];
    entry->pitch = val;
    entry->field14 = 0;
}


/**
 * @brief Set field1C on an SFX entry.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param val Value to store.
 */
void func_8002CA7C(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries[idx];
    entry->field1C = val;
}


/**
 * @brief Set the rate delta on an SFX entry.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param val Rate delta value (negative = fade out).
 */
void func_8002CA9C(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries[idx];
    entry->rateDelta = val;
}


/**
 * @brief Get field1C of an SFX entry.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @return Signed 16-bit value.
 */
s32 func_8002CABC(s32 idx) {
    SfxEntry *entry = &g_sfxEntries[idx];
    return entry->field1C;
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002CAE0);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002CC4C);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002CDE4);


/** @brief Stores a byte to global D_800831DC. */
void func_8002CE68(s32 a0) {
    extern s8 D_800831DC;
    D_800831DC = a0;
}


/** @brief Returns the signed byte value of global D_800831DC. */
s32 func_8002CE74(void) {
    extern s8 D_800831DC;
    return D_800831DC;
}


/**
 * @brief Get the remaining duration for an SFX entry.
 *
 * Looks up the entry at g_sfxEntries + idx * 60. If the entry's active
 * flag (signed byte at +0x19) is zero, returns -1. Otherwise returns
 * the difference between the total length (+0x2B) and current position (+0x29).
 *
 * @param idx Index into the SFX entry array (stride 60).
 * @return Remaining duration, or -1 if inactive.
 */
/**
 * @note Non-matching: leaf register allocation (entry in $a0 instead of $v0),
 *       branch direction inversion (beqz vs bnez), and compiler fills j
 *       delay slot with subu making function 1 instruction shorter.
 */
INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002CE84);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002CECC);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002CF54);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002D040);


/**
 * @brief Initialize a sound effect entry for playback.
 *
 * Computes the entry address as g_sfxEntries + a0 * 60, clears various
 * fields, stores the data pointer (a1) at offsets +0x08 and +0x0C,
 * modifies the status word at +0x14 (sets nibble to 0x7, rotates top nibble
 * down), calls func_8002FF24, and sets default volume bytes.
 *
 * @param a0 Sound effect index.
 * @param a1 Data pointer to store.
 */
void func_8002D6AC(s32 a0, s32 a1) {
    s32 mask1 = 0x0FFFFFFF;
    s32 mask2 = (s32)0xF0FFFFFF;
    u8 *entry = (u8 *)g_sfxEntries + a0 * 60;
    s32 status = *(s32 *)(entry + 0x14);
    *(u8 *)(entry + 0x28) = 0;
    *(s32 *)(entry + 0x08) = a1;
    *(s32 *)(entry + 0x0C) = a1;
    *(u16 *)(entry + 0x12) = 0;
    *(s32 *)(entry + 0x24) = 0;
    status &= mask1;
    status |= 0x70000000;
    mask2 &= status;
    mask2 |= (u32)status >> 28 << 24;
    *(s32 *)(entry + 0x14) = mask2;
    func_8002FF24(entry);
    *(u8 *)(entry + 0x29) = 0xFF;
    *(u8 *)(entry + 0x2A) = 0xFF;
    *(u8 *)(entry + 0x19) = 0;
}


/**
 * @brief Skips past a given number of null-terminated strings, then calls func_8002D6AC.
 *
 * Advances @p a1 past @p a2 null-terminated strings by scanning bytes until
 * null is found for each string. After skipping, calls func_8002D6AC with
 * the original @p a0 and @p a1.
 *
 * @param a0 First parameter passed through to func_8002D6AC.
 * @param a1 Pointer to the start of the string data.
 * @param a2 Number of strings to skip.
 */
void func_8002D744(s32 a0, u8 *a1, s32 a2) {
    while (a2 > 0) {
        while (*a1++ != 0) {
        }
        a2--;
    }
    func_8002D6AC(a0, a1);
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002D784);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002D818);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002D8CC);


// sfx_entry_init_fields_16_1E_2D - g_sfxEntries stride 60

INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002D970);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002DBF8);


/**
 * @brief Configure an SFX entry for playback: mark as active, set rate, and set mode.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param rate Playback rate value (e.g. 0x200, 0x1000).
 * @param mode Playback mode byte.
 */
void func_8002DCA4(s32 idx, s32 rate, s32 mode) {
    SfxEntry *entry = &g_sfxEntries[idx];
    entry->state = 1;
    entry->rateDelta = rate;
    entry->mode = mode;
}


/** @brief Start SFX entry playback at slow rate (0x200). */
void func_8002DCD0(s32 idx) {
    func_8002DCA4(idx, 0x200, 0);
}


/** @brief Start SFX entry playback at normal rate (0x1000). */
void func_8002DCF4(s32 idx) {
    func_8002DCA4(idx, 0x1000, 0);
}


/**
 * @brief Set the rate delta (field 0x1E) for an SFX entry.
 * @param idx Index into the SFX entry array.
 * @param val Rate delta value (negative values = fade out).
 */
void func_8002DD18(s32 idx, s32 val) {
    func_8002CA9C(idx, val);
}


/** @brief Set SFX entry to fade out at slow rate (-0x200). */
void func_8002DD38(s32 idx) {
    func_8002DD18(idx, -0x200);
}


/** @brief Set SFX entry to fade out at fast rate (-0x1000). */
void func_8002DD58(s32 idx) {
    func_8002DD18(idx, -0x1000);
}


/**
 * @brief Set reverb mode on the SFX entry's linked entity, clamped to [3, 11].
 *
 * Adds 3 to @p val, clamps to [3, 11], then calls func_8002AC88 with
 * the entry's entityIdx and the clamped value.
 *
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param val Base reverb mode value.
 */
void func_8002DD78(s32 idx, s32 val) {
    s32 off;
    s32 base;
    s32 clamped;
    val += 3;
    if (val >= 3) {
        clamped = 11;
        if (val < 12) {
            clamped = val;
        }
    } else {
        clamped = 3;
    }
    off = idx * 60;
    base = (s32)g_sfxEntries;
    func_8002AC88(*(u8 *)(off + base + 0x18), clamped);
}


/**
 * @brief Get field28 of an SFX entry.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @return Value of field28.
 */
s32 func_8002DDD8(s32 idx) {
    SfxEntry *entry = &g_sfxEntries[idx];
    return entry->field28;
}


/**
 * @brief Set entity type flags on the battle entity linked to an SFX entry.
 *
 * Reads the entity index from the SFX entry, then sets entity type
 * (and derived draw mode) on that battle entity with the value OR'd with 8.
 *
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param val Flag value to OR with 8 before storing.
 */
void func_8002DDFC(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries[idx];
    func_8002AE30(entry->entityIdx, val | 8);
}


/**
 * @brief Read the entity type of the battle entity linked to an SFX entry.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 */
void func_8002DE38(s32 idx) {
    SfxEntry *entry = &g_sfxEntries[idx];
    func_8002AE14(entry->entityIdx);
}


/**
 * @brief Set field2F on an SFX entry.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param val Value to store.
 */
void func_8002DE74(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries[idx];
    entry->field2F = val;
}


/**
 * @brief Initialize an SFX entity slot to default values.
 *
 * Zeros out field14, field19, field2F, then configures defaults:
 * pitch = 0x1000, state = 0, reverb mode = 3, rate = 0, delta = 0,
 * entity flags = 6|8, display rect = (64,64,128,128), volume = 0x1000.
 *
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 */
void func_8002DE94(idx)

s32 idx;
{
    s32 a1 = 0;
    SfxEntry *entry = &g_sfxEntries[idx];
    entry->field14 = 0;
    entry->field19 = 0;
    entry->field2F = 0;
    func_8002D6AC(idx, a1);
    func_8002CA58(idx, 0x1000);
    func_8002CA10(idx, 0);
    func_8002DD78(idx, 3);
    func_8002CA7C(idx, 0);
    func_8002CA9C(idx, 0);
    func_8002DDFC(idx, 6);
    {
        s16 buf[4];
        buf[0] = 0x40;
        buf[1] = 0x40;
        buf[2] = 0x80;
        buf[3] = 0x80;
        func_8002E064(idx, buf);
    }
    func_8002C868(idx, 0x1000);
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002DF5C);


/**
 * @brief Copy an SFX entry's source rectangle to destination.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param dst Destination RECT.
 */
void func_8002E028(s32 idx, RECT *dst) {
    SfxEntry *entry = &g_sfxEntries[idx];
    *dst = entry->rect;
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E064);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E1B4);


/**
 * @brief Reset all SFX entries and clear global SFX state.
 *
 * Sets g_sfxEntries[0x1EC] to -1, calls func_8002DF5C for each of the 8
 * SFX slots, clears D_800831D3 and several fields in g_sfxEntries, then
 * calls func_8002C130 to finalize.
 */
void func_8002E1E8(void) {
    extern u8 D_800831D3;
    s32 i;
    s32 base = (s32)g_sfxEntries;
    *(s8 *)(base + 0x1EC) = -1;
    for (i = 0; i < 8; i++) {
        func_8002DF5C(i);
    }
    D_800831D3 = 0;
    *(u16 *)(base + 0x1F8) = 0;
    *(u16 *)(base + 0x1FA) = 0;
    *(u8 *)(base + 0x1F4) = 0;
    *(u8 *)(base + 0x1F5) = 0;
    func_8002C130();
}


/**
 * @brief Look up linked entity's animation speed and dispatch.
 *
 * Reads the entity index from the SFX entry, gets its animation speed,
 * then passes the result to func_8002BEEC.
 *
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 */
void func_8002E254(s32 idx) {
    SfxEntry *entry = &g_sfxEntries[idx];
    s32 val = func_8002ACBC(entry->entityIdx);
    func_8002BEEC(val);
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E298);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E3A4);


/** @brief Extracts a 4-bit nibble from packed byte array D_800834D8.
 *  Even indices return the low nibble; odd indices return the high nibble.
 *  @param a0 Nibble index.
 *  @return The 4-bit value (0-15).
 */
s32 func_8002E428(s32 a0) {
    extern u8 D_800834D8[];
    s32 base = (s32)D_800834D8;
    u32 val = *(u8 *)(base + (a0 >> 1));
    if (a0 & 1) {
        val >>= 4;
    }
    return val & 0xF;
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E454);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E4AC);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E680);


/** @brief Calls func_8002E4AC with a1=1. */
void func_8002E744(s32 a0) {
    func_8002E4AC(a0, 1);
}


/** @brief Calls func_8002E4AC with a1=1. */
void func_8002E764(s32 a0) {
    func_8002E4AC(a0, 1);
}


/** @brief Calls func_8002E4AC(a0, 1) and returns the result as u16. */
u16 func_8002E784(s32 a0) {
    return (u16)func_8002E4AC(a0, 1);
}


/**
 * @brief Wrapper that calls func_8002E4AC with second argument 0 and returns result as u16.
 * @param a0 First argument passed through to func_8002E4AC.
 * @return Result of func_8002E4AC truncated to 16 bits.
 * @note Purpose uncertain -- appears to query a 16-bit status value.
 */
u16 func_8002E7A4(s32 a0) {
    return (u16)func_8002E4AC(a0, 0);
}


/**
 * @brief Store raw intensity and build packed grayscale color for SFX overlay.
 *
 * Stores the raw value in D_80083850, then divides by 32, masks to 8 bits,
 * replicates across R/G/B, sets command byte to 0x64, and stores to
 * g_menuColor before triggering an SFX update.
 *
 * @param a0 Scalar intensity value.
 */
void func_8002E7C4(s32 a0) {
    extern s32 D_80083850;
    extern s32 g_menuColor;
    D_80083850 = a0;
    {
        s32 val = (u32)a0 >> 5;
        a0 = val & 0xFF;
    }
    a0 |= (a0 << 16) | (a0 << 8);
    a0 |= 0x64000000;
    g_menuColor = a0;
    func_8002C8A4();
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E810);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E8DC);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002EAD0);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002EE10);


