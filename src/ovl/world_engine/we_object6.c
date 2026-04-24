#include "common.h"

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800ACC68);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800ACD38);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800ACDC4);

void func_800AD688(void) {
}

void func_800AD690(void) {
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800AD698);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800ADE24);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800AE0C8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800AE31C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800AE518);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800AEB58);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800AEEB0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800AF004);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800AF28C);

/**
 * @brief Inverse of @c func_800B00D8 — map a marker code back to raw kind.
 *
 * Accepts @p kind in @c [0, 0x84); returns the compact marker byte used
 * by surrounding slot code. Out-of-range or unmapped kinds return @c -1.
 */
s32 func_800B0010(u32 kind) {
    s32 out = -1;
    if (kind < 0x84) {
        switch (kind) {
            case 0x01: out = 4;    break;
            case 0x02: out = 5;    break;
            case 0x06: out = 6;    break;
            case 0x00:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x07:
            case 0x08:
            case 0x09: out = 0;    break;
            case 0x83: out = 0x46; break;
            case 0x22: out = 0x51; break;
            case 0x23: out = 0x52; break;
            case 0x24: out = 0x53; break;
            case 0x25: out = 0x54; break;
            case 0x26: out = 0x55; break;
            case 0x27: out = 0x56; break;
            case 0x28: out = 0x57; break;
            case 0x20: out = 0x4A; break;
            case 0x30: out = 0x40; break;
            case 0x31: out = 2;    break;
            case 0x32: out = 1;    break;
            case 0x41: out = 0x4D; break;
            case 0x42: out = 0x4E; break;
            case 0x21: out = 0x49; break;
        }
    }
    return out;
}

/**
 * @brief Remap a raw slot-kind byte into a compact marker code.
 *
 * Lookup helper used by world-engine slot classifiers. Accepts an 8-bit
 * @p kind; returns @c -1 for values >= @c 0x58 or for cases left blank in
 * the table. Covered cases (partial): menu items @c 0x00-0x06, level
 * markers @c 0x40/0x41, string groups @c 0x49-0x4E, and the @c 0x51-0x57
 * command row.
 */
s32 func_800B00D8(u32 kind) {
    s32 out = -1;
    if (kind < 0x58) {
        switch (kind) {
            case 0x00: out = 0;    break;
            case 0x04: out = 1;    break;
            case 0x05: out = 2;    break;
            case 0x06: out = 6;    break;
            case 0x46: out = 0x83; break;
            case 0x51: out = 0x22; break;
            case 0x52: out = 0x23; break;
            case 0x53: out = 0x24; break;
            case 0x54: out = 0x25; break;
            case 0x55: out = 0x26; break;
            case 0x56: out = 0x27; break;
            case 0x57: out = 0x28; break;
            case 0x4A:
            case 0x4B: out = 0x20; break;
            case 0x40:
            case 0x41: out = 0x30; break;
            case 0x02:
            case 0x03: out = 0x31; break;
            case 0x01: out = 0x32; break;
            case 0x4D: out = 0x41; break;
            case 0x4E: out = 0x42; break;
            case 0x49: out = 0x21; break;
        }
    }
    return out;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B01A0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B04CC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B0EAC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B1174);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B13B8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B164C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B18B8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B1BCC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B1FD0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B20E4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B21EC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B28C8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B2B6C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B2D94);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object6", func_800B2F5C);
