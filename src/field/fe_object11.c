#include "common.h"
#include "283B0.h"
#include "field.h"
#include "gamestate.h"
#include "battle.h"
#include "sound.h"
#include "ui/countdown.h"
#include "ui/seed_rank.h"
#include "ui/window.h"
#include "ui/dialog.h"
#include "cd.h"
#include "field/fe_object1.h"
#include "field/fe_object1_2.h"
#include "field/fe_object3.h"
#include "field/fe_object5.h"
#include "field/fe_object10.h"
#include "field/fe_object11.h"

/**
 * @brief Initialise / reset the field-engine sound and slot state.
 *
 * Mode-dispatched bookkeeping called at field transitions. Always
 * marks the sound bank as loaded (@c g_fieldVars->soundLoadComplete)
 * and arms @c D_800DE8C8 byte @c 0xD; the per-mode work then runs:
 *
 *   - @c mode @c == @c 0 — full music reset: stop current channel,
 *     replay any queued @ref D_80074F20 voice-pool entries, then
 *     (unless @c g_battleConfig.unk2 bit @c 0x10 is set) start the
 *     bank file or send the @ref D_8005F11C fade command depending
 *     on whether @c audioChannel0State is armed. Always writes
 *     @c g_battleConfig.unk2 and pulses @ref sndCmd45.
 *   - @c mode @c == @c 1 — full slot reset: zero the 16-byte
 *     @c g_fieldContext.unkActive180 region, walk the @ref g_fieldActors
 *     Actor pool and clear the cleanup-trigger bytes / @c flags @c & @c ~8
 *     on each entity flagged @c unk218 @c == @c -1, then fall into
 *     the mode-3 slot-clear block.
 *   - @c mode @c == @c 2 — just @ref func_800BF4A4 (Actor pool reset).
 *   - @c mode @c == @c 3 — slot table reset: @ref func_800BF28C plus
 *     reinit of each @c g_fieldContext.slots[i] (mode/param/submode/p3-p6).
 *
 * After the dispatch all paths run a common tail:
 *   - @ref FIELD_STATE_COUNTDOWN → show the countdown timer again.
 *   - @ref FIELD_STATE_FIELD_READY clear and @c levelUpDisplayTimer @c > @c 0
 *     → fire the SeeD level-up notification via @ref showSeedRankNotification.
 *   - Set each dialog's text speed from the message-speed setting
 * (@ref setDialogTextSpeed).
 *   - Mirror @c D_80078DF8 bit @c 0x10 → @c FieldVars.field58 and,
 *     if @c fieldF0 is set, forward it through @ref func_800A4550.
 *
 * @param mode  Dispatch selector (0..3; other values run only the
 *              common tail).
 */
