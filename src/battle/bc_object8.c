#include "common.h"
#include "psxsdk/libetc.h"
#include "battle.h"
#include "battle/bc_object8.h"
#include "battle/bc_object11.h"
#include "battle/bc_object9.h"
#include "battle/bc_object10.h"

s32 func_8002E680(u8 *text);
void func_800B8F98(s32 a0);
void func_800C66E4(void);
void func_800C674C(void);

extern u8 *D_800EEED8;
s32 func_800B304C(u8 *arg0);
extern u8 D_8007DADB[];
extern u8 D_800EE42C[];
extern u8 D_800EEEC4[];
extern u8 D_800E3D0C[];
extern u8 D_800EE4C1;
void func_800B2D0C(void);
void func_800B36D8(void);
void func_800B2EDC(void);
void func_800B3470(void);
u8 *func_800B4248(void);
u8 *func_800B2C14(void);
void func_800C675C(void);
void func_800B6954(void);
void func_800B79B8(void);
void func_800B64E0(void);
void func_800DCC08(void);
void func_8009B6B0(void);
u8 *func_800C0134(void);
s32 func_800C749C(void);
s32 func_800B84C8(void);
void func_800C42DC(void);
void func_800B650C(void);
void func_800BAE6C(void);
void func_800C65A8(void);
void func_800B7C48(void);
extern u8 D_80077E5E;
extern s8 D_800F1B88;
extern s8 D_800F05D8;
extern s8 D_800EEC60;
extern s8 D_800EEC54;
extern void *D_800EEC58;
extern void *D_800EEC50;
extern void *D_800EEC4C;
extern void *D_800EEC48;
extern void *D_800EEC38;
extern void *D_800EEC44;
extern s32 D_800EEC5C;
extern u8 D_800EF4A4[];
extern u8 D_800EEEC8[];
extern u8 D_800EEECC[];
extern u8 D_800EEEB8[];
extern u8 D_800EEEBC[];
extern u8 D_800EEEC0[];
extern u8 g_gameState[];
s32 func_800B3164(u8 *arg0);
s32 func_800B2F3C(u8 *arg0);
void func_800B3574(s32, s32, s32);
u16 func_800A9904(s32);
extern u8 D_8007809A[];
extern u8 D_800EE45C[];
extern u8 D_800EEDD8[];
extern u8 D_800EEDE8[];
extern u8 D_800EE465[];
extern u8 D_80082C11[];
extern u8 D_8005F388[];
extern u8 D_80063388[];
extern u8 D_800EF020[];
extern u8 D_800EEFB0[];
extern u8 D_800EF724[];
extern u8 D_800EE454[];
s32 func_800AE788(void);
s32 func_800AA4E0(void);
extern u8 D_800EEED0[];
extern u8 D_800EEED4[];
void sndSetMasterVolume(s32);
extern s32 D_800F1B90;
extern u8 D_801A0900[];
extern u8 D_800E3CF4[];
extern u8 D_800E3D04[];
extern u8 D_800E3D0D;
void func_80037308(void *, void *);
void func_800BB024(void *, void *);
void func_800C434C(void *, void *);
void func_80022E08(s32, s32);
void func_800231E0(s32, s32);
void recalcAllGfStats(void);
void func_800AB3E0(void);
s32 func_800B228C(void);
s32 func_800B24C8(u8 *arg0);
void func_800B2388(void);
void func_800A6288(s32);
void func_800A5F24(s32, s32, s32, s32, u16);
s32 func_800A9888(void);

