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
    u8 data[4];       /* 0x00 */
    s16 rangeStart;   /* 0x04 */
    s16 rangeEnd;     /* 0x06 */
    s16 inputStart;   /* 0x08 */
    s16 inputEnd;     /* 0x0A */
    s16 current;      /* 0x0C */
    u8 flags;         /* 0x0E */
    u8 pad;           /* 0x0F */
} AnimEntry; /* 0x10 */

/** @brief Palette transition state machine (D_80083754). */
typedef struct {
    u16 state;      /* 0x00 */
    u16 pad02;      /* 0x02 */
    s16 brightness; /* 0x04 */
    s16 fade;       /* 0x06 */
    u8 srcPalette;  /* 0x08 */
    u8 dstPalette;  /* 0x09 */
    u8 timer;       /* 0x0A */
} PaletteTransition;

/* --- Externs (sorted by address) --- */

extern u8 g_battleSfxTable[];              /* 0x80052A34 — SPU command table */
extern AnimEntry D_80083772[];       /* 0x80083772 — animation entry table */
extern PaletteTransition D_80083754;     /* 0x80083754 — palette transition state */
extern u8 D_80083756;                /* 0x80083756 — transition flag */
extern BattleCmdEntry g_battleCmdTable[];   /* 0x80083878 — battle command entries (4 × 0x24) */
/** @brief Battle HUD OT buffer (smaller display list for UI elements). */
typedef struct {
    u8 pad00[0x70];          /* 0x00 */
    u32 ot[2];               /* 0x70: 2-entry ordering table */
    u32 pktAlloc;            /* 0x78: current packet allocation pointer */
    u32 pktBase;             /* 0x7C: packet buffer start */
} HudDisplayBuf;

extern HudDisplayBuf *D_80083918;    /* 0x80083918 — active HUD display buffer */
extern HudDisplayBuf *D_80083920[];  /* 0x80083920 — HUD display buffer pair */
extern u8 D_80083938[];              /* 0x80083938 — battle OT data */
extern u8 D_80085134[];              /* 0x80085134 — battle display buffer */
extern BattleCameraState g_cameraShake; /* 0x800834D0 */
extern s32 g_gpuColor;               /* 0x800834C8 — GPU color value */
extern u16 g_cameraVibrateIntensity;               /* 0x800834D4 — camera shake color */
extern s32 g_battleTimer;               /* 0x80083750 — battle timer */

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
 * @brief Find the best available battle command entry.
 *
 * Scans the 4-entry battle command table for a free or low-priority slot.
 * If any entry has active == 0, returns it immediately (first-fit).
 * Otherwise, returns the entry with the lowest active value that is
 * still <= threshold. Returns NULL if no suitable entry is found.
 *
 * @param threshold Maximum active value to consider as a candidate.
 * @return Pointer to the best entry, or NULL if none found.
 */
BattleCmdEntry* findBestBattleCmd(s32 threshold) {
    BattleCmdEntry* ptr;
    s32 best;
    s32 i;

    ptr = getBattleCmdTable();
    ptr++; ptr--; /* Regalloc: boost ptr priority */
    best = 0xFF;

    for (i = 0; i < 4; i++, ptr++) {
        if (ptr->active == 0) {
            return ptr;
        }
        if (threshold >= ptr->active && ptr->active < best) {
            best = i;
        }
    }

    if (best != 0xFF) {
        return &getBattleCmdTable()[best];
    }
    return 0;
}


/**
 * @brief Check if any battle command entry is active.
 *
 * Scans the 4-entry battle command table. Returns 1 immediately if any
 * entry has a non-zero active flag, or 0 if all are inactive.
 *
 * @return 1 if any entry is active, 0 otherwise.
 */
