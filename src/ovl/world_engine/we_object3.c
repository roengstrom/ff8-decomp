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

extern s32 D_800C4D20;

/**
 * @brief Tag-based flag lookup — larger sibling of @c func_800A4670.
 *
 * Returns 1 when @c D_800C4D20 is zero. Otherwise picks a bit from the
 * upper bytes of @p a based on @p b's low 16 bits:
 *   - 0x00..0x09, or 0x80: bit 7 of @c (a >> 16)
 *   - 0x20..0x28, or 0x84: bit 6 of @c (a >> 16)
 *   - 0x30: bit 5 of @c (a >> 16)
 *   - 0x31: bit 4 of @c (a >> 16)
 *   - otherwise: 1
 *
 * @note Purpose uncertain — likely command/slot flag dispatch.
 *
 * @param a Source word containing the flag bits in its upper bytes.
 * @param b Tag selector (low 16 bits examined).
 * @return The extracted bit (0 or the mask value), or 1 as a default.
 */
s32 func_800A45D8(u32 a, s32 b) {
    u16 op = (u16)b;
    if (D_800C4D20 == 0) return 1;
    if (op < 0xA || (s16)b == 0x80) return (a >> 16) & 0x80;
    if ((u16)(b - 0x20) < 9 || (s16)b == 0x84) return (a >> 16) & 0x40;
    if ((s16)b == 0x30) return (a >> 16) & 0x20;
    if ((s16)b == 0x31) return (a >> 16) & 0x10;
    return 1;
}

/**
 * @brief Extract one of several bit flags from @p a based on @p b's tag.
 *
 * Uses @p b as a selector to pick which bit of @p a's upper bytes to return.
 * When @c D_800C4D20 is zero the upper byte of @p a is forced to 0xFF before
 * extraction. The selector ranges are:
 *   - 0x20..0x28, or 0x84: bit 4 of @c (a >> 16)
 *   - 0x30: bit 2 of @c (a >> 16)
 *   - 0x31: bit 1 of @c (a >> 16)
 *   - 0x32: bit 7 of @c (a >> 8)
 *   - otherwise: 1
 *
 * @note Purpose uncertain — appears to be a tag-based flag lookup on
 *       command/slot data.
 *
 * @param a Source word containing the flag bits.
 * @param b Tag selector (low 16 bits examined).
 * @return The extracted bit (0 or the masked value), or 1 as a default.
 */
s32 func_800A4670(u32 a, s32 b) {
    if (D_800C4D20 == 0) {
        a |= 0xFFFFFF00;
    }
    if ((u16)(b - 0x20) < 9 || (s16)b == 0x84) return (a >> 16) & 4;
    if ((s16)b == 0x30) return (a >> 16) & 2;
    if ((s16)b == 0x31) return (a >> 16) & 1;
    if ((s16)b == 0x32) return (a >> 8) & 0x80;
    return 1;
}

/**
 * @brief Signed delta of (a mod 128) and (b mod 128), wrapped into [-64, 64].
 *
 * Computes the difference of @p a and @p b after each is reduced modulo 128
 * (signed truncation toward zero), then wraps the difference into a
 * ~[-64, 64] band by adding or subtracting 128. Looks like 256-step angle
 * arithmetic — 128 maps to a half-turn — used to find the shortest signed
 * distance between two angle-like values.
 *
 * @param a First value.
 * @param b Second value.
 * @return Wrapped signed delta in approx [-64, 64].
 */
s32 func_800A4700(s32 a, s32 b) {
    s32 d = (a % 128) - (b % 128);
    if (d < 65) {
        if (d < -64) {
            d += 128;
        }
    } else {
        d -= 128;
    }
    return d;
}

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

extern WorldObject D_800D3320[16];
extern WorldObject *D_800D3318;
extern WorldObject *D_800D34E0;
extern WorldObject *D_800D34E4;
extern u32 D_800D2284;
extern u32 D_800D34A0[16];
extern u32 D_800D34F0;
extern WorldObject D_800D33E0[16];
extern WorldObject D_800C9EF0[16];
extern WorldObject *D_800CA030;
extern u16 D_800C4D60;