void func_800B1624(s32 arg0) {
    if (D_800ED148.entities[arg0].controlFlags & CTRL_FLAG_10) {
        D_800ED148.entities[arg0].controlFlags |= 0x200;
    } else {
        func_800A6288(arg0);
        func_800A5F24(0, arg0, 1, 0, func_800A9888() & 0xFFFF);
    }
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B16C0);

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
    u8 *base = (u8 *)&D_800ED148;
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
    if (D_800ED148.unk130C != 0) {
        return;
    }
    {
        u8 *data = (u8 *)&D_800EE4C0;
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

/**
 * @brief Conditionally trigger limit break action based on entity and GF state.
 *
 * Returns immediately if D_800ED148[0x130C] is non-zero, or if the entity's
 * status halfword at offset 0x90 has bit 0 set, or if D_800EE4C1 is not 0x1F.
 * Otherwise calls func_800A97FC to get a value and passes it to func_800B0754.
 *
 * @param a0 Entity index (stride 0xD0 in D_800ED148).
 */
void func_800B18A0(s32 arg0) {
    if ((D_800ED148.unk130C == 0) && !(D_800ED148.entities[arg0].status & 1) && (D_800EE4C0.unk1 == 0x1F)) {
        func_800B0754(arg0, 0, 7, func_800A97FC(arg0));
    }
}

s32 func_800B1930(s32 arg0) {
    s32 count = 0;

    if (D_800ED148.unk130C == 0) {
        if (!(D_800ED148.entities[arg0].status & 1)) {
            if ((D_800EE4C1 == 0x1C) || (D_800EE4C1 == 0xF3)) {
                func_800B0754(arg0, 0, 6, func_800A97FC(arg0) & 0xFFFF);
                count = 1;
            }
            if (!(D_800ED148.entities[arg0].status & 1) && (D_800ED148.entities[arg0].status & 2)) {
                func_800B0754(arg0, 0, 2, func_800A97FC(arg0) & 0xFFFF);
                count += 1;
            }
        }
    }
    if (count != 0) {
        func_8009AE08(9);
    }
    return count;
}

/**
 * @brief Get entity from func_800AE6F8 and call func_800A59AC with mode 7.
 */
void func_800B1A48(void) {
    s32 val = func_800AE6F8();
    func_800A59AC(val, 7, 0);
}

/**
 * @brief Check if all enemy entities have HP below threshold 0xC8.
 *
 * @return 0 if a live enemy has value >= 0xC8, 1 otherwise.
 */
s32 func_800B1A78(void) {
    s32 i = 3;
    s32 base = (s32)&D_800ED148;
    s32 ptr = base + 0x270;
top:
    if (!(*(u16 *)(ptr + 0x90) & 1)) {
        if (*(u8 *)(ptr + 0xB8) >= 0xC8) {
            return 0;
        }
    }
    i++;
    ptr += 0xD0;
    if (i < 7) goto top;
    return 1;
}

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
    D_800ED148.unk1314 = 0;
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
    u8 *base = (u8 *)&D_800ED148;
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
    u8 *base = (u8 *)&D_800ED148;
    base[0x1314] = a0;
    *(u16 *)(base + 0x12E6) = a1;
    func_800A59AC(func_800AE6F8(), 8, 0);
}

/**
 * @brief Check if any other party entity has specific status flags.
 *
 * @param a0 Entity index to skip.
 * @param a1 Status mask to check.
 * @return 1 if a matching entity is found, 0 otherwise.
 */
s32 func_800B1BE4(s32 a0, s32 a1) {
    s32 i = 0;
    s32 ptr = (s32)&D_800ED148;
top:
    if (i != a0) {
        if (*(s32 *)(ptr + 0x8C) & 1) {
            if (*(u16 *)(ptr + 0x90) & a1) {
                return 1;
            }
        }
    }
    i++;
    ptr += 0xD0;
    if (i < 3) goto top;
    return 0;
}

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

void func_800B1C68(s32 arg0) {
    u8 *gs = g_gameState;

    if (!(gs[0xD22] & 0x10)) {
        if ((gs[0xB1A] & 2) && (D_800ED148.entities[arg0].status & 0x100)) {
            if (func_8009B79C(0x10, 0xFF) != 0) {
                func_800B1C3C(arg0, 0xC, func_800A97FC(arg0) & 0xFFFF);
                return;
            }
        }
        if ((D_80077E92 & 1) && (func_8009B79C(0x10, 0xFF) != 0)) {
            func_800B1C3C(arg0, 0xB, func_800A9904(arg0) & 0xFFFF);
        }
    }
}

