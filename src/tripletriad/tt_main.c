#include "common.h"
#include "item.h"
#include "tripletriad.h"
#include "psxsdk/libc.h"
#include "psxsdk/libgpu.h"
#include "tripletriad/tt_main.h"
#include "tripletriad/tt_text.h"
#include "tripletriad/tt_game.h"
#include "tripletriad/tt_script.h"
#include "tripletriad/tt_ui.h"
#include "gamestate.h"



/**
 * @brief Latch the field-VM encounter parameters into the Triple Triad globals.
 *
 * Reads @ref D_80082C90 (the @ref EncounterParams block the field VM fills in
 * before launching a card game) and stores its pieces where the engine reads
 * them for the whole session:
 *  - @ref g_tripleTriadRules = @c encounterPtr (for card games the word is the
 *    TT_RULE_* bitmask rather than a pointer);
 *  - @ref D_801A2C44 = @c field_04 (the trade-rule selector);
 *  - @ref D_801A2C70 per-player layout types: a negative rules word selects
 *    type 2 for both players, bit 30 selects the offset-hand layout (type 3)
 *    for both, otherwise player 0 gets type 3 and player 1 type 2.
 *
 * First initializer run by @ref initTripleTriad, defined just ahead of it
 * (its only caller) as the first function in tt_main.c.
 */
void func_800981BC(void) {
    s32 rules = D_80082C90.encounterPtr;

    g_tripleTriadRules = rules;
    D_801A2C44 = D_80082C90.field_04;
    if (rules < 0) {
        D_801A2C70[0] = 2;
        D_801A2C70[1] = 2;
    } else if (rules & 0x40000000) {
        D_801A2C70[0] = 3;
        D_801A2C70[1] = 3;
    } else {
        D_801A2C70[0] = 3;
        D_801A2C70[1] = 2;
    }
}


/**
 * @brief Initialize the Triple Triad engine for a session: run the subsystem
 *        initializers, reset match state, and tally the player's owned count
 *        for each card.
 */
void initTripleTriad(void) {
    s32 i;

    func_800A2D34();
    func_800981BC();
    initScratchHeap();
    initTriadTaskPool();
    func_80098B68();
    initGraphics();
    resetTriadMenuState();
    func_800A1BE0();
    func_800A2208();
    initTextBuffer();
    initGradientFadeList();
    queueTimUpload(&g_tripleTriadCardArt);
    queueTimUpload(&g_tripleTriadCardFrames);

    g_tripleTriadInputFlags = 0;
    g_tripleTriadFrameCount = 0;
    g_tripleTriadState = TT_STATE_INIT;

    for (i = 0; i < 110; i++) {
        g_tripleTriadCardCounts[i] = func_80023B14(i);
    }
}

/**
 * @brief Triple Triad main loop — drives the game until it signals exit.
 *
 * Initializes the engine, then runs one iteration per frame: service input,
 * advance the state-machine handlers, and tick the per-frame subsystems. On
 * exit it fades out, runs two finalize frames, and restores the vsync rate.
 *
 * @return Always 0.
 */
s32 tripleTriadMainLoop(void) {
    s32 i;

    initTripleTriad();

    do {
        if (g_tripleTriadInputFlags & TT_INPUT_DISABLED) {
            if (g_padPressed[2] & 0x30) {
                closeMenu();
            }
            for (i = 0; i < 3; i++) {
                g_padPressed[i] = 0;
                g_padHeld[i] = g_padRepeat[i] = 0;
            }
        }

        while (g_tripleTriadState != TT_STATE_IDLE && g_tripleTriadState != TT_STATE_EXIT) {
            g_tripleTriadActiveList = g_tripleTriadStateHandlers[g_tripleTriadState - 1]();
            if (g_tripleTriadActiveList != 0) {
                g_tripleTriadState = TT_STATE_IDLE;
            }
        }

        if (g_tripleTriadActiveList != 0) updateObjectList(g_tripleTriadActiveList);
        processTriadTasks();
        updateFadeEffects();
        updateTriadMenu();
        func_800A1C6C();
        flipBuffers();
        sampleInput();
        flushTriadSfxQueue();
        g_tripleTriadFrameCount++;
    } while (g_tripleTriadState != TT_STATE_EXIT);

    g_fadeCounter = -1;
    for (i = 0; i < 2; i++) {
        func_800A1C6C();
        flipBuffers();
    }
    clearAllSfx();
    g_vsyncRate = 100;
    return 0;
}

