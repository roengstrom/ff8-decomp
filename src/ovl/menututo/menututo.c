#include "common.h"
#include "gamestate.h"

/**
 * @brief Read tutorial column index 1.
 *
 * @return Byte value of D_801E4EC1.
 */
s32 func_801E2800(void) {
    extern u8 D_801E4EC1;
    return D_801E4EC1;
}

/**
 * @brief Read tutorial column index 2.
 *
 * @return Byte value of D_801E4EC2.
 */
s32 func_801E2810(void) {
    extern u8 D_801E4EC2;
    return D_801E4EC2;
}

/**
 * @brief Load a sub-overlay at an offset from the base address.
 *
 * Dispatches to loadSubOverlay with a base pointer adjusted by a
 * fixed offset determined by the tutorial section index.
 *
 * @param index Tutorial section index (0-6).
 * @param base Base address of the overlay data.
 */
void func_801E2820(s32 index, u8 *base) {
    switch (index) {
    case 0:
        base += 0x1C;
        break;
    case 1:
        base += 0x14;
        break;
    case 2:
        base += 0x18;
        break;
    case 3:
        base += 0x2C;
        break;
    case 4:
        base += 0x30;
        break;
    case 5:
        base += 0x47;
        break;
    case 6:
        base += 0xB4;
        break;
    }

    loadSubOverlay(base, 0x801D1000);
}

/**
 * @brief Load tutorial page indices from table.
 *
 * Reads two bytes from D_801E4EB4[a0] (4-byte stride) and stores
 * them into D_801E4EC1 and D_801E4EC2 respectively.
 *
 * @param a0 Tutorial page table index.
 */
void func_801E28A8(s32 a0) {
    extern u8 D_801E4EB4[];
    extern u8 D_801E4EC1;
    extern u8 D_801E4EC2;
    u8 *entry;

    entry = D_801E4EB4;
    entry = entry + a0 * 4;
    D_801E4EC1 = entry[0];
    D_801E4EC2 = entry[1];
}

/**
 * @brief Read tutorial page index.
 *
 * @return Byte value of D_801E4EC0.
 */
s32 func_801E28D4(void) {
    extern u8 D_801E4EC0;
    return D_801E4EC0;
}

/** @brief Draw inner panel with section id 0xD and clear flag. */
s32 func_801E28E4(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 0);
}

/** @brief Draw inner panel with section id 0xD and set flag. */
s32 func_801E2910(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 1);
}

/** @brief Call func_801F0A34 with a0, 0, 0x22, and a1 * 15 + 0x3E. */
void func_801E293C(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0x22, a1 * 15 + 0x3E);
}

/**
 * @brief Draw tutorial text with computed CLUT and Y position.
 *
 * Computes a CLUT index from 0x1000 minus field 0x2C via lookup table
 * D_801FA3C8, then calls func_801F0A34 with the computed x/y coordinates.
 *
 * @param a0 OT pointer
 * @param a1 Pointer to tutorial state structure
 */
/** @brief Tutorial state structure for rendering callbacks. */
typedef struct {
    u8 pad00[0x20];
    u16 availMask;      /**< Bitfield of available characters (set by func_801E2ABC). */
    u8 pad22;
    u8 availCount;      /**< Number of available characters (set by func_801E2ABC). */
    u8 pad24[6];
    s16 field_2A;       /**< Scroll/fade position. */
    s16 field_2C;       /**< Scroll/fade progress value. */
    u8 pad2E[4];
    s8 field_32;        /**< Tutorial entry index (signed). */
    u8 pad33[2];
    u8 field_35;        /**< Tutorial page parameter index. */
} TutoState;

void func_801E296C(s32 a0, TutoState *data) {
    extern u16 D_801FA3C8[];

    s32 index;
    s32 tableVal;
    s32 scaled;
    s32 val = data->field_2C;
    u8 param = data->field_35;

    scaled = val;
    index = (tableVal = 0x1000 - val);
    index /= 64;

    tableVal = D_801FA3C8[index];
    scaled = (D_801FA3C8[index] * 190) / 4096;
    func_801F0A34(a0, 0, scaled + 0xA8, (param * 15) + 0x3E);
}

/**
 * @brief Draw tutorial text with table-based X scaling and entry-based Y.
 *
 * Computes Y from the entry index modulo 10, and X from a CLUT lookup
 * table indexed by the scroll position divided by 64.
 *
 * @param a0 OT pointer.
 * @param data Pointer to tutorial state structure.
 */
void func_801E29F8(s32 a0, TutoState *data) {
    extern u16 D_801FA3C8[];

    s32 tableVal;
    u32 scaled;
    u32 height;
    u32 mod = 10;
    s16 pos = data->field_2A;
    s8 val = (s8)data->field_32;

    height = ((val % (tableVal = 10)) * 15) + 0x3E;
    tableVal = D_801FA3C8[(tableVal = pos) / 64];
    scaled = (tableVal * 190) / 4096;

    func_801F0A34(a0, 0, scaled + 0xA8, height);
}

/**
 * @brief Initialize game state from save data buffers.
 *
 * Copies character data from 0x801DB000 and GF data from 0x801DC000
 * into g_gameState, sets initial HP to 9999, copies GF names from
 * 0x801D9000, and initializes party/limit break/config fields.
 * If D_801FABC7 is set, scans characters to build an availability
 * mask for the party selection menu.
 *
 * @param output Tutorial state receiving character availability data.
 */
