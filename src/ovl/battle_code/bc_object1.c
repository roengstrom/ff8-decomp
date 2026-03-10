/**
 * @file bc_object1.c
 * @brief Battle scene orchestration and entity management.
 *
 * This file contains functions for managing battle entities within the
 * battle_code overlay. It handles:
 * - Entity state initialization and transitions (state machine at D_800ED148)
 * - Sound/SFX command queuing via a 16-entry task queue (D_800EE24B)
 * - Battle animation triggers and status effect application
 * - Random number generation via a shuffle buffer (D_800EEBA8/D_800EEBB0)
 * - Damage clamping and probability checks
 * - Music/sound playback coordination
 *
 * The primary data structure is the battle state array at D_800ED148, with
 * 0xD0-byte stride per entity (up to 7 entities). Entity fields include
 * state (0x4), flags (0x18), animation params (0x84-0x92), linked index
 * (0xCB), and various control bytes at large offsets (0x5C2, 0x12E8-0x1319).
 */
#include "common.h"
#include "battle.h"
#include "gf.h"

extern u8 D_800ED148[];
extern u8 D_800ED157[];
extern u8 D_800ED158[];
extern u8 D_800E19BC[];
extern u16 D_800E3CA4[];
extern u16 D_800E3CA8[];
extern u16 D_800E3CB0[];
extern u8 D_800EDE24[];
extern u8 D_800EE24B[];
extern u8 D_800EE28C[];
extern u8 D_800EEBA8[];
extern u8 D_800EEBB0[];
extern u8 D_800EE449[];
extern u8 D_800EE456[];
extern u8 D_800EE4C1[];
extern u16 D_80082C08[];
extern u8 D_80082C0A[];
extern u8 D_80082C0F[];
extern s16 D_8005F11C;
extern u8 g_gfData[];

extern u8 D_80098030[];
extern u8 D_800E19B4[];
extern u8 D_800ED1D8[];

extern u32 jtbl_80098018[];

void func_80027448(void);
void func_8009A254(void);
void func_8009A308(void);
void func_8009A38C(void);
void func_8009A3BC(void);
void func_8009A42C(s32, s32);
void func_8009A4A4(void);
void func_8009A528(s32, s32);
void func_8009A638(void);
void func_8009A6A8(s32);
void func_8009A74C(void);
void func_8009A8B4(s32);
void func_8009A928(void);
void func_8009A990(s32);
void func_8009AA2C(void);
void func_8009AAC4(s32);
void func_8009AB54(s32);
void func_8009ABE4(void);
void func_8009ABFC(void);
void func_8009AC14(void);
void func_8009AC34(void);
void func_8009AC68(void);
void func_8009ACB4(void);
void func_8009ACEC(void);
void func_8009AD7C(void);
void func_8009AE08(s32);
void func_8009AE9C(void);
void func_8009AF14(s32);
s32 func_8009AF3C(s32, s32, s32, s32, s32);
void func_8009AF98(s32);
s32 func_8009AFF0(s32);
void func_8009B088(s32, s32, s32, s32);
void func_8009B0F8(s32);
s32 func_8009B134(s32, s32, s32);
s32 func_8009B15C(void);
void func_8009B198(s32);
void func_8009B208(u8 *, u8 *, s32);
s32 func_8009B238(u8 *, s32);
s32 func_8009B270(u8 *, s32);
s32 func_8009B2A4(u8 *, u8 *, s32);
void func_8009B320(s32, u8 *, u8 *);
s32 func_8009B390(u8 *, s32);
s32 func_8009B3D0(s32);
void func_8009B428(void);
void func_8009B478(void);
void func_8009B520(void);
void func_8009B59C(s32, s32 *, s32 *);
void func_8009B5C4(s32, s32, s32, s32);
void func_8009B654(void);
void func_8009B690(void);
void func_8009B6B0(void);
s32 func_8009B74C(s32, s32);
void func_8009B79C(s32, s32);
s32 func_8009B7BC(s32);
s32 func_8009B7F4(s32, s32);
void func_8009B878(s32, u16 *, s32 *, s32);
void func_8009B924(s32, s32, s32);
s32 func_8009BA5C(s32, s32);

void func_800393C8(void);
void func_80042634(s32);
void func_800A30E4(void);
void func_800A6288(s32);
void func_800A62B0(void);
void func_800A1CFC(s32);
void func_800A1AB8(s32, s32, s32);
void func_800A240C(s32, s32, s32);
void func_800A59AC(s32, s32, s32);
void func_800A69BC(void);
void func_800A6724(void);
void func_800A6D30(void);
void func_800A7B48(void);
void func_800A7884(void);
void func_800A853C(void);
void func_800A864C(void);
void func_800A86F0(s32);
void func_800A94E0(void);
void func_800A97D4(void);
void func_800A79A0(void);
s32 func_800AE6C0(void);
s32 func_800AE730(void);
s32 func_800AE788(void);
void func_800AECD4(void);
void func_800AED30(void);
void func_800AEC04(void);
void func_800AED9C(void);
void func_800AEB50(void);
void func_800AF8A4(s32);
s32 func_800B0600(s32, s32);
s32 func_800B0668(s32, s32);
s32 func_800B0F7C(s32);
s32 func_800B0F9C(s32);
s32 func_800B1930(s32);
void func_800B1ACC(void);
void func_800B2024(void);
void func_800B2084(void);
void func_800B25E4(void);
void func_800B26B8(void);
s32 func_800B8564(s32, s32);
s32 func_800CEDA4(void);
void func_8003023C(s32);
void func_80030248(s32);
void func_80013744(s32, s32, s32);
void func_80038868(s32, s32, s32, s32);
void func_8003882C(s32, s32, s32, s32);
void func_80038920(s32, s32, s32, s32);
void func_80020644(s32, s32, s32);
void func_800D0F74(void);

/**
 * @brief Battle initialization entry point.
 *
 * Sets up entity state array, initializes task queue, shuffle buffer,
 * sound channels, animation entities, and queues initial battle commands.
 *
 * @note Non-matching: prologue saves s1 before s0 (s0 in jal delay slot),
 * plus complex mid-function scheduling with D_800EDE24/D_800ED148 address
 * reuse across multiple code paths. Also maspsx fills epilogue delay slots.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_80099FE8);

/**
 * @brief Transition to battle active state.
 *
 * Calls animation setup, entity init, position calculation, and queues
 * sound/SFX commands. Sets entity state to 2 (active).
 */