/**
 * @brief Initialize the GPU draw/display environments and the GTE.
 *
 * Sets up the two double-buffered draw/display environments, clears the
 * framebuffer and texture VRAM, primes the ordering table and primitive
 * pool, then initializes the GTE and masks the display. Called once during
 * engine startup.
 */
void initGraphics(void) {
    DISPENV *disp;
    RECT *screen0;
    RECT *screen1;

    SetDefDrawEnv(&g_drawEnvs[0], 0, 0, TT_DRAW_W, TT_SCREEN_H);
    disp = g_dispEnvs;
    SetDefDispEnv(&disp[0], TT_DRAW_W, 0, TT_DRAW_W, TT_SCREEN_H);
    screen0 = &disp[0].screen;
    screen0->x = 0;
    screen0->y = TT_SCREEN_Y;
    screen0->w = TT_SCREEN_W;
    screen0->h = TT_SCREEN_H;

    SetDefDrawEnv(&g_drawEnvs[1], TT_DRAW_W, 0, TT_DRAW_W, TT_SCREEN_H);
    SetDefDispEnv(&disp[1], 0, 0, TT_DRAW_W, TT_SCREEN_H);
    disp[1].screen.x = 0;
    screen1 = &disp[1].screen;
    screen1->y = TT_SCREEN_Y;
    screen1->w = TT_SCREEN_W;
    screen1->h = TT_SCREEN_H;

    ClearImage(&g_fbClearRect, 0, 0, 0);
    g_fbClearRect.x = TT_DRAW_W;
    g_fbClearRect.y = 0;
    ClearImage(&g_fbClearRect, 0, 0, 0);

    ClearImage(&g_texClearRect, 0xFF, 0xFF, 0xFF);

    g_drawBufferIndex = 0;
    g_otBase = &g_orderingTables[0][0];
    ClearOTagR(&g_orderingTables[0][0], TT_OT_LEN);
    g_primCursor = &g_primPools[g_drawBufferIndex][0];

    InitGeom();
    SetGeomOffset(TT_DRAW_W / 2, TT_SCREEN_H / 2);
    SetGeomScreen(TT_PROJ_DIST);
    SetDispMask(0);

    g_fadeCounter = 2;
    g_vsyncMode = 0;
    resetVramQueue();
}

/**
 * @brief Present the frame: wait for vsync, step the fade, and swap the double buffer.
 *
 * Advances @c g_fadeCounter (toggling the display mask as it reaches zero),
 * swaps @c g_drawBufferIndex, draws the just-finished buffer's ordering table,
 * and resets the new buffer's OT and primitive pool for the next frame.
 */
void flipBuffers(void) {
    flipTextBuffer();
    VSync(1);
    DrawSync(0);
    VSync(1);
    VSync(g_vsyncMode);

    if (g_fadeCounter > 0) {
        g_fadeCounter--;
        if (g_fadeCounter == 0) {
            SetDispMask(1);
        }
    } else if (g_fadeCounter < 0) {
        g_fadeCounter++;
        if (g_fadeCounter == 0) {
            SetDispMask(0);
        }
    }

    g_drawBufferIndex ^= 1;

    PutDrawEnv(&g_drawEnvs[g_drawBufferIndex]);
    PutDispEnv(&g_dispEnvs[g_drawBufferIndex]);
    flushVramTransfers();

    DrawOTag(&g_orderingTables[g_drawBufferIndex ^ 1][TT_OT_LEN - 1]);
    g_primCursor = &g_primPools[g_drawBufferIndex][0];
    g_otBase = &g_orderingTables[g_drawBufferIndex][0];
    ClearOTagR(&g_orderingTables[g_drawBufferIndex][0], TT_OT_LEN);

    g_activeDrawEnv = &g_drawEnvs[g_drawBufferIndex ^ 1];
}