void func_800BF718(s32 mode) {
    s32 i;
    FieldVars *seed;
    Actor *e;
    s32 v;
    s32 prevLevel, currLevel;
    s32 ch0;
    s32 entIdx;
    u8 *fs = (u8 *)D_800DE8C8;

    g_fieldVars->soundLoadComplete = 1;
    fs[0xD] = 1;

    switch (mode) {
    case 1:
        for (i = 0; i < 16; i++) {
            g_fieldContext.unkActive180[i] = 0;
        }
        e = g_fieldActors;
        D_800DE4FC = 0;
        if (g_fieldActorCount != 0) {
            do {
                if (e->unk218 == -1) {
                    e->context.flags &= ~8;
                    e->field_0x24C = 0;
                    e->unk249 = 0;
                    e->field_0x24B = 0;
                }
                D_800DE4FC++;
                e++;
            } while (D_800DE4FC < g_fieldActorCount);
        }
        /* fall through */
    case 3:
        func_800BF28C(0);
        for (i = 0; i < 8; i++) {
            entIdx = g_fieldContext.entityIndex[0];
            g_fieldContext.slots[i].mode = 0;
            g_fieldContext.slots[i].submode = 0;
            g_fieldContext.slots[i].p5 = 0x100;
            g_fieldContext.slots[i].p6 = 0x100;
            g_fieldContext.slots[i].p3 = 0;
            g_fieldContext.slots[i].p4 = 0;
            g_fieldContext.slots[i].param = entIdx;
        }
        break;
    case 0:
        func_800BF4A4();
        sndCmdF1();
        if (g_fieldVars->audioChannel2State != -1) {
            func_800B21E0();
            if (fs[0xD] == 0) {
                do {
                    func_800393C8();
                } while (fs[0xD] == 0);
            }
        }
        for (i = 0; i < 12; i++) {
            if (D_80074F20[i].cmd < 0) {
                sndPlayBankSfx(D_80074F20[i].cmd, D_80074F20[i].arg1,
                               D_80074F20[i].fieldC, D_80074F20[i].field8);
            } else if (D_80074F20[i].cmd > 0) {
                sndPlaySfx(D_80074F20[i].cmd - 1, D_80074F20[i].arg1,
                           D_80074F20[i].fieldC, D_80074F20[i].field8);
            }
        }
        g_fieldVars->soundBankSelector = (g_battleConfig.unk9 & 1) ^ 1;
        if (!(g_battleConfig.unk2 & 0x10)) {
            ch0 = g_fieldVars->audioChannel0State;
            if (ch0 == -1) {
                sndCmdC1(D_8005F11C, 0x3C, 0);
            } else {
                func_80037FB0(0, ch0, D_8005F13C);
                while (g_fieldVars->soundLoadComplete == 0) {
                    func_800393C8();
                }
                v = (s32)toggleSoundBank();
                g_fieldVars->soundHandle0 = sndCmd1A(v, 0x3C, g_fieldVars->musicVolume);
            }
        }
        g_battleConfig.unk2 = g_fieldVars->fieldB6;
        sndCmd45();
        break;
    case 2:
        func_800BF4A4();
        break;
    }

    if (g_fieldVars->stateFlags & FIELD_STATE_COUNTDOWN) {
        setCountdownPosition(g_fieldVars->countdownX, g_fieldVars->countdownY);
        setCountdownVisible(1);
    }
    seed = g_fieldVars;
    if (!(seed->stateFlags & FIELD_STATE_FIELD_READY)) {
        if ((s16)seed->levelUpDisplayTimer > 0) {
            prevLevel = (s32)((s16)seed->prevSeedExp) / 100;
            currLevel = (s32)((s16)seed->seedExp) / 100;
            showSeedRankNotification(prevLevel, currLevel,
                          g_seedSalaryTable[prevLevel] * 10,
                          g_seedSalaryTable[currLevel] * 10);
            g_fieldVars->levelUpDisplayTimer = 0x5A;
        }
    }
    for (i = 0; i < getMaxBattleEntities(); i++) {
        setDialogTextSpeed(i, g_textSpeeds[g_gameState.config.fieldMsgSpeed]);
    }
    g_fieldVars->field58 = (g_battleChars.abilityFlags & 0x10) >> 4;
    if (g_fieldVars->fieldF0 != 0) {
        func_800A4550(g_fieldVars->fieldF1 | g_fieldVars->field58);
    }
}



/**
 * @brief Field-engine area-load / scene-reset.
 *
 * Repopulates @ref g_fieldVars, snapshots a few @c SaveMainData fields,
 * resets dialog/camera state, then — for @c mode @c == @c 1 or @c 3 —
 * wipes and re-primes the script-VM entity pools and rebinds each
 * active party slot to its @ref Actor.
 *
 * @param entity  Area / event header passed through to
 *                @ref func_800BE30C / @ref func_800BE36C /
 *                @ref func_800BE4B0.
 * @param eline      Block-B entity table base (stored to @c g_fieldLines
 *                and passed to @ref func_800BE7F4).
 * @param a2      Halfword table base forwarded to @ref func_800BE4B0.
 * @param mode    Reset selector: 0 / 2 = common only, 1 / 3 = full
 *                reset, mode @c != @c 0 also runs @ref func_800BF080.
 * @return        @ref func_800BE4B0 's return — the per-area
 *                script-VM dispatch table.
 */
