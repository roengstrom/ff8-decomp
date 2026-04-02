#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "gamestate.h"

/* --- Local type definitions --- */

typedef struct {
    s16 f0;
    s8 f2;
    s8 f3;
    s16 f4;
    u8 f6;
    u8 f7;
} BattleCameraState;

typedef struct {
    u8 data[4];
    s16 lerpStart;
    s16 lerpEnd;
    s16 value;
    s16 lerpParam;
    s16 result;
    u8 flags;
    u8 pad0F;
} AnimEntry;

typedef struct {
    s16 f0;
    s16 f2;
    s16 f4;
    s16 f6;
    u8 f8;
    u8 f9;
} Struct3754;

/* --- Externs (sorted by address) --- */

extern u8 D_80052A34[];              /* 0x80052A34 — SPU command table */
extern AnimEntry D_80083772[];       /* 0x80083772 — animation entry table */
extern Struct3754 D_80083754;        /* 0x80083754 — transition state */
extern u8 D_80083756;                /* 0x80083756 — transition flag */
extern BattleCmdEntry g_battleCmdTable[];   /* 0x80083878 — battle command entries (4 × 0x24) */
extern s32 D_80083918;               /* 0x80083918 — battle OT buffer index */
extern s32 D_80083920[];             /* 0x80083920 — battle OT buffers */
extern u8 D_80083938[];              /* 0x80083938 — battle OT data */
extern u8 D_80085134[];              /* 0x80085134 — battle display buffer */
extern BattleCameraState g_cameraShake; /* 0x800834D0 */
extern s32 g_gpuColor;               /* 0x800834C8 — GPU color value */
extern u16 g_cameraVibrateIntensity;               /* 0x800834D4 — camera shake color */
extern s32 D_80083750;               /* 0x80083750 — battle timer */

/**
 * @brief Clear the RGB color bytes at offsets 0x20, 0x21, and 0x22 of a structure.
 * @param a0 Pointer to the base of the structure whose color fields are zeroed.
 */
void clearEntityColor(SfxEntry *entry) {
    entry->field20 = 0;
    entry->field22 = 0;
    entry->field21 = 0;
}


INCLUDE_ASM("asm/nonmatchings/btl_color", func_8002FF34);


/**
 * @brief Build a packed grayscale GPU color and store to g_gpuColor.
 * @param intensity Scalar intensity value (divided by 32, masked to 8 bits).
 */
void buildGrayscaleGpuColor(s32 intensity) {
    intensity /= 32;
    intensity &= 0xFF;
    g_gpuColor = intensity | (intensity << 8) | (intensity << 16) | 0x64000000;
}


/**
 * @brief Build a packed RGB GPU color and store to g_gpuColor.
 * @param r Red intensity (divided by 32, masked to 8 bits).
 * @param g Green intensity.
 * @param b Blue intensity.
 */
void buildRgbGpuColor(s32 r, s32 g, s32 b) {
    r /= 32;
    g /= 32;
    b /= 32;
    r &= 0xFF;
    g &= 0xFF;
    b &= 0xFF;
    g_gpuColor = r | (g << 8) | (b << 16) | 0x64000000;
}


INCLUDE_ASM("asm/nonmatchings/btl_color", func_800300F8);


/** @brief Call buildGrayscaleGpuColor with the default parameter value 0x1000. */
void setDefaultGpuColor(void) { buildGrayscaleGpuColor(0x1000); }


/** @brief Empty stub -- no operation. */
void btlColorStub0234(void) {
}


/**
 * @brief Set the global 16-bit value g_cameraVibrateIntensity.
 * @param val Value to store.
 */
void setCameraVibrateIntensity(s32 val) {
    g_cameraVibrateIntensity = val;
}


/**
/**
 * @brief Set battle camera vibration state.
 *
 * Always stores the control value to g_cameraShake.f3. If non-zero,
 * also resets the vibration counter (f6) and snapshots the current
 * battle state flag into f7 for change detection.
 *
 * @param enable Vibration enable flag.
 */
void setCameraVibrateState(unsigned int enable) {

    g_cameraShake.f3 = enable;
    if (enable != 0) {
        g_cameraShake.f6 = 0;
        g_cameraShake.f7 = (s8)((volatile GameState *)(&g_gameState))->battleStateFlag;
    }
}


/** @brief Set camera shake intensity and direction.
 *  @param intensity Shake intensity (stored as s16).
 *  @param direction Shake direction (stored as s8).
 */
