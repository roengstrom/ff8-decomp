#include "common.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"
#include "psxsdk/libc.h"
#include "battle.h"


u8 *emitDrawEnvPackets(u32 *ot, u8 *pkt);
void callCdTick(void);
void shutdownCardSubsystem(void);
void initBattleSubsystems(void);
s32 func_80047384(void);
void func_800472E4(void);
void func_800472F4(void);
s32 getAnimFrameParam(s32, s32);
u16 remapControllerInput(s32);
s32 func_80027DB4(s32, s32, s32);
s32 func_80027CF8(s32, s32, s32);
s32 getAnimFrameStatusFlags(s32, s32);
s32 func_8002CF54(s32);
void decrementSfxCounter(void);
s32 GetActiveFlag(s32);
void dispatchBattleEntity(s32, s32, s32);
void updateCameraVibrate(void);
void updatePaletteTransition(void);
void stepAnimEntries(void);

extern u8 g_animInitialized;
extern u8 g_animFlag;
extern CardDataBlock g_cardData;
extern s8 g_cardFlag; /**< D_80082FD4: Memory card operation status flag. */
extern u16 g_animState;
extern u8 g_cardFilename[];  /* encoded save filename (max 8 chars + null) */
extern s16 g_cardFileSlot;   /* save slot index */
extern u8 g_cardFileType;    /* card/save type */
extern u8 g_cardFileActive;
extern u8 g_animCurveFadeOut[];
extern u8 g_animCurveFadeIn[];
extern u8 D_800101C4[];
extern u8 D_800101CC[];
extern DRAWENV *g_activeDrawEnv;
extern BattleDisplayEntity g_battleEntities[];
extern u8 g_paletteIndices[];

/**
 * @brief Set or clear opacity of a battle animation entity.
 * @param idx Entity index (masked to 0 or 1).
 * @param val If nonzero, set to 0xFF (visible); otherwise 0 (hidden).
 */
void setAnimEntityOpacity(s32 idx, s32 val) {
    BattleAnimEntity *entry = &g_battleAnims.entities[idx & 1];
    if (val != 0) {
        entry->opacity = 0xFF;
    } else {
        entry->opacity = 0;
    }
}


/**
 * @brief Set animation parameters on a battle entity.
 *
 * Conditionally updates field07 and field06 (skipped if < 0),
 * then marks the entity active by setting field0A to 1.
 *
 * @param idx Entity index (masked to 0 or 1).
 * @param param7 Value for field07 (-1 to skip).
 * @param param6 Value for field06 (-1 to skip).
 */
void setAnimEntityParams(s32 idx, s32 param7, s32 param6) {
    BattleAnimEntity *entry = &g_battleAnims.entities[idx & 1];
    if (param7 >= 0) {
        entry->field07 = param7;
    }
    if (param6 >= 0) {
        entry->field06 = param6;
    }
    entry->field0A = 1;
}


/**
 * @brief Get a global coordinate (X or Y) for a battle anim slot.
 * @param idx Entity index (masked to 0 or 1).
 * @param axis 0 = X, 1 = Y (clamped to [0,1]).
 * @return The coordinate value as s16.
 */
s16 getAnimGlobalCoord(s32 idx, s32 axis) {
    s32 slot = idx & 1;

    do { slot++; slot--; } while (0); /* Regalloc: boost slot priority for s-reg order */

    if (axis >= 0) {
        idx = 1;
        if (axis < 2) {
            idx = axis;
        }
    } else {
        idx = 0;
    }
    return g_battleAnims.globalCoords[slot][idx];
}


/**
 * @brief Set global coordinates (X and Y) for a battle anim slot.
 * @param idx Entity index (masked to 0 or 1).
 * @param x X coordinate.
 * @param y Y coordinate.
 */
void setAnimGlobalCoords(s32 idx, s16 x, s16 y) {
    idx &= 1;
    g_battleAnims.globalCoords[idx][0] = x;
    g_battleAnims.globalCoords[idx][1] = y;
}


/**
 * @brief Read a parameter from an animation frame slot.
 * @param idx Entity index (masked to 0 or 1).
 * @param param Parameter index (clamped to [0,3]).
 * @param frameOffset Frame counter offset to subtract.
 * @return Parameter value (u8), or -1 if slot is inactive or wrong type.
 */
s32 getAnimFrameSlotParam(s32 idx, s32 param, s32 frameOffset) {
    BattleAnimEntity *entity;
    AnimFrame *frame;
    s32 frameSlot;
    s32 result;

    idx &= 1;
    entity = &g_battleAnims.entities[g_battleAnims.entities[idx].linkedIdx];
    frameSlot = (entity->frameCounter - frameOffset) & 7;
    result = -1;
    frame = &entity->frames[frameSlot];

    if (frame->field00 == 0) {
        param = CLAMP(param, 0, 3);
        if ((frame->field01 >> 4) == 1) {
            result = frame->params[param];
        }
    }

    return result;
}


/**
 * @brief Check if battle animation entity 0 has an active frame.
 * @return 1 if entity 0 has an active frame, 0 otherwise.
 */
s32 isAnimActive(void) {
    return getAnimFrameType(0, 0) >= 0;
}


/**
 * @brief Get the type of an animation frame slot, with optional sync.
 * @param idx Entity index (bit 0 selects entity).
 * @param frameOffset Frame counter offset to subtract.
 * @return Frame type (field01 >> 4), or -1 if slot is inactive.
 */
s32 getAnimFrameType(s32 idx, s32 frameOffset) {
    s32 syncFlag;
    s32 slot;
    BattleAnimEntity *entity;
    AnimFrame *frame;
    s32 frameSlot;

    syncFlag = func_80047384() & 4;
    if (syncFlag == 0) {
        func_800472E4();
    }

    slot = idx & 1;
    entity = &g_battleAnims.entities[g_battleAnims.entities[slot].linkedIdx];
    frameSlot = (entity->frameCounter - frameOffset) & 7;
    frame = &entity->frames[frameSlot];

    if (syncFlag == 0) {
        func_800472F4();
    }

    if (frame->field00 != 0) {
        return -1;
    }
    return frame->field01 >> 4;
}


/**
 * @brief Set an s16 value in the unk10 array of both battle animation entities.
 *
 * Writes @p value to both entities' unk10[index].
 *
 * @param unused Unused parameter.
 * @param index Index into the unk10 array (0-3).
 * @param value Value to store.
 */
void setAnimUnk10Both(s32 unused, s32 index, s16 value) {
    int new_var;
    new_var = 1;
    g_battleAnims.entities[0].unk10[index] = value;
    g_battleAnims.entities[new_var].unk10[index] = value;
}


/**
 * @brief Initialize a linked battle animation entity.
 * @param idx Entity index (selects via linkedIdx).
 * @param frameId Frame ID to set in each frame's field02.
 */
void resetAnimEntity(s32 idx, s32 frameId) {
    AnimFrame *fp;
    s32 fid;
    BattleAnimEntity *entity;
    s32 i;

    entity = &g_battleAnims.entities[g_battleAnims.entities[idx].linkedIdx];
    entity->frameCounter = 0;
    entity->field0A = 0;
    entity->field0C = g_battleAnims.defaultColor;
    entity->field0D = g_battleAnims.defaultColor;
    entity->field0E = g_battleAnims.defaultColor;
    entity->field0F = g_battleAnims.defaultColor;

    fid = frameId;
    for (i = 0; 8 > i; i++) {
        AnimFrame *frame = &entity->frames[i];
        frame->field00 = 0;
        frame->field01 = 0;
        fp = frame;
        fp->field02 = fid;
        fp->params[0] = 0;
        frame->params[1] = 0;
        frame->params[2] = 0;
        frame->params[3] = 0;
        fp->field08 = 0;
        frame->field0A = 0;
        frame->field0C = 0;
        frame->field0E = 0;
        fp->field10 = 0;
        fp->field12 = 0;
    }
}


/**
 * @brief Initialize a battle entity's color fields from the global default.
 *
 * Sets all four color fields (0C-0F) to g_battleAnims.defaultColor,
 * then calls resetAnimEntity to reset frame state.
 *
 * @param idx Entity index (0 or 1).
 */
void initAnimEntityColor(s32 idx) {
    BattleAnimEntity *entity = &g_battleAnims.entities[idx];
    entity->field0C = g_battleAnims.defaultColor;
    entity->field0D = g_battleAnims.defaultColor;
    entity->field0E = g_battleAnims.defaultColor;
    entity->field0F = g_battleAnims.defaultColor;
    resetAnimEntity(idx, 0);
}


/**
 * @brief Initialize battle animation state and wait for completion.
 *
 * Sets g_animInitialized to 1, initializes both animation slots via resetAnimEntity,
 * triggers a fade via func_80039764(3), then polls func_80027360 up to 24
 * frames. Finishes with VSync(2).
 */
void initAnimStateAndWait(void) {
    s32 i;

    g_animInitialized = 1;
    for (i = 0; i < 2; i++) {
        resetAnimEntity(i, 0);
    }
    func_80039764(3);
    for (i = 0; i < 24; i++) {
        VSync(0);
        if (func_80027360(0) != 0) {
            break;
        }
    }
    VSync(2);
}