s32 func_800B1D4C(s32 arg0, s32 arg1) {
    u8 *gs = g_gameState;

    if (gs[0xD22] & 0x10) goto ret0;
    if (arg1 < 3) goto ret0;
    if (D_800ED148.entities[arg0].linkedIdx != 4) goto ret0;
    if (*(u16 *)&g_battleConfig == 0x13D) goto ret0;
    if (!(gs[0xB1A] & 4)) goto ret0;
    if (func_8009B79C(0x20, 0xFF) == 0) goto ret0;
    return 1;
ret0:
    return 0;
}


INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B1DFC);

/**
 * @brief Copy a byte from D_8007DADB to D_800EE42C as a halfword.
 */
void func_800B2024(void) {
    D_800ED148.unk12E4 = *(u8 *)D_8007DADB;
}

/**
 * @brief Decrement entity timer at D_800ED148+0x12E4 or trigger completion.
 *
 * If the halfword timer at D_800ED148+0x12E4 is zero, calls func_800B1DFC
 * and func_800B2024 to handle completion. Otherwise decrements the timer.
 */
void func_800B2038(void) {
    s32 base = (s32)&D_800ED148;
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
    D_800ED148.unk131D = 0;
}

/**
 * @brief Check if all active party entities have status bit 2 set.
 *
 * @return 0 if an active entity without bit 2 is found, 1 otherwise.
 */
s32 func_800B20D8(void) {
    s32 i = 0;
    s32 ptr = (s32)&D_800ED148;
top:
    if (*(s32 *)(ptr + 0x8C) & 1) {
        if (!(*(u16 *)(ptr + 0x90) & 4)) {
            return 0;
        }
    }
    i++;
    ptr += 0xD0;
    if (i < 3) goto top;
    return 1;
}

/**
 * @brief Check if conditions are met to initiate an auto-battle action.
 *
 * Checks a chain of conditions: whether func_800AE788 returns the sentinel
 * 0xFF, whether func_800B20D8 indicates busy, bit 2 of D_8007809A flags,
 * whether g_battleConfig matches 0x13D, and whether entity slot 0x40 is
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
        if (*(u16 *)&g_battleConfig != 0x13D) {
            if (func_8009B79C(0x40, 0xFF) != 0) {
                D_800ED148.unk1314 = 1;
                func_800B1A48();
                return 1;
            }
        }
    }
    return 0;
}

/**
 * @brief Copy 3 bytes from g_gameState to D_800EE9E8 at stride 0x47.
 *
 * Copies bytes from g_gameState[i+0xAF4] to D_800EE9E8[i*0x47+0xA3]
 * for i = 0, 1, 2.
 */