/**
 * @brief Initialise the world-engine subsystem's object pools.
 *
 * Builds the 16-entry free list at @c D_800D3320 (linked via @c next),
 * publishes its head at @c D_800D3318, zeros the various object-list
 * roots (@c D_800D34E0, @c D_800D34E4, @c D_800D2284, @c D_800CA030)
 * and the 16-entry @c D_800D34A0 u32 slot table, then resets each of
 * the 16 entries in both @c D_800D33E0 and @c D_800C9EF0 to
 * @c { .next = NULL, .id = -1 }. Finally sets @c D_800C4D60 to its
 * sentinel value 0xFFFF and zeros @c D_800D34F0.
 */
void func_800A581C(void) {
    s32 i;

    for (i = 14; i >= 0; i--) {
        D_800D3320[i].next = &D_800D3320[i + 1];
    }
    D_800D3320[15].next = 0;
    D_800D3318 = &D_800D3320[0];
    D_800D34E0 = 0;
    D_800D34E4 = 0;
    D_800D2284 = 0;
    D_800CA030 = 0;

    for (i = 15; i >= 0; i--) {
        D_800D34A0[i] = 0;
    }

    for (i = 0; i < 16; i++) {
        D_800C9EF0[i].next = 0;
        D_800C9EF0[i].id = -1;
        D_800D33E0[i].next = 0;
        D_800D33E0[i].id = -1;
    }

    D_800C4D60 = 0xFFFF;
    D_800D34F0 = 0;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A58EC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5A3C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5B48);

extern POLY_F4 D_800D3300;
extern s16 D_800C97EA;
extern s16 D_800C97E8;

/**
 * @brief Draw a translucent dark-gray fullscreen quad and wait for the GPU.
 *
 * Sets up @c D_800D3300 as a 24-byte POLY_F4 covering the viewport
 * (0,0)-(width,height) with RGB(8,8,8) and code 0x2A (semi-transparent
 * 4-vertex polygon). Submits via @c DrawPrim, then @c DrawSync(0) blocks
 * until the GPU finishes. Likely a screen-darken pass for fades.
 */
void func_800A5D10(void) {
    POLY_F4 *prim = &D_800D3300;
    s16 w = D_800C97EA;
    s16 h = D_800C97E8;

    setlen(prim, 5);
    setcode(prim, 0x2A);

    prim->x0 = 0;
    prim->y0 = 0;
    prim->y1 = 0;
    prim->x2 = 0;

    prim->r0 = 8;
    prim->g0 = 8;
    prim->b0 = 8;

    prim->x1 = w;
    prim->y2 = h;
    prim->x3 = w;
    prim->y3 = h;

    DrawPrim(prim);
    DrawSync(0);
}

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

/**
 * @brief Compute a 2D tile index from world coordinates (128-wide grid).
 *
 * Like @c func_800A5E40 but finer-grained: divides by 0x800 (2048) instead
 * of 0x2000 and the Y multiplier is 128 (bit 7) instead of 32. Maps
 * (x, y) world coords to a linear tile index on a 128-column grid.
 *
 * @note The ugly pointer-alias expression @c ((*(new_var2 = &rnd)) >> 18)
 *       and the split @c rnd assignment are load-bearing — they coerce
 *       gcc's register allocator to keep the rounded quotient in @c a1
 *       throughout the signed-mod calculation, matching the target.
 *       Cleaner forms (inline cast, single-line rnd expression) drop the
 *       match rate by 1–5 instructions.
 *
 * @param x World X coordinate.
 * @param y World Y coordinate.
 * @return Linear tile index on a 128-column grid.
 */
s32 func_800A5DC8(s32 x, s32 y) {
    s32 xm;
    s32 *new_var2;
    s32 ym;
    s32 new_var;
    x += 0x60000;
    {
        s32 rnd = x;
        if (x < 0) {
            rnd = 0x3FFFF + x;
        }
        new_var = x;
        rnd = ((*(new_var2 = &rnd)) >> 18) << 18;
        xm = new_var - rnd;
    }
    ym = ((y + 0x48000) % 0x30000) >> 11;
    return (xm >> 11) + (ym << 7);
}

