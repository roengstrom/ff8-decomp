#include "common.h"
#include "psxsdk/libgpu.h"


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


INCLUDE_ASM("asm/nonmatchings/264E0", func_80035E68);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80035E8C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80035F70);


/**
 * @brief Enqueue an overlay load command into the circular command queue.
 *
 * Writes a 20-byte command entry to D_80085168, advances the write index
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
    extern u8 D_80085168[];
    u8 *slot;
    s32 write_idx;
    s32 was_equal;

    write_idx = D_80085140;
    slot = D_80085168 + write_idx * 20;
    *(s16 *)(slot + 0x0) = cmd;
    *(s16 *)(slot + 0x2) = param;
    *(s32 *)(slot + 0x8) = load_addr;
    *(s32 *)(slot + 0xC) = callback1;
    *(s32 *)(slot + 0x10) = callback2;
    *(s16 *)(slot + 0x4) = overlay_id;
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


/**
 * @brief Initialize 128 card hand slots to empty.
 *
 * Sets each 2-byte slot: byte 0 (card ID) = 0, byte 1 (card type) = 0xFF.
 *
 * @param ptr Pointer to the card hand array (256 bytes).
 */
void func_800366E8(u8 *ptr) {
    s32 i = 0;
    do {
        i++;
        ptr[1] = 0xFF;
        ptr[0] = 0;
        ptr += 2;
    } while (i < 128);
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80036710);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003678C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003685C);


/**
 * @brief Map a card ID to its rarity level (0-6).
 *
 * Divides the card ID range into 7 tiers:
 * 0-19 = level 0, 20-38 = level 1, 39-57 = level 2,
 * 58-77 = level 3, 78-82 = level 4, 83-91 = level 5, 92+ = level 6.
 *
 * @param a0 Card ID.
 * @return Rarity level (0-6).
 */
s32 func_80036978(s32 a0) {
    s32 result;
    if (a0 < 0x14) {
        result = 0;
    } else if (a0 < 0x27) {
        result = 1;
    } else if (a0 < 0x3A) {
        result = 2;
    } else if (a0 < 0x4E) {
        result = 3;
    } else if (a0 < 0x53) {
        result = 4;
    } else if (a0 < 0x5C) {
        result = 5;
    } else {
        result = 6;
    }
    return result;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_800369CC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80036B90);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80036C74);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80036D44);


/**
 * @brief Set the active party leader and clear the other two party slots.
 *
 * Writes a0 to D_80077378+0xAF4 (party slot 0), sets slots 1 and 2 to
 * 0xFF (empty), then calls func_80023888 to apply the party change.
 *
 * @param a0 Character ID for the party leader.
 */
void func_80036E8C(s32 a0) {
    extern u8 D_80077378[];
    D_80077378[0xAF4] = a0;
    D_80077378[0xAF5] = 0xFF;
    D_80077378[0xAF6] = 0xFF;
    func_80023888();
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80036EC0);


/**
 * @brief Build a 16-bit bitmask of GF availability flags.
 *
 * Iterates over 16 GF entries in D_80077378 (stride 0x44 = 68 bytes),
 * checking bit 0 of byte +0x61 (availability flag). Sets the corresponding
 * bit in the result mask for each available GF.
 *
 * @return Bitmask where bit N is set if GF N is available.
 */
u16 func_80036F60(void) {
    extern u8 D_80077378[];
    u16 mask;
    s32 i;
    s32 bit;
    u8 *p;

    mask = 0;
    i = 0;
    bit = 1;
    p = D_80077378;
    do {
        if (p[0x61] & 1) {
            mask |= (bit << i);
        }
        p += 0x44;
        i++;
    } while (i < 16);
    return mask;
}


/**
 * @brief Copy a GF's current HP from the character table to the GF table.
 *
 * Reads a u16 from D_80078720 + a0*12 + 0x61A and writes it to
 * D_80077378 + a0*68 + 0x62.
 *
 * @param a0 GF index (0-15).
 */
void func_80036FA4(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078720[];
    s32 base1 = (s32)D_80077378;
    s32 off1 = a0 * 68;
    s32 base2 = (s32)D_80078720;
    *(s16 *)(base1 + off1 + 0x62) = *(u16 *)(base2 + a0 * 12 + 0x61A);
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80036FE0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800370AC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037198);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800371D0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003720C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037240);