void func_800B21B4(void) {
    s32 i = 0;
    u8 *src = g_gameState;
    u8 *dst = (u8 *)&D_800EE9E8;
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
 */
s32 func_800B21EC(s32 a0) {
    s32 i = 0;
    u8 *dst = (u8 *)&D_800EE9E8;
    s32 c;

loop_1:
    c = dst[0xA3];
    if (c != a0) {
        i += 1;
        dst += 0x47;
        if (i >= 3) {
            return 1;
        }
        goto loop_1;
    }
    return 0;
}

s32 func_800B2224(u8 *arg0, s32 arg1) {
    s32 i;
    s32 best;
    u32 minVal;
    s32 idx;
    u32 val;
    u8 *base;

    minVal = -1U;
    i = 0;
    if (arg1 > 0) {
        do {
            idx = *arg0;
            base = g_gameState + idx * 0x98;
            arg0 += 1;
            val = *(u32 *)(base + 0x494);
            if (val < minVal) {
                best = idx;
                minVal = val;
            }
            i += 1;
        } while (i < arg1);
    }
    return best;
}

/**
 * @brief Collect eligible party slots and pick one via func_800B2224.
 *
 * For each of 8 character strides in g_gameState: if exists bit0 is set,
 * statusFlags bit0 is clear, and func_800B21EC accepts the slot, record
 * the index. Returns func_800B2224 on the list, or 0xFF if empty.
 */
s32 func_800B228C(void) {
    u8 buf[8];
    s32 count = 0;
    s32 i = 0;
    do {
        u8 *base = g_gameState + (i * 0x98);
        if ((*(u16 *)(base + 0x524) & 1) && ((*(u16 *)(base + 0x526) & 1) == 0) &&
            func_800B21EC(i)) {
            *(buf + count) = i;
            count++;
        }
        i++;
    } while (i < 8);
    if (count == 0) {
        return 0xFF;
    }
    return func_800B2224(buf, count);
}

/**
 * @brief Process current battle entity: update effects and activate ability.
 *
 * Reads the entity index from D_800ED148[0x1301], calls func_800AE6C0
 * and func_800D3090 with it, then calls func_800D0530 and func_800AB3C4.
 */
void func_800B2338(void) {
    u8 *base = (u8 *)&D_800ED148;
    func_800AE6C0(base[0x1301]);
    func_800D3090(base[0x1301], 1);
    func_800D0530();
    func_800AB3C4();
}

void func_800B2388(void) {
    u8 *base = (u8 *)&D_800ED148;
    u8 slot = base[0x1301];
    u8 *gs = g_gameState + slot;

    gs[0xAF4] = base[0x1328];
    func_80022E08(gs[0xAF4], slot);
    func_800231E0(gs[0xAF4], slot);
    recalcAllGfStats();
    func_800A7518(slot);
    func_800A71C0(slot);
    func_8009A8B4(slot);
    func_8009A6A8(slot);
    func_8009B134(0xE, 0x80, 0);
    func_8009B134(0x70, 0x80, 0);
    func_8009AF14(func_800B2338);
}

void func_800B243C(s32 arg0) {
    s32 chosen;

    if (*(u16 *)&g_battleConfig == 0x1FF) {
        func_800B21B4();
        chosen = func_800B228C();
        if (chosen != 0xFF) {
            func_800AB3E0();
            func_800D3090(arg0, 0);
            D_800ED148.unk1301 = arg0;
            D_800ED148.unk1328 = chosen;
            func_8009AF14(func_800B2388);
        }
    }
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B24C8);

s32 func_800B25E4(void) {
    D_800EEC5C = 0;
    func_800B36D8();
    func_800B7C48();
    func_800C65A8();
    func_800BAE6C();
    func_800B2EDC();
    func_800B650C();
    func_800C42DC();
    func_800B3470();
    D_800EEC44 = (void *)func_800B84C8();
    D_800EEC38 = (void *)func_800B4248();
    D_800EEC48 = (void *)func_800B2C14();
    D_800EEC4C = (void *)func_800C749C();
    D_800EEC50 = func_800C0134();
    D_800EEC58 = 0;
    D_800EEC60 = 0;
    D_800EEC54 = 0;
    D_800F05D8 = 0;
    D_800F1B88 = 4 - D_80077E5E;
    return 0;
}

void func_800B26B8(void) {
    func_8009B6B0();
    func_800B2B68(D_800EEC44);
    func_800B2B68(D_800EEC48);
    D_800EEC60 = func_800B2B68(D_800EEC4C);
    if (D_800EEC58 != 0) {
        if (func_800B2B68(D_800EEC58) == 0) {
            D_800EEC58 = 0;
        }
    }
    D_800EEC54 = func_800B2B68(D_800EEC50);
    func_800B3738();
    func_800DCC08();
    func_8009B6B0();
    func_800B2B68(D_800EEC38);
    func_8009B6B0();
    func_800B64E0();
    func_8009B6B0();
    func_800B79B8();
    func_800B2D0C();
    func_800B6954();
    func_800B3738();
    func_800C675C();
}

s32 func_800B27AC(u8 *arg0) {
    u8 state = arg0[0xD];
    s32 target = *(s32 *)(arg0 + 0x10);

    if (state == 0) goto case0;
    if (state == 1) goto case1;
    goto ret0;

case0:
    D_800EEC5C |= 0x80;
    *(u8 *)(target + 1) = 0xFF;
    arg0[0xD] = arg0[0xD] + 1;
    goto ret0;

case1:
    if (func_800C5A94(0x18, 0x40) != 0) goto ret0;
    D_800EEC5C &= ~0x80;
    return 2;

ret0:
    return 0;
}


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

s32 func_800B28C8(u8 *arg0) {
    s16 kind = *(u16 *)(arg0 + 2) - 1;
    u8 *node;

    switch (kind) {
    case 0:
        func_800C66E4();
        return 0xF;
    case 1:
        func_800C674C();
        return 0xF;
    case 2:
        func_800B8F4C(0xF);
        return 0xF;
    case 3:
        func_800B8F98(0xD);
        return 0xF;
    case 7:
        func_800B2E04(*(void **)(arg0 + 4), *(s16 *)(arg0 + 8), arg0[0xA], arg0[0xB]);
        return 0xF;
    case 8:
        node = func_800B853C(func_800B24C8);
        goto finish8;
    case 9:
        ((void (*)(s32))*(void **)(arg0 + 4))(*(s32 *)(arg0 + 8));
        return 0xF;
    case 12:
        node = func_800B853C(func_800B27AC);
        node[0xD] = 0;
        node[0xF] = 0x14;
        *(s32 *)(node + 0x10) = (s32)arg0;
        return 8;
    case 13:
        node = func_800B853C(func_800B2848);
finish8:
        node[0xD] = 0;
        *(s32 *)(node + 0x10) = (s32)arg0;
        return 8;
    default:
        return 0xF;
    }
}


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
void func_800B2A00(void *header, void *data, s32 stride, s32 count) {
    s32 i = 0;
    u8 *a0 = header;
    u8 *entry = data;
    *(s32 *)a0 = 0;
    *(s32 *)(a0 + 4) = 0;
    *(s32 *)(a0 + 8) = (s32)entry;
    *(u16 *)(a0 + 0xC) = stride;
    *(u16 *)(a0 + 0xE) = count;
    if (count > 0) {
        do {
            *(u16 *)entry = 0;
            i++;
            entry += stride;
        } while (i < count);
    }
}

u16 *func_800B2A38(void *pool) {
    s16 count = *(s16 *)((u8 *)pool + 0xE);
    u16 *entry = *(u16 **)((u8 *)pool + 8);
    s32 i = 0;

    if (count > 0) {
        s32 limit = count;
        do {
            if (!(*entry & 1)) {
                return entry;
            }
            i += 1;
            entry = (u16 *)((u8 *)entry + *(s16 *)((u8 *)pool + 0xC));
        } while (i < limit);
    }
    return NULL;
}


/**
 * @brief Allocate a pool node, init it, and append via the pool tail pointer.
 *
 * Takes a node from @p pool via func_800B2A38, clears +2/+4, stores @p task
 * at +8, sets flags bit0, then links it as the new tail (pool+4). If the
 * pool was empty, also sets the head (pool+0).
 */
void *func_800B2A84(void *pool, void *task) {
    u8 *node = (u8 *)func_800B2A38(pool);
    if (node != NULL) {
        u16 flags = *(u16 *)node;
        *(u16 *)(node + 2) = 0;
        *(s32 *)(node + 4) = 0;
        *(s32 *)(node + 8) = (s32)task;
        *(u16 *)node = flags | 1;
        {
            u8 *prev = *(u8 **)((u8 *)pool + 4);
            if (prev != NULL) {
                *(s32 *)(prev + 4) = (s32)node;
            } else {
                *(s32 *)pool = (s32)node;
            }
            *(s32 *)((u8 *)pool + 4) = (s32)node;
        }
    }
    return node;
}

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

/**
 * @brief Run one frame of every task in @p pool.
 * @return The number of tasks still live afterwards.
 */
s32 func_800B2B68(void *pool) {
    u8 *prev = NULL;
    u8 *node = *(u8 **)pool;
    s32 count = 0;

    if (node != NULL) {
        do {
            s32 (*cb)(u8 *) = *(void **)(node + 8);
            if (cb(node) & 2) {
                *(u16 *)node = 0;
                if (prev != NULL) {
                    *(s32 *)(prev + 4) = *(s32 *)(node + 4);
                } else {
                    *(s32 *)pool = *(s32 *)(node + 4);
                }
            } else {
                prev = node;
                count += 1;
            }
            node = *(u8 **)(node + 4);
        } while (node != NULL);
    }
    *(s32 *)((u8 *)pool + 4) = (s32)prev;
    return count;
}


extern u8 D_800EEC68[];
extern u8 D_800EEDC8[];
extern u8 D_800EEEAF[];
extern u8 D_800EEEAC;
extern void *D_800EEEA8;
extern u8 D_800EEEB0[];
extern u16 D_800E3CFC[];

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
 * @param task Per-frame step installed on the new task.
 * @return Result from func_800B2A84.
 */
void *func_800B2C58(void *task) {
    return func_800B2A84(D_800EEDC8, task);
}

/**
 * @brief Tick a delayed flag slot; may promote it to D_800EEEA8.
 *
 * If D_800EEEAF[arg0->unkF] is already set, returns 0. Otherwise decrements
 * arg0->unkE; on wrap to 0xFF returns 2. Else marks the flag, and if unkF
 * exceeds D_800EEEAC, records arg0 in D_800EEEA8 / D_800EEEAC. Returns 0.
 */
s32 func_800B2C80(u8 *arg0)
{
  if (D_800EEEAF[arg0[0xF]] == 0)
  {
    u8 temp = arg0[0xE] - 1;
    arg0[0xE] = temp;
    if ((temp & 0xFF) == 0xFF)
    {
      return 2;
    }
    D_800EEEAF[arg0[0xF]] = 1;
    if (((u8) arg0[0xF]) > ((u8) D_800EEEAC))
    {
      D_800EEEA8 = arg0;
      D_800EEEAC = arg0[0xF];
    }
    return 0;
  }
  return 0;
}

void func_800B2D0C(void) {
    s32 i;
    u8 *p;
    u16 w;
    u16 *rect;

    D_800EEEAC = 0;
    i = 2;
    p = D_800EEEB0;
    p = p + 2;
    do {
        *p = 0;
        i -= 1;
        p -= 1;
    } while (i >= 0);

    func_800B2B68(D_800EEDD8);
    if (D_800EEEAC != 0) {
        func_800DF8E4(3, ((u8 *)D_800EEEA8)[0x13]);
        rect = D_800E3CFC;
        w = *(u16 *)((u8 *)D_800EEEA8 + 0x10);
        rect[2] = w;
        rect[0] = (0x140 - (s16)w) / 2;
        func_800DF884(3, rect);
        func_800DF804(3, ((u8 *)D_800EEEA8)[0x12], 0);
        func_800DF824(3, *(s32 *)((u8 *)D_800EEEA8 + 0x14));
        func_800DF864(3);
        return;
    }
    func_800DF844(3);
}

s32 func_800B2E04(u8 *arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 *node;
    s32 w;
    s32 kind;
    s32 half;

    node = func_800B2A84(D_800EEDD8, func_800B2C80);
    if (node == 0) goto done;
    node[0xC] = 0;
    *(s32 *)(node + 0x14) = (s32)arg0;
    node[0xE] = arg1;
    node[0x13] = arg3;
    w = func_8002E680(arg0) & 0xFFFF;
    kind = arg2 & 0xFF;
    node[0xF] = arg2;
    if (kind == 1) goto case1;
    if (kind == 0) goto done;
    if (kind >= 4) goto done;
    *(s16 *)(node + 0x10) = 0x130;
    half = 0x120;
    goto compute;
case1:
    *(s16 *)(node + 0x10) = 0xA0;
    half = 0x90;
compute:
    node[0x12] = (half - w) / 2;
done:
    return 0;
}


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

s32 func_800B2F3C(u8 *arg0) {
    s32 state = arg0[0xC];

    switch (state) {
    case 0:
        func_800C5304(0xA3, *(s32 *)(arg0 + 0x10));
        goto block_9;
    case 1:
        if (D_800F1B90 >= 0) {
            func_800BB084((u8 *)*(s32 *)(arg0 + 0x10));
block_9:
            arg0[0xC] = arg0[0xC] + 1;
        }
        return 0;
    case 2:
        **(u8 **)(arg0 + 0x14) = 0xFF;
        return 2;
    default:
        return 0;
    }
}

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
    u8 *result = func_800B2C58(func_800B2F3C);
    if (result != 0) {
        result[0xC] = 0;
        *(s32 *)(result + 0x10) = a0;
        *(s32 *)(result + 0x14) = (s32)a1;
        *a1 = 0;
    }
}