s32 isAnyBattleCmdActive(void) {
    BattleCmdEntry* ptr = getBattleCmdTable();
    s32 i;

    for (i = 0; i < 4; i++, ptr++) {
        if (ptr->active != 0) {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief Check if a battle command matches the expected source entity.
 *
 * Returns 0 if @p cmd is zero. Otherwise, looks up the entry at index
 * (cmd & 3), checks if it is active, then compares its sourceId against
 * (cmd >> 4).
 *
 * @param cmd Packed command: bits [1:0] = entry index, bits [15:4] = source ID.
 * @return 1 if active and source matches, 0 otherwise.
 */
s32 checkBattleCmdSource(s32 cmd) {
    BattleCmdEntry *base;
    BattleCmdEntry *entry;

    if (cmd == 0) {
        return 0;
    }
    base = getBattleCmdTable();
    entry = &base[cmd & 3];
    if (entry->active != 0) {
        if (entry->sourceId == (cmd >> 4)) {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief Deactivate a battle command entry or clear all entries.
 *
 * If @p id is -1, clears all 4 entries' active flag and resets anim
 * entity params. Otherwise, validates the command via checkBattleCmdSource
 * and clears just that entry's active flag.
 *
 * @param id Packed command identifier, or -1 to clear all.
 */
void deactivateBattleCmd(s32 id) {
    BattleCmdEntry* ptr = getBattleCmdTable();
    s32 i;

    if (id == -1) {
        for (i = 3; i >= 0; i--, ptr++) {
            ptr->active = 0;
        }
        setAnimEntityParams(0, 0, 0);
    } else {
        if (checkBattleCmdSource(id)) {
            ptr += id & 3;
            ptr->active = 0;
        }
    }
}


/**
 * @brief Load a command from a packed data block into a battle command entry.
 *
 * Finds a free or low-priority command slot via findBestBattleCmd, then
 * loads stream data from the packed block at @p data. The block contains
 * an offset table followed by variable-length stream pairs. Each pair
 * has a CmdStreamHeader (two u16 lengths) followed by the raw data.
 *
 * @param data     Pointer to packed command data block (offset table + streams).
 * @param idx      Index into the offset table.
 * @param priority Priority value for the command slot.
 * @return Packed command ID (sourceId << 4 | index), 0 if no slot, -1 if no data.
 */
s32 loadBattleCmd(u8 *data, s32 idx, s32 priority) {
    BattleCmdEntry *cmd;
    s32 offset;
    CmdStreamHeader *hdr;
    u16 len1, len2;
    u8 *base;
    u8 *block2;

    cmd = findBestBattleCmd(priority);
    if (cmd == 0) {
        return 0;
    }

    offset = ((s32 *)data)[idx];
    data += offset;
    if (offset == 0) {
        return -1;
    }

    hdr = (CmdStreamHeader *)data;
    len1 = hdr->len1;
    len2 = hdr->len2;
    base = hdr->data;
    block2 = hdr->data + len1;

    if (len1 != 0) {
        cmd->streams[0].start = base;
        cmd->streams[0].end = base + len1;
        cmd->streams[0].cursor = -1;
        cmd->streams[0].enabled = 1;
        cmd->streams[0].length = len1;
    } else {
        cmd->streams[0].enabled = 0;
    }

    if (len2 != 0) {
        cmd->streams[1].start = block2;
        cmd->streams[1].end = block2 + len2;
        cmd->streams[1].cursor = -1;
        cmd->streams[1].enabled = 1;
        cmd->streams[1].length = len2;
    } else {
        cmd->streams[1].enabled = 0;
    }

    cmd->active = priority;
    cmd->sourceId++;
    if (cmd->sourceId >= 0x400) {
        cmd->sourceId = 1;
    }

    return (cmd->sourceId << 4) | cmd->index;
}


/**
 * @brief Read and interpolate the next value from a command stream.
 *
 * Reads keyframe pairs (value, duration) from the stream. Linearly
 * interpolates between the current value and the next over the
 * duration, advances the cursor each call, and moves to the next
 * keyframe pair when the duration expires. Returns the interpolated
 * result doubled and clamped to 0-255, or -1 if the stream is
 * disabled or exhausted (0xFF duration marker).
 *
 * Stream format: [val0][dur0][val1][dur1]...[0xFF]
 *
 * @param stream Pointer to a CmdStream.
 * @return Interpolated value (0-255), or -1 if stream ended.
 * @see https://decomp.me/scratch/oOOHt
 */
INCLUDE_ASM("asm/nonmatchings/btl_color", func_80030A54);


INCLUDE_ASM("asm/nonmatchings/btl_color", func_80030B2C);


/**
 * @brief Add to the battle timer and process ticks.
 *
 * Accumulates @p delta into g_battleTimer. For every 4 units accumulated,
 * calls func_80030B2C() once. The remainder is stored back.
 *
 * @param delta Amount to add to the timer.
 */
void advanceBattleTimer(s32 delta) {
    s32 counter = g_battleTimer;
    counter += delta;
top:
    if (counter >= 4) {
        func_80030B2C();
        counter -= 4;
        goto top;
    }
    g_battleTimer = counter;
}


/**
 * @brief Initialize the 4 battle command entries and reset the battle timer.
 *
 * Sets each entry's index to its slot number, clears the active flag,
 * and sets sourceId to 1. Zeroes g_battleTimer.
 */
void initBattleCmdEntries(void) {
    BattleCmdEntry* ptr = getBattleCmdTable();
    s32 i;

    for (i = 0; i < 4; i++, ptr++) {
        ptr->index = i;
        ptr->active = 0;
        ptr->sourceId = 1;
    }

    g_battleTimer = 0;
}


/**
 * @brief Send an SPU command from the battle SFX table.
 *
 * Looks up a command byte from g_battleSfxTable and sends it to the
 * SPU via sndKeyOn.
 *
 * @param idx Index into g_battleSfxTable.
 */
void sendSpuCommand(s32 idx) {
    sndKeyOn(g_battleSfxTable[idx]);
}


/**
 * @brief Play a sound effect from the battle SFX table.
 *
 * Looks up a sound ID from g_battleSfxTable and plays it via sndPlaySfx
 * with default volume (0x80) and pan (0x7F).
 *
 * @param idx Index into the g_battleSfxTable sound table.
 */
void playSoundEffect(s32 idx) {
    sndPlaySfx(g_battleSfxTable[idx], 0, 0x80, 0x7F);
}


/**
 * @brief Configure sound reverb channels based on a bitmask.
 *
 * Reads hardware state via func_80047384, optionally pauses/resumes audio
 * hardware. Mutes master volume, then enables reverb on channels indicated
 * by bits 0-2 of a0. If a0 == 7, enables reverb on channel 0 (all).
 *
 * @param mask Bitmask of reverb channels to enable (bits 0, 1, 2).
 */
void enableSoundReverb(s32 mask) {
    s32 hwState = func_80047384();

    if (!(hwState & 4)) {
        func_800472E4();
    }
    sndSetMasterVolume(0);
    if (mask == 7) {
        sndEnableReverb(0);
    } else {
        if (mask & 1) {
            sndEnableReverb(1);
        }
        if (mask & 2) {
            sndEnableReverb(2);
        }
        if (mask & 4) {
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
 * @param mask Bitmask of reverb channels to disable (bits 0, 1, 2).
 */
void disableSoundReverb(s32 mask) {
    s32 hwState = func_80047384();

    if (!(hwState & 4)) {
        func_800472E4();
    }
    if (mask == 7) {
        sndDisableReverb(0);
    } else {
        if (mask & 1) {
            sndDisableReverb(1);
        }
        if (mask & 2) {
            sndDisableReverb(2);
        }
        if (mask & 4) {
            sndDisableReverb(3);
        }
    }
    sndSetMasterVolume(0x7F);
    if (!(hwState & 4)) {
        func_800472F4();
    }
}


/**
 * @brief Remap a party bitmask through the controller config remap table.
 *
 * When CONFIG_CONTROLLER is set in g_gameState.config.flags, each set bit
 * in the lower 12 bits is remapped through the button remap table
 * g_gameState.config.buttons. The upper 4 bits (0xF000) are preserved.
 * Returns the original value if remapping is inactive.
 *
 * @param bitmask Party bitmask (lower 12 bits = members, upper 4 = flags).
 * @return Remapped bitmask, or original if remapping inactive.
 */
u16 remapControllerInput(u16 bitmask) {
    s32 top;
    u8* table;
    s32 result;
    s32 i;

    if (!(g_gameState.config.flags & CONFIG_CONTROLLER)) {
        return bitmask;
    }

    top = bitmask & 0xF000;
    table = g_gameState.config.buttons;
    result = 0;
    bitmask &= 0xFFF;

    for (i = 0; i < 12; i++) {
        if ((bitmask >> i) & 1) {
            s32 remap = *table++;
            if (remap) {
                result |= 1 << (remap - 1);
            }
        } else {
            table++;
        }
    }

    return (u16)(top | result);
}


/**
 * @brief Remap a battle palette index through the controller button table.
 *
 * If CONFIG_CONTROLLER is set and @p index is within the 12-button range,
 * returns the remapped button value minus 1. Otherwise returns the index
 * unchanged.
 *
 * @param index Button/palette index to remap.
 * @return Remapped index, or original if remapping inactive or out of range.
 */
s32 remapButtonIndex(s32 index) {
    if ((g_gameState.config.flags & CONFIG_CONTROLLER) && index < 12) {
        return g_gameState.config.buttons[index] - 1;
    }
    return index;
}


/**
 * @brief Reverse-lookup a logical button index to its physical button.
 *
 * Inverse of remapButtonIndex. Given a logical index, searches the
 * controller remap table for which physical button maps to it.
 * Returns the index unchanged if remapping is inactive or out of range,
 * or -1 if no physical button maps to the given logical index.
 *
 * @param index Logical button index to reverse-lookup.
 * @return Physical button index, original index if inactive, or -1 if not found.
 */
s32 reverseButtonRemap(s32 index) {
    u8* table;
    s32 i;
    u8 remap;

    if (!(g_gameState.config.flags & CONFIG_CONTROLLER) || index >= 12) {
        return index;
    }

    table = g_gameState.config.buttons;
    index++;

    for (i = 0; i < 12; i++) {
        remap = *table++;
        if (remap == index) {
            return i;
        }
    }

    return -1;
}


/** @brief Empty stub -- no operation. */
void btlColorStub1044(void) {
}


/**
 * @brief Advance the palette transition state machine.
 *
 * Drives a multi-phase screen fade effect for battle transitions.
 * State 0-1: fade out (ramp fade to 0x1000 in steps of 0x100).
 * State 2-3: hold (countdown timer from 24).
 * State 4-5: crossfade brightness (ramp to 0x1000 in steps of 0x155).
 * State 7-8: fade in (ramp fade back down to 0 in steps of 0x100).
 * States 6, 9, 10: idle/complete.
 */
void updatePaletteTransition(void) {
    u16 *state = &D_80083754.state;
    PaletteTransition *p = &D_80083754;

    switch (*state) {
    case 0:
        p->brightness = 0;
        p->fade = 0;
        *state = 1;
    case 1:
        p->fade += 0x100;
        if ((s16)p->fade < 0x1000) {
            return;
        }
        p->fade = 0x1000;
        *state = 2;
        return;
    case 2:
        p->timer = 24;
        *state = 3;
    case 3:
        p->timer--;
        if ((u8)p->timer) {
            return;
        }
        *state = 4;
        return;
    case 4:
        if (p->dstPalette == p->srcPalette) {
            p->brightness = 0x1000;
            *state = 6;
            return;
        }
        *state = 5;
        return;
    case 5:
        p->brightness += 0x155;
        if ((s16)p->brightness < 0x1000) {
            return;
        }
        p->brightness = 0x1000;
        *state = 6;
        return;
    case 6:
        return;
    case 7:
        *state = 8;
        return;
    case 8:
        p->fade -= 0x100;
        if ((s16)p->fade > 0) {
            return;
        }
        p->fade = 0;
        *state = 9;
        return;
    case 9:
    case 10:
        return;
    }
}


/**
 * @brief Render a null-terminated string, skipping separator characters.
 *
 * Iterates through each byte of the string at @p str. If the character is
 * zero (null terminator), returns the current packet pointer. If the
 * character equals 7 (separator), skips it but advances the y position.
 * Otherwise calls func_8002FF34 to render the character glyph.
 *
 * @param ot    OT base pointer.
 * @param pkt   Current packet buffer pointer.
 * @param str   Pointer to null-terminated string.
 * @param y     Starting y position.
 * @param width Width parameter (passed to func_8002FF34).
 * @param color Color parameter (passed to func_8002FF34).
 * @return Updated packet buffer pointer after rendering.
 */
s32 renderBattleString(s32 ot, s32 pkt, u8 *str, s32 y, s32 width, s32 color) {
    s32 otBase = ot;
    u8 *ptr = str;
    s32 yPos = y;
    s32 w = width;
    s32 sep = 7;
    u8 ch;
    s32 col = color;

    do {
    top:
        ch = *ptr++;
        if (ch == 0) {
            return pkt;
        }
        if (ch == sep) goto skip;
        pkt = func_8002FF34(otBase, pkt, ch, yPos, w, col);
    } while (0);
skip:
    yPos = yPos + 9;
    goto top;
}


INCLUDE_ASM("asm/nonmatchings/btl_color", func_80031224);


INCLUDE_ASM("asm/nonmatchings/btl_color", func_80031364);


// init_battle_transition - initializes D_80083754 (Struct3754)

/** @brief Begin the fade-out phase of the palette transition. */
void setTransitionPhase7(void) {
    D_80083754.state = 7;
}


INCLUDE_ASM("asm/nonmatchings/btl_color", func_800316D4);


/** @brief Stores a byte to global D_80083756. */
void setTransitionFlag(s32 a0) {
    D_80083756 = a0;
}


/**
 * @brief Reset the palette transition to its idle state.
 *
 * Sets state to 9 (idle), clears brightness and fade, and resets
 * source/destination palette indices.
 */
void initBattleTransition(void) {
    D_80083754.state = 9;
    D_80083754.brightness = 0;
    D_80083754.fade = 0;
    D_80083754.srcPalette = 0;
    D_80083754.dstPalette = 0;
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


/**
 * @brief Step animation entries toward their interpolated targets.
 *
 * For each active entry (flags bit 7 set), computes the target via
 * lerpRange and moves the current value toward it by +/-2 per step.
 */
void stepAnimEntries(void) {
    s32 i;
    AnimEntry *entry = D_80083772;
    s32 current;
    s32 target;

    for (i = 0; i < 2; i++, entry++) {
        if (!(entry->flags & 0x80)) {
            continue;
        }

        current = entry->current;
        target = lerpRange(entry->rangeStart, entry->rangeEnd, entry->inputStart, entry->inputEnd);

        if (current < target) {
            current += 2;
            if (target < current) {
                current = target;
            }
        }
        if (target < current) {
            current -= 2;
            if (current < target) {
                current = target;
            }
        }

        entry->current = current;
    }
}


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
 * @brief Deactivate an animation entry by clearing the active flag.
 * @param idx Entry index into D_80083772.
 */
void clearAnimEntryActive(s32 idx) {
    AnimEntry *entry = D_80083772;
    entry = &entry[idx];
    entry->flags &= 0x7F;
}


/**
 * @brief Update an animation entry's input and optionally recompute current.
 *
 * Sets inputStart to the new value and interpolates via lerpRange.
 * If flags bit 1 is set, stores the interpolated result to current.
 *
 * @param idx   Entry index into D_80083772.
 * @param value New inputStart value.
 */
void updateAnimEntry(s32 idx, s32 value) {
    AnimEntry *entry = D_80083772;
    s16 result;

    entry = &entry[idx];
    result = lerpRange(entry->rangeStart, entry->rangeEnd, value, entry->inputEnd);
    entry->inputStart = value;
    if (entry->flags & 2) {
        entry->current = result;
    }
}


/**
 * @brief Copy the first 4 bytes of data into an animation entry.
 *
 * @param idx Entry index into D_80083772.
 * @param src Source for the 4-byte copy.
 */
void copyAnimEntryField(s32 idx, u8 *src) {
    AnimEntry *entry = D_80083772;
    entry = &entry[idx];
    memcpy(entry->data, src, 4);
}


/**
 * @brief Initialize an animation entry with full parameters.
 *
 * Copies source data, sets the active flag, configures interpolation
 * range and input values, then computes the initial current value.
 *
 * @param idx     Entry index.
 * @param flags   Flags byte (ORed with 0x80 for active).
 * @param src     Source data pointer (first 4 bytes copied).
 * @param start   Range start value.
 * @param end     Range end value.
 * @param inStart Input start value.
 * @param inEnd   Input end value.
 */
void initAnimEntry(s32 idx, s32 flags, s32 src, s32 start, s32 end, s32 inStart, s32 inEnd) {
    AnimEntry *entry = D_80083772;
    s32 activeFlags = flags | 0x80;

    entry = &entry[idx];
    copyAnimEntryField(idx, (void *)src);
    entry->flags = activeFlags;
    entry->rangeStart = start;
    entry->rangeEnd = end;
    entry->inputStart = inStart;
    entry->inputEnd = inEnd;
    entry->current = lerpRange(start, end, inStart, inEnd);
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
    HudDisplayBuf *buf;
    HudDisplayBuf *active;

    buf = D_80083920[0];
    if (D_80083918 == buf) {
        buf = D_80083920[1];
    }
    D_80083918 = buf;
    ClearOTag(buf->ot, 2);
    active = D_80083918;
    active->pktAlloc = (u32)&active->pktBase;
}


INCLUDE_ASM("asm/nonmatchings/btl_color", func_80032010);


INCLUDE_ASM("asm/nonmatchings/btl_color", func_800320BC);


INCLUDE_ASM("asm/nonmatchings/btl_color", renderBattleFrame);


