#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

/**
 * @brief Clear the RGB color bytes at offsets 0x20, 0x21, and 0x22 of a structure.
 * @param a0 Pointer to the base of the structure whose color fields are zeroed.
 */
void func_8002FF24(u8 *a0) {
    a0[0x20] = 0;
    a0[0x22] = 0;
    a0[0x21] = 0;
}


INCLUDE_ASM("asm/nonmatchings/20724", func_8002FF34);


INCLUDE_ASM("asm/nonmatchings/20724", func_80030058);


INCLUDE_ASM("asm/nonmatchings/20724", func_80030094);


INCLUDE_ASM("asm/nonmatchings/20724", func_800300F8);


/** @brief Call func_80030058 with the default parameter value 0x1000. */
void func_80030214(void) { func_80030058(0x1000); }


/** @brief Empty stub -- no operation. */
void func_80030234(void) {
}


extern s16 D_800834D4;
/**
 * @brief Set the global 16-bit value D_800834D4.
 * @param val Value to store.
 */
void func_8003023C(s32 val) {
    D_800834D4 = val;
}


INCLUDE_ASM("asm/nonmatchings/20724", func_80030248);


/** @brief Stores u16 and u8 to adjacent fields of D_800834D0.
 *  @param a0 Value stored as u16.
 *  @param a1 Value stored as u8 at offset 2.
 */
void func_80030274(s32 a0, s32 a1) {
    extern u16 D_800834D0;
    D_800834D0 = a0;
    *((u8 *)&D_800834D0 + 2) = a1;
}


INCLUDE_ASM("asm/nonmatchings/20724", func_80030288);


INCLUDE_ASM("asm/nonmatchings/20724", func_800302DC);


typedef struct {
    s16 f0;
    s8 f2;
    s8 f3;
    s16 f4;
    s8 f6;
    s8 f7;
} BattleCameraState;

extern BattleCameraState D_800834D0;

// init_battle_camera

INCLUDE_ASM("asm/nonmatchings/20724", func_80030518);


/**
 * @brief Reset the BattleCameraState struct (D_800834D0) to default values.
 *
 * Clears all fields to zero except f4, which is set to 0x1000 (default zoom/distance).
 */
void func_80030720(void) {
    D_800834D0.f3 = 0;
    D_800834D0.f4 = 0x1000;
    D_800834D0.f0 = 0;
    D_800834D0.f2 = 0;
    D_800834D0.f7 = 0;
    D_800834D0.f6 = 0;
}


extern u8 D_80083878;

/**
 * @brief Get a pointer to the global byte D_80083878.
 * @return Address of D_80083878.
 */
u8 *func_80030748(void) {
    return &D_80083878;
}

INCLUDE_ASM("asm/nonmatchings/20724", func_80030754);


INCLUDE_ASM("asm/nonmatchings/20724", func_800307F8);


INCLUDE_ASM("asm/nonmatchings/20724", func_80030848);


INCLUDE_ASM("asm/nonmatchings/20724", func_800308B0);


INCLUDE_ASM("asm/nonmatchings/20724", func_8003093C);


INCLUDE_ASM("asm/nonmatchings/20724", func_80030A54);


INCLUDE_ASM("asm/nonmatchings/20724", func_80030B2C);


INCLUDE_ASM("asm/nonmatchings/20724", func_80030CB0);


INCLUDE_ASM("asm/nonmatchings/20724", func_80030CFC);


/**
 * @brief Send an SPU command looked up from table D_80052A34.
 *
 * Uses the input as an index into D_80052A34 to retrieve a command byte,
 * then passes it to func_8001313C which writes it to the SPU command buffer
 * (D_80075058) and triggers SPU processing.
 *
 * @param a0 Index into the D_80052A34 lookup table.
 */
void func_80030D48(s32 a0) {
    extern u8 D_80052A34[];
    func_8001313C(D_80052A34[a0]);
}


