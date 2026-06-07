#include "common.h"
#include "cd.h"
#include "sound.h"
#include "field.h"
#include "world.h"
#include "world/we_object1.h"
#include "world/we_object2.h"   /* getAngleDelta */
#include "psxsdk/libapi.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libc.h"


INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object1", func_800997E8);


/**
 * @brief Walk an entity's world-engine script, blitting each entry's image(s),
 *        then refresh a fixed VRAM region unless the display mode forbids it.
 *
 * When @c entity->field46 bit 0 is set, walks the 0-terminated @p script offset
 * table: each non-zero entry is a byte-offset to a streaming-image record at
 * @c (u8 *)script + offset, which @c func_8009CA34 parses into an @ref ImageDesc.
 * @c rect1 / @c data1 are blitted through the scratch @c D_800C8640
 * (via @c func_80048EFC), and — when bit 3 of @c desc.flag is set —
 * @c rect2 / @c data2 are blitted too.
 *
 * Afterwards, unconditionally: if the low 5 bits of @c D_800D2440 are neither 3
 * nor 4, the fixed VRAM rectangle @c {0x360,0x100,0x10,0x40} is refreshed via
 * @c func_80048DD4.
 *
 * @param entity Source entity; bit 0 of @c field46 gates the script walk.
 * @param script World-engine script — a 0-terminated table of byte-offsets,
 *               each pointing to an image record within the same buffer.
 */
void func_80099B48(Entity *entity, s32 *script) {
    s32 mode;

    if (entity->field46 & 1) {
        s32 i = 0;
        s32 off = script[0];
        while (off != 0) {
            ImageDesc desc;
            func_8009CA34((s32 *)((u8 *)script + off), &desc);
            D_800C8640.x = desc.rect1.x;
            D_800C8640.y = desc.rect1.y;
            D_800C8640.w = desc.rect1.w;
            D_800C8640.h = desc.rect1.h;
            func_80048EFC(&D_800C8640, desc.data1);
            if ((desc.flag >> 3) & 1) {
                D_800C8640.x = desc.rect2.x;
                D_800C8640.y = desc.rect2.y;
                D_800C8640.w = desc.rect2.w;
                D_800C8640.h = desc.rect2.h;
                func_80048EFC(&D_800C8640, desc.data2);
            }
            i++;
            off = script[i];
        }
    }
    mode = D_800D2440 & 0x1F;
    if (mode != 3 && mode != 4) {
        RECT rect;
        rect.x = 0x360;
        rect.y = 0x100;
        rect.w = 0x10;
        rect.h = 0x40;
        func_80048DD4(&rect, 0, 0, 0);
    }
}

/**
 * @brief (Re)initialise the world-map sound sequence and reload the sample banks.
 *
 * Decides whether to reload the current sequence: it skips the reload (just issuing
 * @c sndCmdC1(0,0x1E,0x7F)) only when the scatter key @c D_800D2442 already matches
 * @c g_fieldVars->audioChannel0State AND the scene is not mode 2. Otherwise it parses
 * the sequence for @c (s8)D_800D2442 via @c getScrollState (which fills bank selector,
 * a "ready" flag, and three handles), wires it up with @c func_80039678, records the
 * bank selector, and then either — for an active mode-2 scene — drains the audio
 * engine and pushes the new sequence (@c sndProcessAudio + @c func_8009CDC4), or stops
 * the old one and caches its handles in @c D_800C4FC0 / @c D_800C4FBC. The new channel
 * state is latched via @c func_8009D7D8.
 *
 * Afterwards it always drains the sound engine, uploads the sample bank selected by the
 * scene command @c D_80082C8D (@c D_800C9EE0 for @c 0x40..0x42, else @c D_800C9EDC),
 * rebinds @c D_800C4FD8 sequence slots 4-9/12 to self-relative pointers in the bank
 * header @c D_800C97A8, and clears every slot's runtime state (@c field12).
 */
void func_80099C84(void) {
    u8  b18;
    s32 w1C, w20, w24, w28;
    s32 result;
    s32 mode2;
    s32 key;
    s32 i;
    s32 *bank;
    s32 *offs;

    D_800C4FBC = 0;
    D_800C4FC0 = 0;
    mode2 = (D_80082C8C.mode == 2);
    key = D_800D2442;
    if (key != g_fieldVars->audioChannel0State || mode2) {
        result = getScrollState((s8)key, &b18, &w1C, &w20, &w24, &w28);
        if (result != 0) {
            func_80039678(w24, result, w28);
            g_fieldVars->soundBankSelector = b18;
            if (mode2 && w1C != 0) {
                while (sndGetEngineState() > 0) {}
                sndProcessAudio(w20, 1);
                func_8009CDC4(w24, 0x7F);
            } else {
                if (sndGetStatus() != 0) {
                    sndCmdC1(0, 0x1E, 0);
                }
                D_800C4FC0 = w20;
                D_800C4FBC = w24;
            }
            g_fieldVars->audioChannel0State = func_8009D7D8(result);
        } else {
            func_8009CE40();
        }
    } else {
        sndCmdC1(0, 0x1E, 0x7F);
    }
    while (sndGetEngineState() > 0) {}
    if ((u32)(D_80082C8D - 0x40) < 3) {
        while (sndUploadSamples(D_800C9EE0, 1) != 0) {}
    } else {
        while (sndUploadSamples(D_800C9EDC, 1) != 0) {}
    }
    bank = D_800C97A8;
    offs = bank + 1;
    D_800C4FD8[4].bankHandle = D_800C97AC;
    D_800C4FD8[5].bankHandle = (s32)bank + offs[1];
    D_800C4FD8[6].bankHandle = (s32)bank + offs[2];
    D_800C4FD8[7].bankHandle = (s32)bank + offs[3];
    D_800C4FD8[8].bankHandle = (s32)bank + offs[4];
    D_800C4FD8[9].bankHandle = (s32)bank + offs[5];
    D_800C4FD8[12].bankHandle = (s32)bank + offs[6];
    for (i = 0x1F; i >= 0; i--) {
        D_800C4FD8[i].field12 = 0;
    }
}


/**
 * @brief Kick off the SFX for @c D_800C4FD8[idx] and mark it active.
 *
 * Dispatches on the entry's @c bankHandle:
 *  - non-zero: @c sndPlayBankSfx(bankHandle, @c field04, @c field08,
 *    @c field0C).
 *  - zero:     @c sndPlaySfx(field10, @c field04, @c field08,
 *    @c field0C).
 *
 * Then sets @c field12 = @c 1 to mark the entry as running.
 *
 * @param idx Index into @c D_800C4FD8.
 */
