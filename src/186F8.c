#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

extern BattleAnimEntity g_battleAnims[];

void func_800286FC(void);
void func_8002A090(void);
void func_8002A128(void);

/**
 * @brief Set or clear opacity of a battle animation entity.
 * @param idx Entity index (masked to 0 or 1).
 * @param val If nonzero, set to 0xFF (visible); otherwise 0 (hidden).
 */
void func_80027EF8(s32 idx, s32 val) {
    BattleAnimEntity *entry = &g_battleAnims[idx & 1];
    if (val != 0) {
        entry->opacity = 0xFF;
    } else {
        entry->opacity = 0;
    }
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_80027F38);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80027F78);


/**
 * @brief Store two u16 values into global state area beyond g_battleAnims entries.
 * @param idx Entity index (masked to 0 or 1).
 * @param a1 Value stored at offset 0x1D0.
 * @param a2 Value stored at offset 0x1D2.
 * @note Offsets 0x1D0+ are global state beyond the 2 BattleAnimEntity entries.
 */
void func_80027FBC(s32 idx, s16 a1, s16 a2) {
    s32 base;
    idx &= 1;
    base = (s32)g_battleAnims;
    *(u16 *)(base + idx * 4 + 0x1D0) = a1;
    *(u16 *)(base + idx * 4 + 0x1D2) = a2;
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_80027FDC);


/**
 * @brief Checks if all battle animation entities are inactive.
 * @return 1 if all inactive (func_800280C0(0, 0) returns -1), 0 otherwise.
 */
s32 func_80028098(void) {
    return (u32)~func_800280C0(0, 0) >> 31;
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_800280C0);


INCLUDE_ASM("asm/nonmatchings/186F8", func_800281A4);


INCLUDE_ASM("asm/nonmatchings/186F8", func_800281C4);


/**
 * @brief Initialize a battle entity's color fields from the global default.
 *
 * Copies the byte at g_battleAnims+0x1E0 into the entity's color fields
 * at offsets +0x0C through +0x0F (RGBX), then calls func_800281C4 to
 * perform further entity initialization.
 *
 * @param a0 Entity index (stride 196 into g_battleAnims).
 */
void func_8002828C(s32 a0) {
    s32 off = a0 * 196;
    s32 base = (s32)g_battleAnims;
    u8 *entry = (u8 *)(off + base);
    entry[0xC] = *(u8 *)(base + 0x1E0);
    entry[0xD] = *(u8 *)(base + 0x1E0);
    entry[0xE] = *(u8 *)(base + 0x1E0);
    entry[0xF] = *(u8 *)(base + 0x1E0);
    func_800281C4(a0, 0);
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_800282F4);


extern u16 D_80083794;
// get D_80083794 (u16)

/** @brief Initializes D_80082FB2 to 0, calls func_80039764(0), then loops twice calling func_800281C4(i, 0). */
void func_8002837C(void) {
    extern u8 D_80082FB2;
    s32 i;
    D_80082FB2 = 0;
    func_80039764(0);
    for (i = 0; i < 2; i++) {
        func_800281C4(i, 0);
    }
}


/** @brief Get the current value of D_80083794 (global u16 state variable). */
u16 func_800283CC(void) {
    return D_80083794;
}


extern u8 D_80082FB3;

/**
 * @brief Set D_80083794 to a new value.
 * @param a0 Value to store.
 * @return The value that was set.
 */
s32 func_800283DC(s32 a0) {
    D_80083794 = a0;
    return a0;
}


/**
 * @brief Set the global flag D_80082FB3.
 * @param a0 Value to store in D_80082FB3.
 */
void func_800283EC(s32 a0) {
    D_80082FB3 = a0;
}


/**
 * @brief Initialize or reset the CD audio/streaming subsystem state.
 * @note Calls several initialization functions and resets a counter at g_battleAnims + 0x9C4 to 0.
 *       Passes two g_battleAnims buffer pointers (offsets 0x188 and 0x1AC) to func_8003BC24.
 */
void func_800283F8(void) {
    u8 *base = (u8 *)g_battleAnims;
    func_800982B8();
    func_8003BC24(base + 0x188, base + 0x1AC);
    func_8003AB84();
    func_800282F4();
    *(s16 *)(base + 0x9C4) = 0;
}


