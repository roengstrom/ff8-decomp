#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"


void callCdTick(void);
void shutdownCardSubsystem(void);
void initBattleSubsystems(void);
s32 func_80047384(void);
void func_800472E4(void);
void func_800472F4(void);

extern u8 g_animInitialized;
extern u8 D_80082FB3;
extern s8 D_80082FD4;
extern u16 D_80083794;
extern s32 D_800834C0;
extern s16 D_800837AC;
extern u8 D_800837A0[];
extern u8 D_800837AE;
extern u8 D_80052958;
extern u8 D_800101D0[];
extern u8 *D_8005F134;
extern BattleDisplayEntity g_battleEntities[];

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
 *  Computes the entry address as g_battleAnims + (a0 & 1) * 196.
 *  Conditionally stores a1 to field 7 and a2 to field 6 (only if >= 0).
 *  Always stores 1 to field 0xA.
 *
 * @param a0 Entity index (masked to 0 or 1).
 * @param a1 Value for field 7 (-1 to skip).
 * @param a2 Value for field 6 (-1 to skip).
 */
void setAnimEntityParams(s32 idx, s32 a1, s32 a2) {
    BattleAnimEntity *entry = &g_battleAnims.entities[idx & 1];
    if (a1 >= 0) {
        entry->field07 = a1;
    }
    if (a2 >= 0) {
        entry->field06 = a2;
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
 * @return 1 if active (frame type >= 0), 0 if inactive.
 */
s32 areAllAnimsInactive(void) {
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
 * Copies the byte at g_battleAnims+0x1E0 into the entity's color fields
 * at offsets +0x0C through +0x0F (RGBX), then calls resetAnimEntity to
 * perform further entity initialization.
 *
 * @param a0 Entity index (stride 196 into g_battleAnims).
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


// get D_80083794 (u16)

/** @brief Initializes g_animInitialized to 0, calls func_80039764(0), then loops twice calling resetAnimEntity(i, 0). */
void resetAnimState(void) {
    s32 i;
    g_animInitialized = 0;
    func_80039764(0);
    for (i = 0; i < 2; i++) {
        resetAnimEntity(i, 0);
    }
}


/** @brief Get the current value of D_80083794 (global u16 state variable). */
u16 getAnimGlobalState(void) {
    return D_80083794;
}



/**
 * @brief Set D_80083794 to a new value.
 * @param a0 Value to store.
 * @return The value that was set.
 */
s32 setAnimGlobalState(s32 a0) {
    D_80083794 = a0;
    return a0;
}


/**
 * @brief Set the global flag D_80082FB3.
 * @param a0 Value to store in D_80082FB3.
 */
void setAnimFlag(s32 a0) {
    D_80082FB3 = a0;
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
void func_80028444(void) {
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


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_800284BC);


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80028564);


/** @brief Wrapper that calls func_80050BC4. */
void callSfxInit(void) {
    func_80050BC4();
}


/** @brief Wrapper that calls func_8004E720. */
void callSfxUpdate(void) {
    func_8004E720();
}


/** @brief Wrapper that calls func_8004DFF4 (likely a CD subsystem tick or finalization). */
void callCdTick(void) { func_8004DFF4(); }


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
 * @param a0 Unused parameter.
 */
void waitVSync(s32 a0) { VSync(); }


/**
 * @brief Set the global flag D_80082FD4.
 * @param val Value to store in D_80082FD4.
 */
void setCardFlag(s8 val) {
    D_80082FD4 = val;
}


/**
 * @brief Encode a memory card port and slot into a single byte.
 * @param a0 Memory card port number (reduced mod 2: 0 or 1).
 * @param a1 Memory card slot number (reduced mod 4: 0-3).
 * @return Packed value: (port << 4) | slot.
 */
s32 packCardId(s32 a0, s32 a1) {
    a1 %= 4;
    a0 %= 2;
    return (a0 << 4) | a1;
}


/**
 * @brief Extract the memory card port number from a packed card identifier.
 * @param a0 Packed card identifier (port in bit 4).
 * @return Port number (0 or 1).
 */
s32 getCardPort(s32 a0) {
    return (a0 >> 4) % 2;
}


/**
 * @brief Extract the memory card slot number from a packed card identifier.
 * @param a0 Packed card identifier (slot in lower 2 bits).
 * @return Slot number (0-3).
 */
s32 getCardSlot(s32 a0) {
    return a0 % 4;
}


/**
 * @brief Wait for a memory card operation to complete on the specified port.
 * @param a0 Packed card identifier; port is extracted via getCardPort.
 */
void waitCardReady(s32 a0) {
    _card_wait(getCardPort(a0));
}


/**
 * @brief Test a BIOS event (wrapper for PsyQ TestEvent).
 * @param a0 Event descriptor to test.
 * @return Nonzero if the event has been delivered.
 */
s32 testCardEvent(s32 a0) { TestEvent(a0); }


/**
 * @brief Poll the first 4 memory card events and return which one fired.
 * @return Event index 0-3 if an event was delivered, or -1 if none fired.
 * @note Tests events from D_80082FB4 array entries [0..3] in order.
 */
s32 pollCardEvents(void) {
    extern u8 D_80082FB4[];
    s32 *s0 = (s32 *)((s32)D_80082FB4);
    if (testCardEvent(s0[0])) return 0;
    if (testCardEvent(s0[1])) return 1;
    if (testCardEvent(s0[2])) return 2;
    if (testCardEvent(s0[3])) return 3;
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


/** @brief Poll memory card events once (discards result). */
void pollCardEventsDiscard(void) { pollCardEvents(); }


/**
 * @brief Poll the second set of 4 memory card events and return which one fired.
 * @return Event index 0-3 if an event was delivered, or -1 if none fired.
 * @note Tests events from D_80082FB4 array entries [4..7] in order.
 */
s32 pollCardEventsSecondary(void) {
    extern u8 D_80082FB4[];
    s32 *s0 = (s32 *)((s32)D_80082FB4);
    if (TestEvent(s0[4])) return 0;
    if (TestEvent(s0[5])) return 1;
    if (TestEvent(s0[6])) return 2;
    if (TestEvent(s0[7])) return 3;
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
 * @param a0 Packed card identifier (port in bit 4, slot in lower bits).
 * @return Status byte from D_80082FB4 at offset 0x2C + port*4 + slot.
 */
u8 getCardStatus(a0)

s32 a0;
{
    extern u8 D_80082FB4[];
    s32 base = (s32)D_80082FB4; /* (s32) cast prevents symbol+constant folding */
    s32 port = getCardPort(a0);
    s32 r2 = getCardSlot(a0);
    return *(u8 *)(port * 4 + base + r2 + 0x2C);
}


/**
 * @brief Write a memory card status byte for a given port and slot.
 * @param a0 Packed card identifier (port in bit 4, slot in lower bits).
 * @param a1 Status value to write.
 */
void setCardStatus(s32 a0, u8 a1) {
    extern u8 D_80082FB4[];
    s32 base = (s32)D_80082FB4; /* (s32) cast prevents symbol+constant folding */
    s32 port = getCardPort(a0);
    s32 r2 = getCardSlot(a0);
    *(u8 *)(port * 4 + base + r2 + 0x2C) = a1;
}


/**
 * @brief Write a secondary memory card status byte for a given port and slot.
 * @param a0 Packed card identifier (port in bit 4, slot in lower bits).
 * @param a1 Status value to write at offset 0x34 (secondary status table).
 */
void setCardStatusSecondary(s32 a0, u8 a1) {
    extern u8 D_80082FB4[];
    s32 base = (s32)D_80082FB4; /* (s32) cast prevents symbol+constant folding */
    s32 port = getCardPort(a0);
    s32 r2 = getCardSlot(a0);
    *(u8 *)(port * 4 + base + r2 + 0x34) = a1;
}


/** @brief Mark a memory card slot as busy/active (set status byte to 1). */
void markCardBusy(s32 a0) { setCardStatus(a0, 1); }


/** @brief Read memory card status (wrapper, discards result). */
void readCardStatusDiscard(void) { getCardStatus(); }


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80028B98);


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
 * @brief Initialize the memory card data block at D_80082FB4.
 *
 * Clears byte at +0x21, fills 2 sets of 4 consecutive bytes starting at
 * +0x27 (then +0x2B) with the value 2, then calls initCardEventHandlers.
 */
/**
 * @brief Initialize SFX entry command bytes and reset state.
 *
 * Clears the status byte at D_80082FB4+0x21 to zero, then fills
 * 8 bytes (2 groups of 4) starting at D_80082FB4+0x24 with value 2.
 * Finally calls initCardEventHandlers to complete initialization.
 */
/**
 * @brief Initialize card data slots and reset card state.
 *
 * Clears the status byte at D_80082FB4+0x21 to zero, fills 8 bytes
 * (2 groups of 4) starting at D_80082FB4+0x24 with value 2, then
 * calls initCardEventHandlers to complete initialization.
 */
void initCardDataBlock(void) {
    extern u8 D_80082FB4[];
    register u8 *base asm("$2") = D_80082FB4;
    s32 i;
    s32 val;
    u8 *ptr;
    s32 j;
    u8 *p;

    REGALLOC_BARRIER(base);
    i = 0;
    val = 2;
    base[0x21] = 0;
    ptr = base;
    top:
    j = 3;
    p = ptr + 0x27;
    inner:
    *p = val;
    j--;
    p--;
    if (j >= 0) goto inner;
    i++;
    ptr += 4;
    if (i < 2) goto top;
    initCardEventHandlers();
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80028D80);


/**
 * @brief Attempt to load a memory card save, retrying up to 180 times.
 * @param a0 Packed card identifier.
 * @return 0 on success, 2 if new card detected, 3 on timeout, 4 on other failure.
 * @note Waits for the card, issues _card_load, then polls events. Updates card status
 *       bytes based on the result.
 */
s32 loadCardSave(s32 a0) {
    s32 counter;

    counter = 0;
    do {
        waitCardReady(a0);
        if (_card_load(a0) != 0) {
            s32 result = waitCardEvent();
            switch (result) {
            case 0:
                setCardStatusSecondary(a0, 0);
                setCardStatus(a0, 0);
                return 0;
            case 3:
                setCardStatusSecondary(a0, 1);
                markCardBusy(a0);
                return 2;
            case 2:
                markCardBusy(a0);
                return 3;
            default:
                markCardBusy(a0);
                return 4;
            }
        }
        counter++;
    } while (counter < 0xB4);
    return 4;
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80029028);


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_800290C0);


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80029150);


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_800291FC);


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_800292AC);


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80029360);


