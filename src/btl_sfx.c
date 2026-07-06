#include "common.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libetc.h"
#include "battle.h"
#include "btl_sfx.h"
#include "btl_entity.h"

extern SfxSystem g_sfxEntries;
extern s32 g_flashColor;
extern s8 D_800831DC;
extern u8 D_800831D3;
extern s32 D_80083850;
extern s32 g_menuColor[2];
extern u8 D_800834D8[];
void func_8002D970(void);
void func_8002D040(s32 sfxId, s32 arg1, s32 arg2);
void func_8002CC4C(s32 idx, s32 arg0);
void func_8002CDE4(RECT *rect, s32 scale, s32 arg2);
void setBattleEntityBoundRect(s32 idx, RECT *src);
void setBattleEntityRectClamp(s32 idx, RECT *src);
void func_8002E064(s32 index, RECT *srcRect);
void func_8002E1B4(s32 index, s32 value);


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


/**
 * @brief Scale a battle SFX rectangle about its center by a fixed-point factor.
 *
 * @p scale is a signed Q12 fixed-point multiplier where @c 0x1000 (4096 == ONE)
 * is 1.0; at exactly 1.0 the rectangle is left unchanged. Otherwise each axis is
 * recentered: the origin moves to @c center @c - @c (dimension @c * @c scale) @c /
 * @c 8192 (unscale the Q12 product, then halve) and the dimension becomes
 * @c (dimension @c * @c scale) @c / @c 4096.
 *
 * Written in the in-place register-reuse style of the sibling RECT helpers
 * (cf. @ref func_8002B080, and @c func_800A3398 which scales a RECT the same
 * way): the packed @c s16 pairs are read once as words and each value is
 * progressively transformed in its own variable, which keeps gcc 2.7.2 from
 * spilling copies. The @c do/while(0) wraps the arithmetic as a single basic
 * block so the compiler emits both extractions before the multiplies rather
 * than interleaving them.
 *
 * @param rect  Rectangle to scale in place.
 * @param scale Q12 fixed-point scale factor (@c 0x1000 == 1.0).
 * @param arg2  Unused by this routine (the caller passes @c rateDelta).
 */
void func_8002CDE4(RECT *rect, s32 scale, s32 arg2) {
    s32 x, y, w, h, prodW, prodH;

    if (scale == 0x1000) {
        return;
    }

    x = *(s32 *)&rect->x;
    y = x >> 16;
    x = x << 16;
    x = x >> 16;
    w = *(s32 *)&rect->w;
    h = w >> 16;
    w = w << 16;
    do {
        w = w >> 16;
        x = x + ((u32)w / 2);
        y = y + ((u32)h / 2);
        prodW = w * scale;
        prodH = h * scale;
        w = (u32)prodW / 4096;
        rect->x = x - ((u32)prodW / 8192);
        rect->w = w;
        h = (u32)prodH / 4096;
        rect->y = y - ((u32)prodH / 8192);
        rect->h = h;
    } while (0);
}


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


/** @brief Holds the packed auto-repeat delays for @ref func_8002CECC (@c a0).
 *  @note Real type TBD — a structure that sits before @c g_sfxEntries; the
 *        caller @c func_8002CF54 passes @c &g_sfxEntries-0x220. */
typedef struct {
    u8 pad00[0x1E0];
    u16 delays; /**< 0x1E0: repeatInterval (high byte) | restartDelay (low byte). */
} SfxRepeatBase;

/** @brief Per-item element carrying the per-channel edge masks (@c a1).
 *  @note Real type TBD — a @c 0xC4-byte element within @ref SfxRepeatBase. */
typedef struct {
    u8 pad00[0x10];
    u16 masks[4]; /**< 0x10: per-channel edge mask. */
} SfxRepeatElem;