/**
 * @brief Play sound effect from D_80052A34 table.
 *
 * Loads a byte from D_80052A34[a0] and calls func_8001302C
 * with default volume/pan parameters (0, 0x80, 0x7F).
 *
 * @param a0 Index into D_80052A34 sound table.
 */
void func_80030D78(s32 a0) {
    extern u8 D_80052A34[];
    u8 *ptr = D_80052A34 + a0;
    func_8001302C(*ptr, 0, 0x80, 0x7F);
}


INCLUDE_ASM("asm/nonmatchings/20724", func_80030DB0);


INCLUDE_ASM("asm/nonmatchings/20724", func_80030E60);


INCLUDE_ASM("asm/nonmatchings/20724", func_80030F10);


INCLUDE_ASM("asm/nonmatchings/20724", func_80030FA0);


INCLUDE_ASM("asm/nonmatchings/20724", func_80030FDC);


/** @brief Empty stub -- no operation. */
void func_80031044(void) {
}


INCLUDE_ASM("asm/nonmatchings/20724", func_8003104C);


INCLUDE_ASM("asm/nonmatchings/20724", func_80031188);


INCLUDE_ASM("asm/nonmatchings/20724", func_80031224);


INCLUDE_ASM("asm/nonmatchings/20724", func_80031364);


typedef struct {
    s16 f0;
    s16 f2;
    s16 f4;
    s16 f6;
    u8 f8;
    u8 f9;
} Struct3754;
extern Struct3754 D_80083754;
// init_battle_transition - initializes D_80083754 (Struct3754)

/** @brief Sets D_80083754.f0 to 7. */
void func_800316C4(void) {
    D_80083754.f0 = 7;
}


INCLUDE_ASM("asm/nonmatchings/20724", func_800316D4);


/** @brief Stores a byte to global D_80083756. */
void func_800318E0(s32 a0) {
    extern u8 D_80083756;
    D_80083756 = a0;
}


/**
 * @brief Initialize the battle transition struct D_80083754 to its starting state.
 *
 * Sets the transition phase to 9, clears the timer fields (f4, f6), and resets
 * the flag bytes (f8, f9) to zero.
 */
void func_800318EC(void) {
    D_80083754.f0 = 9;
    D_80083754.f4 = 0;
    D_80083754.f6 = 0;
    D_80083754.f8 = 0;
    D_80083754.f9 = 0;
}


/**
 * @brief Perform linear interpolation within a range.
 *
 * Returns 0 if a2 < a0 (below range start), a3 if a2 >= a1 (at or above range end),
 * or a proportional value between 0 and a3 based on where a2 falls in [a0, a1).
 *
 * @param a0 Range start (minimum input value).
 * @param a1 Range end (maximum input value).
 * @param a2 Current input value to interpolate.
 * @param a3 Maximum output value (returned when a2 >= a1).
 * @return Interpolated value in [0, a3].
 */
s32 func_80031910(s32 a0, s32 a1, s32 a2, s32 a3) {
    if (a2 < a0) {
        return 0;
    }
    if (a2 >= a1) {
        return a3;
    }
    a2 -= a0;
    a1 -= a0;
    return a2 * a3 / a1;
}


INCLUDE_ASM("asm/nonmatchings/20724", func_80031954);


INCLUDE_ASM("asm/nonmatchings/20724", func_80031A18);


INCLUDE_ASM("asm/nonmatchings/20724", func_80031CDC);


/**
 * @brief Clear bit 7 of animation entry flags.
 *
 * Clears the high bit of the byte at offset 0xE in D_80083772[a0]
 * (stride 16), effectively marking the entry as inactive.
 *
 * @param a0 Entry index into D_80083772.
 */
void func_80031D68(s32 a0) {
    extern u8 D_80083772[];
    s32 base = (s32)D_80083772;
    base = a0 * 16 + base;
    *(u8 *)(base + 0xE) &= 0x7F;
}


INCLUDE_ASM("asm/nonmatchings/20724", func_80031D8C);


/**
 * @brief Copy 4-byte field from source to D_80083772 entry.
 *
 * @param a0 Entry index into D_80083772 (stride 16 bytes).
 * @param src Source for the 4-byte unaligned copy.
 */
