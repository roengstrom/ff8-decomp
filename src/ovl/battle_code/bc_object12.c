#include "common.h"

extern u8 D_800F1A5C[];
extern u8 D_800F1A54[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF0E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF168);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF308);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF3D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF444);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF448);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF5A8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF5C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF7E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF888);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF92C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF95C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF9CC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF9EC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFA58);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFBA8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFC14);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFDA0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFDB4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFE1C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFEC8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFEF8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFF38);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0098);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C00D8);

extern u8 D_800F1940[];
extern u8 D_800F16C0[];

/**
 * @brief Initialize D_800F1940 buffer via func_800B2A00 and return it.
 *
 * Calls func_800B2A00 with D_800F1940, D_800F16C0, size 0x20, and count 0x14.
 *
 * @return Pointer to D_800F1940.
 */
u8 *func_800C0134(void) {
    u8 *buf = D_800F1940;
    func_800B2A00(buf, D_800F16C0, 0x20, 0x14);
    return buf;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0178);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C01A4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C02D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C03DC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0780);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C07B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0AE4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0B3C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0BB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0CB8);

/**
 * @brief Store byte to D_800F1A5C target, then look up address from table.
 *
 * Stores a0 to the byte pointed to by D_800F1A5C. Then reads a table base
 * pointer from D_800F1A54->0x64->0xC, uses a0 as an index into a u16 offset
 * table, and returns the base pointer plus that offset.
 *
 * @param a0 Index value (also stored as byte).
 * @return Resolved pointer from table lookup.
 */
s32 func_800C0D00(s32 a0) {
    s32 ptr;
    s32 base;
    *(u8 *)(*(s32 *)D_800F1A5C) = a0;
    ptr = *(s32 *)(*(s32 *)D_800F1A54 + 0x64);
    a0 &= 0xFF;
    base = *(s32 *)(ptr + 0xC);
    return base + *(u16 *)(base + a0 * 2);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0D3C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0DB0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0FD4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C112C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C15EC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C197C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C22A8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C2528);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C26B0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C2868);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C2888);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C28B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C29C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C2A18);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C2A38);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C2AB0);
