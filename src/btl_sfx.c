#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

extern SfxSystem g_sfxEntries;
extern s32 g_flashColor;
extern s8 D_800831DC;
extern u8 D_800831D3;
extern s32 D_80083850;
extern s32 g_menuColor[2];
extern u8 D_800834D8[];
void func_8002D970(void);
void func_8002DBF8(void);


/**
 * @brief Process flash color based on SFX counter flags.
 *
 * If counter bit 4 is set, attenuates both the flash color and menu color
 * to 75% intensity. Stores the results to the global state output fields.
 */
void func_8002C8A4(void) {
    u32 color1 = g_sfxEntries.state.color1;
    u8 flags = g_sfxEntries.state.counter;
    u32 color2 = g_menuColor[0];

    if (flags & 0x10) {
        color1 &= 0xFF;
        color1 = (color1 * 3) >> 2;
        color1 = color1 | ((color1 << 16) | (color1 << 8));
        color1 |= 0x64000000;

        color2 &= 0xFF;
        color2 = (color2 * 3) >> 2;
        color2 = color2 | ((color2 << 16) | (color2 << 8));
        color2 |= 0x64000000;
    }

    g_sfxEntries.state.color2 = color1;
    g_menuColor[1] = color2;
}


/**
 * @brief Decrement the global SFX counter and trigger an SFX update.
 */
void decrementSfxCounter(void) {
    g_sfxEntries.state.counter--;
    func_8002C8A4();
}


/**
 * @brief Build a packed grayscale color from intensity and store as flash color.
 * @param intensity Scalar intensity value (divided by 32, clamped to 0-255).
 */
void setFlashColor(s32 intensity) {
    intensity /= 32;
    intensity &= 0xFF;
    intensity |= (intensity << 16) | (intensity << 8);
    intensity |= 0x64000000;
    g_flashColor = intensity;
    func_8002C8A4();
}


/**
 * @brief Read volume from an SFX entry and dispatch to color/effect updates.
 * @param idx SFX entry index.
 */
void dispatchSfxColorUpdate(s32 idx) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    s32 val = entry->volume;
    setFlashColor(val);
    buildGrayscaleGpuColor(val);
}


/**
 * @brief Set the linked battle entity index on an SFX entry.
 * @param idx SFX entry index.
 * @param val Battle entity index.
 */
void setSfxEntityIndex(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    entry->entityIdx = val;
}


/**
 * @brief Swap the state of an SFX entry, returning the old value.
 * @param idx SFX entry index.
 * @param val New state value.
 * @return Previous state value.
 */
s32 swapSfxState(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    s32 old = entry->flags.fields.state;
    entry->flags.fields.state = val;
    return old;
}


/**
 * @brief Get the state of an SFX entry.
 * @param idx SFX entry index.
 * @return State value (0 = inactive, 1 = active).
 */
s32 getSfxState(s32 idx) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    return entry->flags.fields.state;
}


/**
 * @brief Set pitch and clear field14 on an SFX entry.
 * @param idx SFX entry index.
 * @param val Pitch value.
 */
void setSfxPitch(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    entry->pitch = val;
    entry->flags.fields.field14 = 0;
}


/**
 * @brief Set field1C on an SFX entry.
 * @param idx SFX entry index.
 * @param val Value to store.
 */
void setSfxField1C(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    entry->field1C = val;
}


/**
 * @brief Set the rate delta on an SFX entry.
 * @param idx SFX entry index.
 * @param val Rate delta value (negative = fade out).
 */
void setSfxRateDelta(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    entry->rateDelta = val;
}


/**
 * @brief Get field1C of an SFX entry.
 * @param idx SFX entry index.
 * @return Signed 16-bit value.
 */
s32 getSfxField1C(s32 idx) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    return entry->field1C;
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002CAE0);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002CC4C);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002CDE4);


/** @brief Set the global SFX flag. */
void setSfxGlobalFlag(s32 val) {
    D_800831DC = val;
}


/** @brief Get the global SFX flag. */
s32 getSfxGlobalFlag(void) {
    return D_800831DC;
}


/**
 * @brief Get the remaining duration for an SFX entry.
 *
 * Returns -1 if inactive, otherwise the difference between
 * total length (field2B) and current position (field29).
 *
 * @param idx SFX entry index.
 * @return Remaining duration, or -1 if inactive.
 */
s32 func_8002CE84(s32 idx) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    SfxEntry *e2;
    s32 always;
    e2 = entry;
    always = 1; /* Regalloc */
    if (always) {
        if (entry->field19 == 0) {
            return -1;
        }
        return e2->field2B - e2->field29;
    }
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002CECC);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002CF54);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002D040);


/**
 * @brief Initialize a sound effect entry for playback.
 *
 * Clears fields, stores the data pointer, sets up the packed flags
 * nibbles, calls clearEntityColor, and sets default volume bytes.
 *
 * @param index SFX entry index.
 * @param param Data pointer to store.
 */
