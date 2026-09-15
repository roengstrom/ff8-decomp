#include "common.h"

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800BF80C);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800BFBFC);
#include "world.h"
#include "world/we_object10.h"

extern s32 D_800DD8F8[0x20];
extern void func_800C3278(s32 arg0);

void func_800BFCC0(unsigned long long arg0) {
    s32 i;
    s32 *dst;
    s32 *src;

    i = 0;
    dst = D_800DD8F8;
    src = (s32 *)D_800DDB00;
    do {
        s32 v = *src;
        src++;
        i++;
        *dst = v;
        dst++;
    } while (i < 0x20);
    func_800C3278(arg0 + 4);
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800BFD10);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800BFFEC);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C0084);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C0154);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C0224);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C02D8);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C0CA4);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C0D00);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C1718);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C17E4);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C22B0);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C239C);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C2B04);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C2C00);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C3278);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C3674);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C3714);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C3918);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C3B74);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C3CE4);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C3DB0);