s32 func_800B304C(u8 *arg0) {
    s32 state = arg0[0xC];
    u8 *temp_a0;
    u8 *temp_s0;

    switch (state) {
    case 0:
        func_800C5304(0xA4, (s32)D_801A0900);
        goto block_9;
    case 1:
        if (D_800F1B90 >= 0) {
            temp_a0 = D_801A0900;
            temp_s0 = temp_a0 - 0x900;
            func_80037308(temp_a0, temp_s0);
            func_800BB024(D_800E3D04, temp_s0);
block_9:
            arg0[0xC] = arg0[0xC] + 1;
        }
        return 0;
    case 2:
        **(u8 **)(arg0 + 0x14) = 0xFF;
        return 2;
    default:
        return 0;
    }
}

/**
 * @brief Allocate a handler for func_800B304C, initialize fields, and clear target byte.
 *
 * @param a0 Pointer whose first byte will be cleared; stored at result[0x14].
 */
void func_800B3128(u8 *a0) {
    u8 *result = func_800B2C58(func_800B304C);
    result[0xC] = 0;
    *(s32 *)(result + 0x14) = (s32)a0;
    *a0 = 0;
}

s32 func_800B3164(u8 *arg0) {
    s32 state = arg0[0xC];

    switch (state) {
    case 0:
        *(u8 *)D_800EEEC4 = 1;
        *(u8 *)D_800E3D0C = 1;
        goto block_11;
    case 1:
        if (sndGetEngineState() == 0) {
            if (D_800E3D0D != 0) {
                D_800E3D0D = 0;
                g_battleConfig.unk9 ^= 1;
            }
            func_800B3574(*(s32 *)D_800EEED0, *(s32 *)D_800EEEBC, *(s32 *)D_800EEEC0);
            *(u8 *)D_800EEEC4 = 1;
            *(u8 *)D_800E3D0C = 2;
block_11:
            arg0[0xC] = arg0[0xC] + 1;
        }
        return 0;
    case 2:
        **(u8 **)(arg0 + 0x10) = 1;
        return 2;
    default:
        return 0;
    }
}

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
    result = func_800B2C58(func_800B3164);
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

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B3330);

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