/** @brief Initializes g_animInitialized to 0, calls func_80039764(0), then loops twice calling resetAnimEntity(i, 0). */
void resetAnimState(void) {
    s32 i;
    g_animInitialized = 0;
    func_80039764(0);
    for (i = 0; i < 2; i++) {
        resetAnimEntity(i, 0);
    }
}


/** @brief Get the current value of g_animState (global u16 state variable). */
u16 getAnimGlobalState(void) {
    return g_animState;
}


/**
 * @brief Set g_animState to a new value.
 * @param value Value to store.
 * @return The value that was set.
 */
s32 setAnimGlobalState(s32 value) {
    g_animState = value;
    return value;
}


/**
 * @brief Set the global flag g_animFlag.
 * @param value Value to store in g_animFlag.
 */
void setAnimFlag(s32 value) {
    g_animFlag = value;
}


/**
 * @brief Initialize or reset the CD audio/streaming subsystem state.
 * @note Calls several initialization functions and resets a counter at g_battleAnims + 0x9C4 to 0.
 *       Passes two g_battleAnims buffer pointers (offsets 0x188 and 0x1AC) to func_8003BC24.
 */
void initCdAnimSubsystem(void) {
    BattleAnimState *bas = &g_battleAnims;
    func_800982B8();
    func_8003BC24(bas->cdBufA, bas->cdBufB);
    cdInitHandlerWrapper();
    initAnimStateAndWait();
    bas->cdStreamCounter = 0;
}


/**
 * @brief Initialize GPU display and clear battle animation state fields.
 *
 * Calls GsInitGraph, GsDefDispBuff, and initCdAnimSubsystem, then
 * resets display state fields.
 */
void initBattleDisplay(void) {
    func_800982D8();
    func_800980D0();
    initCdAnimSubsystem();
    g_battleAnims.field6FC = 0;
    g_battleAnims.field9C2 = 0x4611;
    g_battleAnims.field9C8 = 0;
    g_battleAnims.field9CC = 0;
}


/** @brief Calls initBattleSubsystems, callCdTick, and shutdownCardSubsystem in sequence. */
void shutdownAnimSubsystem(void) {
    initBattleSubsystems();
    callCdTick();
    shutdownCardSubsystem();
}


/**
 * @brief Convert 3 RGB555 CLUT entries to RGB888 palette colors.
 * @param clut Pointer to RGB555 color lookup table.
 *
 * Reads entries at indices g_paletteIndices[0..2] (3, 5, 6), converts from
 * RGB555 to RGB888 with bit extension, and stores as 0x40BBGGRR.
 */
void convertClutPalette(u16 *clut) {
    s32 i;

    for (i = 0; i < 3; i++) {
        u16 color = clut[g_paletteIndices[i]];
        s32 r = color & 0x1F;
        s32 g = color >> 2;
        s32 b = color >> 7;
        r <<= 3;
        g &= 0xF8;
        b &= 0xF8;

        if (r) r |= 7;
        if (g) g |= 7;
        if (b) b |= 7;

        g &= 0xFF;
        b &= 0xFF;
        g_battleAnims.palette[i] = (r | (g << 8) | (b << 16)) | 0x40000000;
    }
}


/**
 * @brief Load a TIM image (CLUT + pixels) into VRAM, preserving a region.
 * @param data TIM image with CLUT and pixel sections.
 */
void loadBattleTimImage(Tim *data) {
    RECT rect;
    u8 storeBuf[0x100];

    TimSection *clut = &data->clut;
    TimSection *pixels = (TimSection *)((u8 *)clut + clut->len);
    rect = pixels->rect;
    rect.x = 0x380;
    rect.y = 0x100;
    LoadImage(&rect, pixels->data);
    DrawSync(0);

    rect.x = 0x110;
    rect.y = 0xE8;
    rect.w = 0x10;
    rect.h = 0x8;
    StoreImage(&rect, storeBuf);
    DrawSync(0);

    rect = clut->rect;
    rect.x = 0x100;
    rect.y = 0xE0;
    convertClutPalette(clut->data);
    LoadImage(&rect, clut->data);
    DrawSync(0);

    rect.x = 0x110;
    rect.y = 0xE8;
    rect.w = 0x10;
    rect.h = 0x8;
    LoadImage(&rect, storeBuf);
    DrawSync(0);
}


/** @brief Wrapper that calls sfxInit. */
void callSfxInit(void) {
    sfxInit();
}


/** @brief Wrapper that calls sfxUpdate. */
void callSfxUpdate(void) {
    sfxUpdate();
}


/** @brief Wrapper that calls cdTick (likely a CD subsystem tick or finalization). */
void callCdTick(void) {
    cdTick();
}


/**
 * @brief Copy a null-terminated string from src to dst (strcpy implementation).
 * @param dst Destination buffer.
 * @param src Source null-terminated string.
 */
void btlStrcpy(u8 *dst, u8 *src) {
    s32 c;
    do {
        c = *src++;
        *dst++ = c;
    } while (c != 0);
}


/**
 * @brief Append a null-terminated string to dst (strcat implementation).
 * @param dst Destination buffer containing an existing null-terminated string.
 * @param src Source null-terminated string to append.
 */
void btlStrcat(u8 *dst, u8 *src) {
    do { } while (*dst++);
    btlStrcpy(dst - 1, src);
}


/**
 * @brief Copy a block of bytes from src to dst (memcpy implementation).
 * @param src Source buffer.
 * @param dst Destination buffer.
 * @param numBytes Number of bytes to copy.
 */
void btlMemcpy(u8 *src, u8 *dst, s32 numBytes) {
    while (numBytes > 0) {
        *dst++ = *src++;
        numBytes--;
    }
}


/**
 * @brief Wait for vertical sync (VSync wrapper).
 * @param mode VSync mode parameter (passed to VSync).
 */
void waitVSync(s32 mode) { VSync(); }


/**
 * @brief Set the memory card operation flag.
 * @param val Value to store in g_cardFlag.
 */
void setCardFlag(s8 val) {
    g_cardFlag = val;
}


/**
 * @brief Encode a memory card port and slot into a single byte.
 * @param port Memory card port number (reduced mod 2: 0 or 1).
 * @param slot Memory card slot number (reduced mod 4: 0-3).
 * @return Packed value: (port << 4) | slot.
 */
s32 packCardId(s32 port, s32 slot) {
    slot %= 4;
    port %= 2;
    return (port << 4) | slot;
}


/**
 * @brief Extract the memory card port number from a packed card identifier.
 * @param id Packed card identifier (port in bit 4).
 * @return Port number (0 or 1).
 */
s32 getCardPort(s32 id) {
    return (id >> 4) % 2;
}


/**
 * @brief Extract the memory card slot number from a packed card identifier.
 * @param id Packed card identifier (slot in lower 2 bits).
 * @return Slot number (0-3).
 */
s32 getCardSlot(s32 id) {
    return id % 4;
}


/**
 * @brief Wait for a memory card operation to complete on the specified port.
 * @param id Packed card identifier; port is extracted via getCardPort.
 */
void waitCardReady(s32 id) {
    _card_wait(getCardPort(id));
}


/**
 * @brief Test a BIOS event (wrapper for PsyQ TestEvent).
 * @param event Event descriptor to test.
 * @return Nonzero if the event has been delivered.
 */
s32 testCardEvent(s32 event) { return TestEvent(event); }


/**
 * @brief Poll the first 4 memory card events and return which one fired.
 * @return Event index 0-3 if an event was delivered, or -1 if none fired.
 * @note Tests events from g_cardData array entries [0..3] in order.
 */
s32 pollCardEvents(void) {

    CardDataBlock *blk = &g_cardData;
    if (testCardEvent(blk->events[0])) return 0;
    if (testCardEvent(blk->events[1])) return 1;
    if (testCardEvent(blk->events[2])) return 2;
    if (testCardEvent(blk->events[3])) return 3;
    return -1;
}


/**
 * @brief Wait up to 180 frames (0xB4) for a memory card event, polling each VSync.
 * @return Event index 0-3 if an event fires, or 2 (timeout) if none fires within the limit.
 */
s32 waitCardEvent(void) {
    s32 i;
    s32 result;
    for (i = 0; i < 0xB4; i++) {
        result = pollCardEvents();
        if (result != -1) return result;
        waitVSync(0);
    }
    return 2;
}


/** @brief Poll memory card events once, returning the result.
 * @return Event index 0-3 if fired, or -1 if none.
 */
s32 pollCardEventsDiscard(void) { return pollCardEvents(); }


/**
 * @brief Poll the second set of 4 memory card events and return which one fired.
 * @return Event index 0-3 if an event was delivered, or -1 if none fired.
 * @note Tests events from g_cardData array entries [4..7] in order.
 */
