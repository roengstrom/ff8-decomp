#include "common.h"
#include "field.h"
#include "gamestate.h"
#include "card.h"
#include "sound.h"
#include "cd.h"
#include "battle.h"
#include "overlay.h"
#include "field/fe_object1.h"
#include "field/fe_object1_2.h"
#include "field/fe_object5.h"

/**
 * @brief op159 SEALEDOFF — pop a flag mask and unseal the matching
 *        Ultimecia's-Castle features.
 *
 * Clears the popped bits from @c g_fieldVars->fieldF3 (the "sealed
 * status" byte), mirrors the result into @c D_80082C10 / @c GameConfig.sealedFeatures,
 * and recalculates party stats so menus reflect the newly-available
 * features. Counterpart to @c opHandler_LASTIN (which sets the same
 * byte to all-sealed = @c 0xFF on entry).
 *
 * @return 2 (continue processing).
 */
s32 opHandler_SEALEDOFF(ScriptContext *context) {
    s32 popped = POP(context);
    g_fieldVars->fieldF3 &= ~popped;
    D_80082C10 = g_fieldVars->fieldF3;
    g_gameState.config.sealedFeatures = g_fieldVars->fieldF3;
    recalcPartyStats();
    return 2;
}

/**
 * @brief Pop a character/party token, resolve it via @c findCharacterSlot,
 *        and if the slot is valid hand it to @c func_80036B90.
 */
s32 opHandler_RESETGF(ScriptContext *context) {
    u8 idx;
    s32 result;

    idx = context->stackPtr;
    context->stackPtr = idx - 1;
    result = findCharacterSlot(context->stack[(s8)idx]);
    if (result != 0xFF) {
        func_80036B90(result);
    }
    return 2;
}

/**
 * @brief op131 HOLD — pop three values (top byte discarded, then a
 *        hold/release flag and an actor id) and toggle the
 *        @c CharacterData.exists bits @c 0x6 for that character.
 *
 * @c actorId values below @c 8 index @c g_gameState.chars directly;
 * higher values are looked up via @c findCharacterSlot (which can
 * return @c 0xFF for "no slot", in which case the opcode bails out
 * early without touching @c chars).
 *
 * @return 2 (continue processing).
 */
s32 opHandler_HOLD(ScriptContext *context) {
    u8 idx;
    s32 flag;
    s32 actorId;

    idx = context->stackPtr;
    context->stackPtr = idx - 2;
    flag = context->stack[(s8)(idx - 1)];
    context->stackPtr = idx - 3;
    actorId = context->stack[(s8)(idx - 2)];

    if (actorId >= 8) {
        actorId = findCharacterSlot(actorId);
        if (actorId == 0xFF) {
            return 2;
        }
    }
    if (flag != 0) {
        g_gameState.chars[actorId].exists |= 6;
    } else {
        g_gameState.chars[actorId].exists &= ~6;
    }
    return 2;
}

/**
 * @brief Disable an entity. Clears the "active" flag bit @c 0x8, hides
 *        the render slot (unless field-debug bit @c 0x2 is set), captures
 *        the @c 0x10 flag into @c field_0x24C, and clears the entity's
 *        secondary @c unk249 / @c field_0x24B flags so collision /
 *        scripting skip this entity.
 */
s32 opHandler_SHOW(Actor *actor) {
    *(volatile s32 *)&actor->context.flags = *(volatile s32 *)&actor->context.flags & ~0x8;
    if (!(D_800DE8CC & 0x2)) {
        EntityRenderSlot **base = D_800D9630;
        u8 slot = actor->field_0x256;
        *(u8 *)((s32)base[slot] + 0x60) = 0;
    }
    if (*(volatile s32 *)&actor->context.flags & 0x10) {
        actor->field_0x24C = 1;
    } else {
        actor->field_0x24C = 0;
    }
    actor->unk249 = 0;
    actor->field_0x24B = 0;
    return 2;
}

/**
 * @brief Re-enable an entity. Counterpart of @c opHandler_SHOW — sets the
 *        "active" flag bit @c 0x8, restores render-slot visibility,
 *        re-installs the @c 0x10 flag from @c field_0x24C, then sets
 *        @c field_0x24C / @c unk249 / @c field_0x24B back to 1.
 */
s32 opHandler_HIDE(Actor *actor) {
    *(volatile s32 *)&actor->context.flags = *(volatile s32 *)&actor->context.flags | 0x8;
    if (!(D_800DE8CC & 0x2)) {
        EntityRenderSlot **base = D_800D9630;
        u8 slot = actor->field_0x256;
        *(u8 *)((s32)base[slot] + 0x60) = 1;
    }
    if (actor->field_0x24C != 0) {
        actor->context.flags |= 0x10;
    } else {
        actor->context.flags &= ~0x10;
    }
    actor->field_0x24C = 1;
    actor->unk249 = 1;
    actor->field_0x24B = 1;
    return 2;
}

/** @brief Clear @c field_0x24B (entity "B flag"). */
s32 opHandler_TALKON(Actor *actor) {
    actor->field_0x24B = 0;
    return 2;
}

/** @brief Set @c field_0x24B (entity "B flag") to 1. */
s32 opHandler_TALKOFF(Actor *actor) {
    actor->field_0x24B = 1;
    return 2;
}

/** @brief Clear @c unk249 (entity "self-collision enable" flag). */
s32 opHandler_PUSHON(Actor *actor) {
    actor->unk249 = 0;
    return 2;
}

/** @brief Set @c unk249 (entity "self-collision enable" flag) to 1. */
s32 opHandler_PUSHOFF(Actor *actor) {
    actor->unk249 = 1;
    return 2;
}