s32 *func_800BFBBC(u8 *entity, Eline *eline, u16 *a2, s32 mode) {
    s32 t14;
    u16 t18, t1A;
    u16 t57;
    u16 rot;
    FieldVars *seed;
    s32 i;
    Actor *e;
    s32 *ret;
    u8 *gs_bytes;

    g_fieldVars = &g_gameState.fieldVars;
    t57 = g_previousGameMode;
    t14 = g_gameState.mainData.fieldCDC;
    t18 = g_gameState.mainData.fieldCE0;
    t1A = g_gameState.mainData.fieldCE2;
    g_fieldVars->killSum = 0;
    g_fieldVars->field57 = t57;
    g_fieldVars->field14 = t14;
    g_fieldVars->field18 = t18;
    g_fieldVars->field1A = t1A;

    for (i = 0; i < 8; i++) {
        g_fieldVars->killSum += g_gameState.chars[i].kills;
    }

    seed = g_fieldVars;
    seed->charKills[0] = g_gameState.chars[0].kills;
    seed->charKills[1] = g_gameState.chars[1].kills;
    seed->charKills[2] = g_gameState.chars[2].kills;
    seed->charKills[3] = g_gameState.chars[3].kills;
    seed->charKills[4] = g_gameState.chars[4].kills;
    seed->charKills[5] = g_gameState.chars[5].kills;
    seed->charKills[6] = g_gameState.chars[6].kills;
    seed->charKills[7] = g_gameState.chars[7].kills;
    seed->charKos[0] = g_gameState.chars[0].kos;
    seed->charKos[1] = g_gameState.chars[1].kos;
    seed->charKos[2] = g_gameState.chars[2].kos;
    seed->charKos[3] = g_gameState.chars[3].kos;
    seed->charKos[4] = g_gameState.chars[4].kos;
    seed->charKos[5] = g_gameState.chars[5].kos;
    seed->charKos[6] = g_gameState.chars[6].kos;
    seed->charKos[7] = g_gameState.chars[7].kos;

    seed->field54 = g_fieldContext.previousFieldId;
    seed->field56 = D_80082C8D;
    D_800DE8C8[1] = 2;
    ((u8 *)D_800DE8C8)[0xB] = 0;
    resetAllDialogs();
    setHudBrightness(0x1000);
    D_800DE4FD[0] = 0;
    D_800DE7B0.count = 0;
    func_800BE30C(entity);

    if (mode == 1 || mode == 3) {
        D_80085390 = 0;
        g_fieldContext.field1B4 = 0xFFFFFF;
        g_fieldContext.unk1A6 = 0;
        g_fieldContext.oscillators[0].mode = 0;
        g_fieldContext.oscillators[1].mode = 0;
        g_fieldContext.unk1A3 = 0;
        g_fieldVars->stateFlags &= ~FIELD_STATE_FLAG_400;
        g_fieldVars->fieldCF = 0;
        g_fieldVars->fieldD1 &= 0xFC;
        g_fieldVars->dialogStartMask = 0;
        g_fieldVars->dialogEntryMask = 0;
        g_fieldVars->dialogActiveMask = 0;
        for (i = 0; i < getMaxBattleEntities(); i++) {
            D_80085300[i].type = 6;
            D_80085300[i].brightness = 0x1000;
        }
        g_fieldVars->fieldF0 = 0;
        g_fieldVars->fieldF1 = 0;
        g_fieldLines = eline;
        g_fieldActorCount = D_800DE8C0;
        g_fieldDoorCount = D_800DE8D9;
        g_fieldLineCount = D_800DE8DA;
        g_fieldBgAnimCount = D_800DE8D8;
        D_800852F0 = D_800DE4E4;
        D_80085380 = D_800DE4E8;
        g_battleConfig.unk2 = g_fieldVars->fieldB6;
        g_fieldDoors = (Dline *)func_800BE7F4((Eline *)eline);
        g_fieldBgAnims = (Bganime *)func_800BEA84(g_fieldDoors);
        g_fieldActors = (Actor *)func_800BE924(g_fieldBgAnims);
        func_800BE5E4(g_fieldActors);

        gs_bytes = (u8 *)&g_gameState;
        for (i = 0x400; i < 0x500; i++) {
            gs_bytes[i + GAMESTATE_MISC3_OFFSET] = 0;
        }
        g_fieldContext.dialogState = 4;
        g_fieldContext.entityIndex[0] = 0xFF;
        g_fieldContext.entityIndex[1] = 0xFF;
        g_fieldContext.entityIndex[2] = 0xFF;
        g_fieldVars->memberSlot[0] = 0xFF;
        g_fieldVars->memberSlot[1] = 0xFF;
        g_fieldVars->memberSlot[2] = 0xFF;
        g_fieldVars->dialogStateMirror = 0;
        func_800BEBD0();
        func_800BE36C(entity);

        for (i = 0; i < 3; i++) {
            for (e = g_fieldActors, D_800DE4FC = 0; D_800DE4FC < g_fieldActorCount; D_800DE4FC++, e++) {
                if (g_gameState.battleParty[i] == e->field_0x255) {
                    e->posX = g_fieldContext.position_x << 12;
                    e->posY = g_fieldContext.position_y << 12;
                    e->facingDirection = g_fieldContext.facingDirection;
                    rot = g_fieldContext.spawnTriIdx;
                    e->context.flags |= 4;
                    e->triIdx = rot;
                    g_fieldVars->memberSlot[i] = e->field_0x256;
                    g_fieldContext.entityIndex[i] = e->field_0x256;
                    break;
                }
            }
        }
    }

    ret = func_800BE4B0(entity, a2);
    if (mode != 0) {
        func_800BF080();
    }
    D_800DE8C8[1] &= ~2;
    return ret;
}