/**
 * @brief Compute a tile index from 2D world coordinates.
 *
 * Offsets @p x and @p y to be positive, wraps them into 0x40000 × 0x30000
 * ranges, divides by the tile size (0x2000), and linearises:
 *   - X tile = ((x + 0x60000) mod 0x40000) / 0x2000   — range [0, 32)
 *   - Y tile = ((y + 0x48000) mod 0x30000) / 0x2000   — range [0, 24)
 *   - index = X + Y × 32
 *
 * @note The declaration of @c ym is load-bearing despite being "unused" —
 *       removing it changes gcc's register allocation and breaks the match.
 *       The inline recomputation of the @c y expression in the return is
 *       similarly intentional; gcc folds the duplicate into a single
 *       computation at runtime.
 *
 * @param x World X coordinate.
 * @param y World Y coordinate.
 * @return Linear tile index.
 */
s32 func_800A5E40(s32 x, s32 y) {
    s32 xo = x + 0x60000;
    s32 xm = xo % 0x40000;
    s32 ym = (y + 0x48000) % 0x30000;
    s32 xtile = xm / 0x2000;
    return xtile + ((((y + 0x48000) % 0x30000) / 0x2000) * 32);
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5EC4);

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

extern WorldSection *D_800C4D5C;

/**
 * @brief Walk a WorldObject list and return the first node whose section
 *        key (first byte of @c D_800C4D5C[sectionIdx]) matches @p key.
 *
 * @param key Key byte to match (low 8 bits of @p key are used).
 * @param head Head of the WorldObject list (may be NULL).
 * @return Matching WorldObject, or NULL if none found.
 */
WorldObject *func_800A60B4(s32 key, WorldObject *head) {
    if (head) {
        WorldSection *base = D_800C4D5C;
        key &= 0xFF;
        do {
            WorldSection *section = &base[head->sectionIdx];
            if (key == section->key) return head;
            head = head->next;
        } while (head != 0);
    }
    return NULL;
}

extern u8 D_800C5398[];

/**
 * @brief Walk a WorldObject list and return the first section-byte that
 *        isn't 0xFF, @c D_800C5398[0], or @c D_800C5398[2]; else 0xFF.
 *
 * For each node in @p head, reads the first byte of its world-section
 * (@c D_800C4D5C[sectionIdx]). A byte is "interesting" when it differs from
 * all three skip values, at which point it's returned. If no interesting
 * byte is found (or @p head is NULL), returns 0xFF.
 *
 * @note @c D_800C5398[0] is cached once outside the loop; @c D_800C5398[2]
 *       is re-read each iteration (gcc emits it inside the loop because
 *       it's the third byte read — CSE doesn't hoist it).
 *
 * @param head Head of the WorldObject list (may be NULL).
 * @return An interesting section byte, or 0xFF if none found.
 */
s32 func_800A610C(WorldObject *head) {
    if (head) {
        u8 firstSect = D_800C5398[0];
        do {
            WorldSection *section = &D_800C4D5C[head->sectionIdx];
            u8 key = section->key;
            if (key != 0xFF && firstSect != key && D_800C5398[2] != key) {
                return key;
            }
            head = head->next;
        } while (head != 0);
    }
    return 0xFF;
}

typedef struct {
    u16 x;
    u16 y;
} ImageCoord;

typedef struct {
    /* 0x0000 */ u32 id;                /**< TIM magic */
    /* 0x0004 */ u32 flags;             /**< TIM flags */
    /* 0x0008 */ u32 size1;             /**< First image block size */
    /* 0x000C */ RECT rect1;            /**< First image rect (unused here) */
    /* 0x0014 */ u32 image1[0x800];     /**< First image pixel data (256*16 16bpp) */
    /* 0x2014 */ u32 size2;             /**< Second image block size */
    /* 0x2018 */ RECT rect2;            /**< Second image rect (unused here) */
    /* 0x2020 */ u32 image2[1];         /**< Second image pixel data */
} PackedTIMPair;

