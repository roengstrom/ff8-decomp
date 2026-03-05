#include "common.h"
#include "psxsdk/libgpu.h"
#include "overlay.h"


/** @brief Empty stub (no-op). */
void func_80035CE0(void) {
}


/** @brief Empty stub (no-op). */
void func_80035CE8(void) {
}


/** @brief Wrapper that initiates a CD-ROM read via func_8003882C. */
void func_80035CF0(void) { func_8003882C(); }


/** @brief Wrapper that initiates an async CD-ROM read via func_80038868. */
void func_80035D10(void) { func_80038868(); }


INCLUDE_ASM("asm/nonmatchings/264E0", func_80035D30);


extern s32 D_8008514C;
extern volatile u16 D_80085208;

/** @brief Wrapper that polls CD-ROM read completion via func_800393C8. */
void func_80035E00(void) {
    func_800393C8();
}


/** @brief Wrapper that calls func_800389CC (CD-ROM related). */
void func_80035E20(void) {
    func_800389CC();
}


/**
 * @brief Get overlay load status.
 * @return Current value of D_8008514C (overlay load result code).
 */
s32 func_80035E40(void) {
    return D_8008514C;
}


/**
 * @brief Get a signed 16-bit game state value.
 * @return D_80085208 sign-extended to s32.
 */
s32 func_80035E50(void) {
    return (s16)D_80085208;
}


/**
 * @brief Reset overlay queue state — clears read/write indices and sets result to -1.
 */
void func_80035E68(void) {
    extern s32 D_80085140;
    extern s32 D_80085144;
    extern volatile s32 D_8008514C;
    D_80085140 = 0;
    D_80085144 = 0;
    D_8008514C = -1;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80035E8C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80035F70);


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
void func_80035FF4(s32 cmd, s32 overlay_id, s32 param, s32 load_addr, s32 callback1, s32 callback2) {
    extern s32 D_80085140;
    extern s32 D_80085144;
    extern OvlCmdEntry g_ovlCmdQueue[];
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
    func_800472E4();
    was_equal = D_80085140;
    D_80085140 = (was_equal + 1) & 7;
    was_equal = was_equal == D_80085144;
    func_800472F4();
    if (was_equal == 1) {
        func_80035D30(cmd, overlay_id, param, load_addr);
    }
}


/**
 * @brief Enqueue a type-0x11 overlay load (dependency/sub-overlay).
 * @param a0 Parameter passed as the command param.
 * @param a1 Destination load address.
 */
void func_800360D0(s32 a0, s32 a1) {
    func_80035FF4(0, 0x11, a0, a1, 0, 0);
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
void func_80036104(s32 overlay_id, s32 a1, s32 a2) {
    extern u32 load_table[]; // D_80053C58
    extern s32 D_8008514C;
    extern u8 D_8008520A;
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
            func_800360D0(dep, 0x801E0000);
            D_8008520A = dep;
        }
    }
    func_80035FF4(0, overlay_id, -1, descriptor, a1, a2);
}


/**
 * @brief Enqueue an overlay load by explicit ID and address, no callbacks.
 * @param a0 Overlay identifier.
 * @param a1 Destination load address.
 */
void func_800361C0(s32 a0, s32 a1) {
    func_80035FF4(0, a0, -1, a1, 0, 0);
}


extern u8 D_80035F70[];
extern s32 D_80085140;
extern s32 D_80085144;

/**
 * @brief Enqueue a type-0x11 overlay load with a completion callback.
 *
 * Uses func_80035F70 as the first callback and the load address as the
 * second callback argument.
 *
 * @param a0 Parameter passed as the command param.
 * @param a1 Destination load address (also used as callback2).
 */
void func_800361F8(s32 a0, s32 a1) {
    func_80035FF4(0, 0x11, a0, a1, (s32)D_80035F70, a1);
}


/**
 * @brief Check if the overlay command queue is empty.
 * @return 1 if the write index equals the read index (queue empty), 0 otherwise.
 */
s32 func_80036234(void) {
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
void func_80036254(s32 a0) {
    extern u8 D_80053CF0[];
    DrawSync(0);
    VSync(0);
    StoreImage(D_80053CF0, (u32 *)0x801BF000);
    DrawSync(0);
    VSync(0);
    ClearImage(D_80053CF0, 0, 0, 0);
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


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003631C);


/** @brief Load overlay 0 (default/main module) with no callbacks. */
void func_80036444(void) {
    func_80036104(0, 0, 0);
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003646C);


// init_card_hand_slots - initializes 128 card slots (id=0, type=0xFF)

INCLUDE_ASM("asm/nonmatchings/264E0", func_80036690);


