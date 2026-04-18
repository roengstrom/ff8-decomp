#include "common.h"

/** @brief Field entity (actor), 612 bytes (0x264). Same as "actor" in debug print. */
typedef struct {
    /* 0x000 */ u8 pad000[0x190];
    /* 0x190 */ s32 posX;
    /* 0x194 */ s32 posY;
    /* 0x198 */ s32 posZ;
    /* 0x19C */ u8 pad19C[0xA5];
    /* 0x241 */ u8 field_0x241;
    /* 0x242 */ u8 pad242[0x0A];
    /* 0x24C */ u8 field_0x24C;
    /* 0x24D */ u8 pad24D[0x02];
    /* 0x24F */ u8 field_0x24F;
    /* 0x250 */ u8 field_0x250;
    /* 0x251 */ u8 field_0x251;
    /* 0x252 */ u8 field_0x252;
    /* 0x253 */ u8 field_0x253;
    /* 0x254 */ u8 field_0x254;
    /* 0x255 */ u8 pad255[0x0F];
} Entity; /* 0x264 = 612 bytes */

/** @brief Animation parameter entry. */
typedef struct {
    /* 0x00 */ u8 pad00[0x09];
    /* 0x09 */ s8 field_09;
    /* 0x0A */ u8 field_0A;
    /* 0x0B */ u8 field_0B;
} AnimParam;

/** @brief System state block (at D_800704A8). */
typedef struct {
    /* 0x000 */ u8 mode;
    /* 0x001 */ u8 pad001;
    /* 0x002 */ s16 counter;
    /* 0x004 */ u8 pad004[0x0E];
    /* 0x012 */ u8 entityIndex[2];
    /* 0x014 */ u8 pad014[0x17C];
    /* 0x190 */ u8 slotActive[16];
} SystemState;

extern Entity *D_80085224;
extern SystemState D_800704A8;
extern u16 D_8005F118;
extern u16 D_8005F11A;
extern u16 D_8005F160;
extern u16 D_8005F162;

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_80098314);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800983F0);

/**
 * Zero 0x40 bytes at D_800704A8+0x1B8 (backwards loop).
 */
void func_80098934(void) {
    s32 i = 0x3F;
    volatile u8 *base = (u8 *)&D_800704A8;
    u8 *ptr = (u8 *)base + 0x3F;
    do {
        *(u8 *)(ptr + 0x1B8) = 0;
        i--;
        ptr--;
    } while (i >= 0);
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009895C);

void func_80099124(void) {
}

/** @brief Call fadeOutSfxFast for sound channels 0-7, then renderAndUpdateDisplay(1). */
void func_8009912C(void) {
    s16 i = 0;

    do {
        fadeOutSfxFast(i);
        i++;
    } while (i < 8);

    renderAndUpdateDisplay(1);
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_80099180);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_80099348);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009A0E8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009A2BC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009A4C0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009A7E8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009A8E0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009A920);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009AA64);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009AAC8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009AC9C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009AEC0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009B4A8);

/**
 * @brief Dispatch entity animation based on slot index and animation parameters.
 *
 * Looks up the entity by slot index, sets animation state, checks
 * screen position thresholds, then dispatches to func_8009B4A8 with
 * the appropriate animation field based on the parameter's field_0A value.
 *
 * @param slotIdx Slot index (0 or 1).
 * @param paramIdx Index into the animation parameter array.
 * @param params Animation parameter array.
 * @param multiplier Speed/direction multiplier for the animation.
 */