void func_8009A160(void) {
    volatile s32 *f;
    func_800A7B48();
    func_800A6D30();
    func_8009A638();
    func_8009A928();
    func_8009A74C();
    func_8009AF14((s32)&func_800D0F74);
    func_800A69BC();
    func_8009B134(0x70, 0x80, 0);
    func_8009AF14((s32)&func_8009ABE4);
    f = (volatile s32 *)D_800ED148;
    f[1] = 2;
}

/**
 * @brief Transition to post-battle state.
 *
 * Sets animation flag 0x12EA, calls victory/result handlers,
 * initializes idle animations, and sets entity state to 4.
 */
/**
 * @note Non-matching: maspsx fills `lw ra` load delay slot with
 * `sw v0, 4(s0)` and `jr ra` branch delay slot with `addiu sp`,
 * producing a FILLED epilogue. Original has UNFILLED with nops.
 *
 * Best attempt:
 * @code
 * void func_8009A1E0(void) {
 *     s32 base = (s32)D_800ED148;
 *     s32 one = 1;
 *     *(u8 *)(base + 0x12EA) = one;
 *     func_800AE6C0();
 *     func_800A97D4();
 *     func_8009A254();
 *     *(u8 *)(base + 0x5C2) = one;
 *     func_800B1ACC();
 *     func_800B2084();
 *     func_800B2024();
 *     *(s32 *)(base + 0x4) = 4;
 * }
 * @endcode
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_8009A1E0);

/**
 * @brief Initialize idle animations for all active entities.
 *
 * Iterates 7 entities (stride 0xD0). For each with flags (0x8C) having
 * bits 0x1 and 0x10 set but not 0x80, triggers idle animation via
 * func_800A59AC.
 */
void func_8009A254(void) {
    s32 i = 0;
    s32 base = (s32)D_800ED148;

    for (; i < 7; i++, base += 0xD0) {
        if (*(volatile s32 *)(base + 0x8C) & 1) {
            if (*(volatile s32 *)(base + 0x8C) & 0x10) {
                if (!(*(volatile s32 *)(base + 0x8C) & 0x80)) {
                    func_800A59AC(i, 0, 0);
                }
            }
        }
    }
}

/**
 * @brief Get battle animation active flag from D_800ED148+0x12E9.
 * @return The byte at offset 0x12E9 in the battle state array.
 */
u32 func_8009A2E0(void) {
    s32 base = (s32)D_800ED148;
    REGALLOC_BARRIER(base);
    return *(u8 *)(base + 0x12E9);
}

/**
 * @brief Get battle animation sequence flag from D_800ED148+0x12EA.
 * @return The byte at offset 0x12EA in the battle state array.
 */
u32 func_8009A2F4(void) {
    s32 base = (s32)D_800ED148;
    REGALLOC_BARRIER(base);
    return *(u8 *)(base + 0x12EA);
}

/**
 * @brief Process pending animation commands.
 *
 * While 0x12EA is set and func_800CEDA4 returns nonzero, sets 0x12E9
 * to 1 and yields via func_8009B690. On exit, clears 0x12E9 and
 * yields again, then calls func_800B26B8.
 */
/**
 * @note Non-matching: s-reg allocation swapped (s0=base, s1=one vs original
 * s1=base, s0=one), and compiler eliminates the intermediate v1 register
 * for D_800ED148 address, producing 4 fewer instructions.
 *
 * Best attempt:
 * @code
 * void func_8009A308(void) {
 *     s32 base = (s32)D_800ED148;
 *     s32 one;
 *     if (*(u8 *)(base + 0x12EA) == 0) goto skip;
 *     one = 1;
 * loop:
 *     if (func_800CEDA4() == 0) goto done;
 *     *(u8 *)(base + 0x12E9) = one;
 *     func_8009B690();
 *     goto loop;
 * skip: done:
 *     *(u8 *)((s32)D_800ED148 + 0x12E9) = 0;
 *     func_8009B690();
 *     func_800B26B8();
 * }
 * @endcode
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_8009A308);

/**
 * @brief Queue a battle music command.
 *
 * Plays sound ID 0x3EA at volume 0x80, then stores D_800EDE24 value
 * into the returned command buffer.
 */
void func_8009A38C(void) {
    s16 *result = (s16 *)func_8009B134(0x3EA, 0x80, 0);
    *result = *(u8 *)D_800EDE24;
}

/**
 * @brief Play two startup sound effects.
 *
 * Plays sound ID 1 and sound ID 0x3EB, both at volume 0x80.
 */
void func_8009A3BC(void) {
    func_8009B134(1, 0x80, 0);
    func_8009B134(0x3EB, 0x80, 0);
}

/**
 * @brief Conditionally apply vibration feedback.
 *
 * If bit 2 of D_80082C0A is set, calls func_8003023C(0x1000) and
 * func_80030248(1) to trigger controller vibration.
 */
void func_8009A3F4(void) {
    if (*(u16 *)D_80082C0A & 4) {
        func_8003023C(0x1000);
        func_80030248(1);
    }
}

/**
 * @brief Queue a hit-reaction sound for a specific entity.
 *
 * Computes entity offset (a0 * 0xD0), plays sound ID 0x66 at volume 0x80
 * targeting that entity, then stores entity index and hit type from
 * D_800ED158 + offset 0xD04.
 * @param a0 Entity slot index.
 * @param a1 Offset into entity data for hit type lookup.
 */
/**
 * @note Non-matching: prologue interleaving. Original interleaves entity offset
 * computation (sll/addu chain) between s2 init and s0 save. Compiler groups
 * all s-reg saves before computations. 30-byte difference, same instruction count.
 *
 * Best attempt:
 * @code
 * void func_8009A42C(s32 a0, s32 a1) {
 *     s32 idx = a0;
 *     s32 off = a1;
 *     s32 offset = ((idx * 2 + idx) * 4 + idx) * 16;
 *     s32 base = (s32)D_800ED158;
 *     u8 *buf;
 *     buf = (u8 *)func_8009B134(0x66, 0x80, offset + base);
 *     base += off;
 *     *(u16 *)buf = idx;
 *     *(u16 *)(buf + 0x2) = *(u8 *)(base + 0xD04);
 * }
 * @endcode
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_8009A42C);

/**
 * @brief Queue hit-reaction sounds for all active party members.
 *
 * Iterates 8 slots checking D_800ED148+0xD5C. For each active slot,
 * calls func_8009A42C with sequential sound IDs starting at 3.
 */
void func_8009A4A4(void) {
    s32 cmdId = 3;
    s32 i = 0;
    s32 base = (s32)D_800ED148;

    for (; i < 8; i++) {
        if (*(u8 *)(i + base + 0xD5C)) {
            func_8009A42C(cmdId, i);
            cmdId++;
        }
    }
}

