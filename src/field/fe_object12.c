#include "common.h"
#include "283B0.h"
#include "gamestate.h"
#include "game.h"
#include "field.h"
#include "battle.h"
#include "ui/gauge.h"
#include "ui/seed_rank.h"
#include "cdrom.h"
#include "render.h"
#include "lzs.h"
#include "snd_init.h"
#include "psxsdk/libc.h"
#include "psxsdk/libpress.h"
#include "field/fe_object4.h"
#include "field/fe_object12.h"

/**
 * @brief Field-engine state initializer (new game / field reset).
 *
 * The developers' own debug trace opens the routine, and is where its name
 * comes from — it prints @c "::SmInitEventAll" followed by the sizes of the
 * four field structures the game calls @c actor, @c eline, @c dline and
 * @c bganime (@ref Actor, @ref Eline, @ref Dline, @ref Bganime here).
 * It then resets the MDEC decoder and:
 *  - On @p fullReset: clears the whole of @c *g_fieldVars,
 *    stamps the "FF-8" init tag, and seeds the defaults — disc 1, battle
 *    music 5, both volumes 0x7F, SeeD exp 500, the @ref FIELD_STATE_TRANSITION
 *    and @ref FIELD_STATE_FIELD_READY state bits,
 *    and message speed 2 (@c GameConfig.fieldMsgSpeed).
 *  - Always: clears the dialog masks and the first two anim-shadow slots,
 *    disables reverb, resets the sound-bank selector, and marks all
 *    three audio channel states and both sound handles inactive (-1).
 *  - Mirrors @c battleFlags into @c g_battleConfig.unk2; when
 *    @ref FIELD_STATE_PARTY_OVERRIDE is set, also mirrors @c fieldF3 into @c g_battleConfig.sealedAbilities
 *    and @c GameConfig.sealedAbilities and replays @c opHandler_SETPARTY2.
 *  - Publishes @c field56 to @c g_worldTransfer.vehicle, pushes the expected disc to
 *    the CD layer (@c setDiscNumber, @c g_gameState.unk048 = disc - 1), derives
 *    the salary-enabled flag from @ref FIELD_STATE_TRANSITION, and installs the
 *    @c stopAllSounds VSync callback and @c func_80037D40 draw callback.
 *
 * @param fullReset Nonzero to wipe @c *g_fieldVars and apply new-game
 *                  defaults; zero to keep current values.
 */
void SmInitEventAll(s32 fullReset)
{
    s32 i;
    s32 neg;
    s32 m;
    s32 disc;
    FieldVars *fv;
    volatile FieldVars *vfv;

    printf("::SmInitEventAll(%d);\n", fullReset);
    printf("----------------------------------------\n");
    printf("sizeof(actor) %d\n", sizeof(Actor));
    printf("sizeof(eline) %d\n", sizeof(Eline));
    printf("sizeof(dline) %d\n", sizeof(Dline));
    printf("sizeof(bganime) %d\n", sizeof(Bganime));
    printf("address(DrawPointFlag) %p\n", g_fieldVars->drawPointFlag);
    printf("%x\n", &g_fieldVars);
    printf("%x\n", &g_fieldVars->stateFlags);
    DecDCTReset(0);

    if (fullReset) {
        func_800396E0(g_fieldVars, sizeof(FieldVars));
        g_fieldVars->initTag[0] = 'F';
        g_fieldVars->initTag[1] = 'F';
        g_fieldVars->initTag[2] = '-';
        g_fieldVars->initTag[3] = '8';
        g_fieldVars->expectedDiscId = 1;
        g_fieldVars->battleMusicId = 5;
        g_fieldVars->musicVolume = 0x7F;
        g_fieldVars->sfxVolume = 0x7F;
        g_fieldVars->seedExp = 500;
        g_fieldVars->stateFlags |= FIELD_STATE_TRANSITION | FIELD_STATE_FIELD_READY;
        g_gameState.config.fieldMsgSpeed = 2;
    }

    g_fieldVars->dialogStartMask = 0;
    g_fieldVars->dialogEntryMask = 0;
    g_fieldVars->dialogActiveMask = 0;
    for (i = 0; i < 2; i++) {
        D_80085398[i].flag = 0;
        hideGauge(i);
    }
    sndDisableReverb(0);
    g_fieldVars->soundBankSelector = 0;
    neg = -1;
    g_fieldVars->audioChannel0State = neg;
    g_fieldVars->audioChannel1State = neg;
    neg = 0; /* dead-set: retires the live -1 so the next group rematerializes it (regalloc) */
    fv = g_fieldVars;
    m = -1;  /* fresh name: one rematerialized -1 serves all three stores below (regalloc) */
    fv->audioChannel2State = m;
    fv->soundHandle0 = m;
    fv->soundHandle1 = m;

    g_battleConfig.unk2 = g_fieldVars->battleFlags;
    if (g_fieldVars->stateFlags & FIELD_STATE_PARTY_OVERRIDE) {
        g_battleConfig.sealedAbilities = g_fieldVars->fieldF3;
        g_gameState.config.sealedAbilities = g_fieldVars->fieldF3;
        opHandler_SETPARTY2(NULL, 0);
    } else {
        g_battleConfig.sealedAbilities = 0;
        g_gameState.config.sealedAbilities = 0;
    }

    g_worldTransfer.vehicle = g_fieldVars->field56;
    setDiscNumber(g_fieldVars->expectedDiscId);
    vfv = g_fieldVars; /* volatile view: forces the tail's reloads of disc/stateFlags */
    disc = vfv->expectedDiscId;
    do { g_gameState.unk048 = disc - 1; } while (0);
    setSalaryEnabled((((u32)vfv->stateFlags >> 3) ^ 1) & 1);
    setVsyncCallback((s32)stopAllSounds);
    setDrawCallback((s32)func_80037D40);
}

