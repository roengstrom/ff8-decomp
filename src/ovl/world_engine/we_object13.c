#include "common.h"

extern void func_80043578(s32);
extern void func_8004358C(s32);
extern void func_800435A0(s32, s32, s32);

/** Initializes audio/display subsystem. */
void func_800C40F8(void) {
    func_80043578(0);
    func_8004358C(0);
    func_800435A0(9, 0, 0);
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4130);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C42D8);

extern u8 D_800E3E70[];

/** Resets playback state and reinitializes subsystem.
 * @return Always returns 1.
 */
s32 func_800C4450(void) {
    s32 base = (s32)D_800E3E70;
    *(s32 *)(base + 0x10) = 0;
    *(volatile u8 *)(base + 0x25) = 0;
    func_800C40F8();
    return 1;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4480);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4558);

extern s32 D_800E3E60;

/** Exchanges the callback pointer, returning the previous value.
 * @param newVal New callback value to store.
 * @return Previous callback value.
 */
s32 func_800C4634(s32 newVal) {
    s32 old = D_800E3E60;
    D_800E3E60 = newVal;
    return old;
}

/** Clears all playback state and callback pointer. */
void func_800C4644(void) {
    s32 base = (s32)D_800E3E70;
    D_800E3E60 = 0;
    *(s32 *)(base + 0x00) = 0;
    *(s32 *)(base + 0x04) = 0;
    *(s32 *)(base + 0x08) = 0;
    *(s32 *)(base + 0x0C) = 0;
    *(s32 *)(base + 0x10) = 0;
    *(s32 *)(base + 0x14) = 0;
    *(s32 *)(base + 0x18) = 0;
    *(s32 *)(base + 0x1C) = 0;
    *(s32 *)(base + 0x20) = 0;
    *(u8 *)(base + 0x24) = 0;
    *(volatile u8 *)(base + 0x25) = 0;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4688);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C48C0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C492C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C49CC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4A58);

extern s32 getPackedField2Bit(s32);
extern s32 fieldRandom(void);
extern void func_800383B8(s32, s32);

/** Scans all 256 entries and processes active ones. */
void func_800C4A74(void) {
    s32 i;
    s32 result;
    for (i = 0; i < 256; i++) {
        result = getPackedField2Bit(i) - 1;
        if ((u32)result < 2) {
            if (fieldRandom() & 1) {
                func_800383B8(i, result);
            }
        }
    }
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object13", func_800C4AE4);