/**
 * @brief Test if a specific bit is set in a bitmask.
 * @param a0 The bitmask value.
 * @param a1 The bit position to test.
 * @return 1 if the bit is set, 0 otherwise.
 */
s32 func_8009A514(s32 a0, s32 a1) {
    s32 mask = 1 << a1;
    s32 val = a0 & mask;
    return val != 0;
}

/**
 * @brief Queue an attack animation command for an entity.
 *
 * Calls func_8009AFF0 to snapshot entity state, then plays sound 0x67
 * targeting the entity. Stores entity index, sets flag based on entity
 * flags bit 1, and copies position data from D_800ED148 tables.
 * @param a0 Entity slot index.
 * @param a1 Source entity index for position lookup.
 *
 * @note Non-matching: scrambled prologue (s2,s3,s1,s0 save order) with offset
 * computation interleaved between s-reg saves. Also maspsx fills epilogue
 * delay slots (FILLED epilogue, 2 instructions shorter).
 *
 * Best attempt:
 * @code
 * void func_8009A528(s32 a0, s32 a1) {
 *     s32 idx = a0;
 *     s32 src = a1;
 *     s32 offset, base;
 *     u8 *buf;
 *     func_8009AFF0(idx);
 *     func_800A1CFC(idx);
 *     offset = ((idx * 2 + idx) * 4 + idx) * 16;
 *     base = (s32)D_800ED158;
 *     buf = (u8 *)func_8009B134(0x67, 0x80, offset + base);
 *     base += offset - 0x10;
 *     *(u16 *)buf = idx;
 *     buf[2] = 1;
 *     if (*(s32 *)(base + 0x8C) & 2) buf[3] = 1;
 *     else buf[3] = 0;
 *     { s32 b = (s32)D_800ED148;
 *       s32 o1 = ((idx * 2 + idx) * 4 + idx) * 16;
 *       *(u8 *)(o1 + b + 0xCB) = *(u8 *)(src + b + 0xD14);
 *       s32 o2 = (src * 2 + src) * 2;
 *       *(u16 *)(o1 + b + 0x84) = *(u16 *)(o2 + b + 0xCE4);
 *       *(u16 *)(o1 + b + 0x86) = *(u16 *)(o2 + b + 0xCE6);
 *       *(u16 *)(o1 + b + 0x88) = *(u16 *)(o2 + b + 0xCE8); }
 * }
 * @endcode
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_8009A528);

/**
 * @brief Queue attack animations for all active party members.
 *
 * Iterates 8 slots checking D_800ED148+0xD5C. For each active slot,
 * calls func_8009A528 with sequential command IDs starting at 3.
 */
void func_8009A638(void) {
    s32 cmdId = 3;
    s32 i = 0;
    s32 base = (s32)D_800ED148;

    for (; i < 8; i++) {
        if (*(u8 *)(i + base + 0xD5C)) {
            func_8009A528(cmdId, i);
            cmdId++;
        }
    }
}

/**
 * @brief Queue a return-to-position animation for an entity.
 *
 * Computes entity offset, calls animation handler and state snapshot,
 * then plays sound 0x67 targeting the entity with position data cleared.
 * @param a0 Entity slot index.
 *
 * @note Non-matching: deeply scrambled prologue (s3,s1,s0,s2 save order)
 * with entity offset computation interleaved between saves.
 *
 * Best attempt:
 * @code
 * void func_8009A6A8(s32 a0) {
 *     s32 idx = a0;
 *     s32 base = (s32)D_800ED148;
 *     s32 offset = ((idx * 2 + idx) * 4 + idx) * 16;
 *     s32 entity = offset + base;
 *     func_800A240C(*(s32 *)(entity + 0x28), offset + base + 0x90);
 *     func_8009AFF0(idx);
 *     func_800A1AB8(idx, *(u16 *)(entity + 0x90), *(s32 *)(entity + 0x18));
 *     { u8 *buf = (u8 *)func_8009B134(0x67, 0x80, offset + base + 0x10);
 *       *(u16 *)buf = idx;
 *       buf[2] = 0;
 *       buf[3] = 0; }
 * }
 * @endcode
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_8009A6A8);

/**
 * @brief Complex battle scene setup — positions, animations, and sound.
 *
 * Counts active entities (those with CB != 0xFF), plays sound 0xD,
 * then iterates entities calling func_8009A6A8 for each active one.
 * Sets position data based on entity count using lookup tables
 * D_800E3CA4, D_800E3CA8, and D_800E3CB0.
 *
 * @note Non-matching: deeply scrambled prologue (s3,s1,...,s6,s5,s4,s2,s0
 * save order). s3 and s1 initialized to 0 with s3 saved first; 7 s-regs used.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_8009A74C);

/**
 * @brief Queue a damage/heal visual effect sound for an entity.
 *
 * Computes entity offset, plays sound 0x66 at volume 0x80 targeting
 * the entity, and stores the linked entity index (offset 0xBB) as
 * parameter.
 * @param a0 Entity slot index.
 */
/**
 * @note Non-matching: prologue interleaving. Original has s2=idx(a0) saved
 * first, then s0 for entity offset computation interleaved with s1 save.
 * Compiler groups saves before computation. Also maspsx fills epilogue delay
 * slots (FILLED epilogue, 2 instructions shorter).
 *
 * Best attempt:
 * @code
 * void func_8009A8B4(s32 a0) {
 *     s32 idx = a0;
 *     s32 offset = ((idx * 2 + idx) * 4 + idx) * 16;
 *     s32 base = (s32)D_800ED158;
 *     u8 *buf;
 *     buf = (u8 *)func_8009B134(0x66, 0x80, offset + base);
 *     base += offset;
 *     *(u16 *)buf = idx;
 *     *(u16 *)(buf + 0x2) = *(u8 *)(base + 0xBB);
 * }
 * @endcode
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_8009A8B4);

/**
 * @brief Queue damage sounds for all entities with linked targets.
 *
 * Iterates 3 entity slots. For each with CB != 0xFF, calls
 * func_8009A8B4 to queue the damage sound.
 */
void func_8009A928(void) {
    s32 i = 0;
    s32 sentinel = 0xFF;
    s32 base = (s32)D_800ED148;

    for (; i < 3; i++, base += 0xD0) {
        if (*(u8 *)(base + 0xCB) != sentinel) {
            func_8009A8B4(i);
        }
    }
}