/** @brief Returns a pointer to global D_800786D8. */
u8 *func_800372D0(void) {
    extern u8 D_800786D8;
    return &D_800786D8;
}


/** @brief Sets bit 0x1 in the byte at D_800786D8. */
void func_800372DC(void) {
    u8 *p = func_800372D0();
    *p |= 0x1;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037308);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800375A0);


/**
 * Wrapper for func_800375A0 with fixed 6th argument 0x64808080.
 *
 * @param a0 First argument passed through
 * @param a1 Second argument passed through
 * @param a2 Third argument passed through
 * @param a3 Fourth argument passed through
 * @param arg4 Fifth argument passed through from caller's stack
 */
void func_80037678(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    func_800375A0(a0, a1, a2, a3, arg4, 0x64808080);
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_800376A8);


// mc_xor_checksum

INCLUDE_ASM("asm/nonmatchings/264E0", func_8003777C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800377B4);


/**
 * @brief Compute an XOR checksum over 127 bytes (memory card frame header).
 *
 * XORs each of the first 127 bytes together and returns the low 8 bits.
 * Used to compute byte 0x7F of a memory card block header.
 *
 * @param a0 Pointer to the 128-byte memory card frame.
 * @return XOR checksum (0-255).
 */
u32 func_8003786C(u8 *a0) {
    u32 acc = 0;
    s32 i = 0;
    do {
        acc ^= *a0++;
        i++;
    } while ((u32)i < 0x7F);
    return acc & 0xFF;
}


/**
 * @brief Zero 128 bytes of memory (memory card frame header size).
 * @param ptr Pointer to the buffer to clear.
 */
void func_80037894(u8 *ptr) {
    s32 count = 128;
    do {
        *ptr++ = 0;
    } while (--count > 0);
}


/**
 * @brief Initialize a memory card header frame (block 0, "MC" magic).
 *
 * Zeroes the 128-byte frame, sets bytes 0-1 to 'M','C' (0x4D, 0x43),
 * then computes and stores the XOR checksum at byte 0x7F.
 *
 * @param a0 Pointer to the 128-byte frame buffer.
 */
void func_800378B0(u8 *a0) {
    func_80037894(a0);
    a0[0] = 0x4D;
    a0[1] = 0x43;
    a0[0x7F] = func_8003786C(a0);
}


/**
 * @brief Initialize a memory card directory frame (used save slot, type 0xA0).
 *
 * Zeroes the 128-byte frame, sets byte 0 to 0xA0 (in-use flag),
 * bytes 8-9 to 0xFF, then computes and stores the XOR checksum.
 *
 * @param a0 Pointer to the 128-byte frame buffer.
 */
void func_800378F4(u8 *a0) {
    func_80037894(a0);
    a0[0] = 0xA0;
    a0[8] = 0xFF;
    a0[9] = 0xFF;
    a0[0x7F] = func_8003786C(a0);
}


/**
 * @brief Initialize a memory card free/unused directory frame.
 *
 * Zeroes the 128-byte frame, sets bytes 0-3 and 8-9 to 0xFF
 * (marks the directory entry as free), then computes and stores
 * the XOR checksum.
 *
 * @param a0 Pointer to the 128-byte frame buffer.
 */
void func_8003793C(u8 *a0) {
    func_80037894(a0);
    a0[0] = 0xFF;
    a0[1] = 0xFF;
    a0[2] = 0xFF;
    a0[3] = 0xFF;
    a0[8] = 0xFF;
    a0[9] = 0xFF;
    a0[0x7F] = func_8003786C(a0);
}


/** @brief Fills 128 bytes at a0 with 0xFF.
 *  @param a0 Pointer to buffer.
 */
void func_8003798C(u8 *a0) {
    s32 i = 128;
    do {
        *a0++ = 0xFF;
    } while (--i > 0);
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_800379AC);


/** @brief Sets global D_80085218 to 1. */
void func_80037ACC(void) {
    extern u8 D_80085218;
    D_80085218 = 1;
}


/** @brief Returns the unsigned byte value of global D_80085218. */
u32 func_80037ADC(void) {
    extern u8 D_80085218;
    return D_80085218;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037AEC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037B44);

