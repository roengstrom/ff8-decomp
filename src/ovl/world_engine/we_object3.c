#include "common.h"
#include "psxsdk/libgpu.h"
#include "world.h"

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A01DC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A0388);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A05E8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A1540);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A1678);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A1F10);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A2350);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A246C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A25F8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A26E8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A2920);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A2D50);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A358C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A3870);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A39BC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A3C9C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A3EE4);

typedef struct {
    s32 val;    /* +0x00 */
    s16 hval;   /* +0x04 */
    s16 pad;    /* +0x06 */
} FeaEntry40C0; /* size 0x08 */

extern FeaEntry40C0 D_800D24A8[12];

/** Clears an array of 12 entries. */
void func_800A40C0(void) {
    FeaEntry40C0 *ptr = D_800D24A8;
    FeaEntry40C0 *end = ptr + 12;
    while (ptr < end) {
        ptr->val = 0;
        ptr->hval = 0;
        ptr++;
    }
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A40F8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A41E0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A4420);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A45D8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A4670);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A4700);

/**
 * @brief Signed delta between two /128-scaled inputs, wrapped into (-48, 48].
 *
 * Divides @p a and @p b by 128 (signed truncation toward zero), subtracts,
 * and wraps the result into a ~[-48, 48] band by adding or subtracting 96.
 * Looks like modular-angle arithmetic: @c 192 maps to a full circle and
 * @c 96 to a half turn, so this computes "shortest signed distance"
 * between two angle-like values.
 *
 * @param a First value (numerator scale 128).
 * @param b Second value (numerator scale 128).
 * @return Wrapped signed delta in approx [-48, 48].
 */
s32 func_800A475C(s32 a, s32 b) {
    s32 d = a / 128 - b / 128;
    if (d < 49) {
        if (d < -48) {
            d += 96;
        }
    } else {
        d -= 96;
    }
    return d;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A47A4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A4EF4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A50A0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A568C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A581C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A58EC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5A3C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5B48);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5D10);

/**
 * @brief Linear search a world-object list for a node with matching @p id.
 *
 * @param id Signed 16-bit id to match.
 * @param head Head of the linked list (may be NULL).
 * @return Matching WorldObject, or NULL if none found.
 */
WorldObject *func_800A5D8C(s16 id, WorldObject *head) {
    while (head != NULL) {
        if (id == head->id) return head;
        head = head->next;
    }
    return NULL;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5DC8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5E40);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5EC4);

extern s16 D_800C97EA;       /**< Viewport width (s16). */
extern s16 D_800C97E8;       /**< Viewport height (s16). */

/**
 * @brief Set up a draw environment for screen @p screenIdx and submit it.
 *
 * Initialises a DRAWENV for a viewport at (screenIdx * width, 0) of size
 * (width × height) where width = @c D_800C97EA and height = @c D_800C97E8.
 * Patches @c tpage = 0x40 (default GPU texture page) and @c dfe = 1
 * (enable drawing to display area), then pushes it via @c PutDrawEnv.
 *
 * @param screenIdx Horizontal viewport index (multiplied by width).
 */
void func_800A5F78(s32 screenIdx) {
    DRAWENV env;
    SetDefDrawEnv(&env, screenIdx * D_800C97EA, 0, D_800C97EA, D_800C97E8);
    env.dfe = 1;
    env.tpage = 0x40;
    PutDrawEnv(&env);
}

/**
 * @brief Set up a display environment for screen @p screenIdx and submit it.
 *
 * Mirror of @c func_800A5F78 but for @c DISPENV (the VRAM-readout side
 * of the GPU pair). Patches the visible screen region to @c y=8,
 * @c h=224 (NTSC full-frame minus 16 overscan lines) before pushing.
 *
 * @param screenIdx Horizontal viewport index (multiplied by width).
 */
void func_800A5FD4(s32 screenIdx) {
    DISPENV env;
    SetDefDispEnv(&env, screenIdx * D_800C97EA, 0, D_800C97EA, D_800C97E8);
    env.screen.y = 8;
    env.screen.h = 0xE0;
    PutDispEnv(&env);
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A6030);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A60B4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A610C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A6188);

extern s32 func_800A629C(WorldObject *target);

/**
 * @brief Walk a WorldObject list and return 1 if any node's id hits D_800C9EF0's list.
 *
 * For each node in the chain starting at @p head, calls @c func_800A629C
 * (which checks whether the node's id is present in the @c D_800C9EF0
 * static list) and returns 1 as soon as one reports a match. Returns 0 if
 * the list is empty or no node matches.
 *
 * @param head Head of the WorldObject chain to test (may be NULL).
 * @return 1 if any node's id is in the D_800C9EF0 list, else 0.
 */
s32 func_800A6254(WorldObject *head) {
    while (head != NULL) {
        if (func_800A629C(head) != 0) {
            return 1;
        }
        head = head->next;
    }
    return 0;
}

extern WorldObject D_800C9EF0;

/**
 * @brief Check whether any node in the D_800C9EF0 list shares @p target's id.
 *
 * Walks the static list starting at @c D_800C9EF0 and returns 1 if any
 * node's @c id matches @p target->id, else 0.
 *
 * @param target Query node — only its @c id field is read.
 * @return 1 if a matching id was found in the list, 0 otherwise.
 */
s32 func_800A629C(WorldObject *target) {
    WorldObject *node = &D_800C9EF0;
    if (node != NULL) {
        s16 key = target->id;
        do {
            if (key == node->id) return 1;
            node = node->next;
        } while (node != NULL);
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A62E0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A6358);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A63F0);