/**
 * @brief Initialize GPU display and clear battle animation state fields.
 *
 * Calls GsInitGraph (func_800982D8), GsDefDispBuff (func_800980D0), and
 * func_800283F8. Then clears g_battleAnims fields at +0x6FC, +0x9C8,
 * +0x9CC to zero and sets +0x9C2 to 0x4611.
 *
 * @note PsyQ 4.3 function (FILLED epilogue) — cannot be decomped from
 *       this file (compiled with PsyQ 4.1).
 */
INCLUDE_ASM("asm/nonmatchings/186F8", func_80028444);


/** @brief Calls func_8002A128, func_800286FC, and func_8002A090 in sequence. */
void func_8002848C(void) {
    func_8002A128();
    func_800286FC();
    func_8002A090();
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_800284BC);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80028564);


/** @brief Wrapper that calls func_80050BC4. */
void func_800286BC(void) {
    func_80050BC4();
}


/** @brief Wrapper that calls func_8004E720. */
void func_800286DC(void) {
    func_8004E720();
}


/** @brief Wrapper that calls func_8004DFF4 (likely a CD subsystem tick or finalization). */
void func_800286FC(void) { func_8004DFF4(); }


/**
 * @brief Copy a null-terminated string from src to dst (strcpy implementation).
 * @param dst Destination buffer.
 * @param src Source null-terminated string.
 */
