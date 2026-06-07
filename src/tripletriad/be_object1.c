#include "common.h"
#include "battle.h"
#include "psxsdk/libc.h"
#include "psxsdk/libgpu.h"

#define BE_OT_LEN 28  /**< OT entries per buffer (D_801A2CE8). */

typedef struct {
    u32 type;
    u32 size;
    s32 offset;
} ResHeader;

/* TIM file structs (Tim / TimSection) come from tim.h via battle.h. */

/** @brief Deferred VRAM transfer dispatched by @c func_800988E0. */
typedef enum {
    POOL_LOAD_IMAGE  = 0,  /**< LoadImage(rect, src). */
    POOL_LOAD_TIM    = 1,  /**< LoadImage twice from a TIM file (CLUT + image). */
    POOL_STORE_IMAGE = 2,  /**< StoreImage(rect, dst). */
    POOL_MOVE_IMAGE  = 3   /**< MoveImage(rect, dstX, dstY) with @c src packed as @c y<<16|x. */
} PoolAction;

typedef struct {
    u8   active;        /**< @c PoolAction enum value. */
    u8   pad01[3];
    RECT rect;          /**< Destination/source RECT (unused when @c active == POOL_LOAD_TIM). */
    void *src;          /**< TIM pointer / pixel buffer / packed dest coords (depends on @c active). */
} PoolEntry;

extern s32 D_801C2FD0;
extern s32 D_801C2FD8;
extern volatile s32 D_801A2C6C;
extern s32 D_801A2C74;
extern PoolEntry D_801C2ED0[];
extern u8 *D_801D2FE0;
extern u8 *D_801D3000;
extern s16 D_80182B54;
extern s16 D_80182B5A;
extern s16 g_textCursorX;
extern s16 D_80182B58;
extern u8 D_801D2FF0[2][8];
extern u8 D_801C2FE0[2][0x8000];
extern u8 D_800A45B8[];
extern u8 D_800B7638[];
extern u8 D_80182B84[];
extern u8 D_801A2C78[];
extern u8 D_801A2CE6;
extern u8 *D_801A2C40;
extern u8 D_801C2DC8;
extern s8 D_801C2DC9;
extern u8 D_801C2DCA;
extern DRAWENV D_801C2DD0[2];
extern DISPENV D_801C2E88[2];
extern RECT D_800A45A8;
extern RECT D_800A45B0;
extern u32 D_801A2CE8[2][BE_OT_LEN];
extern u8  D_801A2DC8[2][0x10000];  /* primitive pool, 64KB per buffer */
extern DRAWENV *g_activeDrawEnv;
extern void func_800988D4(void);
extern void func_800988E0(void);
extern void func_800408C4(s32, s32);   /* SetGeomOffset */
extern u8 D_801D3028[];
extern u8 D_801D3038[];
extern u8 D_8012E66C[];
extern volatile u16 D_8005F158;

typedef struct { u8 r, g, b; } RGB;
extern RGB D_80182B5C;          /* debug-text rgb color. */
extern u32 D_80182AA0[];        /* color palette table, indexed by ASCII byte '0'..'8'. */
extern void SetSprt(SPRT *p);   /* PsyQ: init SPRT tag/code header. */

typedef u8 *(*BattleStateFn)(void);
extern BattleStateFn D_800A4588[];

extern s32 func_80099C78();
extern s32 func_8009A314();
extern s32 func_8009A508();
extern void func_800A271C(void);
extern void func_8009E224(void);
extern void func_8009EBCC(void);
extern void func_8009BAF4(void);
extern void func_800A1C6C(void);
extern void func_800A2214(void);
extern void func_800A21C4(void);


/**
 * @brief Initialize the battle engine subsystems and build the object lookup table.
 *
 * Calls initialization routines for various battle subsystems (rendering, objects,
 * input, etc.), then populates D_801A2C78 with object type mappings by querying
 * func_80023B14 for each of the 110 (0x6E) object indices.
 */
void func_8009822C(void) {
    s32 i;

    func_800A2D34();
    i = 0;
    func_800981BC();
    func_80098B70();
    func_8009E1F0();
    func_80098B68();
    func_800984DC();
    func_8009B494();
    func_800A1BE0();
    func_800A2208();
    func_80098DD4();
    func_8009EB98();
    func_80098A6C(D_800B7638);
    func_80098A6C(D_800A45B8);

    D_801A2C74 = 0;
    D_801A2C6C = 0;
    D_801A2CE6 = 1;

    do {
        D_801A2C78[i] = func_80023B14(i);
        i++;
    } while (i < 0x6E);
}

/**
 * @brief Battle engine main loop — runs until the state machine sentinel (6) is reached.
 *
 * After initializing all subsystems via @c func_8009822C, repeatedly:
 *  - If the disable-input flag is set in @c D_801A2C74 (bit 2) and bits 4 or 5
 *    of @c D_801C2EC0[2] are set, calls @c func_800A271C and clears the
 *    3-element controller-input mask arrays (@c D_801C2EC0/EB8/EC8).
 *  - Dispatches via @c D_800A4588 (function-pointer table indexed 1..5 by
 *    @c D_801A2CE6) until the state returns non-zero (storing it in
 *    @c D_801A2C40 and clearing state) or state becomes 6 (exit).
 *  - Drains the active list at @c D_801A2C40 via @c func_80098D28, then runs
 *    per-frame sub-tick callbacks and increments the frame counter @c D_801A2C6C.
 *
 * After the main loop, runs two frame-finalize iterations and stores 100 to
 * @c g_vsyncRate (D_8005F158) before returning.
 */
s32 func_80098304(void) {
    s32 i;

    func_8009822C();

    do {
        if (D_801A2C74 & 0x4) {
            if (D_801C2EC0[2] & 0x30) {
                func_800A271C();
            }
            for (i = 0; i < 3; i++) {
                D_801C2EC0[i] = 0;
                D_801C2EC8[i] = D_801C2EB8[i] = 0;
            }
        }

        while (D_801A2CE6 != 0 && D_801A2CE6 != 6) {
            D_801A2C40 = D_800A4588[D_801A2CE6]();
            if (D_801A2C40 != 0) {
                D_801A2CE6 = 0;
            }
        }

        if (D_801A2C40 != 0) func_80098D28(D_801A2C40);
        func_8009E224();
        func_8009EBCC();
        func_8009BAF4();
        func_800A1C6C();
        func_80098690();
        func_80098828();
        func_800A2214();
        D_801A2C6C++;
    } while (D_801A2CE6 != 6);

    D_801C2DC9 = -1;
    for (i = 0; i < 2; i++) {
        func_800A1C6C();
        func_80098690();
    }
    func_800A21C4();
    D_8005F158 = 100;
    return 0;
}

/**
 * @brief Initialize the battle engine's GPU and GTE subsystems.
 *
 * Sets up double-buffered draw/display environments for a 384x224 screen
 * with the visible area at 256x224 offset by (0, 8), clears the two
 * framebuffer regions to black plus a 256x256 texture region to white,
 * primes the reverse-order primitive table and pool tail, then initializes
 * the GTE with screen offset (192, 112), projection distance 512, and
 * masks the display while finishing setup.
 */
void func_800984DC(void) {
    DISPENV *disp;
    RECT *screen0;
    RECT *screen1;

    SetDefDrawEnv(&D_801C2DD0[0], 0, 0, 0x180, 0xE0);
    disp = D_801C2E88;
    SetDefDispEnv(&disp[0], 0x180, 0, 0x180, 0xE0);
    screen0 = &disp[0].screen;
    screen0->x = 0;
    screen0->y = 8;
    screen0->w = 0x100;
    screen0->h = 0xE0;

    SetDefDrawEnv(&D_801C2DD0[1], 0x180, 0, 0x180, 0xE0);
    SetDefDispEnv(&disp[1], 0, 0, 0x180, 0xE0);
    disp[1].screen.x = 0;
    screen1 = &disp[1].screen;
    screen1->y = 8;
    screen1->w = 0x100;
    screen1->h = 0xE0;

    ClearImage(&D_800A45A8, 0, 0, 0);
    D_800A45A8.x = 0x180;
    D_800A45A8.y = 0;
    ClearImage(&D_800A45A8, 0, 0, 0);

    ClearImage(&D_800A45B0, 0xFF, 0xFF, 0xFF);

    D_801C2DCA = 0;
    D_801C2EB0 = &D_801A2CE8[0][0];
    ClearOTagR(&D_801A2CE8[0][0], BE_OT_LEN);
    D_801C2EB4 = &D_801A2DC8[D_801C2DCA][0];

    InitGeom();
    func_800408C4(0xC0, 0x70);
    SetGeomScreen(0x200);
    SetDispMask(0);

    D_801C2DC9 = 2;
    D_801C2DC8 = 0;
    func_800988D4();
}