/** @brief Returns the signed byte value at D_80082FD4. */
s32 getCardFlagValue(void) {
    return D_80082FD4;
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80029400);


/** @brief Wrapper that calls func_80029400. */
void callCardInit(void) {
    func_80029400();
}


/**
 * @brief Close a file descriptor if it is valid (not -1).
 * @param a0 File descriptor to close.
 */
void closeFileDescriptor(s32 a0) {
    if (a0 != -1) {
        close(a0);
    }
}


/** @brief Wrapper for closeFileDescriptor (close a file descriptor if valid). */
void closeFileDescriptorWrapper(s32 a0) {
    closeFileDescriptor(a0);
}


/**
 * @brief Open the first file on a memory card matching the given parameters.
 * @param a0 Packed card identifier.
 * @param a1 File index or name parameter.
 * @param a2 Directory entry buffer for firstfile result.
 * @return 0 if card initialization failed, otherwise the result of firstfile.
 * @note Calls initCardEventHandlers for setup, checks card status via func_80029150, builds
 *       the filename via func_80029360, then calls PsyQ firstfile.
 */
s32 openCardFirstFile(s32 a0, s32 a1, s32 a2) {
    s32 buf[8];
    initCardEventHandlers();
    if (func_80029150(a0) != 0) return 0;
    func_80029360(a0, a1, (s32)buf);
    firstfile((s32)buf, a2);
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80029550);


