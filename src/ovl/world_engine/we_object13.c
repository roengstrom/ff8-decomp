#include "common.h"
#include "gamestate.h"
#include "battle.h"

extern void func_80043578(s32);
extern void func_8004358C(s32);
extern void func_800435A0(s32, s32, s32);

/** Initializes audio/display subsystem. */
void func_800C40F8(void) {
    func_80043578(0);
    func_8004358C(0);
    func_800435A0(9, 0, 0);
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4130);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C42D8);

extern u8 D_800E3E70[];

/** Resets playback state and reinitializes subsystem.
 * @return Always returns 1.
 */
s32 func_800C4450(void) {
    s32 base = (s32)D_800E3E70;
    *(s32 *)(base + 0x10) = 0;
    *(volatile u8 *)(base + 0x25) = 0;
    func_800C40F8();
    return 1;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4480);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4558);

extern s32 D_800E3E60;

/** Exchanges the callback pointer, returning the previous value.
 * @param newVal New callback value to store.
 * @return Previous callback value.
 */
s32 func_800C4634(s32 newVal) {
    s32 old = D_800E3E60;
    D_800E3E60 = newVal;
    return old;
}

/** Clears all playback state and callback pointer. */
void func_800C4644(void) {
    s32 base = (s32)D_800E3E70;
    D_800E3E60 = 0;
    *(s32 *)(base + 0x00) = 0;
    *(s32 *)(base + 0x04) = 0;
    *(s32 *)(base + 0x08) = 0;
    *(s32 *)(base + 0x0C) = 0;
    *(s32 *)(base + 0x10) = 0;
    *(s32 *)(base + 0x14) = 0;
    *(s32 *)(base + 0x18) = 0;
    *(s32 *)(base + 0x1C) = 0;
    *(s32 *)(base + 0x20) = 0;
    *(u8 *)(base + 0x24) = 0;
    *(volatile u8 *)(base + 0x25) = 0;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4688);

/**
 * @brief Tick all active GFs' HP up by 1 toward their battle max.
 *
 * Iterates the 16 GF save entries. For each unlocked GF (@c exists bit 0)
 * with non-zero HP, if the current HP is below the GF's battle max (at
 * @c g_battleChars.gfEntries[i].hp), it's incremented by 1. Likely runs
 * once per field step to slowly regenerate GF HP while walking.
 *
 * @note Match requires reading @c g_gameState.gfs[i].hp directly at the
 *       compare AND increment — not through the cached @c hp local.
 */
void func_800C48C0(void) {
    s32 i;

    for (i = 0; i < 16; i++) {
        if (g_gameState.gfs[i].exists & 1) {
            u16 hp = g_gameState.gfs[i].hp;
            if (hp != 0) {
                if (g_gameState.gfs[i].hp < (s16)g_battleChars.gfEntries[i].hp) {
                    g_gameState.gfs[i].hp = g_gameState.gfs[i].hp + 1;
                }
            }
        }
    }
}

/**
 * @brief Tick each active party member's HP up by 1 toward their battle regen cap.
 *
 * Iterates the 3 party slots. For each occupied slot with its field-status
 * bit 0 set, increments the character's @c currentHp by 1 as long as it
 * stays below the slot's @c hpRegenCap (stored in the BattleCharData at
 * offset 0x174). Paired with @c func_800C48C0 which does the same for GF HP.
 *
 * @note Match requires reading @c g_gameState.mainData.party.party[i] and
 *       @c g_gameState.chars[charIdx].currentHp directly at each use site
 *       (not through cached locals) — identical pattern to func_800C48C0.
 */
void func_800C492C(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        u8 charIdx = g_gameState.mainData.party.party[i];
        if (charIdx != 0xFF) {
            if (g_battleChars.chars[i].fieldStatusByte & 1) {
                u16 hp = g_gameState.chars[g_gameState.mainData.party.party[i]].currentHp;
                if (hp != 0) {
                    if ((s32)g_gameState.chars[g_gameState.mainData.party.party[i]].currentHp <
                        (s32)g_battleChars.chars[i].hpRegenCap) {
                        g_gameState.chars[g_gameState.mainData.party.party[i]].currentHp =
                            g_gameState.chars[g_gameState.mainData.party.party[i]].currentHp + 1;
                    }
                }
            }
        }
    }
}

extern void sndPlaySfx(s32 sfxId, s32 a1, s32 a2, s32 a3);

/**
 * @brief Tick the currently-learning Angelo trick's points; unlock + play SFX at 0.
 *
 * When Rinoa (CHAR_RINOA = 4) is in the battle party, decrements the
 * learning-points counter for the currently-learning Angelo trick (index
 * at @c mainData.party.trickLearning). When the counter reaches 0 and the
 * trick isn't already completed, sets the completed bit and plays the
 * learn SFX (0x83). Paired with other field-tick helpers.
 */
void func_800C49CC(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (g_gameState.battleParty[i] == CHAR_RINOA) {
            u8 idx = g_gameState.mainData.party.trickLearning;
            if (g_gameState.mainData.limitBreaks.angeloPoints[idx] != 0) {
                g_gameState.mainData.limitBreaks.angeloPoints[idx]--;
            } else {
                u8 mask = g_gameState.mainData.limitBreaks.angeloCompleted;
                if (!((mask >> idx) & 1)) {
                    g_gameState.mainData.limitBreaks.angeloCompleted = mask | (1 << idx);
                    sndPlaySfx(0x83, 0, 0x80, 0x7F);
                }
            }
            return;
        }
    }
}

extern s32 getPackedField2Bit(s32);
extern s32 fieldRandom(void);
extern void func_800383B8(s32, s32);

/** Scans all 256 entries and processes active ones. */
void func_800C4A74(void) {
    s32 i;
    s32 result;
    for (i = 0; i < 256; i++) {
        result = getPackedField2Bit(i) - 1;
        if ((u32)result < 2) {
            if (fieldRandom() & 1) {
                func_800383B8(i, result);
            }
        }
    }
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4AE4);