void func_8002871C(u8 *dst, u8 *src) {
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
void func_80028738(u8 *dst, u8 *src) {
    do { } while (*dst++);
    func_8002871C(dst - 1, src);
}


/**
 * @brief Copy a block of bytes from src to dst (memcpy implementation).
 * @param src Source buffer.
 * @param dst Destination buffer.
 * @param numBytes Number of bytes to copy.
 */
void func_80028768(u8 *src, u8 *dst, s32 numBytes) {
    while (numBytes > 0) {
        *dst++ = *src++;
        numBytes--;
    }
}


/**
 * @brief Wait for vertical sync (VSync wrapper).
 * @param a0 Unused parameter.
 */
void func_80028790(s32 a0) { VSync(); }


extern s8 D_80082FD4;
/**
 * @brief Set the global flag D_80082FD4.
 * @param val Value to store in D_80082FD4.
 */
void func_800287B0(s8 val) {
    D_80082FD4 = val;
}


/**
 * @brief Encode a memory card port and slot into a single byte.
 * @param a0 Memory card port number (reduced mod 2: 0 or 1).
 * @param a1 Memory card slot number (reduced mod 4: 0-3).
 * @return Packed value: (port << 4) | slot.
 */
s32 func_800287BC(s32 a0, s32 a1) {
    a1 %= 4;
    a0 %= 2;
    return (a0 << 4) | a1;
}


/**
 * @brief Extract the memory card port number from a packed card identifier.
 * @param a0 Packed card identifier (port in bit 4).
 * @return Port number (0 or 1).
 */
s32 func_800287F4(s32 a0) {
    return (a0 >> 4) % 2;
}


/**
 * @brief Extract the memory card slot number from a packed card identifier.
 * @param a0 Packed card identifier (slot in lower 2 bits).
 * @return Slot number (0-3).
 */
s32 func_80028810(s32 a0) {
    return a0 % 4;
}


/**
 * @brief Wait for a memory card operation to complete on the specified port.
 * @param a0 Packed card identifier; port is extracted via func_800287F4.
 */
void func_8002882C(s32 a0) {
    _card_wait(func_800287F4(a0));
}


/**
 * @brief Test a BIOS event (wrapper for PsyQ TestEvent).
 * @param a0 Event descriptor to test.
 * @return Nonzero if the event has been delivered.
 */
s32 func_80028854(s32 a0) { TestEvent(a0); }


/**
 * @brief Poll the first 4 memory card events and return which one fired.
 * @return Event index 0-3 if an event was delivered, or -1 if none fired.
 * @note Tests events from D_80082FB4 array entries [0..3] in order.
 */
s32 func_80028874(void) {
    extern u8 D_80082FB4[];
    s32 *s0 = (s32 *)((s32)D_80082FB4);
    if (func_80028854(s0[0])) return 0;
    if (func_80028854(s0[1])) return 1;
    if (func_80028854(s0[2])) return 2;
    if (func_80028854(s0[3])) return 3;
    return -1;
}


/**
 * @brief Wait up to 180 frames (0xB4) for a memory card event, polling each VSync.
 * @return Event index 0-3 if an event fires, or 2 (timeout) if none fires within the limit.
 */
s32 func_800288F8(void) {
    s32 i;
    s32 result;
    for (i = 0; i < 0xB4; i++) {
        result = func_80028874();
        if (result != -1) return result;
        func_80028790(0);
    }
    return 2;
}


/** @brief Poll memory card events once (discards result). */
void func_80028950(void) { func_80028874(); }


/**
 * @brief Poll the second set of 4 memory card events and return which one fired.
 * @return Event index 0-3 if an event was delivered, or -1 if none fired.
 * @note Tests events from D_80082FB4 array entries [4..7] in order.
 */
s32 func_80028970(void) {
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
 * @note Unlike func_800288F8, this does not VSync between polls (tight busy-wait).
 */
s32 func_800289F8(void) {
    s32 i;
    s32 result;
    for (i = 0; i < 0x4000; i++) {
        result = func_80028970();
        if (result != -1) return result;
    }
    return 2;
}


/**
 * @brief Read a memory card status byte for a given port and slot.
 * @param a0 Packed card identifier (port in bit 4, slot in lower bits).
 * @return Status byte from D_80082FB4 at offset 0x2C + port*4 + slot.
 */
u8 func_80028A44(a0)

s32 a0;
{
    extern u8 D_80082FB4[];
    s32 base = (s32)D_80082FB4;
    s32 r1 = func_800287F4(a0);
    s32 r2 = func_80028810(a0);
    return *(u8 *)(r1 * 4 + base + r2 + 0x2C);
}


/**
 * @brief Write a memory card status byte for a given port and slot.
 * @param a0 Packed card identifier (port in bit 4, slot in lower bits).
 * @param a1 Status value to write.
 */
void func_80028A98(s32 a0, u8 a1) {
    extern u8 D_80082FB4[];
    s32 base = (s32)D_80082FB4;
    s32 r1 = func_800287F4(a0);
    s32 r2 = func_80028810(a0);
    *(u8 *)(r1 * 4 + base + r2 + 0x2C) = a1;
}


/**
 * @brief Write a secondary memory card status byte for a given port and slot.
 * @param a0 Packed card identifier (port in bit 4, slot in lower bits).
 * @param a1 Status value to write at offset 0x34 (secondary status table).
 */
void func_80028AF8(s32 a0, u8 a1) {
    extern u8 D_80082FB4[];
    s32 base = (s32)D_80082FB4;
    s32 r1 = func_800287F4(a0);
    s32 r2 = func_80028810(a0);
    *(u8 *)(r1 * 4 + base + r2 + 0x34) = a1;
}


/** @brief Mark a memory card slot as busy/active (set status byte to 1). */
void func_80028B58(s32 a0) { func_80028A98(a0, 1); }


/** @brief Read memory card status (wrapper, discards result). */
void func_80028B78(void) { func_80028A44(); }


INCLUDE_ASM("asm/nonmatchings/186F8", func_80028B98);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80028CB4);


/**
 * @brief Initialize the memory card data block at D_80082FB4.
 *
 * Clears byte at +0x21, fills 2 sets of 4 consecutive bytes starting at
 * +0x27 (then +0x2B) with the value 2, then calls func_80028CB4.
 *
 * @note Non-matching: register allocation differs — original keeps base
 *       address in $v0 for initial store then copies to $a0 for loop;
 *       compiler loads directly into $a0. Also const 2 is placed before
 *       the loop in original but inside in ours (1 instruction shorter).
 */
INCLUDE_ASM("asm/nonmatchings/186F8", func_80028D20);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80028D80);