/** @brief Advance to the next file in the memory card directory listing (PsyQ nextfile wrapper). */
void cardNextFile(void) { nextfile(); }


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_800295F0);


/**
 * @brief Check if func_80029550 succeeds with a local buffer.
 *
 * Calls func_80029550 with a0, a1, and a stack-allocated buffer.
 * Returns 1 if the result is nonzero, 0 otherwise.
 *
 * @param a0 First parameter passed through.
 * @param a1 Second parameter passed through.
 * @return Boolean: 1 if func_80029550 returned nonzero.
 */
s32 checkCardFileExists(s32 a0, s32 a1) {
    s32 buf[10];
    return func_80029550(a0, a1, buf) != 0;
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80029680);


/**
 * @brief Seek to an offset in a file and write data.
 * @param a0 File descriptor.
 * @param a1 Pointer to data buffer to write.
 * @param a2 Number of bytes to write.
 * @param a3 File offset to seek to before writing.
 * @return Number of bytes written on success, -1 if the seek failed.
 */
s32 seekAndWrite(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result = lseek(a0, a3, 0);
    if (result == -1) return -1;
    return write(a0, a1, a2);
}


/**
 * @brief Seek to an offset in a file and read data.
 * @param a0 File descriptor.
 * @param a1 Pointer to destination buffer.
 * @param a2 Number of bytes to read.
 * @param a3 File offset to seek to before reading.
 * @return Number of bytes read on success, -1 if the seek failed.
 */
