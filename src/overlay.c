#include "common.h"
#include "memcard.h"
#include "psxsdk/libetc.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "input/gamepad.h"
#include "ui/seed_rank.h"
#include "cd.h"
#include "gamestate.h"
#include "overlay.h"
#include "render.h"
#include "psxsdk/libapi.h"


/** @brief Empty stub (no-op). */
void ovlStubA(void) {
}


/** @brief Empty stub (no-op). */
void ovlStubB(void) {
}


/** @brief Wrapper that initiates a CD-ROM read via LoadDataFromDisc. */
void startCdRead(s32 lba, u32 size, u8 *dest, void (*callback)(void)) {
    LoadDataFromDisc(lba, size, dest, callback);
}


/** @brief Wrapper that initiates an async CD-ROM read via LoadCompressedDataFromDisc. */
void startCdReadAsync(s32 lba, u32 size, u8 *dest, void (*callback)(void)) {
    LoadCompressedDataFromDisc(lba, size, dest, callback);
}


extern volatile u16 D_80085208;
extern s32 D_80085140;
extern s32 D_80085144;
extern OvlCmdEntry g_ovlCmdQueue[];
extern u32 load_table[];
extern u32 D_80097420[];
extern u32 D_80053D08[][2];
extern RECT D_80053CF0;
extern RECT D_80053CF8;
extern RECT D_80053D00;
extern DISPENV D_80085150;
extern s32 D_8005F138;
extern void func_80035E8C(void);

void func_80035D30(s32 cmd, s32 overlay_id, s32 lba_offset, s32 load_addr) {
    s32 lba;
    u32 size;
    s32 offset;
    void (*callback)(void);

    callback = func_80035E8C;

    if (lba_offset < 0) {
        lba = D_80097420[overlay_id * 2];
        size = D_80097420[overlay_id * 2 + 1];
        offset = 0;
    } else {
        u32 (*entry)[2];
        s32 bit;
        s32 adjusted;

        entry = D_80053D08;
        entry += lba_offset;
        offset = (*entry)[0];
        lba = D_80097420[overlay_id * 2];
        bit = offset & 1;
        cmd = (u32)bit < 1;
        offset = offset & ~1;
        if (offset < 0) {
            adjusted = offset + 0x7FF;
        } else {
            adjusted = offset;
        }
        offset = adjusted >> 11;
        size = (*entry)[1];
    }

    if (size != 0) {
        if (cmd != 0) {
            startCdReadAsync(lba + offset, size, (u8 *)load_addr, callback);
        } else {
            startCdRead(lba + offset, size, (u8 *)load_addr, callback);
        }
    }
}

/** @brief Wrapper that polls CD-ROM read completion via func_800393C8. */
s32 pollCdReadStatus(void) {
    return func_800393C8();
}


/** @brief Wrapper that calls resetCdDrive (CD-ROM related). */
void resetCdState(void) {
    resetCdDrive();
}


/**
 * @brief Get overlay load status.
 * @return Current value of D_8008514C (overlay load result code).
 */
s32 getOverlayLoadStatus(void) {
    return D_8008514C;
}


/**
 * @brief Get a signed 16-bit game state value.
 * @return D_80085208 sign-extended to s32.
 */
s32 getGameStateS16(void) {
    return (s16)D_80085208;
}


/**
 * @brief Reset overlay queue state — clears read/write indices and sets result to -1.
 */
void resetOverlayQueue(void) {
    D_80085140 = 0;
    D_80085144 = 0;
    D_8008514C = -1;
}


void func_80035E8C(void) {
    OvlCmdEntry *slot;
    s32 next_read_idx;
    u32 ovl_id;

    slot = &g_ovlCmdQueue[D_80085144];
    ovl_id = (u16)slot->ovlId;
    if (ovl_id - 1 < 0x10) {
        D_8008514C = ovl_id;
    }
    D_80085208 = slot->param;
    if (slot->callback1 != 0) {
        ((void (*)(s32))slot->callback1)(slot->callback2);
    }

    next_read_idx = (D_80085144 + 1) & 7;
    D_80085144 = next_read_idx;
    if (D_80085140 != next_read_idx && getRenderFlag() == 0) {
        slot = &g_ovlCmdQueue[D_80085144];
        func_80035D30((u16)slot->cmd, (u16)slot->ovlId, slot->param, slot->loadAddr);
    }
}