void func_801E2ABC(TutoState *output) {
    extern u8 D_801FABC7;

    s32 nameSrc = 0x801D9000;
    CharacterData *charSrc = (CharacterData *)0x801DB000;
    GfSaveData *gfSrc = (GfSaveData *)0x801DC000;
    s32 i;
    s32 nameOff;

    for (i = 0; i < CHARACTER_COUNT; i++) {
        g_gameState.chars[i] = *charSrc;
        charSrc++;
        g_gameState.chars[i].currentHp = 9999;
        g_gameState.chars[i].statusFlags = 0;
        g_gameState.chars[i].characterId = i;
    }

    for (i = 0, nameOff = 0x50; i < GF_COUNT; i++) {
        g_gameState.gfs[i] = *gfSrc;
        gfSrc++;
        g_gameState.gfs[i].hp = 9999;
        copyString((u8 *)&g_gameState.gfs[i], (u8 *)(nameSrc + nameOff));
        nameOff += 0x44;
    }

    g_gameState.party.party[0] = 0;
    g_gameState.party.party[1] = 1;
    g_gameState.party.party[2] = 4;
    g_gameState.battleParty[0] = 0;
    g_gameState.battleParty[1] = 1;
    g_gameState.battleParty[2] = 4;

    if (D_801FABC7 != 0) {
        s32 mask = 0;
        s32 count = 0;

        for (i = 0; i < CHARACTER_COUNT; i++) {
            s32 bit = 1;

            if (g_gameState.chars[i].exists & 1) {
                mask |= bit << i;
                count++;
            }
        }

        output->availMask = mask;
        output->availCount = count;
        g_gameState.party.party[0] = 1;
        g_gameState.party.party[1] = 0;
        g_gameState.party.party[2] = 5;
        g_gameState.battleParty[0] = 0;
        g_gameState.battleParty[1] = 1;
        g_gameState.battleParty[2] = 5;
    }

    g_gameState.limitBreaks.angeloCompleted = 0x11;
    g_gameState.limitBreaks.angeloKnown = 0x13;
    g_gameState.limitBreaks.quistisLimits = 1;
    g_gameState.limitBreaks.irvineLimits = 1;
    g_gameState.limitBreaks.selphieLimits = 1;
    g_gameState.config.pad07 = 0;
    g_gameState.partyLockFlag = 0;
    g_gameState.party.unknown17 = 0;
    g_gameState.fieldD20 = 0;
    g_gameState.limitBreaks.zellLimits = 0x4F;

    for (i = 0; i < 8; i++) {
        g_gameState.limitBreaks.angeloPoints[i] = 0xFF;
    }

    recalcPartyStats();
    func_801F5440();
}

/**
 * @brief Save game state back to the tutorial save buffer.
 *
 * Copies character data, GF data, config, party/inventory/battle data,
 * and battle party from g_gameState back to the 0x801D9000 buffer.
 * This is the reverse of func_801E2ABC which loads from the buffer
 * into g_gameState.
 */
void func_801E2D3C(void) {
    typedef struct {
        u32 data[0x244 / 4];
    } SaveMiscBlock;

    GameState *dst = (GameState *)0x801D9000;
    s32 i;

    for (i = 0; i < CHARACTER_COUNT; i++) {
        dst->chars[i] = g_gameState.chars[i];
    }

    for (i = 0; i < GF_COUNT; i++) {
        dst->gfs[i] = g_gameState.gfs[i];
    }

    dst->config = g_gameState.config;

    *(SaveMiscBlock *)&dst->party = *(SaveMiscBlock *)&g_gameState.party;

    for (i = 0; i < 4; i++) {
        dst->battleParty[i] = g_gameState.battleParty[i];
    }
}

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E2EF0);

/**
 * @brief Scan tutorial entry table and build list of available entries.
 *
 * Iterates over 9 entries in D_801E4E3C (12-byte stride), calls
 * testFieldFlag to check if each entry's item (byte at offset 8) is
 * available. Available entries' indices are stored sequentially at
 * a0+0x39, and the total count is stored at a0+0x36.
 *
 * @param a0 Pointer to tutorial state structure
 */
void func_801E30C4(u8 *a0) {
    extern u8 D_801E4E3C[];
    s32 i = 0;
    s32 count = 0;
    u8 *table = D_801E4E3C;

    do {
        if (testFieldFlag(table[8]) != 0) {
            *(u8 *)(a0 + count + 0x39) = i;
            count++;
        }
        i++;
        table += 0xC;
    } while (i < 9);

    *(u8 *)(a0 + 0x36) = count;
}

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E3140);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E3EC0);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E3F8C);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4080);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4214);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E431C);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E43D4);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4598);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E46DC);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E47F8);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E48C0);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4BD0);

/**
 * @brief Conditionally call func_801E4BD0 if field 0x20 is non-zero.
 * @param a0 Pointer to structure with s16 field at offset 0x20
 * @param a1 First argument passed to func_801E4BD0
 * @param a2 Second argument passed to func_801E4BD0
 */
INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4CB0);

/**
 * @brief Initialize tutorial menu: register callbacks, clear state, and enter.
 *
 * Registers func_801E48C0 and func_801E4CB0 as callbacks via func_801F179C.
 * If registration succeeds, clears the 16-bit fields at offsets 0x20 and 0x22,
 * calls func_801F010C(0x140) for display setup, then enters via func_801E48C0.
 */
void func_801E4CE4(void) {
    extern void func_801E48C0();
    extern void func_801E4CB0();
    s32 result = func_801F179C((s32)func_801E48C0, (s32)func_801E4CB0);

    if (result != 0) {
        *(s16 *)(result + 0x20) = 0;
        *(s16 *)(result + 0x22) = 0;
        func_801F010C(0x140);
        func_801E48C0(result);
    }
}