void func_80031DF4(s32 a0, void *src) {
    typedef struct { s16 a, b; } S16Pair;
    extern u8 D_80083772[];
    s32 base = (s32)D_80083772;
    base += a0 * 16;
    *(S16Pair *)base = *(S16Pair *)src;
}


INCLUDE_ASM("asm/nonmatchings/20724", func_80031E1C);


extern u8 D_80083772[];
// clear_animation_entries

/**
 * @brief Call func_80031E1C with truncated a1 and constant 7th arg (0x60).
 *
 * Passes through all 6 caller args, truncates a1 to u8,
 * and appends 0x60 as the 7th argument.
 */
void func_80031EBC(s32 a0, u8 a1, s32 a2, s32 a3, s32 arg4, s32 arg5) {
    func_80031E1C(a0, a1, a2, a3, arg4, arg5, 0x60);
}


/**
 * @brief Call func_80031E1C with truncated a1, passing all 7 args through.
 *
 * Truncates a1 to u8 and passes all caller arguments
 * through to func_80031E1C.
 */
void func_80031EF4(s32 a0, u8 a1, s32 a2, s32 a3, s32 arg4, s32 arg5, s32 arg6) {
    func_80031E1C(a0, a1, a2, a3, arg4, arg5, arg6);
}


/**
 * @brief Clear animation entry active flags.
 *
 * Iterates over 2 entries in D_80083772 (stride 0x10) and sets the byte at
 * offset 0x0E to zero for each, marking them as inactive.
 */
void func_80031F2C(void) {
    s32 i;
    u8 *ptr = D_80083772;
    for (i = 1; i >= 0; i--) {
        ptr[0xE] = 0;
        ptr += 0x10;
    }
}


extern u8 D_80085134[];
extern u8 D_80083938[];

/**
 * @brief Get a pointer to the global buffer D_80085134.
 * @return Address of D_80085134.
 */
u8 *func_80031F50(void) {
    return D_80085134;
}

/**
 * @brief Get a pointer to the global buffer D_80083938.
 * @return Address of D_80083938.
 */
u8 *func_80031F5C(void) {
    return D_80083938;
}

/**
 * @brief Spin-wait until a VSync event is received.
 *
 * Polls func_8004D208 (VSync) with argument 1 in a busy loop until it
 * returns a value other than -1, indicating the vertical blank has occurred.
 */
void func_80031F68(void) {
    do {
    } while (func_8004D208(1) == -1);
}


/**
 * @brief Return the base address of the battle allocation region.
 * @return 0x801F4000 (fixed address in PS1 RAM).
 */
u32 func_80031F9C(void) {
    return 0x801F4000;
}


/**
 * @brief Return the size of the battle allocation region.
 * @return 0x4000 (16384 bytes / 16 KB).
 */
s32 func_80031FA8(void) {
    return 0x4000;
}


/**
 * @brief Flip the double-buffered ordering table and initialize the new back buffer.
 *
 * Selects the OT buffer that is not currently active (from D_80083920[0..1]),
 * sets it as the active buffer in D_80083918, clears its 2-entry ordering tag
 * at offset 0x70 via ClearOTag, and initializes the primitive allocation pointer
 * at offset 0x78 to point to offset 0x7C (start of free space).
 */
void func_80031FB0(void) {
    extern s32 D_80083918;
    extern s32 D_80083920[];
    s32 buf;
    s32 ptr;

    buf = D_80083920[0];
    if (D_80083918 == buf) {
        buf = D_80083920[1];
    }
    D_80083918 = buf;
    ClearOTag((u32 *)(buf + 0x70), 2);
    ptr = D_80083918;
    *(s32 *)(ptr + 0x78) = ptr + 0x7C;
}


INCLUDE_ASM("asm/nonmatchings/20724", func_80032010);


INCLUDE_ASM("asm/nonmatchings/20724", func_800320BC);


INCLUDE_ASM("asm/nonmatchings/20724", func_80032204);


