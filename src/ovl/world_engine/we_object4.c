#include "common.h"
#include "psxsdk/libgpu.h"
#include "world.h"

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A64DC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A688C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A6A74);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A6BE0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A735C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A7590);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A7B38);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A7CD0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A7E74);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A8024);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A8270);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A8400);

extern DR_MODE D_800D4FB0[2][96];

/**
 * @brief Initialise two 96-slot DR_MODE pools with fixed tag fields.
 *
 * For each of the two pools at @c D_800D4FB0[0..1], walks 96 DR_MODE
 * slots and sets @c len = 2 (2-word payload) and @c code = 0x6A via
 * the PsyQ @c setlen/@c setcode macros. Used to prime the primitive
 * templates before per-frame GPU command generation fills in the rest.
 */
void func_800A84D0(void) {
    s32 s, i;
    for (s = 0; s < 2; s++) {
        for (i = 0; i < 96; i++) {
            setlen(&D_800D4FB0[s][i], 2);
            setcode(&D_800D4FB0[s][i], 0x6A);
        }
    }
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A8524);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A8868);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A8A28);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A8C1C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A9254);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A9300);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A9CC0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A9E24);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A9ED4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800A9F54);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800AA210);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800AAD48);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800AAE04);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800AAEAC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800AAED4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800AAF84);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800AAFBC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800AB02C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800AB06C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800AB07C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800AB100);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800AB2D4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object4", func_800AB300);