/**
 * @brief Flip the battle engine's double-buffered draw/display environments.
 *
 * Called once per frame (paired with @c func_80099464). Synchronizes with
 * vsync and the previous draw, advances the fade-in/fade-out counter
 * @c D_801C2DC9 (decrements positives, increments negatives; on reaching
 * zero, toggles @c SetDispMask), then flips @c D_801C2DCA to the next
 * buffer index. The just-flipped buffer's draw and disp envs are pushed
 * to the GPU, the previous buffer's OT is drawn, and the new buffer's
 * OT base + primitive pool tail are reset so the next frame's primitives
 * accumulate into the fresh buffer. @c g_activeDrawEnv is updated to
 * point at the buffer being prepared (the one NOT just put on display).
 */
void func_80098690(void) {
    func_80099464();
    VSync(1);
    DrawSync(0);
    VSync(1);
    VSync(D_801C2DC8);

    if (D_801C2DC9 > 0) {
        D_801C2DC9--;
        if (D_801C2DC9 == 0) {
            SetDispMask(1);
        }
    } else if (D_801C2DC9 < 0) {
        D_801C2DC9++;
        if (D_801C2DC9 == 0) {
            SetDispMask(0);
        }
    }

    D_801C2DCA ^= 1;

    PutDrawEnv(&D_801C2DD0[D_801C2DCA]);
    PutDispEnv(&D_801C2E88[D_801C2DCA]);
    func_800988E0();

    DrawOTag(&D_801A2CE8[D_801C2DCA ^ 1][BE_OT_LEN - 1]);
    D_801C2EB4 = &D_801A2DC8[D_801C2DCA][0];
    D_801C2EB0 = &D_801A2CE8[D_801C2DCA][0];
    ClearOTagR(&D_801A2CE8[D_801C2DCA][0], BE_OT_LEN);

    g_activeDrawEnv = &D_801C2DD0[D_801C2DCA ^ 1];
}

/**
 * @brief Initialize controller input arrays for battle engine.
 *
 * Reads current controller state for three input types (digital, analog X, analog Y)
 * and fills their respective 3-element history arrays with the initial values.
 */
void func_80098828(void) {
    s32 i;

    func_800A2BD8();
    D_801C2EC8[0] = func_800A2B84(0);
    D_801C2EC0[0] = func_800A2BA0(0);
    D_801C2EB8[0] = func_800A2BBC(0);

    for (i = 1; i < 3; i++) {
        D_801C2EC8[i] = D_801C2EC8[0];
        D_801C2EC0[i] = D_801C2EC0[0];
        D_801C2EB8[i] = D_801C2EB8[0];
    }
}

/**
 * @brief Clear D_801C2FD0 to zero.
 */
void func_800988D4(void) {
    D_801C2FD0 = 0;
}

/**
 * @brief Dispatch all pending entries in the @c D_801C2ED0 GPU-transfer pool.
 *
 * Walks @c D_801C2FD0 entries and performs the requested operation on each
 * (LoadImage / LoadImage TIM / StoreImage / MoveImage), then resets the pool
 * count to zero. Entries are typically queued by @c func_800988E0's siblings
 * during a frame and flushed here once vsync has elapsed.
 */
void func_800988E0(void) {
    s32 i;
    PoolEntry *p = D_801C2ED0;

    for (i = 0; i < D_801C2FD0; p++, i++) {
        switch (p->active) {
            case POOL_LOAD_IMAGE:
                LoadImage(&p->rect, p->src);
                break;
            case POOL_LOAD_TIM: {
                Tim *tim = p->src;
                TimSection *image;
                LoadImage(&tim->clut.rect, tim->clut.data);
                /* Re-derive image block from p->src (not cached @c tim) so gcc
                   reloads the pointer — matches the original instruction order. */
                image = (TimSection *)((u8 *)&((Tim *)p->src)->clut + tim->clut.len);
                LoadImage(&image->rect, image->data);
                break;
            }
            case POOL_STORE_IMAGE:
                StoreImage(&p->rect, p->src);
                break;
            case POOL_MOVE_IMAGE:
                MoveImage(&p->rect, ((u32)p->src) & 0xFFFF, ((u32)p->src) >> 16);
                break;
        }
    }
    D_801C2FD0 = 0;
}

/**
 * @brief Register an inactive D_801C2ED0 pool entry with an 8-byte data copy.
 *
 * Takes the next slot in the D_801C2ED0 pool (indexed by D_801C2FD0),
 * marks it inactive, copies 8 bytes from @p a0 into the slot's data
 * region (offsets 4..0xB, via unaligned-safe copy), and stores @p a1
 * as the callback at offset 0xC.
 *
 * @param a0 Source of 8 bytes to copy (may be unaligned).
 * @param a1 Callback pointer stored at offset 0xC of the slot.
 */
void func_80098A1C(u8 *a0, u8 *a1) {
    PoolEntry *entry = &D_801C2ED0[D_801C2FD0++];
    entry->active = 0;
    memcpy(&entry->rect, a0, 8);
    entry->src = a1;
}

/**
 * @brief Register a callback into D_801C2ED0 and advance into @p a0.
 *
 * Takes the next free slot in D_801C2ED0 (indexed by the counter at
 * D_801C2FD0), marks it active, stores @p a0 as the callback pointer,
 * then follows a two-step offset chain from @p a0:
 *   1. Read an @c s32 offset at @c a0+8; advance @c a0 by 8 plus that offset.
 *   2. Read an @c s32 offset at the new @c a0; return (@c a0 + that offset).
 *
 * @param a0 Pointer to a header whose byte-offset fields chain into another
 *           location in the same record.
 * @return The final address resulting from the two-step offset chain.
 */
u8 *func_80098A6C(ResHeader *res) {
    PoolEntry *entry;
    s32 *rel;

    entry = &D_801C2ED0[D_801C2FD0++];
    entry->active = 1;
    entry->src = res;

    rel = &res->offset;
    rel = (s32 *)((u8 *)rel + *rel);
    return (u8 *)rel + *rel;
}

/**
 * @brief Register a D_801C2ED0 slot with @c active=2 and an 8-byte data copy.
 *
 * Variant of @c func_80098A1C: takes the next pool slot, marks it with flag
 * @c 2 (vs @c 0 in @c func_80098A1C), copies 8 bytes from @p a0 into the
 * data region (offsets 4..0xB, unaligned-safe), and stores @p a1 as the
 * callback at offset 0xC.
 */
void func_80098AB4(u8 *a0, u8 *a1) {
    PoolEntry *entry = &D_801C2ED0[D_801C2FD0++];
    entry->active = 2;
    memcpy(&entry->rect, a0, 8);
    entry->src = a1;
}

/**
 * @brief Register a D_801C2ED0 slot with @c active=3, data copy, and a
 *        packed 16-bit pair at offset 0xC.
 *
 * Same pool-registration pattern as @c func_80098A1C /
 * @c func_80098AB4, but stores a packed 32-bit value at offset 0xC
 * formed from two 16-bit args (@p a2 as the upper 16 bits, sign-extended
 * @p a1 as the lower 16 bits).
 */
void func_80098B08(u8 *a0, s16 a1, u16 a2) {
    PoolEntry *entry = &D_801C2ED0[D_801C2FD0++];
    entry->active = 3;
    memcpy(&entry->rect, a0, 8);
    entry->src = (void *)(((s32)a2 << 16) | a1);
}

void func_80098B68(void) {
}

/**
 * @brief Set D_801C2FD8 to 0x1F800000 (scratchpad RAM base address).
 */
void func_80098B70(void) {
    D_801C2FD8 = 0x1F800000;
}

/**
 * @brief Allocate aligned memory from the scratchpad heap and return the new pointer.
 *
 * Rounds @p a0 up to the next multiple of 4, then advances D_801C2FD8
 * by that amount. This is the counterpart of func_80098BA0 (which subtracts).
 *
 * @param a0 Size to allocate (will be aligned up to 4).
 * @return The previous value of D_801C2FD8 (start of allocated block).
 */
s32 func_80098B80(s32 a0) {
    s32 old = D_801C2FD8;
    D_801C2FD8 = old + ((a0 + 3) & ~3);
    return old;
}

/**
 * @brief Align a size up to 4 bytes and subtract from the allocation pointer.
 *
 * Rounds a0 up to the next multiple of 4 and decrements D_801C2FD8 by that amount.
 *
 * @param a0 Size to allocate (will be aligned up to 4).
 */
void func_80098BA0(s32 a0) {
    D_801C2FD8 -= (a0 + 3) & ~3;
}

/**
 * @brief Initialize a linked list header and clear all node slots.
 *
 * @param a0 Pointer to the list header (head, tail, pool ptr, size, count).
 * @param a1 Pointer to the node pool.
 * @param a2 Size of each node in bytes.
 * @param a3 Number of nodes in the pool.
 */