/**
 * @brief Process a TIM image packet and upload to VRAM.
 *
 * If the packet type byte is 0x10 (TIM format), optionally loads
 * a CLUT (if bit 3 of flags byte is set), then always loads the
 * pixel data via LoadImage.
 *
 * @param a0 Pointer to the TIM packet header.
 */
void loadTimImage(u8 *a0) {
    u8 *base = a0;
    u8 *ptr;
    u8 *extra;

    if (base[0] != 0x10) {
        return;
    }
    if (base[4] & 8) {
        extra = base + 8;
        ptr = base + (*(s32 *)(base + 8) + 8);
    } else {
        extra = 0;
        ptr = base + 8;
    }
    if (extra != 0) {
        LoadImage((RECT *)(extra + 4), (u32 *)(extra + 0xC));
        DrawSync(0);
    }
    LoadImage((RECT *)(ptr + 4), (u32 *)(ptr + 0xC));
}


/**
 * @brief Enqueue an overlay load command into the circular command queue.
 *
 * Writes a 20-byte command entry to g_ovlCmdQueue, advances the write index
 * (D_80085140) with wrap-around at 8 slots, and immediately dispatches
 * via func_80035D30 if the queue was previously empty.
 *
 * @param cmd       Command type.
 * @param overlay_id Overlay identifier.
 * @param param     Command-specific parameter.
 * @param load_addr Destination load address for the overlay.
 * @param callback1 First callback address (or 0).
 * @param callback2 Second callback address (or 0).
 */
void enqueueOverlayCmd(s32 cmd, s32 overlay_id, s32 param, s32 load_addr, s32 callback1, s32 callback2) {
    OvlCmdEntry *slot;
    s32 write_idx;
    s32 was_equal;

    write_idx = D_80085140;
    slot = &g_ovlCmdQueue[write_idx];
    slot->cmd = cmd;
    slot->param = param;
    slot->loadAddr = load_addr;
    slot->callback1 = callback1;
    slot->callback2 = callback2;
    slot->ovlId = overlay_id;
    EnterCriticalSection();
    was_equal = D_80085140;
    D_80085140 = (was_equal + 1) & 7;
    was_equal = was_equal == D_80085144;
    ExitCriticalSection();
    if (was_equal == 1) {
        func_80035D30(cmd, overlay_id, param, load_addr);
    }
}


/**
 * @brief Enqueue a type-0x11 overlay load (dependency/sub-overlay).
 * @param a0 Parameter passed as the command param.
 * @param a1 Destination load address.
 */
void loadSubOverlay(s32 a0, s32 a1) {
    enqueueOverlayCmd(0, 0x11, a0, a1, 0, 0);
}


/**
 * @brief Load an overlay by ID, resolving dependencies from load_table.
 *
 * Looks up the overlay descriptor from load_table (D_80053C58), extracts
 * the dependency byte (low 8 bits), and loads the dependency first if
 * it differs from the currently loaded dependency (D_8008520A). Then
 * enqueues the main overlay load with callbacks a1 and a2.
 *
 * @param overlay_id Index into the overlay load table.
 * @param a1         Callback address invoked on load completion (or 0).
 * @param a2         Second callback address (or 0).
 */
void loadOverlay(s32 overlay_id, s32 a1, s32 a2) {
    u32 descriptor;
    s32 dep;
    u32 *p;

    p = load_table + overlay_id * 2;
    descriptor = *p;
    D_8008514C = -2;
    dep = descriptor & 0xFF;
    descriptor &= 0xFFFFFF00;
    if (dep != D_8008520A) {
        if (dep != 0) {
            loadSubOverlay(dep, 0x801E0000);
            D_8008520A = dep;
        }
    }
    enqueueOverlayCmd(0, overlay_id, -1, descriptor, a1, a2);
}


/**
 * @brief Enqueue an overlay load by explicit ID and address, no callbacks.
 * @param a0 Overlay identifier.
 * @param a1 Destination load address.
 */
void loadOverlayDirect(s32 a0, s32 a1) {
    enqueueOverlayCmd(0, a0, -1, a1, 0, 0);
}



