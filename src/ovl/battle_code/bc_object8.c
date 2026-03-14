#include "common.h"

extern u8 D_800ED148[];
extern u8 D_800EEED8[];
void func_800B304C();
extern u8 D_8007DADB[];
extern u8 D_800EE42C[];
extern u8 D_800EEEC4[];
extern u8 D_800E3D0C[];
extern u8 D_800EF4A4[];
extern u8 D_800EEEC8[];
extern u8 D_800EEECC[];
extern u8 D_800EEEB8[];
extern u8 D_800EEEBC[];
extern u8 D_800EEEC0[];
extern u8 D_80077378[];
extern u8 D_800EE9E8[];
void func_800B3164(void);
void func_800B2F3C(void);
extern u8 D_8007809A[];
extern u8 D_800EE45C[];
extern u8 D_800EEDD8[];
extern u8 D_800EEDE8[];
extern u8 D_800EE465[];
extern u8 D_80082C11[];
extern u8 D_8005F388[];
extern u8 D_80063388[];
extern u8 D_80082C08[];
extern u8 D_800EF2D0[];
extern u8 D_800EF020[];
extern u8 D_800EEFB0[];
extern u8 D_800EF724[];
extern u8 D_800EE454[];
extern u8 D_800EE4C0[];
s32 func_800AE788(void);
s32 func_800AA4E0(void);
extern u8 D_800EEED0[];
extern u8 D_800EEED4[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1624);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B16C0);

/**
 * @brief Conditionally trigger entity action based on D_800ED148 flags.
 *
 * If D_800ED148[0x130C] is non-zero or D_800ED148[0x1326] is zero,
 * returns immediately. Otherwise calls func_800A97FC to get a value,
 * passes it to func_800B0754, and clears D_800ED148[0x1326].
 *
 * @param a0 Entity parameter for func_800A97FC and func_800B0754.
 */
void func_800B17B8(s32 a0) {
    u8 *base = D_800ED148;
    if (base[0x130C] != 0) {
        return;
    }
    if (base[0x1326] == 0) {
        return;
    }
    {
        s32 val = func_800A97FC(a0);
        func_800B0754(a0, 0, 0xA, (u16)val);
        base[0x1326] = 0;
    }
}

/**
 * @brief Check GF compatibility bytes and trigger special attack if matched.
 *
 * If D_800EE454 is zero and D_800EE4C0 bytes at +1 and +3 match 0xF4 and
 * 0x1F respectively, calls func_800AA4E0 to get a value, sets bit 0x4000,
 * and dispatches via func_800B0754.
 *
 * @param a0 Entity index.
 */