/**
 * @brief op0AB — counterpart of @c opHandler_FOLLOWON. Finds the active-
 *        party slot whose @c battleParty member matches
 *        @c e->field_0x255, then disables that entity: clears flag
 *        bit @c 0x4, calls @c func_800B912C with the entity's
 *        animation byte, sets the @c 0x2040 flag pair, and marks the
 *        slot in @c g_fieldEntity.entityIndex as inactive (@c 0xFF).
 *
 * @return 2 (continue processing).
 */
s32 opHandler_FOLLOWOFF(Actor *actor) {
    s32 i;
    for (i = 0; i < 3; i++) {
        if (g_gameState.battleParty[i] == actor->field_0x255) {
            actor->context.flags &= ~4;
            func_800B912C(actor, actor->field_0x24F);
            actor->context.flags |= 0x2040;
            g_fieldEntity.entityIndex[i] = 0xFF;
            break;
        }
    }
    return 2;
}

/**
 * @brief op0AC — locate the active-party slot whose @c battleParty
 *        member matches @c e->field_0x255, then snapshot
 *        @c e->field_0x256 into @c g_fieldEntity.entityIndex for that
 *        slot and clear the entity's "needs broadcast" flag (0x40).
 *
 * @return 2 (continue processing).
 */
s32 opHandler_FOLLOWON(Actor *actor) {
    s32 i;
    for (i = 0; i < 3; i++) {
        if (g_gameState.battleParty[i] == actor->field_0x255) {
            actor->context.flags &= ~0x40;
            g_fieldEntity.entityIndex[i] = actor->field_0x256;
            break;
        }
    }
    return 2;
}

/** @brief Set @c field_0x24C (entity "C flag") to 1. */
s32 opHandler_THROUGHON(Actor *actor) {
    actor->field_0x24C = 1;
    return 2;
}

/** @brief Clear @c field_0x24C (entity "C flag"). */
s32 opHandler_THROUGHOFF(Actor *actor) {
    actor->field_0x24C = 0;
    return 2;
}

/**
 * @brief Pop an entity-id token and stash @c func_8009F74C's result for
 *        that entity in @c resultSlots[0] (so a following @c GETN can
 *        retrieve it).
 */
s32 opHandler_ISTOUCH(Actor *actor) {
    u8 idx;
    s32 val;

    idx = actor->context.stackPtr;
    actor->context.stackPtr = idx - 1;
    val = actor->context.stack[(s8)idx];
    actor->context.resultSlots[0] = func_8009F74C(actor, (s32)D_80085230[val]);
    return 2;
}

/** @brief Pop halfword from stack and store to @c talkRadius. */
s32 opHandler_TALKRADIUS(Actor *actor) {
    u8 idx = actor->context.stackPtr;
    actor->context.stackPtr = idx - 1;
    actor->talkRadius = *(u16 *)&actor->context.stack[(s8)idx];
    return 2;
}

/** @brief Pop halfword from stack and store to @c radius (collision radius). */
s32 opHandler_PUSHRADIUS(Actor *actor) {
    u8 idx = actor->context.stackPtr;
    actor->context.stackPtr = idx - 1;
    actor->radius = *(u16 *)&actor->context.stack[(s8)idx];
    return 2;
}

/**
 * @brief Snapshot the entity's position and a few descriptor bytes into
 *        the result-slot register file so subsequent script @c GETN
 *        opcodes can read them. Position is rounded toward zero by
 *        dividing the fixed-point @c posX/Y/Z by 4096.
 */
s32 opHandler_GETINFO(Actor *actor) {
    actor->context.resultSlots[0] = actor->posX / 4096;
    actor->context.resultSlots[1] = actor->posY / 4096;
    actor->context.resultSlots[2] = actor->posZ / 4096;
    actor->context.resultSlots[4] = actor->field_0x241;
    actor->context.resultSlots[5] = actor->triIdx;
    actor->context.resultSlots[6] = actor->moveSpeed;
    return 2;
}

/**
 * @brief op070 PGETINFO — pop a party-slot id and snapshot the
 *        corresponding party-member entity's position (rounded toward
 *        zero by dividing the fixed-point @c posX/Y/Z by 4096) and a
 *        few descriptor halfwords into the result-slot register file.
 *
 * Counterpart to @c opHandler_GETINFO — same store pattern, but the source
 * entity is the one indexed by @c g_fieldVars->memberSlot[popped]
 * rather than the current Actor.
 *
 * @return 2 (continue processing).
 */
s32 opHandler_PGETINFO(ScriptContext *context) {
    FieldVars *seed = g_fieldVars;
    u8 idx;
    s32 popped;
    s32 entIdx;

    idx = context->stackPtr;
    context->stackPtr = idx - 1;
    popped = context->stack[(s8)idx];
    entIdx = seed->memberSlot[popped];
    context->resultSlots[0] = D_80085224[entIdx].posX / 4096;
    context->resultSlots[1] = D_80085224[entIdx].posY / 4096;
    context->resultSlots[2] = D_80085224[entIdx].posZ / 4096;
    context->resultSlots[4] = D_80085224[entIdx].field_0x241;
    context->resultSlots[5] = D_80085224[entIdx].triIdx;
    context->resultSlots[6] = D_80085224[entIdx].moveSpeed;
    return 2;
}

/** @brief Pop a character-id and stash @c findCharacterSlot's result in @c resultSlots[0]. */
s32 opHandler_WHOAMI(ScriptContext *context) {
    u8 idx = context->stackPtr;
    context->stackPtr = idx - 1;
    context->resultSlots[0] = findCharacterSlot(context->stack[(s8)idx]);
    return 2;
}