/**
 * @brief Process a deferred damage/effect trigger for a specific type.
 *
 * Iterates up to 7 entities (stride 0xD0). When entity D7/D8 fields
 * match the given type, triggers the effect via func_800A59AC and
 * clears the D7/D8 fields. Special handling for type 2 with bit 0
 * of status (0x90).
 * @param a0 Effect type to process.
 */
/**
 * @note Non-matching: compiler folds base+0xD7 into s0 (offset folding),
 * hoists constant 2 into a register before the loop, and uses v1 for
 * effectType instead of a1.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_8009A990);

/**
 * @brief Process all pending deferred damage triggers.
 *
 * If D_80082C0F is zero, checks animation state and processes each
 * pending trigger type via func_8009A990.
 */
void func_8009AA2C(void) {
    s32 base;
    s32 sentinel;
    s32 i;

    if (*(u8 *)D_80082C0F != 0) {
        return;
    }
    base = (s32)D_800ED148;

    if (*(u8 *)(base + 0x12F9) != 1) {
        sentinel = 0xFF;
        if (func_800AE730() == sentinel) {
            return;
        }
        if (func_800AE788() == sentinel) {
            return;
        }
    }
    {
        s32 count = *(u8 *)(base + 0x12F8);
        if (count != 0) {
            i = 0;
            do {
                func_8009A990(i);
                count = *(u8 *)(base + 0x12F8);
                i++;
            } while (i < count);
        }
    }
}

/**
 * @brief Timer-based sound effect callback.
 *
 * Decrements a timer at offset 0x8 in the task entry. When timer
 * reaches zero, plays sound 0x4 at volume 0xF0, and conditionally
 * plays a timed sound via func_80013744 if condition 0x1BD == 3.
 * @param a0 Task queue slot index (multiplied by 0x10).
 */
void func_8009AAC4(s32 a0) {
    s32 off = a0 << 4;
    s32 base = (s32)D_800EE28C;
    u8 *entry = (u8 *)(off + base);

    if (*(u16 *)(entry + 0x8) == 0) {
        func_8009B134(4, 0xF0, 0);
        if (*(u8 *)(base + 0x1BD) == 3) {
            func_80013744(D_8005F11C, 0x3C, 0);
        }
        *(u8 *)(entry + 0xF) = 1;
    }
    *(u16 *)(entry + 0x8) = *(u16 *)(entry + 0x8) - 1;
}

/**
 * @brief Schedule a timer-based sound effect.
 *
 * Queues func_8009AAC4 as a task callback via func_8009B3D0, then
 * stores the given duration into the task's timer field.
 * @param a0 Duration in frames for the timer.
 */
void func_8009AB54(s32 a0) {
    s32 dur = a0;
    s32 off;
    s32 base;
    off = func_8009B3D0((s32)func_8009AAC4) << 4;
    base = (s32)D_800EE28C;
    *(u16 *)(off + base + 0x8) = dur;
}

/**
 * @brief Conditionally run startup sequence if battle state is idle.
 *
 * If D_800ED148 word 0 is zero, calls a chain of initialization
 * functions: func_800AECD4, func_800AED30, func_800AEC04,
 * func_800AED9C, func_800AEB50.
 */
void func_8009AB98(void) {
    if (*(s32 *)D_800ED148 == 0) {
        func_800AECD4();
        func_800AED30();
        func_800AEC04();
        func_800AED9C();
        func_800AEB50();
    }
}

/**
 * @brief Set entity state to 3 (victory/win).
 *
 * Writes value 3 to D_800ED148 offset 0x4 (entity state field).
 */
void func_8009ABE4(void) {
    volatile s32 *f = (volatile s32 *)D_800ED148;
    f[1] = 3;
}

/**
 * @brief Set entity state to 1 (active/ready).
 *
 * Writes value 1 to D_800ED148 offset 0x4 (entity state field).
 *
 */
void func_8009ABFC(void) {
    volatile s32 *f = (volatile s32 *)D_800ED148;
    f[1] = 1;
}

/**
 * @brief Wrapper for func_800A30E4.
 *
 * Simple pass-through call to the animation processing function.
 */
void func_8009AC14(void) {
    func_800A30E4();
}

/**
 * @brief Reset battle state and reinitialize animation engine.
 *
 * Clears entity state word, calls func_8009AA2C (deferred triggers),
 * func_800A30E4 (animation), and func_800A79A0 (state reset).
 */
void func_8009AC34(void) {
    *(s32 *)D_800ED148 = 0;
    func_8009AA2C();
    func_800A30E4();
    func_800A79A0();
}

/**
 * @brief Full reset with entity-specific reinitialization.
 *
 * Clears entity state, calls func_8009AA2C, func_800A30E4,
 * func_800A79A0, then reads entity index from offset 0xF and
 * calls func_800AF8A4 with it.
 */
void func_8009AC68(void) {
    s32 base = (s32)D_800ED148;
    *(s32 *)base = 0;
    func_8009AA2C();
    func_800A30E4();
    func_800A79A0();
    func_800AF8A4(*(u8 *)(base + 0xF));
}

/**
 * @brief Conditional reset based on entity activity check.
 *
 * Reads entity type from D_800ED157, checks via func_800B1930.
 * If inactive, calls func_8009AC68 for full reset.
 */
void func_8009ACB4(void) {
    s32 a0 = *(u8 *)D_800ED157;
    if (func_800B1930(a0) == 0) {
        func_8009AC68();
    }
}

/**
 * @brief Initialize battle entities for a new round.
 *
 * Sets control bytes (0x12E8=2, 0xD=0, 0x12FD=1, 0x12EA=0, 0x5C2=0),
 * then iterates 3 entity slots calling func_800A6288 and clearing
 * bit 31 of flags word (0x18). Finally calls func_800A62B0.
 */
void func_8009ACEC(void) {
    s32 i = 0;
    s32 mask = 0x7FFFFFFF;
    register s32 tmp asm("$2") = D_800ED148;
    s32 base;
    REGALLOC_BARRIER(tmp);
    base = tmp;

    *(u8 *)(base + 0x12E8) = 2;
    *(u8 *)(base + 0xD) = 0;
    *(u8 *)(base + 0x12FD) = 1;
    *(u8 *)(base + 0x12EA) = 0;
    *(u8 *)(base + 0x5C2) = 0;

    for (; i < 3; i++) {
        func_800A6288(i);
        *(volatile s32 *)(base + 0x18) = *(volatile s32 *)(base + 0x18) & mask;
        base += 0xD0;
    }
    func_800A62B0();
}