void setCameraShakeParams(s32 intensity, s32 direction) {
    g_cameraShake.f0 = intensity;
    g_cameraShake.f2 = direction;
}


/**
 * @brief Update camera vibration timer and check for game state change.
 *
 * Increments g_cameraShake.f6 (clamped to 0x40), then compares the low byte of
 * g_gameState.battleStateFlag against g_cameraShake.f7. If they differ, resets
 * f6 to 0 and updates f7 to the current battle state byte.
 */
void updateCameraVibrate(void) {
    s32 counter;
    s32 clamped;
    s32 gsVal;
    s32 curVal;

    counter = g_cameraShake.f6;
    counter++;
    clamped = 0x40;
    if (counter < 0x41U) {
        clamped = counter;
    }
    g_cameraShake.f6 = clamped;
    gsVal = ((volatile GameState *)(&g_gameState))->battleStateFlag;
    curVal = g_cameraShake.f7;
    gsVal &= 0xFF;
    counter = gsVal;
    if (counter != curVal) {
        g_cameraShake.f6 = 0;
        g_cameraShake.f7 = counter;
    }
}


/**
 * @brief Render battle HUD text (HP, damage numbers, status text).
 *
 * Checks the camera vibration state and game mode, converts battle
 * values to display strings via intToDecStringShort, and renders them
 * as GPU sprite text using func_8002FF34. Links the result into the OT.
 *
 * @param ot OT entry pointer.
 * @param cursor Packet buffer pointer.
 * @return Updated cursor past all emitted packets.
 *
 * @see https://decomp.me/scratch/CEsOX
 */
INCLUDE_ASM("asm/nonmatchings/btl_color", func_800302DC);


INCLUDE_ASM("asm/nonmatchings/btl_color", func_80030518);


/**
 * @brief Reset the BattleCameraState struct (g_cameraShake) to default values.
 *
 * Clears all fields to zero except f4, which is set to 0x1000 (default zoom/distance).
 */
void resetBattleCameraState(void) {
    g_cameraShake.f3 = 0;
    g_cameraShake.f4 = 0x1000;
    g_cameraShake.f0 = 0;
    g_cameraShake.f2 = 0;
    g_cameraShake.f7 = 0;
    g_cameraShake.f6 = 0;
}



/**
 * @brief Get the battle command table.
 * @return Pointer to the battle command entry array.
 */
BattleCmdEntry *getBattleCmdTable(void) {
    return g_battleCmdTable;
}

/**
 * @brief Find the battle command entry with the lowest non-zero active value
 *        that is still greater than or equal to the threshold.
 *
 * Scans 4 entries (stride 0x24) from getBattleCmdTable(). For each entry,
 * checks if the signed byte at +0x22 is non-zero and >= threshold a0.
 * If the entry's value is also less than the current best, updates best.
 * If no valid entry is found, returns NULL. If an entry with active==0 is found,
 * returns a pointer to that entry directly.
 *
 * @param a0 Threshold: entries with active >= a0 are candidates.
 * @return Pointer to the best (lowest active) entry, or 0 if none found.
 */
s32 findBestBattleCmd(s32 a0) {
    u8 *ptr = (u8 *)getBattleCmdTable();
    s32 best = 0xFF;
    s32 i = 0;
    s32 active;

top:
    do {
        active = *(s8 *)(ptr + 0x22);
        if (active == 0) {
            return (s32)ptr;
        }
        if (a0 < active) goto skip;
        if (active >= best) goto skip;
    } while (0);
    best = i;
skip:
    i++;
    if (i < 4) {
        ptr += 0x24;
        goto top;
    }
    if (best != 0xFF) {
        ptr = getBattleCmdTable();
        ptr += (best * 9) * 4;
        return (s32)ptr;
    }
    return 0;
}


/**
 * @brief Check if any battle command entry is active.
 *
 * Iterates over 4 entries (stride 0x24) from getBattleCmdTable(). Returns 1
 * immediately if any entry has a non-zero signed byte at offset 0x22 (active
 * flag), or 0 if all are zero.
 *
 * @return 1 if any entry is active, 0 otherwise.
 */
s32 isAnyBattleCmdActive(void) {
    u8 *ptr = (u8 *)getBattleCmdTable();
    s32 i = 0;
    s32 active;
top:
    active = *(s8 *)(ptr + 0x22);
    i++;
    if (active != 0) {
        return 1;
    }
    if (i < 4) {
        ptr += 0x24;
        goto top;
    }
    return 0;
}