/**
 * @brief op??? JUNCTION — swap the active battle party with the bench
 *        roster (and re-sync the per-character GFs).
 *
 * Pops a flag word. Bit 0 selects the direction:
 *   - bit 0 set: SAVE — copy the current active party
 *     (@c g_gameState.battleParty / @c party) into the bench backup
 *     (@c g_fieldVars->partyOrderA / @c partyOrderB), then re-apply
 *     character-data settings via @c func_80036B90 for every character
 *     whose @c characterId is in range.
 *   - bit 0 clear: RESTORE — copy the bench backup back into the
 *     active party.
 *
 * Bit 1 (only meaningful with bit 0 set) chooses between:
 *   - bit 1 set: park @c g_fieldEntity into mode-5 (active-party swap
 *     animation) with a @c 0x16-frame counter and @c unk1AB copied
 *     from the bottom 3 bits of @c stateFlags.
 *   - bit 1 clear: defer to @c func_80036D44 with those same 3 bits.
 *
 * @return 3 (advance two stack slots and continue).
 */
s32 opHandler_JUNCTION(ScriptContext *context) {
    s32 popped;
    s32 saveMode;
    s32 i;

    {
        u8 idx = context->stackPtr;
        context->stackPtr = idx - 1;
        popped = context->stack[(s8)idx];
    }
    saveMode = popped & 1;
    resetCardSlots(saveMode);

    if (saveMode != 0) {
        for (i = 0; i < 3; i++) {
            g_fieldVars->partyOrderA[i] = g_gameState.battleParty[i];
            g_fieldVars->partyOrderB[i] = g_gameState.mainData.party.party[i];
        }

        if (popped & 2) {
            g_fieldEntity.mode = 5;
            g_fieldEntity.counter = 0x16;
            g_fieldEntity.unk1AB = g_fieldVars->stateFlags & 7;
        } else {
            func_80036D44(g_fieldVars->stateFlags & 7);
        }

        for (i = 0; i < 8; i++) {
            if (g_gameState.chars[i].characterId < 8) {
                func_80036B90(i);
            }
        }
        return 3;
    }

    for (i = 0; i < 3; i++) {
        g_gameState.battleParty[i] = g_fieldVars->partyOrderA[i];
        g_gameState.mainData.party.party[i] = g_fieldVars->partyOrderB[i];
    }
    return 3;
}

/**
 * @brief Pop an entity id, look up the corresponding @c Actor in the
 *        @c D_80085230 table, and copy six locator fields (pos x/y/z,
 *        @c field_0x241, @c triIdx, @c moveSpeed) from that
 *        entity into the current one. Finally call @c func_8009A8E0
 *        on the global @c D_8008538C anchor to recompute derived
 *        coordinates.
 *
 * @return 2 (continue processing).
 */
s32 opHandler_COPYINFO(Actor *actor) {
    u8 idx;
    s32 entityId;

    idx = actor->context.stackPtr;
    actor->context.stackPtr = idx - 1;
    entityId = actor->context.stack[(s8)idx];
    actor->posX = D_80085230[entityId]->posX;
    actor->posY = D_80085230[entityId]->posY;
    actor->posZ = D_80085230[entityId]->posZ;
    actor->field_0x241 = D_80085230[entityId]->field_0x241;
    actor->triIdx = D_80085230[entityId]->triIdx;
    actor->moveSpeed = D_80085230[entityId]->moveSpeed;
    func_8009A8E0(D_8008538C);
    return 2;
}

/**
 * @brief Variant of @c opHandler_COPYINFO that resolves the source entity
 *        through the active-party @c memberSlot[] table — pop a
 *        party-slot id, look up @c g_fieldVars->memberSlot[party] for
 *        the field-entity index, and copy six locator fields
 *        (pos x/y/z, @c field_0x241, @c triIdx, @c moveSpeed)
 *        into the current entity. Calls @c func_8009A8E0(D_8008538C)
 *        afterwards to recompute derived state.
 *
 * @return 2 (continue processing).
 */
s32 opHandler_PCOPYINFO(Actor *actor) {
    FieldVars *seed = g_fieldVars;
    u8 idx;
    s32 popped;
    s32 entIdx;

    idx = actor->context.stackPtr;
    actor->context.stackPtr = idx - 1;
    popped = actor->context.stack[(s8)idx];
    entIdx = seed->memberSlot[popped];
    actor->posX = D_80085224[entIdx].posX;
    actor->posY = D_80085224[entIdx].posY;
    actor->posZ = D_80085224[entIdx].posZ;
    actor->field_0x241 = D_80085224[entIdx].field_0x241;
    actor->triIdx = D_80085224[entIdx].triIdx;
    actor->moveSpeed = D_80085224[entIdx].moveSpeed;
    func_8009A8E0(D_8008538C);
    return 2;
}

/**
 * @brief op12D ACTORMODE — pop a mode byte and toggle two flag bits
 *        in the entity's @c flags word, then invoke @c func_800A97E4
 *        with the entity's @c field_0x256 and a mode-specific
 *        (mask, param) tuple to update derived actor state.
 *
 *        Mode  flags          mask  param
 *        ----  -----          ----  -----
 *          0   |= 0x02000000  0x0F    1
 *          1   &= ~0x02000000 0x0F    0
 *          2   &= ~0x01000000 0x22   (unused)
 *          3   |= 0x01000000  0x21   (unused)
 *      other   no flag change, call still fires
 *
 * @return 2 (continue processing).
 */
s32 opHandler_ACTORMODE(Actor *actor) {
    s32 mode = POP(&actor->context);
    s32 a1_val;
    s32 a2_val;

    switch (mode) {
    case 0:
        actor->context.flags |= 0x02000000;
        a1_val = 0xF;
        a2_val = 1;
        break;
    case 1:
        actor->context.flags &= ~0x02000000;
        a1_val = 0xF;
        a2_val = 0;
        break;
    case 2:
        actor->context.flags &= ~0x01000000;
        a1_val = 0x22;
        break;
    case 3:
        actor->context.flags |= 0x01000000;
        a1_val = 0x21;
        break;
    }
    func_800A97E4(actor->field_0x256, a1_val, a2_val, 0);
    return 2;
}