INCLUDE_ASM("asm/nonmatchings/264E0", func_80037B7C);

INCLUDE_ASM("asm/nonmatchings/264E0", func_80037BB0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037BF0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037C30);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037C6C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037CD4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037D40);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037E14);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037E60);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037F08);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037FB0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038030);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800381BC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038308);


/** @brief Returns bits 3-4 of the flags word at offset 0x68 through D_800562C4. */
s32 func_80038364(void) {
    extern u8 *D_800562C4;
    return *(s32 *)(D_800562C4 + 0x68) & 0x18;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003837C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800383B8);


/** @brief Looks up byte from D_8005644B table at index a0 (masked to 8 bits).
 *  @param a0 Table index (only low 8 bits used).
 *  @return The byte value at D_8005644B[a0 & 0xFF].
 */
u8 func_80038424(s32 a0) {
    extern u8 D_8005644B[];
    return D_8005644B[a0 & 0xFF];
}


/** @brief Returns halfword from D_800562C8 table indexed by D_80077E5A. */
u16 func_80038440(void) {
    extern u8 D_80077E5A;
    extern u16 D_800562C8[];
    return D_800562C8[D_80077E5A];
}


/** @brief Linear congruential generator: D_800562D4 = D_800562D4 * 0x41C64E6D + 0x3039.
 *  @return Bits 16-30 of the new state (0-32767).
 */
s32 func_80038464(void) {
    extern s32 D_800562D4;
    D_800562D4 = D_800562D4 * 0x41C64E6D + 0x3039;
    return ((u32)D_800562D4 >> 16) & 0x7FFF;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038490);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800385B0);


/** @brief Calls func_8001313C with a0 passed through and fixed constants.
 *  @param a0 First parameter passed through.
 */
void func_8003863C(s32 a0) {
    func_8001313C(a0, 0, 0x80, 0x7F, 0);
}


/**
 * @brief Initialize the CD-ROM subsystem.
 *
 * Clears CD state variables (D_8008A3D8, D_8008A3D0). If a0 is nonzero,
 * spins until CdInit succeeds, disables CD debug output, sends a
 * CdControlB command (0xE = SetMode, double-speed 0x80), waits 3 vsyncs,
 * then reads the disc ID via func_800385B0 and stores it.
 *
 * @param a0 If nonzero, perform full CD hardware initialization.
 */
void func_80038668(s32 a0) {
    extern u8 D_8008A3D8;
    extern s16 D_8008A3D0;
    extern u8 D_8008A3DA;
    s32 result;
    u8 buf;

    D_8008A3D8 = 0;
    (&D_8008A3D8)[1] = 0;
    D_8008A3D0 = 0;

    if (a0 != 0) {
        do { } while (!CdInit());
        CdSetDebug(0);
        buf = 0x80;
        CdControlB(0xE, &buf, 0);
        VSync(3);
        result = func_800385B0();
        D_8008A3DA = result;
        func_80038CF0((s8)result);
    }
}


/** @brief Clears bit 1 in global D_8008A3D8. */
void func_800386F0(void) {
    extern u8 D_8008A3D8;
    D_8008A3D8 &= 0xFD;
}


/** @brief Sets bit 1 in global D_8008A3D8. */
void func_80038708(void) {
    extern u8 D_8008A3D8;
    D_8008A3D8 |= 0x02;
}


/**
 * @brief Flush CD subsystem and wait for completion.
 *
 * Calls the CD clear, flush, and init routines, then polls until
 * the CD subsystem reports ready.
 */
void func_80038720(void) {
    func_800386F0();
    CdFlush();
    func_800389CC();
    do {
    } while (func_800393C8() != 0);
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038760);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800387F8);


/**
 * @brief Initiate a CD-ROM read operation.
 *
 * Delegates to func_80038760 with mode=3, passing through all four
 * parameters (sector, count, destination, etc.).
 *
 * @param a0 First CD read parameter (e.g. sector number).
 * @param a1 Second CD read parameter (e.g. sector count).
 * @param a2 Third CD read parameter (e.g. destination address).
 * @param a3 Fourth CD read parameter.
 * @return Always 0.
 */
s32 func_8003882C(s32 a0, s32 a1, s32 a2, s32 a3) {
    func_80038760(3, a0, a1, a2, a3);
    return 0;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038868);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800388CC);