s32 seekAndRead(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result = lseek(a0, a3, 0);
    if (result == -1) return -1;
    return read(a0, a1, a2);
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80029850);


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80029A20);


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80029BA0);


/**
 * @brief Seek to an offset in a file and write data, returning fd on success.
 * @param a0 File descriptor.
 * @param a1 Pointer to data buffer to write.
 * @param a2 Number of bytes to write.
 * @param a3 File offset to seek to before writing.
 * @return File descriptor on success, -1 on failure.
 */
s32 seekWriteReturnFd(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result;
    if (lseek(a0, a3, 0) < 0) {
        return -1;
    }
    result = write(a0, a1, a2);
    if (result != 0) {
        return -1;
    }
    return a0;
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80029CB8);


/**
 * @brief Seek to an offset in a file and read data, returning fd on success.
 * @param a0 File descriptor.
 * @param a1 Pointer to destination buffer.
 * @param a2 Number of bytes to read.
 * @param a3 File offset to seek to before reading.
 * @return File descriptor on success, -1 on failure.
 */
s32 seekReadReturnFd(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result;
    if (lseek(a0, a3, 0) < 0) {
        return -1;
    }
    result = read(a0, a1, a2);
    if (result != 0) {
        return -1;
    }
    return a0;
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80029DAC);


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80029E40);


/**
 * @brief Erase a memory card file.
 *
 * Sets up the card subsystem, checks if the file exists via checkCardFileExists,
 * builds the filename via func_80029360, and calls erase. If erase succeeds
 * (returns non-zero), returns 1. Otherwise calls cleanup and returns -1.
 *
 * @param a0 Packed card identifier.
 * @param a1 File index or name parameter.
 * @return 1 if erase succeeded, -1 on failure.
 */
s32 eraseCardFile(s32 a0, s32 a1) {
    s32 buf[8];
    initCardEventHandlers();
    if (checkCardFileExists(a0, a1) == 0) {
        return -1;
    }
    func_80029360(a0, a1, (s32)buf);
    if (erase((s32)buf) != 0) {
        return 1;
    }
    markCardBusy(a0);
    return -1;
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80029F5C);


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_80029FDC);


/**
 * @brief Shut down the memory card subsystem by closing all 8 card events.
 * @note Disables interrupts via func_800472E4, closes all events in D_80082FB4[0..7],
 *       re-enables interrupts, then calls func_8004D968 for final cleanup.
 */
void shutdownCardSubsystem(void) {
    extern s32 D_80082FB4[];
    func_800472E4();
    CloseEvent(D_80082FB4[0]);
    CloseEvent(D_80082FB4[1]);
    CloseEvent(D_80082FB4[2]);
    CloseEvent(D_80082FB4[3]);
    CloseEvent(D_80082FB4[4]);
    CloseEvent(D_80082FB4[5]);
    CloseEvent(D_80082FB4[6]);
    CloseEvent(D_80082FB4[7]);
    func_800472F4();
    func_8004D968();
}


/** @brief Calls func_80027448 and cdDisableInterruptWrapper in sequence. */
void initBattleSubsystems(void) {
    func_80027448();
    cdDisableInterruptWrapper();
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_8002A150);