/**
 * @brief op0A3 MOVIEREADY — peek the top two stack slots
 *        (a @c movieIndex and a @c priorityMode) and kick off an
 *        asynchronous CD read of the corresponding movie data into
 *        the @c 0x801E8000 frame buffer.
 *
 *        @c priorityMode picks one of three priorities:
 *          0 -> 8, 1 -> 0xA, anything else -> 6.
 *        The first invocation (active for this script slot) seeds the
 *        movie subsystem (@c func_800A59D0, then @c func_80038868 with
 *        @c D_800C2D14[0..1] as the LBA/size pair) and marks
 *        @c D_800DE4FD as in-flight. Subsequent invocations poll
 *        @c func_800393C8 — they return @c 1 to keep the script
 *        blocked until the read completes, then commit the movie
 *        entry @c (movieIndex + 1) via @c func_801E8104 and pop both
 *        stack slots before returning @c 2.
 *
 * @return 1 while CD read is pending, 2 once committed.
 */
s32 opHandler_MOVIEREADY(ScriptContext *context) {
    s32 priority;
    s32 entryIdx;

    priority = context->stack[(s8)context->stackPtr];
    entryIdx = context->stack[(s8)context->stackPtr - 1] + 1;

    switch (priority) {
    case 0:  priority = 8;   break;
    case 1:  priority = 0xA; break;
    default: priority = 6;   break;
    }

    if ((context->activeMask >> context->scriptSlot) & 1) {
        D_8007064B = 1;
        func_800A59D0();
        func_80038868(D_800C2D14[0], D_800C2D14[1], 0x801E8000, 0);
        D_800DE4FD[0] = 1;
    }

    if (func_800393C8() != 0) {
        return 1;
    }

    D_800DE4FD[0] |= 0x2;
    func_801E8000(priority);
    D_800DE4EC = func_801E8104(D_800C2D14[entryIdx * 2],
                               D_800C2D14[entryIdx * 2 + 1],
                               1,
                               D_8005F13C);
    g_fieldVars->padInitStatus = 0;
    context->stackPtr -= 2;
    return 2;
}

/**
 * @brief op04F MOVIE — enter cinematic mode.
 *
 * Sets the @c 0x1000 bit in @c g_fieldVars->stateFlags and asks the
 * movie subsystem (@c func_801E82CC) whether it is ready to play. If
 * it returns @c 0 (busy), the opcode blocks by returning @c 1.
 *
 * Once ready, doubles the three rate halfwords
 * (@c moveSpeed / @c msgChannel / @c field_0x208) of every active
 * entity — counterpart of @c func_800B14C8's halve step. If the
 * @ref FIELD_STATE_FIELD_READY is clear, also kicks off
 * @c initBattleTransition (so the next scene starts clean) and
 * resets @c g_fieldVars->levelUpDisplayTimer. Finally calls
 * @c func_801E870C to commit the mode switch.
 *
 * @return 1 to block while the movie subsystem is busy, 2 once it
 *         takes over.
 */
s32 opHandler_MOVIE(ScriptContext *context) {
    s32 i;
    Actor *p;
    s32 sc, mc, fc;

    g_fieldVars->stateFlags |= FIELD_STATE_FLAG_1000;
    if (func_801E82CC()) {
        p = D_80085224;
        for (i = 0; i < D_80085388; i++) {
            sc = (s16)p->moveSpeed;
            mc = (s16)p->msgChannel;
            fc = (s16)p->field_0x208;
            p->moveSpeed = sc << 1;
            p->msgChannel = mc << 1;
            p->field_0x208 = fc << 1;
            p++;
        }
        if (!(g_fieldVars->stateFlags & FIELD_STATE_FIELD_READY)) {
            initBattleTransition();
            g_fieldVars->levelUpDisplayTimer = 0;
        }
        func_801E870C();
        return 2;
    }
    return 1;
}

/**
 * @brief Halve the rate triple (@c moveSpeed / @c msgChannel /
 *        @c field_0x208 ) of every active entity,
 *        clear the @c 0x1000 bit of the state flags, and reset two
 *        re-arm flags ( @c g_fieldEntity.unk1A3 — only if @c unk015 is
 *        clear — and @c D_800DE4FD[0] ). Counterpart of @c
 *        opHandler_MOVIE's doubling step — called as part of the
 *        cinematic / movie postlude to restore the dialog channels
 *        after a scene.
 *
 * @note Falls through without a return statement — callers don't read
 *       the value (it leaves @c hi(D_800DE4FD) in @c $v0).
 */
void func_800B14C8(void) {
    s32 i;
    Actor *p;

    g_fieldVars->stateFlags &= ~FIELD_STATE_FLAG_1000;
    p = D_80085224;
    for (i = 0; i < D_80085388; i++) {
        p->moveSpeed = (s16)p->moveSpeed / 2;
        p->msgChannel = (s16)p->msgChannel / 2;
        p->field_0x208 = (s16)p->field_0x208 / 2;
        p++;
    }
    if (g_fieldEntity.unk015 == 0) {
        g_fieldEntity.unk1A3 = 0;
    }
    D_800DE4FD[0] = 0;
}

/**
 * @brief Wait for the movie to finish playing.
 *
 * Re-runs while @c func_801E8B84 returns nonzero, then lets the script
 * advance.
 *
 * @note @c func_801E8B84 lives in another overlay and has no body here, so
 *       "movie still playing" is inferred from the opcode name and from the
 *       shape it shares with the other @c *SYNC waits.
 *
 * @param context Context of the script running the opcode.
 * @return 1 to run this opcode again, 2 once the movie has finished.
 */