/**
 * @brief Perform a synchronous (blocking) CD-ROM read.
 *
 * Initiates a CD read via func_8003882C, then busy-waits on func_800393C8
 * until the read is complete.
 *
 * @param a0 First CD read parameter.
 * @param a1 Second CD read parameter.
 * @param a2 Third CD read parameter.
 * @param a3 Fourth CD read parameter.
 * @return Always 0.
 */
s32 func_80038920(s32 a0, s32 a1, s32 a2, s32 a3) {
    do {
    } while (func_8003882C(a0, a1, a2, a3) != 0);
    do {
    } while (func_800393C8() != 0);
    return 0;
}


/**
 * @brief Perform a synchronous async-initiated CD-ROM read.
 *
 * Calls func_80038868 to start an async read, saves its return value,
 * then busy-waits on func_800393C8 until the read completes.
 *
 * @return The value returned by func_80038868 (async read handle/status).
 */
s32 func_80038994(void) {
    s32 saved = func_80038868();
    do {
    } while (func_800393C8() != 0);
    return saved;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_800389CC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038A18);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038A60);


extern s8 D_8008A3DA;
/**
 * @brief Get the current disc ID.
 * @return Disc identifier as a signed byte (D_8008A3DA).
 */
s8 func_80038CE0(void) {
    return D_8008A3DA;
}


extern u16 D_8008A3D2;
extern u8 D_8008A3D9;

/**
 * @brief Set the current disc number.
 * @param a0 Disc number to store in D_8008A3D2.
 */
void func_80038CF0(s32 a0) {
    D_8008A3D2 = a0;
}


/**
 * @brief Get the current CD-ROM subsystem state.
 * @return CD state machine phase (D_8008A3D9).
 */
u32 func_80038CFC(void) {
    return D_8008A3D9;
}


/**
 * @brief Stop the CD-ROM drive and flush pending commands.
 *
 * Calls CdFlush to clear the CD command queue, then issues CdControl(9)
 * which is the CdlPause command to halt disc spinning.
 */
void func_80038D0C(void) {
    CdFlush();
    CdControl(9, 0, 0);
}


/**
 * @brief Clear CD status flag and invoke pending callback if set.
 *
 * Clears the status byte at D_8008A3D8+1, then calls the function pointer
 * stored at D_8008A3D8+0x20 if it is non-NULL.
 */
void func_80038D3C(void) {
    extern u8 D_8008A3D8[];
    s32 base = (s32)D_8008A3D8;
    void (*callback)(void) = *(void (**)(void))(base + 0x20);
    *(u8 *)(base + 1) = 0;
    if (callback != 0) {
        callback();
    }
}


/** @brief Empty stub (no-op), placeholder in CD-ROM subsystem. */
void func_80038D74(void) {
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038D7C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038DD4);


/**
 * @brief Poll CD-ROM read state and handle completion or timeout.
 *
 * Checks CdSync(1, 0) for asynchronous read status:
 * - Case 2 (complete): resets timeout counter, advances to state 5,
 *   calls func_80038ED4 for post-read processing.
 * - Case 5 (error/retry): increments timeout counter; if >= 0x708 (1800)
 *   frames, resets counter and plays an error sound via func_8001313C.
 *   Calls VSync(0) and returns to state 3 to retry.
 */
void func_80038E28(void) {
    extern u32 D_8008A3C8;
    extern u8 D_8008A3D9;

    switch (CdSync(1, 0)) {
    case 2:
        D_8008A3C8 = 0;
        D_8008A3D9 = 5;
        func_80038ED4();
        break;
    case 5:
        D_8008A3C8++;
        if (D_8008A3C8 >= 0x708) {
            D_8008A3C8 = 0;
            func_8001313C(0x10, 0, 0x80, 0x7F, 0);
        }
        VSync(0);
        D_8008A3D9 = 3;
        break;
    }
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038ED4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038F88);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039040);


/**
 * @brief Poll CD-ROM seek/read state and handle completion or timeout.
 *
 * Similar to func_80038E28 but for a different CD operation phase:
 * - Case 2 (complete): resets timeout counter, advances to state 9,
 *   calls func_80039140 for post-seek processing.
 * - Case 5 (error/retry): increments timeout counter; if >= 0x708 (1800)
 *   frames, resets and plays error sound. Returns to state 7 to retry.
 */