/**
 * @brief Attempt to load a memory card save, retrying up to 180 times.
 * @param a0 Packed card identifier.
 * @return 0 on success, 2 if new card detected, 3 on timeout, 4 on other failure.
 * @note Waits for the card, issues _card_load, then polls events. Updates card status
 *       bytes based on the result.
 */
s32 func_80028F3C(s32 a0) {
    s32 counter;

    counter = 0;
    do {
        func_8002882C(a0);
        if (_card_load(a0) != 0) {
            s32 result = func_800288F8();
            switch (result) {
            case 0:
                func_80028AF8(a0, 0);
                func_80028A98(a0, 0);
                return 0;
            case 3:
                func_80028AF8(a0, 1);
                func_80028B58(a0);
                return 2;
            case 2:
                func_80028B58(a0);
                return 3;
            default:
                func_80028B58(a0);
                return 4;
            }
        }
        counter++;
    } while (counter < 0xB4);
    return 4;
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029028);


INCLUDE_ASM("asm/nonmatchings/186F8", func_800290C0);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029150);


INCLUDE_ASM("asm/nonmatchings/186F8", func_800291FC);


INCLUDE_ASM("asm/nonmatchings/186F8", func_800292AC);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029360);


/** @brief Returns the signed byte value at D_80082FD4. */
s32 func_800293F0(void) {
    extern s8 D_80082FD4;
    return D_80082FD4;
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029400);


/** @brief Wrapper that calls func_80029400. */
void func_8002947C(void) {
    func_80029400();
}


/**
 * @brief Close a file descriptor if it is valid (not -1).
 * @param a0 File descriptor to close.
 */
void func_8002949C(s32 a0) {
    if (a0 != -1) {
        close(a0);
    }
}


/** @brief Wrapper for func_8002949C (close a file descriptor if valid). */
void func_800294C4(s32 a0) {
    func_8002949C(a0);
}


/**
 * @brief Open the first file on a memory card matching the given parameters.
 * @param a0 Packed card identifier.
 * @param a1 File index or name parameter.
 * @param a2 Directory entry buffer for firstfile result.
 * @return 0 if card initialization failed, otherwise the result of firstfile.
 * @note Calls func_80028CB4 for setup, checks card status via func_80029150, builds
 *       the filename via func_80029360, then calls PsyQ firstfile.
 */
s32 func_800294E4(s32 a0, s32 a1, s32 a2) {
    s32 buf[8];
    func_80028CB4();
    if (func_80029150(a0) != 0) return 0;
    func_80029360(a0, a1, (s32)buf);
    firstfile((s32)buf, a2);
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029550);


/** @brief Advance to the next file in the memory card directory listing (PsyQ nextfile wrapper). */
void func_800295D0(void) { nextfile(); }


INCLUDE_ASM("asm/nonmatchings/186F8", func_800295F0);


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
s32 func_80029660(s32 a0, s32 a1) {
    s32 buf[10];
    return func_80029550(a0, a1, buf) != 0;
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029680);


/**
 * @brief Seek to an offset in a file and write data.
 * @param a0 File descriptor.
 * @param a1 Pointer to data buffer to write.
 * @param a2 Number of bytes to write.
 * @param a3 File offset to seek to before writing.
 * @return Number of bytes written on success, -1 if the seek failed.
 */
s32 func_80029778(s32 a0, s32 a1, s32 a2, s32 a3) {
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
s32 func_800297E4(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result = lseek(a0, a3, 0);
    if (result == -1) return -1;
    return read(a0, a1, a2);
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029850);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029A20);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029BA0);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029C44);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029CB8);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029D38);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029DAC);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029E40);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029EE4);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029F5C);


INCLUDE_ASM("asm/nonmatchings/186F8", func_80029FDC);


/**
 * @brief Shut down the memory card subsystem by closing all 8 card events.
 * @note Disables interrupts via func_800472E4, closes all events in D_80082FB4[0..7],
 *       re-enables interrupts, then calls func_8004D968 for final cleanup.
 */
