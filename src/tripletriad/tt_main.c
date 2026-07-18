#include "common.h"
#include "item.h"
#include "tripletriad.h"
#include "psxsdk/libc.h"
#include "psxsdk/libgpu.h"
#include "tripletriad/tt_main.h"
#include "tripletriad/tt_game.h"
#include "tripletriad/tt_script.h"
#include "tripletriad/tt_ui.h"
#include "gamestate.h"


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

/**
 * @brief Set up the debug-text overlay's buffers for the first frame.
 *
 * Queues the text-font TIM upload, clears the current text ordering table, and
 * points the framebuffer/OT cursors at the active buffer. Like @ref
 * flipTextBuffer but without flipping the buffer index first.
 */
void initTextBuffer(void) {
    queueTimUpload(&g_textBufferRes);
    ClearOTag(g_textOTs[g_textBufferIndex], 2);
    g_textFbPtr = g_textFrameBufs[g_textBufferIndex];
    g_textOtPtr = g_textOTs[g_textBufferIndex];
}

/**
 * @brief Set the debug-text cursor origin; each axis is updated only if >= 0.
 *
 * @param x New line/cursor X (ignored if negative).
 * @param y New cursor Y (ignored if negative).
 */
void setTextOrigin(s32 x, s32 y) {
    if (x >= 0) {
        g_textLineX = x;
        g_textCursorX = x;
    }
    if (y >= 0) {
        g_textCursorY = y;
    }
}

/**
 * @brief Emit one debug-text glyph as a 5x5 textured sprite and advance the cursor.
 *
 * @p ch is folded to uppercase and drawn from the font atlas. Newline returns
 * to the line origin and steps down 7 px; space and control bytes advance the
 * cursor without emitting a glyph.
 *
 * @param ch Character to render.
 */
void drawTextChar(u8 ch) {
    SPRT *prim;
    s32 adjusted;
    s32 row;

    prim = (SPRT *)g_textFbPtr;

    if (ch == '\n') {
        g_textCursorX = g_textLineX;
        g_textCursorY += 7;
        return;
    }

    /* Any space or control byte: just advance the cursor, no glyph. */
    if (ch <= ' ') {
        g_textCursorX += 6;
        return;
    }

    /* Lowercase 'a'..'z': fold to uppercase. */
    if ((u8)(ch - 'a') < 26) {
        ch -= 'a' - 'A';
    }

    SetSprt(prim);

    /* Drop down to glyph index relative to space (the first printable char). */
    adjusted = ch - ' ';

    prim->r0 = g_textColor.r;
    prim->g0 = g_textColor.g;
    prim->b0 = g_textColor.b;

    prim->x0 = g_textCursorX;
    prim->y0 = g_textCursorY;

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

    AddPrim((u32 *)g_textOtPtr, prim);

    g_textFbPtr = (u8 *)(prim + 1);
    g_textCursorX += 6;
}

/**
 * @brief Render a debug-text string, honoring @c '#<digit>' color escapes.
 *
 * Each character is drawn via @ref drawTextChar. A @c '#' followed by a digit
 * @c '0'..'8' switches the text color to that @c g_textPalette entry; any other
 * byte after @c '#' is drawn literally.
 *
 * @param str Null-terminated string to render.
 */
void drawText(u8 *str) {
    u8 ch;

    for (ch = *str++; ch != 0; ch = *str++) {
        s32 digitEscape = '#';
        if (ch == digitEscape) {
            s32 next = *str++;
            s32 digit = next & 0xFF;
            if (digit < '9') {
                if (digit >= '0') {
                    /* Set color from palette entry indexed by ASCII digit
                       (the - '0' folds into the relocation: %lo(sym - 0xC0)). */
                    memcpy(&g_textColor, &g_textPalette[digit - '0'], 4);
                } else {
                    drawTextChar(next);
                }
            } else {
                drawTextChar(next);
            }
        } else {
            drawTextChar(ch);
        }
    }
}

/**
 * @brief Format @p value as a signed decimal string into @p out.
 *
 * @param value Value to convert.
 * @param out   Output buffer.
 * @return Pointer to the written string's terminating null.
 */