void func_80039094(void) {
    extern u32 D_8008A3C8;
    extern u8 D_8008A3D9;

    switch (CdSync(1, 0)) {
    case 2:
        D_8008A3C8 = 0;
        D_8008A3D9 = 9;
        func_80039140();
        break;
    case 5:
        D_8008A3C8++;
        if (D_8008A3C8 >= 0x708) {
            D_8008A3C8 = 0;
            func_8001313C(0x10, 0, 0x80, 0x7F, 0);
        }
        VSync(0);
        D_8008A3D9 = 7;
        break;
    }
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039140);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039218);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039344);


/**
 * @brief Break (cancel) a CD-ROM read if it has completed.
 *
 * Polls CdSync(1, 0); if the result is 2 (complete), calls CdReadBreak
 * to abort the read and resets the CD state machine to state 0 (idle).
 */
void func_80039388(void) {
    extern u8 D_8008A3D9;
    if (CdSync(1, 0) == 2) {
        CdReadBreak();
        D_8008A3D9 = 0;
    }
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_800393C8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039444);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003947C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800394D0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039520);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039678);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800396E0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039728);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003974C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039764);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003987C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800398A8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039910);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039AA0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039AB4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039B80);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039BC4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039EF8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039FE8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A1F8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A470);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A500);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A534);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A57C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A654);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A700);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A7C4);


/** @brief Wrapper that calls func_80039AA0. */
void func_8003AB64(void) { func_80039AA0(); }


/** @brief Wrapper that calls func_80039AB4. */
void func_8003AB84(void) { func_80039AB4(); }


/** @brief Calls func_80039B80. */
void func_8003ABA4(void) {
    func_80039B80();
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003ABC4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003AC10);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003ACDC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003ADD4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003AEA8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003AF50);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003AF88);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003AFD0);


/**
 * @brief Set a sound voice's data pointer and parameter byte.
 * @param a0 Pointer to the voice control structure.
 * @param a1 Value to store at offset +0x28 (data pointer/address).
 * @param a2 Value to store at offset +0x34 (parameter byte).
 */
void func_8003B024(u8 *a0, s32 a1, s32 a2) {
    *(s32 *)(a0 + 0x28) = a1;
    a0[0x34] = a2;
}


/**
 * @brief Set a sound voice's command, data pointer, and payload size.
 * @param a0 Pointer to the voice control structure.
 * @param a1 Command byte to store at offset +0x37.
 * @param a2 Data pointer to store at offset +0x2C.
 * @param a3 Payload size byte to store at offset +0x36.
 */
void func_8003B030(u8 *a0, s32 a1, s32 a2, s32 a3) {
    a0[0x37] = a1;
    *(s32 *)(a0 + 0x2C) = a2;
    a0[0x36] = a3;
}


/**
 * @brief Dispatch a sound voice action based on the command type at offset +0x46.
 *
 * Reads the command type from a0[0x46] and dispatches:
 * - 2: stop voice (func_8003BB98)
 * - 3: set program from a0[0xE4] (func_8003BBAC)
 * - 4: set volume from a0[0x47] (func_8003BBEC)
 *
 * @param a0 Pointer to the voice control structure.
 */
void func_8003B040(u8 *a0) {
    switch (a0[0x46]) {
        case 2:
            func_8003BB98(a0);
            break;
        case 3:
            func_8003BBAC(a0, a0[0xE4]);
            break;
        case 4:
            func_8003BBEC(a0, a0[0x47]);
            break;
    }
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003B0C4);


/**
 * @brief Calculate the total data size for a sound voice's sequence data.
 *
 * Computes the size from three components at different offsets in the
 * voice structure: a0[0xE3] (part1), a0[0xE9] (part2), and the u16
 * at a0+0xEC (part3). The formula aligns parts to 4-byte boundaries.
 *
 * @param a0 Pointer to the voice control structure.
 * @return Total computed data size in bytes.
 */
s32 func_8003B334(u8 *a0) {
    s32 v0 = a0[0xE3];
    s32 a1val = a0[0xE9];
    u16 ecval = *(u16 *)(a0 + 0xEC);
    s32 part1 = ((v0 + 1) >> 1) << 2;
    s32 part2 = ((a1val * 5 + 3) & 0xFFC) + 4;
    return part1 + part2 + ecval;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003B36C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003B440);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003B4E8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003B8E0);