/**
 * @brief Check if a battle command matches the expected source entity.
 *
 * Returns 0 if a0 is zero. Otherwise, looks up the entry at
 * getBattleCmdTable()[(a0 & 3) * 36], checks if byte 0x22 is nonzero,
 * then compares halfword 0x20 against (a0 >> 4).
 *
 * @param a0 Packed command: bits [1:0] = entry index, bits [15:4] = source ID.
 * @return 1 if valid and source matches, 0 otherwise.
 */
s32 checkBattleCmdSource(s32 a0) {
    BattleCmdEntry *base;
    BattleCmdEntry *entry;

    if (a0 == 0) {
        return 0;
    }
    base = getBattleCmdTable();
    entry = &base[a0 & 3];
    if (entry->active != 0) {
        if (entry->sourceId == (a0 >> 4)) {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief Deactivate a battle command entry or clear all entries.
 *
 * If a0 is -1, clears all 4 entries' active byte and calls setAnimEntityParams
 * with all zeros. Otherwise, validates the command via checkBattleCmdSource
 * and clears just that entry's active byte.
 *
 * @param a0 Packed command identifier, or -1 to clear all.
 */
void deactivateBattleCmd(s32 a0) {
    u8 *base = (u8 *)getBattleCmdTable();
    s32 i;

    if (a0 == -1) {
        i = 3;
    top:
        *(u8 *)(base + 0x22) = 0;
        i--;
        base += 0x24;
        if (i >= 0) {
            goto top;
        }
        setAnimEntityParams(0, 0, 0);
        return;
    }
    if (checkBattleCmdSource(a0)) {
        s32 idx = a0 & 3;
        base += (idx * 9) * 4;
        *(u8 *)(base + 0x22) = 0;
    }
}


INCLUDE_ASM("asm/nonmatchings/btl_color", func_8003093C);


INCLUDE_ASM("asm/nonmatchings/btl_color", func_80030A54);


INCLUDE_ASM("asm/nonmatchings/btl_color", func_80030B2C);


/**
 * @brief Add to the battle timer and call func_80030B2C for each full tick.
 *
 * Adds @p a0 to D_80083750. While the counter >= 4, subtracts 4 and calls
 * func_80030B2C(). Stores the remainder back to D_80083750.
 *
 * @param a0 Amount to add to the timer.
 */
void advanceBattleTimer(s32 a0) {
    s32 counter = D_80083750;
    counter += a0;
top:
    if (counter >= 4) {
        func_80030B2C();
        counter -= 4;
        goto top;
    }
    D_80083750 = counter;
}


/**
 * @brief Initialize 4 battle command entries and clear D_80083750.
 *
 * Calls getBattleCmdTable() to get the entry table base, then for each of
 * the 4 entries (stride 0x24, starting at offset 0x20): sets the index
 * at byte +3, clears byte +2, and sets halfword +0 to 1.
 * Finally zeroes D_80083750.
 */
void initBattleCmdEntries(void) {
    u8 *base = (u8 *)getBattleCmdTable();
    s32 i = 0;
    s32 one = 1;
    u8 *ptr = base + 0x20;
top:
    ptr[3] = i;
    i++;
    ptr[2] = 0;
    *(s16 *)ptr = one;
    ptr += 0x24;
    if (i < 4) goto top;
    D_80083750 = 0;
}


/**
 * @brief Send an SPU command looked up from table D_80052A34.
 *
 * Uses the input as an index into D_80052A34 to retrieve a command byte,
 * then passes it to sndKeyOn which writes it to the SPU command buffer
 * (D_80075058) and triggers SPU processing.
 *
 * @param a0 Index into the D_80052A34 lookup table.
 */
void sendSpuCommand(s32 a0) {
    sndKeyOn(D_80052A34[a0]);
}


/**
 * @brief Play sound effect from D_80052A34 table.
 *
 * Loads a byte from D_80052A34[a0] and calls sndPlaySfx
 * with default volume/pan parameters (0, 0x80, 0x7F).
 *
 * @param a0 Index into D_80052A34 sound table.
 */
void playSoundEffect(s32 a0) {
    u8 *ptr = D_80052A34 + a0;
    sndPlaySfx(*ptr, 0, 0x80, 0x7F);
}


/**
 * @brief Configure sound reverb channels based on a bitmask.
 *
 * Reads hardware state via func_80047384, optionally pauses/resumes audio
 * hardware. Mutes master volume, then enables reverb on channels indicated
 * by bits 0-2 of a0. If a0 == 7, enables reverb on channel 0 (all).
 *
 * @param a0 Bitmask of reverb channels to enable (bits 0, 1, 2).
 */
void enableSoundReverb(s32 a0) {
    s32 hwState = func_80047384();

    if (!(hwState & 4)) {
        func_800472E4();
    }
    sndSetMasterVolume(0);
    if (a0 == 7) {
        sndEnableReverb(0);
    } else {
        if (a0 & 1) {
            sndEnableReverb(1);
        }
        if (a0 & 2) {
            sndEnableReverb(2);
        }
        if (a0 & 4) {
            sndEnableReverb(3);
        }
    }
    if (!(hwState & 4)) {
        func_800472F4();
    }
}


/**
 * @brief Disable sound reverb channels based on a bitmask and restore volume.
 *
 * Reads hardware state via func_80047384, optionally pauses/resumes audio
 * hardware. Disables reverb on channels indicated by bits 0-2 of a0,
 * then restores master volume to 0x7F.
 *
 * @param a0 Bitmask of reverb channels to disable (bits 0, 1, 2).
 */
void disableSoundReverb(s32 a0) {
    s32 hwState = func_80047384();

    if (!(hwState & 4)) {
        func_800472E4();
    }
    if (a0 == 7) {
        sndDisableReverb(0);
    } else {
        if (a0 & 1) {
            sndDisableReverb(1);
        }
        if (a0 & 2) {
            sndDisableReverb(2);
        }
        if (a0 & 4) {
            sndDisableReverb(3);
        }
    }
    sndSetMasterVolume(0x7F);
    if (!(hwState & 4)) {
        func_800472F4();
    }
}


/**
 * @brief Remap a party bitmask through the game state remap table.
 *
 * When bit 0x20 of the game state flags at offset 0xAE4 is set, each set
 * bit in the lower 12 bits of a0 is remapped through the table at
 * g_gameState+0xAE8. The upper 4 bits (0xF000) are preserved unchanged.
 * Returns the original value if remapping is inactive.
 *
 * @param a0 Party bitmask (lower 12 bits = party members, upper 4 = flags).
 * @return Remapped bitmask as u16, or original a0 if remapping inactive.
 */
INCLUDE_ASM("asm/nonmatchings/btl_color", remapPartyBitmask);


INCLUDE_ASM("asm/nonmatchings/btl_color", remapBattlePalette);


INCLUDE_ASM("asm/nonmatchings/btl_color", reversePaletteRemap);


/** @brief Empty stub -- no operation. */
void btlColorStub1044(void) {
}


INCLUDE_ASM("asm/nonmatchings/btl_color", func_8003104C);


/**
 * @brief Render a null-terminated string, skipping separator characters.
 *
 * Iterates through each byte of the string at @p str. If the character is
 * zero (null terminator), returns the current packet pointer. If the
 * character equals 7 (separator), skips it but advances the y position.
 * Otherwise calls func_8002FF34 to render the character glyph.
 *
 * @param a0 OT base pointer.
 * @param a1 Current packet buffer pointer.
 * @param a2 Pointer to null-terminated string.
 * @param a3 Starting y position.
 * @param arg4 Width parameter (passed on stack to func_8002FF34).
 * @param arg5 Color parameter (passed on stack to func_8002FF34).
 * @return Updated packet buffer pointer after rendering.
 */
s32 func_80031188(s32 a0, s32 a1, u8 *a2, s32 a3, s32 arg4, s32 arg5) {
    s32 ot = a0;
    u8 *str = a2;
    s32 yPos = a3;
    s32 width = arg4;
    s32 sep = 7;
    u8 ch;
    s32 color = arg5;

    do {
    top:
        ch = *str++;
        if (ch == 0) {
            return a1;
        }
        if (ch == sep) goto skip;
        a1 = func_8002FF34(ot, a1, ch, yPos, width, color);
    } while (0);
skip:
    yPos = yPos + 9;
    goto top;
}


INCLUDE_ASM("asm/nonmatchings/btl_color", func_80031224);


INCLUDE_ASM("asm/nonmatchings/btl_color", func_80031364);


// init_battle_transition - initializes D_80083754 (Struct3754)

/** @brief Sets D_80083754.f0 to 7. */
void setTransitionPhase7(void) {
    D_80083754.f0 = 7;
}


INCLUDE_ASM("asm/nonmatchings/btl_color", func_800316D4);


/** @brief Stores a byte to global D_80083756. */
void setTransitionFlag(s32 a0) {
    D_80083756 = a0;
}


/**
 * @brief Initialize the battle transition struct D_80083754 to its starting state.
 *
 * Sets the transition phase to 9, clears the timer fields (f4, f6), and resets
 * the flag bytes (f8, f9) to zero.
 */
void initBattleTransition(void) {
    D_80083754.f0 = 9;
    D_80083754.f4 = 0;
    D_80083754.f6 = 0;
    D_80083754.f8 = 0;
    D_80083754.f9 = 0;
}


/**
 * @brief Perform linear interpolation within a range.
 *
 * Returns 0 if a2 < a0 (below range start), a3 if a2 >= a1 (at or above range end),
 * or a proportional value between 0 and a3 based on where a2 falls in [a0, a1).
 *
 * @param a0 Range start (minimum input value).
 * @param a1 Range end (maximum input value).
 * @param a2 Current input value to interpolate.
 * @param a3 Maximum output value (returned when a2 >= a1).
 * @return Interpolated value in [0, a3].
 */
s32 lerpRange(s32 a0, s32 a1, s32 a2, s32 a3) {
    if (a2 < a0) {
        return 0;
    }
    if (a2 >= a1) {
        return a3;
    }
    a2 -= a0;
    a1 -= a0;
    return a2 * a3 / a1;
}


INCLUDE_ASM("asm/nonmatchings/btl_color", stepAnimEntries);


INCLUDE_ASM("asm/nonmatchings/btl_color", func_80031A18);


/**
 * @brief Render both animation overlay channels.
 *
 * Builds a grayscale GPU color from g_cameraVibrateIntensity, then calls func_80031A18
 * twice (once for each channel, indices 0 and 1).
 *
 * @param a0 OT base pointer.
 * @param a1 Packet buffer pointer.
 * @return Updated packet pointer (from last func_80031A18 call).
 */
/**
 * @brief Render both animation overlay channels.
 *
 * Builds a grayscale GPU color from g_cameraVibrateIntensity, then calls func_80031A18
 * twice (once for each channel, indices 0 and 1).
 *
 * @param a0 OT base pointer.
 * @param a1 Packet buffer pointer.
 * @return Updated packet pointer (from last func_80031A18 call).
 */
INCLUDE_ASM("asm/nonmatchings/btl_color", renderAnimOverlay);


/**
 * @brief Clear bit 7 of animation entry flags.
 *
 * Clears the high bit of the byte at offset 0xE in D_80083772[a0]
 * (stride 16), effectively marking the entry as inactive.
 *
 * @param a0 Entry index into D_80083772.
 */
void clearAnimEntryActive(s32 a0) {
    s32 base = (s32)D_80083772;
    base = a0 * 16 + base;
    *(u8 *)(base + 0xE) &= 0x7F;
}


/**
 * @brief Update a D_80083772 entry with position and optionally store result.
 *
 * Calls lerpRange with fields from the entry at index a0 (stride 16).
 * Always stores a1 to entry offset 0x8. If bit 1 of entry byte 0xE is set,
 * also stores the return value to entry offset 0xC.
 *
 * @param a0 Entry index into D_80083772 (stride 16 bytes).
 * @param a1 Value to store at entry offset 0x8.
 */
void updateAnimEntry(s32 a0, s32 a1) {
    s32 base = (s32)D_80083772;
    s16 result;

    base = a0 * 16 + base;
    result = lerpRange(*(s16 *)(base + 4), *(s16 *)(base + 6), a1, *(s16 *)(base + 0xA));
    *(s16 *)(base + 8) = a1;
    if (*(u8 *)(base + 0xE) & 2) {
        *(s16 *)(base + 0xC) = result;
    }
}


/**
 * @brief Copy 4-byte field from source to D_80083772 entry.
 *
 * @param a0 Entry index into D_80083772 (stride 16 bytes).
 * @param src Source for the 4-byte unaligned copy.
 */
void copyAnimEntryField(s32 a0, void *src) {
    typedef struct { s16 a, b; } S16Pair;
    s32 base = (s32)D_80083772;
    base += a0 * 16;
    *(S16Pair *)base = *(S16Pair *)src;
}


/**
 * @brief Initialize an animation entry with full parameters.
 *
 * Sets up the D_80083772 entry at index a0 with source data, interpolation
 * range, current/target values, and the active flag. Calls copyAnimEntryField
 * for the first 4 bytes, then stores remaining fields and calls lerpRange
 * to compute the initial interpolated value.
 *
 * @param a0 Entry index.
 * @param a1 Flags byte (ORed with 0x80 for active).
 * @param a2 Source data pointer (first 4 bytes copied).
 * @param a3 Interpolation start value.
 * @param arg4 Interpolation end value (on stack).
 * @param arg5 Current position (on stack).
 * @param arg6 Target max value (on stack).
 */
void initAnimEntry(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4, s32 arg5, s32 arg6) {
    s32 base = (s32)D_80083772;
    s32 flags = a1 | 0x80;

    base = a0 * 16 + base;
    copyAnimEntryField(a0, (void *)a2);
    *(u8 *)(base + 0xE) = flags;
    *(s16 *)(base + 4) = a3;
    *(s16 *)(base + 6) = arg4;
    *(s16 *)(base + 8) = arg5;
    *(s16 *)(base + 0xA) = arg6;
    *(s16 *)(base + 0xC) = lerpRange(a3, arg4, arg5, arg6);
}



/**
 * @brief Call func_80031E1C with truncated a1 and constant 7th arg (0x60).
 *
 * Passes through all 6 caller args, truncates a1 to u8,
 * and appends 0x60 as the 7th argument.
 */
void setupAnimEntry(s32 a0, u8 a1, s32 a2, s32 a3, s32 arg4, s32 arg5) {
    initAnimEntry(a0, a1, a2, a3, arg4, arg5, 0x60);
}


/**
 * @brief Call func_80031E1C with truncated a1, passing all 7 args through.
 *
 * Truncates a1 to u8 and passes all caller arguments
 * through to func_80031E1C.
 */
void setupAnimEntryFull(s32 a0, u8 a1, s32 a2, s32 a3, s32 arg4, s32 arg5, s32 arg6) {
    initAnimEntry(a0, a1, a2, a3, arg4, arg5, arg6);
}


/**
 * @brief Clear animation entry active flags.
 *
 * Iterates over 2 entries in D_80083772 (stride 0x10) and sets the byte at
 * offset 0x0E to zero for each, marking them as inactive.
 */
void clearAnimEntries(void) {
    AnimEntry *entry = D_80083772;
    s32 i = 1;
    do {
        entry->flags = 0;
        i--;
        entry++;
    } while (i >= 0);
}



/**
 * @brief Get a pointer to the global buffer D_80085134.
 * @return Address of D_80085134.
 */
u8 *getBattleBuffer1(void) {
    return D_80085134;
}

/**
 * @brief Get a pointer to the global buffer D_80083938.
 * @return Address of D_80083938.
 */
u8 *getBattleBuffer2(void) {
    return D_80083938;
}

/**
 * @brief Spin-wait until a VSync event is received.
 *
 * Polls func_8004D208 (VSync) with argument 1 in a busy loop until it
 * returns a value other than -1, indicating the vertical blank has occurred.
 */
void waitBattleVSync(void) {
    do {
    } while (func_8004D208(1) == -1);
}


/**
 * @brief Return the base address of the battle allocation region.
 * @return 0x801F4000 (fixed address in PS1 RAM).
 */
u32 getBattleAllocBase(void) {
    return 0x801F4000;
}


/**
 * @brief Return the size of the battle allocation region.
 * @return 0x4000 (16384 bytes / 16 KB).
 */
s32 getBattleAllocSize(void) {
    return 0x4000;
}


/**
 * @brief Flip the double-buffered ordering table and initialize the new back buffer.
 *
 * Selects the OT buffer that is not currently active (from D_80083920[0..1]),
 * sets it as the active buffer in D_80083918, clears its 2-entry ordering tag
 * at offset 0x70 via ClearOTag, and initializes the primitive allocation pointer
 * at offset 0x78 to point to offset 0x7C (start of free space).
 */
void flipBattleOtBuffer(void) {
    s32 buf;
    s32 ptr;

    buf = D_80083920[0];
    if (D_80083918 == buf) {
        buf = D_80083920[1];
    }
    D_80083918 = buf;
    ClearOTag((u32 *)(buf + 0x70), 2);
    ptr = D_80083918;
    *(s32 *)(ptr + 0x78) = ptr + 0x7C;
}


INCLUDE_ASM("asm/nonmatchings/btl_color", func_80032010);


INCLUDE_ASM("asm/nonmatchings/btl_color", func_800320BC);


INCLUDE_ASM("asm/nonmatchings/btl_color", renderBattleFrame);