u8 *intToDecStr(s32 value, u8 *out) {
    u8 buf[36];
    u8 *dst = out;
    u8 *p;

    if (value < 0) {
        *dst = '-';
        dst++;
        value = -value;
    }

    p = buf + 33;
    buf[33] = 0;

    do {
        p--;
        *p = (value % 10) + '0';
        value = value / 10;
    } while (value != 0);

    strcpy(dst, p);
    return dst + strlen(dst);
}

/**
 * @brief Format @p value as a signed hexadecimal string into @p out.
 *
 * @param value Value to convert.
 * @param out   Output buffer.
 * @return Pointer to the written string's terminating null.
 */
u8 *intToHexStr(s32 value, u8 *out) {
    u8 buf[20];
    u8 *dst = out;
    u8 *p;
    u8 *table;

    if (value < 0) {
        *dst = 0x2D;
        dst++;
        value = -value;
    }

    p = buf + 17;
    buf[17] = 0;
    table = g_hexDigits;

    do {
        p--;
        *p = *(u8 *)((value & 0xF) + (s32)table);
        value >>= 4;
    } while (value != 0);

    strcpy(dst, p);
    return dst + strlen(dst);
}

/**
 * @brief Format @p value as a binary-digit string into @p out.
 *
 * @param value Value whose bits are written (no sign handling).
 * @param out   Output buffer.
 * @return Pointer to the written string's terminating null.
 */
u8 *intToBinStr(s32 value, u8 *out) {
    u8 buf[36];
    u8 *dst = out;
    u8 *p;

    p = buf + 33;
    buf[33] = 0;

    do {
        p--;
        *p = (value & 1) + '0';
        value >>= 1;
    } while (value != 0);

    strcpy(dst, p);
    return dst + strlen(dst);
}

/**
 * @brief printf-style formatter: render @p args into @p dst.
 *
 * @c args[0] is the format string; @c args[1..] supply the values. Supported
 * specifiers (lower or upper case):
 *   - @c %d / @c %x / @c %b : signed decimal / hexadecimal / binary
 *   - @c %c : one character
 *   - @c %s : null-terminated string
 *
 * A @c % may carry an optional @c '0' (zero-pad, default space) and a decimal
 * field width.
 *
 * @param dst  Output buffer; receives the null-terminated result.
 * @param args Argument array: the format string followed by its values.
 * @return Length of the result, excluding the null terminator.
 */
s32 formatString(char *dst, s32 *args) {
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
            width = strtol(fmt, &fmt, 0);
            ch = *fmt++;
            switch (ch) {
                case 'D': case 'd':
                    intToDecStr(*args++, buf);
                    break;
                case 'X': case 'x':
                    intToHexStr(*args++, buf);
                    break;
                case 'B': case 'b':
                    intToBinStr(*args++, buf);
                    break;
                case 'C': case 'c':
                    buf[0] = (u8)*args++;
                    buf[1] = 0;
                    break;
                case 'S': case 's':
                    strcpy(buf, (char *)*args++);
                    break;
                default:
                    buf[0] = ch;
                    buf[1] = 0;
                    break;
            }
            if (width > 0) {
                s32 len = strlen(buf);
                if (len < width) {
                    s32 i;
                    width -= len;
                    for (i = 0; i < width; i++) {
                        *out++ = padCh;
                    }
                }
            }
            strcpy(out, buf);
            out += strlen(buf);
        } else {
            *out++ = ch;
        }
        ch = *fmt++;
    } while (ch != 0);
end:
    *out = 0;
    return strlen(dst);
}

/**
 * @brief printf-style: format the variadic args into @p dst via @ref formatString.
 *
 * @param dst Output buffer.
 * @param fmt Format string, followed by its values.
 * @return Length of the result, excluding the null terminator.
 */
s32 ttSprintf(s32 dst, s32 fmt, ...) {
    return formatString((char *)dst, &fmt);
}

/**
 * @brief printf-style debug print: format the args and render them via @ref drawText.
 *
 * @param fmt Format string, followed by its values.
 */
void drawTextf(s32 fmt, ...) {
    s8 buf[256];
    formatString((char *)buf, &fmt);
    drawText(buf);
}

/**
 * @brief Present and flip the debug-text overlay's double buffer.
 *
 * Draws the current text ordering table, swaps @c g_textBufferIndex, resets the
 * text cursor to the origin, and rebases the framebuffer/OT cursors to the new
 * buffer. Independent of the main @ref flipBuffers double buffer.
 */