void func_800B34B0(void) {
    s32 i;

    if (*(u8 *)D_800EEED4 != 0) {
        i = 1;
        do {
            if (!((*(u8 *)D_800EEED4 >> i) & 1)) {
                sndDisableReverb(i);
            }
            i += 1;
        } while (i < 4);
    } else {
        sndDisableReverb(0);
    }
    sndSetMasterVolume(0x7F);
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B3534);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B3574);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B3650);

/**
 * @brief Allocate aligned memory from the scratchpad buffer.
 *
 * Aligns the requested size up to 4 bytes, advances the D_800EEED8
 * pointer, and returns the old (pre-advance) pointer.
 *
 * @param size Number of bytes to allocate.
 * @return Pointer to the allocated region.
 */
void *func_800B3698(s32 size) {
    u8 *ptr = D_800EEED8;
    D_800EEED8 = ptr + ((size + 3) & ~3);
    return ptr;
}

/**
 * @brief Free aligned memory back to the scratchpad buffer.
 *
 * Aligns the requested size up to 4 bytes and moves the D_800EEED8 pointer
 * back by that much. The subtraction folded into the store is what leaves the
 * new pointer in v0 the way the original does; a two-statement spelling swaps
 * v0 and v1.
 *
 * @param size Number of bytes to free.
 */