void func_80099EDC(s32 idx) {
    if (D_800C4FD8[idx].bankHandle != 0) {
        sndPlayBankSfx(D_800C4FD8[idx].bankHandle,
                       D_800C4FD8[idx].field04,
                       D_800C4FD8[idx].field08,
                       D_800C4FD8[idx].field0C);
    } else {
        sndPlaySfx(D_800C4FD8[idx].field10,
                   D_800C4FD8[idx].field04,
                   D_800C4FD8[idx].field08,
                   D_800C4FD8[idx].field0C);
    }
    D_800C4FD8[idx].field12 = 1;
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object1", func_80099F78);


/**
 * @brief Build a screen-covering dim overlay: @p count semi-transparent dark
 *        quads plus a draw-mode tpage, linked into the @c primList[0] chain.
 *
 * Picks the quad buffer / DR_TPAGE pair by whether the active scene context
 * @c D_800D244C is the worldmap sentinel @c D_800CA040 (@c D_800C89A8 +
 * @c &D_800C8CA8[1]) or not (@c D_800C86A8 + @c D_800C8CA8). Each @ref POLY_F4
 * is a semi-transparent (code @c 0x2A) flat quad spanning @c (0,0) ..
 * @c (D_800C97EA, D_800C97E8) in dark grey @c (8,8,8); the trailing DR_TPAGE
 * carries GP0 draw-mode @c 0xE1000255.
 *
 * @param count Number of overlay quads to emit.
 */
void func_8009A4DC(s32 count) {
    POLY_F4 *quad;
    DR_TPAGE *tp;
    /* Kept live across the loop (assigned here, consumed by the final addPrim)
       so %hi(D_800D244C) is re-materialised each iteration rather than hoisted
       into a spare register — required to match the original register alloc. */
    u32 new_var;
    s32 i;

    quad = (D_800D244C == &D_800CA040) ? D_800C89A8 : D_800C86A8;
    tp   = (D_800D244C == &D_800CA040) ? &D_800C8CA8[1] : D_800C8CA8;
    for (i = 0; i < count; i++) {
        new_var = 0;
        setlen(quad, 5);
        setcode(quad, 0x2A);
        setXYWH(quad, 0, 0, D_800C97EA, D_800C97E8);
        setRGB0(quad, 8, 8, 8);
        addPrim(&D_800D244C->primList[0], quad);
        quad++;
    }
    setlen(tp, 1);
    tp->code[0] = 0xE1000255;
    new_var = getaddr(&D_800D244C->primList[0]);
    setaddr(tp, new_var);
    setaddr(&D_800D244C->primList[0], tp);
}

/* --- World-map engine control block (reset by func_8009A638) --- */


/**
 * @brief Reset the world-map engine control block to its initial state.
 *
 * Clears the large @c D_800C4D04 .. @c D_800C4DB4 control block plus a few related
 * globals, seeding the non-zero defaults: @c D_800C4D44 = @c 0x10,
 * @c D_800C4D30 = @c 0x1460, @c D_800C4D20 = @c 1, @c D_800C4D60 = @c 0xFFFF, the
 * section pointer @c D_800C4D5C = @c 0x80122000, and the "inactive" markers
 * @c D_800C4D78 / @c D_800C4D7C / @c D_800C4D88 / @c D_800C4D90 / @c D_800C4D94 = @c -1.
 * It then fills the 4-byte @c D_800C5398 flag block with @c 0xFF and clears the
 * light-matrix work buffer @c D_800C9758 via @c func_8009FEDC.
 */
void func_8009A638(void) {
    s32 i;

    D_800C4D34 = 0;
    D_800C4D3C = 0;
    D_800C4D38 = 0;
    D_800C4D40 = 0;
    D_800C4D44 = 0x10;
    D_800C4D48 = 0;
    D_800C4D4C = 0;
    D_800C4D50 = 0;
    D_800C4D54 = 0;
    D_800C4D58 = 0;
    D_800C4D5C = (WorldSection *)0x80122000;
    D_800C4D60 = 0xFFFF;
    D_800C4D64 = NULL;
    D_800C4D74 = NULL;
    D_800C4D78 = -1;
    D_800C4D7C = -1;
    D_800C4D04 = 0;
    D_800C4D18 = 0;
    D_800C4D1C = 0;
    D_800C4D20 = 1;
    D_800C4D24 = 0;
    D_800C4D2C = 0;
    D_800C4D30 = 0x1460;
    D_800D2458 = 0;
    D_800C9714 = 0;
    D_800C4D84 = 0;
    D_800C4D88 = -1;
    D_800C4D68 = NULL;
    D_800C4D90 = -1;
    D_800C4D94 = -1;
    D_800D212C = 0;
    D_800C97A0 = 0;
    D_800C4DA8 = 0;
    D_800C4DAC = 0;
    D_800C4DB0 = 0;
    D_800C4DB4 = 0;
    for (i = 0; i < 4; i++) {
        D_800C5398[i] = 0xFF;
    }
    func_8009FEDC(D_800C9758, 0);
}



/**
 * @brief Scatter the world-map configuration table @c D_800C4CA4 into the
 *        active working globals.
 *
 * Walks @c D_800C4CA4 with a moving pointer and copies its words out to a set
 * of scattered scalar globals; several values are mirrored into the
 * @c D_800C4F2C / @c D_800C4F4C slot tables as well. The table's leading
 * entries are 8 bytes (only the first word is consumed); the remainder are
 * packed 4-byte words — hence the @c +2 then @c +1 pointer steps.
 *
 * @note Field meanings are not individually known; this is a bulk config load.
 */
void func_8009A7C0(void) {
    s32 *p = D_800C4CA4;
    p += 2;

    D_800C97E0 = D_800C4CA4[0];
    D_800C97E4 = *p; p += 2;
    D_800C9E58 = *p; p += 2;
    D_800C4F14 = D_800C9E80 = *p; p += 2;
    D_800C4F2C[2] = D_800C9FE0 = *p; p += 1;
    D_800C4F2C[3] = D_800C97DC = *p; p += 1;
    D_800C9718 = *p; p += 1;
    D_800C9710 = *p; p += 1;
    D_800C4F4C[2] = D_800C9FC8 = *p; p += 1;
    D_800C4F4C[3] = D_800C9FB0 = *p; p += 1;
    D_800C4F4C[6] = D_800C9FCC = *p; p += 1;
    D_800C4F4C[7] = D_800C9FB4 = *p; p += 1;
    D_800C4F4C[10] = D_800C9FD0 = *p; p += 1;
    D_800C4F4C[11] = D_800C9FB8 = *p; p += 1;
    D_800C4F4C[14] = D_800C9FD8 = *p; p += 1;
    D_800C4F4C[15] = D_800C9FBC = *p; p += 1;
    D_800C4F4C[18] = D_800C9FDC = *p; p += 1;
    D_800C4F4C[19] = D_800C9FC0 = *p; p += 1;
    D_800C4F4C[22] = D_800C9FE4 = *p;
    D_800C4F4C[23] = D_800C9FC4 = p[1];
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object1", func_8009A954);




/**
 * @brief Install the world-map display/draw environment and (optionally) re-init the screen.
 *
 * Selects a load RECT from the active scene context @c D_800D244C — at @c +0x5C when it is
 * not the @c D_800CA040 sentinel, else at @c +0x40CC — and, if its width field is non-zero,
 * loads its image (@c func_80048FBC) after a @c func_80042634 prep. It then copies the
 * sentinel's DRAWENV and DISPENV templates into the active @c D_80082C30 / @c D_80082C18 and
 * publishes them via @c g_activeDrawEnv / @c D_8005F138. Finally, when the scene mode
 * @c D_80082C8C is 5, it re-initialises both screen buffers (16 passes of
 * @c func_800A5D10 bracketed by @c func_800A5FD4) and flushes via @c func_80048BB8.
 */
void func_8009AD3C(void) {
    RECT *r;
    s32 i;

    /* Pointer difference (0 only when D_800D244C is the sentinel itself). */
    if (D_800D244C - &D_800CA040 != 0) {
        r = (RECT *)((u8 *)&D_800CA040 + 0x5C);
    } else {
        r = (RECT *)((u8 *)&D_800CA040 + 0x40CC);
    }
    func_80042634(0);
    if (r->x != 0) {
        func_80048FBC(r, 0, 0);
        func_80048C50(0);
    }
    D_80082C30 = D_800CA040.drawEnv;
    D_80082C18 = ((CtxDispView *)&D_800CA040)->disp;
    g_activeDrawEnv = &D_80082C30;
    D_8005F138 = (s32)&D_80082C18;
    if (D_80082C8C.mode == 5) {
        func_800A5F78(0);
        func_800A5FD4(0);
        for (i = 0; i < 0x10; i++) {
            func_80042634(0);
            func_800A5D10();
            func_800A5FD4(0);
        }
        func_80048BB8(0);
    }
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object1", func_8009AEE4);

/**
 * @brief Set up a positioned world-map SFX/voice clip, skipping if already current.
 *
 * Sibling of @c func_8009B550. Resolves the clip's data pointer (explicit @p text,
 * else a self-relative @c D_800C97D4 string-table lookup) but first early-outs when
 * the slot already records this @p strIdx and no explicit @p text was given. Primes
 * the voice via @c initSfxPlayback, then sets pitch (from the current field music),
 * entity type 6 and reverb (@c field03). Finally it sizes a text box —
 * @c func_8002E680 returns the rendered dimensions packed as @c width|(height<<16) —
 * positioned relative to the slot anchor (@c field04, @c field06) per the alignment
 * mode @c field01, submits it via @c func_8002E064, and starts the SFX.
 *
 * @note Declared non-void to match the original's codegen: a non-void return keeps
 *       @c v0 live, which leaves the early-return branch-delay slot as a @c nop.
 *       No meaningful value is returned.
 *
 * @param slotIdx Index into the @c D_800C526C SFX-slot table.
 * @param strIdx  String-table index (@c -2 = none); also the "already current" key.
 * @param text    Explicit clip pointer; overrides @p strIdx when non-NULL.
 */
s32 func_8009B358(s32 slotIdx, s32 strIdx, u8 *text) {
    RECT rect;
    s32 id;
    u8 *ptr;
    s32 dim;
    StringTable *tbl;
    s32 *offsets;
    s32 off;
    s32 hi;

    ptr = text;
    tbl = D_800C97D4;
    offsets = tbl->first;
    off = offsets[strIdx];
    if (text == 0 && strIdx == D_800C526C[slotIdx].field00) {
        return;
    }
    id = D_800C526C[slotIdx].field02;
    if (text == 0 && strIdx != -2) {
        ptr = (u8 *)tbl + off;
        D_800C526C[slotIdx].field00 = strIdx;
    } else {
        D_800C526C[slotIdx].field00 = -2;
    }

    initSfxPlayback(id, ptr);
    setSfxPitch(id, getCurrentFieldMusic());
    setSfxEntityType(id, 6);
    setSfxReverbMode(id, D_800C526C[slotIdx].field03);

    dim = func_8002E680(ptr);
    hi = (u32)dim >> 16;
    if (D_800C526C[slotIdx].field01 == 1) {
        setRECT(&rect, D_800C526C[slotIdx].field04 - dim - 0x10, D_800C526C[slotIdx].field06, dim + 0x10, hi + 0x10);
    } else if (D_800C526C[slotIdx].field01 == 0) {
        setRECT(&rect, D_800C526C[slotIdx].field04, D_800C526C[slotIdx].field06, dim + 0x10, hi + 0x10);
    } else if (D_800C526C[slotIdx].field01 == 2) {
        setRECT(&rect, D_800C526C[slotIdx].field04 - ((s16)dim >> 1) - 8, D_800C526C[slotIdx].field06, dim + 0x10, hi + 0x10);
    } else if (D_800C526C[slotIdx].field01 == 3) {
        setRECT(&rect, D_800C526C[slotIdx].field04 - dim - 0x10, D_800C526C[slotIdx].field06 - hi - 0x10, dim + 0x10, hi + 0x10);
    }
    func_8002E064(id, &rect);
    startSfxSlow(id);
}


/**
 * @brief Configure and start a positioned world-map SFX/voice clip on a slot.
 *
 * Resolves the clip's data pointer: the caller-supplied @p text when non-NULL,
 * otherwise — unless @p strIdx is the @c -2 sentinel — a self-relative lookup
 * @c (u8 *)D_800C97D4 + D_800C97D4->first[strIdx] into the string table. The
 * slot's @c field00 records @p strIdx (or @c -2). After priming the voice via
 * @c func_8002D784, it sets pitch from the current field music, entity type 6,
 * reverb from @c field03, and the global flag.
 *
 * It then sizes a text box: @c func_8002E680 returns the rendered dimensions
 * packed as @c width|(height<<16); the @ref RECT is placed relative to the
 * slot's anchor (@c field04, @c field06) per the alignment mode @c field01
 * (0 = top-left, 1/3 = right-aligned, 2 = centered, 3 = also bottom-aligned),
 * submitted via @c func_8002E064, and the SFX is started with @c startSfxSlow.
 *
 * @param slotIdx Index into the @c D_800C526C SFX-slot table.
 * @param strIdx  String-table index for the clip text (@c -2 = none).
 * @param text    Explicit text/data pointer; overrides @p strIdx when non-NULL.
 * @param arg3..arg6 Forwarded to @c func_8002D784.
 */
void func_8009B550(s32 slotIdx, s32 strIdx, u8 *text, s32 arg3, s32 arg4, s32 arg5, s32 arg6) {
    RECT rect;
    s32 id;
    u8 *ptr;
    s32 dim;
    StringTable *tbl;
    s32 *offsets;
    s32 off;
    s32 v;
    s32 hi;

    ptr = text;
    tbl = D_800C97D4;
    offsets = tbl->first;
    off = offsets[strIdx];
    id = D_800C526C[slotIdx].field02;
    if (text == 0 && strIdx != -2) {
        ptr = (u8 *)tbl + off;
        D_800C526C[slotIdx].field00 = strIdx;
    } else {
        D_800C526C[slotIdx].field00 = -2;
    }

    func_8002D784(id, ptr, arg3, arg4, arg5, arg6);
    setSfxPitch(id, getCurrentFieldMusic());
    setSfxEntityType(id, 6);
    setSfxReverbMode(id, D_800C526C[slotIdx].field03);
    setSfxGlobalFlag(id);

    dim = func_8002E680(ptr);
    v = dim + 0x20;
    hi = (u32)dim >> 16;
    if (D_800C526C[slotIdx].field01 == 1) {
        setRECT(&rect, D_800C526C[slotIdx].field04 - v - 0x10, D_800C526C[slotIdx].field06, dim + 0x30, hi + 0x10);
    } else if (D_800C526C[slotIdx].field01 == 0) {
        setRECT(&rect, D_800C526C[slotIdx].field04, D_800C526C[slotIdx].field06, dim + 0x30, hi + 0x10);
    } else if (D_800C526C[slotIdx].field01 == 2) {
        setRECT(&rect, D_800C526C[slotIdx].field04 - ((s16)v >> 1) - 8, D_800C526C[slotIdx].field06, dim + 0x30, hi + 0x10);
    } else if (D_800C526C[slotIdx].field01 == 3) {
        setRECT(&rect, D_800C526C[slotIdx].field04 - v - 0x10, D_800C526C[slotIdx].field06 - hi - 0x10, dim + 0x30, hi + 0x10);
    }
    func_8002E064(id, &rect);
    startSfxSlow(id);
}


/**
 * @brief One-shot: silence the high SFX slots when a trigger fires.
 *
 * Gated by two early-outs — the global disable byte @c D_800D23D8[0] and the
 * one-shot latch @c D_800C4DBC (so the body runs at most once until the latch
 * is cleared elsewhere).
 *
 * Reads the double-buffered input state @c D_800D2278: @c D_800C4D04 selects the
 * active buffer and @c (idx+1)%2 is the previous frame's buffer. If input bit
 * @c 0x40 is newly pressed this frame — @c cur & ~prev & 0x40, written as
 * @c cur & (cur ^ prev) & 0x40 — or the external trigger @c D_800C4D50 is set,
 * it fades out @c D_800C526C SFX slots 9..12 (@c fadeOutSfxSlow on each active
 * slot's @c field02, then marks @c field00 inactive) and latches @c D_800C4DBC.
 *
 * @note The exact meaning of input bit @c 0x40 is uncertain.
 */
void func_8009B748(void) {
    u32 *buf;
    s32 idx;
    u32 cur;
    u32 prev;
    s32 i;

    if (D_800D23D8[0] != 0) {
        return;
    }
    if (D_800C4DBC != 0) {
        return;
    }
    buf = D_800D2278;
    idx = D_800C4D04;
    cur = buf[idx];
    prev = buf[(idx + 1) % 2];
    if (((cur & (cur ^ prev) & 0x40) != 0) || (D_800C4D50 != 0)) {
        for (i = 9; i < 13; i++) {
            s32 field00 = D_800C526C[i].field00;
            s32 field02 = D_800C526C[i].field02;
            if (field00 != -1) {
                D_800C526C[i].field00 = -1;
                fadeOutSfxSlow(field02);
            }
        }
        D_800C4DBC = 1;
    }
}


/**
 * @brief Sync the world-map voice/SFX on channel 1 to the requested clip.
 *
 * Drives a one-voice state machine from the request slot @c D_800C4D90 and the
 * currently-playing tracker @c D_800C4D94 (both @c -1 when idle):
 *  - Request pending (@c D_800C4D90 @c >= @c 0) and nothing playing
 *    (@c D_800C4D94 @c < @c 0): resolve the clip record from the @c D_800C97D4
 *    string-table blob (@c table @c + @c table->first[req]), start playback
 *    (@c initSfxPlayback), set its pitch from the current field music and its
 *    entity type to 6, build a horizontally-centered display @c RECT from the
 *    clip's packed dimensions (@c func_8002E680 returns @c width|height<<16) and
 *    submit it (@c func_8002E064), then @c startSfxSlow and latch
 *    @c D_800C4D94 @c = @c D_800C4D90.
 *  - No request (@c D_800C4D90 @c < @c 0) but a clip is playing
 *    (@c D_800C4D94 @c >= @c 0): @c fadeOutSfxSlow and clear the tracker to -1.
 *
 * Centering uses screen half-width @c D_800C97EA:
 * @c x @c = @c D_800C97EA/2 @c - @c width/2, with a 16px margin on the others.
 */
void func_8009B840(void) {
    if (D_800C4D90 >= 0) {
        if (D_800C4D94 < 0) {
            StringTable *table = D_800C97D4;
            s32 *offsets = table->first;
            u8 *record = (u8 *)table + offsets[D_800C4D90];
            RECT rect;
            u32 dims;

            initSfxPlayback(1, record);
            setSfxPitch(1, getCurrentFieldMusic());
            setSfxEntityType(1, 6);
            dims = func_8002E680(record);
            rect.x = (D_800C97EA >> 1) - ((dims & 0xFFFF) >> 1);
            rect.y = 0x10;
            rect.w = dims + 0x10;
            rect.h = (dims >> 16) + 0x10;
            func_8002E064(1, &rect);
            startSfxSlow(1);
            D_800C4D94 = D_800C4D90;
        }
    } else {
        if (D_800C4D94 >= 0) {
            fadeOutSfxSlow(1);
            D_800C4D94 = -1;
        }
    }
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object1", func_8009B954);

/* Cached world-map scroll position, stored at HALF resolution (x, y). */

/**
 * @brief Snap the cached scroll position to the nearest candidate vector.
 *
 * Scans @p coords (an array of @p count 2D vectors) for the first whose (x, y)
 * lies within ±8 of the cached scroll position. That position
 * (@c D_800C4DA0 = x, @c D_800C4DA4 = y) is stored at half resolution, so it is
 * doubled for the comparison and the matched vector is halved on store. If no
 * candidate is close enough, the cache is left unchanged.
 *
 * @param coords Candidate 2D positions.
 * @param count  Number of candidates.
 */
void func_8009BFA0(DVECTOR *coords, s16 count) {
    DVECTOR *v = coords;
    s32 i;

    for (i = 0; i < count; i++, v++) {
        /* accept when |vx - x*2| < 8, tested per sign so it compiles to the
           original's two-branch (blez/slti) form rather than an abs() */
        s32 dx = v->vx - D_800C4DA0 * 2;
        if (dx > 0 ? (dx < 8) : (D_800C4DA0 * 2 - v->vx < 8)) {
            s32 dy = v->vy - D_800C4DA4 * 2;
            if (dy > 0 ? (dy < 8) : (D_800C4DA4 * 2 - v->vy < 8)) {
                D_800C4DA0 = (s16)v->vx >> 1;
                D_800C4DA4 = (s16)v->vy >> 1;
                return;
            }
        }
    }
    /* no-op that keeps coords live to the end, so the loop walker is held in a
       saved register — matches the original's register allocation */
    coords++;
    coords--;
}


/* func_800ACD38 is void func_800ACD38(s32) in we_object6; this call site sees an
   int-returning function (K&R-era view), which the matching codegen requires. */

/**
 * @brief Compose the world-map view transform and the resulting camera position.
 *
 * Transforms the constant view offset @c D_800980DC by a rotation matrix into a
 * local vector. The matrix is normally the global @c D_800C9838, but a per-frame
 * matrix from @c func_800ACD38 is used instead when a special mode is active:
 * @c D_800C4D38 == 0x32, @c D_800C4D3C == 0, @c D_800D23D8[0] == 0, and the angle
 * from @c getAngleDelta(D_800C977A, D_800D239A) is within @c ±0x241.
 *
 * The transformed offset's X and Z are added to the base position @c D_800C9868
 * to form @c D_800D23C0 (Z cleared), then the rotation and translation matrices
 * @c D_800C9838 are loaded into the GTE.
 */
void func_8009C070(void) {
    VECTOR localA;
    VECTOR localB;
    MATRIX localMtx;
    s16 angle;
    s32 v;

    localA = D_800980DC;
    angle = getAngleDelta(D_800C977A, D_800D239A);
    /* Special mode when the three flags hold and the angle's absolute value (in
       @c v) is within ±0x241 of zero — build a per-frame matrix; else use the
       global one. @c v is assigned in the condition so the lazy @c abs() compares
       a plain variable (a narrow operand would split the compare in two). */
    if (D_800C4D38 == 0x32 && D_800C4D3C == 0 && D_800D23D8[0] == 0 &&
        (v = angle, abs(v) < 0x241)) {
        func_800ACD38(&localMtx);
        func_8003FD84(&localMtx, &localA, &localB);
    } else {
        func_8003FD84(&D_800C9838, &localA, &localB);
    }
    D_800D23C0.z = 0;
    D_800D23C0.x = D_800C9868.x + localB.vx;
    D_800D23C0.y = D_800C9868.y + localB.vz;
    SetRotMatrix(&D_800C9838);
    SetTransMatrix(&D_800C9838);
}




/**
 * @brief Set up the world-to-screen transform and refresh the active command
 *        descriptors.
 *
 * Transforms @c D_800C9868 with @c func_800BC544 into a local vector, mirrors
 * it into @c D_800C9748, then projects it with @c func_800A40F8 (whose return
 * angle is stashed for @c D_800D212C) and resolves a command descriptor with
 * @c func_800A3870. That descriptor is published to @c D_800C4D64 / @c D_800C4D74
 * (and to @c D_800C4D6C / @c D_800C4D68 when @c D_800C4D38 holds 0x31). Per-frame
 * scratch (@c D_800C4D48, @c D_800C9E38[0..2]) is cleared, and the rotation and
 * translation matrices @c D_800C9838 are loaded into the GTE.
 */
void func_8009C1A4(void) {
    VECTOR local;
    ProjBuf buf;
    CmdDesc *desc;

    func_800BC544((VECTOR *)&D_800C9868, &local);
    D_800C9748 = local;
    buf.angle = func_800A40F8(&local, &buf.proj);
    desc = func_800A3870(&local, &D_800C97F4);
    D_800C4D64 = desc;
    D_800C4D74 = desc;
    if (D_800C4D38 == 0x31) {
        D_800C4D6C = desc;
        D_800C4D68 = desc;
    }
    D_800C4D48 = 0;
    D_800C9E38[0] = 0;
    D_800C9E38[1] = 0;
    D_800C9E38[2] = 0;
    D_800D212C = buf.angle;
    SetRotMatrix(&D_800C9838);
    SetTransMatrix(&D_800C9838);
}



/**
 * @brief Tear down the current world-map scene and hand off for the given command.
 *
 * For the relevant dispatch modes (@c D_800C4D38 of 0x30 / 0x32) and commands
 * @c cmd of 0x2C / 0x41 / 0x11, runs two @c func_800A2D50 visibility checks and
 * toggles display bit @c 0x20 of @c D_800780D8[0x108] / @c D_800D23D8[0x66]
 * (cleared when both pass, set otherwise). Then @c func_8009D630 tears down prior
 * state; if a command descriptor @c D_800C4D64 is active, its flag bit is mirrored
 * into @c D_800D226C->unk6C (bit @c 0x100). Finally it stamps the scene state
 * @c D_80082C8C (mode 1, dispatch code, markers), calls @c func_800B3FD4, clears
 * the slot's action byte, fades out all @c D_800C526C SFX, pushes 2 display frames,
 * and submits the scene color tag.
 *
 * @param cmd Dispatch command code being handed off.
 */
void func_8009C294(s32 cmd) {
    s32 i;

    if ((D_800C4D38 == 0x32 || D_800C4D38 == 0x30) &&
        ((cmd & 0xFFFF) == 0x2C || (cmd & 0xFFFF) == 0x41 || (cmd & 0xFFFF) == 0x11)) {
        if (func_800A2D50(0, (s16)func_800BEC1C(D_800C4D38), D_800C9778, 0, 0, 0) &&
            func_800A2D50(D_800C4D38, 0, D_800C9778, 0, 0, 0)) {
            D_800780D8[0x108] &= ~0x20;
            D_800D23D8[0x66] &= ~0x20;
        } else {
            D_800780D8[0x108] |= 0x20;
            D_800D23D8[0x66] |= 0x20;
        }
    }
    func_8009D630();
    if (D_800C4D64 != 0) {
        D_800D226C->unk6C = (D_800D226C->unk6C & ~0x100) | ((D_800C4D64->flag << 5) & 0x100);
    }
    D_80082C8C.mode = 1;
    D_80082C8C.unk02 = cmd;
    D_80082C8C.unk03 = -1;
    D_80082C8C.cmd = D_800C4D38;
    func_800B3FD4(D_800D226C, 1);
    D_800D226C->bytes[1] = 0;
    for (i = 0; i < 13; i++) {
        s32 sfxIdx = D_800C526C[i].field02;
        D_800C526C[i].field00 = -1;
        fadeOutSfxFast(sfxIdx);
    }
    renderAndUpdateDisplay(2);
    renderBattleDisplayList(&D_800D244C->primList[BSC_COLORTAG_IDX]);
}


/**
 * @brief Variant of @c func_8009C5FC that overrides the first
 *        @c RECT's @c (x, y) with caller-supplied coordinates.
 *
 * Same two-stage @c LoadImage flow as @c func_8009C5FC, but the first
 * blit's top-left corner is replaced by @p x, @p y (the @c w / @c h
 * still come from the descriptor). The second blit (when bit 3 of
 * @c desc.flag is set) uses @c desc.rect2 verbatim.
 *
 * Streams to the scratch @c D_800C8698 (separate from
 * @c func_8009C5FC's @c D_800C8640).
 *
 * @param src Source buffer fed to @c func_8009CA34.
 * @param x   Override X coordinate for the first blit.
 * @param y   Override Y coordinate for the first blit.
 */
void func_8009C478(s32 *src, s32 x, s32 y) {
    ImageDesc desc;
    func_8009CA34(src, &desc);
    D_800C8698.x = x;
    D_800C8698.y = y;
    D_800C8698.w = desc.rect1.w;
    D_800C8698.h = desc.rect1.h;
    LoadImage(&D_800C8698, desc.data1);
    if ((desc.flag >> 3) & 1) {
        D_800C8698.x = desc.rect2.x;
        D_800C8698.y = desc.rect2.y;
        D_800C8698.w = desc.rect2.w;
        D_800C8698.h = desc.rect2.h;
        LoadImage(&D_800C8698, desc.data2);
    }
}

/**
 * @brief Fatal-error thunk that invokes the PSX SDK SystemError with category 0x57.
 *
 * Forwards @p rc to SystemError with a fixed category code. Callers pass a
 * line-number-like token so the fault site can be identified post-mortem.
 *
 * @note Category values in use elsewhere: field → 0x53, battle
 *       → 0x59, world → 0x57. They happen to be ASCII ('S','Y','W')
 *       but the mapping to subsystem isn't obvious — purpose uncertain.
 *
 * @param rc Diagnostic code (typically a unique line/site identifier).
 */
void func_8009C528(s32 rc) {
    SystemError(0x57, rc);
}


/**
 * @brief Rewrite an ISO9660 disc path into a dev-filesystem path.
 *
 * Copies @p src to @p dst with two transforms:
 *  - if the path starts with @c '\\' it is prefixed with @c D_800980CC
 *    (@c "x:\USPC\WORLD"): 14 bytes are copied (13 chars + NUL) and the write
 *    cursor advances 13, so the NUL is overwritten by the copied path;
 *  - a @c ';' (the start of an ISO @c ;1 version suffix) skips two source
 *    characters, dropping the suffix.
 *
 * The copy runs until (and including) the source NUL terminator. Example:
 * @c "\DAT\WMX.OBJ;1" becomes @c "x:\USPC\WORLD\DAT\WMX.OBJ".
 *
 * @param dst Destination buffer for the rewritten path.
 * @param src Source ISO9660 path string.
 */
void func_8009C54C(u8 *dst, u8 *src) {
    u8 ch;
    s32 i;
    s32 j;

    i = 0;
    j = 0;
    while (1) {
        if ((i == 0) && (src[i] == '\\')) { /* 0x5C == '\' */
            memcpy(&dst[j], D_800980CC, 14);
            j += 13;
        }
        if (src[i] == ';') { /* 0x3B == ';' */
            i += 2;
        }
        ch = src[i];
        dst[j++] = ch;
        if (ch == '\0') {
            break;
        }
        i++;
    }
}

/**
 * @brief Pull a two-image descriptor from @p src and stream both TIMs
 *        through @c LoadImage, gated by bit 3 of the descriptor flag.
 *
 * Steps:
 *  1. @c func_8009CA34(@p src, @c &desc) — parse a streaming-image
 *     header into @c desc.
 *  2. Copy @c desc.rect1 into the scratch @c D_800C8640 and
 *     @c LoadImage(@c &D_800C8640, @c desc.data1).
 *  3. If bit 3 of @c desc.flag is set, copy @c desc.rect2 and
 *     @c LoadImage the second payload too.
 *
 * The scratch @c D_800C8640 is re-used between both loads — the GPU
 * blit is synchronous from this routine's POV, so the RECT can be
 * overwritten right after the call returns.
 *
 * @param src Source buffer fed to @c func_8009CA34.
 */
void func_8009C5FC(s32 *src) {
    ImageDesc desc;
    func_8009CA34(src, &desc);
    D_800C8640.x = desc.rect1.x;
    D_800C8640.y = desc.rect1.y;
    D_800C8640.w = desc.rect1.w;
    D_800C8640.h = desc.rect1.h;
    LoadImage(&D_800C8640, desc.data1);
    if ((desc.flag >> 3) & 1) {
        D_800C8640.x = desc.rect2.x;
        D_800C8640.y = desc.rect2.y;
        D_800C8640.w = desc.rect2.w;
        D_800C8640.h = desc.rect2.h;
        LoadImage(&D_800C8640, desc.data2);
    }
}


/**
 * @brief Silence the audio subsystem: zero tempo, zero master volume, send cmd F1.
 *
 * Likely used when leaving a map or suspending sound playback. The @c sndCmdF1
 * call appears in similar "stop audio" sequences elsewhere in the codebase.
 */
void func_8009C69C(void) {
    sndSeqSetTempoAlt(0);
    sndSetMasterVolume(0);
    sndCmdF1();
}


/**
 * @brief Set a sound channel's volume, narrowing the volume arg to signed 8 bits.
 *
 * Forwards to sndSetChannelVolume with @p vol sign-extended from s8 to s32
 * (the canonical `sll 24 / sra 24` pair). Useful when the caller only has
 * an s8 volume value that needs to be promoted into the s32 SDK signature.
 *
 * @param channel Sound channel index.
 * @param vol Volume level treated as signed 8-bit.
 */
void func_8009C6CC(s32 channel, s8 vol) {
    sndSetChannelVolume(channel, vol);
}


/**
 * @brief Start a sound sequence with pan for entry @p idx of the table.
 *
 * Looks up the sequence-table entry at @c D_800C4FD8[idx] and dispatches
 * @c sndSeqStartPan with its @c field10 and @c field04, plus the caller's
 * @p a1 and @p pan (sign-extended from s8).
 *
 * @param idx Index into the sequence table.
 * @param a1 Sound parameter forwarded as the third arg.
 * @param pan Signed 8-bit pan value (sign-extended to s32 for the ABI).
 */
void func_8009C6F0(s32 idx, s32 a1, s8 pan) {
    sndSeqStartPan(D_800C4FD8[idx].field10, D_800C4FD8[idx].field04, a1, pan);
}

/**
 * @brief Stop the sound sequence associated with table entry @p idx.
 *
 * Dispatches @c sndCmd21(entry->field10, entry->field04) then clears the
 * entry's runtime-state halfword at +0x12.
 *
 * @param idx Index into the @c D_800C4FD8 sequence table.
 */
void func_8009C738(s32 idx) {
    sndCmd21(D_800C4FD8[idx].field10, D_800C4FD8[idx].field04);
    D_800C4FD8[idx].field12 = 0;
}


/**
 * @brief Start a panned sound sequence with a 7-bit-clamped pan value.
 *
 * Clamps @p pan into [0, 0x7F] (7-bit range) before calling
 * @c sndSeqPlayPan7bit with the sequence-table entry's @c field10 and
 * @c field04, plus the forwarded @p arg.
 *
 * @param idx Index into the @c D_800C4FD8 sequence table.
 * @param arg Forwarded as the third arg of sndSeqPlayPan7bit.
 * @param pan Pan value; clamped to [0, 0x7F].
 */
void func_8009C780(s32 idx, s32 arg, s32 pan) {
    if (pan >= 0x80) pan = 0x7F;
    if (pan < 0) pan = 0;
    sndSeqPlayPan7bit(D_800C4FD8[idx].field10, D_800C4FD8[idx].field04, arg, pan);
}

/** Sets bit 0x20 on two related flag bytes. */
void func_8009C7DC(void) {
    D_800780D8[0x108] |= 0x20;
    D_800D23D8[0x66] |= 0x20;
}

/** Clears bit 0x20 on two related flag bytes. */
void func_8009C808(void) {
    D_800780D8[0x108] &= ~0x20;
    D_800D23D8[0x66] &= ~0x20;
}

/**
 * @brief Fade out SFX slot tracked by D_800C4D94 (if any) and clear the slot.
 *
 * If D_800C4D94 holds a non-negative value (an active slot handle), calls
 * @c fadeOutSfxSlow(1) to fade channel 1, then resets the tracker to -1
 * (inactive). A no-op when already inactive.
 */
void func_8009C834(void) {
    if (D_800C4D94 >= 0) {
        fadeOutSfxSlow(1);
        D_800C4D94 = -1;
    }
}

/** Advances index and returns difference from table lookup. */
u8 func_8009C870(void) {
    u8 idx;
    D_800C4FD6++;
    idx = D_800C4FD6;
    if (idx == 0) {
        D_800C4FD5 += 0xD;
    }
    return (D_800C4DCC[D_800C4FD6] - D_800C4FD5) & 0xFF;
}

/** Sets two related byte values. */
void func_8009C8CC(s32 val) {
    D_800C4FD6 = val;
    D_800C4FD5 = val;
}


/**
 * @brief Build and queue a textured quad (@c POLY_FT4) for the world map.
 *
 * Picks one of the two @c D_800C8648 prim slots — slot 1 when the active
 * @c D_800D244C scene context is the worldmap sentinel @c D_800CA040, else
 * slot 0 — and fills a @c POLY_FT4 covering @c (x,y) to
 * @c (x + scale*128, y + scale*96) at neutral RGB, mapping the full
 * @c 0..0xFF × 0..0xBF texture region (CLUT chosen from @p frame, fixed
 * tpage). @c func_8004D604 finishes the prim setup from @p frame, then it is
 * linked into the scene's main OT chain (@c primList[BSC_OTHEAD_IDX]).
 *
 * @param x     Quad left edge (screen X).
 * @param y     Quad top edge (screen Y).
 * @param scale Size multiplier (quad is @c scale*128 wide, @c scale*96 tall).
 * @param frame Texture frame index — feeds the CLUT and @c func_8004D604.
 */
void func_8009C8E0(u16 x, s32 y, s32 scale, s32 frame) {
    POLY_FT4 *p;

    p = &D_800C8648[0];
    if (D_800D244C == &D_800CA040) {
        p = &D_800C8648[1];
    }
    setPolyFT4(p);
    func_8004D604(p, frame);
    setRGB0(p, 0x80, 0x80, 0x80);
    /* Four corners, paired by the coordinate they share: x1/x3 = right edge,
       y2/y3 = bottom edge, x0/x2 = left, y0/y1 = top. */
    p->x1 = p->x3 = x + scale * 128;
    p->y2 = p->y3 = y + scale * 96;
    p->clut = getClut(0x340, frame + 0xE9);
    p->tpage = 0xC;
    p->u1 = p->u3 = 0xFF;
    p->v0 = p->v1 = p->u0 = p->u2 = 0;
    p->v2 = p->v3 = 0xBF;
    p->x0 = p->x2 = x;
    p->y0 = p->y1 = y;
    addPrim(&D_800D244C->primList[BSC_OTHEAD_IDX], p);
}

/**
 * @brief Parse a streaming-image header out of @p src into @p desc,
 *        skipping any embedded data payload, and return the leading
 *        @c count word.
 *
 * Wire format of the buffer at @p src (u32 words unless noted):
 *  - @c +0x00 : count / magic word (returned, not interpreted here).
 *  - @c +0x04 : flag byte at @c [0] (only the low nibble is kept).
 *  - if bit 3 of the flag is set, an embedded payload follows:
 *    - @c +0x08 : payload size in bytes (rounded down to a multiple
 *      of 4, then the 12 leading header bytes are subtracted to get
 *      the data-stride to skip).
 *    - @c +0x0C : @c rect2 (x, y, w, h — four u16s).
 *    - @c +0x14 : @c data2 starts here; consumed (size - 12) bytes
 *      worth, after which @p src is advanced past it.
 *  - the next u32 boundary holds the @c rect1 header (x, y, w, h)
 *    followed by the @c data1 pointer.
 *
 * @param src   Source buffer.
 * @param desc  Output @ref ImageDesc — receives @c rect1, @c data1,
 *              optional @c rect2 / @c data2 (when the flag bit is
 *              set), and the @c flag itself.
 * @return      The leading @c count word at @p src[0]. Callers in
 *              this TU ignore it, but the value is real ABI.
 *
 * @note Matching needs the two-step @c size mask
 *       (@c size = (u32)size >> 2; @c size = size << 2;) instead of
 *       the one-line @c (((u32)size) >> 2) << 2 — gcc 2.8.0 otherwise
 *       picks a slightly different register cascade for the
 *       @c rect2.y / @c size temps.
 */
s32 func_8009CA34(s32 *src, ImageDesc *desc) {
    s32 count;
    s32 size;

    count = *src;
    src++;

    desc->flag = *(u8 *)src & 0xF;
    src++;

    if (desc->flag >> 3) {
        size = *src;
        src++;

        desc->rect2.x = ((u16 *)src)[0];
        desc->rect2.y = ((u16 *)src)[1];
        src++;

        size = (u32)size >> 2;
        size = size << 2;
        size -= 12;

        desc->rect2.w = ((u16 *)src)[0];
        desc->rect2.h = ((u16 *)src)[1];
        src++;

        desc->data2 = (u32 *)src;
        src = (s32 *)((u8 *)src + size);
    }

    src++;
    desc->rect1.x = ((u16 *)src)[0];
    desc->rect1.y = ((u16 *)src)[1];
    src++;

    desc->rect1.w = ((u16 *)src)[0];
    desc->rect1.h = ((u16 *)src)[1];
    src++;

    desc->data1 = (u32 *)src;

    return count;
}


/**
 * @brief Walk a NULL-terminated @ref CdLoadEntry list, issuing a @c cdRead
 *        for each entry and spinning on @c func_800393C8 between entries.
 *
 * For each non-terminator entry, kicks off @c cdRead(lba, size, dest, NULL)
 * then busy-waits in a poll loop on @c func_800393C8 until it returns 0
 * (read complete). While polling, optionally calls @p spin_cb each iteration
 * after a @c func_80042634(0) prep step — used by callers to keep the frame
 * advancing / GPU buffer flushing during the wait.
 *
 * Returns immediately if the first entry's marker is 0.
 *
 * @param entries  CD load list (NULL-terminated by @c marker = 0).
 * @param spin_cb  Optional polling callback (may be @c NULL).
 *
 * @note Uses a @c goto top loop to suppress gcc 2.8.0's induction-variable
 *       optimizer — without it, the @c entries pointer is split into a
 *       second @c entries+4 base register for the field cluster
 *       (@c lba/@c size/@c dest), forcing @p spin_cb out to @c s2 and
 *       adding 3 extra instructions. The byte-perfect output requires a
 *       single base register, which structured loops can't produce here.
 */
void func_8009CAE0(CdLoadEntry *entries, void (*spin_cb)(void)) {
    CdLoadEntry *e;
    if (entries->marker == 0) return;
    e = entries;
top:
    cdRead(e->lba, e->size, e->dest, 0);
    while (func_800393C8() != 0) {
        if (spin_cb != 0) {
            func_80042634(0);
            spin_cb();
        }
    }
    e++;
    if (e->marker != 0) goto top;
}

#define CD_SECTOR_SIZE 0x800 /* bytes per CD sector */
#define ID_LIST_END    0xFF  /* terminator id */
#define ID_LIST_MAX    2     /* id slots in the descriptor */
#define ID_OFFSET      2     /* id bytes begin 2 into the descriptor */


/**
 * @brief Load up to two CD sectors named by an object descriptor.
 *
 * Reads the id bytes at @c desc[ID_OFFSET] and @c desc[ID_OFFSET+1] (at most
 * @c ID_LIST_MAX, stopping at the @c ID_LIST_END terminator). For each id it
 * reads one @c CD_SECTOR_SIZE sector at @c section->baseLba @c + @c id into a
 * destination buffer — @p dest0 for the first id, @p dest1 for the rest — and
 * spins on @c func_800393C8 until that read finishes.
 *
 * @param desc    Object descriptor; the id bytes begin at offset @c ID_OFFSET.
 * @param section Provides the base sector the ids are added to.
 * @param dest0   Destination buffer for the first id's sector.
 * @param dest1   Destination buffer for any subsequent id's sector.
 */
void func_8009CB70(u8 *desc, CdSection *section, u8 *dest0, u8 *dest1) {
    u8 *p;
    u8 *first;
    u8 *end;
    u32 id;
    s32 term;

    if (desc[ID_OFFSET] == ID_LIST_END) {
        return;
    }
    /* hold the terminator in a local so it stays in a saved register across
       the cdRead / func_800393C8 calls rather than being reloaded each pass */
    term = ID_LIST_END;
    p = desc;
    end = p + ID_LIST_MAX;
    first = p;
loop:
    /* signed compare matches the original's slt (the ids sit just past the
       descriptor header, so the addresses never cross the sign boundary) */
    if ((s32)p >= (s32)end) {
        goto done;
    }
    id = p[ID_OFFSET];
    cdRead(section->baseLba + id, CD_SECTOR_SIZE, (p != first) ? dest1 : dest0, 0);
    while (func_800393C8()) {
        ;
    }
    p++;
    if (p[ID_OFFSET] != term) {
        goto loop;
    }
done:;
}

void func_8009CC34(void) {
}

/** @brief Advance D_800C4FD4 index, add 0xD to D_800C4FD3 on wraparound, return table diff. */
s32 func_8009CC3C(void) {
    D_800C4FD4++;
    if ((u8)D_800C4FD4 == 0) {
        D_800C4FD3 += 0xD;
    }
    return (u8)(D_800C4DCC[D_800C4FD4] - D_800C4FD3);
}

/** @brief Increment D_800C4FD7 index, return D_800C4DCC table value at new index. */
s32 func_8009CC98(void) {
    D_800C4FD7++;
    return D_800C4DCC[D_800C4FD7];
}

/** Sets two related byte values. */
void func_8009CCC8(s32 val) {
    D_800C4FD4 = val;
    D_800C4FD3 = val;
}

/** Sets a single byte value. */
void func_8009CCDC(s32 val) {
    D_800C4FD7 = val;
}