s32 pollCardEventsSecondary(void) {

    CardDataBlock *blk = &g_cardData;
    if (TestEvent(blk->events[4])) return 0;
    if (TestEvent(blk->events[5])) return 1;
    if (TestEvent(blk->events[6])) return 2;
    if (TestEvent(blk->events[7])) return 3;
    return -1;
}


/**
 * @brief Busy-wait up to 16384 iterations for a secondary memory card event.
 * @return Event index 0-3 if an event fires, or 2 (timeout) if none fires within the limit.
 * @note Unlike waitCardEvent, this does not VSync between polls (tight busy-wait).
 */
s32 busyWaitCardEvent(void) {
    s32 i;
    s32 result;
    for (i = 0; i < 0x4000; i++) {
        result = pollCardEventsSecondary();
        if (result != -1) return result;
    }
    return 2;
}


/**
 * @brief Read a memory card status byte for a given port and slot.
 * @param cardId Packed card identifier (port in bit 4, slot in lower bits).
 * @return Status byte from CardDataBlock.status[port][slot].
 */
s32 getCardStatus(s32 cardId) {
    CardDataBlock *blk = &g_cardData;
    s32 port = getCardPort(cardId);
    s32 slot = getCardSlot(cardId);
    return blk->status[port][slot];
}


/**
 * @brief Write a memory card status byte for a given port and slot.
 * @param cardId Packed card identifier (port in bit 4, slot in lower bits).
 * @param val Status value to write.
 */
void setCardStatus(s32 cardId, u8 val) {
    CardDataBlock *blk = &g_cardData;
    s32 port = getCardPort(cardId);
    s32 slot = getCardSlot(cardId);
    blk->status[port][slot] = val;
}


/**
 * @brief Write a secondary memory card status byte for a given port and slot.
 * @param cardId Packed card identifier (port in bit 4, slot in lower bits).
 * @param val Status value to write.
 */
void setCardStatusSecondary(s32 cardId, u8 val) {
    CardDataBlock *blk = &g_cardData;
    s32 port = getCardPort(cardId);
    s32 slot = getCardSlot(cardId);
    blk->statusAlt[port][slot] = val;
}


/** @brief Mark a memory card slot as busy/active (set status byte to 1). */
void markCardBusy(s32 cardId) {
    setCardStatus(cardId, 1);
}


/** @brief Read memory card status for a given card slot.
 * @param cardId Packed card identifier.
 * @return Status byte.
 */
s32 readCardStatusDiscard(s32 cardId) {
    return getCardStatus(cardId); // Return value appears unused
}


/**
 * @brief Query memory card info and return status code.
 *
 * Marks the card's command byte as pending (cmdBytes[port][slot] = 2),
 * then issues _card_info and waits for the event result. Retries up to
 * 180 times if the card doesn't respond.
 *
 * @param cardId Packed card identifier (used for _card_info calls).
 * @param chanId Packed card identifier (used for port/slot lookup).
 * @return 0 if card has no status, 1 if card present or new,
 *         3 on unknown event, 4 on timeout.
 * @note K&R declaration + shadowed chanId required for matching codegen.
 */
s32 getCardInfo(cardId, chanId)
s32 cardId;
s32 chanId;
{
    s32 port;
    CardDataBlock *card;
    s32 chanId;
    s32 slot;
    s32 retries;

    card = &g_cardData;
    port = getCardPort(chanId);
    slot = getCardSlot(chanId);

    card->cmdBytes[port][slot] = 2;
    for (retries = 0; retries < 180; retries++) {
        waitCardReady(cardId);
        if (_card_info(cardId) == 0)
            continue;
        switch (waitCardEvent()) {
        case 0:
            return getCardStatus(cardId) != 0;
        case 1:
            markCardBusy(cardId);
            return 4;
        case 3:
            markCardBusy(cardId);
            setCardStatusSecondary(cardId, 0);
            return 1;
        default:
            markCardBusy(cardId);
            return 3;
        }
    }
    markCardBusy(cardId);
    return 4;
}


/**
 * @brief Initialize all memory card event handlers.
 *
 * Iterates over 2 ports and 4 events per port, calling packCardId
 * to get an event handle and waitCardReady to configure it. Then calls
 * pollCardEventsDiscard to finalize.
 */
void initCardEventHandlers(void) {
    s32 j;
    s32 i;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            waitCardReady(packCardId(i, j));
        }
    }
    pollCardEventsDiscard();
}


/**
 * @brief Initialize the memory card data block.
 *
 * Clears the status byte, fills all command byte slots with 2
 * (2 ports x 4 slots), then calls initCardEventHandlers.
 */
void initCardDataBlock(void)
{
    CardDataBlock *card = &g_cardData;
    s32 i, j;

    card->statusByte = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            card->cmdBytes[i][j] = 2;
        }
    }
    initCardEventHandlers();
}


/**
 * @brief Poll memory card events and advance the card state machine.
 *
 * State 0 (idle): Issues _card_info to start a card operation, transitions
 * to state 1, sets the poll timer to 180 frames.
 *
 * State 1 (polling): Calls pollCardEventsDiscard and dispatches on the result:
 *   event 0: Card ready — clears state, resets cmd byte, returns getCardStatus != 0.
 *   event 1: Match — clears state, marks busy, returns 4.
 *   event 3: New card — clears state, resets cmd byte, marks busy, returns 1.
 *   event -1: Timeout tick — decrements timer and cmd byte. If timer expires,
 *             resets and returns 3.
 *   other: Decrements cmd byte. If it reaches 0, resets to 1 and returns 3.
 *          Otherwise marks busy and returns 4.
 *
 * Any other state: marks busy and returns -1.
 *
 * @param cardId Packed card identifier.
 * @return Status code, or -1 if idle/error.
 */
s32 pollCardStatus(s32 cardId) {
    CardDataBlock *card = &g_cardData;
    s32 port;
    s32 state;
    s32 event;
    u8 *cmdByte;

    port = getCardPort(cardId);
    cmdByte = &card->cmdBytes[port][getCardSlot(cardId)];
    state = card->statusByte;

    switch (state) {
    case 0:
        if (_card_info(cardId) != 0) {
            card->statusByte = 1;
            card->pad22[0] = 180;
        }
        break;
    case 1:
        event = pollCardEventsDiscard();
        switch (event) {
        case 0:
            card->statusByte = 0;
            *cmdByte = 2;
            return getCardStatus(cardId) != 0;
        case 3:
            card->statusByte = 0;
            *cmdByte = 2;
            markCardBusy(cardId);
            setCardStatusSecondary(cardId, 0);
            return 1;
        case 1:
            card->statusByte = 0;
            markCardBusy(cardId);
            return 4;
        case -1:
            card->pad22[0]--;
            (*cmdByte)--;
            if (card->pad22[0] != 0) {
                return -1;
            }
            card->pad22[0] = state;
            card->statusByte = 0;
            markCardBusy(cardId);
            return 3;
        default:
            card->statusByte = 0;
            (*cmdByte)--;
            if ((s8)*cmdByte <= 0) {
                *cmdByte = 1;
                markCardBusy(cardId);
                return 3;
            }
            markCardBusy(cardId);
            return 4;
        }
    default:
        markCardBusy(cardId);
        break;
    }
    return -1;
}


/**
 * @brief Attempt to load a memory card save, retrying up to 180 times.
 *
 * Waits for the card, issues _card_load, then polls for completion.
 * Updates card status bytes based on the result.
 *
 * @param id Packed card identifier.
 * @return 0 on success, 2 if new card detected, 3 on timeout, 4 on other failure.
 */
s32 loadCardSave(s32 id) {
    s32 counter;

    counter = 0;
    do {
        waitCardReady(id);
        if (_card_load(id) != 0) {
            s32 result = waitCardEvent();
            switch (result) {
            case 0:
                setCardStatusSecondary(id, 0);
                setCardStatus(id, 0);
                return 0;
            case 3:
                setCardStatusSecondary(id, 1);
                markCardBusy(id);
                return 2;
            case 2:
                markCardBusy(id);
                return 3;
            default:
                markCardBusy(id);
                return 4;
            }
        }
        counter++;
    } while (counter < 0xB4);
    return 4;
}


/**
 * @brief Clear a memory card, retrying up to 180 times.
 *
 * Calls _card_clear in a retry loop. On success, waits for a card event
 * and attempts loadCardSave. Marks card busy on timeout.
 *
 * @param id Packed card identifier.
 * @return 0 on success, 3 on event failure, 4 on timeout/busy.
 */
s32 clearCardSync(s32 id) {
    s32 counter;
    s32 one;

    counter = 0;
    one = 1;
    do {
        if (_card_clear(id) != 0) {
            switch (busyWaitCardEvent()) {
            case 0:
                return loadCardSave(id);
            case 1:
                return 4;
            default:
                return 3;
            }
        }
        counter++;
    } while (counter < 0xB4);
    markCardBusy(id);
    return 4;
}


/**
 * @brief Check a memory card and clear it if needed.
 *
 * Calls getCardInfo to query the card. If the card is new (result 0),
 * checks card status and clears if present. Returns the final card status.
 *
 * @param cardId Packed card identifier.
 * @return 0 = cleared, 1 = card present but clear failed, other = error.
 */