/**
 * @brief Keyboard-style auto-repeat for one SFX channel's edge bits.
 *
 * Latches @p newVal into @c sys->state.stored[channel] and, using the per-channel
 * mask, decides whether the (masked) edge bits should fire this frame. If the new
 * and previous masked bits overlap (a held cue) it ticks the per-channel countdown
 * @c sys->state.counters[channel]: while it is running the event is suppressed
 * (returns 0), and when it expires the countdown reloads to the repeat interval and
 * fires. When the bits do not overlap the countdown resets to the restart delay and
 * fires. Mirrors @c func_800A29D4 (the Triple Triad edge auto-repeat).
 *
 * @param base    Packed repeat delays (@c base->delays).
 * @param elem    Per-channel edge masks (@c elem->masks).
 * @param sys     SFX system (@c &g_sfxEntries); holds the stored bits and counters.
 * @param newVal  Raw new edge bitmask for this frame.
 * @param channel SFX channel index, 0..3.
 * @return The masked edge bits that should fire this frame, or 0 while suppressed.
 */
s32 func_8002CECC(SfxRepeatBase *base, SfxRepeatElem *elem, SfxSystem *sys, u16 newVal, s32 channel) {
    s32 counter;
    s32 restartDelay;
    s32 repeatInterval;
    u16 mask;
    u16 prevMasked;

    prevMasked = sys->state.stored[channel];
    sys->state.stored[channel] = newVal;
    restartDelay = base->delays;
    counter = sys->state.counters[channel];
    mask = elem->masks[channel];

    repeatInterval = restartDelay >> 8;
    restartDelay &= 0xFF;
    newVal &= mask;
    prevMasked &= mask;
    if (newVal & prevMasked) {
        if (newVal != prevMasked) {
            counter = restartDelay;
        }
        counter--;
        if (counter < 0) {
            counter = repeatInterval;
        } else {
            newVal = 0;
        }
    } else {
        counter = restartDelay;
    }
    sys->state.counters[channel] = counter;
    return newVal;
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002CF54);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002D040);


/**
 * @brief Initialize a sound effect entry for playback.
 *
 * Clears fields, stores the data pointer, sets up the packed flags
 * nibbles, calls clearEntityColor, and sets default volume bytes.
 *
 * @param index SFX entry index.
 * @param data  Pointer to SFX script data (or NULL).
 */