s32 opHandler_MOVIESYNC(ScriptContext *context) {
    s32 result;
    result = func_801E8B84(context);
    if (result != 0) {
        return 1;
    }
    return 2;
}

/**
 * @brief op056 SPUREADY — peek the top stack slot as a sound-bank
 *        index and kick off an asynchronous CD read of the
 *        corresponding sample bank into the @c 0x801E8000 staging
 *        buffer.
 *
 * The first invocation (active for this script slot) seeds the
 * subsystem via @c func_800A59D0 + @c func_80038868 with
 * @c D_800C2D14[0..1] as the LBA/size pair and marks
 * @c D_800DE4FD as in-flight. Subsequent invocations poll
 * @c func_800393C8 — returning @c 1 to keep the script blocked
 * until the read completes, then committing the indexed bank entry
 * via @c func_801E8104 (priority @c 0x10, mode @c 2 for SPU vs
 * mode @c 1 for the movie variant in @c MOVIEREADY) before popping
 * the stack and returning @c 2.
 *
 * @return 1 while CD read is pending, 2 once the SPU bank is loaded.
 */
s32 opHandler_SPUREADY(ScriptContext *context) {
    s32 entryIdx = context->stack[(s8)context->stackPtr];

    if ((context->activeMask >> context->scriptSlot) & 1) {
        func_800A59D0();
        func_80038868(D_800C2D14[0], D_800C2D14[1], 0x801E8000, 0);
        D_800DE4FD[0] = 1;
    }
    if (func_800393C8() != 0) {
        return 1;
    }
    D_800DE4FD[0] |= 0x2;
    func_801E8000(0x10);
    func_801E8104(D_800C2E14[entryIdx * 4],
                  D_800C2E14[entryIdx * 4 + 1],
                  2,
                  D_8005F13C);
    func_801E870C();
    context->stackPtr -= 1;
    return 2;
}

/**
 * @brief op164 SPUSYNC — peek top-of-stack; if positive and below the
 *        SPU's free-sample threshold (@c D_800772B8), pop and return 2
 *        (advance). If non-positive, defer to @c func_801E8B98 (an SPU
 *        readiness query) and pop only when it reports ready. Otherwise
 *        return 1 to block the script.
 *
 * Counterpart to SPUREADY — used right after a sample upload to wait
 * for the SPU to actually have the requested sample loaded.
 */
s32 opHandler_SPUSYNC(ScriptContext *context) {
    s32 top = context->stack[(s8)context->stackPtr];
    if (top > 0) {
        if ((u32)top >= D_800772B8) {
            return 1;
        }
    } else {
        if (func_801E8B98() != 0) {
            return 1;
        }
    }
    context->stackPtr--;
    return 2;
}

/**
 * @brief Movie-cut opcode; does nothing in the retail build.
 *
 * @note Purpose uncertain -- the body is empty, so whatever cut/skip
 *       handling the name implies was either dropped before release or
 *       lives entirely in the movie overlay.
 *
 * @param context Context of the script running the opcode (unused).
 * @return 2 (continue processing).
 */
s32 opHandler_MOVIECUT(ScriptContext *context) {
    return 2;
}

/**
 * @brief Pop two values and stage a battle via @c loadBattleCmd. Top of
 *        stack supplies a flag byte (OR'd with @c 1), and the value
 *        below it supplies the encounter id. The returned handle is
 *        stashed in @c D_800DE878 for later use.
 */
s32 opHandler_SETVIBRATE(ScriptContext *context) {
    u8 idx;
    s32 val1, val2;

    idx = context->stackPtr;
    context->stackPtr = idx - 1;
    val1 = context->stack[(s8)idx];
    context->stackPtr = idx - 2;
    val2 = context->stack[(s8)(idx - 1)];
    *(s32 *)D_800DE878 = loadBattleCmd(D_800C5FB0, val2, val1 | 1);
    return 2;
}

/**
 * @brief Stop controller vibration; does nothing in the retail build.
 *
 * @note Purpose uncertain -- the body is empty. The pad is left to finish
 *       whatever effect is already running.
 *
 * @param context Context of the script running the opcode (unused).
 * @return 2 (continue processing).
 */
s32 opHandler_STOPVIBRATE(ScriptContext *context) {
    return 2;
}

/**
 * @brief Wait-on-CD-read opcode body — block until @c func_800393C8
 *        (a CD poll) returns @c 0, then advance.
 */
s32 opHandler_LOADSYNC(ScriptContext *context) {
    if (func_800393C8() == 0) {
        return 2;
    }
    return 1;
}

/**
 * @brief op0CF — reset SPU volume state.
 *
 * Issues @c sndCmdF1, restores both active sound handles to volume
 * @c 0x7F, resets the stored @c musicVolume / @c sfxVolume to
 * @c 0x7F, sets the master volume back to max, and resets the SEQ
 * tempo to @c 0x80.
 *
 * @return 2 (continue processing).
 */
s32 opHandler_INITSOUND(void) {
    sndCmdF1();
    if (g_fieldVars->soundHandle0 != -1) {
        sndCmdC0(g_fieldVars->soundHandle0, 0x7F);
    }
    if (g_fieldVars->soundHandle1 != -1) {
        sndCmdC0(g_fieldVars->soundHandle1, 0x7F);
    }
    g_fieldVars->musicVolume = 0x7F;
    g_fieldVars->sfxVolume = 0x7F;
    sndSetMasterVolume(0x7F);
    sndSeqSetTempo(0x80);
    return 2;
}