void func_800B1828(s32 a0) {
    if (*(u8 *)D_800EE454 != 0) {
        return;
    }
    {
        u8 *data = D_800EE4C0;
        if (data[1] != 0xF4) {
            return;
        }
        if (data[3] != 0x1F) {
            return;
        }
    }
    {
        s32 val = func_800AA4E0();
        val |= 0x4000;
        func_800B0754(a0, 0, 8, (u16)val);
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B18A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1930);

/**
 * @brief Get entity from func_800AE6F8 and call func_800A59AC with mode 7.
 */
void func_800B1A48(void) {
    s32 val = func_800AE6F8();
    func_800A59AC(val, 7, 0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1A78);

/**
 * @brief Check battle conditions and trigger entity action sequence.
 *
 * Checks bit 1 of D_8007809A. If set, calls func_800B1A78 to validate.
 * If valid, calls func_8009B79C(0x20, 0xFF) to test entity availability.
 * If available, clears D_800EE45C and calls func_800B1A48 to start action.
 */
void func_800B1ACC(void) {
    if (!(*(u8 *)D_8007809A & 2)) {
        return;
    }
    if (func_800B1A78() == 0) {
        return;
    }
    if (func_8009B79C(0x20, 0xFF) == 0) {
        return;
    }
    *(u8 *)D_800EE45C = 0;
    func_800B1A48();
}

/**
 * @brief Determine quadrant index from a rotation value.
 *
 * Calls func_8009B15C to get a rotation value, then maps it to a quadrant:
 * 0-63 → 0, 64-127 → 1, 128-191 → 2, 192+ → 3.
 *
 * @return Quadrant index (0-3).
 */
s32 func_800B1B1C(void) {
    s32 val = func_8009B15C();
    if (val < 0x40) {
        return 0;
    }
    if (val < 0x80) {
        return 1;
    }
    if (val < 0xC0) {
        return 2;
    }
    return 3;
}

/**
 * @brief Process battle entity: compute type, store to D_800ED148[0x1314],
 *        trigger entity action, then set flag at D_800ED148[0x131D].
 */
void func_800B1B68(void) {
    s32 val = func_800B1B1C();
    u8 *base = D_800ED148;
    base[0x1314] = val + 7;
    func_800B1A48();
    base[0x131D] = 1;
}

/**
 * @brief Store battle type and value, then trigger entity action mode 8.
 *
 * @param a0 Type byte to store at D_800ED148[0x1314].
 * @param a1 Value to store at D_800ED148[0x12E6] as halfword.
 */
void func_800B1BA8(s32 a0, s32 a1) {
    u8 *base = D_800ED148;
    base[0x1314] = a0;
    *(u16 *)(base + 0x12E6) = a1;
    func_800A59AC(func_800AE6F8(), 8, 0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1BE4);

/**
 * @brief Call func_800B0754 with rearranged parameters.
 *
 * @param a0 First parameter (passed through as a0).
 * @param a1 Becomes a2 in the callee.
 * @param a2 Masked to 16 bits, becomes a3 in the callee.
 */
void func_800B1C3C(s32 a0, s32 a1, s32 a2) {
    func_800B0754(a0, 0xF0, a1, (u16)a2);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1C68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1D4C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1DFC);

/**
 * @brief Copy a byte from D_8007DADB to D_800EE42C as a halfword.
 */
void func_800B2024(void) {
    *(u16 *)D_800EE42C = *(u8 *)D_8007DADB;
}

/**
 * @brief Decrement entity timer at D_800ED148+0x12E4 or trigger completion.
 *
 * If the halfword timer at D_800ED148+0x12E4 is zero, calls func_800B1DFC
 * and func_800B2024 to handle completion. Otherwise decrements the timer.
 */
void func_800B2038(void) {
    s32 base = (s32)D_800ED148;
    u16 val = *(u16 *)(base + 0x12E4);
    if (val == 0) {
        func_800B1DFC();
        func_800B2024();
    } else {
        *(u16 *)(base + 0x12E4) = val - 1;
    }
}

/**
 * @brief Check battle mode flag and conditionally trigger entity processing.
 *
 * Checks bit 3 of D_8007809A. If set, calls func_8009B79C(8, 0xFF) to test
 * entity availability. If available, calls func_800B1B68 and returns.
 * Otherwise (bit not set or entity unavailable), clears D_800EE465.
 */
void func_800B2084(void) {
    if (*(u8 *)D_8007809A & 8) {
        if (func_8009B79C(8, 0xFF) != 0) {
            func_800B1B68();
            return;
        }
    }
    *(u8 *)D_800EE465 = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B20D8);

/**
 * @brief Check if conditions are met to initiate an auto-battle action.
 *
 * Checks a chain of conditions: whether func_800AE788 returns the sentinel
 * 0xFF, whether func_800B20D8 indicates busy, bit 2 of D_8007809A flags,
 * whether D_80082C08 matches 0x13D, and whether entity slot 0x40 is
 * available via func_8009B79C. If all pass, sets D_800EE45C to 1 and
 * calls func_800B1A48 to start the action.
 *
 * @return 1 if action was initiated, 0 otherwise.
 */
s32 func_800B2128(void) {
    if (func_800AE788() == 0xFF) {
        return 0;
    }
    if (func_800B20D8() != 0) {
        return 0;
    }
    if (*(u8 *)D_8007809A & 4) {
        if (*(u16 *)D_80082C08 != 0x13D) {
            if (func_8009B79C(0x40, 0xFF) != 0) {
                *(u8 *)D_800EE45C = 1;
                func_800B1A48();
                return 1;
            }
        }
    }
    return 0;
}

/**
 * @brief Copy 3 bytes from D_80077378 to D_800EE9E8 at stride 0x47.
 *
 * Copies bytes from D_80077378[i+0xAF4] to D_800EE9E8[i*0x47+0xA3]
 * for i = 0, 1, 2.
 */
void func_800B21B4(void) {
    s32 i = 0;
    u8 *src = D_80077378;
    u8 *dst = D_800EE9E8;
    do {
        dst[0xA3] = src[i + 0xAF4];
        i++;
        dst += 0x47;
    } while (i < 3);
}

/**
 * @brief Search D_800EE9E8 for an entry matching a given value.
 *
 * Scans 3 entries at stride 0x47 in D_800EE9E8. If byte at offset 0xA3
 * matches a0, returns 0 (found). Returns 1 if no match found.
 *
 * @param a0 Value to search for.
 * @return 0 if found, 1 if not found.
 *
 * @note Non-matching: CC1PSX puts loop increment in beq delay slot
 * instead of return value setup, generating a separate return block
 * (16 instructions vs 14 in original).
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B21EC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2224);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B228C);

/**
 * @brief Process current battle entity: update effects and activate ability.
 *
 * Reads the entity index from D_800ED148[0x1301], calls func_800AE6C0
 * and func_800D3090 with it, then calls func_800D0530 and func_800AB3C4.
 */
void func_800B2338(void) {
    u8 *base = D_800ED148;
    func_800AE6C0(base[0x1301]);
    func_800D3090(base[0x1301], 1);
    func_800D0530();
    func_800AB3C4();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2388);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B243C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B24C8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B25E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B26B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B27AC);

/**
 * @brief Advance a two-phase process based on state byte at offset 0xD.
 *
 * State 0: Calls func_800B3128 with data at offset 0xE, increments state.
 * State 1: If byte at 0xE is non-zero, writes 0xFF to target+1 and returns 2.
 * Otherwise returns 0.
 *
 * @param a0 Pointer to process state structure.
 * @return 0 if still in progress, 2 if complete.
 */
s32 func_800B2848(u8 *a0) {
    u8 state = a0[0xD];
    s32 target = *(s32 *)(a0 + 0x10);

    if (state == 0) goto case0;
    if (state == 1) goto case1;
    goto ret0;

case0:
    func_800B3128(a0 + 0xE);
    a0[0xD] = a0[0xD] + 1;
    goto ret0;

case1:
    if (a0[0xE] == 0) goto ret0;
    *(u8 *)(target + 1) = 0xFF;
    return 2;

ret0:
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B28C8);

/**
 * @brief Initialize a table header and zero its entries.
 *
 * Sets up a table header at a0 with pointer to data, entry stride,
 * and entry count. Then zeros the first halfword of each entry.
 *
 * @param a0 Pointer to table header.
 * @param data Pointer to table data.
 * @param stride Byte stride between entries.
 * @param count Number of entries to zero.
 */
void func_800B2A00(u8 *a0, u8 *data, s32 stride, s32 count) {
    s32 i = 0;
    *(s32 *)a0 = 0;
    *(s32 *)(a0 + 4) = 0;
    *(s32 *)(a0 + 8) = (s32)data;
    *(u16 *)(a0 + 0xC) = stride;
    *(u16 *)(a0 + 0xE) = count;
    if (count > 0) {
        do {
            *(u16 *)data = 0;
            i++;
            data += stride;
        } while (i < count);
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2A38);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2A84);

/**
 * @brief Allocate a node, initialize it, and prepend to linked list.
 *
 * Allocates a node via func_800B2A38, sets bit 0 of its flags,
 * clears halfword at offset 2, stores callback at offset 8,
 * and prepends the node to the linked list at a0.
 *
 * @param list Pointer to linked list head pointer.
 * @param callback Value to store at node offset 8.
 * @return Pointer to the new node, or NULL if allocation failed.
 */
s32 func_800B2B00(u8 *list, s32 callback) {
    u8 *node = (u8 *)func_800B2A38(list);
    if (node != 0) {
        u16 flags = *(u16 *)node;
        *(u16 *)(node + 2) = 0;
        *(s32 *)(node + 8) = callback;
        *(u16 *)node = flags | 1;
        *(s32 *)(node + 4) = *(s32 *)list;
        *(s32 *)list = (s32)node;
    }
    return (s32)node;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2B68);

extern u8 D_800EEC68[];
extern u8 D_800EEDC8[];

/**
 * @brief Initialize D_800EEDC8 buffer via func_800B2A00 and return it.
 *
 * Calls func_800B2A00 with D_800EEDC8, D_800EEC68, size 0x2C, and count 8.
 *
 * @return Pointer to D_800EEDC8.
 */
u8 *func_800B2C14(void) {
    u8 *buf = D_800EEDC8;
    func_800B2A00(buf, D_800EEC68, 0x2C, 8);
    return buf;
}

/**
 * @brief Call func_800B2A84 with D_800EEDC8 and the given parameter.
 *
 * @param a0 Second argument to func_800B2A84.
 * @return Result from func_800B2A84.
 */
s32 func_800B2C58(s32 a0) {
    return func_800B2A84(D_800EEDC8, a0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2C80);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2D0C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2E04);

/**
 * @brief Initialize sound system resources and configuration.
 *
 * Sets up sound data via func_800B2A00 with D_800EEDD8 and D_800EEDE8,
 * then initializes sound channels via sequential calls.
 */
void func_800B2EDC(void) {
    func_800B2A00(D_800EEDD8, D_800EEDE8, 0x18, 8);
    func_800DF904();
    func_800DF8E4(3, 0x57);
    func_800DF8C4(3, 0);
    func_800DF8A4(3, 0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2F3C);

/**
 * @brief Allocate handler for func_800B2F3C and store entity pointers.
 *
 * Registers func_800B2F3C as callback via func_800B2C58. If allocation
 * succeeds, clears byte at +0xC, stores a0 at +0x10 and a1 at +0x14,
 * then clears the first byte of a1.
 *
 * @param a0 Entity pointer stored at result offset 0x10.
 * @param a1 Pointer to completion flag byte, stored at result offset 0x14.
 */
void func_800B2FF8(s32 a0, u8 *a1) {
    u8 *result = (u8 *)func_800B2C58((s32)func_800B2F3C);
    if (result != 0) {
        result[0xC] = 0;
        *(s32 *)(result + 0x10) = a0;
        *(s32 *)(result + 0x14) = (s32)a1;
        *a1 = 0;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B304C);

/**
 * @brief Allocate a handler for func_800B304C, initialize fields, and clear target byte.
 *
 * @param a0 Pointer whose first byte will be cleared; stored at result[0x14].
 */
void func_800B3128(u8 *a0) {
    u8 *result = (u8 *)func_800B2C58((s32)func_800B304C);
    result[0xC] = 0;
    *(s32 *)(result + 0x14) = (s32)a0;
    *a0 = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3164);

/**
 * @brief Set up audio stream from descriptor and register playback callback.
 *
 * Computes buffer start and end from the descriptor's offset fields,
 * stores them to D_800EEEB8/D_800EEEBC/D_800EEEC0, then registers
 * func_800B3164 as a callback and links the control byte.
 *
 * @param a0 Audio descriptor with offsets at +4, +8, +0xC.
 * @param a1 Pointer to control byte (cleared after setup).
 */
void func_800B3270(s32 *a0, u8 *a1) {
    u8 *result;
    *(s32 *)D_800EEEB8 = (s32)a0 + a0[1];
    *(s32 *)D_800EEEBC = (s32)a0 + a0[2];
    *(s32 *)D_800EEEC0 = a0[3] - a0[2];
    result = (u8 *)func_800B2C58((s32)func_800B3164);
    result[0xC] = 0;
    *(s32 *)(result + 0x10) = (s32)a1;
    *a1 = 0;
}

/**
 * @brief Set D_800EEEC4 to 1, D_800E3D0C to 3, and store a0/a1 to D_800EEEC8/CC.
 *
 * @param a0 Value to store to D_800EEEC8.
 * @param a1 Value to store to D_800EEECC.
 */
void func_800B32E0(s32 a0, s32 a1) {
    *(u8 *)D_800EEEC4 = 1;
    *(u8 *)D_800E3D0C = 3;
    *(s32 *)D_800EEEC8 = a0;
    *(s32 *)D_800EEECC = a1;
}

/**
 * @brief Set D_800EEEC4 to 1, D_800E3D0C to 4, and store a0 to D_800EEEC8.
 *
 * @param a0 Value to store to D_800EEEC8.
 */
void func_800B330C(s32 a0) {
    *(u8 *)D_800EEEC4 = 1;
    *(u8 *)D_800E3D0C = 4;
    *(s32 *)D_800EEEC8 = a0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3330);

/**
 * @brief Initialize sound data pointer based on D_80082C11 flag.
 *
 * Clears D_800EEEC4 and D_800EEED4, then sets D_800EEED0 to either
 * D_8005F388 (if D_80082C11 is zero) or D_80063388 (if non-zero).
 */
void func_800B3470(void) {
    u8 flag = *(u8 *)D_80082C11;
    *(u8 *)D_800EEEC4 = 0;
    if (flag == 0) {
        *(s32 *)D_800EEED0 = (s32)D_8005F388;
    } else {
        *(s32 *)D_800EEED0 = (s32)D_80063388;
    }
    *(u8 *)D_800EEED4 = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B34B0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3534);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3574);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3650);

/**
 * @brief Allocate aligned memory from the scratchpad buffer.
 *
 * Aligns the requested size up to 4 bytes, advances the D_800EEED8
 * pointer, and returns the old (pre-advance) pointer.
 *
 * @param size Number of bytes to allocate.
 * @return Pointer to the allocated region.
 */
s32 func_800B3698(s32 size) {
    s32 ptr = *(s32 *)D_800EEED8;
    *(s32 *)D_800EEED8 = ptr + ((size + 3) & ~3);
    return ptr;
}

/**
 * @brief Free aligned memory back to the scratchpad buffer.
 *
 * Aligns the requested size up to 4 bytes, decrements the D_800EEED8
 * pointer, and returns the new (post-decrement) pointer.
 *
 * @param size Number of bytes to free.
 * @return New pointer value after deallocation.
 */
s32 func_800B36B8(s32 size) {
    s32 ptr = *(s32 *)D_800EEED8;
    ptr -= (size + 3) & ~3;
    *(s32 *)D_800EEED8 = ptr;
    return ptr;
}

/**
 * @brief Set D_800EEED8 to the scratchpad base address 0x1F800000.
 */
void func_800B36D8(void) {
    *(s32 *)D_800EEED8 = 0x1F800000;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B36E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3738);

/**
 * @brief Initialize SFX entry fields, setting pitch based on address range.
 *
 * If the entry pointer is below D_800EF4A4, pitch is cleared to 0.
 * Otherwise pitch is set to 0x800. Both paths clear volume and rate fields.
 *
 * @param entry Pointer to SFX entry structure.
 */
void func_800B37B4(u8 *entry) {
    if ((u32)entry < (u32)D_800EF4A4) {
        *(u16 *)(entry + 0xE) = 0;
    } else {
        *(u16 *)(entry + 0xE) = 0x800;
    }
    *(u16 *)(entry + 0xC) = 0;
    *(u16 *)(entry + 0x10) = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B37E0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3960);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3AE8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3B54);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3C2C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3E54);

/**
 * @brief Initialize sound effect table and clear entries.
 *
 * Clears 7 halfwords at stride 0x9C starting from D_800EF2D0+0x3A8
 * going backwards. Then initializes D_800EF020 via func_800B2A00
 * with D_800EEFB0, stride 0x10, and count 7. Clears D_800EF724.
 *
 * @return Pointer to D_800EF020.
 */
u8 *func_800B4248(void) {
    s32 i = 6;
    s32 base = (s32)D_800EF2D0;
    u8 *ptr = (u8 *)(base + 0x3A8);
    u8 *buf;
    top:
    *(s16 *)ptr = 0;
    i--;
    ptr -= 0x9C;
    if (i >= 0) goto top;
    buf = D_800EF020;
    func_800B2A00(buf, D_800EEFB0, 0x10, 0x7);
    *(s16 *)D_800EF724 = 0;
    return buf;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B42B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B44D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B4920);