void func_80098BC0(u8 *a0, u8 *a1, s32 a2, s32 a3) {
    s32 i = 0;
    *(s32 *)(a0 + 0) = 0;
    *(s32 *)(a0 + 4) = 0;
    *(s32 *)(a0 + 8) = (s32)a1;
    *(s16 *)(a0 + 0xC) = a2;
    *(s16 *)(a0 + 0xE) = a3;
    if (a3 > 0) {
        do {
            *(s16 *)a1 = 0;
            i++;
            a1 += a2;
        } while (i < a3);
    }
}

/**
 * @brief Find and return the first free node in the pool.
 *
 * Scans the node pool referenced by the list header for a node whose
 * flags bit 0 is clear (inactive). Returns the first free node found,
 * or NULL if all nodes are in use.
 *
 * @param a0 Pointer to list header (+0x8=pool, +0xC=stride, +0xE=count).
 * @return Pointer to the first free node, or NULL if none available.
 */
void *func_80098BF8(u8 *a0) {
    s32 count = *(s16 *)(a0 + 0xE);
    u8 *pool = (u8 *)*(s32 *)(a0 + 0x8);
    s32 i = 0;
    if (count > 0) {
        s32 n = count;
        do {
            if (!(*(u16 *)pool & 1)) {
                return pool;
            }
            i++;
            pool += *(s16 *)(a0 + 0xC);
        } while (i < n);
    }
    return 0;
}

/**
 * @brief Allocate a node and append it to a doubly-tracked linked list.
 *
 * Calls func_80098BF8 to allocate a node. If successful, initializes
 * flags (sets bit 0), clears fields at +2 and +4, stores @p a1 at +8,
 * and appends the node: if the list is non-empty, links old tail to
 * new node; otherwise sets head. Always updates tail pointer.
 *
 * @param a0 Pointer to list header (head at +0, tail at +4).
 * @param a1 Value to store at offset +8 of the new node (callback pointer).
 * @return Pointer to the new node, or NULL if allocation failed.
 */
void *func_80098C44(u8 *a0, s32 a1) {
    u8 *node = (u8 *)func_80098BF8(a0);
    if (node != 0) {
        s32 tail;
        *(u16 *)(node + 0) |= 1;
        *(u16 *)(node + 2) = 0;
        *(s32 *)(node + 4) = 0;
        *(s32 *)(node + 8) = a1;
        tail = *(s32 *)(a0 + 4);
        if (tail != 0) {
            *(s32 *)(tail + 4) = (s32)node;
        } else {
            *(s32 *)a0 = (s32)node;
        }
        *(s32 *)(a0 + 4) = (s32)node;
    }
    return node;
}

/**
 * @brief Allocate and insert a node into a linked list.
 *
 * Calls func_80098BF8 to allocate a node. If allocation succeeds,
 * initializes flags (sets bit 0), clears field at +2, stores @p a1
 * at offset +8, and prepends the node to the list headed at @p a0.
 *
 * @param a0 Pointer to the list head pointer.
 * @param a1 Value to store at offset +8 of the new node (callback pointer).
 * @return Pointer to the new node, or NULL if allocation failed.
 */
void *func_80098CC0(u8 *a0, s32 a1) {
    u8 *node = (u8 *)func_80098BF8(a0);
    if (node != 0) {
        *(u16 *)(node + 0) |= 1;
        *(u16 *)(node + 2) = 0;
        *(s32 *)(node + 8) = a1;
        *(s32 *)(node + 4) = *(s32 *)a0;
        *(s32 *)a0 = (s32)node;
    }
    return node;
}

/**
 * @brief Iterate a linked list, calling each node's callback and removing finished nodes.
 *
 * Walks through the linked list at @p a0, calling the function pointer at
 * offset +8 of each node with the node as the argument. If the callback
 * returns a value with bit 1 set, the node is removed from the list
 * (flags cleared, unlinked). Otherwise the node is kept and the count
 * incremented. Updates the list tail pointer when done.
 *
 * @param a0 Pointer to the list header (head at +0, tail at +4).
 * @return Number of nodes remaining in the list.
 */
s32 func_80098D28(u8 *a0) {
    u8 *prev = 0;
    s32 count = 0;
    u8 *node = (u8 *)*(s32 *)(a0 + 0);
    s32 result;

    while (node != 0) {
        result = ((s32 (*)(u8 *))*(s32 *)(node + 8))(node);
        if (result & 2) {
            *(s16 *)(node + 0) = 0;
            if (prev != 0) {
                *(s32 *)(prev + 4) = *(s32 *)(node + 4);
            } else {
                *(s32 *)(a0 + 0) = *(s32 *)(node + 4);
            }
        } else {
            prev = node;
            count++;
        }
        node = (u8 *)*(s32 *)(node + 4);
    }
    *(s32 *)(a0 + 4) = (s32)prev;
    return count;
}

extern ResHeader D_800B71D8;

/**
 * @brief Register @c D_800B71D8 and set up the current frame's draw targets.
 *
 * Registers the @c D_800B71D8 resource into the D_801C2ED0 pool via
 * @c func_80098A6C, kicks off @c ClearOTag for the current buffer's
 * drawenv slot, then rebases the framebuffer (@c D_801D2FE0) and drawenv
 * (@c D_801D3000) pointers to the slot indexed by @c D_80182B54. Unlike
 * @c func_80099464, this does not flip the buffer index first.
 */
void func_80098DD4(void) {
    func_80098A6C(&D_800B71D8);
    ClearOTag(D_801D2FF0[D_80182B54], 2);
    D_801D2FE0 = D_801C2FE0[D_80182B54];
    D_801D3000 = D_801D2FF0[D_80182B54];
}

/**
 * @brief Set battle viewport dimensions if non-negative.
 *
 * Stores a0 to D_80182B5A and g_textCursorX if a0 >= 0.
 * Stores a1 to D_80182B58 if a1 >= 0.
 *
 * @param a0 Width value (stored if >= 0).
 * @param a1 Height value (stored if >= 0).
 */
void func_80098E54(s32 a0, s32 a1) {
    if (a0 >= 0) {
        D_80182B5A = a0;
        g_textCursorX = a0;
    }
    if (a1 >= 0) {
        D_80182B58 = a1;
    }
}

/**
 * @brief Append one debug-text character as a 5x5 textured sprite primitive.
 *
 * Renders @p ch (uppercased on the fly) from the debug font atlas into a
 * fresh @c SPRT at the current packet-buffer tail (@c D_801D2FE0), then
 * advances the cursor and the buffer pointer. Newline resets the cursor
 * to the line origin and steps down 7 px; any non-printable byte (below
 * space) just advances the cursor without emitting a glyph.
 *
 * The U/V atlas math packs 8 glyphs per row across 64 px; biases by
 * @c -0x80 / @c -0x20 wrap the indices into the texture-page coordinate
 * space. Odd/even character codes alternate between two CLUTs.
 */
void func_80098E7C(u8 ch) {
    SPRT *prim;
    s32 adjusted;
    s32 row;

    prim = (SPRT *)D_801D2FE0;

    if (ch == '\n') {
        g_textCursorX = D_80182B5A;
        D_80182B58 += 7;
        return;
    }

    /* Any space or control byte: just advance the cursor, no glyph. */
    if (ch <= 0x20) {
        g_textCursorX += 6;
        return;
    }

    /* Lowercase 'a'..'z' (0x61..0x7A): fold to uppercase by subtracting 0x20. */
    if ((u8)(ch - 0x61) < 0x1A) {
        ch -= 0x20;
    }

    SetSprt(prim);

    /* Drop down to glyph index relative to space (0x20 = first printable). */
    adjusted = ch - 0x20;

    prim->r0 = D_80182B5C.r;
    prim->g0 = D_80182B5C.g;
    prim->b0 = D_80182B5C.b;

    prim->x0 = g_textCursorX;
    prim->y0 = D_80182B58;

    prim->u0 = ((adjusted << 2) & 0x38) - 0x80;

    row = adjusted;
    if (adjusted < 0) {
        row = ch - 0x11;
    }
    prim->v0 = ((row >> 4) << 3) - 0x20;

    if (ch & 1) {
        prim->clut = 0x384F;
    } else {
        prim->clut = 0x380F;
    }

    prim->h = 5;
    prim->w = 5;

    AddPrim((u32 *)D_801D3000, prim);

    D_801D2FE0 = (u8 *)(prim + 1);
    g_textCursorX += 6;
}

/**
 * @brief Render a null-terminated text string, with @c '#<digit>' color escapes.
 *
 * Walks @p str byte by byte. Characters are passed to @c func_80098E7C for
 * sprite rendering. A @c '#' begins an escape: if the next byte is an ASCII
 * digit @c '0'..'8', it indexes the palette table @c D_80182AA0 and the
 * matching 4-byte RGB entry is copied into the live @c D_80182B5C color;
 * any other byte after @c '#' is rendered as a plain character (the @c '#'
 * is consumed silently).
 *
 * @note The @c escape_marker variable holds @c '#' across iterations to
 * anchor the constant in a callee-saved register (matches target codegen).
 */