/**
 * @brief Set battle round timer based on speed setting.
 *
 * Reads D_800EE449 (speed setting 0-3), maps to frame counts:
 * 0 or 3 -> 0x3C (60), 1 -> 0x1E (30), 2 -> 0x28 (40).
 * Schedules timer via func_8009AB54 and stores to entity timer field.
 *
 * @note Non-matching: switch binary search generates inverted branch
 * direction (bnez vs beqz for the <2 check) and merges cases 0/3/default,
 * eliminating the explicit case-3 comparison.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_8009AD7C);

/**
 * @brief State machine dispatch for battle transitions.
 *
 * Switch on (a0 - 5) with 6 cases:
 * - case 5: Set D_800ED148 word 0 to 1
 * - case 6: Schedule func_8009AC14
 * - case 7: Schedule func_8009AC34
 * - case 8: Schedule func_8009ACB4
 * - case 9: Schedule func_8009AC68
 * - case 10: Schedule func_8009ACEC
 * @param a0 State transition command (5-10).
 *
 * @note Non-matching: switch generates jump table in .rodata at a
 * different address than the original jtbl_80098018 (in bc_dispatch .text).
 * Overlay decomp limitation: compiler-generated jump tables cannot be
 * placed at the original address.
 *
 * Best attempt:
 * @code
 * void func_8009AE08(s32 a0) {
 *     switch (a0) {
 *     case 5:  *(s32 *)D_800ED148 = 1; break;
 *     case 6:  func_8009AF14((s32)func_8009AC14); break;
 *     case 7:  func_8009AF14((s32)func_8009AC34); break;
 *     case 8:  func_8009AF14((s32)func_8009ACB4); break;
 *     case 9:  func_8009AF14((s32)func_8009AC68); break;
 *     case 10: func_8009AF14((s32)func_8009ACEC); break;
 *     }
 * }
 * @endcode
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_8009AE08);

/**
 * @brief Update battle phase transition sound.
 *
 * Checks 0x12E8 for phase 2 and compares 0x12ED with 0x12EE.
 * If different, plays sound 0x6D (or 0x6C if 0x12ED == 1) at
 * volume 0xF0. Copies 0x12ED to 0x12EE.
 */
/**
 * @note Non-matching: maspsx schedules sb into lw-ra load delay slot
 * and fills jr-ra branch delay slot with addiu-sp (FILLED epilogue),
 * but original has UNFILLED epilogue (4 instructions shorter).
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_8009AE9C);

/**
 * @brief Schedule a callback function as a battle task.
 *
 * Wraps the given function pointer with sound ID 0xA at volume 0x80
 * by calling func_8009B134, effectively queueing it for execution.
 * @param a0 Function pointer to schedule as a task callback.
 */
void func_8009AF14(s32 a0) {
    func_8009B134(0xA, 0x80, a0);
}

/**
 * @brief Queue a custom sound command with full parameters.
 *
 * Plays sound ID 8 at volume a3, then stores entity ID, flags,
 * and extra parameter into the command buffer.
 * @param a0 Entity data pointer for the command target.
 * @param a1 Entity ID to store in command.
 * @param a2 Flag byte 1.
 * @param a3 Volume / priority.
 * @param stack_arg Extra flag byte 2 (5th argument on stack).
 */
s32 func_8009AF3C(s32 a0, s32 a1, s32 a2, s32 a3, s32 stack_arg) {
    s32 entity_id = a1;
    s32 flag1 = a2;
    u8 *buf;
    s32 flag2 = stack_arg;

    buf = (u8 *)func_8009B134(8, a3, a0);
    *(u16 *)buf = entity_id;
    *(u8 *)(buf + 2) = flag1;
    *(u8 *)(buf + 3) = flag2;
}

/**
 * @brief Queue a status effect animation command.
 *
 * Snapshots entity state via func_8009AFF0, then plays sound 0x75
 * at volume 0x80 targeting the entity. Stores entity index.
 * @param a0 Entity slot index.
 */
void func_8009AF98(s32 a0) {
    s32 idx = a0;
    s32 offset;
    u8 *buf;

    func_8009AFF0(idx);
    offset = ((idx * 2 + idx) * 4 + idx) * 16;
    buf = (u8 *)func_8009B134(0x75, 0x80, offset + (s32)D_800ED158);
    *(u16 *)buf = idx;
}

/**
 * @brief Snapshot entity animation state before a command.
 *
 * Copies status (0x90) to backup (0x92) and flags (0x18) to backup
 * (0x1C) for the given entity. If entity index >= 3, checks linked
 * entity data flags and conditionally clears bit 6 of status and
 * bit 0x2000 of flags.
 * @param a0 Entity slot index.
 */
s32 func_8009AFF0(s32 a0) {
    s32 base = (s32)D_800ED148;
    s32 offset = ((a0 * 2 + a0) * 4 + a0) * 16;
    u8 *entity = (u8 *)(offset + base);
    s32 flags;
    u8 *linked;
    s32 linkedFlags;

    *(u16 *)(entity + 0x92) = *(u16 *)(entity + 0x90);
    flags = *(s32 *)(entity + 0x18);
    *(s32 *)(entity + 0x1C) = flags;

    if (a0 >= 3) {
        linked = *(u8 **)(*(s32 *)(entity + 0x10));
        linkedFlags = *(u8 *)(linked + 0xF7);
        if (linkedFlags & 1) {
            *(u16 *)(entity + 0x92) = *(u16 *)(entity + 0x92) & 0xFFBF;
        }
        linkedFlags = *(u8 *)(linked + 0xF7);
        if (linkedFlags & 2) {
            *(s32 *)(entity + 0x1C) = *(s32 *)(entity + 0x1C) & ~0x2000;
        }
    }
}

/**
 * @brief Queue an effect command with directional flag.
 *
 * Snapshots state, then plays sound 0x76 or 0x77 based on direction
 * flag, at volume 0xF0. Stores entity ID and two flag bytes.
 * @param a0 Entity slot index.
 * @param a1 Direction flag (0 = sound 0x76, nonzero = sound 0x77).
 * @param a2 Flag byte 1.
 * @param a3 Flag byte 2.
 */
void func_8009B088(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 idx = a0;
    s32 dir = a1;
    s32 flag1 = a2;
    s32 flag2 = a3;
    s32 snd;
    u8 *buf;

    func_8009AFF0(idx);
    snd = 0x77;
    if (dir == 0) {
        snd = 0x76;
    }
    buf = (u8 *)func_8009B134(snd, 0xF0, 0);
    *(u16 *)buf = idx;
    buf[2] = flag1;
    buf[3] = flag2;
}