/**
 * @brief Apply a transformation or encoding to a value if a global flag is set.
 * @param a0 First parameter (context or key).
 * @param a1 Value to potentially transform; returned unchanged if D_80052958 is 0.
 * @return Transformed a1 if D_80052958 is nonzero, otherwise the original a1.
 * @note When active, calls func_8002E8DC with parameters from D_800837A0/AC/AE globals,
 *       then applies func_8002A45C to the result.
 */
s32 transformValueIfActive(s32 a0, s32 a1) {
    if (D_80052958 != 0) {
        s32 result = func_8002E8DC(a0, a1, D_800837AC, D_800837AE, (s32)D_800837A0, 7);
        a1 = func_8002A45C(a0, result);
    }
    return a1;
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
 * @brief Count the number of non-zero bytes from a0 until a null terminator.
 * @param a0 Pointer to null-terminated byte string.
 * @return Number of bytes before the null terminator (string length).
 */
s32 btlStrlen(u8 *a0) {
    s32 count = 0;
    goto test;
inc:
    count++;
test:
    if (*a0++ != 0) goto inc;
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
INCLUDE_ASM("asm/nonmatchings/btl_anim", func_8002A36C);


/** @brief Wrapper for getCharNamePtr. */
void getCharNamePtrWrapper(void) { getCharNamePtr(); }


/** @brief Wrapper for getCharNamePtr (duplicate of getCharNamePtrWrapper). */
void getCharNamePtrWrapper2(void) { getCharNamePtr(); }


/** @brief Wrapper for getBattleCharName. */
void getBattleCharNameWrapper(void) { getBattleCharName(); }


/**
 * @brief Copy 8-byte display field from *D_8005F134 to destination.
 *
 * @param dst Destination for the 8-byte unaligned copy (RECT-sized).
 */
void copyDisplayRect(RECT *dst) {
    RECT *src = (RECT *)D_8005F134;
    *dst = *src;
}


/**
 * @brief Copy display coordinates (x, y) from the global display struct D_8005F134.
 * @param a0 Destination buffer; receives x at offset 0 and y at offset 2 (both u16).
 */
void copyDisplayCoords(u8 *a0) {
    u8 *p = D_8005F134;
    *(u16 *)(a0 + 0) = *(u16 *)(p + 8);
    *(u16 *)(a0 + 2) = *(u16 *)(p + 0xA);
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_8002A45C);


/**
 * @brief Swap the active display list buffer and clear the new OT.
 *
 * Toggles between two display list buffers at g_battleAnims+0x640 and
 * g_battleAnims+0x698. Stores the new active buffer at +0x6F0, clears
 * its ordering table (18 entries), and copies the GPU packet pointer
 * from offset +0x54 to offset +0x00.
 */
void swapDisplayList(void) {
    s32 base = (s32)&g_battleAnims; /* (s32) cast prevents symbol+constant folding */
    s32 cur = *(s32 *)(base + 0x6F0); /* active DisplayListBuf pointer */
    s32 newBuf = base + 0x640; /* DisplayListBuf A */
    s32 v1;
    if (cur == newBuf) {
        newBuf = base + 0x698; /* DisplayListBuf B */
    }
    *(s32 *)(base + 0x6F0) = newBuf;
    ClearOTag((u32 *)(newBuf + 8), 0x12); /* clear OT (18 entries) */
    v1 = *(s32 *)(base + 0x6F0);
    *(s32 *)v1 = *(s32 *)(v1 + 0x54); /* pktAlloc = pktBase */
}

/**
 * @brief Swap the active display list buffer and clear the new OT (duplicate).
 *
 * Identical logic to swapDisplayList — toggles display list buffers,
 * clears OT, copies GPU packet pointer.
 */
void swapDisplayList2(void) {
    s32 base = (s32)&g_battleAnims; /* (s32) cast prevents symbol+constant folding */
    s32 cur = *(s32 *)(base + 0x6F0); /* active DisplayListBuf pointer */
    s32 newBuf = base + 0x640; /* DisplayListBuf A */
    s32 v1;
    if (cur == newBuf) {
        newBuf = base + 0x698; /* DisplayListBuf B */
    }
    *(s32 *)(base + 0x6F0) = newBuf;
    ClearOTag((u32 *)(newBuf + 8), 0x12); /* clear OT (18 entries) */
    v1 = *(s32 *)(base + 0x6F0);
    *(s32 *)v1 = *(s32 *)(v1 + 0x54); /* pktAlloc = pktBase */
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_8002A5E8);



/** @brief Calls func_8002A5E8(a0, 0) then advanceBattleTimer(a0).
 *  @param a0 Parameter passed to both calls.
 */
void renderAndUpdateDisplay(s32 a0) {
    func_8002A5E8(a0, 0);
    advanceBattleTimer(a0);
}


/** @brief Calls func_8002A5E8(a0, 1).
 *  @param a0 First parameter.
 */
void renderDisplay(s32 a0) {
    func_8002A5E8(a0, 1);
}


/**
 * @brief Read the current packet allocation pointer from the active display list buffer.
 * @return The pktAlloc field of the DisplayListBuf pointed to by D_800834C0.
 */
s32 getDisplayListHead(void) {
    return ((DisplayListBuf *)D_800834C0)->pktAlloc;
}


/**
 * @brief Read the base packet pointer from the active display list buffer.
 * @return The pktBase field of the DisplayListBuf pointed to by D_800834C0.
 */
s32 getDisplayListPacketPtr(void) {
    return ((DisplayListBuf *)D_800834C0)->pktBase;
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
void storeGpuPacket(s32 pkt) {
    s32 base = (s32)&g_battleAnims; /* (s32) cast prevents symbol+constant folding */
    s32 limit;
    *(s32 *)*(s32 *)(base + 0x6F0) = pkt; /* store pkt to active DisplayListBuf.pktAlloc */
    base = *(s32 *)(base + 0x6F0); /* reload active DisplayListBuf pointer */
    limit = *(s32 *)(base + 4); /* DisplayListBuf.pktLimit */
    if ((u32)limit < (u32)pkt) {
        if ((u32)pkt <= 0x801AFFFFU) {
            printf((char *)D_800101D0, (u32)pkt - (u32)limit);
        }
    }
}


/** @brief Returns the address of the ordering table (DisplayListBuf.ot) in the active buffer. */
s32 getDisplayListOtBase(void) {
    return D_800834C0 + 8; /* offset of ot[] in DisplayListBuf */
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_8002A92C);


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_8002AA18);


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_8002AAC0);


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_8002AB5C);