void func_8002A090(void) {
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


/** @brief Calls func_80027448 and func_8003ABA4 in sequence. */
void func_8002A128(void) {
    func_80027448();
    func_8003ABA4();
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_8002A150);


/**
 * @brief Apply a transformation or encoding to a value if a global flag is set.
 * @param a0 First parameter (context or key).
 * @param a1 Value to potentially transform; returned unchanged if D_80052958 is 0.
 * @return Transformed a1 if D_80052958 is nonzero, otherwise the original a1.
 * @note When active, calls func_8002E8DC with parameters from D_800837A0/AC/AE globals,
 *       then applies func_8002A45C to the result.
 */
s32 func_8002A238(s32 a0, s32 a1) {
    extern u8 D_80052958;
    extern u8 D_800837A0[];
    extern s16 D_800837AC;
    extern u8 D_800837AE;
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
 * @note This is a second copy of the strcpy function (see also func_8002871C).
 */
void func_8002A2A8(u8 *dst, u8 *src) {
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
 * @note This is a second copy of the strcat function (see also func_80028738).
 */
void func_8002A2C4(u8 *dst, u8 *src) {
    do { } while (*dst++);
    func_8002A2A8(dst - 1, src);
}


/**
 * @brief Count the number of non-zero bytes from a0 until a null terminator.
 * @param a0 Pointer to null-terminated byte string.
 * @return Number of bytes before the null terminator (string length).
 */
s32 func_8002A2F4(u8 *a0) {
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
void func_8002A318(u8 *src, u8 *dst, s32 n) {
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
void func_8002A340(u8 *src, u8 *dst, s32 n) {
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
 *
 * @note Non-matching: compiler loop-inverts the blez pre-check + bgtz do-while
 *       into j-to-check + bgtz (1 instruction longer). Also inverts beq
 *       (skip over mismatch return) to bne (jump to mismatch at end).
 */
INCLUDE_ASM("asm/nonmatchings/186F8", func_8002A36C);


/** @brief Wrapper for func_80020E4C. */
void func_8002A3A8(void) { func_80020E4C(); }


/** @brief Wrapper for func_80020E4C (duplicate of func_8002A3A8). */
void func_8002A3C8(void) { func_80020E4C(); }


/** @brief Wrapper for func_80020DB8. */
void func_8002A3E8(void) { func_80020DB8(); }


/**
 * @brief Copy 8-byte display field from *D_8005F134 to destination.
 *
 * @param dst Destination for the 8-byte unaligned copy (RECT-sized).
 */
void func_8002A408(RECT *dst) {
    extern u8 *D_8005F134;
    RECT *src = (RECT *)D_8005F134;
    *dst = *src;
}


extern u8 *D_8005F134;
/**
 * @brief Copy display coordinates (x, y) from the global display struct D_8005F134.
 * @param a0 Destination buffer; receives x at offset 0 and y at offset 2 (both u16).
 */
void func_8002A438(u8 *a0) {
    u8 *p = D_8005F134;
    *(u16 *)(a0 + 0) = *(u16 *)(p + 8);
    *(u16 *)(a0 + 2) = *(u16 *)(p + 0xA);
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_8002A45C);


/**
 * @brief Swap the active display list buffer and clear the new OT.
 *
 * Toggles between two display list buffers at g_battleAnims+0x640 and
 * g_battleAnims+0x698. Stores the new active buffer at +0x6F0, clears
 * its ordering table (18 entries), and copies the GPU packet pointer
 * from offset +0x54 to offset +0x00.
 */
void func_8002A528(void) {
    s32 base = (s32)g_battleAnims;
    s32 cur = *(s32 *)(base + 0x6F0);
    s32 newBuf = base + 0x640;
    s32 v1;
    if (cur == newBuf) {
        newBuf = base + 0x698;
    }
    *(s32 *)(base + 0x6F0) = newBuf;
    ClearOTag((u32 *)(newBuf + 8), 0x12);
    v1 = *(s32 *)(base + 0x6F0);
    *(s32 *)v1 = *(s32 *)(v1 + 0x54);
}

/**
 * @brief Swap the active display list buffer and clear the new OT (duplicate).
 *
 * Identical logic to func_8002A528 — toggles display list buffers,
 * clears OT, copies GPU packet pointer.
 */
void func_8002A588(void) {
    s32 base = (s32)g_battleAnims;
    s32 cur = *(s32 *)(base + 0x6F0);
    s32 newBuf = base + 0x640;
    s32 v1;
    if (cur == newBuf) {
        newBuf = base + 0x698;
    }
    *(s32 *)(base + 0x6F0) = newBuf;
    ClearOTag((u32 *)(newBuf + 8), 0x12);
    v1 = *(s32 *)(base + 0x6F0);
    *(s32 *)v1 = *(s32 *)(v1 + 0x54);
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_8002A5E8);


extern s32 D_800834C0;

/** @brief Calls func_8002A5E8(a0, 0) then func_80030CB0(a0).
 *  @param a0 Parameter passed to both calls.
 */
void func_8002A834(s32 a0) {
    func_8002A5E8(a0, 0);
    func_80030CB0(a0);
}


/** @brief Calls func_8002A5E8(a0, 1).
 *  @param a0 First parameter.
 */
void func_8002A868(s32 a0) {
    func_8002A5E8(a0, 1);
}


/**
 * @brief Read the first s32 word from the structure pointed to by D_800834C0.
 * @return Value at *D_800834C0.
 */
s32 func_8002A888(void) {
    return *(s32 *)D_800834C0;
}


/**
 * @brief Read an s32 value at offset 0x54 from the structure pointed to by D_800834C0.
 * @return Value at D_800834C0 + 0x54.
 */
s32 func_8002A8A0(void) {
    return *(s32 *)(D_800834C0 + 0x54);
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
void func_8002A8B8(s32 pkt) {
    extern u8 D_800101D0[];
    s32 base = (s32)g_battleAnims;
    s32 limit;
    *(s32 *)*(s32 *)(base + 0x6F0) = pkt;
    base = *(s32 *)(base + 0x6F0);
    limit = *(s32 *)(base + 4);
    if ((u32)limit < (u32)pkt) {
        if ((u32)pkt <= 0x801AFFFFU) {
            printf((char *)D_800101D0, (u32)pkt - (u32)limit);
        }
    }
}


/** @brief Returns the value of D_800834C0 plus 8. */
s32 func_8002A91C(void) {
    extern s32 D_800834C0;
    return D_800834C0 + 8;
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_8002A92C);


INCLUDE_ASM("asm/nonmatchings/186F8", func_8002AA18);


INCLUDE_ASM("asm/nonmatchings/186F8", func_8002AAC0);


INCLUDE_ASM("asm/nonmatchings/186F8", func_8002AB5C);


extern BattleDisplayEntity g_battleEntities[];

/**
 * @brief Get a pointer to a battle entity by index.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @return Pointer to the entity.
 */
BattleDisplayEntity *func_8002AC74(s32 idx) {
    return &g_battleEntities[idx];
}

/**
 * @brief Set a battle entity's animation speed, clamped to [3, 11].
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @param val Value to set; clamped to minimum 3 and maximum 11.
 */
void func_8002AC88(s32 idx, s32 val) {
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
s32 func_8002ACBC(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->animSpeed;
}


/**
 * @brief Store a byte value into a battle entity's subFields array.
 * @param idx Entity index (stride 64 bytes).
 * @param offset Index into the subFields array (0 or 1).
 * @param val Byte value to store.
 */
void func_8002ACD8(s32 idx, s32 offset, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->subFields[offset] = val;
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_8002ACF4);


/**
 * @brief Set a battle entity's bounding rectangle.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @param src Source RECT to copy.
 */
void func_8002AD04(s32 idx, RECT *src) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->boundRect = *src;
}


INCLUDE_ASM("asm/nonmatchings/186F8", func_8002AD3C);


// battle_entity_get_field_38 - g_battleEntities stride 64

/**
 * @brief Get a battle entity's bounding rectangle.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @param dst Destination RECT to copy into.
 */
void func_8002ADA4(s32 idx, RECT *dst) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    *dst = entity->boundRect;
}


/**
 * @brief Get a battle entity's display rectangle.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @param dst Destination RECT to copy into.
 */
void func_8002ADDC(s32 idx, RECT *dst) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    *dst = entity->dispRect;
}


/**
 * @brief Get a battle entity's entity type.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @return Entity type value.
 */
s32 func_8002AE14(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->entityType;
}