void flipTextBuffer(void) {
    u8 *fb = g_textFbPtr;

    SetDrawTPage(fb, 0, 1, 3);
    AddPrim(g_textOtPtr, fb);
    DrawOTag(g_textOtPtr);

    g_textBufferIndex = g_textBufferIndex ^ 1;
    ClearOTag(g_textOTs[g_textBufferIndex], 2);

    g_textLineX = 8;
    g_textCursorX = 8;
    g_textCursorY = 8;

    g_textFbPtr = g_textFrameBufs[g_textBufferIndex];
    g_textOtPtr = g_textOTs[g_textBufferIndex];
}

/**
 * @brief Set up the 10 Triple Triad card objects for a new match.
 *
 * Resets the board, then assigns each @c g_tripleTriadCardHands slot to a player
 * (the low bit of its @c initFlags) and a sequence index within that player's
 * hand. When a player uses the offset hand layout (@c D_801A2C70[player] @c == @c 3)
 * and the matching rule is off, the card is shifted sideways so it draws offset.
 */
void initCardHands(void) {
    s32 cnt[2];
    s32 i;
    s32 owner;
    TripleTriadCardObject *entry;

    resetTriadBoard();

    cnt[1] = 0;
    cnt[0] = 0;

    for (i = 0; i < 10; i++) {
        s32 seq;
        entry = &g_tripleTriadCardHands[i];
        owner = entry->initFlags & 1;
        entry->fieldD = 0;
        entry->groupId = owner;
        seq = cnt[owner];
        entry->priority = seq;
        cnt[owner] = seq + 1;
        if (D_801A2C70[owner] == 3 && !(g_tripleTriadRules & 1)) {
            entry->posData[1] = 0x800;
        }
    }

    D_801D3340[1].field2 = 0;
    D_801D3340[2].field2 = 0;
}

/**
 * @brief Project the tetrahedral icon model and emit a @c POLY_G3 per visible face.
 *
 * Transforms the 4 model vertices through the GTE, then for each of the 4 faces
 * in @c g_triadIconFaces back-face culls it and, if visible, writes a @c POLY_G3
 * with the face's colors and projected vertices and links it into @p ot.
 *
 * @param ot    Ordering table the primitives are chained into.
 * @param prims Output buffer for the primitives (up to 4 emitted).
 * @return The next free primitive slot after the last one emitted.
 */
POLY_G3 *drawTriadIcon(void *ot, POLY_G3 *prims) {
    POLY_G3 *out;
    s32 i;
    TriadFaceDesc *face;
    s32 *ptr;

    ptr = (s32 *)scratchAlloc(0x18);
    g_triadIconScratch = (u32 *)ptr;

    RotTransPers4(&g_triadIconVerts[0], &g_triadIconVerts[1], &g_triadIconVerts[2], &g_triadIconVerts[3],
                  &ptr[0], &ptr[1], &ptr[2], &ptr[3], &ptr[4], &ptr[5]);

    out = prims;
    for (i = 0; i < 4; i++) {
        face = &g_triadIconFaces[i];
        if (NormalClip(g_triadIconScratch[face->v0], g_triadIconScratch[face->v1], g_triadIconScratch[face->v2]) >= 0) {
            out->tag = 0x06000000;
            *(u32 *)&out->r0 = face->color0Word;
            *(u32 *)&out->r1 = face->color1Word;
            *(u32 *)&out->r2 = face->color2Word;
            *(u32 *)&out->x0 = g_triadIconScratch[face->v0];
            *(u32 *)&out->x1 = g_triadIconScratch[face->v1];
            *(u32 *)&out->x2 = g_triadIconScratch[face->v2];
            AddPrim(ot, out);
            out++;
        }
    }

    scratchFree(0x18);
    return out;
}

/**
 * @brief Per-frame card-flip animation handler (a @c g_tripleTriadStateHandlers entry).
 *
 * Drives the card's flip animation through four states — init, entry arc, idle,
 * and re-flip — animating a YXZ rotation and a morphing position vector. Each
 * frame it composes the GTE transform and emits the card's icon via @ref
 * drawTriadIcon into the active ordering table. The card re-flips whenever
 * @c g_cardFlipPhase disagrees with the node's current phase.
 *
 * @param node Handler node (state/counter/phase at 0x10..0x12).
 * @return Always 0 (the handler keeps running).
 */