/**
 * @brief op0CB SETBATTLEMUSIC — pop a music ID and store as the next
 *        battle's music selector.
 *
 * @return 2 (continue processing).
 */
s32 opHandler_SETBATTLEMUSIC(ScriptContext *context) {
    g_fieldVars->battleMusicId = (u8)POP(context);
    return 2;
}

/**
 * @brief op0B5 MUSICLOAD — pop a sound-bank id and queue a bank load
 *        via the audio CD pipeline if the new bank differs from the
 *        currently-playing one and no SFX is in-flight.
 *
 *        First-time invocation (active for this script slot):
 *          - clears the @c D_800DE8D0 in-flight flag,
 *          - pops the byte and stores it as @c g_fieldVars->nextSoundBank,
 *          - early-returns @c 2 when the bank already matches
 *            @c audioChannel0State, or when an SFX is currently playing
 *            (@c soundHandle1 != -1),
 *          - otherwise toggles @ref FIELD_STATE_FLAG_400 based on
 *            @c fieldCF, marks @c fieldCF = 1, calls @c func_800A59D0,
 *            and kicks off @c func_80037FB0(0, nextSoundBank, D_8005F13C)
 *            to start the actual CD read; sets @c D_800DE8D0 = 1.
 *
 *        Subsequent invocations (and the no-op fall-through path)
 *        check @c g_fieldVars->soundLoadComplete — return @c 1 while
 *        still loading, @c 2 once finished.
 *
 * @return 1 while loading, 2 once the bank is staged.
 */
s32 opHandler_MUSICLOAD(ScriptContext *context) {
    FieldVars *seed;
    if ((context->activeMask >> context->scriptSlot) & 1) {
        u8 sp;
        D_800DE8D0 = 0;
        sp = context->stackPtr;
        seed = g_fieldVars;
        context->stackPtr = sp - 1;
        seed->nextSoundBank = (u8)context->stack[(s8)sp];

        if (g_fieldVars->audioChannel0State == (s8)g_fieldVars->nextSoundBank) return 2;
        if (g_fieldVars->soundHandle1 != -1) return 2;

        if (g_fieldVars->fieldCF != 0) {
            g_fieldVars->stateFlags &= ~FIELD_STATE_FLAG_400;
        } else {
            g_fieldVars->stateFlags |= FIELD_STATE_FLAG_400;
        }
        g_fieldVars->fieldCF = 1;
        func_800A59D0();
        func_80037FB0(0, (s8)g_fieldVars->nextSoundBank, D_8005F13C);
        D_800DE8D0 = 1;
    }
    if (g_fieldVars->soundLoadComplete != 0) {
        return 2;
    }
    return 1;
}

/**
 * @brief Toggle @c fieldCF from the inverse of @ref FIELD_STATE_FLAG_400,
 *        then clear that bit. Helper for the music-state machine.
 */
void func_800B19D4(void) {
    if (g_fieldVars->stateFlags & FIELD_STATE_FLAG_400) {
        g_fieldVars->fieldCF = 0;
    } else {
        g_fieldVars->fieldCF = 1;
    }
    g_fieldVars->stateFlags &= ~FIELD_STATE_FLAG_400;
}

/**
 * @brief op0B4 MUSICCHANGE — if @c D_800DE8C8[8] is set (= a sound-bank
 *        change was queued), clear the flag, copy the staged bank into
 *        @c audioChannel0State, toggle the bank, start the new music
 *        track, save the SPU handle, issue the @c sndCmdC0 with the
 *        current music id, and re-arm via @c func_800B19D4. Otherwise
 *        return 2 (no-op).
 *
 * @return 2 if no change was queued, 3 once the new track is started.
 */
s32 opHandler_MUSICCHANGE(void) {
    u8 *p = D_800DE8C8;
    if (p[8] == 0) {
        return 2;
    }
    p[8] = 0;
    g_fieldVars->audioChannel0State = g_fieldVars->nextSoundBank;
    g_fieldVars->soundHandle0 = sndCmd10(toggleSoundBank());
    sndCmdC0(0, g_fieldVars->musicVolume);
    func_800B19D4();
    return 3;
}

/**
 * @brief op141 — start the next music track using the bank currently
 *        selected by @c g_fieldVars->soundBankSelector, save the
 *        returned handle in @c soundHandle0, then issue @c sndCmdC0
 *        with the current @c musicId and re-arm via
 *        @c func_800B19D4.
 *
 * @return 3 (advance PC after deferred wait).
 */
s32 opHandler_MUSICREPLAY(void) {
    u8 *p;
    if ((s8)g_fieldVars->soundBankSelector == 0) {
        p = D_8005F388;
    } else {
        p = D_80063388;
    }
    g_fieldVars->soundHandle0 = sndCmd10(p);
    sndCmdC0(0, g_fieldVars->musicVolume);
    func_800B19D4();
    return 3;
}

/**
 * @brief op144 — variant of @c MUSICCHANGE that also takes a stack
 *        argument. If @c D_800DE8C8[8] is set, pop the argument,
 *        clear the flag, perform the bank-swap dance, and start the
 *        new track via @c sndCmd12 (instead of @c sndCmd10) with the
 *        popped value as second argument.
 *
 * @return 2 if no swap queued, 3 once the new track is launched.
 */
s32 opHandler_MUSICSKIP(ScriptContext *context) {
    s32 popped = POP(context);
    u8 *flag = D_800DE8C8;
    if (flag[8] == 0) {
        return 2;
    }
    flag[8] = 0;
    g_fieldVars->audioChannel0State = g_fieldVars->nextSoundBank;
    g_fieldVars->soundHandle0 = sndCmd12(toggleSoundBank(), popped);
    sndCmdC0(0, g_fieldVars->musicVolume);
    func_800B19D4();
    return 3;
}