/**
 * @brief Sample the controllers and publish pad 0's button masks.
 *
 * Reads both pads via @c readPads, then writes pad 0's held / pressed / repeat
 * masks into all three slots of @c g_padHeld / @c g_padPressed / @c g_padRepeat.
 */
void sampleInput(void) {
    s32 i;

    readPads();
    g_padHeld[0] = getPadHeld(0);
    g_padPressed[0] = getPadPressed(0);
    g_padRepeat[0] = getPadRepeat(0);

    for (i = 1; i < 3; i++) {
        g_padHeld[i] = g_padHeld[0];
        g_padPressed[i] = g_padPressed[0];
        g_padRepeat[i] = g_padRepeat[0];
    }
}

/**
 * @brief Empty the deferred VRAM-transfer queue.
 */
void resetVramQueue(void) {
    g_vramQueueCount = 0;
}

/**
 * @brief Run every queued VRAM transfer, then empty the queue.
 *
 * Each entry performs its requested GPU transfer (LoadImage, TIM upload,
 * StoreImage, or MoveImage). The queue is filled during the frame by the
 * @c queue* helpers and drained here after vsync.
 */
void flushVramTransfers(void) {
    s32 i;
    PoolEntry *p = g_vramQueue;

    for (i = 0; i < g_vramQueueCount; p++, i++) {
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
    g_vramQueueCount = 0;
}

/**
 * @brief Queue a LoadImage (copy @p src pixels into VRAM @p rect) for the next flush.
 *
 * @param rect Destination VRAM rectangle.
 * @param src  Source pixel data.
 */
void queueLoadImage(RECT *rect, void *src) {
    PoolEntry *entry = &g_vramQueue[g_vramQueueCount++];
    entry->active = 0;
    memcpy(&entry->rect, rect, 8);
    entry->src = src;
}

/**
 * @brief Queue a TIM (CLUT + image) upload for the next flush and return the
 *        resource's payload address.
 *
 * The return value follows @p res's offset chain to the start of its data.
 *
 * @param res Resource header for the TIM.
 * @return The resource's resolved payload address.
 */
u8 *queueTimUpload(ResHeader *res) {
    PoolEntry *entry;
    s32 *rel;

    entry = &g_vramQueue[g_vramQueueCount++];
    entry->active = 1;
    entry->src = res;

    rel = &res->offset;
    rel = (s32 *)((u8 *)rel + *rel);
    return (u8 *)rel + *rel;
}

/**
 * @brief Queue a StoreImage (read VRAM @p rect back into @p dst) for the next flush.
 *
 * @param rect Source VRAM rectangle.
 * @param dst  Destination buffer in main RAM.
 */
void queueStoreImage(RECT *rect, void *dst) {
    PoolEntry *entry = &g_vramQueue[g_vramQueueCount++];
    entry->active = 2;
    memcpy(&entry->rect, rect, 8);
    entry->src = dst;
}

/**
 * @brief Queue a MoveImage (VRAM-to-VRAM copy of @p rect to @p dstX, @p dstY)
 *        for the next flush.
 *
 * @param rect Source VRAM rectangle.
 * @param dstX Destination X in VRAM.
 * @param dstY Destination Y in VRAM.
 */
void queueMoveImage(RECT *rect, s16 dstX, u16 dstY) {
    PoolEntry *entry = &g_vramQueue[g_vramQueueCount++];
    entry->active = 3;
    memcpy(&entry->rect, rect, 8);
    entry->src = (void *)(((s32)dstY << 16) | dstX);
}

/** @brief Empty stub (no-op). */
void func_80098B68(void) {
}

/**
 * @brief Reset the scratchpad allocator to the base of scratchpad RAM.
 */
void initScratchHeap(void) {
    g_scratchPtr = 0x1F800000;
}

/**
 * @brief Allocate @p size bytes from the scratchpad (4-byte-aligned bump allocator).
 *
 * @param size Bytes to allocate (rounded up to a multiple of 4).
 * @return Start of the allocated block.
 */
s32 scratchAlloc(s32 size) {
    s32 old = g_scratchPtr;
    g_scratchPtr = old + ((size + 3) & ~3);
    return old;
}

/**
 * @brief Release the most recent scratchpad allocation (bump-allocator pop).
 *
 * @param size Bytes to release (rounded up to 4); must match the paired @ref scratchAlloc.
 */
void scratchFree(s32 size) {
    g_scratchPtr -= (size + 3) & ~3;
}

/**
 * @brief Initialize an @c ObjList header and mark every pool node free.
 *
 * @param listMem List header to initialize.
 * @param pool    Node-pool base.
 * @param stride  Size of each node in bytes.
 * @param count   Number of nodes in the pool.
 */
void initObjList(ObjList *list, u8 *pool, s32 stride, s32 count) {
    s32 i;

    list->head = 0;
    list->tail = 0;
    list->pool = pool;
    list->stride = stride;
    list->count = count;

    for (i = 0; i < count; i++) {
        ((ObjListNode *)pool)->flags = 0;
        pool += stride;
    }
}

/**
 * @brief Find and return the first free node in the pool.
 *
 * Scans the pool for a node whose @c flags bit 0 is clear (inactive).
 *
 * @param listMem List header whose pool is scanned.
 * @return The first free node, or NULL if all nodes are in use.
 */
void *findFreeNode(ObjList *list) {
    s32 count = list->count;
    u8 *node = list->pool;
    s32 i = 0;

    if (count > 0) {
        s32 n = count;
        do {
            if (!(((ObjListNode *)node)->flags & 1)) {
                return node;
            }
            i++;
            node += list->stride;
        } while (i < n);
    }
    return 0;
}

/**
 * @brief Allocate a node from the pool and append it to the list tail.
 *
 * @param listMem  List header.
 * @param callback Per-frame callback stored in the new node.
 * @return The new node, or NULL if the pool is full.
 */
void *allocObjNode(ObjList *list, ObjNodeFn callback) {
    ObjListNode *node = findFreeNode(list);

    if (node != 0) {
        ObjListNode *tail;
        node->flags |= 1;
        node->field02 = 0;
        node->next = 0;
        node->callback = callback;
        tail = list->tail;
        if (tail != 0) {
            tail->next = node;
        } else {
            list->head = node;
        }
        list->tail = node;
    }
    return node;
}

/**
 * @brief Allocate a node and prepend it to the list head.
 *
 * @param listMem  List header.
 * @param callback Per-frame callback stored in the new node.
 * @return The new node, or NULL if the pool is full.
 */
void *allocObjNodeFront(ObjList *list, ObjNodeFn callback) {
    ObjListNode *node = findFreeNode(list);

    if (node != 0) {
        node->flags |= 1;
        node->field02 = 0;
        node->callback = callback;
        node->next = list->head;
        list->head = node;
    }
    return node;
}

/**
 * @brief Tick every node's callback, unlinking those that report completion.
 *
 * Walks the list, calling each node's @c callback; a return value with bit 1
 * set unlinks (and frees) that node. Surviving nodes are counted and the tail
 * pointer is rebuilt.
 *
 * @param listMem List header.
 * @return Number of nodes still live.
 */
s32 updateObjectList(ObjList *list) {
    ObjListNode *prev = 0;
    ObjListNode *node = list->head;
    s32 count = 0;
    s32 result;

    while (node != 0) {
        result = node->callback(node);
        if (result & 2) {
            node->flags = 0;
            if (prev != 0) {
                prev->next = node->next;
            } else {
                list->head = node->next;
            }
        } else {
            prev = node;
            count++;
        }
        node = node->next;
    }
    list->tail = prev;
    return count;
}