void func_80098FD8(u8 *str) {
    u8 ch;

    for (ch = *str++; ch != 0; ch = *str++) {
        s32 digitEscape = '#';
        if (ch == digitEscape) {
            s32 next = *str++;
            s32 digit = next & 0xFF;
            if (digit < '9') {
                if (digit >= '0') {
                    /* Set color from palette entry indexed by ASCII digit. */
                    memcpy(&D_80182B5C, &D_80182AA0[digit], 4);
                } else {
                    func_80098E7C(next);
                }
            } else {
                func_80098E7C(next);
            }
        } else {
            func_80098E7C(ch);
        }
    }
}

/**
 * Converts an integer to a decimal string representation.
 *
 * @param a0 The integer value to convert.
 * @param a1 Pointer to the output buffer.
 * @return Pointer to the end of the written string.
 */
u8 *func_800990A0(s32 a0, u8 *a1) {
    u8 buf[36];
    u8 *dst = a1;
    u8 *p;

    if (a0 < 0) {
        *dst = 0x2D;
        dst++;
        a0 = -a0;
    }

    p = buf + 33;
    buf[33] = 0;

    do {
        p--;
        *p = (a0 % 10) + 0x30;
        a0 = a0 / 10;
    } while (a0 != 0);

    func_80047CA4(dst, p);
    return dst + func_80047CB4(dst);
}

/**
 * Converts an integer to a hexadecimal string representation.
 *
 * @param a0 The integer value to convert.
 * @param a1 Pointer to the output buffer.
 * @return Pointer to the end of the written string.
 */
u8 *func_80099134(s32 a0, u8 *a1) {
    u8 buf[20];
    u8 *dst = a1;
    u8 *p;
    u8 *table;

    if (a0 < 0) {
        *dst = 0x2D;
        dst++;
        a0 = -a0;
    }

    p = buf + 17;
    buf[17] = 0;
    table = D_80182B84;

    do {
        p--;
        *p = *(u8 *)((a0 & 0xF) + (s32)table);
        a0 >>= 4;
    } while (a0 != 0);

    func_80047CA4(dst, p);
    return dst + func_80047CB4(dst);
}

/**
 * @brief Convert an integer to a binary string representation.
 *
 * Writes the binary digits of @p a0 into the buffer at @p a1,
 * then returns a pointer to the end of the written string.
 *
 * @param a0 The integer value to convert.
 * @param a1 Pointer to the output buffer.
 * @return Pointer to the end of the written string.
 */
u8 *func_800991AC(s32 a0, u8 *a1) {
    u8 buf[36];
    u8 *dst = a1;
    u8 *p;

    p = buf + 33;
    buf[33] = 0;

    do {
        p--;
        *p = (a0 & 1) + 0x30;
        a0 >>= 1;
    } while (a0 != 0);

    func_80047CA4(dst, p);
    return dst + func_80047CB4(dst);
}

/**
 * @brief printf-style format-into-buffer.
 *
 * Walks the format string given as @c args[0] and writes the formatted
 * output into @p dst. The remaining @c args[1..] are the values referenced
 * by format specifiers. Supported specifiers:
 *
 *   - @c %d, @c %D : signed decimal (via @c func_800990A0)
 *   - @c %x, @c %X : hexadecimal    (via @c func_80099134)
 *   - @c %b, @c %B : binary         (via @c func_800991AC)
 *   - @c %c, @c %C : single character byte
 *   - @c %s, @c %S : null-terminated string
 *
 * Each @c % may be followed by an optional @c '0' to select zero-padding
 * (default is space-padding), then a decimal field width parsed by
 * @c func_80047C54 (strtol). Plain characters are copied verbatim. The
 * @c escape_marker local anchors the @c '%' constant in a callee-saved
 * register, matching the target's register allocation.
 *
 * @param dst   Output buffer; receives a null-terminated formatted string.
 * @param args  Variadic argument array: @c args[0] is the format string,
 *              subsequent words are pulled in order by format specifiers.
 * @return Length of the produced string (excluding null terminator).
 */
s32 func_80099204(char *dst, s32 *args) {
    char *fmt = (char *)*args++;
    char *out = dst;
    char buf[256];
    u8 ch;
    u8 padCh;
    s32 width;

    ch = *fmt++;
    if (ch == 0) goto end;

    do {
        s32 escape_marker = '%';
        if (ch == escape_marker) {
            padCh = ' ';
            if (*fmt == '0') {
                padCh = '0';
                fmt++;
            }
            width = func_80047C54(fmt, &fmt, 0);
            ch = *fmt++;
            switch (ch) {
                case 'D': case 'd':
                    func_800990A0(*args++, buf);
                    break;
                case 'X': case 'x':
                    func_80099134(*args++, buf);
                    break;
                case 'B': case 'b':
                    func_800991AC(*args++, buf);
                    break;
                case 'C': case 'c':
                    buf[0] = (u8)*args++;
                    buf[1] = 0;
                    break;
                case 'S': case 's':
                    func_80047CA4(buf, (char *)*args++);
                    break;
                default:
                    buf[0] = ch;
                    buf[1] = 0;
                    break;
            }
            if (width > 0) {
                s32 len = func_80047CB4(buf);
                if (len < width) {
                    s32 i;
                    width -= len;
                    for (i = 0; i < width; i++) {
                        *out++ = padCh;
                    }
                }
            }
            func_80047CA4(out, buf);
            out += func_80047CB4(buf);
        } else {
            *out++ = ch;
        }
        ch = *fmt++;
    } while (ch != 0);
end:
    *out = 0;
    return func_80047CB4(dst);
}

/**
 * @brief Variadic forwarder: passes @p a0 and a pointer to the variadic
 *        args to @c func_80099204.
 *
 * The `...` is load-bearing — it triggers gcc's varargs prologue that
 * saves a1-a3 to the caller's shadow area, letting @c &a1 act as the
 * start of a variadic argument list.
 */
s32 func_800993F4(s32 a0, s32 a1, ...) {
    return func_80099204((char *)a0, &a1);
}

/**
 * @brief Printf-style wrapper: formats variadic args into a 256-byte stack
 *        buffer via @c func_80099204, then outputs the result via
 *        @c func_80098FD8.
 *
 * The `...` triggers gcc's varargs prologue, saving a1-a3 to the caller's
 * shadow save area so @c &a0 can act as the start of a variadic arg list.
 */
void func_80099424(s32 a0, ...) {
    s8 buf[0x100];
    func_80099204((char *)buf, &a0);
    func_80098FD8(buf);
}

/**
 * @brief Flip the battle double-buffer and reinitialize per-frame state.
 *
 * Performs end-of-frame teardown on the current framebuffer/drawenv,
 * then toggles the active buffer index in @c D_80182B54 (0 <-> 1),
 * starts the new frame, resets viewport fields to 8, and rebases the
 * framebuffer (32 KB per buffer) and drawenv (8 bytes per buffer)
 * pointers to the new slot.
 */
void func_80099464(void) {
    u8 *fb = D_801D2FE0;

    SetDrawTPage(fb, 0, 1, 3);
    AddPrim(D_801D3000, fb);
    DrawOTag(D_801D3000);

    D_80182B54 = D_80182B54 ^ 1;
    ClearOTag(D_801D2FF0[D_80182B54], 2);

    D_80182B5A = 8;
    g_textCursorX = 8;
    D_80182B58 = 8;

    D_801D2FE0 = D_801C2FE0[D_80182B54];
    D_801D3000 = D_801D2FF0[D_80182B54];
}

/**
 * @brief Initialize the 10 Triple Triad battle-object slots for a new match.
 *
 * Calls @c func_8009C6D8 for one-shot setup, then assigns each of the 10
 * @c D_801D31C0 slots to a player by tagging it with that player's index
 * (low bit of @c initFlags) and a sequence number (0..N) within that
 * player's hand. The @c fieldD reset clears any stale sub-state.
 *
 * For the "offset hand" layout (@c D_801A2C70[player] @c == @c 3) with
 * the corresponding rule bit clear, @c posData[1] is biased to @c 0x800
 * so the card draws shifted; otherwise it stays at zero.
 *
 * Finally, slots 1 and 2 of the substate parameter table @c D_801D3340
 * have their @c field2 halfword zeroed (idle substate state).
 *
 * @note The reversed @c cnt initialization order matches the compiler's
 *       scheduling for the original source — natural order produces the
 *       wrong store sequence.
 */