/**
 * @brief MUSICCHANGE variant taking two stack args. Pop @c b (top) and
 *        @c a (below), and once the staged-bank flag is armed, call
 *        @c sndCmd14 with the toggled bank handle, @c a masked to
 *        26 bits, and @c b passed through.
 *
 * @return 2 if no bank swap was queued, 3 once the new track starts.
 */
s32 opHandler_CHOICEMUSIC(ScriptContext *context) {
    s32 b = POP(context);
    s32 a = POP(context);
    s32 aMasked = a & 0x3FFFFFF;
    u8 *flag = D_800DE8C8;
    if (flag[8] == 0) {
        return 2;
    }
    flag[8] = 0;
    g_fieldVars->audioChannel0State = g_fieldVars->nextSoundBank;
    g_fieldVars->soundHandle0 = sndCmd14(toggleSoundBank(), aMasked, b);
    func_800B19D4();
    return 3;
}

/**
 * @brief Music-channel variant: pop @c first (top, the fade-ramp in
 *        frames) and @c second (the priority byte), then once the
 *        staged-bank flag is armed, call @c sndCmd1A with the toggled
 *        bank handle, @c first shifted left by one (= ramp*2 frames),
 *        and @c second masked to 7 bits.
 *
 * @return 2 if no bank swap was queued, 3 once the new track starts.
 */
s32 opHandler_CROSSMUSIC(ScriptContext *context) {
    s32 first = POP(context);
    s32 firstShifted = first << 1;
    s32 second = POP(context);
    s32 secondMasked = second & 0x7F;
    u8 *flag = D_800DE8C8;
    if (flag[8] == 0) {
        return 2;
    }
    flag[8] = 0;
    g_fieldVars->audioChannel0State = g_fieldVars->nextSoundBank;
    g_fieldVars->soundHandle0 = sndCmd1A(toggleSoundBank(), firstShifted, secondMasked);
    func_800B19D4();
    return 3;
}

/**
 * @brief Variant of @c MUSICCHANGE for the SFX channel: pop a tempo /
 *        priority byte, swap the staged sound bank into @c
 *        audioChannel1State, start the new track on bank 0/1 via
 *        @c sndCmd19 (passing the popped value masked to 7 bits), save
 *        the returned handle in @c soundHandle1, and re-arm via
 *        @c func_800B19D4.
 *
 * @return 2 if no bank-swap was queued, 3 once the new SFX track starts.
 */
s32 opHandler_DUALMUSIC(ScriptContext *context) {
    s32 masked = POP(context) & 0x7F;
    u8 *flag = D_800DE8C8;
    u8 *bank;
    if (flag[8] == 0) {
        return 2;
    }
    flag[8] = 0;
    g_fieldVars->audioChannel1State = g_fieldVars->nextSoundBank;
    if (g_fieldVars->soundBankSelector != 0) {
        bank = D_8005F388;
    } else {
        bank = D_80063388;
    }
    g_fieldVars->soundHandle1 = sndCmd19(bank, masked);
    func_800B19D4();
    return 3;
}

/** @brief Pop a flag value and feed it to @c sndSetEngineFlag. */
s32 opHandler_KEYSIGHNCHANGE(ScriptContext *context) {
    u8 idx;

    idx = context->stackPtr;
    context->stackPtr = idx - 1;
    sndSetEngineFlag(context->stack[(s8)idx]);
    return 2;
}

/**
 * @brief op0BF MUSICSTOP — pop a channel index and silence the
 *        corresponding SPU sound handle, then mark both the handle
 *        and the audio-channel state byte as inactive (-1).
 *
 * @return 2 (continue processing).
 */
s32 opHandler_MUSICSTOP(ScriptContext *context) {
    s32 ch = POP(context) & 1;
    s32 handle = (&g_fieldVars->soundHandle0)[ch];
    s8 *p;
    if (handle != -1) {
        sndCmd11(handle);
        (&g_fieldVars->soundHandle0)[ch] = -1;
        p = (s8 *)g_fieldVars + ch;
        p[0xC7] = -1;
    }
    return 2;
}

/** @brief Stash @c sndGetStatus into @c resultSlots[0]. */
s32 opHandler_MUSICSTATUS(Actor *actor) {
    actor->context.resultSlots[0] = sndGetStatus(actor);
    return 2;
}

/**
 * @brief Call @c sndGetSeqPosition and split its packed 32-bit return into
 *        the high halfword (@c resultSlots[0]) and zero-extended low
 *        halfword (@c resultSlots[1]).
 */
s32 opHandler_OP16F(Actor *actor) {
    s32 result;
    result = sndGetSeqPosition(actor);
    actor->context.resultSlots[1] = result;
    actor->context.resultSlots[0] = result >> 16;
    actor->context.resultSlots[1] = *(u16 *)&actor->context.resultSlots[1];
    return 2;
}

/**
 * @brief op0C0 MUSICVOL — pop a volume and channel index, ramp the
 *        corresponding SPU sound handle to that volume, and persist
 *        the new value in @c g_fieldVars->musicVolume /
 *        @c g_fieldVars->sfxVolume (the @c [musicVolume,sfxVolume]
 *        adjacent-byte pair indexed by @c channel&1).
 *
 * @return 2 (continue processing).
 */
s32 opHandler_MUSICVOL(ScriptContext *context) {
    s32 vol = POP(context);
    s32 ch = POP(context) & 1;
    u8 *p;
    sndCmdC1((&g_fieldVars->soundHandle0)[ch], 0x10, vol);
    /* 0xC5 is FieldVars.musicVolume, with sfxVolume adjacent at 0xC6. The
     * base+ch expression must come first and the constant index second, so gcc
     * folds 0xC5 into the store displacement. The typed equivalents
     * (&g_fieldVars->musicVolume)[ch] and p = &g_fieldVars->musicVolume; p[ch]
     * do not match. */
    p = (u8 *)g_fieldVars + ch;
    p[0xC5] = vol;
    return 2;
}

