#include "common.h"
#include "gamestate.h"

extern u8 D_80077BA8[];

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C0098);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C00C8);

/** @brief Sets bit 0x20 of the partyLockFlag. */
void func_800C0384(void) {
    g_gameState.mainData.partyLockFlag |= 0x20;
}

/** @brief Clears bit 0x20 of the partyLockFlag. */
void func_800C03A0(void) {
    g_gameState.mainData.partyLockFlag &= ~0x20;
}

/** @brief Sets bit 0x10 of the partyLockFlag. */
void func_800C03BC(void) {
    g_gameState.mainData.partyLockFlag |= 0x10;
}

/** @brief Clears bit 0x10 of the partyLockFlag. */
void func_800C03D8(void) {
    g_gameState.mainData.partyLockFlag &= ~0x10;
}

/** @brief Sets bit 0x02 of the partyLockFlag. */
void func_800C03F4(void) {
    g_gameState.mainData.partyLockFlag |= 0x02;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C0410);

/**
 * Copies 0x40 bytes from D_80077BA8 - 0x98 to D_80077BA8 using memcopy,
 * then calls memzero16 with D_80077BA8 and mode 4.
 */
void func_800C0448(void) {
    memcopy(D_80077BA8, D_80077BA8 - 0x98, 0x40);
    memzero16(D_80077BA8, 4);
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C048C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C0634);