void func_8009B74C(s16 slotIdx, u16 paramIdx, AnimParam *params, s16 multiplier) {
    u8 entityIdx;

    entityIdx = D_800704A8.entityIndex[slotIdx];

    if (entityIdx == 0xFF) {
        return;
    }

    D_80085224[D_800704A8.entityIndex[(s16)slotIdx]].field_0x241 = params[paramIdx].field_0B;
    D_80085224[D_800704A8.entityIndex[slotIdx]].field_0x24C = 1;

    if (slotIdx == 1) {
        if (D_8005F160 > D_8005F118) {
            params[paramIdx].field_0A = 2;
        }
    } else {
        if (D_8005F162 > D_8005F11A) {
            params[paramIdx].field_0A = 2;
        }
    }

    switch (params[paramIdx].field_0A) {
    case 0:
        entityIdx = D_800704A8.entityIndex[slotIdx];
        func_8009B4A8(entityIdx, D_80085224[D_800704A8.entityIndex[slotIdx]].field_0x250, 0, (s8)(params[paramIdx].field_09 * multiplier));
        break;
    case 1:
        entityIdx = D_800704A8.entityIndex[slotIdx];
        func_8009B4A8(entityIdx, D_80085224[D_800704A8.entityIndex[slotIdx]].field_0x251, 0, (s8)(params[paramIdx].field_09 * multiplier));
        break;
    case 2:
        entityIdx = D_800704A8.entityIndex[slotIdx];
        func_8009B4A8(entityIdx, D_80085224[D_800704A8.entityIndex[slotIdx]].field_0x24F, 0, (s8)(params[paramIdx].field_09 * multiplier));
        break;
    case 3:
        entityIdx = D_800704A8.entityIndex[slotIdx];
        func_8009B4A8(entityIdx, D_80085224[D_800704A8.entityIndex[slotIdx]].field_0x252, 0, (s8)(params[paramIdx].field_09 * multiplier));
        break;
    case 4:
        entityIdx = D_800704A8.entityIndex[slotIdx];
        func_8009B4A8(entityIdx, D_80085224[D_800704A8.entityIndex[slotIdx]].field_0x253, 0, (s8)(params[paramIdx].field_09 * multiplier));
        break;
    case 5:
        entityIdx = D_800704A8.entityIndex[slotIdx];
        func_8009B4A8(D_800704A8.entityIndex[slotIdx], D_80085224[entityIdx].field_0x254, 0, (s8)(params[paramIdx].field_09 * multiplier));
        break;
    }
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009BB18);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009BD50);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009BEC8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009CEE8);

/**
 * Looks up a halfword from the D_800C32A0 table by index.
 *
 * @param a0 Table index (masked to 8 bits).
 * @return The halfword value at D_800C32A0[a0].
 */
s16 func_8009D234(s32 a0) {
    extern u8 D_800C32A0[];
    a0 &= 0xFF;
    return *(s16 *)(D_800C32A0 + a0 * 2);
}

/**
 * Looks up a halfword from the D_800C3320 table by index.
 *
 * @param a0 Table index (masked to 8 bits).
 * @return The halfword value at D_800C3320[a0].
 */
s16 func_8009D254(s32 a0) {
    extern u8 D_800C3320[];
    a0 &= 0xFF;
    return *(s16 *)(D_800C3320 + a0 * 2);
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009D274);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009D500);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009D598);

/**
 * Subtracts two 3-component short vectors, storing result as words.
 *
 * @param a0 Destination word array.
 * @param a1 Source vector A (s16 array).
 * @param a2 Source vector B (s16 array).
 */
void func_8009DED8(u8 *a0, u8 *a1, u8 *a2) {
    *(s32 *)(a0 + 0) = *(s16 *)(a1 + 0) - *(s16 *)(a2 + 0);
    *(s32 *)(a0 + 4) = *(s16 *)(a1 + 2) - *(s16 *)(a2 + 2);
    *(s32 *)(a0 + 8) = *(s16 *)(a1 + 4) - *(s16 *)(a2 + 4);
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009DF18);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009E338);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009E468);

/**
 * Extracts position data from two entity structures (offsets 0x190/0x194,
 * right-shifted by 12) and calls func_8009A0E8 with them.
 *
 * @param a0 First entity pointer.
 * @param a1 Second entity pointer.
 */
void func_8009E604(u8 *a0, u8 *a1) {
    s32 pos1[4];
    s32 pos2[4];
    s32 result[2];

    pos1[0] = *(s32 *)(a0 + 0x190) >> 12;
    pos1[1] = *(s32 *)(a0 + 0x194) >> 12;
    pos2[0] = *(s32 *)(a1 + 0x190) >> 12;
    pos2[1] = *(s32 *)(a1 + 0x194) >> 12;
    func_8009A0E8(pos1, pos2, result);
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009E660);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009ECA4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009F74C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009F7F4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009F8D0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009F990);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_8009FE18);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A0640);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A06F0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A0D6C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A0E54);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A0EB8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A0F34);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A0FB8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A10F4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A11E0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A1318);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A15C0);

/**
 * Clears fields at offsets 0, 1 (bytes) and 0xA, 0xC (halfwords) in the structure.
 *
 * @param a0 Pointer to the structure.
 */
void func_800A17A4(u8 *a0) {
    *(u8 *)(a0 + 0x0) = 0;
    *(u8 *)(a0 + 0x1) = 0;
    *(u16 *)(a0 + 0xA) = 0;
    *(u16 *)(a0 + 0xC) = 0;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A17B8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A19B8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A1BB8);

/**
 * If D_8005F0F8 byte at offset 0xE is 1, sets up a display region
 * (0x200 x 0xF0 at 0x100, 0x10) and calls func_80048EFC.
 */