/**
 * @brief Enqueue a type-0x11 overlay load with a completion callback.
 *
 * Uses func_80035F70 as the first callback and the load address as the
 * second callback argument.
 *
 * @param a0 Parameter passed as the command param.
 * @param a1 Destination load address (also used as callback2).
 */
void loadOverlayWithTimCallback(s32 a0, s32 a1) {
    enqueueOverlayCmd(0, 0x11, a0, a1, (s32)loadTimImage, a1);
}


/**
 * @brief Check if the overlay command queue is empty.
 * @return 1 if the write index equals the read index (queue empty), 0 otherwise.
 */
s32 isOverlayQueueEmpty(void) {
    return D_80085140 == D_80085144;
}


/**
 * @brief Save VRAM framebuffer contents, clear, and optionally relocate a region.
 *
 * Stores the current framebuffer (D_80053CF0 rect) to 0x801BF000 in main RAM,
 * clears the framebuffer area, and if a0 < 0, moves a 128x224 VRAM region
 * from x=0x300 to x=0x180 (swaps display/draw pages).
 *
 * @param a0 If negative, performs the VRAM region move.
 */
void saveAndClearFramebuffer(s32 a0) {
    DrawSync(0);
    VSync(0);
    StoreImage(&D_80053CF0, (u32 *)0x801BF000);
    DrawSync(0);
    VSync(0);
    ClearImage(&D_80053CF0, 0, 0, 0);
    DrawSync(0);
    VSync(0);
    if (a0 < 0) {
        s16 rect[4];
        rect[0] = 0x300;
        rect[1] = 0;
        rect[2] = 0x80;
        rect[3] = 0xE0;
        MoveImage(rect, 0x180, 0);
    }
    DrawSync(0);
    VSync(0);
}


/**
 * @brief Clear the display, reload the image at 0x801BF000 and, if requested, reset the display environment.
 *
 * A negative @p a0 also copies the 128x224 block at x 384 to x 768. When
 * D_8008520C is set, D_80085150 becomes a 320x224 display environment, moved
 * down 24 lines in PAL, and D_8005F138 points at it.
 *
 * @param a0 Negative to copy the block.
 */
void func_8003631C(s32 a0) {
    RECT rect;

    ClearImage(&D_80053CF8, 0, 0, 0);
    ClearImage(&D_80053D00, 0, 0, 0);
    DrawSync(0);
    VSync(0);
    LoadImage(&D_80053CF0, (u32 *)0x801BF000);
    DrawSync(0);
    VSync(0);

    if (a0 < 0) {
        rect.x = 0x180;
        rect.y = 0;
        rect.w = 0x80;
        rect.h = 0xE0;
        MoveImage(&rect, 0x300, 0);
    }

    DrawSync(0);
    VSync(0);

    if (D_8008520C != 0) {
        SetDefDispEnv(&D_80085150, 0, 0, 0x140, 0xE0);
        if (GetVideoMode() == MODE_PAL) {
            D_80085150.screen.y += 0x18;
        }
        D_8005F138 = (s32)&D_80085150;
    }
}


/** @brief Load overlay 0 (default/main module) with no callbacks. */
void loadDefaultOverlay(void) {
    loadOverlay(0, 0, 0);
}


extern void func_801F04E8(s32 a0, s32 a1, s32 a2, s32 a3);

/**
 * @brief Run a battle/menu transition: snapshot state, swap overlays, restore.
 *
 * Saves dialog 0's text position, brightness and entity type, the countdown's
 * visibility and both gauges' flags, then zeroes the position, visibility, flags
 * and a small block of static state (D_80085210, D_8008520A, D_8008520C, the
 * @c D_8008513C render-flag bitmask). After running @c resetSeedRankNotification
 * and forcing dialog 0 into a known idle configuration (type 6, brightness
 * 0x1000, text speed 0, corner icon 0x56), the function blanks the display, drains the
 * CD pipeline, snapshots the framebuffer to @p arg0, brings in the two
 * fixed-address overlays at 0x801CD000 / 0x801D5000, and jumps into the
 * post-transition entry point at @c func_801F04E8 with the low 31 bits of
 * @p arg0 plus @p arg1 / @p arg2 / @p arg3 passed through. Once that returns,
 * a second CD-drain + DrawSync/VSync pair settles the GPU, @c func_8003631C
 * finalizes against @p arg0, the saved state is restored, and the final
 * @c D_8008513C bitmask is returned to the caller so it can branch on the
 * flags accumulated during the transition.
 *
 * @param arg0 Framebuffer/state pointer (high bit gets stripped before the
 *             @c func_801F04E8 call).
 * @param arg1 Passthrough argument 1 to @c func_801F04E8.
 * @param arg2 Passthrough argument 2 to @c func_801F04E8.
 * @param arg3 Passthrough argument 3 to @c func_801F04E8.
 * @return The contents of @c D_8008513C at function exit (render-flag bitmask).
 */