/**
 * @brief Get a pointer to a battle entity by index.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @return Pointer to the entity.
 */
BattleDisplayEntity *getBattleEntity(s32 idx) {
    return &g_battleEntities[idx];
}

/**
 * @brief Set a battle entity's animation speed, clamped to [3, 11].
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
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
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @return Animation speed value for the entity.
 */
s32 getBattleEntityAnimSpeed(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->animSpeed;
}


/**
 * @brief Store a byte value into a battle entity's subFields array.
 * @param idx Entity index (stride 64 bytes).
 * @param offset Index into the subFields array (0 or 1).
 * @param val Byte value to store.
 */
void setBattleEntitySubField(s32 idx, s32 offset, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->subFields[offset] = val;
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_8002ACF4);


/**
 * @brief Set a battle entity's bounding rectangle.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @param src Source RECT to copy.
 */
void setBattleEntityBoundRect(s32 idx, RECT *src) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->boundRect = *src;
}


INCLUDE_ASM("asm/nonmatchings/btl_anim", func_8002AD3C);


// battle_entity_get_field_38 - g_battleEntities stride 64

/**
 * @brief Get a battle entity's bounding rectangle.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @param dst Destination RECT to copy into.
 */
void getBattleEntityBoundRect(s32 idx, RECT *dst) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    *dst = entity->boundRect;
}


/**
 * @brief Get a battle entity's display rectangle.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @param dst Destination RECT to copy into.
 */
void getBattleEntityDispRect(s32 idx, RECT *dst) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    *dst = entity->dispRect;
}


/**
 * @brief Get a battle entity's entity type.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @return Entity type value.
 */
s32 getBattleEntityType(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->entityType;
}
