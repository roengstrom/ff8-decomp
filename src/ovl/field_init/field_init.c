#include "common.h"
#include "cd.h"

/** @brief Field initialization entry: call setup then init step. */
void func_80098000(void) {
    func_8001F5C8();
    func_800980B0();
}

/**
 * @brief Initialize battle command state and load battle data from CD.
 *
 * Clears the battle command descriptor at D_80082C08, resets the
 * D_8005F170 flag, fills 3 process-state bytes with 0xFF, then
 * reads battle data from disc (D_80097800) into a scratch buffer
 * and copies it to the battle data buffer (D_80078E00).
 */
void func_80098028(void) {
    extern u8 D_80082C08[];
    extern s8 D_8005F170;
    extern CdFileDesc D_80097800;
    extern u8 D_80078E00[];

    s32 fillVal = 0xFF;
    s32 i = 2;
    u8 *base = D_80082C08;
    u8 *ptr = base + 2;

    *(u16 *)D_80082C08 = 0;
    *(u16 *)(base + 2) = 0;
    *(u8 *)(base + 8) = 0;
    D_8005F170 = 0;

    do {
        *(u8 *)(ptr + 4) = fillVal;
        i--;
        ptr--;
    } while (i >= 0);

    func_80038920(D_80097800.sector, D_80097800.size, 0x801A0000, 0);
    func_80020644((u8 *)0x801A0000, D_80078E00, D_80097800.size);
}

/**
 * @brief Wrapper that calls func_80098028 (field init step).
 */
void func_800980B0(void) {
    func_80098028();
}

/**
 * @brief Initialize memory card events and battle entity slots.
 *
 * Sets up the battle/entity system, initializes the memory card driver,
 * opens and enables 8 PsyQ card events (HwCARD + SwCARD for IOE, ERROR,
 * TIMEOUT, NEW specs), then creates 4x2 battle entity slots.
 */
void func_800980D0(void) {
    extern s32 D_80082FB4[];

    s32 *events;
    s32 i;
    s32 j;

    func_800287B0(-1);
    func_8004D8C4(0);
    func_8004D930();
    events = D_80082FB4;
    func_800471A4();
    func_8004D844(0);
    j = 0;
    func_800472E4(); /* EnterCriticalSection */

    events[0] = func_80047204(0xF4000001, 4, 0x2000, 0);
    events[1] = func_80047204(0xF4000001, 0x8000, 0x2000, 0);
    events[2] = func_80047204(0xF4000001, 0x100, 0x2000, 0);
    events[3] = func_80047204(0xF4000001, 0x2000, 0x2000, 0);
    events[4] = func_80047204(0xF0000011, 4, 0x2000, 0);
    events[5] = func_80047204(0xF0000011, 0x8000, 0x2000, 0);
    events[6] = func_80047204(0xF0000011, 0x100, 0x2000, 0);
    events[7] = func_80047204(0xF0000011, 0x2000, 0x2000, 0);

    func_80047244(events[0]);
    func_80047244(events[1]);
    func_80047244(events[2]);
    func_80047244(events[3]);
    func_80047244(events[4]);
    func_80047244(events[5]);
    func_80047244(events[6]);
    func_80047244(events[7]);

    func_800472F4(); /* ExitCriticalSection */

    do {
        i = 0;
        do {
            s32 entity = func_800287BC(i, j);
            func_80028B58(entity);
            func_80028AF8(entity, 0);
        } while (++i < 2);
        i = 0;
    } while (++j < 4);
}

/**
 * @brief Wrapper that calls func_8004DF84 (memory card initialization).
 */
void func_800982B8(void) {
    func_8004DF84();
}

/**
 * @brief Set memory card event status to "ready" (0x404) for 4 card slots.
 *
 * Reads the event table base from ECB+0x10, then writes 0x404 (EvStACTIVE |
 * EvMdNOINTR) to the status field (+0x94) of each of 4 card event entries
 * (stride 0xC0) within a critical section.
 */
void func_800982D8(void) {
    s32 ecb_base = 0x100;

    func_800472E4(); /* EnterCriticalSection */

    {
        s32 i = 0;
        s32 val = 0x404;
        s32 offset = i;
        s32 base;

        top:
        base = *(s32 *)(ecb_base + 0x10);
        i++;
        *(s32 *)(base + offset + 0x94) = val;
        offset += 0xC0;
        if (i < 4) goto top;
    }

    func_800472F4(); /* ExitCriticalSection */
}

/**
 * @brief Decode table data from D_80098960 into D_8008369C.
 *
 * First byte is a start index, second is the limit. Copies pairs of
 * bytes from the source data into the destination buffer until the
 * index exceeds the limit.
 */
void func_80098330(void) {
    extern u8 D_80098960[];
    u8 *src = D_80098960;
    extern u8 D_8008369C[];
    u8 *dst = D_8008369C;
    s32 count;
    u8 byte;
    s32 limit;

    src++;
    count = D_80098960[0];
    limit = *src;
    src++;

    if (limit < count) return;

    do {
        *dst++ = *src++;
        count++;
        *dst++ = (byte = *src++);
    } while (!(limit < count));
}

/** @brief Calls func_80098330 then func_800983B8 in sequence. */
void func_80098390(void) {
    func_80098330();
    func_800983B8();
}

/**
 * @brief Initialize two render context structs for the field display system.
 *
 * Sets global display parameters (default color intensity, blend mode) in the
 * BattleAnimEntity header area at D_80082DD0:
 *   - D_80082DD0[0x1E0] = 0x10 (default color intensity)
 *   - D_80082DD0[0x1E1] = 5 (blend mode)
 *   - D_80082DD0[0x1E3] = 0
 *
 * Calls func_80027558 with a full-screen RECT(0, 0, 320, 224) to set display
 * bounds, then loops twice (i = 0..1) to initialize each of the two 0xC4-byte
 * BattleAnimEntity structs:
 *   1. Calls func_8002828C(i) to copy default color into entity RGBX fields
 *   2. Sets field19 (active flag) to 1
 *   3. Sets unk10[0..3] to {0xFFF, 0x5000, 0xA000, 0x900}
 *   4. Clears frame data bytes (entry[1..7] offset +6) with a fill loop
 *   5. Sets pad00[0] to 0x40 and clears pad00[6]
 *   6. Runs a delay spin loop (5 iterations)
 *   7. Fills padBC[0..5] with 0xFF (opacity region)
 *   8. Clears opacity (0x1B) and field0B (0x0B)
 *   9. Calls func_80027FBC(i, 0, 0) to reset display coordinates
 *  10. Sets fieldC3 to 0x31 and linkedIdx to i
 *  11. Clears field0C..field0F (color fields)
 *
 * @note Non-matching. Two toolchain differences prevent byte-matching:
 *   (1) CC1PSX -G0 address expansion uses $v0 as intermediate
 *       (lui $v0; addiu $s0,$v0), but the original has direct $s0
 *       (lui $s0; addiu $s0,$s0) suggesting ASPSX pseudo expansion.
 *   (2) CC1PSX -O2 optimizes away a redundant register copy
 *       (addu $v0,$v1,$zero before sb) that the original retains,
 *       causing the function to be 1 instruction shorter and all
 *       subsequent instructions to shift. Permuter score: 715.
 */
INCLUDE_ASM("asm/ovl/field_init/nonmatchings/field_init", func_800983B8);