void initSfxPlayback(s32 index, u8 *data) {
    SfxEntry *entry = &g_sfxEntries.entries[index];

    entry->field28 = 0;
    entry->dataPtr = data;
    entry->dataPtrCopy = data;
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


void func_8002D784(s32 arg0, u8 *data, s32 min, s32 max, s32 val, s32 arg5) {
    val = CLAMP(val, min, max);

    initSfxPlayback(arg0, data);
    setSfxEntryTimings(arg0, min, max, arg5);
    setSfxEntryField2B(arg0, val);
}


void func_8002D818(s32 arg0, u8 *str, s32 count, s32 min, s32 max, s32 val, s32 arg6) {
    s32 clamped;
    while (count > 0) {
        while (*str++) { }
        count--;
    }

    clamped = val;
    clamped = CLAMP(clamped, min, max);

    initSfxPlayback(arg0, str);
    setSfxEntryTimings(arg0, min, max, arg6);
    setSfxEntryField2B(arg0, clamped);
}


/**
 * @brief Execute an SFX entry's callback and update colors.
 *
 * If the entry is active (state != 0), switches GP to scratchpad,
 * invokes the entry's callback (field34) if set, updates flash/GPU
 * colors via dispatchSfxColorUpdate, and processes the entry via
 * func_8002CC4C. Restores GP before returning.
 *
 * @param arg0 Value passed as second arg to the callback and func_8002CC4C.
 * @param index SFX entry index.
 * @see https://decomp.me/scratch/mYKYb
 */
void func_8002D8CC(s32 arg0, s32 index) {
    SfxEntry *entry = &g_sfxEntries.entries[index];

    if (entry->flags.fields.state != 0) {
        s32 savedGp;
        s16 saved;
        s32 ret;

        GP_SAVE_SCRATCH(savedGp, getScratchAddr(0xC0));
        /* Narrowing the saved GP to s16 here (it never leaves a register, so no
         * truncation happens) forces the same GP-save register routing the
         * original compiler emitted. */
        saved = savedGp;
        if (entry->field34 != 0) {
            ((void (*)(SfxEntry *, s32))entry->field34)(entry, arg0);
        }
        dispatchSfxColorUpdate(index);
        func_8002CC4C(index, arg0);
        GP_RESTORE_RET(saved, ret);
    }
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002D970);


/**
 * @brief Per-frame update callback for an active SFX battle entity.
 *
 * Installed as the entity's field00 callback by @ref func_8002DF5C, and run with
 * @c $gp pointed at the scratchpad. The linked SFX entry index is held in the
 * entity's @c subFields[0]; if that entry is active (@ref getSfxState), its
 * @c field38 callback is invoked when present, then the entry is processed via
 * @c func_8002D040. Restores @c $gp before returning.
 *
 * @param entity Battle display entity; its @c subFields[0] holds the SFX index.
 * @param arg1   Passed through to the entry callback and @c func_8002D040.
 * @param arg2   Passed through to the entry callback and @c func_8002D040.
 */
void func_8002DBF8(BattleDisplayEntity *entity, s32 arg1, s32 arg2) {
    s32 savedGp;
    s16 saved;
    s32 ret;
    s32 sfxId;

    GP_SAVE_SCRATCH(savedGp, getScratchAddr(0xC0));

    sfxId = entity->subFields[0];
    /* Narrowing the saved GP to s16 (it never leaves a register, so nothing is
     * truncated) forces the v0->s1 GP-save routing the original compiler emitted,
     * matching the sibling func_8002D8CC. */
    saved = savedGp;

    if (getSfxState(sfxId) != 0) {
        SfxEntry *entry = &g_sfxEntries.entries[sfxId];
        if (entry->field38 != 0) {
            ((void (*)(SfxEntry *, s32, s32)) entry->field38)(entry, arg1, arg2);
        }
        func_8002D040(sfxId, arg1, arg2);
    }

    GP_RESTORE_RET(saved, ret);
}


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
 * @return The entity type byte of the linked battle entity.
 */
s32 readSfxEntityType(s32 idx) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    return getBattleEntityType(entry->entityIdx);
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
void initSfxSlot(s32 idx) {
    u8 *nullData = 0;
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
        RECT buf;
        buf.x = 0x40;
        buf.y = 0x40;
        buf.w = 0x80;
        buf.h = 0x80;
        func_8002E064(idx, &buf);
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


/**
 * @brief Configure an SFX entry's rectangle and propagate it to its battle entity.
 *
 * Inflates the source rect inwards by 6 on each side (width/height shrink by 12),
 * derives field23 as the inset height divided by 16 (minimum 1), copies the
 * original source rect into the entry, and forwards the original rect (offset by
 * the entry's stored offsets) to the entity's bound rect. The inset rect is then
 * passed to the entity's clamp rect.
 *
 * @param index SFX entry index.
 * @param srcRect Source rectangle.
 */
void func_8002E064(s32 index, RECT *srcRect) {
    SfxEntry *entry = &g_sfxEntries.entries[index];
    RECT rect;
    s32 entityId;
    SfxEntry *ep = entry;

    entityId = ep->entityIdx;

    rect.x = srcRect->x;
    rect.y = srcRect->y;
    rect.w = srcRect->w;
    rect.h = srcRect->h;
    rect.x += 6;
    rect.y += 6;
    rect.w -= 12;
    rect.h -= 12;

    ep->field23 = rect.h / 16;
    if (ep->field23 == 0) {
        ep->field23 = 1;
    }

    ep->rect = *srcRect;
    rect = *srcRect;

    func_8002CDE4(&rect, ep->field1C, ep->rateDelta);
    setBattleEntityBoundRect(entityId, &rect);

    rect.x += 6;
    rect.y += 6;
    rect.w -= 12;
    rect.h -= 12;
    setBattleEntityRectClamp(entityId, &rect);
}


/**
 * @brief Store a numeric value into one of the 8 message-value slots.
 *
 * The index is clamped to [0, 7]. Used by message formatting to pass
 * numeric arguments into decodeMessage.
 *
 * @param index Slot index (clamped to 0..7).
 * @param value Value to store.
 */
void func_8002E1B4(s32 index, s32 value) {
    SfxSystem *data = &g_sfxEntries;
    s32 clamped = CLAMP(index, 0, 7);
    data->msgValues[clamped] = value;
}


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
    sys->state.stored[0] = 0;
    sys->state.stored[1] = 0;
    sys->state.counters[0] = 0;
    sys->state.counters[1] = 0;
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


/**
 * @brief One 8-byte sprite cell of a glyph in the @c D_80052A68 font table.
 *
 * A glyph is drawn from one or more of these cells. @c texInfo carries the PS1
 * sprite attributes (texture page / CLUT / UV) and @c metrics packs the cell's
 * placement as four bytes: @c x + (s8)@c xExtent give the cell's right edge,
 * @c y + (s8)@c yExtent its bottom edge.
 */
typedef struct {
    /* 0x00 */ u32 texInfo; /**< PS1 sprite/texture attributes for the cell. */
    /* 0x04 */ u32 metrics; /**< x | (s8)xExtent<<8 | y<<16 | (s8)yExtent<<24. */
} GlyphCell;

/**
 * @brief Header view of the @c D_80052A68 font table (baked into executable data).
 *
 * A glyph count followed by one descriptor per glyph. Each descriptor packs the
 * glyph's cell @c count (high 16 bits) and the byte offset from the table base
 * to that glyph's @ref GlyphCell list (low 16 bits). The cell lists themselves
 * live in the trailing area the descriptors point at.
 */
typedef struct {
    /* 0x00 */ u32 glyphCount;
    /* 0x04 */ u32 descriptors[1]; /**< cellCount<<16 | byteOffsetToCells. */
} GlyphTable;

/** @brief Font glyph table (glyph count + per-glyph descriptors + cell lists). */
extern GlyphTable D_80052A68;

/**
 * @brief Compute the bounding width of a multi-cell glyph.
 *
 * Walks glyph @p idx's cells and tracks the unsigned running maximum of each
 * cell's right edge (@c x + (s8)@c xExtent) and bottom edge (@c y +
 * (s8)@c yExtent), returning the peak width (low byte).
 *
 * @note The peak height is computed but unused by the return value — the caller
 *       presumably only needs the advance width. Purpose inferred from the
 *       neighbouring glyph-metric routines.
 *
 * @param idx Glyph index into @c D_80052A68.
 * @return Bounding width of the glyph, masked to 8 bits.
 */
s32 func_8002E3A4(s32 idx) {
    GlyphCell *cell = (GlyphCell *)&D_80052A68;
    u32 word = D_80052A68.descriptors[idx];
    s32 cellCount = word >> 16;
    s32 maxWidth = 0;
    s32 maxHeight = 0;

    word &= 0xFFFF;
    cell = (GlyphCell *)((u8 *)cell + word);
    while (cellCount != 0) {
        s32 width, height;
        word = cell->metrics;
        width = word & 0xFF;
        width += (s8)(word >> 8);
        height = (word >> 16) & 0xFF;
        height += (s8)(word >> 24);
        if ((u32)maxWidth < (u32)width) {
            maxWidth = width;
        }
        if ((u32)maxHeight < (u32)height) {
            do { maxHeight = height; } while (0);
        }
        cellCount--;
        cell++;
    }
    return maxWidth & 0xFF;
}


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


/**
 * @brief Remap a battle SFX id into a range, then extract its 4-bit nibble.
 *
 * Folds the SFX id @p idx into a compact table index depending on which of
 * three ranges it falls in (< 0x100, < 0x1C00, or higher — the last range is
 * offset and tagged with bit 0x400), then reads the packed nibble table
 * @c D_800834D8 exactly like @ref getNibbleValue: even indices take the low
 * nibble, odd indices the high nibble.
 *
 * @param idx SFX id to look up.
 * @return The 4-bit table value (0-15).
 */
s32 func_8002E454(s32 idx) {
    u8 *base;
    u32 val;

    if (idx < 0x100) {
        idx -= 0x20;
    } else if (idx < 0x1C00) {
        idx -= 0x1820;
    } else {
        idx -= 0x1C20;
        idx |= 0x400;
    }

    base = D_800834D8;
    val = base[idx >> 1];
    if (idx & 1) {
        val >>= 4;
    }
    return val & 0xF;
}


/**
 * @brief Measure a battle-message string's packed {width, height}.
 *
 * Scans the string @p s, interpreting control codes (1/2/7 = line breaks,
 * 5 = special glyph resolved via @c func_8002C734 / @c func_8002E3A4,
 * 0x18-0x1F = multi-byte glyphs) and accumulating each line's pixel width from
 * the packed nibble-width table @ref D_800834D8. Tracks the widest line and the
 * line count; codes below 0x10 (other than 1/2/5/7) consume a following
 * argument byte. When @p flag is 0 the scan stops at the first line break.
 *
 * @param s    Null-terminated message string.
 * @param flag If nonzero, measure every line; if zero, stop at the first break.
 * @return Packed dimensions: width in the low 16 bits, @c (lines*16 - 4) in the
 *         high 16 bits.
 */
s32 func_8002E4AC(u8 *s, s32 flag) {
    s32 width = 0;
    s32 maxWidth = 0;
    s32 lines = 1;
    s32 maxLines = 1;
    s32 height;

    for (;;) {
        s32 c = *s++;
        if (c == 0) {
            if (maxWidth < width) {
                maxWidth = width;
            }
            if (maxLines < lines) {
                maxLines = lines;
            }
            height = maxLines * 16;
            break;
        }
        if ((u32) (c - 1) < 2 || c == 7) {
            /* control codes 1, 2, 7: line break */
            lines++;
            if (c == 1) {
                lines = 1;
            }
            if (c == 7) {
                lines = 1;
            }
            if (maxLines < lines) {
                maxLines = lines;
            }
            if (maxWidth < width) {
                maxWidth = width;
            }
            width = 0;
            if (flag == 0) {
                height = maxLines * 16;
                break;
            }
        } else if (c == 5) {
            c = *s++;
            width += func_8002E3A4(func_8002C734(c));
            width++;
        } else if (c < 0x10) {
            s++;
        } else if (c >= 0x18) {
            u8 packed;
            s32 nib;
            if (c >= 0x20) {
                c = c - 0x20;
            } else if (c < 0x1C) {
                c *= 0xE0;
                c += *s++;
                c -= 0x1520;
            } else {
                c *= 0xE0;
                c += *s++;
                c -= 0x18A0;
                c |= 0x400;
            }
            packed = D_800834D8[c >> 1];
            if (c & 1) {
                packed >>= 4;
            }
            nib = packed & 0xF;
            width += nib;
        }
    }

    return maxWidth | (((maxLines * 16) - 4) << 16);
}


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E680);


/** @brief Get a string's packed {width, height} (variant A). */
s32 getGlyphWidthA(u8 *code) {
    return func_8002E4AC(code, 1);
}


/** @brief Get glyph width (variant B). */
void getGlyphWidthB(u8 *code) {
    func_8002E4AC(code, 1);
}


/** @brief Get glyph width as u16. */
u16 getGlyphWidthU16(u8 *code) {
    return (u16)func_8002E4AC(code, 1);
}


/**
 * @brief Get glyph status as u16.
 * @param code Glyph code.
 * @return Status value truncated to 16 bits.
 */
u16 getGlyphStatusU16(u8 *code) {
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


/**
 * @brief Build a tile sprite GPU primitive for a font glyph.
 *
 * Sets up a 12x12 tile sprite with OT linkage, palette, tpage,
 * color (from D_8008384C or g_menuColor based on palette page),
 * and UV coordinates computed from the tile index (21 tiles per row).
 * Uses swl-based setaddr (non-standard OT macro variant).
 *
 * @param ot   Current OT link value.
 * @param prim Pointer to the tile sprite primitive to fill.
 * @param tileIdx Tile index (bit 10 selects alternate tpage).
 * @param palArg  Palette argument (bits 2:0 = palette, bits 7:3 = page).
 * @param xy   Packed x|y position.
 * @return Updated OT link value for the next primitive.
 * @see https://decomp.me/scratch/IELV1
 */
INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E810);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002E8DC);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002EAD0);


INCLUDE_ASM("asm/nonmatchings/btl_sfx", func_8002EE10);