void initSfxPlayback(s32 index, s32 param) {
    SfxEntry *entry = &g_sfxEntries.entries[index];

    entry->field28 = 0;
    entry->dataPtr = param;
    entry->dataPtrCopy = param;
    entry->field12 = 0;
    entry->seqState = 0;
    entry->flags.raw = (entry->flags.raw & 0x0FFFFFFF) | 0x70000000;
    entry->flags.raw = (entry->flags.raw & 0xF0FFFFFF) | (entry->flags.raw >> 28 << 24);

    clearEntityColor(entry);

    entry->field29 = 0xFF;
    entry->field2A = 0xFF;
    entry->field19 = 0;
}


/**
 * @brief Skips past a given number of null-terminated strings, then calls initSfxPlayback.
 *
 * Advances @p a1 past @p a2 null-terminated strings by scanning bytes until
 * null is found for each string. After skipping, calls initSfxPlayback with
 * the original @p a0 and @p a1.
 *
 * @param index SFX entry index.
 * @param data  Pointer to the start of the string data.
 * @param count Number of strings to skip.
 */
void initSfxPlaybackAfterStrings(s32 index, u8 *data, s32 count) {
    while (count > 0) {
        while (*data++ != 0) {
        }
        count--;
    }
    initSfxPlayback(index, data);
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002D784);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002D818);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002D8CC);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002D970);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002DBF8);


/**
 * @brief Configure an SFX entry for playback: mark as active, set rate, and set mode.
 * @param idx SFX entry index.
 * @param rate Playback rate value (e.g. 0x200, 0x1000).
 * @param mode Playback mode byte.
 */
void configureSfxPlayback(s32 idx, s32 rate, s32 mode) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    entry->flags.fields.state = 1;
    entry->rateDelta = rate;
    entry->mode = mode;
}


/** @brief Start SFX entry playback at slow rate (0x200). */
void startSfxSlow(s32 idx) {
    configureSfxPlayback(idx, 0x200, 0);
}


/** @brief Start SFX entry playback at normal rate (0x1000). */
void startSfxNormal(s32 idx) {
    configureSfxPlayback(idx, 0x1000, 0);
}


/**
 * @brief Set the rate delta for an SFX entry (wrapper).
 * @param idx SFX entry index.
 * @param val Rate delta value (negative = fade out).
 */
void setSfxFadeRate(s32 idx, s32 val) {
    setSfxRateDelta(idx, val);
}


/** @brief Set SFX entry to fade out at slow rate (-0x200). */
void fadeOutSfxSlow(s32 idx) {
    setSfxFadeRate(idx, -0x200);
}


/** @brief Set SFX entry to fade out at fast rate (-0x1000). */
void fadeOutSfxFast(s32 idx) {
    setSfxFadeRate(idx, -0x1000);
}


/**
 * @brief Set reverb mode on the SFX entry's linked entity, clamped to [3, 11].
 *
 * Adds 3 to @p val, clamps to [3, 11], then calls setBattleEntityAnimSpeed with
 * the entry's entityIdx and the clamped value.
 *
 * @param idx SFX entry index.
 * @param val Base reverb mode value.
 */
void setSfxReverbMode(s32 idx, s32 val) {
    SfxEntry *entry;
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
    entry = &g_sfxEntries.entries[idx];
    setBattleEntityAnimSpeed(entry->entityIdx, clamped);
}


/**
 * @brief Get field28 of an SFX entry.
 * @param idx SFX entry index.
 * @return Value of field28.
 */
s32 getSfxField28(s32 idx) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    return entry->field28;
}


/**
 * @brief Set entity type flags on the battle entity linked to an SFX entry.
 *
 * Reads the entity index from the SFX entry, then sets entity type
 * (and derived draw mode) on that battle entity with the value OR'd with 8.
 *
 * @param idx SFX entry index.
 * @param val Flag value to OR with 8 before storing.
 */
void setSfxEntityType(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    setBattleEntityType(entry->entityIdx, val | 8);
}


/**
 * @brief Read the entity type of the battle entity linked to an SFX entry.
 * @param idx SFX entry index.
 */
void readSfxEntityType(s32 idx) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    getBattleEntityType(entry->entityIdx);
}


/**
 * @brief Set field2F on an SFX entry.
 * @param idx SFX entry index.
 * @param val Value to store.
 */
void setSfxField2F(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    entry->field2F = val;
}


/**
 * @brief Initialize an SFX entity slot to default values.
 *
 * Zeros out field14, field19, field2F, then configures defaults:
 * pitch = 0x1000, state = 0, reverb mode = 3, rate = 0, delta = 0,
 * entity flags = 6|8, display rect = (64,64,128,128), volume = 0x1000.
 *
 * @param idx SFX entry index.
 */
void initSfxSlot(idx)