/**
 * @brief Conditionally play a timed sound effect.
 *
 * If bit 1 of D_80082C0A is clear, plays a timed sound via
 * func_80013744 using D_8005F11C as the sound ID.
 * @param a0 Duration parameter for the timed sound.
 */
void func_8009B0F8(s32 a0) {
    if (!(*(u16 *)D_80082C0A & 2)) {
        func_80013744(D_8005F11C, a0, 0);
    }
}

/**
 * @brief Play a sound effect with sign-extended ID and masked volume.
 *
 * Sign-extends a0 from 16 bits, masks a1 to 8 bits, then calls
 * func_800B8564 to play the sound.
 * @param a0 Sound effect ID (16-bit, sign-extended).
 * @param a1 Volume (masked to 8 bits).
 * @param a2 Target entity data pointer (passed through).
 * @return Result from func_800B8564.
 */
s32 func_8009B134(s32 a0, s32 a1, s32 a2) {
    return func_800B8564((s16)a0, a1 & 0xFF);
}

/**
 * @brief Get next random value from shuffle buffer.
 *
 * Reads the current index from D_800EEBB0, uses it to index into
 * D_800EEBA8, increments the value at that position, then returns
 * the corresponding byte from D_80098030.
 * @return Random byte value from the lookup table.
 */
s32 func_8009B15C(void) {
    s32 idx = *(u8 *)D_800EEBB0;
    u8 *entry = D_800EEBA8 + idx;
    s32 val = *entry;
    *entry = val + 1;
    return *(u8 *)(D_80098030 + val);
}

/**
 * @brief Initialize shuffle buffer with random permutation.
 *
 * Clears D_800EEBB0, fills 8-entry shuffle buffer D_800EEBA8 with
 * sequential random values from func_8009B15C, then sets D_800EEBB0
 * to the final value & 7.
 * @param a0 Initial seed value stored into each buffer entry.
 */
void func_8009B198(s32 a0) {
    s32 buf;
    s32 i;

    *(u8 *)D_800EEBB0 = 0;
    i = 0;
    buf = (s32)D_800EEBA8;

    do {
        *(u8 *)(i + buf) = a0;
        a0 = func_8009B15C() & 0xFF;
        i++;
    } while (i < 8);

    *(u8 *)D_800EEBB0 = func_8009B15C() & 7;
}

/**
 * @brief Clear a color buffer array and set terminator.
 *
 * Zeros out a0[0..2] for each of a2 entries (4-byte stride), then
 * stores 0xFF into a1[0] as a terminator.
 * @param a0 Pointer to color buffer entries.
 * @param a1 Pointer to terminator byte.
 * @param a2 Number of entries to clear.
 */
void func_8009B208(u8 *a0, u8 *a1, s32 a2) {
    s32 i = 0;
    if (a2 > 0) {
top:
        a0[2] = 0;
        a0[1] = 0;
        a0[0] = 0;
        asm("");
        i++;
        if (i < a2) {
            a0 += 4;
            goto top;
        }
    }
    *a1 = 0xFF;
}

/**
 * @brief Find first entry with value 0xFF in a buffer.
 *
 * Scans up to a1 entries (4-byte stride) looking for a0[0] == 0xFF.
 * Returns the index (as u8) of the matching entry.
 * @param a0 Pointer to buffer entries.
 * @param a1 Maximum number of entries to scan.
 * @return Index of the first 0xFF entry, or 0 if not found.
 */
s32 func_8009B238(u8 *a0, s32 a1) {
    s32 i;
    for (i = 0; i < a1; i++) {
        if (a0[0] == 0xFF) {
            return (u8)i;
        }
        a0 += 4;
    }
    return 0;
}

/**
 * @brief Find first empty (inactive) entry in a buffer.
 *
 * Scans up to a1 entries (4-byte stride) looking for one where
 * both a0[0] and a0[1] are zero. Returns the entry index.
 * @param a0 Pointer to buffer entries.
 * @param a1 Maximum number of entries to scan.
 * @return Index of the first empty entry.
 */
s32 func_8009B270(u8 *a0, s32 a1) {
    s32 i;
    for (i = 0; i < a1; i++) {
        if (a0[1] == 0xFF) {
            return i;
        }
        a0 += 4;
    }
}

/**
 * @brief Allocate a task queue slot and link it.
 *
 * Finds an empty slot via func_8009B390, writes the source entry
 * data into it, and updates the doubly-linked list pointers.
 * @param a0 Pointer to task entry array.
 * @param a1 Pointer to head index.
 * @param a2 Number of slots.
 * @return Allocated slot index (as u8).
 */
s32 func_8009B2A4(u8 *table, u8 *head, s32 count) {
    s32 slot = (u8)func_8009B390(table, count);
    u8 *entry = (u8 *)(slot * 4 + (s32)table);

    entry[0] = *head;
    entry[1] = 0xFF;
    entry[2] = 0;

    if (*head != 0xFF) {
        table[*head * 4 + 1] = slot;
    }

    *head = slot;
    return slot;
}

/**
 * @brief Remove a task entry and relink neighbors.
 *
 * Given slot index a0, updates forward/backward links of adjacent
 * entries. Clears the removed entry. If the removed entry's forward
 * link is not 0xFF, updates that entry's backward link. Stores
 * new head if needed.
 * @param a0 Slot index to remove (masked to u8).
 * @param a1 Pointer to task entry array.
 * @param a2 Pointer to head index storage.
 */
void func_8009B320(s32 a0, u8 *a1, u8 *a2) {
    u8 *entry;
    s32 sentinel;
    s32 fwd;

    entry = (u8 *)((a0 & 0xFF) * 4 + (s32)a1);
    sentinel = 0xFF;

    fwd = entry[0];
    if (fwd != sentinel) {
        a1[fwd * 4 + 1] = entry[1];
    }
    fwd = entry[1];
    if (fwd != sentinel) {
        a1[fwd * 4] = entry[0];
    } else {
        *a2 = entry[0];
    }
    {
        u8 *e2 = (u8 *)((a0 & 0xFF) * 4 + (s32)a1);
        e2[2] = 0;
        e2[1] = 0;
        e2[0] = 0;
    }
}

/**
 * @brief Find first active entry in a linked buffer.
 *
 * Scans up to a1 entries (4-byte stride). An entry is active if
 * either byte 0 or byte 1 is nonzero. Returns the index of the
 * first inactive (both zero) entry found, or falls through.
 * @param a0 Pointer to buffer entries.
 * @param a1 Maximum number of entries to scan.
 * @return Index of the first inactive entry.
 */