void func_800A1C64(void) {
    extern s32 D_8005F0F8;
    extern s32 D_800C71E4;
    u8 *data = (u8 *)D_8005F0F8;

    if (*(u8 *)(data + 0xE) == 1) {
        s16 rect[4];
        rect[0] = 0x200;
        rect[1] = 0xF0;
        rect[2] = 0x100;
        rect[3] = 0x10;
        func_80048EFC(rect, D_800C71E4);
    }
}

/** @brief Initialize 3 entries in D_800D5F50 and D_800D61A8 arrays to -1. */
void func_800A1CC0(void) {
    extern u8 D_800D5F50[];
    extern u8 D_800D61A8[];
    s32 i = 0;
    s32 val = -1;
    u8 *a = D_800D5F50;
    u8 *b = D_800D61A8;

    do {
        *(s32 *)b = val;
        *(s32 *)a = val;
        a += 0x70;
        i++;
        b += 0x70;
    } while (i < 3);
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A1CFC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A2128);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A222C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A29C0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A2A30);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A2AF8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A2D2C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A2EA4);

/**
 * Initializes an object by calling a sequence of setup functions.
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800A2EE0(u8 *a0) {
    func_800A3534(a0);
    func_800A3018(a0);
    func_800A2F48(a0);
    func_800A2F70(a0 + 0x3720);
    func_800A2F70(a0 + 0x4B20);
}

/**
 * Clears 16 bytes at offset 0x190 (backwards loop).
 *
 * @param a0 Unused parameter.
 * @param a1 Pointer to the object structure base.
 */
void func_800A2F28(s32 a0, u8 *a1) {
    s32 i = 0xF;
    a1 += 0xF;
    do {
        *(u8 *)(a1 + 0x190) = 0;
        i--;
        a1--;
    } while (i >= 0);
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A2F48);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A2F70);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A2FE0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A3018);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A303C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A327C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A3488);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A3534);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A355C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A37A8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A38B4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A39D8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A3FE0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A42EC);

/**
 * Zero 8 bytes of D_8005F168 (backwards loop).
 */
void func_800A44D8(void) {
    extern u8 D_8005F168[];
    s32 i = 7;
    volatile u8 *base = D_8005F168;
    u8 *ptr = (u8 *)base + 7;
    do {
        *ptr = 0;
        i--;
        ptr--;
    } while (i >= 0);
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A4500);

/**
 * Stores a halfword value to the global D_8005F122.
 *
 * @param a0 The value to store.
 */
void func_800A4550(s16 a0) {
    extern s16 D_8005F122;
    D_8005F122 = a0;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A455C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A4758);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A48CC);

/**
 * Linear interpolation: a0 + (a1 - a0) * a3 / a2.
 *
 * @param a0 Start value.
 * @param a1 End value.
 * @param a2 Divisor.
 * @param a3 Numerator.
 * @return Interpolated value.
 */
s32 func_800A4910(s32 a0, s32 a1, s32 a2, s32 a3) {
    a1 -= a0;
    return a0 + a1 * a3 / a2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A4934);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A4C14);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A5224);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A5360);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A553C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A5698);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A5700);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A5748);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A5788);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A5898);

/**
 * If D_8005F14A equals 1, calls resetCdDrive. Then clears D_8005F100 and D_8005F14A.
 *
 * @param a0 Pointer to the script/object structure (unused).
 */
void func_800A59D0(u8 *a0) {
    extern s16 D_8005F14A;
    extern s16 D_8005F100;

    if (D_8005F14A == 1) {
        resetCdDrive();
    }
    D_8005F100 = 0;
    D_8005F14A = 0;
}

/**
 * Stores a halfword value to the global D_8005F142.
 *
 * @param a0 The value to store.
 */
void func_800A5A14(s16 a0) {
    extern s16 D_8005F142;
    D_8005F142 = a0;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A5A20);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A5C9C);

/**
 * Increments the global byte D_8005F103 and returns the value at
 * D_800C3520[D_8005F103].
 *
 * @return The byte from the D_800C3520 lookup table.
 */
u8 func_800A5CF8(void) {
    extern u8 D_8005F103;
    extern u8 D_800C3520[];

    D_8005F103++;
    return D_800C3520[D_8005F103];
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A5D28);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A5FA4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A6100);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A62EC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A63AC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A6A80);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A7194);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A7224);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A736C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A74B4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A7564);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A8058);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A81AC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A8CDC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A8DAC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A91C8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A9434);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800A97E4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AA46C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AA5F8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AA870);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AA8A0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AA8F8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AAD5C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AAE04);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AAE40);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AAEAC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AAED4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AAEFC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AAF30);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AAF50);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AAF84);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AAFBC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AAFEC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AB02C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AB07C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object1", func_800AB100);