s32 func_8003646C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u16 saved250;
    u8 saved252;
    u8 savedCountdownVisible;
    u8 savedGauge0Flags;
    u8 savedGauge1Flags;
    s32 savedBrightness;
    s32 savedEntityType;

    InitGameVersion();

    saved250 = g_engine.dialogs.entries[0].textX;
    D_80085210 = arg0;
    saved252 = g_engine.dialogs.entries[0].textY;
    D_8008520A = 0;
    savedCountdownVisible = g_engine.countdown.visible;
    savedGauge0Flags = g_engine.gauges[0].flags;
    savedGauge1Flags = g_engine.gauges[1].flags;

    g_engine.dialogs.entries[0].textY = 0;
    D_8008520C = (g_engine.dialogs.entries[0].textX = 0);
    g_engine.countdown.visible = 0;
    g_engine.gauges[0].flags = 0;
    g_engine.gauges[1].flags = 0;
    D_8008520B = savedCountdownVisible;

    resetSeedRankNotification();

    savedBrightness = g_engine.dialogs.entries[0].brightness;
    savedEntityType = readDialogEntityType(0);
    setDialogEntityType(0, 6);
    setDialogBrightness(0, 0x1000);
    setDialogTextSpeed(0, 0);
    setDialogCornerIcon(0, 0x56);
    D_8008513C = 0;
    setRenderFlag(0);
    DrawSync(0);
    VSync(0);
    SetDispMask(0);
    resetOverlayQueue();
    loadDefaultOverlay();

    while (pollCdReadStatus() != 0) {
    }

    saveAndClearFramebuffer(arg0);
    loadOverlayWithTimCallback(9, 0x801CD000);
    loadOverlayWithTimCallback(0xA, 0x801D5000);
    RequestActuatorSetup(0);
    func_801F04E8(arg0 & 0x7FFFFFFF, arg1, arg2, arg3);

    while (pollCdReadStatus() != 0) {
    }

    DrawSync(0);
    VSync(0);
    func_8003631C(arg0);
    DrawSync(0);
    VSync(0);

    g_engine.gauges[0].flags = savedGauge0Flags;
    g_engine.gauges[1].flags = savedGauge1Flags;
    g_engine.countdown.visible = D_8008520B;
    setDialogBrightness(0, savedBrightness);
    setDialogCornerIcon(0, 0);
    setDialogEntityType(0, savedEntityType);
    g_engine.dialogs.entries[0].textX = saved250;
    g_engine.dialogs.entries[0].textY = saved252;
    return D_8008513C;
}


/** @brief Initialize or reset card hand slot states.
 *
 *  If @p a0 is non-zero, sets chars[0].characterId to 8 and sets
 *  @ref PARTY_LOCK_LOCKED. If @p a0 is zero, clears that bit
 *  and fills all 8 character slots with descending index values (7 down to 0).
 *
 *  @param a0 Non-zero to set single slot, zero to reset all 8 character slots.
 */
void resetCardSlots(s32 a0) {
    if (a0 != 0) {
        u8 flags = g_gameState.mainData.partyLockFlag;
        g_gameState.chars[0].characterId = 8;
        g_gameState.mainData.partyLockFlag = flags | PARTY_LOCK_LOCKED;
    } else {
        a0 = 7;
        g_gameState.mainData.partyLockFlag &= ~PARTY_LOCK_LOCKED;
        do {
            g_gameState.chars[a0].characterId = a0;
            a0--;
        } while (a0 >= 0);
    }
}
