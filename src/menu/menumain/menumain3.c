#include "common.h"
#include "gamestate.h"
#include "input/gamepad.h"
#include "menu.h"
#include "numstr.h"
#include "psxsdk/libetc.h"
#include "snd_init.h"
#include "snd_sfx.h"

extern u8 D_80056290[];

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain3", func_801F776C);

/**
 * @brief Find ability slot by ID in character data.
 *
 * Searches 19 ability slots in g_gameState.chars[a0].junctions for value a1.
 * Returns the slot index if found, or a2 (default) if not.
 */
s32 func_801F77F8(s32 a0, s32 a1, s32 a2) {
    u8 *junctionSlot = g_gameState.chars[a0].junctions;
    s32 i;
    for (i = 0; i < 19; i++) {
        if (*junctionSlot++ == a1) {
            return i;
        }  
    }

    return a2;
}

/** @brief Equip magic a1 to character a0's junction slot of type a2. */
void func_801F784C(s32 a0, s32 a1, s32 a2) {
    s32 ret;
    ret = func_801F776C(a1, a2);
    if (ret) {
        ret = func_801F77F8(a0, a1, a2);
        g_gameState.chars[a0].junctions[ret] = a1;
    }
    func_801F1B4C(a0);
}

/** @brief Remove ability a1 from character a0's junction slots. */
void func_801F78D8(s32 a0, s32 a1) {
    u8 *junctionSlot = g_gameState.chars[a0].junctions;
    s32 i;
    for (i = 0; i < 19; i++) {
        if (*junctionSlot == a1) {
            *junctionSlot = 0;
        }
        junctionSlot++;
    }
}

/* ======================================================================== */
/* Misc/Utility                                                             */
/* ======================================================================== */

/** @brief Apply vibration config setting from g_gameState.config.flags bit 1. */
void func_801F7928(void) {
    s32 flags = g_gameState.config.flags & 2;
    sndSelectMode(flags != 0);
}

/** @brief Apply the Vibration option to pad port 0. */
void func_801F7954(void) {
    s32 a1 = 0;
    if (g_gameState.config.flags & CONFIG_VIBRATION) {
        a1 = 0xFF;
    }
    SetVibrationEnabled(0, a1);
}

/**
 * @brief Push the analog-volume and controller-mode config to the sound layer.
 *
 * Sends (analogVolume + 5) * 12 as the volume argument of SetStickDeadZone,
 * then tells SetSwapAnalogSticks whether the analog flag is active — only
 * honored when the controller is in customize mode (CONFIG_CONTROLLER).
 *
 * @note SetStickDeadZone/SetSwapAnalogSticks are main-executable sound-layer
 *       routines that are not decompiled yet.
 */
void func_801F798C(void) {
    s32 v;

    v = g_gameState.config.analogVolume;
    v += 5;
    SetStickDeadZone(0, v * 12);
    v = g_gameState.config.flags & CONFIG_ANALOG;
    v = v != 0;
    if (!(g_gameState.config.flags & CONFIG_CONTROLLER)) {
        v = 0;
    }
    SetSwapAnalogSticks(0, v);
}

/** @brief Test sealed-ability bits (@c GameConfig.sealedAbilities & @p a0). */
s32 func_801F79F8(s32 a0) {
    return g_gameState.config.sealedAbilities & a0;
}

/** @brief Copy pad port 0's vibration setting back into the Vibration option. */
void func_801F7A08(void) {
    if (IsVibrationEnabled(0) == 0xFF) {
        g_gameState.config.flags |= CONFIG_VIBRATION;
    } else {
        g_gameState.config.flags &= ~CONFIG_VIBRATION;
    }
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain3", func_801F7A54);

/**
 * @brief Scale a value by 100 and store it clamped to [100, 3100] in g_gameState.fieldVars.seedExp.
 *
 * @note Purpose uncertain — appears to set a percentage-derived global
 *       (e.g. a speed or magnification parameter).
 *
 * @param n Input value; stored as n*100 clamped to 100..3100.
 */
void func_801F7AD4(s32 n) {
    s32 val = n * 100;
    s32 clamped;

    if (val >= 100) {
        if (val <= 3100) {
            clamped = val;
        } else {
            clamped = 3100;
        }
    } else {
        clamped = 100;
    }
    /* Fold-away bump: raises clamped's allocno priority above val's so
     * clamped takes v1 and val keeps a0 (allocation-order match). */
    clamped++;
    clamped--;
    g_gameState.fieldVars.seedExp = clamped;
}

/**
 * @brief Clear magic slots with zero quantity.
 *
 * Iterates 32 magic entries (stride 2) for character a0. If the
 * quantity byte is 0, clears the magic ID byte to 0.
 */
void func_801F7B10(s32 a0) {
    MagicSlot *slot = g_gameState.chars[a0].magic;
    s32 i;
    for (i = 0; i < 32; i++) {
        if (slot->quantity == 0) {
            slot->magicId = 0;
        }
        slot++;
    }
}

/**
 * @brief Sync magic IDs with quantities (bidirectional zero-clear).
 *
 * Iterates 198 entries in D_80077EBC. If quantity is 0, clears the
 * ID; if ID is 0, clears the quantity.
 */
void func_801F7B60(void) {
    s32 i;
    ItemSlot* itemSlot;

    itemSlot = g_gameState.mainData.itemSlots;
    
    for (i = 0; i < 198; i++, itemSlot++) {
        u8 id = itemSlot->id;
        u8 qt = itemSlot->count;
        
        if (qt == 0) {
            itemSlot->id = 0;
        }
        
        if (id == 0) {
            itemSlot->count = 0;
        }
    }
}

/** @brief Convert 0-255 value to 0-100 percentage. */
s32 func_801F7BAC(s32 a0) {
    return a0 * 100 / 255;
}

/** @brief Identity function (returns a0 unchanged, compatibility placeholder). */
s32 func_801F7BE4(s32 a0) {
    return a0;
}

/** @brief Click confirm or cancel, from a press-edge input word. */
void func_801F7BEC(s32 a0) {
    if (a0 & PADRdown) {
        sendSpuCommand(2);
    } else {
        sendSpuCommand(3);
    }
}

/**
 * @brief Expand compressed status flags to rendering icon bitmask.
 *
 * Maps individual status bits from a compact internal format to a
 * wider bitmask with spacing for display icon positioning.
 */
s32 func_801F7C20(s32 a0) {
    s32 result = a0 & 0x7FF;
    if (a0 & 0x800)   result |= 0x800;
    if (a0 & 0x2000)  result |= 0x1800;
    if (a0 & 0x4000)  result |= 0x7800;
    if (a0 & 0x1000)  result |= 0x8000;
    if (a0 & 0x8000)  result |= 0x18000;
    if (a0 & 0x10000) {
        s32 mask = 0x10000;
        result |= 0x8000;
        result |= mask;
        result |= 0x20000;
        result |= 0x40000;
    }
    return result;
}

/** @brief Render save/card-related text (convert value to string, draw at position). */
void func_801F7C98(s32 a0, s32 a1) {
    u8 buf[16];
    intToDecStringShort(a0, buf, 0x30);
    copyString(a1, D_80056290);
    btlStrcat2(a1, &buf[3]);
}
