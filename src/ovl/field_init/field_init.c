#include "common.h"

/** @brief Field initialization entry: call setup then init step. */
void func_80098000(void) {
    func_8001F5C8();
    func_800980B0();
}

INCLUDE_ASM("asm/ovl/field_init/nonmatchings/field_init", func_80098028);

/**
 * @brief Wrapper that calls func_80098028 (field init step).
 */
void func_800980B0(void) {
    func_80098028();
}

INCLUDE_ASM("asm/ovl/field_init/nonmatchings/field_init", func_800980D0);

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