s32 cardFlipHandler(HandlerNode *node) {
    s32 tmp;
    u32 *ot;

    g_cardFlipXform = (TransformBuf *)scratchAlloc(0x28);
    switch (node->state) {
    case CARD_FLIP_INIT:
        node->phase = func_80023D04() % 2;
        g_cardFlipSpin = !node->phase ? -0x400 : 0x400;
        g_cardFlipTarget.vx = !node->phase ? -0x8C : 0x8C;
        g_cardFlipXform->vec = g_cardFlipUpVec;
        playTriadSfx(0x70);
        node->state = CARD_FLIP_ENTER;
        node->counter = 0;
        break;
    case CARD_FLIP_ENTER: {
        s32 frame = node->counter;
        if (frame < 60) {
            s32 t = (frame << 12) / 60;
            s32 sine = rsin(t / 4);
            g_cardFlipAngles.vx = (u32)t >> 2;
            g_cardFlipAngles.vy = ((g_cardFlipSpin + 0xA000) * sine) >> 12;
            g_cardFlipXform->vec = g_cardFlipUpVec;
        } else if ((frame -= 60) < 10) {
            g_cardFlipXform->vec = g_cardFlipUpVec;
        } else if ((frame -= 10) < 15) {
            s32 d = (frame << 12) / 15;
            tmp = d;
            g_cardFlipAngles.vx = (-(d << 10) >> 12) + 0x400;
            g_cardFlipAngles.vy = g_cardFlipSpin + (((-g_cardFlipSpin) * d) >> 12);
            func_8003F884(&g_cardFlipUpVec, &g_cardFlipTarget, 0x1000 - tmp, d, &g_cardFlipXform->vec);
            d = (rsin(d / 2) << 4) >> 12;
            g_cardFlipXform->vec.vy -= d;
        } else {
            g_cardFlipAngles.vx = 0;
            g_cardFlipXform->vec = g_cardFlipTarget;
            g_cardFlipPhase = node->phase;
            node->state = CARD_FLIP_IDLE;
            node->counter = 0;
        }
        node->counter++;
        break;
    }
    case CARD_FLIP_IDLE: {
        s32 xPos = 0x8C;
        g_cardFlipAngles.vy += 0x10;
        xPos = !node->phase ? -0x8C : xPos;
        g_cardFlipXform->vec.vy = -0x5C;
        g_cardFlipXform->vec.vz = 0x200;
        g_cardFlipXform->vec.vx = xPos;
        tmp = g_cardFlipPhase != node->phase;
        if (tmp) {
            node->state = CARD_FLIP_REFLIP;
            node->counter = 0;
        }
        break;
    }
    case CARD_FLIP_REFLIP: {
        s32 t = (node->counter << 12) / 10;
        s32 sine = rsin(t / 4);
        if (node->phase) {
            sine = 0x1000 - sine;
        }
        g_cardFlipXform->vec.vx = ((sine * 0x118) >> 12) - 0x8C;
        g_cardFlipXform->vec.vy = -0x5C;
        g_cardFlipXform->vec.vz = (-(rsin(t / 2) << 6) >> 12) + 0x200;
        node->counter++;
        if (node->counter >= 10) {
            node->state = CARD_FLIP_IDLE;
            node->counter = 0;
            node->phase ^= 1;
        }
        break;
    }
    }
    RotMatrixYXZ(&g_cardFlipAngles, &g_cardFlipXform->mat);
    g_cardFlipXform->mat.t[0] = g_cardFlipXform->vec.vx;
    g_cardFlipXform->mat.t[1] = g_cardFlipXform->vec.vy;
    g_cardFlipXform->mat.t[2] = g_cardFlipXform->vec.vz;
    func_80041794(0x100, &g_cardFlipXform->mat);
    SetRotMatrix(&g_cardFlipXform->mat);
    SetTransMatrix(&g_cardFlipXform->mat);
    ot = &g_otBase[4];
    g_primCursor = drawTriadIcon(ot, g_primCursor);
    scratchFree(0x28);
    return 0;
}