s32 idx;
{
    s32 nullData = 0;
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    entry->flags.fields.field14 = 0;
    entry->field19 = 0;
    entry->field2F = 0;
    initSfxPlayback(idx, nullData);
    setSfxPitch(idx, 0x1000);
    swapSfxState(idx, 0);
    setSfxReverbMode(idx, 3);
    setSfxField1C(idx, 0);
    setSfxRateDelta(idx, 0);
    setSfxEntityType(idx, 6);
    {
        s16 buf[4];
        buf[0] = 0x40;
        buf[1] = 0x40;
        buf[2] = 0x80;
        buf[3] = 0x80;
        func_8002E064(idx, buf);
    }
    setSfxEntryVolume(idx, 0x1000);
}


/**
 * @brief Initialize an SFX slot: set active flag, callbacks, entity index, and clear fields.
 *
 * Activates the battle entity, assigns update/render callbacks (func_8002D970
 * and func_8002DBF8), links the entity index, configures the sub-field, calls
 * initSfxSlot for default values, then clears sequence state and status bits.
 *
 * @param idx SFX entry index.
 */
void func_8002DF5C(s32 idx) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];

    setBattleEntityActive(idx, 1);
    entry->entityIdx = idx;
    setBattleEntityField04(idx, (s32)func_8002D970);
    setBattleEntityField00(idx, (s32)func_8002DBF8);
    setBattleEntityField35(idx, 0);
    setBattleEntitySubField(idx, 0, idx);
    initSfxSlot(idx);

    entry->seqState = 0;
    entry->field30 = 0;
    entry->field32 = 0;
    *(u32 *)&entry->field2C &= 0xFF7FFFFF;

    setSfxEntryField34(idx, 0);
    setSfxEntryField38(idx, 0);
}


/**
 * @brief Copy an SFX entry's source rectangle to destination.
 * @param idx SFX entry index.
 * @param dst Destination RECT.
 */
void getSfxRect(s32 idx, RECT *dst) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    *dst = entry->rect;
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E064);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E1B4);


/**
 * @brief Reset all SFX entries and clear global SFX state.
 *
 * Marks the global state as inactive, reinitializes all 8 SFX slots,
 * clears global counters, then calls func_8002C130 to finalize.
 */
void resetAllSfx(void) {
    SfxSystem *sys = &g_sfxEntries;
    s32 i;
    sys->state.activeFlag = -1;
    for (i = 0; i < 8; i++) {
        func_8002DF5C(i);
    }
    D_800831D3 = 0;
    sys->state.field18 = 0;
    sys->state.field1A = 0;
    sys->state.field14 = 0;
    sys->state.field15 = 0;
    func_8002C130();
}


/**
 * @brief Look up linked entity's animation speed and dispatch.
 *
 * Reads the entity index from the SFX entry, gets its animation speed,
 * then passes the result to getEntityTablePtr.
 *
 * @param idx SFX entry index.
 */
void dispatchSfxAnimSpeed(s32 idx) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    s32 val = getBattleEntityAnimSpeed(entry->entityIdx);
    getEntityTablePtr(val);
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E298);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E3A4);


/** @brief Extracts a 4-bit nibble from packed byte array D_800834D8.
 *  Even indices return the low nibble; odd indices return the high nibble.
 *  @param idx Nibble index.
 *  @return The 4-bit value (0-15).
 */
s32 getNibbleValue(s32 idx) {
    u8 *base = D_800834D8;
    u32 val = base[idx >> 1];
    if (idx & 1) {
        val >>= 4;
    }
    return val & 0xF;
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E454);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E4AC);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E680);


/** @brief Get glyph width (variant A). */
void getGlyphWidthA(s32 code) {
    func_8002E4AC(code, 1);
}


/** @brief Get glyph width (variant B). */
void getGlyphWidthB(s32 code) {
    func_8002E4AC(code, 1);
}


/** @brief Get glyph width as u16. */
u16 getGlyphWidthU16(s32 code) {
    return (u16)func_8002E4AC(code, 1);
}


/**
 * @brief Get glyph status as u16.
 * @param code Glyph code.
 * @return Status value truncated to 16 bits.
 */
u16 getGlyphStatusU16(s32 code) {
    return (u16)func_8002E4AC(code, 0);
}


/**
 * @brief Store raw intensity and build packed grayscale color for the menu overlay.
 *
 * Saves the raw value, builds a packed RGB color (R=G=B), sets command
 * byte to 0x64, stores to g_menuColor, then triggers an SFX update.
 *
 * @param intensity Scalar intensity value.
 */
void setMenuColorIntensity(s32 intensity) {
    D_80083850 = intensity;
    {
        s32 val = (u32)intensity >> 5;
        intensity = val & 0xFF;
    }
    intensity |= (intensity << 16) | (intensity << 8);
    intensity |= 0x64000000;
    g_menuColor[0] = intensity;
    func_8002C8A4();
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E810);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E8DC);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002EAD0);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002EE10);


