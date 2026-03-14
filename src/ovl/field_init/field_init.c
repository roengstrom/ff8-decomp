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

INCLUDE_ASM("asm/ovl/field_init/nonmatchings/field_init", func_800982D8);

INCLUDE_ASM("asm/ovl/field_init/nonmatchings/field_init", func_80098330);

/** @brief Calls func_80098330 then func_800983B8 in sequence. */
void func_80098390(void) {
    func_80098330();
    func_800983B8();
}

INCLUDE_ASM("asm/ovl/field_init/nonmatchings/field_init", func_800983B8);