void func_800B36B8(s32 size) {
    u8 *ptr = D_800EEED8;

    D_800EEED8 = ptr - ((size + 3) & ~3);
}

/**
 * @brief Set D_800EEED8 to the scratchpad base address 0x1F800000.
 */
void func_800B36D8(void) {
    D_800EEED8 = (u8 *)getScratchAddr(0);
}

/**
 * @brief Walk the 0x8C ring from @p node and return the lowest address.
 */
void *func_800B36E8(u8 *node) {
    u8 *best;
    u8 *start;

    if (*(u8 **)(node + 0x8C) == 0) {
        return node;
    }
    best = node;
    start = best;
    do {
        node = *(u8 **)(node + 0x8C);
        if (node == 0) {
            return start;
        }
        if (node < best) {
            best = node;
        }
    } while (node != start);
    return best;
}


INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B3738);

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

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B37E0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B3960);

/**
 * @brief Copy selected words from arg0->unk4 into @p out, then retarget.
 *
 * Bits of @p mask skip the corresponding word copy (bit0:+0, bit1:+4,
 * bit2:+8, bit3:+0xC). Always stores @p out at arg0+4.
 */
void func_800B3AE8(void *arg0, s32 *out, s32 mask) {
    s32 *src = *(s32 **)((u8 *)arg0 + 4);

    if (!(mask & 1)) {
        out[0] = src[0];
    }
    if (!(mask & 2)) {
        out[1] = src[1];
    }
    if (!(mask & 4)) {
        out[2] = src[2];
    }
    if (!(mask & 8)) {
        out[3] = src[3];
    }
    *(s32 **)((u8 *)arg0 + 4) = out;
}


INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B3B54);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B3C2C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B3E54);

/**
 * @brief Initialize sound effect table and clear entries.
 *
 * Clears the flags of all 7 battle slots, then initializes D_800EF020
 * via func_800B2A00
 * with D_800EEFB0, stride 0x10, and count 7. Clears D_800EF724.
 *
 * @return Pointer to D_800EF020.
 */
u8 *func_800B4248(void) {
    s32 i = 6;
    /* The table address is materialised before the offset is added; as one
       expression gcc folds both into a single addiu. */
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

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B42B4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object8", func_800B44D8);

s32 func_800B4920(u8 *arg0) {
    u8 *temp_a2 = *(u8 **)(arg0 + 0x10);
    u8 state = arg0[0xD];
    s32 idx = *(s16 *)(temp_a2 + 8);
    u8 *temp_a0 = (u8 *)((s32)D_800EF2D0 + idx * 0x9C);

    switch (state) {
    case 0:
        if (temp_a0[5] == 0) {
            func_800C63D4(temp_a0, 5, temp_a2);
            arg0[0xD] = arg0[0xD] + 1;
        }
        return 0;
    case 1:
        if (temp_a0[5] == 0) {
            temp_a2[1] = 0xFF;
            return 2;
        }
        return 0;
    default:
        return 0;
    }
}