s32 func_8009B390(u8 *a0, s32 a1) {
    s32 i;
    for (i = 0; i < a1; i++) {
        if (a0[0] == 0 && a0[1] == 0) {
            return (u8)i;
        }
        a0 += 4;
    }
}

/**
 * @brief Queue a task into the battle command queue.
 *
 * Allocates a slot in D_800EE24B (16 entries, head at +0x1F3),
 * multiplies slot index by 16 for the data offset, stores the
 * callback pointer, and returns the slot index as s16.
 * @param a0 Callback function pointer.
 * @return Allocated slot index (sign-extended to s32).
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_8009B3D0);

/**
 * @brief Reset the task queue and associated data.
 *
 * Clears all 16 entries in D_800EE24B via func_8009B208, then
 * iterates 16 slots zeroing out extended data at offset 0x1153
 * (with -0x10 stride, descending).
 */
void func_8009B428(void) {
    register s32 base asm("$16") = D_800EE24B;
    s32 i;

    func_8009B208((u8 *)base, (u8 *)(base + 0x1F3), 0x10);

    i = 15;
    base += -0x1013;
    do {
        *(u8 *)(base + 0x1153) = 0;
        i--;
        base += -0x10;
    } while (i >= 0);
}

/**
 * @brief Execute pending tasks from the task queue.
 *
 * Scans 16 queue entries for the first active one (byte at 0x1103
 * == 0xFF). Loads and calls the callback function pointer from the
 * corresponding data slot via jalr. Continues processing linked
 * entries until terminator (0xFF) or abort flag (0x12F6).
 */
void func_8009B478(void) {
    s32 i = 0;
    s32 sentinel = 0xFF;
    s32 base = (s32)D_800ED148;
    s32 base2;
    s32 entry;

    do {
        if (*(u8 *)(base + 0x1103) == sentinel) {
            goto found;
        }
        i++;
        base += 4;
    } while (i < 16);
    return;

loop:
    i = *(u8 *)(entry + 0x1104);
found:
    base2 = (s32)D_800ED148;
    {
        s32 off;
        void (*fn)(s32);

        fn = (void (*)(s32))*(s32 *)(base2 + i * 16 + 0x1144);
        fn(i);
        off = i * 4;
        entry = off + base2;
        if (*(u8 *)(entry + 0x1104) == 0xFF) {
            return;
        }
        if (*(u8 *)(base2 + 0x12F6) != 0xFF) {
            goto loop;
        }
    }
}

/**
 * @brief Process completed tasks and remove them from queue.
 *
 * Iterates 16 queue slots. For each with completion flag == 1
 * (at offset 0x1153), clears the flag and calls func_8009B320
 * to unlink and free the slot.
 */
void func_8009B520(void) {
    s32 i = 0;
    s32 one = 1;
    s32 base = (s32)D_800ED148;
    s32 ptr = base + ptr - ptr;

top:
    if (*(u8 *)(ptr + 0x1153) == one) {
        *(u8 *)(ptr + 0x1153) = 0;
        func_8009B320(i & 0xFF, (u8 *)(base + 0x1103), (u8 *)(base + 0x12F6));
    }
    i++;
    if (i < 16) {
        ptr += 0x10;
        goto top;
    }
}

/**
 * @brief Read a pair of values from a lookup table.
 *
 * Indexes into D_800E19BC at offset a0*8, reads two consecutive
 * s32 values and stores them to the output pointers.
 * @param a0 Table index.
 * @param a1 Output pointer for first value.
 * @param a2 Output pointer for second value.
 *
 * @note Non-matching: CSE merges two separate `base + offset` address
 * computations into one, using a single addu and pointer for both loads.
 * Original computes `addu v1, a0, v0` and `addu v0, v0, a0` separately
 * (redundant but different registers). GCC -O2 CSE cannot be prevented.
 *
 * Best attempt:
 * @code
 * void func_8009B59C(s32 a0, s32 *a1, s32 *a2) {
 *     s32 base = (s32)D_800E19BC;
 *     REGALLOC_BARRIER(base);
 *     a0 <<= 3;
 *     *a1 = *(s32 *)(a0 + base);
 *     *a2 = *(s32 *)(base + a0 + 4);
 * }
 * @endcode
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_8009B59C);

/**
 * @brief Start music/sound playback with callback.
 *
 * Reads source parameters from D_800E19BC, calls either
 * func_8003882C or func_80038868 based on direction flag (a2),
 * passing func_8009B654 as callback. Stores callback pointer
 * and data pointer into D_800ED148+0x128C/0x12D8.
 * @param a0 Sound table index.
 * @param a1 Sound bank ID.
 * @param a2 Direction flag (0 = func_8003882C, else func_80038868).
 * @param a3 Callback user data.
 *
 * @note Non-matching: CSE merges two D_800E19BC address computations into one
 * addu (original has addu v1,a0,v0 + addu v0,v0,a0). With asm barrier + register
 * constraint, matches except for sw s1 prologue position. Without hacks, CSE
 * causes a1 to go to $v1 instead of $t0 (v0/v1 not occupied by double addu).
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object1", func_8009B5C4);

/**
 * @brief Music playback completion callback.
 *
 * Called when music playback finishes. If the stored callback at
 * D_800ED148+0x128C is non-null, calls it with the stored data
 * pointer from D_800ED148+0x12D8.
 */
void func_8009B654(void) {
    s32 base = (s32)D_800ED148;
    s32 callback = *(s32 *)(base + 0x128C);
    if (callback != 0) {
        ((void (*)(s32))callback)(*(s32 *)(base + 0x12D8));
    }
}

/**
 * @brief Yield execution (wrapper for func_800393C8).
 *
 * Calls the system yield/wait function to give up the current
 * execution timeslice.
 */
void func_8009B690(void) {
    func_800393C8();
}

/**
 * @brief Yield execution (identical wrapper for func_800393C8).
 *
 * Second yield wrapper, functionally identical to func_8009B690.
 * Both exist as separate symbols for different call contexts.
 */
void func_8009B6B0(void) {
    func_800393C8();
}

/**
 * @brief Load and play a sound bank segment.
 *
 * Computes VRAM page from a0 (shifted left 7), reads base pointer
 * from D_800E19B4, calls func_80038920 to load the segment, then
 * calls func_80020644 to start playback.
 *
 * @param a0 Sound bank index (u16).
 * @param a1 Playback parameters pointer.
 */