void func_8009953C(void) {
    s32 cnt[2];
    s32 i;
    s32 owner;
    BattleObject *entry;

    func_8009C6D8();

    cnt[1] = 0;
    cnt[0] = 0;

    for (i = 0; i < 10; i++) {
        s32 c;
        entry = &D_801D31C0[i];
        owner = entry->initFlags & 1;
        entry->fieldD = 0;
        entry->groupId = owner;
        c = cnt[owner];
        entry->priority = c;
        cnt[owner] = c + 1;
        if (D_801A2C70[owner] == 3 && !(g_tripleTriadRules & 1)) {
            entry->posData[1] = 0x800;
        }
    }

    D_801D3340[1].field2 = 0;
    D_801D3340[2].field2 = 0;
}

/**
 * @brief Per-triangle face descriptor for the 4-vertex G3 icon model.
 *
 * The 4 entries of @c D_80182BB8 pair with the 4-vertex SVECTOR table
 * @c D_80182B98 to describe a tetrahedral 3D icon (3 yellow faces and one
 * white face). The 3 vertex indices select corners from the transformed
 * vertex output; the three color words pre-pack the @c POLY_G3
 * @c r/g/b/code byte quartets for direct word-store into the primitive.
 */
typedef struct {
    u8  v0;             /**< Index 0..3 into the transformed vertex table. */
    u8  v1;
    u8  v2;
    u8  pad03;
    u32 color0Word;     /**< Packed @c r0|g0|b0|code (with @c 0x30 = G3 code). */
    u32 color1Word;     /**< Packed @c r1|g1|b1|pad1. */
    u32 color2Word;     /**< Packed @c r2|g2|b2|pad2. */
} TriadFaceDesc;        /* 0x10 bytes */

extern SVECTOR        D_80182B98[4];   /**< 4-vertex tetrahedron model. */
extern TriadFaceDesc  D_80182BB8[4];   /**< 4 G3 face descriptors. */
extern u32           *D_801D3008;      /**< Scratch buffer pointer for RotTransPers4 outputs. */

/**
 * @brief Emit @c POLY_G3 primitives for a 4-vertex tetrahedral icon model.
 *
 * Allocates a 24-byte scratch buffer (6 words: 4 packed sxy + p + flag),
 * batches the 4 model vertices through @c RotTransPers4 to project them
 * into screen space, then walks the 4 face descriptors in @c D_80182BB8.
 * For each face it picks 3 of the 4 transformed vertices, runs
 * @c NormalClip to drop back-facing triangles, and (if visible) writes a
 * @c POLY_G3 with the face's pre-packed colors + projected screen
 * positions, links it into @p ot via @c AddPrim, and advances @p prims.
 *
 * The packed @c color0/1/2Word fields are copied into the primitive via
 * @c sw-equivalent word stores to match the source's u32 copy pattern.
 *
 * @param ot     Display-list OT head this batch is chained into.
 * @param prims  Output buffer for @c POLY_G3 primitives (up to 4 emitted).
 * @return Pointer to the next free slot after the last emitted primitive.
 */
POLY_G3 *func_800995F8(void *ot, POLY_G3 *prims) {
    POLY_G3 *out;
    s32 i;
    TriadFaceDesc *face;
    s32 *ptr;

    ptr = (s32 *)func_80098B80(0x18);
    D_801D3008 = (u32 *)ptr;

    RotTransPers4(&D_80182B98[0], &D_80182B98[1], &D_80182B98[2], &D_80182B98[3],
                  &ptr[0], &ptr[1], &ptr[2], &ptr[3], &ptr[4], &ptr[5]);

    out = prims;
    for (i = 0; i < 4; i++) {
        face = &D_80182BB8[i];
        if (NormalClip(D_801D3008[face->v0], D_801D3008[face->v1], D_801D3008[face->v2]) >= 0) {
            out->tag = 0x06000000;
            *(u32 *)&out->r0 = face->color0Word;
            *(u32 *)&out->r1 = face->color1Word;
            *(u32 *)&out->r2 = face->color2Word;
            *(u32 *)&out->x0 = D_801D3008[face->v0];
            *(u32 *)&out->x1 = D_801D3008[face->v1];
            *(u32 *)&out->x2 = D_801D3008[face->v2];
            AddPrim(ot, out);
            out++;
        }
    }

    func_80098BA0(0x18);
    return out;
}

/**
 * @brief Per-frame 4-state animation handler for a 3D icon transform.
 *
 * Allocates a 40-byte scratch @c TransformBuf (8-byte rotation @c SVECTOR
 * + 32-byte @c MATRIX) and dispatches on the node's state byte:
 *
 *  - **State 0** (init): pick a random phase (0 or 1) via @c func_80023D04,
 *    set the directional @c D_801D300C (±0x400) and @c D_80182C00.vx (±0x8C)
 *    from the phase, seed the scratch vector from the +Z unit @c D_80182BF8,
 *    call @c func_800A233C(0x70), advance to state 1.
 *  - **State 1** (entry arc, 85 frames total): three sub-phases driven by
 *    the counter — 0..59 = sin-driven Y arc, 60..69 = hold (vector reset),
 *    70..84 = blend back to @c D_80182C00 with an oscillating Y dip; on
 *    counter >= 85 latches the phase to @c D_801D30F8 and goes to state 2.
 *  - **State 2** (idle): nudges @c D_80182C08.vy by 0x10 per frame and
 *    writes the static pose (-0x5C / 0x200 / ±0x8C). Transitions to state 3
 *    when @c D_801D30F8 disagrees with the node's phase.
 *  - **State 3** (flip): 10-frame swing using @c rsin to interpolate X/Z;
 *    on completion flips the phase bit and returns to state 2.
 *
 * Always runs the common tail: build a @c YXZ rotation matrix from
 * @c D_80182C08, copy the scratch vector into the matrix translation,
 * apply a constant +0x100 X tilt, set the GTE rotation + translation
 * matrices, and emit one batch of @c POLY_G3 primitives via
 * @c func_800995F8 into the active OT.
 *
 * @note Near-match (92.24%): remaining gap is gcc 2.7.2 register naming and
 *       scheduling choices that no clean C structure reproduces. The C body
 *       in @c permuter/func_80099798/base.c — reproduced below — is the
 *       most natural form that holds up under both readability and match
 *       criteria. Higher scores (up to ~99%) are achievable with permuter
 *       tricks (anchor variables like @c new_var = 0x8C, @c c=d aliases,
 *       @c short r narrowing) but produce non-idiomatic source.
 *
 * @verbatim
 * s32 func_80099798(HandlerNode *node) {
 *     D_801D3010 = (TransformBuf *)func_80098B80(0x28);
 *     switch (node->state) {
 *         case 0: {
 *             s32 r = func_80023D04() % 2;
 *             s32 delta = -0x400;
 *             s32 xPos  = -0x8C;
 *             node->phase = r;
 *             if (r) delta = 0x400;
 *             D_801D300C = delta;
 *             if (r) xPos = 0x8C;
 *             D_80182C00.vx = xPos;
 *             D_801D3010->vec = D_80182BF8;
 *             func_800A233C(0x70);
 *             node->state = 1;  node->counter = 0;
 *             break;
 *         }
 *         case 1: {
 *             s32 c = node->counter;
 *             if (c < 60) {
 *                 s32 t = (c << 12) / 60;
 *                 D_80182C08.vx = (u32)t >> 2;   // t is non-negative; srl
 *                 D_80182C08.vy = ((D_801D300C + 0xA000) * rsin(t / 4)) >> 12;
 *                 D_801D3010->vec = D_80182BF8;
 *             } else if ((c -= 60) < 10) {
 *                 D_801D3010->vec = D_80182BF8;
 *             } else if ((c -= 10) < 15) {
 *                 s32 d = (c << 12) / 15;
 *                 D_80182C08.vx = (-(d << 10) >> 12) + 0x400;
 *                 D_80182C08.vy = D_801D300C + (((-D_801D300C) * d) >> 12);
 *                 LoadAverageShort12(&D_80182BF8, &D_80182C00, 0x1000 - d, d,
 *                                    &D_801D3010->vec);
 *                 D_801D3010->vec.vy -= (rsin(d / 2) << 4) >> 12;
 *             } else {
 *                 D_80182C08.vx = 0;
 *                 D_801D3010->vec = D_80182C00;
 *                 D_801D30F8 = node->phase;
 *                 node->state = 2;  node->counter = 0;
 *             }
 *             node->counter++;
 *             break;
 *         }
 *         case 2: {
 *             s32 xPos = -0x8C;
 *             D_80182C08.vy += 0x10;
 *             if (node->phase) xPos = 0x8C;
 *             D_801D3010->vec.vy = -0x5C;
 *             D_801D3010->vec.vz =  0x200;
 *             D_801D3010->vec.vx = xPos;
 *             if (D_801D30F8 != node->phase) {
 *                 node->state = 3;  node->counter = 0;
 *             }
 *             break;
 *         }
 *         case 3: {
 *             s32 t    = (node->counter << 12) / 10;
 *             s32 sinv = rsin(t / 4);
 *             if (node->phase) sinv = 0x1000 - sinv;
 *             D_801D3010->vec.vx = ((sinv * 0x118) >> 12) - 0x8C;
 *             D_801D3010->vec.vy = -0x5C;
 *             D_801D3010->vec.vz = (-(rsin(t / 2) << 6) >> 12) + 0x200;
 *             node->counter++;
 *             if (node->counter >= 10) {
 *                 node->state = 2;  node->counter = 0;
 *                 node->phase ^= 1;
 *             }
 *             break;
 *         }
 *     }
 *     RotMatrixYXZ(&D_80182C08, &D_801D3010->mat);
 *     D_801D3010->mat.t[0] = D_801D3010->vec.vx;
 *     D_801D3010->mat.t[1] = D_801D3010->vec.vy;
 *     D_801D3010->mat.t[2] = D_801D3010->vec.vz;
 *     RotMatrixX(0x100, &D_801D3010->mat);
 *     SetTransMatrix(&D_801D3010->mat);
 *     SetRotMatrix(&D_801D3010->mat);
 *     D_801C2EB4 = func_800995F8(&D_801C2EB0[4], D_801C2EB4);
 *     func_80098BA0(0x28);
 *     return 0;
 * }
 * @endverbatim
 */