/** @brief Sets @ref PARTY_LOCK_FLAG_20 in the partyLockFlag. */
void func_800C0384(void) {
    g_gameState.mainData.partyLockFlag |= PARTY_LOCK_FLAG_20;
}

/** @brief Clears @ref PARTY_LOCK_FLAG_20 in the partyLockFlag. */
void func_800C03A0(void) {
    g_gameState.mainData.partyLockFlag &= ~PARTY_LOCK_FLAG_20;
}

/** @brief Sets @ref PARTY_LOCK_FLAG_10 in the partyLockFlag. */
void func_800C03BC(void) {
    g_gameState.mainData.partyLockFlag |= PARTY_LOCK_FLAG_10;
}

/** @brief Clears @ref PARTY_LOCK_FLAG_10 in the partyLockFlag. */
void func_800C03D8(void) {
    g_gameState.mainData.partyLockFlag &= ~PARTY_LOCK_FLAG_10;
}

/** @brief Sets @ref PARTY_LOCK_FLAG_02 in the partyLockFlag. */
void func_800C03F4(void) {
    g_gameState.mainData.partyLockFlag |= PARTY_LOCK_FLAG_02;
}

/**
 * @brief Test whether the player's inventory holds a given item ID.
 *
 * Linear scan over all @ref ITEM_SLOT_COUNT slots of
 * @c g_gameState.mainData.itemSlots, comparing each slot's @c id.
 *
 * @param itemId Item ID to search for.
 * @return @c 1 if any slot holds @p itemId, otherwise @c 0.
 */
s32 func_800C0410(s32 itemId) {
    s32 i;

    for (i = 0; i < ITEM_SLOT_COUNT; i++) {
        if (g_gameState.mainData.itemSlots[i].id == itemId) {
            return 1;
        }
    }
    return 0;
}

/**
 * Copies 0x40 bytes from g_gameState.chars[5].magic to g_gameState.chars[6].magic using memcopy,
 * then calls memzero16 with g_gameState.chars[6].magic and mode 4.
 */
void func_800C0448(void) {
    memcopy(g_gameState.chars[6].magic, g_gameState.chars[5].magic, 0x40);
    memzero16(g_gameState.chars[6].magic, 4);
}

/**
 * @brief Give stocked magic to the party.
 *
 * Distributes @p quantity copies of spell @p magicId across all present
 * characters (@c exists bit 0): first tops up every existing stack of the
 * spell to the 100 cap, carrying any remainder onward; whatever remains
 * (or the full amount when no character holds the spell) is placed in the
 * first empty magic slot of the first present character that does not
 * already know the spell. Silently drops the spell when everyone who could
 * take it is full.
 *
 * @param magicId  Magic spell ID to add (see MAGIC_* defines).
 * @param quantity Number of copies to add.
 */
void func_800C048C(s32 magicId, s32 quantity) {
    s32 total;
    s32 i;
    s32 j;

    total = 0;
    for (i = 0; i < CHARACTER_COUNT; i++) {
        if (g_gameState.chars[i].exists & CHAR_FLAG_PRESENT) {
            for (j = 0; j < MAGIC_SLOT_COUNT; j++) {
                if (g_gameState.chars[i].magic[j].magicId == magicId) {
                    total = g_gameState.chars[i].magic[j].quantity;
                    total += quantity;
                    if (total > 100) {
                        g_gameState.chars[i].magic[j].quantity = 100;
                        quantity = total - 100;
                        break;
                    }
                    g_gameState.chars[i].magic[j].quantity = total;
                    return;
                }
            }
        }
    }

    if (total == 0) {
        for (i = 0; i < CHARACTER_COUNT; i++) {
            if (g_gameState.chars[i].exists & CHAR_FLAG_PRESENT) {
                for (j = 0; j < MAGIC_SLOT_COUNT; j++) {
                    if (g_gameState.chars[i].magic[j].magicId == 0) {
                        g_gameState.chars[i].magic[j].magicId = magicId;
                        g_gameState.chars[i].magic[j].quantity = quantity;
                        return;
                    }
                }
            }
        }
    } else {
        for (i = 0; i < CHARACTER_COUNT; i++) {
            if (g_gameState.chars[i].exists & CHAR_FLAG_PRESENT) {
                for (j = 0; j < MAGIC_SLOT_COUNT; j++) {
                    if (g_gameState.chars[i].magic[j].magicId == magicId) {
                        goto next_char;
                    }
                }
                for (j = 0; j < MAGIC_SLOT_COUNT; j++) {
                    if (g_gameState.chars[i].magic[j].magicId == 0) {
                        g_gameState.chars[i].magic[j].magicId = magicId;
                        g_gameState.chars[i].magic[j].quantity = quantity;
                        return;
                    }
                }
            }
next_char:
        }
    }
}

/**
 * @brief Apply and clear character slot 7's stocked magic.
 *
 * Walks all 32 magic slots of @c g_gameState.chars[7]; each stocked spell
 * (@c magicId @c != @c 0) is applied via @c func_800C048C, then the whole
 * magic list is zeroed. @c g_gameState.chars[7].magic aliases @c &g_gameState.chars[7].magic
 * (same address, 64 bytes); @c memzero16 clears it in 16-byte units.
 */
void func_800C0634(void) {
    s32 i;

    for (i = 0; i < MAGIC_SLOT_COUNT; i++) {
        s32 magicId = g_gameState.chars[7].magic[i].magicId;
        s32 quantity = g_gameState.chars[7].magic[i].quantity;
        if (magicId != 0) {
            func_800C048C(magicId, quantity);
        }
    }
    memzero16(g_gameState.chars[7].magic, 4);
}