extern ImageCoord D_800C5388[];
extern ImageCoord D_800C5378[];
extern RECT D_800D32F0;

/**
 * @brief Upload two subimages from a packed-TIM bundle into VRAM.
 *
 * @p tim points to a back-to-back pair of TIM image blocks. The first image
 * (256×16, 16bpp) is uploaded to the VRAM rect at @c D_800C5388[tableIdx];
 * the second image (128×256, 16bpp) is uploaded to @c D_800C5378[tableIdx].
 *
 * Uses @c D_800D32F0 as a scratch @c RECT for each upload.
 *
 * @note The @c tim++/tim-- and @c img1++/img1-- pairs are load-bearing —
 *       they coerce gcc's register allocator into keeping @c img1 in s2
 *       (the canonical target allocation) instead of keeping raw @c tim.
 *       Without these pairs gcc sees @c img1 and @c tim as equivalent and
 *       arbitrarily picks @c tim.
 */
void func_800A6188(PackedTIMPair *tim, u8 tableIdx) {
    u32 *img1 = tim->image1;
    tim++;
    img1++;
    img1--;
    tim--;

    setRECT(&D_800D32F0, D_800C5388[tableIdx].x, D_800C5388[tableIdx].y, 256, 16);
    LoadImage(&D_800D32F0, img1);
    DrawSync(0);

    setRECT(&D_800D32F0, D_800C5378[tableIdx].x, D_800C5378[tableIdx].y, 128, 256);
    /* FIXME: 0x803 is (sizeof(tim->image1) + sizeof(TIM block header)) / sizeof(u32) =
       byte-distance from image1 to image2 in u32 units. Can't express as tim->image2
       without breaking the s-reg allocation match. */
    LoadImage(&D_800D32F0, img1 + 0x803);
    DrawSync(0);
}

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

/**
 * @brief Split a signed 16-bit value into coarse/fine (q,r × 4,32) components.
 *
 * Divides @p val by 128 (signed) into quotient @c q and remainder @c r, then
 * splits each further by 4:
 *   - @c *coarse = (q/4)*32 + r/4    — 7-bit high portions joined
 *   - @c *fine   = (q%4)*4  + r%4    — 4-bit low portions joined
 *
 * Likely a packed grid-cell / tile-offset decomposition where the coarse
 * output identifies a 32-step bucket and the fine output carries the
 * residual position within it.
 *
 * @param val    s16 input to decompose.
 * @param coarse Output — coarse bucket (high bits of q and r).
 * @param fine   Output — fine residual (low bits of q and r).
 */
void func_800A62E0(s16 val, u16 *coarse, u16 *fine) {
    s32 r = val % 128;
    s32 q = val / 128;
    *coarse = (q / 4) * 32 + r / 4;
    *fine = (q % 4) * 4 + r % 4;
}

/**
 * @brief Free the WorldObject list at @c D_800D34E4 back to the free pool.
 *
 * Unconditionally sets @c D_800C4D60 = 0xFFFF as a marker/sentinel. If the
 * list at @c D_800D34E4 is non-empty, walks every node and clears its slot
 * in the @c D_800D34A0 table (keyed by @c sectionIdx), then splices the
 * whole list onto the front of the @c D_800D3318 free list and clears
 * @c D_800D34E4.
 *
 * @note Purpose uncertain — looks like a subsystem-reset that releases all
 *       active world objects into a reusable pool.
 */
void func_800A6358(void) {
    WorldObject *head;
    D_800C4D60 = 0xFFFF;
    head = D_800D34E4;
    if (head) {
        WorldObject *node = head;
        do {
            D_800D34A0[node->sectionIdx] = 0;
            node = node->next;
        } while (node != 0);

        node = D_800D34E4;
        while (node->next != 0) {
            node = node->next;
        }

        node->next = D_800D3318;
        D_800D3318 = D_800D34E4;
        D_800D34E4 = 0;
    }
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A63F0);