INCLUDE_ASM("asm/ovl/tripletriad/nonmatchings/be_object1", func_80099798);

/**
 * @brief Triple Triad match controller — 10-state machine driving the post-game
 *        result flow (rule resolution → card counting → reward → cleanup).
 *
 * Called as a per-frame handler off the battle-object dispatch chain. Each
 * call advances the state machine until it must wait for an asynchronous
 * event (input poll, sub-handler completion, animation finish), at which
 * point it returns @c 0 and the next frame re-enters. The state byte at
 * @c ctl->state (@c 0..9) is dispatched via a jumptable; states @c 2 and
 * @c 3 are unused and re-loop the dispatch (gcc-default fill for unused
 * cases). State @c >= 10 hangs intentionally.
 *
 * State outline:
 *   - **0** (init): on first frame allocate a sub-handler running
 *     @c func_80099798, mark @c D_801D30F8 = @c -1, clear the substate
 *     parameter slots. Stays here until @c D_801D30F8 becomes non-negative
 *     (the sub-handler picks a starting player), then advances to state 1.
 *   - **1** (player turn): on first frame dispatch on
 *     @c D_801A2C70[D_801D30F8] (player type: 0/1 human, 2 AI, 3 demo) into
 *     @c func_8009C010 / @c func_8009DECC to create a per-turn handler,
 *     stashed at @c ctl->subHandler. Subsequent frames poll
 *     @c func_80098D28(subHandler); on completion advance to state 4.
 *   - **4** (rule resolution, first pass): wait for @c func_8009C0F4 (UI
 *     idle). On entry call @c applyCardRules to evaluate Same/Plus chains.
 *     If the result flags either bit 2 (Same) or bit 3 (Plus), play the
 *     matching SFX (@c func_8009EB30 0/1), then sweep the 3x3 play area
 *     for cells marked with the corresponding flag bit and animate them via
 *     @c func_8009C0A0. After @c counter == 1, finalize and advance to 5.
 *   - **5** (rule resolution, chain pass): keep applying @c applyCardRules
 *     until the result returns 0, playing the chain SFX (@c 5) once. Then
 *     advance to state 6.
 *   - **6** (empty-cell tally): count occupied cells in the play area. If
 *     all 9 are filled, advance to state 7. Otherwise flip @c D_801D30F8
 *     and loop back to state 1 (next player's turn).
 *   - **7** (card count): wait one frame; then tally cards in
 *     @c D_801D31C0 by owner (low bit of @c initFlags). Stash the winner
 *     in @c D_801D30FC (@c 0/1 for winner, @c 2 for draw). Wait until
 *     @c counter >= 12, then advance to state 8.
 *   - **8** (result SFX + input wait): on first frame pick the result SFX
 *     based on winner/draw and start it via @c func_8009EB30, saving the
 *     handle in @c D_801D3018. Wait for @c D_801C2EC4 button input; on
 *     cancel (@c 0x4000) silence the SFX and stay; on confirm play
 *     @c func_800A2054 and advance to state 9 (or in Sudden Death mode
 *     loop back to state 1 via @c func_8009953C reset).
 *   - **9** (fade out): call @c func_800A0370 once, wait 15 frames, then
 *     stop the result SFX, update the Triple Triad win/loss/draw counters
 *     in @c TripleTriadData, set @c D_80082C9C category byte, and trigger
 *     battle exit via @c D_801A2CE6 = @c 4.
 *
 * @param ctl  Battle-object handler context (state at +0x10, counter at
 *             +0x11, sub-handler pointer at +0x0C, rule flags at +0x13,
 *             retry flag at +0x15).
 * @return Always @c 0.
 *
 * @note Near-match (96.85%): remaining gap is gcc 2.7.2 register allocation
 *       (s2↔s3 swap on the row-offset variable in case 6, a1↔a2 swap on
 *       loop counters). The intentional hang on invalid @c state @c >= @c 10
 *       falls out of gcc's switch bounds check (the @c sltiu/beqz-to-self
 *       pattern emerges automatically when no @c default case is provided).
 *       Matching tricks applied: (1) @c ownerMask is pre-initialized to
 *       @c 0 then passed to @c func_8009EB30 in the Same branch — gcc
 *       reuses the s-reg for the SFX-id arg @c 0 then re-loads it with
 *       @c TT_CELL_SAME_MATCHED after the call. (2) @c cnt[1]=0; @c cnt[0]=0;
 *       reversed init order matches the @c sb sequence. (3) @c cnt[0]>cnt[1]
 *       written first (instead of @c cnt[1]<cnt[0]) — gcc loads @c cnt[0]
 *       first matching target. The C body in
 *       @c permuter/func_80099C78/base.c is the canonical form.
 *
 * @verbatim
 * s32 func_80099C78(HandlerNode *ctl) {
 *     while (1) {
 *         switch (ctl->state) {
 *             case 0: {
 *                 if (ctl->counter == 0) {
 *                     HandlerNode *sub = (HandlerNode *)func_80098C44(D_801D3028, (s32)func_80099798);
 *                     sub->state = 0;
 *                     sub->counter = 0;
 *                     D_801D30F8 = -1;
 *                     D_801D3340[1].field2 = 0;
 *                     D_801D3340[2].field2 = 0;
 *                     ctl->counter++;
 *                 }
 *                 if (D_801D30F8 < 0) return 0;
 *                 ctl->state = 1;  ctl->counter = 0;
 *                 break;
 *             }
 *             case 1: {
 *                 if (ctl->counter == 0) {
 *                     u8 playerType = D_801A2C70[D_801D30F8];
 *                     switch (playerType) {
 *                         case 0: ctl->subHandler = (void *)func_8009C010(D_801D30F8, 0); break;
 *                         case 1: ctl->subHandler = (void *)func_8009C010(D_801D30F8, 1); break;
 *                         case 2: ctl->subHandler = (void *)func_8009C010(D_801D30F8, 2); break;
 *                         case 3: ctl->subHandler = (void *)func_8009DECC(D_801D30F8); break;
 *                     }
 *                     ctl->counter++;
 *                     return 0;
 *                 }
 *                 if (func_80098D28(ctl->subHandler) == 0) {
 *                     ctl->state = 4;  ctl->counter = 0;
 *                     break;
 *                 }
 *                 return 0;
 *             }
 *             case 4: {
 *                 if (func_8009C0F4()) return 0;
 *                 if (ctl->counter == 0) {
 *                     u8 rules = applyCardRules(&D_801D3398, 1);
 *                     ctl->rulesFlags = rules;
 *                     if (rules & TT_RESULT_COMBO_MASK) {
 *                         s32 row, col;
 *                         s32 ownerMask = 0;  // also used as SFX arg in Same path
 *                         if (rules & TT_RESULT_SAME_FIRED) {
 *                             func_8009EB30(ownerMask);
 *                             ownerMask = TT_CELL_SAME_MATCHED;
 *                         } else if (rules & TT_RESULT_PLUS_FIRED) {
 *                             func_8009EB30(1);
 *                             ownerMask = TT_CELL_PLUS_COMBO;
 *                         }
 *                         for (row = 1; row <= 3; row++) {
 *                             for (col = 1; col <= 3; col++) {
 *                                 if (D_801D3398.cells[row][col].flags & ownerMask)
 *                                     func_8009C0A0(D_801D3398.cells[row][col].entityIdx, 6);
 *                             }
 *                         }
 *                         func_800A233C(TT_HOLD_FRAMES_RULE);
 *                     }
 *                     ctl->counter++;
 *                 } else {
 *                     ctl->retryFlag = 1;
 *                     func_8009D058(&D_801D3398);
 *                     ctl->state = 5;  ctl->counter = 0;
 *                 }
 *                 break;
 *             }
 *             case 5: {
 *                 if (func_8009C0F4()) return 0;
 *                 if (ctl->rulesFlags == 0) { ctl->state = 6; ctl->counter = 0; break; }
 *                 {
 *                     u8 next = applyCardRules(&D_801D3398, ctl->rulesFlags);
 *                     ctl->rulesFlags = next;
 *                     if (next != 0 && ctl->retryFlag != 0) func_8009EB30(5);
 *                     func_8009D058(&D_801D3398);
 *                 }
 *                 break;
 *             }
 *             case 6: {
 *                 s32 unmatched = 0, row;
 *                 for (row = 1; row <= 3; row++) {
 *                     s32 col;
 *                     for (col = 1; col <= 3; col++) {
 *                         if (!(D_801D3398.cells[row][col].flags & TT_CELL_OCCUPIED))
 *                             unmatched++;
 *                     }
 *                 }
 *                 if (unmatched == 0) ctl->state = 7;
 *                 else { D_801D30F8 ^= 1; ctl->state = 1; }
 *                 ctl->counter = 0;
 *                 break;
 *             }
 *             case 7: {
 *                 ctl->counter++;
 *                 if (ctl->counter == 1) {
 *                     u8 cnt[2]; s32 i;
 *                     cnt[1] = 0;  cnt[0] = 0;
 *                     for (i = 0; i < 10; i++) cnt[D_801D31C0[i].initFlags & 1]++;
 *                     if (cnt[0] > cnt[1])      D_801D30FC = 0;
 *                     else if (cnt[1] > cnt[0]) D_801D30FC = 1;
 *                     else                       D_801D30FC = 2;
 *                 }
 *                 if ((u8)ctl->counter < TT_HOLD_FRAMES_TALLY) return 0;
 *                 ctl->state = 8;  ctl->counter = 0;
 *                 break;
 *             }
 *             case 8: {
 *                 if (ctl->counter == 0) {
 *                     s32 mode;
 *                     if (D_801D30FC == 2) mode = 4;
 *                     else if (D_801A2C70[D_801D30FC] < 3) { func_800A247C(3); mode = 2; }
 *                     else mode = 3;
 *                     D_801D3018 = func_8009EB30(mode);
 *                     ctl->counter++;
 *                 }
 *                 if (D_801C2EC4 & PAD_UP) {
 *                     if (D_801D3018 != 0) { func_8009EB90(D_801D3018, 1); D_801D3018 = 0; }
 *                     return 0;
 *                 }
 *                 if (D_801C2EC4 == 0) return 0;
 *                 func_800A2054(3);
 *                 if (D_801D30FC == 2 && (g_tripleTriadRules & TT_RULE_SUDDEN_DEATH)) {
 *                     if (D_801D3018 != 0) func_8009EB90(D_801D3018, 1);
 *                     func_8009953C();
 *                     D_801D30F8 ^= 1;
 *                     ctl->state = 1;
 *                 } else {
 *                     ctl->state = 9;
 *                 }
 *                 ctl->counter = 0;
 *                 break;
 *             }
 *             case 9: {
 *                 if (ctl->counter == 0) func_800A0370(TT_HOLD_FRAMES_FADE);
 *                 ctl->counter++;
 *                 if ((u8)ctl->counter < TT_HOLD_FRAMES_FADE) return 0;
 *                 if (D_801D3018 != 0) func_8009EB90(D_801D3018, 1);
 *                 {
 *                     TripleTriadData *inv = getTripleTriadData();
 *                     if (D_801D30FC == 2) {
 *                         D_80082C9C = D_801D30FC;
 *                         inv->draws++;
 *                     } else if (D_801A2C70[D_801D30FC] == 3) {
 *                         D_80082C9C = 1;
 *                         inv->defeats++;
 *                     } else {
 *                         D_80082C9C = 0;
 *                         inv->victories++;
 *                     }
 *                     D_801A2CE6 = 4;
 *                 }
 *                 return 0;
 *             }
 *         }
 *     }
 * }
 * @endverbatim
 */