s32 checkAndClearCard(s32 cardId) {
    s32 result;

    result = getCardInfo(cardId);
    if (result == 0) {
        if (getCardStatus(cardId) != 0) {
            result = 1;
        }
    }
    setCardFlag((s8)result);
    if (result == 1) {
        setCardFlag(-1);
        result = clearCardSync(cardId);
        if (result != 0) {
            markCardBusy(cardId);
        }
    }
    return result;
}


/**
 * @brief Poll a memory card for readiness, with retries.
 *
 * Initializes event handlers, then calls checkAndClearCard in a retry loop.
 * If the result is 2 (timeout), retries up to 3 times. Results in
 * the [2,4] range continue looping; others exit immediately.
 *
 * @param id Packed card identifier.
 * @return Card status result.
 */
s32 pollCardReady(s32 id) {
    s32 retryCount;
    s32 loopLimit;
    s32 twoConst;
    s32 result;

    initCardEventHandlers();
    retryCount = 0;
    loopLimit = 4;
    twoConst = 2;
    do {
        result = checkAndClearCard(id);
        if ((u32)(result - 2) >= 3) {
            break;
        }
        if (result == twoConst) {
            retryCount++;
            loopLimit = 4;
            if (retryCount >= 3) {
                break;
            }
        }
        VSync(0);
        loopLimit--;
    } while (loopLimit > 0);
    setCardFlag((s8)result);
    return result;
}


/**
 * @brief Format a memory card.
 *
 * Polls card readiness first. If the card is ready (status 0 or 2),
 * performs a format operation. On format success, sets secondary status.
 * Always marks the card busy after the format attempt.
 *
 * @param cardId Packed card identifier.
 * @return 0 on success or if card not ready, 1 on poll failure after format.
 */
