#include "common.h"
#include "tripletriad.h"
#include "psxsdk/libc.h"
#include "psxsdk/libgpu.h"
#include "tripletriad/tt_main.h"

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