INCLUDE_ASM("asm/ovl/tripletriad/nonmatchings/be_object1", func_80099C78);

/**
 * @brief Execute cleanup via func_8009AD00 and return success.
 *
 * Calls func_8009AD00 to perform cleanup/finalization, then always
 * returns 0 to indicate success.
 *
 * @param a0 Entity or context pointer passed to func_8009AD00.
 * @return Always 0.
 */
s32 func_8009A2F4(s32 a0) {
    func_8009AD00(a0);
    return 0;
}

/**
 * @brief Triple Triad: emit per-cell capture-direction marker sprites and flip
 *        the back-buffer's draw-env entry.
 *
 * Per-frame renderer that walks the 3x3 active play area (rows/cols 1..3).
 * Each board slot carries a @c pad05 bitmask whose set bits each represent a
 * captured-from direction (or similar overlay sprite) to render at that cell.
 * The function finds the lowest set bit of @c pad05, packs a 6-word combined
 * @c DR_TPAGE + @c SPRT primitive (24 bytes) into the active primitive pool
 * via @c D_801C2EB4, and links it into the OT at @c D_801C2EB0[0x1B] (the
 * 0x6C byte slot in the sort tree) using @c AddPrim. The U/V coordinates are
 * computed from the bit position: @c U = ((bit&3)<<6) + ((frameTick<<2)&0x30)
 * (animated by @c D_801A2C6C frame counter modulo 4), @c V = (bit>>2)<<4 + 0x10.
 * Cell pixel positions step by 0x40 in both axes (cell size).
 *
 * After all visible cells emit, @c D_801C2EB4 is bumped to the new tail and
 * @c func_80098A1C is called with @c &D_801C2DD0[!activeBuffer] (the OTHER
 * draw-env) plus @c D_8012E66C as the callback — registering the back-buffer
 * for the next vblank flip.
 *
 * @return Always @c 0.
 *
 * @note Matching tricks: (1) @c TripleTriadCellPrim is a 6-word combined
 *       primitive (P_TAG + DR_TPAGE + SPRT). (2) The bit-scan loop uses a
 *       redundant @c saved = colByteOffset[boardBase+5] re-load (instead of
 *       @c saved = mask) — this re-emits the @c addu for the cell address,
 *       letting gcc allocate @c v1 (matching target) instead of reusing
 *       @c v0. (3) @c v = saved >> bit at the top of the while-loop body
 *       AND in the tail (line 76 below is redundant after the bottom-of-body
 *       shift, but the compiler emits a srav for it in the back-edge delay
 *       slot for the next iter's check). (4) @c volatile s32 D_801A2C6C
 *       prevents gcc from narrowing the global load to @c lbu — the target
 *       uses a 32-bit @c lw. (5) @c i[arr] form @c colByteOffset[boardBase+5]
 *       forces the @c addu operand order @c (s2, s8) matching target.
 *       (6) @c row=1 / @c col=1 in declaration init (not the @c for header)
 *       fixes the s-reg allocation so that @c row→s7 and @c col→s4.
 *       (7) @c rowByteOffset = pixelY chains the init so gcc emits
 *       @c addu s6, s5, $0 (matching target's @c addu s6, s5, zero).
 */
typedef struct {
    /* 0x00 */ u32 tag;        /**< @c P_TAG: len=5, next=0 (six-dword primitive). */
    /* 0x04 */ u32 tpageCmd;   /**< GP0(0xE1) Draw Mode + tpage attribute = @c 0xE100060C. */
    /* 0x08 */ u32 sprtCmd;    /**< GP0(0x66) textured sprite + grey-tint RGB = @c 0x66808080. */
    /* 0x0C */ s16 x0, y0;     /**< Screen-space sprite origin (top-left). */
    /* 0x10 */ u8  u0, v0;     /**< Texture-page UV (bit-position derived). */
    /* 0x12 */ u16 clut;       /**< CLUT id (varies by lowest set bit of @c pad05). */
    /* 0x14 */ s16 w, h;       /**< Sprite size in pixels (15x15). */
} TripleTriadCellPrim;

s32 func_8009A314(void) {
    TripleTriadCellPrim *prim = (TripleTriadCellPrim *)D_801C2EB4;
    s32 row = 1;
    u8 *boardBase = (u8 *)&D_801D3398;
    s32 pixelY = 0x28;
    s32 rowByteOffset = pixelY;

    for (; row <= 3; row++) {
        s32 col = 1;
        s32 pixelX = 0x78;
        s32 colByteOffset = rowByteOffset + 8;

        for (; col <= 3; col++) {
            u8 mask = colByteOffset[boardBase + 5];

            if (mask != 0) {
                s32 bit = 0;
                s32 saved = colByteOffset[boardBase + 5];
                s32 v;

                while (1) {
                    v = saved >> bit;
                    if (v & 1) break;
                    bit++;
                    if (bit >= 8) break;
                    v = saved >> bit;
                }

                prim->tag      = 0x05000000;
                prim->tpageCmd = 0xE100060C;
                prim->clut     = (bit + 0xE1) << 6;
                prim->sprtCmd  = 0x66808080;
                prim->x0       = pixelX;
                prim->y0       = pixelY;
                prim->u0       = ((bit % 4) << 6) + ((D_801A2C6C << 2) & 0x30);
                prim->v0       = ((bit / 4) << 4) + 0x10;
                prim->h        = 0xF;
                prim->w        = 0xF;

                AddPrim((s32 *)&D_801C2EB0[0x1B], prim);
                prim++;
            }

            pixelX += 0x40;
            colByteOffset += 8;
        }

        pixelY += 0x40;
        rowByteOffset += 0x28;
    }

    D_801C2EB4 = prim;
    func_80098A1C((u8 *)&D_801C2DD0[D_801C2DCA ^ 1], D_8012E66C);
    return 0;
}