/** @brief Sets command byte 0x4D, state byte 6, and copies field 0x20 to 0x2C.
 *  @param a0 Pointer to entity structure.
 */
void func_8003B948(u8 *a0) {
    s32 val = *(s32 *)(a0 + 0x20);
    *(u8 *)(a0 + 0x37) = 0x4D;
    *(u8 *)(a0 + 0x36) = 6;
    *(s32 *)(a0 + 0x2C) = val;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003B964);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BA2C);


// snd_voice_cmd_play_note - cmd=0x43, sets payload byte

INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BAC4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BB18);


/**
 * @brief Send a "play note" command (0x43) to a sound voice.
 *
 * Sets the command byte to 0x43, points the data pointer to the
 * voice's own buffer at offset +0x24, writes the note value a1
 * there, and sets the payload size to 1.
 *
 * @param a0 Pointer to the voice control structure.
 * @param a1 Note value to play.
 */
void func_8003BB78(u8 *a0, s32 a1) {
    *(u8 *)(a0 + 0x37) = 0x43;
    *(s32 *)(a0 + 0x2C) = (s32)(a0 + 0x24);
    *(u8 *)(a0 + 0x24) = a1;
    *(u8 *)(a0 + 0x36) = 1;
}


/**
 * @brief Send a "stop" command (0x45) to a sound voice.
 *
 * Sets the command byte to 0x45, clears the data pointer to 0,
 * and sets the payload size to 0.
 *
 * @param a0 Pointer to the voice control structure.
 */
void func_8003BB98(u8 *a0) {
    *(u8 *)(a0 + 0x37) = 0x45;
    *(s32 *)(a0 + 0x2C) = 0;
    *(u8 *)(a0 + 0x36) = 0;
}


/**
 * @brief Send a "set program" command (0x4C) to a sound voice.
 *
 * Sets the command byte to 0x4C, points the data pointer to offset +0x24,
 * writes the program number a1 there, and sets the payload size to 1.
 *
 * @param a0 Pointer to the voice control structure.
 * @param a1 Program/instrument number.
 */
void func_8003BBAC(u8 *a0, s32 a1) {
    *(u8 *)(a0 + 0x37) = 0x4C;
    *(s32 *)(a0 + 0x2C) = (s32)(a0 + 0x24);
    *(u8 *)(a0 + 0x24) = a1;
    *(u8 *)(a0 + 0x36) = 1;
}


/**
 * @brief Send a "set pitch" command (0x46) to a sound voice.
 *
 * Sets the command byte to 0x46, points the data pointer to offset +0x24,
 * writes the pitch value a1 there, and sets the payload size to 1.
 *
 * @param a0 Pointer to the voice control structure.
 * @param a1 Pitch value.
 */
void func_8003BBCC(u8 *a0, s32 a1) {
    *(u8 *)(a0 + 0x37) = 0x46;
    *(s32 *)(a0 + 0x2C) = (s32)(a0 + 0x24);
    *(u8 *)(a0 + 0x24) = a1;
    *(u8 *)(a0 + 0x36) = 1;
}


/**
 * @brief Send a "set volume" command (0x47) to a sound voice.
 *
 * Sets the command byte to 0x47, points the data pointer to offset +0x24,
 * writes the volume value a1 there, and sets the payload size to 1.
 *
 * @param a0 Pointer to the voice control structure.
 * @param a1 Volume level.
 */
void func_8003BBEC(u8 *a0, s32 a1) {
    *(u8 *)(a0 + 0x37) = 0x47;
    *(s32 *)(a0 + 0x2C) = (s32)(a0 + 0x24);
    *(u8 *)(a0 + 0x24) = a1;
    *(u8 *)(a0 + 0x36) = 1;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BC0C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BC24);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BD84);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BDEC);


/** @brief Moves byte at offset 0x37 to offset 0x38 and clears 0x37. */
void func_8003BEE0(u8 *a0) {
    u8 tmp = a0[0x37];
    a0[0x37] = 0;
    a0[0x38] = tmp;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BEF0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BFAC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C228);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C260);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C284);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C2B8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C3C8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C62C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C70C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C744);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C764);