void func_8009B6D0(s32 a0, s32 a1) {
    s32 temp;
    s32 temp2;
    s32 new_var;
    a0 = (a0 & 0xFFFF) << 7;
    new_var = a0;
    temp = new_var;
    if (new_var < 0) {
        temp = new_var + 0x7FF;
    }
    func_80038920(*(s32 *)D_800E19B4 + (temp >> 11), 0x800, 0x801C0000, 0);
    func_80020644((new_var & 0x7FF) | 0x801C0000, a1, 0x80);
}

/**
 * @brief Probability check using random shuffle buffer.
 *
 * Computes (a0 * 255) / a1 as threshold, gets a random value from
 * func_8009B15C, and returns 1 if the random value is less than
 * the threshold (and threshold is nonzero), else 0.
 * @param a0 Numerator for probability (0-255 range).
 * @param a1 Denominator for probability.
 * @return 1 if random check passes, 0 otherwise.
 */
s32 func_8009B74C(s32 a0, s32 a1) {
    s32 threshold = (a0 * 255) / a1;
    s32 rnd = func_8009B15C();
    if (threshold != 0 && (u32)threshold >= (u32)rnd) {
        return 1;
    }
    return 0;
}

/**
 * @brief Wrapper for probability check (func_8009B74C).
 * @param a0 Numerator.
 * @param a1 Denominator.
 */
void func_8009B79C(s32 a0, s32 a1) {
    func_8009B74C(a0, a1);
}

/**
 * @brief Random value in range [1, a0] using modulo.
 *
 * Gets a random value from func_8009B15C, computes modulo a0,
 * and adds 1.
 * @param a0 Upper bound (exclusive before +1).
 * @return Random value in [1, a0].
 */
s32 func_8009B7BC(s32 a0) {
    s32 mod = a0;
    return func_8009B15C() % mod + 1;
}

/**
 * @brief Clamp a damage value to valid range.
 *
 * If D_800EE4C1 == 0xED (special battle mode) and the entity at
 * slot a1 has field 0x28 == 0, returns 0. Otherwise, checks
 * D_800EE456 bit 3: if set, max is 60000 (0xEA60); else max is
 * 9999 (0x270F). Returns clamped value: max if a0 > max, 0 if
 * a0 < 0, else a0.
 * @param a0 Raw damage value.
 * @param a1 Entity slot index.
 * @return Clamped damage value.
 */
s32 func_8009B7F4(s32 a0, s32 a1) {
    s32 max;

    if (*(u8 *)D_800EE4C1 == 0xED) {
        s32 base = (s32)D_800ED148;
        s32 offset = ((a1 * 2 + a1) * 4 + a1) * 16;
        if (*(s32 *)(base + offset + 0x28) == 0) {
            return 0;
        }
    }
    max = 9999;
    if (*(u8 *)D_800EE456 & 8) {
        max = 60000;
    }
    if (a0 > max) {
        return max;
    }
    if (a0 < 0) {
        return 0;
    }
    return a0;
}

/**
 * @brief Apply status effect flags to entity based on linked data.
 *
 * For entities with index >= 3, checks linked entity data at offset
 * 0xF7 for status immunity flags. Bit 0 controls the 0x40 flag on
 * status (u16 at a1), bit 1 controls the 0x2000 flag on ability
 * flags (s32 at a2). The a3 parameter determines set vs clear.
 * @param a0 Entity slot index.
 * @param a1 Pointer to entity status u16.
 * @param a2 Pointer to entity ability flags s32.
 * @param a3 Mode: 0 = clear flags, nonzero = set flags.
 */
void func_8009B878(s32 a0, u16 *a1, s32 *a2, s32 a3) {
    s32 base;
    s32 offset;
    u8 *linked;
    s32 status;
    s32 flags;

    if (a0 < 3) {
        return;
    }
    base = (s32)D_800ED148;
    offset = ((a0 * 2 + a0) * 4 + a0) * 16;
    linked = *(u8 **)(*(s32 *)(offset + base + 0x10));
    status = *a1;
    if (status & 0x40) {
        if (*(u8 *)(linked + 0xF7) & 1) {
            if (a3 != 0) {
                *a1 = status | 0x40;
            } else {
                *a1 = status & 0xFFBF;
            }
        }
    }
    flags = *a2;
    if (flags & 0x2000) {
        if (*(u8 *)(linked + 0xF7) & 2) {
            if (a3 == 0) {
                *a2 = flags & ~0x2000;
            } else {
                *a2 = flags | 0x2000;
            }
        }
    }
}

/**
 * @brief Apply status effects from a bitmask to an entity.
 *
 * Iterates 14 status bits. For each bit set in a2, calls
 * func_800B0668 to check if the effect should be removed; if so,
 * clears that bit. Then iterates again calling func_800B0600 for
 * remaining set bits. Finally calls func_8009B878 to apply linked
 * entity flags.
 * @param a0 Entity slot index.
 * @param a1 Bitmask of status effects to clear from entity.
 * @param a2 Bitmask of status effects to apply.
 */
void func_8009B924(s32 slot, s32 clearMask, s32 applyMask) {
    BattleEntity *entities;
    s32 bit;
    s32 i;

    ((BattleEntity *)D_800ED148)[slot].status &= ~clearMask;

    for (bit = 1, i = 0; i < 14; i++, bit <<= 1) {
        if (applyMask & bit) {
            if (func_800B0668(slot, bit))
                applyMask &= ~bit;
        }
    }

    ((BattleEntity *)D_800ED148)[slot].flags &= ~applyMask;

    for (bit = 1, i = 0; i < 14; i++, bit <<= 1) {
        if (applyMask & bit)
            func_800B0600(slot, bit);
    }

    func_8009B878(slot,
        &((BattleEntity *)D_800ED148)[slot].status,
        (s32 *)&((BattleEntity *)D_800ED148)[slot].flags, 1);
}

/**
 * @brief Get combined status flags for an entity from ability data.
 *
 * Reads ability flags for the given entity slot from g_gfData,
 * queries two flag lookup functions, OR's results together.
 * If bit 15 is set in the combined result, returns it masked to u16;
 * otherwise returns defaultStatus masked to u16.
 * @param slot Entity slot index.
 * @param defaultStatus Default status value if no ability flags apply.
 * @return Combined or default status flags (u16).
 */
s32 func_8009BA5C(s32 slot, s32 defaultStatus) {
    GfDataSubS *gf = (GfDataSubS *)g_gfData;
    unsigned short result;

    result = func_800B0F9C(gf->subTableS[slot].abilityFlags);
    result |= func_800B0F7C(gf->subTableS[slot].abilityFlags);

    if (result & 0x8000)
        return (u16)result;
    return (u16)defaultStatus;
}