/**
 * @brief op0C1 MUSICVOLTRANS — pop a volume, fade ramp, and channel
 *        index; ramp the SPU channel's volume over @c ramp*2 frames
 *        and persist the new volume in @c g_fieldVars->musicVolume /
 *        @c sfxVolume.
 *
 * @return 2 (continue processing).
 */
s32 opHandler_MUSICVOLTRANS(ScriptContext *context) {
    s32 vol = POP(context);
    s32 ramp = POP(context);
    s32 ch = POP(context) & 1;
    u8 *p;
    sndCmdC1((&g_fieldVars->soundHandle0)[ch], ramp << 1, vol);
    /* Same base+ch / 0xC5 phrasing as opHandler_MUSICVOL, and required for the
     * same reason — see the note there. */
    p = (u8 *)g_fieldVars + ch;
    p[0xC5] = vol;
    return 2;
}

/**
 * @brief 4-arg variant of @c MUSICVOLTRANS. Pop @c vol (top), @c depth,
 *        @c ramp, and a channel selector; ramp the SPU channel's volume
 *        toward @c vol with the extended parameters and persist the new
 *        volume in @c g_fieldVars->musicVolume / @c sfxVolume (indexed
 *        by @c channel&1, the adjacent-byte pair at offset @c 0xC5).
 *
 * @return 2 (continue processing).
 */
s32 opHandler_MUSICVOLFADE(ScriptContext *context) {
    s32 vol = POP(context);
    s32 depth = POP(context);
    s32 ramp = POP(context);
    s32 ch = POP(context) & 1;
    u8 *p;
    sndCmdC2((&g_fieldVars->soundHandle0)[ch], ramp << 1, depth, vol);
    /* Same base+ch / 0xC5 phrasing as opHandler_MUSICVOL, and required for the
     * same reason — see the note there. */
    p = (u8 *)g_fieldVars + ch;
    p[0xC5] = vol;
    return 2;
}

/**
 * @brief Wait for a music volume fade to reach its target.
 *
 * Re-runs while @ref sndGetMaxVolume still reports a fade in progress.
 *
 * @param context Context of the script running the opcode (unused).
 * @return 1 to run this opcode again, 2 once the fade has settled.
 */
s32 opHandler_MUSICVOLSYNC(ScriptContext *context) {
    s32 result;
    result = sndGetMaxVolume(1);
    if (result == 0) {
        return 2;
    }
    return 1;
}

/**
 * @brief Wait for the SPU to finish its current command, then keep
 *        retrying @c sndUploadSamples(D_8005F13C, 1) until it returns
 *        a non-error value, then mark @c D_800DE8D5 as ready.
 *
 * Helper for the music-load family — synchronously stages the next
 * sample bank into SPU RAM.
 */
void func_800B2188(void) {
    while (sndGetEngineState() != 0) {
    }
    while (sndUploadSamples(D_8005F13C, 1) == -1) {
    }
    D_800DE8D5 = 1;
}

/**
 * @brief Helper for @c EFFECTLOAD (op0BD, @c opHandler_EFFECTLOAD) — looks up
 *        the SFX entry indexed by @c g_fieldVars->audioChannel2State
 *        in the @c D_800C2E1C @c {LBA, size} table and issues a
 *        @c cdRead targeting the @c D_8005F13C SPU staging buffer,
 *        with @c func_800B2188 as the load-complete callback. Also
 *        clears the @c D_800DE8D5 ready-flag so the caller's poll
 *        loop waits for the callback to flip it back on.
 */
void func_800B21E0(void) {
    s32 entryIdx = g_fieldVars->audioChannel2State;
    D_800DE8D5 = 0;
    func_800A59D0();
    cdRead(D_800C2E1C[entryIdx * 2],
           D_800C2E1C[entryIdx * 2 + 1],
           (u8 *)D_8005F13C,
           func_800B2188);
}

/**
 * @brief op0BD EFFECTLOAD — pop an SFX entry id, stash it in
 *        @c g_fieldVars->audioChannel2State, and kick off a deferred
 *        CD load of the corresponding sample bank via the helper
 *        @c func_800B21E0 (which looks the entry up in
 *        @c D_800C2E1C and issues @c cdRead targeting the SPU staging
 *        buffer).
 *
 * The stack-pop / store / helper-call dance is only performed on the
 * first invocation (gated by the per-group active-mask bit).
 * Subsequent invocations just poll @c D_800DE8D5 — returning @c 1
 * (block) until the load-complete callback flips it back on, then
 * @c 2 to advance.
 *
 * @return 1 while the CD read is pending, 2 once the bank is staged.
 */
s32 opHandler_EFFECTLOAD(ScriptContext *context) {
    FieldVars *seed;
    if ((context->activeMask >> context->scriptSlot) & 1) {
        u8 sp = context->stackPtr;
        seed = g_fieldVars;
        context->stackPtr = sp - 1;
        seed->audioChannel2State = (s8)context->stack[(s8)sp];
        func_800B21E0();
    }
    if (D_800DE8D5 != 0) {
        return 2;
    }
    return 1;
}

/**
 * @brief op0BC EFFECTPLAY — pop 4 SFX parameters and start playback.
 *
 * @return 2 (continue processing).
 */
s32 opHandler_EFFECTPLAY(ScriptContext *context) {
    s32 d = POP(context);
    s32 c = POP(context);
    s32 b = POP(context);
    s32 a = POP(context);
    sndPlaySfx(a, b, c, d);
    return 2;
}