s32 formatCard(s32 cardId) {
    s32 result;

    result = pollCardReady(cardId);
    if (result != 0 && result != 2) {
        return 0;
    }
    setCardFlag(-2);
    initCardEventHandlers();
    waitCardReady(cardId);
    busyWaitCardEvent();
    VSync(4);
    result = _card_format(cardId);
    if (result == 0) {
        setCardStatusSecondary(cardId, 1);
    }
    markCardBusy(cardId);
    if (result != 0) {
        if (pollCardReady(cardId) == 0) {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief Format a memory card (alternate version).
 *
 * Like formatCard, but returns 1 immediately if the card poll returns 0 (ready).
 * Only proceeds with formatting if poll returns 2 (card needs formatting).
 *
 * @param cardId Packed card identifier.
 * @return 1 if card is already ready, 0 on format success or poll failure.
 */
s32 formatCardAlt(s32 cardId) {
    s32 result;

    result = pollCardReady(cardId);
    if (result == 0) {
        return 1;
    }
    if (result != 2) {
        return 0;
    }
    setCardFlag(-2);
    initCardEventHandlers();
    waitCardReady(cardId);
    busyWaitCardEvent();
    VSync(4);
    result = _card_format(cardId);
    if (result == 0) {
        setCardStatusSecondary(cardId, 1);
    }
    markCardBusy(cardId);
    if (result != 0) {
        if (pollCardReady(cardId) == 0) {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief Build a memory card file path string.
 *
 * Constructs a path like "buXY:filename" where X is the port digit
 * and Y is the slot digit, both as ASCII characters ('0'-based).
 *
 * @param cardId Packed card identifier (port in bit 4, slot in lower bits).
 * @param filename Null-terminated filename to append.
 * @param outBuf Destination buffer for the constructed path string.
 */
void buildCardPath(s32 cardId, char *filename, char *outBuf)
{
    u8 templateBuf[8];
    s32 port;
    s32 slot;

    btlStrcpy(templateBuf, D_800101C4);
    port = getCardPort(cardId);
    slot = getCardSlot(cardId);
    port += '0';
    templateBuf[2] = port;
    templateBuf[3] = slot + '0';
    btlStrcpy(outBuf, templateBuf);
    btlStrcat(outBuf, filename);
}


/** @brief Returns the memory card operation flag value. */
s32 getCardFlagValue(void) {
    return g_cardFlag;
}


/**
 * @brief Open a memory card file by building the path and opening it.
 *
 * Initializes card event handlers, builds the card file path, checks
 * the card status, and opens the file. If the status check fails,
 * polls for card readiness before opening.
 *
 * @param cardId Packed card identifier.
 * @param filename Null-terminated filename.
 * @param flags File open flags.
 * @return File descriptor on success, -1 on failure.
 */
s32 openCardFile(s32 cardId, char *filename, s32 openMode)
{
    char pathBuf[32];

    initCardEventHandlers();
    buildCardPath(cardId, filename, pathBuf);

    if (readCardStatusDiscard(cardId) != 0) {
        if (pollCardReady(cardId) != 0) {
            return -1;
        }
    }

    return open(pathBuf, openMode);
}


/** @brief Wrapper that calls openCardFile. */
s32 callCardInit(s32 cardId, char *filename, s32 openMode)
{
    return openCardFile(cardId, filename, openMode);
}


/**
 * @brief Close a file descriptor if it is valid (not -1).
 * @param fd File descriptor to close.
 */
void closeFileDescriptor(s32 fd) {
    if (fd != -1) {
        close(fd);
    }
}


/** @brief Wrapper for closeFileDescriptor. */
void closeFileDescriptorWrapper(s32 fd) {
    closeFileDescriptor(fd);
}


/**
 * @brief Open the first file on a memory card matching a filename pattern.
 *
 * Initializes card event handlers, polls until the card is ready, builds
 * the full card path ("buXY:filename"), and calls PsyQ firstfile to begin
 * directory enumeration.
 *
 * @param cardId Packed card identifier.
 * @param filename File name pattern to search for.
 * @param dirEntry Directory entry buffer for firstfile result.
 * @return 0 if card not ready, otherwise the result of firstfile.
 */
s32 openCardFirstFile(s32 cardId, char *filename, s32 dirEntry)
{
    char pathBuf[32];

    initCardEventHandlers();
    if (pollCardReady(cardId) != 0) {
        return 0;
    }
    buildCardPath(cardId, filename, pathBuf);
    return firstfile(pathBuf, dirEntry);
}


/**
 * @brief Open the first memory card file, retrying up to 5 times.
 *
 * Calls openCardFirstFile in a loop. If the open fails (returns 0),
 * waits one VSync and retries. Returns 0 if all retries are exhausted.
 *
 * @param cardId Packed card identifier.
 * @param filename Filename parameter.
 * @param dirEntry Directory entry buffer.
 * @return Result of openCardFirstFile (nonzero on success, 0 on failure).
 */
s32 openFirstFileRetry(s32 cardId, char *filename, s32 dirEntry)
{
    s32 retries = 4;
    s32 result;

    do {
        result = openCardFirstFile(cardId, filename, dirEntry);
        if (result != 0) break;
        VSync(0);
        retries--;
    } while (retries >= 0);
    return result;
}


/** @brief Advance to the next file in the memory card directory listing (PsyQ nextfile wrapper). */
s32 cardNextFile(s32 *dirEntry)
{
    return nextfile(dirEntry);
}


/**
 * @brief Sum all file sizes on a memory card, rounded to 8KB sectors.
 *
 * Opens the card directory with a wildcard, then iterates all files,
 * summing each file's size rounded up to the next 8KB boundary.
 *
 * @param cardId Packed card identifier.
 * @return Total size in bytes (8KB-aligned), or 0 if directory open failed.
 */
s32 sumCardFileSizes(s32 cardId)
{
    s32 dirEntry[10];
    s32 total = 0;

    if (openFirstFileRetry(cardId, D_800101CC, dirEntry) != 0) {
        do {
            total += ((dirEntry[6] + 0x1FFF) / 0x2000) * 0x2000;
        } while (cardNextFile(dirEntry) != 0);
    }
    return total;
}


/**
 * @brief Check whether a file exists on the memory card.
 *
 * @param cardId Packed card identifier.
 * @param filename File name to search for.
 * @return 1 if the file exists, 0 otherwise.
 */
s32 checkCardFileExists(s32 cardId, s32 filename) {
    s32 buf[10];
    return openFirstFileRetry(cardId, filename, buf) != 0;
}


/**
 * @brief Create a new file on the memory card.
 *
 * Checks if the file already exists (returns 2 if so), verifies there is
 * enough free space on the card, creates the file with the appropriate
 * sector count, then re-opens it. Returns -1 on failure, 1 on success.
 *
 * @param cardId Packed card identifier.
 * @param filename Filename to create.
 * @param size File size in bytes.
 * @return 1 on success, 2 if file exists, -1 on failure.
 */
s32 createCardFile(s32 cardId, char *filename, s32 size) {
    s32 neg1;
    s32 fd;
    s32 free;

    initCardEventHandlers();
    if (checkCardFileExists(cardId, filename) != 0) {
        return 2;
    }
    size = ((size + CARD_BLOCK_SIZE - 1) / CARD_BLOCK_SIZE) * CARD_BLOCK_SIZE;
    free = sumCardFileSizes(cardId);
    free = CARD_TOTAL_CAPACITY - free;
    if (free < size) {
        return -1;
    }
    size /= CARD_BLOCK_SIZE;
    fd = openCardFile(cardId, filename, (size << 16) | CARD_OPEN_CREATE);
    neg1 = -1;
    if (fd == neg1) {
        markCardBusy(cardId);
        return -1;
    }
    closeFileDescriptor(fd);
    fd = openCardFile(cardId, filename, CARD_OPEN_READWRITE);
    if (fd == neg1) {
        markCardBusy(cardId);
        return -1;
    }
    closeFileDescriptor(fd);
    return 1;
}


/**
 * @brief Seek to an offset in a file and write data.
 * @param fd File descriptor.
 * @param buf Pointer to data buffer to write.
 * @param len Number of bytes to write.
 * @param offset File offset to seek to before writing.
 * @return Number of bytes written on success, -1 if the seek failed.
 */
s32 seekAndWrite(s32 fd, s32 buf, s32 size, s32 offset)
{
    if (lseek(fd, offset, 0) == -1) {
        return -1;
    }
    return write(fd, buf, size);
}


/**
 * @brief Seek to an offset in a file and read data.
 * @param fd File descriptor.
 * @param buf Pointer to destination buffer.
 * @param len Number of bytes to read.
 * @param offset File offset to seek to before reading.
 * @return Number of bytes read on success, -1 if the seek failed.
 */
s32 seekAndRead(s32 fd, s32 buf, s32 len, s32 offset)
{
    if (lseek(fd, offset, 0) == -1) {
        return -1;
    }
    return read(fd, buf, len);
}


/**
 * @brief Write a buffer to a file in 128-byte sector-aligned chunks.
 *
 * Handles three regions: (1) an unaligned head block, read-modify-written
 * into the tmpBuf sector; (2) a run of fully-aligned 128-byte sectors
 * written directly; (3) an unaligned tail block, again read-modify-written.
 *
 * @param fd File descriptor.
 * @param src Source buffer.
 * @param size Number of bytes to write.
 * @param offset Target file offset (arbitrary alignment).
 * @return `size` on success, -1 on any failure.
 */
s32 func_80029850(s32 fd, u8 *src, int size, s32 offset) {
    u8 *srcCopy;
    u8 tmpBuf[128];
    s32 headMisalign;
    s16 neg1Short;
    u8 *bufAlias;
    s32 copyLen;
    s32 fullBlockSize;
    s32 headOffset;
    s32 alignedOffset;
    s32 result;
    s32 origSize;
    u8 *tmpDst;

    if (size == 0) {
        return 0;
    }
    origSize = size;
    headMisalign = offset & 0x7F;
    if (headMisalign != 0) {
        alignedOffset = (offset / 128) * 128;
        result = seekAndRead(fd, (s32)tmpBuf, 128, alignedOffset);
        if (result != 128) {
            return -1;
        }
        copyLen = 128 - headMisalign;
        srcCopy = src;
        headOffset = alignedOffset;
        tmpDst = tmpBuf + headMisalign;
        copyLen = (copyLen < 0) ? 0 : ((copyLen > size) ? size : copyLen);
        btlMemcpy(srcCopy, tmpDst, copyLen);
        result = seekAndWrite(fd, (s32)tmpBuf, 128, headOffset);
        if (result != 128) {
            return -1;
        }
        size -= copyLen;
        offset += copyLen;
        src += copyLen;
        if (size == 0) {
            return origSize;
        }
    }

    headMisalign = size & 0x7F;
    fullBlockSize = size - headMisalign;
    result = seekAndWrite(fd, (s32)src, fullBlockSize, offset);
    if (result == -1) {
        return -1;
    }
    if (result != fullBlockSize) {
        return -1;
    }
    neg1Short = -1;
    size -= result;
    offset += result;
    src += result;

    if (headMisalign == 0) {
        return origSize;
    }
    result = seekAndRead(fd, (s32)tmpBuf, 128, offset);
    if (result == -1) {
        return -1;
    }
    if (result != 128) {
        return -1;
    }
    bufAlias = tmpBuf;
    btlMemcpy(src, tmpBuf, size);
    result = seekAndWrite(fd, (s32)bufAlias, 128, offset);
    if (result == -1) {
        return -1;
    } else if (result != 128) {
        return neg1Short;
    }
    return origSize;
}


/**
 * @brief Read a file into a buffer in 128-byte sector-aligned chunks.
 *
 * Counterpart to func_80029850. Handles three regions: (1) an unaligned
 * head portion (read whole sector, copy relevant bytes); (2) a run of
 * fully-aligned 128-byte sectors read directly into dst; (3) an unaligned
 * tail portion (read whole sector, copy leading bytes).
 *
 * @param fd File descriptor.
 * @param dst Destination buffer.
 * @param size Number of bytes to read.
 * @param offset Source file offset (arbitrary alignment).
 * @return `size` on success, -1 on any failure.
 */
s32 func_80029A20(s32 fd, u8 *dst, int size, s32 offset) {
    u8 *dstCopy;
    u8 tmpBuf[128];
    s32 headMisalign;
    s16 neg1Short;
    s32 copyLen;
    s32 fullBlockSize;
    s32 alignedOffset;
    s32 result;
    s32 origSize;
    u8 *tmpSrc;

    if (size == 0) {
        return 0;
    }
    origSize = size;
    headMisalign = offset & 0x7F;
    if (headMisalign != 0) {
        alignedOffset = (offset / 128) * 128;
        result = seekAndRead(fd, (s32)tmpBuf, 128, alignedOffset);
        if (result != 128) {
            return -1;
        }
        copyLen = 128 - headMisalign;
        tmpSrc = tmpBuf + headMisalign;
        dstCopy = dst;
        copyLen = (copyLen < 0) ? 0 : ((copyLen > size) ? size : copyLen);
        btlMemcpy(tmpSrc, dstCopy, copyLen);
        size -= copyLen;
        offset += copyLen;
        dst += copyLen;
        if (size == 0) {
            return origSize;
        }
    }

    headMisalign = size & 0x7F;
    fullBlockSize = size - headMisalign;
    result = seekAndRead(fd, (s32)dst, fullBlockSize, offset);
    neg1Short = -1;
    if (result == neg1Short) {
        return -1;
    }
    if (result != fullBlockSize) {
        return -1;
    }
    size -= result;
    offset += result;
    dst += result;

    if (headMisalign == 0) {
        return origSize;
    }
    result = seekAndRead(fd, (s32)tmpBuf, 128, offset);
    if (result == neg1Short) {
        return -1;
    }
    if (result == 128) {
        btlMemcpy(tmpBuf, dst, size);
        return origSize;
    }
    return -1;
}


/**
 * @brief Open a card file, write data, and close it.
 *
 * Opens the card file with write access (flag 3), performs a sector-aligned
 * write via func_80029850, then closes the file. Returns -1 if open or
 * write fails, marking the card busy.
 *
 * @param cardId Packed card identifier.
 * @param filename Filename on the card.
 * @param data Data buffer to write.
 * @param size Number of bytes to write.
 * @param offset File offset to write at.
 * @return Bytes written on success, -1 on failure.
 */
s32 writeCardSector(s32 cardId, s32 filename, s32 data, s32 size, s32 offset) {
    s32 fd;
    s32 neg1;
    s32 result;

    fd = openCardFile(cardId, filename, 3);
    neg1 = -1;
    if (fd == neg1) {
        markCardBusy(cardId);
        return neg1;
    }
    result = func_80029850(fd, data, size, offset);
    closeFileDescriptor(fd);
    if (result == neg1) {
        markCardBusy(cardId);
    }
    return result;
}


/**
 * @brief Seek to an offset in a file and write data, returning fd on success.
 * @param fd File descriptor.
 * @param buf Pointer to data buffer to write.
 * @param len Number of bytes to write.
 * @param offset File offset to seek to before writing.
 * @return File descriptor on success, -1 on failure.
 */
s32 seekWriteReturnFd(s32 fd, s32 buf, s32 len, s32 offset) {
    s32 result;
    if (lseek(fd, offset, 0) < 0) {
        return -1;
    }
    result = write(fd, buf, len);
    if (result != 0) {
        return -1;
    }
    return fd;
}


/**
 * @brief Open a card file, write data via seekWriteReturnFd, and handle errors.
 *
 * Opens with flag 0x8003 (write+create), writes via seekWriteReturnFd.
 * If open fails (fd == -1) or write fails (result < 0), marks card busy
 * and returns -1.
 *
 * @param cardId Packed card identifier.
 * @param filename Filename on the card.
 * @param data Data buffer to write.
 * @param size Number of bytes to write.
 * @param offset File offset to write at.
 * @return File descriptor on success, -1 on failure.
 */
s32 writeCardFileSync(s32 cardId, s32 filename, s32 data, s32 size, s32 offset) {
    s32 fd;
    s32 result;

    fd = openCardFile(cardId, filename, 0x8003);
    if (fd == -1) {
        markCardBusy(cardId);
        return -1;
    }
    result = seekWriteReturnFd(fd, data, size, offset);
    if (result >= 0) {
        return fd;
    }
    markCardBusy(cardId);
    return -1;
}


/**
 * @brief Seek to an offset in a file and read data, returning fd on success.
 * @param fd File descriptor.
 * @param buf Pointer to destination buffer.
 * @param len Number of bytes to read.
 * @param offset File offset to seek to before reading.
 * @return File descriptor on success, -1 on failure.
 */
s32 seekReadReturnFd(s32 fd, s32 buf, s32 len, s32 offset) {
    s32 result;
    if (lseek(fd, offset, 0) < 0) {
        return -1;
    }
    result = read(fd, buf, len);
    if (result != 0) {
        return -1;
    }
    return fd;
}


/**
 * @brief Open a card file, read data via seekReadReturnFd, and handle errors.
 *
 * Opens with flag 0x8001 (read), reads via seekReadReturnFd. If open fails
 * or read fails, marks card busy, closes fd, and returns -1.
 *
 * @param cardId Packed card identifier.
 * @param filename Filename on the card.
 * @param data Destination buffer.
 * @param size Number of bytes to read.
 * @param offset File offset to read from.
 * @return File descriptor on success, -1 on failure.
 */
s32 readCardFileSync(s32 cardId, s32 filename, s32 data, s32 size, s32 offset) {
    s32 fd;
    s32 result;

    fd = openCardFile(cardId, filename, 0x8001);
    if (fd < 0) {
        markCardBusy(cardId);
        return -1;
    }
    result = seekReadReturnFd(fd, data, size, offset);
    if (result >= 0) {
        return fd;
    }
    markCardBusy(cardId);
    closeFileDescriptor(fd);
    return fd;
}


/**
 * @brief Open a card file with create flag, read data, close, and handle errors.
 *
 * Opens with flag 1 (create/read), reads data via func_80029A20,
 * then closes the file. Returns -1 if open or read fails.
 *
 * @param cardId Packed card identifier.
 * @param filename Filename on the card.
 * @param data Destination buffer.
 * @param size Number of bytes to read.
 * @param offset File offset to read from.
 * @return Bytes read on success, -1 on failure.
 */
s32 readCardCreate(s32 cardId, s32 filename, s32 data, s32 size, s32 offset) {
    s32 fd;
    s32 neg1;
    s32 result;

    fd = openCardFile(cardId, filename, 1);
    neg1 = -1;
    if (fd == neg1) {
        markCardBusy(cardId);
        return neg1;
    }
    result = func_80029A20(fd, data, size, offset);
    closeFileDescriptor(fd);
    if (result == neg1) {
        markCardBusy(cardId);
    }
    return result;
}


/**
 * @brief Erase a memory card file.
 *
 * Sets up the card subsystem, checks if the file exists via checkCardFileExists,
 * builds the filename via buildCardPath, and calls erase. If erase succeeds
 * (returns non-zero), returns 1. Otherwise calls cleanup and returns -1.
 *
 * @param cardId Packed card identifier.
 * @param filename File name parameter.
 * @return 1 if erase succeeded, -1 on failure.
 */
s32 eraseCardFile(s32 cardId, s32 filename) {
    s32 buf[8];
    initCardEventHandlers();
    if (checkCardFileExists(cardId, filename) == 0) {
        return -1;
    }
    buildCardPath(cardId, filename, (s32)buf);
    if (erase((s32)buf) != 0) {
        return 1;
    }
    markCardBusy(cardId);
    return -1;
}


/**
 * @brief Write a single 128-byte block to a memory card with retry.
 *
 * Retries _card_write in a loop. On success, waits for the card
 * event and returns whether it completed (1) or failed (0).
 *
 * @param cardId Packed card identifier.
 * @param buf Data buffer to write.
 * @param sector Sector number to write to.
 * @return 1 on success (event completed), 0 on failure or timeout.
 */
s32 writeCardBlock(s32 cardId, s32 buf, s32 sector) {
    s32 retries = 0xB4;

top:
    waitCardReady(cardId);
    if (_card_write(cardId, sector, buf) == 0) {
        retries++;
        if (retries > 0) goto top;
        return 0;
    }
    return busyWaitCardEvent() == 0;
}


/**
 * @brief Write multiple 128-byte blocks to a memory card.
 *
 * Initializes card event handlers, clamps the sector range to [0, 1024],
 * then writes each block sequentially. Returns the number of blocks
 * successfully written, stopping on first failure.
 *
 * @param cardId Packed card identifier.
 * @param buf Data buffer (increments by 128 per block).
 * @param startSector Starting sector number.
 * @param endSector Ending sector count (added to startSector for range).
 * @return Number of blocks successfully written.
 */
s32 writeCardBlocks(s32 cardId, s32 buf, s32 startSector, s32 endSector) {
    s32 limit;
    s32 clampedLimit;
    s32 count;

    initCardEventHandlers();
    limit = endSector + startSector;
    if (limit >= 0) {
        clampedLimit = 0x400;
        if (limit < 0x401) {
            clampedLimit = limit;
        }
    } else {
        clampedLimit = 0;
    }
    limit = clampedLimit;
    count = 0;
    while (endSector < limit) {
        if (writeCardBlock(cardId, buf, endSector) == 0) {
            return count;
        }
        count++;
        endSector++;
        buf += 0x80;
    }
    return count;
}


/**
 * @brief Shut down the memory card subsystem by closing all 8 card events.
 * @note Disables interrupts via func_800472E4, closes all events in g_cardData[0..7],
 *       re-enables interrupts, then calls func_8004D968 for final cleanup.
 */
void shutdownCardSubsystem(void) {
    func_800472E4();
    CloseEvent(g_cardData.events[0]);
    CloseEvent(g_cardData.events[1]);
    CloseEvent(g_cardData.events[2]);
    CloseEvent(g_cardData.events[3]);
    CloseEvent(g_cardData.events[4]);
    CloseEvent(g_cardData.events[5]);
    CloseEvent(g_cardData.events[6]);
    CloseEvent(g_cardData.events[7]);
    func_800472F4();
    func_8004D968();
}


/** @brief Calls func_80027448 and cdDisableInterruptWrapper in sequence. */
void initBattleSubsystems(void) {
    func_80027448();
    cdDisableInterruptWrapper();
}


/**
 * @brief Encode an ASCII filename into FF8's internal character encoding.
 *
 * Converts up to 8 ASCII characters (digits 0-9, letters A-Z/a-z) into
 * FF8 font indices using the menu string table at index 0x0B. Letters
 * are case-insensitive (uppercased via & 0xDF). Non-alphanumeric chars
 * are skipped. Sets the global card file slot and type.
 *
 * @param str ASCII filename (NULL = mark inactive and return).
 * @param slot Save slot index.
 * @param type Card/save type.
 */
void encodeCardFilename(u8 *str, s16 slot, s8 type) {
    s8 *out = g_cardFilename;
    s32 count = 0;
    s32 ch;
    s32 digit;
    int base;

    g_cardFileSlot = slot;
    g_cardFileType = type;

    if (str == NULL) {
        g_cardFileActive = 0;
        return;
    }

    g_cardFileActive = 1;
    /* Regalloc: out++/-- boosts out to s1 so str stays in s3 */
    out++;
    out--;

top:
    ch = *str++;
    if (ch == 0) goto done;
    if (count >= 8) goto done;

    digit = ch - '0';
    if ((u32)digit < 10) {
        ch = digit;
        base = ((u8 *)getMenuString(11))[1];
        count++;
        goto store;
    }

    ch = ch & 0xDF;
    ch = ch - 'A';
    if ((u32)ch >= 26) goto top;

    base = ((u8 *)getMenuString(11))[11];
    count++;

store:
    ch += base;
    *out++ = ch;
    goto top;

done:
    *out = 0;
}


/**
 * @brief Apply GPU draw area/offset setup if the card file overlay is active.
 *
 * When g_cardFileActive is set, calls func_8002E8DC to process the overlay
 * data, then emitDrawEnvPackets to emit draw area/offset packets into the OT.
 * Returns the packet pointer unchanged if inactive.
 *
 * @param ot   Ordering table entry pointer.
 * @param pkt  Current GPU packet pointer.
 * @return Updated packet pointer, or original if inactive.
 */
s32 transformValueIfActive(s32 ot, s32 pkt) {
    if (g_cardFileActive != 0) {
        s32 result = func_8002E8DC(ot, pkt, g_cardFileSlot, g_cardFileType, (u8 *)g_cardFilename, 7);
        pkt = emitDrawEnvPackets(ot, result);
    }
    return pkt;
}


/**
 * @brief Copy a null-terminated string from src to dst (strcpy implementation, duplicate).
 * @param dst Destination buffer.
 * @param src Source null-terminated string.
 * @note This is a second copy of the strcpy function (see also btlStrcpy).
 */
void copyString(u8 *dst, u8 *src) {
    s32 c;
    do {
        c = *src++;
        *dst++ = c;
    } while (c != 0);
}


/**
 * @brief Append a null-terminated string to dst (strcat implementation, duplicate).
 * @param dst Destination buffer containing an existing null-terminated string.
 * @param src Source null-terminated string to append.
 * @note This is a second copy of the strcat function (see also btlStrcat).
 */
void btlStrcat2(u8 *dst, u8 *src) {
    do { } while (*dst++);
    copyString(dst - 1, src);
}


/**
 * @brief Return the length of a null-terminated byte string.
 * @param str Pointer to null-terminated string.
 * @return Number of bytes before the null terminator.
 */
s32 btlStrlen(u8 *str) {
    s32 count = 0;
    goto test;
inc:
    count++;
test:
    if (*str++ != 0) goto inc;
    return count;
}


/**
 * @brief Copy n bytes from source to destination (forward copy).
 * @param src Source address.
 * @param dst Destination address.
 * @param n Number of bytes to copy.
 */
void btlMemcpyForward(u8 *src, u8 *dst, s32 n) {
    while (n > 0) {
        *dst++ = *src++;
        n--;
    }
}


/**
 * @brief Copy n bytes from source to destination (backward/reverse copy).
 * @param src Source address.
 * @param dst Destination address.
 * @param n Number of bytes to copy.
 */
void btlMemcpyBackward(u8 *src, u8 *dst, s32 n) {
    src += n;
    dst += n;
    while (n > 0) {
        dst--;
        src--;
        *dst = *src;
        n--;
    }
}


/**
 * @brief Compare two byte arrays for equality.
 *
 * @param a Pointer to first byte array.
 * @param b Pointer to second byte array.
 * @param count Number of bytes to compare.
 * @return 1 if all bytes match (or count <= 0), 0 on first mismatch.
 */
s32 compareBytes(u8 *a, u8 *b, s32 count) {
    s32 result = 1;
    while (count > 0) {
        u8 bVal = *b++;
        if (*a++ != bVal) {
            return 0;
        }
        count--;
    }
    return result;
}


/** @brief Wrapper for getCharNamePtr. */
void getCharNamePtrWrapper(void) { getCharNamePtr(); }


/** @brief Wrapper for getCharNamePtr (duplicate of getCharNamePtrWrapper). */
void getCharNamePtrWrapper2(void) { getCharNamePtr(); }


/** @brief Wrapper for getBattleCharName. */
void getBattleCharNameWrapper(void) { getBattleCharName(); }


/**
 * @brief Copy the clip rectangle from the active draw environment.
 *
 * @param dst Destination RECT.
 */
void copyDisplayRect(RECT *dst) {
    *dst = g_activeDrawEnv->clip;
}


/**
 * @brief Copy the draw offset from the active draw environment.
 * @param dst Destination vector for the display coordinates.
 */
void copyDisplayCoords(DVECTOR *dst) {
    DRAWENV *env = g_activeDrawEnv;
    dst->vx = env->dispX;
    dst->vy = env->dispY;
}


/**
 * @brief Set up GPU draw area and offset packets, linking them to an OT entry.
 *
 * Copies the current display rectangle, emits a SetDrawArea and
 * SetDrawOffset packet, and links each into the ordering table via addPrim.
 *
 * @param ot Ordering table entry pointer.
 * @param pkt GPU packet allocation pointer.
 * @return Updated packet pointer (pkt + 24 bytes: two 12-byte packets).
 */
u8* emitDrawEnvPackets(u32* ot, u8* pkt) {
    RECT rect;

    copyDisplayRect(&rect);
    SetDrawArea(pkt, &rect);
    addPrim(ot, pkt);
    pkt += 0xC;

    SetDrawOffset(pkt, &rect);
    addPrim(ot, pkt);

    pkt += 0xC;

    return pkt;
}


/**
 * @brief Swap the active display list buffer and clear the new OT.
 *
 * Toggles between two display list buffers at g_battleAnims+0x640 and
 * g_battleAnims+0x698. Stores the new active buffer at +0x6F0, clears
 * its ordering table (18 entries), and copies the GPU packet pointer
 * from offset +0x54 to offset +0x00.
 */
void swapDisplayList(void) {
    BattleAnimState *base = &g_battleAnims;
    DisplayListBuf *buf;
    DisplayListBuf *active;

    buf = &base->bufs[0];
    if (base->active == buf) {
        buf = &base->bufs[1];
    }

    base->active = buf;
    ClearOTag(buf->ot, OT_SIZE);
    active = base->active;
    active->pktAlloc = active->pktBase;
}

/**
 * @brief Swap the active display list buffer and clear the new OT (duplicate).
 *
 * Identical logic to swapDisplayList.
 */
void swapDisplayList2(void) {
    BattleAnimState *base = &g_battleAnims;
    DisplayListBuf *buf;
    DisplayListBuf *active;

    buf = &base->bufs[0];
    if (base->active == buf) {
        buf = &base->bufs[1];
    }

    base->active = buf;
    ClearOTag(buf->ot, OT_SIZE);
    active = base->active;
    active->pktAlloc = active->pktBase;
}


/**
 * @brief Process battle animation frames and dispatch to entities.
 *
 * In mode 1 (per-frame), builds separate frame/status data for each frame
 * by remapping party bitmasks, resolving coordinates, and collecting status
 * flags. In mode 0 (single-frame), computes one frame's data and replicates
 * it across all frames. Then iterates through frames in reverse, dispatching
 * each to all active battle entities.
 *
 * @param frameCount Number of animation frames to process.
 * @param mode 1 for per-frame processing, 0 for single-frame broadcast.
 */
void processBattleAnimFrames(s32 frameCount, s32 mode) {
    s32 frameData[8];
    s32 statusData[8];
    s32 count = frameCount - 1;
    s32 statusVal;
    s32 val;
    s32 i;
    s32 j;
    s32 param;
    s32 frameVal;
    s32 upperBits;

    if (mode == 1) {
        func_800472E4();
        for (i = count; i >= 0; i--) {
            param = remapControllerInput(getAnimFrameParam(0, i) & 0xFFFF) & 0xFFFF;
            if ((param & 0xF000) == 0) {
                val = func_80027DB4(0, 2, i);
                if (val >= 0) {
                    param |= func_80027CF8(0, val - 128, func_80027DB4(0, 3, i) - 128);
                }
            }
            param |= remapControllerInput(func_80027A58(0, i) & 0xFFFF) << 16;
            j = i;
            frameData[j] = param;
            statusData[j] = remapControllerInput(getAnimFrameStatusFlags(0, j) & 0xFFFF) & 0xFFFF;
        }
        func_800472F4();
    } else {
        func_800472E4();
        param = remapControllerInput(getAnimFrameParam(0, 0) & 0xFFFF) & 0xFFFF;
        upperBits = remapControllerInput(func_80027A58(0, 0) & 0xFFFF) << 16;
        val = func_80027DB4((0, 0), 2, 0);
        if (((param & 0xF000) == 0) && (val >= 0)) {
            param |= func_80027CF8(0, val - 128, func_80027DB4(0, 3, 0) - 128);
        }
        func_800472F4();
        i = count;
        for (; i >= 0; i--) {
            if (1) {
                if (i == 0) {
                    frameData[0] = param | upperBits;
                } else {
                    frameData[i] = param;
                }
                statusData[i] = func_8002CF54(param);
            }
        }
    }

    while (count >= 0) {
        val = statusData[count];
        param = frameData[count];
        frameVal = param;
        statusVal = val;
        decrementSfxCounter();
        for (j = 0; j < 8; j++) {
            if (GetActiveFlag(j)) {
                dispatchBattleEntity(j, frameVal, statusVal);
            }
        }
        updateCameraVibrate();
        updatePaletteTransition();
        stepAnimEntries();
        count--;
    }
}


/**
 * @brief Process battle animation frames and advance the battle timer.
 * @param frameCount Number of frames to process.
 */
void renderAndUpdateDisplay(s32 frameCount) {
    processBattleAnimFrames(frameCount, 0);
    advanceBattleTimer(frameCount);
}


/**
 * @brief Process battle animation frames in render-only mode.
 * @param frameCount Number of frames to process.
 */
void renderDisplay(s32 frameCount) {
    processBattleAnimFrames(frameCount, 1);
}


/**
 * @brief Read the current packet allocation pointer from the active display list buffer.
 * @return The pktAlloc field of the active display list buffer.
 */
s32 getDisplayListHead(void) {
    return g_battleAnims.active->pktAlloc;
}


/**
 * @brief Read the base packet pointer from the active display list buffer.
 * @return The pktBase field of the active display list buffer.
 */
s32 getDisplayListPacketPtr(void) {
    return g_battleAnims.active->pktBase;
}


/**
 * @brief Store a GPU packet pointer and check for OT overflow.
 *
 * Writes @p pkt to the active display list buffer's current position,
 * then checks if the pointer exceeds the buffer's limit. If it overflows
 * (and is within the valid address range <= 0x801AFFFF), prints an error
 * with the overflow amount.
 *
 * @param pkt GPU packet pointer to store.
 */
void storeGpuPacket(u32 pkt) {
    DisplayListBuf *buf;
    u32 limit;

    buf = g_battleAnims.active;
    buf->pktAlloc = pkt;
    limit = g_battleAnims.active->pktLimit;

    if (limit < pkt) {
        if (pkt <= 0x801AFFFFU) {
            printf("WARNING:MesCon required more memory.:%d\n", pkt - limit);
        }
    }
}


/** @brief Returns the ordering table of the active display list buffer. */
s32 getDisplayListOtBase(void) {
    return (s32)g_battleAnims.active->ot;
}


/**
 * @brief Render battle display list and link into ordering table.
 *
 * Swaps the display list, runs the full rendering chain (color, entity,
 * overlay, transform), then links the result into OT entry 17 via P_TAG
 * address swapping. Uses GP scratchpad for fast buffer access.
 *
 * @param colorTag Pointer to the color primitive's P_TAG word.
 * @return Scratchpad buffer pointer.
 */
s32 renderBattleDisplayList(s32 *colorTag) {
    DisplayListBuf *buf;
    u32 *ot;
    s32 head;
    s32 savedGp;
    s32 result;

    GP_SAVE_SCRATCH(savedGp);

    swapDisplayList();
    buf = g_battleAnims.active;
    head = getDisplayListHead();
    head = func_800302DC((s32)&buf->ot[1], head);
    head = func_80031364((s32)&buf->ot[14], head);
    head = transformValueIfActive((s32)&buf->ot[13], head);
    head = renderAnimOverlay((s32)&buf->ot[13], head);
    ot = buf->ot;
    head = func_8002BF24((s32)ot, head);
    storeGpuPacket(head + sizeof(buf->ot));

    setaddr(&ot[17], getaddr(colorTag));
    setaddr(colorTag, (s32)ot);

    GP_RESTORE_RET(savedGp, result);
    return result;
}


/**
 * @brief Link a GPU primitive into the ordering table and return scratchpad pointer.
 *
 * Redirects GP to scratchpad RAM, builds a display list packet, then links
 * the primitive into the last OT entry (ot[17]) using P_TAG address swapping.
 *
 * @param prim Pointer to the primitive's P_TAG word.
 * @return Scratchpad buffer pointer.
 */
s32 addPrimitive(s32 *prim) {
    u32 *ot;
    s32 head;
    s32 savedGp;
    s32 result;

    GP_SAVE_SCRATCH(savedGp);

    ot = g_battleAnims.active->ot;
    head = getDisplayListHead();
    head = func_8002BF24((s32)ot, head);
    storeGpuPacket(head);

    setaddr(&ot[17], getaddr(prim));
    setaddr(prim, (s32)ot);

    GP_RESTORE_RET(savedGp, result);
    return result;
}


/**
 * @brief Generate animation easing curve lookup tables.
 *
 * Builds g_animCurveFadeOut as an exponential decay curve (each step
 * is 9/10 of the previous, scaled to 0–64) and g_animCurveFadeIn as
 * its reverse. Used by battle transition interpolation to ease
 * coordinates between start and end positions.
 */
void buildAnimEasingCurves(void)
{
    u8 *src = g_animCurveFadeOut;
    u8 *dst;
    s32 val, i;

    val = 0x1000;
    src = g_animCurveFadeOut + 64;
    *src = 64;
    for (i = 0; i < 64; i++) {
        src--;
        *src = val / 64;
        val = val * 9 / 10;
    }

    src = g_animCurveFadeOut;
    dst = g_animCurveFadeIn;
    src += 65;
    for (i = 0; i < 64; i++) {
        src--;
        *dst = *src;
        dst++;
    }
}



/**
 * @brief Initialize the battle display system.
 *
 * Sets up display list buffers, computes half-size offsets for double
 * buffering, then initializes all battle subsystems: entities, SFX,
 * GPU colors, camera, command entries, transitions, and animation entries.
 *
 * @param vramBase Display buffer base address in VRAM.
 * @param vramSize Display buffer total size (halved internally for double buffering).
 */
void initBattleAnimSystem(s32 vramBase, s32 vramSize)
{
    s32 i;
    s32 half = vramSize / 2;
    s32 vramEnd = vramBase + half;

    g_battleAnims.bufs[0].pktBase = vramBase;
    g_battleAnims.bufs[1].pktBase = vramEnd;
    g_battleAnims.halfSize = half;

    for (i = 0; i < 2; i++) {
        g_battleAnims.bufs[i].pktLimit = g_battleAnims.bufs[i].pktBase + half - 0x800;
    }

    g_battleAnims.active = &g_battleAnims.bufs[1];
    swapDisplayList();
    initAllBattleEntities();
    resetAllSfx();
    setDefaultGpuColor();
    buildGrayscaleGpuColor(0x1000);
    setMenuColorIntensity(0x1000);
    btlColorStub0234();
    buildAnimEasingCurves();
    resetBattleCameraState();
    initBattleCmdEntries();
    setAnimEntityOpacity(0, 0);
    setAnimEntityOpacity(1, 0);
    btlColorStub1044();
    initBattleTransition();
    clearAnimEntries();
    setDigitBaseCode(((u8 *)getMenuString(0xB))[1]);
    g_battleAnims.pad980[6] = 0;
    g_cardFileActive = 0;
}



/**
 * @brief Get a pointer to a battle entity by index.
 * @param idx Entity index.
 * @return Pointer to the entity.
 */
BattleDisplayEntity *getBattleEntity(s32 idx) {
    return &g_battleEntities[idx];
}

/**
 * @brief Set a battle entity's animation speed, clamped to [3, 11].
 * @param idx Entity index.
 * @param val Value to set; clamped to minimum 3 and maximum 11.
 */
void setBattleEntityAnimSpeed(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    s32 v;
    if (val >= 3) {
        if (val < 12) {
            v = val;
        } else {
            v = 11;
        }
    } else {
        v = 3;
    }
    entity->animSpeed = v;
}


/**
 * @brief Get a battle entity's animation speed.
 * @param idx Entity index.
 * @return Animation speed value for the entity.
 */
s32 getBattleEntityAnimSpeed(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->animSpeed;
}


/**
 * @brief Store a byte value into a battle entity's subFields array.
 * @param idx Entity index.
 * @param offset Index into the subFields array (0 or 1).
 * @param val Byte value to store.
 */
void setBattleEntitySubField(s32 idx, s32 offset, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->subFields[offset] = val;
}


/**
 * @brief Get a byte from a battle entity's subFields array.
 *
 * Dead code — never called anywhere in the binary. The compiler shared the
 * g_battleEntities base address (v0) from the preceding setBattleEntitySubField
 * via cross-function register reuse, producing only 4 instructions. This
 * optimization cannot be reproduced from natural struct access, so pointer
 * math with `register` redeclaration is used to match.
 *
 * Original code:
 * @code
 * u8 getBattleEntitySubField(s32 idx, s32 offset) {
 *     BattleDisplayEntity *entity = &g_battleEntities[idx];
 *     return entity->subFields[offset];
 * }
 * @endcode
 *
 * @param idx Entity index (arrives pre-computed as entity pointer in v0).
 * @param offset Index into the subFields array.
 * @return Byte value at the given subField offset.
 */
u8 getBattleEntitySubField(s32 idx, s32 offset) {
    register idx;
    return *((u8 *)idx + offset + 0x3A);
}


/**
 * @brief Set a battle entity's bounding rectangle.
 * @param idx Entity index.
 * @param src Source RECT to copy.
 */
void setBattleEntityBoundRect(s32 idx, RECT *src) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->boundRect = *src;
}


/**
 * @brief Set a battle entity's display rectangle with minimum size clamping.
 *
 * Copies src RECT into the entity's dispRect, then ensures the height
 * is at least 1 and the width is at least 2.
 *
 * @param idx Entity index.
 * @param src Source RECT to copy.
 */
void setBattleEntityRectClamp(s32 idx, RECT *src) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    BattleDisplayEntity *ent2;
    ent2 = entity;
    ent2->dispRect = *src;
    if (ent2->dispRect.h <= 0) {
        entity->dispRect.h = 1;
    }
    if (entity->dispRect.w < 2) {
        ent2->dispRect.w = 2;
    }
}


/**
 * @brief Get a battle entity's bounding rectangle.
 * @param idx Entity index.
 * @param dst Destination RECT to copy into.
 */
void getBattleEntityBoundRect(s32 idx, RECT *dst) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    *dst = entity->boundRect;
}


/**
 * @brief Get a battle entity's display rectangle.
 * @param idx Entity index.
 * @param dst Destination RECT to copy into.
 */
void getBattleEntityDispRect(s32 idx, RECT *dst) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    *dst = entity->dispRect;
}


/**
 * @brief Get a battle entity's entity type.
 * @param idx Entity index.
 * @return Entity type value.
 */
s32 getBattleEntityType(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->entityType;
}