/**
 * @brief Query the list at @p node->listPtr; return 2 if empty, 0 otherwise.
 *
 * Calls @c func_80098D28 on the node's @c listPtr (a list-head pointer at
 * offset 0x0C) and returns @c 2 when that returns 0 (empty list), else 0.
 */
s32 func_8009A4E0(CallbackNode *node) {
    return (func_80098D28(node->listPtr) == 0) << 1;
}

/**
 * @brief Triple Triad: tally per-player card ownership and render the two
 *        score-digit sprites for the end-of-game tally screen.
 *
 * Walks all 10 @c BattleObject slots in @c D_801D31C0 and bins each by the
 * low bit of @c initFlags (owner: player 0 or player 1) into a 2-element
 * stack-local @c cnt[]. Then emits two 6-word combined @c DR_TPAGE + @c SPRT
 * primitives — one per player — placed at fixed screen positions
 * (player 0 at @c x=0x28, player 1 at @c x=0x140; both at @c y=0xC0). The
 * @c u0 coordinate selects a digit glyph from a tex-page strip via the
 * formula @c u0 = (cnt[p] * 24) - 24 = (cnt[p] - 1) * 24 (each glyph is
 * 24 pixels wide). Sprites are 24x24, CLUT @c 0x3A40, @c v0 = 0x30.
 *
 * @return Always @c 0.
 *
 * @note Matching tricks: (1) @c cnt[1] = 0 then @c cnt[0] = 0 (reversed
 *       init order) matches the target's @c sw zero sequence — same trick
 *       used in @c func_80099C78's case 7. (2) The branch form
 *       @c if (!(i & 1)) ... else ... — with the @c i==0 (player 0) path
 *       in the @c if body — drives gcc to emit @c bnez to the player-1
 *       (@c 0x140) branch and fall through (in delay slot) to the
 *       player-0 (@c 0x28) value, matching target's @c bnez+j layout.
 *       The @c & 1 mask is reused for both the @c x0 branch and the
 *       @c cnt[i & 1] lookup so gcc CSEs the @c andi.
 */
s32 func_8009A508(void) {
    s32 i = 0;
    s32 cnt[2];
    TripleTriadCellPrim *prim;

    cnt[1] = 0;
    cnt[0] = 0;

    for (; i < 10; i++) {
        cnt[D_801D31C0[i].initFlags & 1]++;
    }

    prim = (TripleTriadCellPrim *)D_801C2EB4;

    for (i = 0; i < 2; i++) {
        prim->tag      = 0x05000000;
        prim->tpageCmd = 0xE100060C;
        prim->sprtCmd  = 0x64808080;
        if (!(i & 1)) prim->x0 = 0x28;
        else          prim->x0 = 0x140;
        prim->y0       = 0xC0;
        prim->u0       = (cnt[i & 1] * 3 * 8) - 0x18;
        prim->v0       = 0x30;
        prim->clut     = 0x3A40;
        prim->h        = 0x18;
        prim->w        = 0x18;

        AddPrim((s32 *)&D_801C2EB0[4], prim);
        prim++;
    }

    D_801C2EB4 = prim;
    return 0;
}

/**
 * @brief Initialize the D_801D3028 linked list with battle update callbacks.
 *
 * Calls func_8009C6D8 for initial setup, then initializes D_801D3028
 * as a linked list (pool at D_801D3038, node size 0x18, capacity 8).
 * Registers four callback functions as nodes. Clears fields on the
 * first callback's node. Finally calls func_8009AD24 and returns
 * the list pointer.
 *
 * @return Pointer to D_801D3028 list header.
 */
u8 *func_8009A650(void) {
    u8 *list;
    u8 *node;
    func_8009C6D8();
    list = D_801D3028;
    func_80098BC0(list, D_801D3038, 0x18, 8);
    node = (u8 *)func_80098C44(list, (s32)func_80099C78);
    node[0x10] = 0;
    node[0x11] = 0;
    node[0x14] = 0;
    func_80098C44(list, (s32)func_8009A2F4);
    func_80098C44(list, (s32)func_8009A314);
    func_80098C44(list, (s32)func_8009A508);
    func_8009AD24();
    return list;
}

/**
 * Sets up animation rectangle parameters based on the entity type.
 *
 * Configures position and size values in a 4-halfword output structure
 * based on the type byte at a0[0]. Type 0 and 1 use vertical strips,
 * type 2 uses a tile grid layout.
 *
 * @param a0 Pointer to entity data (byte 0 = type, byte 1 = column, byte 2 = row).
 * @param a1 Pointer to output rectangle (4 s16 values: x, y, w, h).
 * @return Pointer to the output rectangle, or a1 unchanged if type is unknown.
 */
u8 *func_8009A6EC(u8 *a0, s16 *a1) {
    u8 type = a0[0];

    switch (type) {
    case 0:
        a1[0] = -0x8C;
        {
            u8 r = a0[2];
            s32 w = 0x200;
            a1[2] = w;
            a1[1] = r * 32 - 0x40;
        }
        a1[3] = -(s32)a0[2] + 0xE;
        break;
    case 1:
        a1[0] = 0x8C;
        {
            u8 r = a0[2];
            s32 w = 0x200;
            a1[2] = w;
            a1[1] = r * 32 - 0x40;
        }
        a1[3] = -(s32)a0[2] + 0xE;
        break;
    case 2: {
        s32 col = a0[1];
        a1[0] = (col - 1) * 64;
        {
            u8 row = a0[2];
            a1[2] = 0x200;
            a1[3] = 0x12;
            a1[1] = (row - 1) * 64;
        }
        break;
    }
    default:
        return (u8 *)a1;
    }
    return (u8 *)a1;
}

/**
 * @brief Find a battle-object slot in @c D_801D31C0 matching a search key.
 *
 * Three search modes are dispatched off @p groupId:
 *  - @c groupId @c <0: invalid — returns @c -1.
 *  - @c groupId @c 0 or @c 1: scan the 10 slots for an @b active entry
 *    (@c flags @c & @c 1) whose @c groupId matches and whose @c priority
 *    matches @p priority. @p fieldD is ignored.
 *  - @c groupId @c ==2: scan for an entry (active or not) whose
 *    @c groupId is @c 2, @c fieldD matches @p fieldD, and @c priority
 *    matches @p priority.
 *  - @c groupId @c >2: invalid — returns @c -1.
 *
 * @param groupId  Search-mode selector and target @c BattleObject.groupId.
 * @param fieldD   Target @c BattleObject.fieldD (only used in mode 2).
 * @param priority Target @c BattleObject.priority.
 * @return Slot index @c 0..9 of the first matching entry, or @c -1 if
 *         none found / invalid mode.
 */
s32 func_8009A7A4(s32 groupId, s32 fieldD, s32 priority) {
    s32 i;

    if (groupId < 0) return -1;

    switch (groupId) {
    case 0:
    case 1:
        for (i = 0; i < 10; i++) {
            if (D_801D31C0[i].flags & 1) {
                if (D_801D31C0[i].groupId == groupId) {
                    if (D_801D31C0[i].priority == priority) {
                        return i;
                    }
                }
            }
        }
        return -1;

    case 2:
        for (i = 0; i < 10; i++) {
            if (D_801D31C0[i].groupId == groupId) {
                if (D_801D31C0[i].fieldD == fieldD) {
                    if (D_801D31C0[i].priority == priority) {
                        return i;
                    }
                }
            }
        }
        return -1;
    }

    return -1;
}

/**
 * @brief Mark a battle entity's flags with bit 2.
 *
 * Looks up an entity index via func_8009A7A4, then sets bit 1 (0x2)
 * in the flags halfword at offset +4 of the entity's 36-byte entry
 * in D_801D31C0.
 *
 * @param a0 Entity search key passed to func_8009A7A4.
 * @param a1 Secondary parameter passed as third arg to func_8009A7A4.
 */
void func_8009A878(s32 a0, s32 a1) {
    s32 idx = func_8009A7A4(a0, 0, a1);
    if (idx >= 0) {
        D_801D31C0[idx].flags |= 2;
    }
}
