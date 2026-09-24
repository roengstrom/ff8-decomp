#include "common.h"
#include "field.h"
#include "gamestate.h"
#include "main.h"
#include "psxsdk/libgte.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libetc.h"
#include "field/fe_object1.h"
#include "field/fe_object1_2.h"
#include "field/fe_object10.h"
#include "thread.h"

/**
 * @brief Initialize the field engine's double-buffered draw/display envs.
 *
 * Sets up two 320x224 draw/display environments at VRAM x=0 and x=512
 * (standard PSX double-buffering), enables dithering, leaves background
 * clearing off, raises the screen window by 8 pixels, and installs the
 * first buffer pair as the live one.
 */
void func_80098314(void) {
    SetDefDrawEnv(&g_drawEnvs[0],   0, 0, 320, 224);
    SetDefDrawEnv(&g_drawEnvs[1], 512, 0, 320, 224);
    SetDefDispEnv(&g_dispEnvs[0], 512, 0, 320, 224);
    SetDefDispEnv(&g_dispEnvs[1],   0, 0, 320, 224);

    g_drawEnvs[0].dtd  = 1;
    g_drawEnvs[1].dtd  = 1;
    g_drawEnvs[0].isbg = 0;
    g_drawEnvs[1].isbg = 0;

    g_dispEnvs[0].screen.y = 8;
    g_dispEnvs[1].screen.y = 8;
    g_dispEnvs[0].screen.h = 224;
    g_dispEnvs[1].screen.h = 224;

    PutDispEnv(&g_dispEnvs[0]);
    PutDrawEnv(&g_drawEnvs[0]);
}

/** @brief Tag written into a field-file header that carries no script section. */
#define FIELD_HEADER_EMPTY 0x2020
/** @brief Size of the fixed field-file header the script region starts after. */
#define FIELD_HEADER_SIZE 0x5F24

/**
 * @brief CD landing area for the field bundle: a small parameter header
 *        followed by the payload.
 *
 * Each slot is addressed as its own absolute constant rather than through a
 * struct or a @c D_800E1004 -style extern. That is not cosmetic: cc1 emits
 * @c lui/lw with the destination as its own @c %hi temp for a constant
 * address, but allocates a separate @c %hi temp for a symbol, and folds a
 * struct down to one base register plus displacements. Only the form below
 * reproduces the original's two independent address materialisations.
 */
#define FIELD_BUNDLE_BUF     0x800E1000
#define FIELD_BUNDLE_TIM     (FIELD_BUNDLE_BUF + 0x4) /**< Halfword staging buffer for @c func_800A2D2C. */
#define FIELD_BUNDLE_SLOT    (FIELD_BUNDLE_BUF + 0x8) /**< VRAM column slot for @c func_800A2D2C. */
#define FIELD_BUNDLE_STRIPS  (FIELD_BUNDLE_BUF + 0xC) /**< VRAM restore strips for @c func_800A0D6C. */

/** @brief High-RAM staging area the script region is copied into. */
#define FIELD_SCRIPT_STAGE 0x801B0000

/**
 * @brief Upper bound of the field command area handed to @c func_800AA8A0.
 *
 * Load mode 3 stops short of the @c 0x801F0000 mesh-render region; every other
 * mode may run up to the menu image base.
 * @note Purpose inferred from the call site, only the mode-3 split is certain.
 */
#define FIELD_CMD_AREA_END_MODE3 0x801F4000
#define FIELD_CMD_AREA_END       0x801FE800

/**
 * @brief Load/refresh the active field map's asset bundle from CD.
 *
 * Either issues a fresh CD read (when @c D_8005F14A is 0 or the current area
 * @c g_currentMusicTrack differs from the cached @c D_8005F100) or restores from the
 * cached pointer @c D_8005F104. Then loads the secondary asset, snapshots
 * pointer-table headers into globals (@c D_800C7208, @c D_800D5EA4 family,
 * etc.), copies script data to the @c FIELD_SCRIPT_STAGE staging region, and
 * dispatches the field-VM pool setup via @c func_800BFBBC and friends.
 *
 * @return Pointer past the last block laid out after the bundle.
 *
 * @note The streaming table at @c D_800C0900 is three @c {sector,size} pairs
 *       per 24-byte entry, and the two words of a pair must be read as separate
 *       flat-array elements (@c D_800C0900[i*6] / @c [i*6+1]), a struct field
 *       pair makes gcc share one address register where the original computes
 *       it twice. The @c func_800A1CC0 guard is
 *       @c ((state != 1 && state != 6) || unk0D == 1), the call fires for
 *       every state outside {1,6}, not only inside them.
 * @note @c ptr is deliberately re-read into itself (@c ptr++/@c ptr--) after
 *       @c buf is copied from it, and is reused for the return value at the
 *       end. Both are load-bearing for the register allocation gcc 2.7.2
 *       produces here: the bump splits the two pointers into separate registers
 *       (without it they share one, and the header pointer is loaded straight
 *       into the callee-saved register instead of @c a0), and the trailing
 *       reuse of @c ptr keeps @c buf from being picked as the shared name for
 *       the pair, which is what puts the @c D_800C7200 address in @c s0.
 */
s32 *func_800983F0(void) {
    s32 stageBase;
    u8 *ptr;
    u8 *buf;
    u32 tag;
    s32 size;
    u8 *heapEnd;

    if (D_8005F14A == 0 || g_currentMusicTrack != D_8005F100) {
        func_80038868(D_800C0900[D_800C2568[g_currentMusicTrack] * 6],
                      D_800C0900[D_800C2568[g_currentMusicTrack] * 6 + 1], (u8 *)FIELD_BUNDLE_BUF,
                      NULL);
        while (func_800393C8() != 0) {}
    } else {
        while (func_800393C8() != 0) {}
        func_80038490(D_8005F104, FIELD_BUNDLE_BUF);
    }

    func_800A0D6C((u8 *)FIELD_BUNDLE_STRIPS);
    while (DrawSync(1) != 0) {}

    func_800A2D2C(*(s16 **)FIELD_BUNDLE_TIM, *(s32 *)FIELD_BUNDLE_SLOT);

    D_8005F100 = 0;
    D_8005F142 = 0;
    func_80038868(D_800C0908[D_800C2568[g_currentMusicTrack] * 6],
                  D_800C0908[D_800C2568[g_currentMusicTrack] * 6 + 1], (u8 *)FIELD_BUNDLE_BUF, NULL);
    while (func_800393C8() != 0) {}

    D_8005F0F8 = (EventQueue *)*D_800C7208;
    D_800D5EA4 = *D_800C71EC;
    stageBase = FIELD_SCRIPT_STAGE;
    g_fieldEntity.unk018 = *D_800D5EAC;
    size = *D_800D5E8C - *D_800D5ED4;
    func_80039678(FIELD_SCRIPT_STAGE, (s32)*D_800D5ED4, size);
    D_8005F13C = stageBase + size;

    ptr = *D_800D5E94;
    buf = ptr;
    ptr++;
    ptr--;
    tag = *(s16 *)buf;
    D_800C7200 = (FieldSubsceneBuffer *)ptr;
    if (tag != FIELD_HEADER_EMPTY) {
        func_800A2EE0(ptr);
        buf += FIELD_HEADER_SIZE;
        *D_800D5ED4 = buf;
        if (D_8005F14C != 3 && D_8005F14C != 6 && D_8005F14C != 0xA) {
            func_800A2F28((s32)D_800C7200, (u8 *)&g_fieldEntity);
        }
    } else {
        D_800C7200 = NULL;
    }

    D_800704B2 = 20;

    if (D_8005F14C == 3) {
        buf = (u8 *)func_800BFBBC((u8 *)FIELD_SCRIPT_STAGE, (Eline *)0x80090800, (u16 *)*D_800D5ED4, 0);
    } else {
        buf = (u8 *)func_800BFBBC((u8 *)FIELD_SCRIPT_STAGE, (Eline *)0x80090800, (u16 *)*D_800D5ED4, 1);
    }

    D_800C6D98[0] = buf;
    buf = (u8 *)func_800A0640((SPRT_16 *)buf);
    D_800C6D98[1] = buf;
    buf = (u8 *)func_800A0640((SPRT_16 *)buf);

    if (D_8005F0F8->unk0E == 1) {
        D_800D5EC8[0] = buf;
        buf = (u8 *)func_800A29C0((func_800A29C0_arg0 *)buf);
        D_800D5EC8[1] = buf;
        buf = (u8 *)func_800A29C0((func_800A29C0_arg0 *)buf);
        D_800D5EB8[0] = buf;
        buf = (u8 *)func_800A2A30((func_800A2A30_item *)buf);
        D_800D5EB8[1] = buf;
        buf = (u8 *)func_800A2A30((func_800A2A30_item *)buf);
    }

    if ((D_8005F14C != 1 && D_8005F14C != 6) || D_8005F0F8->unk0D == 1) {
        func_800A1CC0();
    }

    if (D_8005F14C == 3) {
        heapEnd = (u8 *)FIELD_CMD_AREA_END_MODE3;
    } else {
        heapEnd = (u8 *)FIELD_CMD_AREA_END;
    }

    if (D_8005F0F8->unk0D == 0) {
        buf = (u8 *)func_800AA8A0(buf, buf + 0x20000, D_800C30DC, D_800C311C,
                                  (u8 *)&D_800C0910[D_800C2568[g_currentMusicTrack] * 3], 0, D_800C06A0,
                                  heapEnd);
    } else {
        buf = (u8 *)func_800AA8A0(buf, buf + 0x20000, D_800C30DC, D_800C315C,
                                  (u8 *)&D_800C0910[D_800C2568[g_currentMusicTrack] * 3], 0, D_800C06A0,
                                  heapEnd);
    }

    if (D_8007064D == 0) {
        while (g_fadeMode != 0) {}
    }
    while (g_renderMode == 4) {}
    while (DrawSync(1) != 0) {}

    if (D_8005F14C == 3 || D_8005F14C == 0) {
        SetDispMask(0);
    }

    ptr = buf;
    return (s32 *)ptr;
}

/**
 * Zero 0x40 bytes at g_fieldEntity+0x1B8 (backwards loop).
 */
void func_80098934(void) {
    s32 i = 0x3F;
    volatile u8 *base = (u8 *)&g_fieldEntity;
    u8 *ptr = (u8 *)base + 0x3F;
    do {
        *(u8 *)(ptr + 0x1B8) = 0;
        i--;
        ptr--;
    } while (i >= 0);
}

/**
 * @brief Field engine reset / map-transition orchestrator (the big state machine).
 *
 * Runs the per-frame outer init/reset loop for the field engine. The whole
 * function dispatches on @c D_8005F14C (the field load mode, 0=fresh,
 * 1=normal, 2=new-area, 3=movie, 6=transition, 0xA=skip-transition) and on
 * @c g_fieldEntity.mode (the engine-level state byte that picks one of several
 * exit paths at the end of each iteration: 4=quit, 5/6=copyFramebuffer +
 * flag-reset, 7=sndCmd21 + snapshot, 1=loop back, 3/8/etc.=plain exit).
 *
 * Major phases each pass:
 *   - @c memcpy 8 bytes of the overlay header at @c D_80098000 onto the
 *     stack and call @c InitClearTiles.
 *   - For load modes 0/1/2: reset the @c SystemState entity flags
 *     (@c unk1A6/1A7/1A9/1AE/1B0/1B1) and clear @c fieldStepDelta /
 *     @c unk104 / @c unk106, then call @c func_800A17A4 on the two
 *     16-byte script-VM scratch arrays at @c sys+0x122 and @c sys+0x130
 *     followed by @c func_800A44D8 + @c func_80098934.
 *   - For mode 0: re-init the two @c DRAWENVs and clear them via
 *     @c ClearImage.
 *   - For modes 1/2 (with @c sys->unk1A5 == 0 for mode 1): kick a
 *     framebuffer copy and set the post-copy state flags.
 *   - For all modes except 6: snapshot 12 consecutive pointer-table fields
 *     from the freshly-loaded overlay at @c 0x800E1000 into the
 *     @c D_800C7208 / @c D_800C71E8 / @c D_800D5E* globals, then call
 *     @c func_800983F0 to install the actor pool.
 *   - Compute centered screen rectangles into @c D_800C7210 / @c D_800C7214
 *     from @c D_8005F0F8 's bounding-box fields, then derive
 *     @c D_800C71F0 (entry-table start = @c *D_800C7204 + 4) and
 *     @c D_800D5E98 (entry-table end = @c D_800C71F0 + count * 3 vertices).
 *   - Dispatch @c func_800BF718 with a mode argument that maps
 *     @c D_8005F14C ∈ {6→2, 0xA→3, 3→0, default→1}.
 *
 * @note Two source shapes carry this to a byte match, and both are the
 *       opposite of what reads naturally:
 *
 *       1. Every @c SystemState access goes through @c g_fieldEntity directly
 *          rather than a cached @c SystemState @c *sys. With a pointer local,
 *          gcc builds the address in two pseudos (@c lui @c -> @c fp,
 *          @c addiu @c -> @c s2) and the original has three, cse only
 *          manufactures the extra base-pointer pseudo, and the copy into
 *          @c s2 that comes with it, when the accesses are written against
 *          the symbol. A pointer local is one instruction short however it
 *          is spelled (initialiser, @c register, cast chain, temp pointer,
 *          duplicate init, reordering against the @c memcpy).
 *
 *       2. @c func_800BF718 is called four times, once per arm, not once
 *          with a computed argument. Cross-jumping merges the four @c jal
 *          into the single call the original has, and because there is no
 *          variable to fold, the @c ==3 arm keeps its branch. Assigning a
 *          @c mode variable instead makes gcc collapse the @c 0 / @c 1 arms
 *          into @c xori + @c sltu, unavoidable in every if/else, ternary,
 *          set-override and switch spelling tried.
 *
 *       Several semantic bugs were caught during decomp: the
 *       @c g_fieldEntity.mode = 0 dispatch had an inverted condition;
 *       @c func_800BF718 's argument mapping had 0xA->2 (should be 3) and
 *       3->3 (should be 0); state==7 was missing the @c field_0x120 save;
 *       @c D_800D5E98 was missing the @c +4 offset. Three more surfaced
 *       while closing the last 2%: both @c isrgb24 clears on the
 *       @c DISPENV pair were absent before @c PutDispEnv; state==1 stored
 *       @c g_currentMusicTrack after @c sndCmd21 instead of before (the original
 *       loads @c counter first and lets dbr sink the store into the jal
 *       delay slot, so doing it after reads a post-call value); and the
 *       loop body ended in an unconditional @c break, dropping out of the
 *       field loop for every state except the six that really exit.
 */
void func_8009895C(void) {
    u8 *p;
    EventQueue *q;
    u8 state;
    u8  header[16];

    /* Copy 8-byte header from D_80098000 (lwl/lwr unaligned copy in asm), dead store, kept for codegen match */
    memcpy(header, D_80098000, 8);
    InitClearTiles();

    g_fieldEntity.dialogState = 0;
    g_fieldEntity.unk1A1 = 0;

    while (1) {
        if ((s16)D_8005F14C != 6) {
            func_800A7194();
        }

        if (D_8005F14C == 0 || (s16)D_8005F14C == 1 || (s16)D_8005F14C == 2) {
            g_fieldEntity.unk1A6 = 0;
            g_fieldEntity.unk1A9 = 0;
            g_fieldEntity.unk1A7 = 1;
            g_fieldEntity.unk1A2 = 0;
            g_fieldEntity.unk1AA = 0;
            g_fieldEntity.unk015 = 0;
            g_fieldEntity.fieldStepDelta = 0;
            g_fieldEntity.unk1AE = 0x1C;
            g_fieldEntity.unk1B0 = 0;
            g_fieldEntity.unk1B1 = 0;
            g_fieldEntity.unk104 = 0;
            g_fieldEntity.unk106 = 0;
            func_800A17A4(&g_fieldEntity.oscillators[0]);
            func_800A17A4(&g_fieldEntity.oscillators[1]);
            func_800A44D8();
            func_80098934();
        }

        if (D_8005F14C == 0) {
            func_80098314();
            ClearImage(&g_drawEnvs[0].clip, 0, 0, 0);
            ClearImage(&g_drawEnvs[1].clip, 0, 0, 0);
        }

        if (((s16)D_8005F14C == 1 && g_fieldEntity.unk1A5 == 0) || (s16)D_8005F14C == 2) {
            VSync(0);
            func_80098314();
            if ((s16)D_8005F14C == 2) {
                g_bufferIndex = 1;
            }
            copyFramebuffer();
            g_fadeMode = 1;
            g_fadeCounter = 0;
            D_8005F11E = 0;
            g_renderMode = 1;
        }

        if ((s16)D_8005F14C != 6) {
            D_800C7208 = (u8 **)0x800E1000;
            D_800C71E8 = (FieldView **)0x800E1004;
            D_800C7204 = (TriangleList **)0x800E1008;
            D_800D5E90 = (ScriptList *)0x800E100C;
            D_800D5E9C = (u16 **)0x800E1010;
            D_800C71F4 = (u8 **)0x800E1014;
            D_800C720C = (u16 **)0x800E1018;
            D_800C71EC = (u8 **)0x800E101C;
            D_800D5EAC = (s32 *)0x800E1020;
            D_800D5E94 = (u8 **)0x800E1024;
            D_800D5ED4 = (u8 **)0x800E1028;
            D_800D5E8C = (u8 **)0x800E102C;
            p = func_800983F0();
            D_8005F104 = (s32)p;
            D_8005F13C = (s32)p;
        }

        func_800A1BB8();
        func_80098314();
        SetDrawEnv((DR_ENV *)D_800CC118, &g_drawEnvs[0]);
        SetDrawEnv((DR_ENV *)&D_800CC118[0x6638], &g_drawEnvs[1]);
        SetDrawEnv((DR_ENV *)&D_800CC118[0x40], &g_drawEnvs[0]);
        SetDrawEnv((DR_ENV *)&D_800CC118[0x6678], &g_drawEnvs[1]);

        if (D_8005F14C == 0
            || (s16)D_8005F14C == 3
            || (s16)D_8005F14C == 6
            || (s16)D_8005F14C == 0xA) {
            *(u8 *)&g_fieldEntity = 0;
        }

        if (D_800C7200 != 0) {
            func_800A3FE0(D_800C7200);
        }

        if (D_8005F14C == 0 || (s16)D_8005F14C == 1 || (s16)D_8005F14C == 2) {
            func_800A62EC(D_8005F0F8->segs);
            q = D_8005F0F8;
            g_fieldEntity.unk1A4 = 0;
            g_fieldEntity.unk1A8 = q->unk09;
            g_fieldEntity.unk100 = q->unk09;
        } else {
            g_fieldEntity.unk010 = 2;
        }

        func_800A42EC(D_800CD1B0, &D_800CD1B0[0x5A0]);
        func_800A42EC(&D_800CD1B0[0x6638], &D_800CD1B0[0x6BD8]);
        func_800A2128(D_800CD1B0 - 0x5F98);
        func_800A2128(&D_800CD1B0[0x6A0]);

        DrawSyncCallback(D_800982F0);
        D_8005F14A = 0;
        D_800C7210 = ((D_8005F0F8->rect_b[0].f4 - D_8005F0F8->rect_b[0].f6) / 2) + D_8005F0F8->rect_b[0].f6;
        D_800C7214 = ((D_8005F0F8->rect_b[0].f2 - D_8005F0F8->rect_b[0].f0) / 2) + D_8005F0F8->rect_b[0].f0;
        /* *D_800C7204 points to a header: { u16 count; pad[2]; entries[count][24]; }.
         * D_800C71F0 skips past the count to the entry array.
         * D_800D5E98 ends up one-past-the-last entry. */
        D_800C71F0 = (SVert *)((u8 *)*D_800C7204 + 4);
        D_800D5E98 = (AdjRec *)((u8 *)D_800C71F0 + (*(u16 *)*D_800C7204) * 24);

        if ((s16)D_8005F14C != 6 && (s16)D_8005F14C != 3) {
            func_8009AEC0();
        }

        if ((s16)D_8005F14C != 6 && (s16)D_8005F14C != 0xA) {
            if ((s16)D_8005F14C == 3) {
                func_800BF718(0);
            } else {
                func_800BF718(1);
            }
        } else {
            if ((s16)D_8005F14C == 6) {
                func_800BF718(2);
            } else {
                func_800BF718(3);
            }
        }

        func_80099348();
        DrawSyncCallback(0);
        while (DrawSync(1) != 0) {}
        VSync(0);
        func_80098314();

        g_dispEnvs[0].isrgb24 = 0;
        g_dispEnvs[1].isrgb24 = 0;
        PutDispEnv(&g_dispEnvs[(s16)g_bufferIndex]);
        PutDrawEnv(&g_drawEnvs[(s16)g_bufferIndex]);

        state = *(u8 *)&g_fieldEntity;
        D_8005F14C = 1;

        if (state == 4) {
            VSync(0);
            SetDispMask(0);
            D_8005F14A = 0;
            break;
        }
        if (state == 3 || state == 8) {
            D_8005F14A = 0;
            break;
        }
        if (state == 5 || state == 6) {
            copyFramebuffer();
            g_fadeMode = 9;
            g_fadeCounter = 0;
            D_8005F11E = 0;
            g_renderMode = 1;
            D_8005F14A = 0;
            break;
        }
        if (state == 7) {
            *(u8 *)&g_fieldEntity = 0;
            g_vsyncRate = 2;
            g_fieldEntity.field_0x120 = g_currentMusicTrack;
            D_80082C8C.unk02 = *(u8 *)&g_fieldEntity.counter;
            D_80082C8C.cmd = *(u8 *)&g_fieldEntity.spawnTriIdx;
            D_80082C8C.unk03 = *(u8 *)&g_fieldEntity.anim_state;
            sndCmd21(-1, 0);
            break;
        }
        if (state == 1) {
            *(u8 *)&g_fieldEntity = 0;
            g_fieldEntity.field_0x120 = g_currentMusicTrack;
            g_currentMusicTrack = g_fieldEntity.counter;
            sndCmd21(-2, g_fieldEntity.field1B4);
            if (g_fieldEntity.unk1B0 != 1) {
                func_800ACB10();
            } else if (D_8005F0F8->unk0D == 0) {
                func_800A1CC0();
            } else {
                func_800ACB10();
            }
            D_8005F14A = 0;
            deactivateBattleCmd(-1);
            func_80027448();
        }
    }

    deactivateBattleCmd(-1);
    func_80027448();
    VSync(0);
}


/**
 * @brief Empty in the retail build, and called from nowhere.
 *
 * @note Purpose unknown. It survives as a real function -- the linker kept
 *       its address and @c fe_object1.h still declares it -- so it was most
 *       likely a debug or teardown hook whose body was compiled out, sitting
 *       as it does between the shutdown path above and the SFX fade-out in
 *       @ref func_8009912C.
 */
void func_80099124(void) {
}

/** @brief Call fadeOutSfxFast for sound channels 0-7, then renderAndUpdateDisplay(1). */
void func_8009912C(void) {
    s16 i = 0;

    do {
        fadeOutSfxFast(i);
        i++;
    } while (i < 8);

    renderAndUpdateDisplay(1);
}

extern s32  getAnimFrameParam(s32 slot, s32 sub); /* per-pad input-frame param (s32 view) */
extern s32  func_80027A58(s32 a, s32 b);          /* per-pad newly-pressed input */
extern s32  remapControllerInput(s32 arg);               /* map pad input word to a button mask */

/**
 * @brief Per-tick controller-input sampling for the field engine's two pad slots.
 *
 * Snapshots the previous held/button state (@c padHeld → @c padHeldPrev,
 * @c unk150 → @c unk154), refreshes the raw pad buffers, then re-reads the held
 * (@c getAnimFrameParam) and pressed (@c func_80027A58) input for slots 0 and 1.
 *
 * When slot 0 has no direction bits latched yet (@c padHeld & 0xF000 == 0) and a
 * pad is present, it converts the two analog axes into direction bits: X read
 * (@c func_80027DB4 axis 2) sets 0x8000 when @c <0x40 or 0x2000 when @c >=0xC1,
 * Y read (axis 3) sets 0x1000 / 0x4000. Each bit is OR'd into @c padHeld always
 * and into @c padPressed only when it was not held last tick (edge detect).
 *
 * Finally derives the held/pressed button masks (@c remapControllerInput) into
 * @c unk150 / @c ambientFlags.
 */
void func_80099180(void) {
    s32 r;

    g_fieldEntity.padHeldPrev = g_fieldEntity.padHeld;
    g_fieldEntity.unk154 = g_fieldEntity.unk150;
    func_800275D4();
    g_fieldEntity.padHeld = getAnimFrameParam(0, 0);
    g_fieldEntity.padPressed = func_80027A58(0, 0);
    g_fieldEntity.field_0x160 = getAnimFrameParam(1, 0);
    g_fieldEntity.field_0x168 = func_80027A58(1, 0);

    if (!(g_fieldEntity.padHeld & 0xF000) && func_80027DB4(0, PAD_AXIS_X, 0) != -1) {
        r = (s16)func_80027DB4(0, PAD_AXIS_X, 0);
        if (r < 0x40) {
            g_fieldEntity.padHeld |= 0x8000;
            if (!(g_fieldEntity.padHeldPrev & 0x8000)) g_fieldEntity.padPressed |= 0x8000;
        } else if (r >= 0xC1) {
            g_fieldEntity.padHeld |= 0x2000;
            if (!(g_fieldEntity.padHeldPrev & 0x2000)) g_fieldEntity.padPressed |= 0x2000;
        }
        r = (s16)func_80027DB4(0, PAD_AXIS_Y, 0);
        if (r < 0x40) {
            g_fieldEntity.padHeld |= 0x1000;
            if (!(g_fieldEntity.padHeldPrev & 0x1000)) g_fieldEntity.padPressed |= 0x1000;
        } else if (r >= 0xC1) {
            g_fieldEntity.padHeld |= 0x4000;
            if (!(g_fieldEntity.padHeldPrev & 0x4000)) g_fieldEntity.padPressed |= 0x4000;
        }
    }

    g_fieldEntity.unk150 = remapControllerInput(g_fieldEntity.padHeld);
    g_fieldEntity.ambientFlags = remapControllerInput(g_fieldEntity.padPressed);
}

/* Park the real stack pointer at 0x1F8003FC and run the next call with its
   stack in the PSX scratchpad, then restore it. There is no way to express a
   stack switch in C, so these are the original's inline asm. */
#define SCRATCH_STACK_ENTER()                                                  \
    __asm__ volatile("lui  $at, 0x1F80;"                                       \
                     "ori  $at, $at, 0x03FC;"                                  \
                     "sw   $sp, 0( $at );"                                     \
                     "lui  $at, 0x1F80;"                                       \
                     "ori  $at, $at, 0x03EC;"                                  \
                     "addu $sp, $at, $zero")

#define SCRATCH_STACK_LEAVE()                                                  \
    __asm__ volatile("lui  $at, 0x1F80;"                                       \
                     "ori  $at, $at, 0x03FC;"                                  \
                     "lw   $sp, 0( $at )")

/**
 * @brief The field engine's per-frame loop: run one frame of the field scene
 *        until something asks the engine to leave.
 *
 * Clears both draw environments (unless @c SystemState::unk1A5 suppressed it),
 * then loops. Each iteration flips @ref g_bufferIndex, points @ref D_800C71E0
 * at that buffer's GPU work area, snapshots the previous display environment
 * into @ref D_8005F110, clears the ordering table and runs the field VM tick
 * (@c func_800BD9C4) on the scratchpad stack.
 *
 * It then checks the two ways the player can leave, the soft-reset pad combo
 * (@c 0x90F held on both this tick and the last) and the menu button, picks
 * the camera view (the battle overlay's when it owns the screen, otherwise the
 * field's, offset by @c 0x28 in sub-scene mode), and runs the render chain:
 * entity update, targeting, oscillators, projection, character shadows, the
 * shimmer ribbons, the walkmesh debug overlay, and the sub-scene sprite pool.
 * Finally it programs the draw environment from the field's clip rectangle,
 * links the two extra prims into the OT, presents, and dispatches on
 * @c SystemState::mode, modes 3, 4, 6, 8 and the menu leave the loop with a
 * code in @ref g_vsyncRate, mode 1/7 waits for the DMA to drain and leaves, and
 * anything else draws the OT and goes round again.
 *
 * @note @c SystemState::dialogState and @ref g_vsyncRate are @c volatile: the
 *       original re-reads @c dialogState for each of the seven comparisons
 *       instead of caching one load, and keeps the @c g_vsyncRate stores out of
 *       branch delay slots.
 * @note The four loop-head assignments are ordered buffer / previous-dispenv /
 *       draw-env / dispenv. That order matters: it keeps the lifetime of
 *       @c D_8005F138 's @c %hi short enough that gcc does not hoist it out of
 *       the loop, which would exhaust the loop-invariant budget before
 *       @c D_800C71F8 (see the memory note on @c threshold @c -= @c 3).
 */
void func_80099348(void) {
    s32 mode;
    s16 i;
    s16 frames;
    u8 *eq;

    if (g_fieldEntity.unk1A5 == 0) {
        ClearImage(&g_drawEnvs[0].clip, 0, 0, 0);
        ClearImage(&g_drawEnvs[1].clip, 0, 0, 0);
    } else {
        g_fieldEntity.unk1A5 = 0;
    }
    frames = 2;
    activateBattleAnim(0);
    func_8009A920(&D_80085224[g_fieldEntity.entityIndex[0]], D_8008538C);

    while (1) {
        func_80099180();
        g_bufferIndex++;
        g_bufferIndex &= 1;
        D_800C71E0 = &D_800C7218[(s16)g_bufferIndex];
        D_8005F110 = D_8005F138;
        g_activeDrawEnv = &g_drawEnvs[(s16)g_bufferIndex];
        D_8005F138 = (s32)&g_dispEnvs[(s16)g_bufferIndex];
        ClearOTagR(D_800C71E0->ot, 0x1000);

        SCRATCH_STACK_ENTER();
        func_800BD9C4(D_800C71E0);
        SCRATCH_STACK_LEAVE();

        /* The whole 0x90F button set held on this tick and the last: tear the
           field down (mode 4) and hand control back to the engine. The bit
           layout of padHeld is not decoded, so the buttons are left unnamed. */
        if ((g_fieldEntity.padHeld & 0x90F) == 0x90F
            && (g_fieldEntity.padHeldPrev & 0x90F) == 0x90F) {
            g_fieldEntity.counter = 0;
            g_fieldEntity.mode = 4;
            g_fieldEntity.spawnTriIdx = 0x7FFF;
            sndStopAll();
            func_800A59D0();
            VSync(0);
            SetDispMask(0);
            break;
        }

        if ((g_fieldEntity.unk150 & 0x20) && func_800BE274() == 0
            && D_80085224[g_fieldEntity.entityIndex[0]].msgActive != 3
            && D_80085224[g_fieldEntity.entityIndex[0]].msgActive != 4
            && (s16)g_fieldEntity.dialogState != 4
            && (s16)g_fieldEntity.dialogState != 1
            && (s16)g_fieldEntity.dialogState != 3
            && (s16)g_fieldEntity.dialogState != 2
            && g_fieldEntity.unk1A3 == 0 && g_fieldEntity.mode == 0) {
            func_8009912C();
            g_fieldEntity.mode = 5;
            g_fieldEntity.counter = 0;
            g_vsyncRate = 6;
            g_fieldEntity.position_x = D_80085224[D_8005F148].posX / 4096;
            g_fieldEntity.position_y = D_80085224[D_8005F148].posY / 4096;
            g_fieldEntity.spawnTriIdx = D_80085224[D_8005F148].triIdx;
            g_fieldEntity.anim_state = D_80085224[D_8005F148].field_0x241;
            func_800A59D0();
            break;
        }

        if (g_fieldEntity.mode == 5) {
            func_8009912C();
            g_vsyncRate = 10;
            func_800A59D0();
            break;
        }

        if (func_800BE274() == 0) {
            if (g_fieldEntity.unk1A6 == 0) {
                D_800C71F8 = *D_800C71E8;
            } else {
                D_800C71F8 = *D_800C71E8 + 1;
            }
        } else {
            D_800C71F8 = D_8005F108;
            g_fieldEntity.unk1B0 = 1;
            if (g_fieldEntity.unk1B1 == 0) {
                g_fieldEntity.unk1B1 = 1;
            }
        }

        SetGeomScreen(D_800C71F8->spriteScale);
        if (g_fieldEntity.unk1A6 != g_fieldEntity.unk1A9) {
            g_fieldEntity.unk1A9 = g_fieldEntity.unk1A6;
            eq = (u8 *)D_8005F0F8;
            g_fieldEntity.unk1A8 = g_fieldEntity.unk100 =
                ((EventQueue *)(eq + g_fieldEntity.unk1A6))->unk09;
        }
        func_8009BEC8(D_80085224, g_fieldEntity.unk150);
        func_8009A7E8(&D_80085224[g_fieldEntity.entityIndex[0]], D_8008538C);
        func_8009CEE8();
        func_800A17B8(&g_fieldEntity.oscillators[0]);
        func_800A17B8(&g_fieldEntity.oscillators[1]);
        func_800A10F4();
        func_800A1318();
        if (g_fieldEntity.unk1A6 == 1) {
            func_800A15C0(D_800C71E0, g_drawEnvs, 1);
        } else {
            func_800A15C0(D_800C71E0, g_drawEnvs, 0);
        }

        if (g_dispEnvs[((s16)g_bufferIndex + 1) & 1].isrgb24 == 0
            && g_fieldEntity.unk1AD == 0) {
            SCRATCH_STACK_ENTER();
            func_800A1CFC(D_80085224, D_800C71E0);
            SCRATCH_STACK_LEAVE();
            func_800A222C(D_800C71E0->ot, &D_800C71F8->m, D_800C71E0->shadowPrims,
                          D_800C71E0->shadowTPages, D_80085224);
            func_800A5224(&D_800C71F8->m, D_800C71E0->ot, D_800C71E0->ribbonPrims,
                          D_800C71E0->ribbonTPages);
        }

        if (func_800BE274() == 0) {
            func_800A06F0(0, D_800C71E0, D_800C6D98[(s16)g_bufferIndex],
                          D_800C71E0->unk4F80);
        } else if (D_8005F0F8->unk0E == 1
                   && g_fieldEntity.unk1A7 == 0) {
            func_800A2AF8(D_800C71E0, D_800D5EC8[(s16)g_bufferIndex],
                          D_800D5EB8[(s16)g_bufferIndex], D_800C71F8);
        }

        if (func_800BE274() == 0 && D_800C7200 != 0) {
            func_800A37A8(&D_800C71F8->m, D_800C71E0, D_800C7200);
            if ((s16)g_bufferIndex == 0) {
                D_800C7200->primCursor = D_800C7200->primArena[0];
            } else {
                D_800C7200->primCursor = D_800C7200->primArena[1];
            }
            for (i = 0; i < 128; i++) {
                if (D_800C7200->entries[i].active == 1) {
                    SetRotMatrix(&D_800C71F8->m);
                    SetTransMatrix(&D_800C71F8->m);
                    func_800A39D8(&D_800C7200->entries[i],
                                  &D_800C7200->records[D_800C7200->entries[i].cmdIndex],
                                  D_800C7200, D_800C71E0->ot);
                    /* stepTotal == 0 means this accumulator ran off the end of its
                       command's waypoints: retire it and drop the command's use count. */
                    if (D_800C7200->records[D_800C7200->entries[i].cmdIndex]
                            .steps[D_800C7200->entries[i].stepIndex].stepTotal == 0) {
                        D_800C7200->entries[i].active = 0;
                        D_800C7200->records[D_800C7200->entries[i].cmdIndex].activeCount--;
                    }
                }
            }
        }

        if ((s16)g_bufferIndex == 0) {
            g_drawEnvs[(s16)g_bufferIndex].clip.x = D_8005F0F8->rect_b[0].f6;
        } else {
            g_drawEnvs[(s16)g_bufferIndex].clip.x = D_8005F0F8->rect_b[0].f6 + 512;
        }
        g_drawEnvs[(s16)g_bufferIndex].clip.y = D_8005F0F8->rect_b[0].f0;
        g_drawEnvs[(s16)g_bufferIndex].clip.w =
            D_8005F0F8->rect_b[0].f4 - D_8005F0F8->rect_b[0].f6;
        g_drawEnvs[(s16)g_bufferIndex].clip.h =
            D_8005F0F8->rect_b[0].f2 - D_8005F0F8->rect_b[0].f0;
        SetDrawEnv(&D_800C71E0->drawEnvPrim, &g_drawEnvs[(s16)g_bufferIndex]);

        addPrim(&D_800C71E0->ot[0xFFF], &D_800C71E0->drawEnvPrim);
        addPrim(&D_800C71E0->ot[1], &D_800C71E0->unk4F00);

        if (func_800BE274()) {
            renderAndUpdateDisplay(g_fieldEntity.unk1AC);
        } else {
            renderAndUpdateDisplay(2);
        }
        renderBattleDisplayList((s32 *)D_800C71E0->ot);

        if (g_fieldEntity.mode == 6) {
            g_vsyncRate = 9;
            func_800A59D0();
            break;
        }
        if (g_fieldEntity.mode == 4) {
            func_800A59D0();
            break;
        }
        if (g_fieldEntity.mode == 3 || g_fieldEntity.mode == 8) {
            g_fieldEntity.position_x = D_80085224[D_8005F148].posX / 4096;
            g_fieldEntity.position_y = D_80085224[D_8005F148].posY / 4096;
            g_fieldEntity.spawnTriIdx = D_80085224[D_8005F148].triIdx;
            func_8009912C();
            mode = g_fieldEntity.mode;
            if (mode == 3) {
                g_vsyncRate = mode;
            } else {
                g_vsyncRate = 8;
            }
            func_800A59D0();
            break;
        }

        if ((g_fieldVars->stateFlags & FIELD_STATE_CAMERA_SHAKE) && g_gameState.mainData.countdownTimer == 0
            && (g_fieldVars->fieldB6 & 0x100) == 0) {
            g_fieldEntity.counter = 0x4B;
            g_fieldEntity.mode = 1;
            g_fieldEntity.spawnTriIdx = 0x7FFF;
            func_800A59D0();
        }
        if (g_fieldEntity.mode == 1 || g_fieldEntity.mode == 7) {
            func_8009912C();
            while (DrawSync(1) != 0) {
            }
            break;
        }

        /* Same four-byte skew into entry 0 that func_8009D598 hands func_8009AAC8;
           see the note there on EventEntry's field names being off by four. */
        func_800A5A20(&D_80085224[D_8005F148],
                      (EventEntry *)&D_8005F0F8->entries[0].z0);
        func_800A5898(D_800C71E0);
        /* Called for its side effect only, func_800BE274 dispatches into the
           overlay when D_800DE4FD bit 1 is set; the original discards the result. */
        func_800BE274();
        encodeCardFilename(0, 0x18, 0xBE);
        D_800D5EA0 = VSync(1);
        while (DrawSync(1) != 0) {
        }
        if (func_800BE274()) {
            VSync(g_fieldEntity.unk1AC);
        } else {
            VSync(2);
        }
        if (frames == 0) {
            SetDispMask(1);
        } else {
            frames--;
        }
        PutDispEnv(&g_dispEnvs[(s16)g_bufferIndex]);

        if ((g_fieldEntity.padHeld & 0x800) && !(g_fieldEntity.padHeldPrev & 0x800)
            && func_800BE274() == 0 && (s16)g_fieldEntity.dialogState != 4
            && (s16)g_fieldEntity.dialogState != 3
            && (s16)g_fieldEntity.dialogState != 2 && g_fieldEntity.unk1A3 == 0) {
            func_800AD7AC(0);
        }

        if (g_fieldEntity.unk1AA == 1) {
            ClearImage(&g_drawEnvs[(s16)g_bufferIndex].clip, 0, 0, 0);
        }
        if (g_fieldEntity.unk1B1 == 1) {
            g_fieldEntity.unk1B1 = 2;
            func_800A1C64();
        }
        func_800393C8();
        func_800BE2DC();
        if (g_fieldEntity.unk1A1 == 0) {
            DrawOTag(&D_800C71E0->ot[0xFFF]);
        }
    }
    func_800BE2AC();
}


/**
 * @brief Angle (8-bit BAM) and distance between two 2D points.
 *
 * Computes @c dx / @c dy from the two points, stores the squared distance to
 * @c *outDist, replaces it with the true distance via @c SquareRoot0 (isqrt),
 * then normalizes the deltas to a fixed ~[-128,128] scale and resolves the
 * octant with a magnitude compare (@c |dx| vs @c |dy|) plus the two sign tests.
 * Each octant reads the arctangent table @c D_800C319C at @c 2*|minor| and adds
 * the appropriate quadrant offset; the result is a full-circle angle where
 * 256 == one revolution.
 *
 * @param p0       First point (@c p0[0]=x, @c p0[1]=y).
 * @param p1       Second point (@c p1[0]=x, @c p1[1]=y).
 * @param outDist  Receives the distance from @p p0 to @p p1.
 * @return Angle from @p p0 to @p p1 in the range 0-255.
 *
 * @note The shared @c "return (r + 0x40) & 0xFF" is what lets gcc 2.7.2
 *       cross-jump the eight octant tails into one, matching the target's
 *       merged code. Return type is @c s32 (not @c u8) so the @c &0xFF mask
 *       stays in this function and the caller keeps a plain @c v0 passthrough.
 */
s32 func_8009A0E8(s32 *p0, s32 *p1, s32 *outDist) {
    s32 dx = p1[0] - p0[0];
    s32 dy = p1[1] - p0[1];
    s32 d2 = dx * dx + dy * dy;
    s32 dist;
    s32 r;

    *outDist = d2;
    dist = SquareRoot0(d2);
    *outDist = dist;

    dx = ((dx << 12) / dist) / 32;
    dy = ((dy << 12) / dist) / 32;

    if (dx * dx > dy * dy) {
        if (dx > 0) {
            if (dy > 0) r = D_800C319C[2 * dy];
            else r = -D_800C319C[-2 * dy];
        } else {
            if (dy > 0) r = -0x80 - D_800C319C[2 * dy];
            else r = D_800C319C[-2 * dy] - 0x80;
        }
    } else {
        if (dy > 0) {
            if (dx > 0) r = 0x40 - D_800C319C[2 * dx];
            else r = D_800C319C[-2 * dx] + 0x40;
        } else {
            if (dx > 0) r = D_800C319C[2 * dx] - 0x40;
            else r = -0x40 - D_800C319C[-2 * dx];
        }
    }
    return (r + 0x40) & 0xFF;
}

/**
 * @brief Project a point onto a 3D line segment and return the squared
 *        distance to the closest point, bounds-checked in X and Y.
 *
 * Computes the fixed-point projection parameter
 * @c t = -256 * dot(P0 - P, dir) / |dir|^2 and writes the closest point
 * @c P0 + t*dir/256 to @p out. The projection is then validated against the
 * segment's X and Y coordinate ranges (either winding accepted; Z is
 * unconstrained): if the closest point falls outside, -1 is returned and the
 * caller treats the segment as missed.
 *
 * @param seg  Line segment (start/end XYZ as consecutive s16 coords).
 * @param p    Query point.
 * @param out  Receives the closest point on the segment's carrier line.
 * @return Squared 3D distance from @p p to the closest point, or -1 when the
 *         projection lies outside the segment's X/Y extent.
 *
 * @note One variable @c d carries the division quotient, the -1 miss value,
 *       and the final squared distance through a single return, and the
 *       bounds checks use bail-out gotos with second-chance labels
 *       (@c xc2 / @c yc2) entered both by branch and fall-through. Both are
 *       original-source structure: restructured single-purpose-variable or
 *       pure if/else forms compile to measurably different code (the
 *       fall-through recheck gets folded away by cse/jump threading).
 */
s32 func_8009A2BC(LineSeg *seg, Vec3i *p, Vec3i *out) {
    s32 dx, dy, dz;
    s32 tx = (seg->x0 - p->x) * (dx = seg->x1 - seg->x0);
    s32 ty = (seg->y0 - p->y) * (dy = seg->y1 - seg->y0);
    s32 tz = (seg->z0 - p->z) * (dz = seg->z1 - seg->z0);
    s32 d = (-((tx + ty + tz) << 8)) / (dx * dx + dy * dy + dz * dz);
    s32 ex, ey, ez;
    s32 ax, ay, ox, oy;

    out->x = ((d * dx) >> 8) + seg->x0;
    out->y = ((d * (seg->y1 - seg->y0)) >> 8) + seg->y0;
    out->z = ((d * (seg->z1 - seg->z0)) >> 8) + seg->z0;

    ax = seg->x0 - (ox = out->x);
    if (ax < 0) goto xc2;
    if (seg->x1 - ox <= 0) goto yaxis;
    if (ax > 0) goto fail;
xc2:
    if (seg->x1 - ox < 0) goto fail;
yaxis:
    ay = seg->y0 - (oy = out->y);
    if (ay < 0) goto yc2;
    if (seg->y1 - oy <= 0) goto sum;
    if (ay > 0) goto fail;
yc2:
    if (seg->y1 - oy < 0) goto fail;
sum:
    ex = out->x - p->x;
    ey = out->y - p->y;
    ez = out->z - p->z;
    d = ex * ex + ey * ey + ez * ez;
    goto done;
fail:
    d = -1;
done:
    return d;
}

/**
 * @brief Per-frame update of the @ref Eline trigger pool against the
 *        self entity's position.
 *
 * Stages @p self 's position and a secondary query point @p pt (its Z is taken
 * from @p self) into the PSX scratchpad, then for each enabled record
 * (@c activeMarker @c == @c 1) projects the query point onto the record's
 * segment (@c x0..z1) via @ref func_8009A2BC. When the projection lies inside
 * @c self->radius the record is latched in-range (@c trigger4) and its trigger
 * state is refreshed: @c trigger2 on the first in-range frame, @c trigger5 when
 * self and the query point straddle the segment edge (2D cross-product signs
 * differ), @c trigger6 / @c unk19D when self coincides with the projected point
 * or falls within a +/-64 facing window, @c unk19C the facing angle
 * (@ref func_8009A0E8), and @c trigger7 (1/2) from the current pad-hold mode.
 * When out of range, @c trigger3 is raised on the frame the record leaves.
 *
 * @param actor    Querying entity.
 * @param records @ref Eline pool (count @c D_800852F8).
 * @param pt      Secondary query point; its Z is taken from @p self.
 * @return Always 0.
 *
 * @note The scratchpad points are staged as three consecutive VECTORs at
 *       @c getScratchAddr(0) / +0x10 / +0x20; @c queryPt and @c proj are
 *       derived from @c selfPos (not built as fresh constants) so the compiler
 *       shares one scratchpad base register across all three (addu+ori), as in
 *       the original. The cursor inits before the staging (fc first, and fc
 *       re-assigned after it), the empty do/while barrier, and the
 *       @c fc++,rec++ increment order pin the original's register allocation
 *       and the loop-end branch-delay schedule.
 */
s32 func_8009A4C0(Actor *actor, Eline *records, VECTOR *pt) {
    VECTOR *selfPos = (VECTOR *)getScratchAddr(0);
    VECTOR *queryPt;
    VECTOR *proj;
    Eline *fc;
    Eline *rec;
    s32 i;
    s32 dist;

    fc = records;
    rec = records;
    selfPos->vx = actor->posX >> 12;
    selfPos->vy = actor->posY >> 12;
    selfPos->vz = actor->posZ >> 12;
    queryPt = (VECTOR *)((u32)selfPos | 0x10);
    proj = (VECTOR *)((u32)selfPos | 0x20);
    queryPt->vx = pt->vx >> 12;
    queryPt->vy = pt->vy >> 12;
    queryPt->vz = actor->posZ >> 12;
    fc = records;
    do { } while (0);

    for (i = 0; i < D_800852F8; i++, fc++, rec++) {
        if (fc->activeMarker != 1) {
            continue;
        }
        fc->unk19D = 0;
        dist = func_8009A2BC((LineSeg *)&rec->x0, queryPt, proj);
        if (dist != -1 && dist < actor->radius * actor->radius) {
            s32 dx;
            s32 dy;
            s32 crossSelf;
            s32 crossPt;

            if (fc->trigger4 == 0) {
                fc->trigger2 = 1;
            }
            fc->trigger4 = 1;
            dx = fc->x1 - fc->x0;
            dy = fc->y1 - fc->y0;
            crossSelf = dx * (selfPos->vy - fc->y0) - (selfPos->vx - fc->x0) * dy;
            crossPt = dx * (queryPt->vy - fc->y0) - (queryPt->vx - fc->x0) * dy;
            if ((crossSelf >= 0 && crossPt < 0) || (crossPt >= 0 && crossSelf < 0)
                || (crossSelf > 0 && crossPt <= 0) || (crossPt > 0 && crossSelf <= 0)) {
                fc->trigger5 = 1;
            }
            if (selfPos->vx == proj->vx && selfPos->vy == proj->vy) {
                fc->trigger6 = 1;
                fc->unk19D = 1;
            } else {
                fc->unk19C = func_8009A0E8((s32 *)selfPos, (s32 *)proj, &dist);
                if (((fc->unk19C - actor->unk23F + 0x40) & 0xFF) < 0x80) {
                    fc->trigger6 = 1;
                    fc->unk19D = 1;
                }
            }
            if (fc->unk19D == 1 && ((fc->unk19C - actor->unk23F + 0x20) & 0xFF) < 0x40) {
                if ((g_fieldEntity.unk150 & 0x40) && !(g_fieldEntity.unk154 & 0x40)) {
                    fc->trigger7 = 1;
                }
                if ((g_fieldEntity.unk150 & 0x80) && !(g_fieldEntity.unk154 & 0x80)) {
                    fc->trigger7 = 2;
                }
            }
        } else {
            if (fc->trigger4 == 1) {
                fc->trigger3 = 1;
            }
            fc->trigger4 = 0;
        }
    }

    return 0;
}

/**
 * @brief Sync per-entity @c trigger7 across the Eline pool from
 *        the global @c unk150 / @c unk154 SFX flag pair.
 *
 * Iterates the @c D_800852F8 entities at @c pool. For each entity that
 * passes the gating tests
 *   - @c activeMarker == 1
 *   - @c actor->msgActive == 0
 *   - @c unk19D == @c activeMarker (so @c unk19D == 1)
 *   - @c entity->unk19C falls within a @c +/-32 window of @c actor->unk23F
 *
 * writes @c trigger7 from the @c g_fieldEntity.unk150 / @c unk154 pair:
 *   - bit 6 set in @c unk150 and clear in @c unk154 → @c trigger7 = @c unk19D (= 1)
 *   - bit 7 set in @c unk150 and clear in @c unk154 → @c trigger7 = 2
 *
 * The for-loop's @c i++, pool++ in the increment clause (comma
 * operator) keeps gcc 2.7.2 from reordering the increments into the
 * count-reload @c lbu 's load-delay slot, target leaves that slot
 * as a @c nop.
 */
void func_8009A7E8(Actor *actor, Eline *pool) {
    s32 i;
    for (i = 0; i < D_800852F8; i++, pool++) {
        if (pool->activeMarker == 1) {
            if (actor->msgActive == 0) {
                if (pool->unk19D == pool->activeMarker) {
                    if ((s32)(((s32)pool->unk19C - (s32)actor->unk23F + 0x20) & 0xFF) < 0x40) {
                        if (g_fieldEntity.unk150 & 0x40) {
                            if (!(g_fieldEntity.unk154 & 0x40)) {
                                pool->trigger7 = pool->unk19D;
                            }
                        }
                        if (g_fieldEntity.unk150 & 0x80) {
                            if (!(g_fieldEntity.unk154 & 0x80)) {
                                pool->trigger7 = 2;
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * @brief Clear @c trigger4 and @c unk19D across every @c Eline in the pool.
 *
 * Walks the entire @c D_8008538C pool (size @c D_800852F8) and zeros each
 * entity's @c trigger4 (offset 0x196) and @c unk19D (offset 0x19D). Called
 * from the @c PCOPYINFO / @c SET script opcodes via @c func_8009A8E0(D_8008538C)
 * after copying entity state, likely a reset of trigger-edge / pending-flag
 * bookkeeping so the new state takes effect without leftover triggers.
 *
 * The loop is written do-while with a top-of-iter @c i++ so the count check
 * uses the post-incremented value (@c v1 < count tests "one more iteration
 * is in range"). The pool count @c D_800852F8 is reloaded each iteration
 * because gcc can't prove the stores through @p e don't alias it.
 */
void func_8009A8E0(Eline *eline) {
    s32 i = 0;
    if (D_800852F8 != 0) {
        do {
            i++;
            eline->trigger4 = 0;
            eline->unk19D = 0;
            eline++;
        } while (i < D_800852F8);
    }
}

/**
 * @brief Per-frame proximity check, for each @c Eline in
 *        @c D_800852F8 entries, run @c func_8009A2BC against the actor's
 *        position and set per-entity trigger bytes based on the hit
 *        distance vs the actor's collision radius.
 *
 * Writes the actor's @c (posX, posY, posZ) @c >> @c 12 to the PSX
 * scratchpad at @c getScratchAddr(2..4), then iterates each
 * @c Eline and (when active and the actor isn't in a message)
 * calls @c func_8009A2BC. The returned distance is compared against
 * @c radius*radius:
 *  - hit (in range): @c trigger4=1; also @c trigger2=1 when
 *    @c D_8005F14C @c == @c 3
 *  - miss (out of range or @c -1): @c trigger3=1, @c trigger4=0
 *
 * @param actor    Querying entity (position, radius, message state).
 * @param entities @c Eline pool; walked directly as the loop cursor.
 *
 * @note @c fc is a vestigial second cursor: initialized and stepped in
 *       lockstep but never read. Removing it (or reading through it)
 *       changes the register allocation away from the original, the
 *       original source evidently carried it, so it stays.
 */
void func_8009A920(Actor *actor, Eline *entities) {
    Vec3i *scratch = (Vec3i *)getScratchAddr(0);
    Vec3i *out = (Vec3i *)getScratchAddr(4);
    Eline *fc;
    s32 i;
    s32 dist;

    scratch->x = actor->posX >> 12;
    i = 0;
    scratch->y = actor->posY >> 12;
    scratch->z = actor->posZ >> 12;
    if (i < D_800852F8) {
        fc = entities;
        do {
            if (entities->activeMarker == 1 && actor->msgActive == 0) {
                dist = func_8009A2BC((LineSeg *)&entities->x0, scratch, out);
                if (dist != -1 && dist < actor->radius * actor->radius) {
                    if (D_8005F14C == 3) {
                        entities->trigger2 = 1;
                    }
                    entities->trigger4 = 1;
                } else {
                    entities->trigger3 = 1;
                    entities->trigger4 = 0;
                }
            }
            i++;
            fc++;
            entities++;
        } while (i < D_800852F8);
    }
}

/**
 * @brief Restore an event-entry snapshot into the live @c g_fieldEntity.
 *
 * Copies the 5 snapshot fields stored in an @c EventEntry slot back into
 * the corresponding live fields of @c g_fieldEntity, and selects the
 * engine @c mode from the snapshotted @c counter:
 *   - @c counter < 72 → @c mode = 7 (e.g. resume an in-progress event)
 *   - otherwise → @c mode = 1 (e.g. start a fresh interaction)
 *
 * Used when reactivating a queued event after it was paused or saved.
 */
void func_8009AA64(EventEntry *e) {
    if (e->counter < 72) {
        g_fieldEntity.mode = 7;
    } else {
        g_fieldEntity.mode = 1;
    }
    g_fieldEntity.counter = e->counter;
    g_fieldEntity.position_x = e->position_x;
    g_fieldEntity.position_y = e->position_y;
    g_fieldEntity.spawnTriIdx = e->spawnTriIdx;
    g_fieldEntity.anim_state = e->anim_state;
}

/**
 * @brief Scan the 12-entry event queue for trigger segments the query point
 *        crosses, and fire the event restore for each hit.
 *
 * Stages the actor's position (@c >>12) into the scratchpad at
 * @c getScratchAddr(0) and the query point (X/Y from @p pt, Z from the
 * actor) at @c getScratchAddr(4), then for each armed @ref EventEntry
 * (@c counter != 0x7FFF, @c rotation != 0xFFFF): projects the query point
 * onto the entry's trigger segment via @ref func_8009A2BC, and when the
 * squared distance is inside @c actor->radius² and the actor and the query
 * point lie on opposite sides of the segment (2D cross-product signs
 * differ), restores the entry's event snapshot via @ref func_8009AA64.
 *
 * @param actor Querying entity.
 * @param segs  12-entry @ref EventEntry queue (32-byte stride).
 * @param pt    Query point (world fixed-point; only X/Y read).
 *
 * @note @c B and @c C are derived from @c A with @c |0x10 / @c |0x20 so the
 *       compiler shares one scratchpad base register (addu+ori), as in the
 *       original.
 */
void func_8009AAC8(Actor *actor, EventEntry *segs, Vec3i *pt) {
    Vec3i *A = (Vec3i *)getScratchAddr(0);
    Vec3i *B;
    Vec3i *C;
    s32 i;
    s32 dist;
    s32 crossSelf;
    s32 crossPt;

    A->x = actor->posX >> 12;
    B = (Vec3i *)((u32)A | 0x10);
    A->y = actor->posY >> 12;
    C = (Vec3i *)((u32)A | 0x20);
    A->z = actor->posZ >> 12;
    B->x = pt->x >> 12;
    B->y = pt->y >> 12;
    B->z = actor->posZ >> 12;
    for (i = 0; i < 12; i++, segs++) {
        if (segs->counter == 0x7FFF) {
            continue;
        }
        if (segs->spawnTriIdx == 0xFFFF) {
            continue;
        }
        dist = func_8009A2BC((LineSeg *)&segs->x0, B, C);
        if (dist == -1) {
            continue;
        }
        if (dist < actor->radius * actor->radius) {
            crossSelf = (segs->x1 - segs->x0) * (A->y - segs->y0)
                      - (A->x - segs->x0) * (segs->y1 - segs->y0);
            crossPt = (segs->x1 - segs->x0) * (B->y - segs->y0)
                    - (B->x - segs->x0) * (segs->y1 - segs->y0);
            if ((crossSelf >= 0 && crossPt < 0) || (crossPt >= 0 && crossSelf < 0)
                || (crossSelf > 0 && crossPt <= 0) || (crossPt > 0 && crossSelf <= 0)) {
                func_8009AA64(segs);
            }
        }
    }
}

/** @brief 16-byte padded s32 vector, stack twin of the PsyQ VECTOR layout,
 *  private to @c func_8009AC9C (the pad keeps the frame at 0x10 strides). */
typedef struct {
    s32 x, y, z, pad;
} func_8009AC9C_vec;

/**
 * @brief Brute-force walkmesh triangle lookup: find the triangle whose 2D
 *        extent contains (px, py) and whose plane Z is nearest pz.
 *
 * For each triangle in @p list, builds the three edge vectors with
 * @c func_8009DED8 and tests the query point against all three 2D edge
 * cross-products (inside when all are >= 0, computed up front and then
 * &&-chained). For containing triangles the plane Z at the query point is
 * interpolated via @c func_8009E338 and the triangle with the smallest
 * |Z - pz| wins.
 *
 * @param px    Query X (walkmesh units).
 * @param py    Query Y.
 * @param pz    Query Z (used only to rank containing triangles).
 * @param list  Counted triangle list.
 * @return Index of the best containing triangle, as @c s16.
 *
 * @note If no triangle contains the point (or the list is empty) the return
 *       value is an uninitialized stack halfword, the original never
 *       seeds @c bestIdx. Callers are expected to only use the result when
 *       the point is known to be on the mesh.
 * @note Twin loop cursors (@c s16 @c i for the record offset and result,
 *       @c s32 @c n for the count compare), the byte-offset vertex pointers,
 *       and the @c i++, @c n++ increment order are all register-allocation /
 *       scheduling keys.
 */
s16 func_8009AC9C(s16 px, s16 py, s16 pz, TriangleList *list) {
    func_8009AC9C_vec e0;
    func_8009AC9C_vec e1;
    func_8009AC9C_vec e2;
    func_8009AC9C_vec qp;
    u16 bestIdx;
    s32 best;
    s16 i;
    s32 n;
    s32 c0, c1, c2, z;
    SVert *vA;
    SVert *vB;
    SVert *vC;
    u8 *tris;
    s32 ofs;

    i = 0;
    best = 0x7FFFFFFF;
    qp.x = px;
    qp.y = py;
    qp.z = pz;
    tris = (u8 *)list->tris;
    for (n = 0; n < list->count; i++, n++) {
        ofs = (s16)i * 24;
        vB = (SVert *)(tris + (ofs + 8));
        vA = (SVert *)(tris + ofs);
        func_8009DED8((Vec3i *)&e0, vB, vA);
        ofs += 16;
        vC = (SVert *)(tris + ofs);
        func_8009DED8((Vec3i *)&e1, vC, vB);
        func_8009DED8((Vec3i *)&e2, vA, vC);
        c0 = e0.y * (px - vA->sx) - e0.x * (py - vA->sy);
        c1 = e1.y * (px - vA[1].sx) - e1.x * (py - vA[1].sy);
        c2 = e2.y * (px - vA[2].sx) - e2.x * (py - vA[2].sy);
        if (c0 >= 0 && c1 >= 0 && c2 >= 0) {
            z = func_8009E338((Vec3i *)&e0, (Vec3i *)&e1, (Vec3i *)&qp, (Vec3s *)vA);
            if (qp.z < z) {
                z = z - qp.z;
            } else {
                z = qp.z - z;
            }
            if (z < best) {
                best = z;
                bestIdx = i;
            }
        }
    }
    return (s16)bestIdx;
}

/** @brief Scale applied to @c g_fieldEntity.unk00A when seeding @c Actor::moveSpeed. */
#define FIELD_CHANNEL_SCALE 0x4367

/** @brief Sentinel in @c g_fieldEntity.spawnTriIdx / @c position_x meaning "no override". */
#define SPAWN_UNSET 0x7FFF

/**
 * @brief Place every field entity on the navmesh when a field is entered.
 *
 * For each of the @c D_80085388 entities:
 *  - The player entity (@c D_8005F148, taken from @c g_fieldEntity.entityIndex[0])
 *    is handled specially. When @c g_fieldEntity.spawnTriIdx carries a triangle index
 *    (i.e. is not @c SPAWN_UNSET) the entity is moved onto that triangle:
 *    with no X override it is dropped at the triangle centroid, otherwise its
 *    existing X/Y are kept and only Z is re-derived from the triangle plane.
 *    When no triangle is supplied the entity is reset onto triangle 0 with a
 *    default radius and animation set.
 *  - Every other entity keeps its X/Y and has its Z re-derived from the
 *    triangle it stands on.
 *
 * Z comes from @c func_8009E338, which intersects the entity's X/Y against the
 * plane spanned by two triangle edges built by @c func_8009DED8; the result is
 * shifted back into the 12-bit fixed-point the position fields use.
 *
 * A second pass clears every entity's @c headingBase before the movement
 * (@c func_8009E660) and path (@c func_8009BB18) tables are rebuilt.
 *
 * @note The navmesh is indexed as a flat vertex array, triangle @c t owns
 *       @c D_800C71F0[t*3 .. t*3+2], which is also how @c func_8009DF18 reads
 *       it. A @c Triangle[] view does not reproduce the original's addressing:
 *       gcc then shares the derived triangle pointer between the two corner
 *       arguments and computes the second as @c ptr+8, where the original
 *       scales @c t*3 once and adds the vertex base to each corner.
 * @note @c pos.z is left uninitialised on the player path, only the
 *       non-player path zeroes it. @c func_8009E338 reads just X and Y, so this
 *       is harmless, and the original has the same asymmetry.
 */
void func_8009AEC0(void) {
    Vec3i edge0;
    Vec3i edge1;
    Vec3i pos;
    s16 i;

    D_8005F102 = 0;
    g_fieldEntity.unk00A = 20;
    D_8005F148 = g_fieldEntity.entityIndex[0];

    for (i = 0; i < D_80085388; i++) {
        if (i == D_8005F148) {
            if (g_fieldEntity.spawnTriIdx != SPAWN_UNSET) {
                D_80085224[i].triIdx = g_fieldEntity.spawnTriIdx;
                if (g_fieldEntity.position_x == SPAWN_UNSET) {
                    D_80085224[i].posX = ((D_800C71F0[D_80085224[i].triIdx * 3].sx +
                                           D_800C71F0[D_80085224[i].triIdx * 3 + 1].sx +
                                           D_800C71F0[D_80085224[i].triIdx * 3 + 2].sx) / 3) << 12;
                    D_80085224[i].posY = ((D_800C71F0[D_80085224[i].triIdx * 3].sy +
                                           D_800C71F0[D_80085224[i].triIdx * 3 + 1].sy +
                                           D_800C71F0[D_80085224[i].triIdx * 3 + 2].sy) / 3) << 12;
                    D_80085224[i].posZ = ((D_800C71F0[D_80085224[i].triIdx * 3].sz +
                                           D_800C71F0[D_80085224[i].triIdx * 3 + 1].sz +
                                           D_800C71F0[D_80085224[i].triIdx * 3 + 2].sz) / 3) << 12;
                } else {
                    func_8009DED8(&edge0, &D_800C71F0[D_80085224[i].triIdx * 3 + 1],
                                  &D_800C71F0[D_80085224[i].triIdx * 3]);
                    func_8009DED8(&edge1,
                                  &D_800C71F0[D_80085224[D_8005F148].triIdx * 3 + 2],
                                  &D_800C71F0[D_80085224[D_8005F148].triIdx * 3 + 1]);
                    pos.x = D_80085224[D_8005F148].posX / 4096;
                    pos.y = D_80085224[D_8005F148].posY / 4096;
                    D_80085224[D_8005F148].posZ =
                        func_8009E338(&edge0, &edge1, &pos,
                                      &D_800C71F0[D_80085224[D_8005F148].triIdx * 3]) << 12;
                }
            } else {
                D_80085224[i].field_0x208 = 0x10;
                D_80085224[i].field_0x24F = 0;
                D_80085224[D_8005F148].field_0x250 = 1;
                D_80085224[D_8005F148].field_0x251 = 2;
                /* Same scale func_800B6738 applies to D_800704B2 (there as *69020>>9), so
                   the entity starts exactly at the threshold that picks field_0x251. */
                D_80085224[D_8005F148].moveSpeed = ((u32)(g_fieldEntity.unk00A * 17255)) >> 7;
                D_80085224[D_8005F148].radius = 0x30;
                D_80085224[D_8005F148].triIdx = 0;
                D_80085224[D_8005F148].posX =
                    ((D_800C71F0[D_80085224[D_8005F148].triIdx * 3].sx +
                      D_800C71F0[D_80085224[D_8005F148].triIdx * 3 + 1].sx +
                      D_800C71F0[D_80085224[D_8005F148].triIdx * 3 + 2].sx) / 3) << 12;
                D_80085224[D_8005F148].posY =
                    ((D_800C71F0[D_80085224[D_8005F148].triIdx * 3].sy +
                      D_800C71F0[D_80085224[D_8005F148].triIdx * 3 + 1].sy +
                      D_800C71F0[D_80085224[D_8005F148].triIdx * 3 + 2].sy) / 3) << 12;
                D_80085224[D_8005F148].posZ =
                    ((D_800C71F0[D_80085224[D_8005F148].triIdx * 3].sz +
                      D_800C71F0[D_80085224[D_8005F148].triIdx * 3 + 1].sz +
                      D_800C71F0[D_80085224[D_8005F148].triIdx * 3 + 2].sz) / 3) << 12;
            }
        } else {
            pos.x = D_80085224[i].posX / 4096;
            pos.y = D_80085224[i].posY / 4096;
            pos.z = 0;
            func_8009DED8(&edge0, &D_800C71F0[D_80085224[i].triIdx * 3 + 1],
                          &D_800C71F0[D_80085224[i].triIdx * 3]);
            func_8009DED8(&edge1, &D_800C71F0[D_80085224[i].triIdx * 3 + 2],
                          &D_800C71F0[D_80085224[i].triIdx * 3 + 1]);
            D_80085224[i].posZ = func_8009E338(&edge0, &edge1, &pos,
                                               &D_800C71F0[D_80085224[i].triIdx * 3]) << 12;
        }
    }

    for (i = 0; i < D_80085388; i++) {
        D_80085224[i].headingBase = 0;
    }

    func_8009E660();
    func_8009BB18();
}

/**
 * @brief Drive an entity's walk-cycle phase and fire its footstep sounds.
 *
 * Selects animation @p anim on the entity's render slot (via command @c 0xD
 * through @c func_800AA46C), restarting the cycle from phase zero whenever the
 * requested animation differs from the one already playing.
 *
 * @p delta then advances the slot's phase counter @c unk52 by
 * @c delta @c * the entity's per-frame phase step (@ref Actor::field_0x208),
 * wrapping against the cycle length @c unk0C: forward motion past the end
 * restarts at zero, reverse motion below zero jumps to the last phase. A
 * @p delta of zero leaves the phase alone (the entity is standing still).
 *
 * When @p mode is @c 1 the function also watches for the phase crossing either
 * of the cycle's two contact points — the mid-point @c 0x80 and the point
 * @c half a cycle away from it — and calls @ref func_800B2864 on channel 1 or 0
 * respectively, which plays the positional footstep SFX. Each walk cycle
 * therefore produces two footsteps, one per foot. The crossing test compares
 * the sign of the phase before and after the step, so it fires exactly once per
 * crossing regardless of step size or direction.
 *
 * @param idx   Field-entity index; selects both the render slot
 *              (@ref D_800D9630) and the Actor record (@ref D_80085224).
 * @param anim  Animation id to play.
 * @param mode  @c 1 to emit footstep sounds while stepping, @c 0 to stay silent.
 * @param delta Signed number of phase steps to advance this frame.
 */
void func_8009B4A8(s16 idx, u8 anim, s16 mode, s8 delta) {
    s16 newPhase;
    s16 oldPhase;
    s16 half;
    s16 newRel;
    s16 oldRel;
    s16 newOff;
    s16 oldOff;

    if (D_800D9630[idx]->unk7E != anim) {
        D_800D9630[idx]->unk52 = 0;
    }
    func_800AA46C(idx, 13, anim, 0);

    oldPhase = D_800D9630[idx]->unk52;
    if (delta != 0) {
        if (delta > 0) {
            newPhase = D_800D9630[idx]->unk52 = oldPhase + D_80085224[idx].field_0x208 * delta;
            if (D_800D9630[idx]->unk52 >= D_800D9630[idx]->unk0C) {
                D_800D9630[idx]->unk52 = 0;
            }
        } else {
            newPhase = D_800D9630[idx]->unk52 = oldPhase + D_80085224[idx].field_0x208 * delta;
            if (D_800D9630[idx]->unk0C < D_800D9630[idx]->unk52) {
                D_800D9630[idx]->unk52 = D_800D9630[idx]->unk0C - 1;
            }
        }
    }

    if (mode == 1) {
        newRel = newPhase - 0x80;
        oldRel = oldPhase - 0x80;
        half = (D_800D9630[idx]->unk0C - (s16)D_80085224[idx].field_0x208) >> 1;
        newOff = newRel - half;
        oldOff = oldRel - half;
        if ((newRel < 0 & oldRel >= 0) || (oldRel < 0 & newRel >= 0)) {
            func_800B2864(&D_80085224[idx], 1, 0x7F, 0x80);
        }
        if ((newOff < 0 & oldOff >= 0) || (oldOff < 0 & newOff >= 0)) {
            func_800B2864(&D_80085224[idx], 0, 0x7F, 0x80);
        }
    }
}

/**
 * @brief Dispatch entity animation based on slot index and animation parameters.
 *
 * Looks up the entity by slot index, sets animation state, checks
 * screen position thresholds, then dispatches to func_8009B4A8 with
 * the appropriate animation field based on the parameter's field_0A value.
 *
 * @param slotIdx Slot index (0 or 1).
 * @param paramIdx Index into the animation parameter array.
 * @param params Animation parameter array.
 * @param multiplier Speed/direction multiplier for the animation.
 */
void func_8009B74C(s16 slotIdx, u16 paramIdx, PathEntry *params, s16 multiplier) {
    u8 entityIdx;

    entityIdx = g_fieldEntity.entityIndex[slotIdx];

    if (entityIdx == 0xFF) {
        return;
    }

    D_80085224[g_fieldEntity.entityIndex[(s16)slotIdx]].field_0x241 = params[paramIdx].field_0B;
    D_80085224[g_fieldEntity.entityIndex[slotIdx]].field_0x24C = 1;

    if (slotIdx == 1) {
        if (D_8005F160 > D_8005F118) {
            params[paramIdx].field_0A = 2;
        }
    } else {
        if (D_8005F162 > D_8005F11A) {
            params[paramIdx].field_0A = 2;
        }
    }

    switch (params[paramIdx].field_0A) {
    case 0:
        entityIdx = g_fieldEntity.entityIndex[slotIdx];
        func_8009B4A8(entityIdx, D_80085224[g_fieldEntity.entityIndex[slotIdx]].field_0x250, 0, (s8)(params[paramIdx].field_09 * multiplier));
        break;
    case 1:
        entityIdx = g_fieldEntity.entityIndex[slotIdx];
        func_8009B4A8(entityIdx, D_80085224[g_fieldEntity.entityIndex[slotIdx]].field_0x251, 0, (s8)(params[paramIdx].field_09 * multiplier));
        break;
    case 2:
        entityIdx = g_fieldEntity.entityIndex[slotIdx];
        func_8009B4A8(entityIdx, D_80085224[g_fieldEntity.entityIndex[slotIdx]].field_0x24F, 0, (s8)(params[paramIdx].field_09 * multiplier));
        break;
    case 3:
        entityIdx = g_fieldEntity.entityIndex[slotIdx];
        func_8009B4A8(entityIdx, D_80085224[g_fieldEntity.entityIndex[slotIdx]].field_0x252, 0, (s8)(params[paramIdx].field_09 * multiplier));
        break;
    case 4:
        entityIdx = g_fieldEntity.entityIndex[slotIdx];
        func_8009B4A8(entityIdx, D_80085224[g_fieldEntity.entityIndex[slotIdx]].field_0x253, 0, (s8)(params[paramIdx].field_09 * multiplier));
        break;
    case 5:
        entityIdx = g_fieldEntity.entityIndex[slotIdx];
        func_8009B4A8(g_fieldEntity.entityIndex[slotIdx], D_80085224[entityIdx].field_0x254, 0, (s8)(params[paramIdx].field_09 * multiplier));
        break;
    }
}

/**
 * @brief Update path-driven entity positions for slots 1 and 2.
 *
 * For each active slot (entityIndex != 0xFF), looks up a path waypoint by
 * angle (computed as (D_8005F144 - phase) & FIELD_PATH_RING_MASK → 0..63 entry) and writes
 * its x/y/z (shifted left 12 for fixed-point), unk6 halfword, and unk8 byte
 * to the entity at offsets 0x190, 0x194, 0x198, 0x1FA, 0x258 respectively.
 *
 * Slot 2 reads from D_80070A60 with phase D_8005F11A; slot 1 reads from
 * D_80070760 with phase D_8005F118.
 */
void func_8009BB18(void) {
    u16 angle;

    if (g_fieldEntity.entityIndex[2] != 0xFF) {
        angle = (D_8005F144 - D_8005F11A) & FIELD_PATH_RING_MASK;
        D_80085224[g_fieldEntity.entityIndex[2]].posX   = D_80070A60[angle].x << 12;
        D_80085224[g_fieldEntity.entityIndex[2]].posY   = D_80070A60[angle].y << 12;
        D_80085224[g_fieldEntity.entityIndex[2]].posZ   = D_80070A60[angle].z << 12;
        D_80085224[g_fieldEntity.entityIndex[2]].triIdx = D_80070A60[angle].unk6;
        D_80085224[g_fieldEntity.entityIndex[2]].unk258 = D_80070A60[angle].unk8;
    }
    if (g_fieldEntity.entityIndex[1] != 0xFF) {
        angle = (D_8005F144 - D_8005F118) & FIELD_PATH_RING_MASK;
        D_80085224[g_fieldEntity.entityIndex[1]].posX   = D_80070760[angle].x << 12;
        D_80085224[g_fieldEntity.entityIndex[1]].posY   = D_80070760[angle].y << 12;
        D_80085224[g_fieldEntity.entityIndex[1]].posZ   = D_80070760[angle].z << 12;
        D_80085224[g_fieldEntity.entityIndex[1]].triIdx = D_80070760[angle].unk6;
        D_80085224[g_fieldEntity.entityIndex[1]].unk258 = D_80070760[angle].unk8;
    }
}

/**
 * @brief Record entity position into both path tables and advance the path phase.
 *
 * Inverse of @c func_8009BB18: writes the entity's posX/posY/posZ (each
 * divided by 4096 for round-toward-zero fixed-point conversion), unk1FA
 * halfword, and two extra bytes (b9 at offset 9, b8 at offset 8) into the
 * current waypoint slot @c D_8005F144 of BOTH path tables (D_80070A60 and
 * D_80070760).
 *
 * If @p mode == 1, also advances the recorder:
 *   - increments @c D_8005F144 (mod 64),
 *   - nudges @c D_8005F118 by one toward @c D_8005F160 (target phase),
 *   - nudges @c D_8005F11A by one toward @c D_8005F162.
 *
 * Each xyz coordinate uses signed `/ 4096` (target compiles this as
 * `bgez; addiu +0xFFF; sra 12`, the round-toward-zero idiom).
 *
 * @param actor   Source entity providing posX/posY/posZ/unk1FA.
 * @param mode If 1, advance D_8005F144 and the phase counters.
 * @param b9  Byte stored at offset 9 of each waypoint.
 * @param b8  Byte stored at offset 8 of each waypoint.
 */
void func_8009BD50(Actor *actor, s16 mode, s8 b9, u8 b8) {
    PathEntry *base1 = D_80070760;
    PathEntry *p1 = base1 + D_8005F144;
    PathEntry *base0 = D_80070A60;
    PathEntry *p0 = base0 + D_8005F144;
    s16 v;
    u16 u;

    v = actor->posX / 4096;
    p0->x = v;
    p1->x = v;
    v = actor->posY / 4096;
    p0->y = v;
    p1->y = v;
    v = actor->posZ / 4096;
    p0->z = v;
    p1->z = v;
    u = actor->triIdx;
    p0->unk6 = u;
    p1->unk6 = u;
    p0->field_09 = b9;
    p1->field_09 = b9;
    {
        PathEntry *q0 = base0 + D_8005F144;
        PathEntry *q1 = base1 + D_8005F144;
        q0->unk8 = b8;
        q1->unk8 = b8;
    }

    if (mode == 1) {
        D_8005F144++;
        if (D_8005F144 == FIELD_PATH_RING_LEN) D_8005F144 = 0;

        if (D_8005F118 != D_8005F160) {
            if (D_8005F160 < D_8005F118) D_8005F118--;
            if (D_8005F118 < D_8005F160) D_8005F118++;
        }
        if (D_8005F11A != D_8005F162) {
            if (D_8005F162 < D_8005F11A) D_8005F11A--;
            if (D_8005F11A < D_8005F162) D_8005F11A++;
        }
    }
}

/**
 * @brief Per-frame movement and animation tick for every field entity.
 *
 * Runs nine sequential passes over the @c D_80085388 entities of @p ents:
 *  1. Re-arm each entity's @c unk258 tick flag.
 *  2. Advance the global heading lerp (@c unk100 -> @c unk102 over @c unk104
 *     frames) into @c g_fieldEntity.unk1A8.
 *  3. Advance each entity's heading lerp (@c field_0x244: 1 = linear,
 *     2 = sine, 3 = finished).
 *  4. Advance each entity's position-offset lerp (@c unk245, same encoding).
 *  5. Drive the player entity: read the analog stick, derive a heading from it
 *     (or from the d-pad bits when the stick is centred), pick a walk or run
 *     speed, step the entity with @c func_8009D598 on the scratchpad stack,
 *     dispatch the walk/run animation and record the step in the two
 *     breadcrumb rings the party followers replay from.
 *  6. @c msgActive 1: walk toward a scripted target (@c func_8009D274).
 *  7. @c msgActive 2: ballistic hop across a navmesh triangle.
 *  8. @c msgActive 4: hand the entity to @c func_8009FE18.
 *  9. @c msgActive 3: linear path move, player-steerable for the self entity.
 *
 * @param ents  Entity array (@c D_80085224).
 * @param flags Pad state for this frame; same bit layout as
 *              @c SystemState::padHeld (0x1000 = down, 0x2000 = right,
 *              0x4000 = up, 0x8000 = left, 0x10 = run). Bit 0x10 is updated
 *              in place from the analog stick's deflection.
 */
void func_8009BEC8(Actor *ents, s32 flags) {
    VECTOR a;
    VECTOR b;
    VECTOR pt;
    s32 dist[2];
    u8 dir;
    s32 i;
    s32 step;
    u8 heading;
    s32 trail;

    for (i = 0; i < D_80085388; i++) {
        ents[i].unk258 = 1;
    }

    if (g_fieldEntity.unk106 < g_fieldEntity.unk104) {
        g_fieldEntity.unk106++;
        g_fieldEntity.unk1A8 = func_800A0E54((s16)g_fieldEntity.unk100, (s16)g_fieldEntity.unk102,
                                          g_fieldEntity.unk104, g_fieldEntity.unk106);
        if (g_fieldEntity.unk104 == g_fieldEntity.unk106) {
            g_fieldEntity.unk100 = g_fieldEntity.unk1A8;
        }
    }

    for (i = 0; i < D_80085388; i++) {
        switch (ents[i].field_0x244) {
        case 0:
        case 3:
            break;
        case 1:
            ents[i].field_0x241 = func_800A0E54(ents[i].field_0x1DC, ents[i].field_0x1DE, ents[i].field_0x242, ents[i].field_0x243);
            if (ents[i].field_0x243 == ents[i].field_0x242) {
                ents[i].field_0x244 = 3;
            } else {
                ents[i].field_0x243++;
            }
            break;
        case 2:
            ents[i].field_0x241 = func_800A0EB8(ents[i].field_0x1DC, ents[i].field_0x1DE, ents[i].field_0x242, ents[i].field_0x243);
            if (ents[i].field_0x243 == ents[i].field_0x242) {
                ents[i].field_0x244 = 3;
            } else {
                ents[i].field_0x243++;
            }
            break;
        }
    }

    for (i = 0; i < D_80085388; i++) {
        switch (ents[i].unk245) {
        case 0:
        case 3:
            break;
        case 1:
            ents[i].posOfsX = func_800A0E54((s16)ents[i].field_0x1E2, (s16)ents[i].field_0x1E4, ents[i].field_0x1F2, ents[i].field_0x1F4);
            ents[i].posOfsY = func_800A0E54((s16)ents[i].field_0x1E8, (s16)ents[i].field_0x1EA, ents[i].field_0x1F2, ents[i].field_0x1F4);
            ents[i].posOfsZ = func_800A0E54((s16)ents[i].field_0x1EE, (s16)ents[i].field_0x1F0, ents[i].field_0x1F2, ents[i].field_0x1F4);
            if (ents[i].field_0x1F4 == ents[i].field_0x1F2) {
                ents[i].unk245 = 3;
            } else {
                ents[i].field_0x1F4++;
            }
            if (i == D_8005F148) {
                func_8009A8E0(D_8008538C);
            }
            break;
        case 2:
            ents[i].posOfsX = func_800A0EB8((s16)ents[i].field_0x1E2, (s16)ents[i].field_0x1E4, ents[i].field_0x1F2, ents[i].field_0x1F4);
            ents[i].posOfsY = func_800A0EB8((s16)ents[i].field_0x1E8, (s16)ents[i].field_0x1EA, ents[i].field_0x1F2, ents[i].field_0x1F4);
            ents[i].posOfsZ = func_800A0EB8((s16)ents[i].field_0x1EE, (s16)ents[i].field_0x1F0, ents[i].field_0x1F2, ents[i].field_0x1F4);
            if (ents[i].field_0x1F4 == ents[i].field_0x1F2) {
                ents[i].unk245 = 3;
            } else {
                ents[i].field_0x1F4++;
            }
            if (i == D_8005F148) {
                func_8009A8E0(D_8008538C);
            }
            break;
        }
    }


    for (i = 0; i < D_80085388; i++) {
        if (ents[i].msgActive != 0 || i != D_8005F148) {
            continue;
        }
        step = 0;
        g_fieldEntity.fieldStepDelta = 0;
        if (g_fieldEntity.unk015 == 1 || (s16)g_fieldEntity.dialogState == 4) {
            continue;
        }
        if (func_80027DB4(0, PAD_AXIS_X, 0) != -1) {
            b.vx = 0x80 - func_80027DB4(0, PAD_AXIS_X, 0);
            b.vy = func_80027DB4(0, PAD_AXIS_Y, 0) - 0x80;
            a.vx = 0;
            a.vy = 0;
            dir = func_8009A0E8((s32 *)&a, (s32 *)&b, dist);
        } else {
            dist[0] = 0xFFFF;
        }
        if ((u32)(dist[0] - 0x31) < 0xCF || (flags & (FIELD_PAD_YLOW | FIELD_PAD_XHIGH | FIELD_PAD_YHIGH | FIELD_PAD_XLOW))) {
            if ((u32)(dist[0] - 0x31) >= 0xCF) {
                if (flags & FIELD_PAD_YLOW) {
                    ents[i].unk23F = 0;
                    if (flags & FIELD_PAD_XLOW) {
                        ents[i].unk23F = 0x20;
                    }
                    if (flags & FIELD_PAD_XHIGH) {
                        ents[i].unk23F = 0xE0;
                    }
                } else if (flags & FIELD_PAD_YHIGH) {
                    ents[i].unk23F = 0x80;
                    if (flags & FIELD_PAD_XLOW) {
                        ents[i].unk23F = 0x60;
                    }
                    if (flags & FIELD_PAD_XHIGH) {
                        ents[i].unk23F = 0xA0;
                    }
                } else {
                    if (flags & FIELD_PAD_XHIGH) {
                        ents[i].unk23F = 0xC0;
                    }
                    if (flags & FIELD_PAD_XLOW) {
                        ents[i].unk23F = 0x40;
                    }
                }
                ents[i].unk23F += g_fieldEntity.unk1A8 + ents[i].headingBase;
            } else {
                if (!(flags & FIELD_PAD_WALK)) {
                    if (dist[0] >= 0x79) {
                        flags &= ~FIELD_PAD_WALK;
                    } else {
                        flags |= FIELD_PAD_WALK;
                    }
                }
                ents[i].unk23F = dir + (g_fieldEntity.unk1A8 + D_8005F0F8->slotHeadingBias[g_fieldEntity.unk1A6]);
            }
            if ((flags & FIELD_PAD_WALK) || g_fieldEntity.unk1A4 == 1) {
                if (func_800BE274() == 0) {
                    ents[D_8005F148].moveSpeed = (u32)(g_fieldEntity.unk00A * 0x4367) >> 7;
                } else {
                    ents[D_8005F148].moveSpeed = (u32)(g_fieldEntity.unk00A * 0x4367) >> 6;
                }
            } else {
                if (func_800BE274() == 0) {
                    ents[D_8005F148].moveSpeed = (u32)(g_fieldEntity.unk00A * 0x631F) >> 6;
                } else {
                    ents[D_8005F148].moveSpeed = (u32)(g_fieldEntity.unk00A * 0x631F) >> 5;
                }
            }
            SCRATCH_STACK_ENTER();
            step = func_8009D598((s16)i);
            SCRATCH_STACK_LEAVE();
            if (step == 1) {
                if ((flags & FIELD_PAD_WALK) || g_fieldEntity.unk1A4 == step) {
                    func_8009B4A8((s16)i, ents[i].field_0x250, 1, 1);
                    trail = D_8005F144;
                    D_80070A60[trail].field_0A = 0;
                    D_80070760[trail].field_0A = 0;
                    g_fieldEntity.fieldStepDelta = 3;
                } else {
                    func_8009B4A8((s16)i, ents[i].field_0x251, 1, 1);
                    trail = D_8005F144;
                    D_80070A60[trail].field_0A = step;
                    D_80070760[trail].field_0A = step;
                    g_fieldEntity.fieldStepDelta = 5;
                }
                func_8009B74C(2, (D_8005F144 - D_8005F11A) & FIELD_PATH_RING_MASK, D_80070A60, 1);
                func_8009B74C(1, (D_8005F144 - D_8005F118) & FIELD_PATH_RING_MASK, D_80070760, 1);
                heading = ents[i].unk23F;
                trail = D_8005F144;
                D_80070A60[trail].field_0B = heading;
                D_80070760[trail].field_0B = heading;
        } else {
            func_8009B4A8((s16)i, ents[i].field_0x24F, 0, 1);
            if (g_fieldEntity.entityIndex[2] != 0xFF) {
                func_8009B4A8(g_fieldEntity.entityIndex[2], ents[g_fieldEntity.entityIndex[2]].field_0x24F, 0, 1);
            }
            if (g_fieldEntity.entityIndex[1] != 0xFF) {
                func_8009B4A8(g_fieldEntity.entityIndex[1], ents[g_fieldEntity.entityIndex[1]].field_0x24F, 0, 1);
            }
        }
        if (ents[i].field_0x240 == 0) {
            ents[i].field_0x241 = ents[i].unk23F;
        }
        if (g_fieldEntity.mode != 1 && g_fieldEntity.mode != 7 && step == 1) {
            func_800A5D28();
        }
            func_8009BD50(&ents[i], step, 1, 1);
            func_8009BB18();
        } else {
            func_8009B4A8((s16)i, ents[i].field_0x24F, 0, 1);
            if (g_fieldEntity.entityIndex[2] != 0xFF) {
                func_8009B4A8(g_fieldEntity.entityIndex[2], ents[g_fieldEntity.entityIndex[2]].field_0x24F, 0, 1);
            }
            if (g_fieldEntity.entityIndex[1] != 0xFF) {
                func_8009B4A8(g_fieldEntity.entityIndex[1], ents[g_fieldEntity.entityIndex[1]].field_0x24F, 0, 1);
            }
            func_8009BD50(&ents[i], step, 1, 1);
            func_8009BB18();
        }
    }


    for (i = 0; i < D_80085388; i++) {
        if (ents[i].msgActive == 1 && g_fieldEntity.pad001 != 1) {
            ents[i].headingBase = 0;
            if (func_8009D274(&ents[i], ents[i].windowId) == 0) {
                ents[i].msgState = 2;
                ents[i].msgActive = 0;
            } else {
                ents[i].msgState = 1;
                SCRATCH_STACK_ENTER();
                func_8009D598((s16)i);
                SCRATCH_STACK_LEAVE();
                if (ents[i].field_0x240 == 0) {
                    ents[i].field_0x241 = ents[i].unk23F;
                }
            }
            if (i == D_8005F148) {
                func_8009A8E0(D_8008538C);
            }
        }
    }

    for (i = 0; i < D_80085388; i++) {
        if (ents[i].msgActive != 2) {
            continue;
        }
        if (ents[i].msgState == 0) {
            ents[i].headingBase = 0;
            ents[i].moveStartX = ents[i].posX;
            ents[i].moveStartY = ents[i].posY;
            ents[i].moveStartZ = ents[i].posZ;
            func_8009DED8((Vec3i *)&a, &D_800C71F0[ents[i].field_0x1FC * 3 + 1], &D_800C71F0[ents[i].field_0x1FC * 3]);
            func_8009DED8((Vec3i *)&b, &D_800C71F0[ents[i].field_0x1FC * 3 + 2], &D_800C71F0[ents[i].field_0x1FC * 3 + 1]);
            pt.vx = ents[i].msgTextPtr / 0x1000;
            pt.vy = ents[i].msgPosX / 0x1000;
            ents[i].msgPosY = func_8009E338((Vec3i *)&a, (Vec3i *)&b, (Vec3i *)&pt, &D_800C71F0[ents[i].field_0x1FC * 3]) << 12;
            ents[i].arcVelZ = (ents[i].msgPosY - ents[i].moveStartZ) / ents[i].field_0x1D8 - -(ents[i].field_0x1D8 * 0x3E80) / 2;
            ents[i].field_0x1DA = 0;
            ents[i].msgState = 1;
        } else {
            if (ents[i].field_0x1D8 == ents[i].field_0x1DA) {
                ents[i].msgState = 2;
                ents[i].triIdx = ents[i].field_0x1FC;
            } else {
                ents[i].field_0x1DA++;
                ents[i].posX = func_800A0E54(ents[i].moveStartX, ents[i].msgTextPtr, ents[i].field_0x1D8, ents[i].field_0x1DA);
                ents[i].posY = func_800A0E54(ents[i].moveStartY, ents[i].msgPosX, ents[i].field_0x1D8, ents[i].field_0x1DA);
                ents[i].posZ = ents[i].arcVelZ * ents[i].field_0x1DA + (ents[i].field_0x1DA * -(ents[i].field_0x1DA * 0x3E80)) / 2 + ents[i].moveStartZ;
            }
        }
        if (i == D_8005F148) {
            func_8009A8E0(D_8008538C);
        }
    }

    for (i = 0; i < D_80085388; i++) {
        if (ents[i].msgActive == 4) {
            if (i == D_8005F148) {
                func_8009A8E0(D_8008538C);
            }
            func_8009FE18(i, &ents[i], flags);
        }
    }

    for (i = 0; i < D_80085388; i++) {
        if (ents[i].msgActive != 3) {
            continue;
        }
        if (ents[i].msgState == 0) {
            ents[i].headingBase = 0;
            ents[i].moveStartX = ents[i].posX;
            ents[i].moveStartY = ents[i].posY;
            ents[i].moveStartZ = ents[i].posZ;
            a.vx = (ents[i].msgTextPtr - ents[i].moveStartX) / 1024;
            a.vy = (ents[i].msgPosX - ents[i].moveStartY) / 1024;
            a.vz = (ents[i].msgPosY - ents[i].moveStartZ) / 1024;
            dist[0] = SquareRoot0(a.vx * a.vx + a.vy * a.vy + a.vz * a.vz);
            ents[i].field_0x1D8 = dist[0] / g_fieldEntity.unk1AE;
            ents[i].field_0x1DA = 0;
            ents[i].msgState = 1;
            if (i == D_8005F148) {
                func_8009A8E0(D_8008538C);
            }
            continue;
        }
        if (i == D_8005F148 && g_fieldEntity.unk015 == 0) {
            if (ents[i].windowId == 0) {
                if (flags & (FIELD_PAD_YLOW | FIELD_PAD_XHIGH)) {
                    if (ents[i].field_0x1DA == 0) {
                        ents[i].msgState = 2;
                    } else {
                        ents[i].field_0x1DA--;
                        func_8009B4A8((s16)i, D_800D9630[i]->unk7E, 1, -1);
                    }
                }
                if (flags & FIELD_PAD_YHIGH) {
                    if (ents[i].field_0x1DA == ents[i].field_0x1D8) {
                        ents[i].msgState = 2;
                        ents[i].triIdx = ents[i].field_0x1FC;
                    } else {
                        ents[i].field_0x1DA++;
                        func_8009B4A8((s16)i, D_800D9630[i]->unk7E, 1, 1);
                    }
                }
            } else {
                if (flags & (FIELD_PAD_YHIGH | FIELD_PAD_XLOW)) {
                    if (ents[i].field_0x1DA == 0) {
                        ents[i].msgState = 2;
                    } else {
                        ents[i].field_0x1DA--;
                        func_8009B4A8((s16)i, D_800D9630[i]->unk7E, 1, -1);
                    }
                }
                if (flags & FIELD_PAD_YLOW) {
                    if (ents[i].field_0x1DA == ents[i].field_0x1D8) {
                        ents[i].msgState = 2;
                        ents[i].triIdx = ents[i].field_0x1FC;
                    } else {
                        ents[i].field_0x1DA++;
                        func_8009B4A8((s16)i, D_800D9630[i]->unk7E, 1, 1);
                    }
                }
            }
        } else if (ents[i].field_0x1DA == ents[i].field_0x1D8) {
            ents[i].msgState = 2;
            ents[i].triIdx = ents[i].field_0x1FC;
        } else {
            ents[i].field_0x1DA++;
            D_800D9630[i]->unk52 += ents[i].field_0x208;
            if (D_800D9630[i]->unk0C - 1 < D_800D9630[i]->unk52) {
                D_800D9630[i]->unk52 = 0;
            }
        }
        ents[i].posX = func_800A0E54(ents[i].moveStartX, ents[i].msgTextPtr, ents[i].field_0x1D8, ents[i].field_0x1DA);
        ents[i].posY = func_800A0E54(ents[i].moveStartY, ents[i].msgPosX, ents[i].field_0x1D8, ents[i].field_0x1DA);
        ents[i].posZ = func_800A0E54(ents[i].moveStartZ, ents[i].msgPosY, ents[i].field_0x1D8, ents[i].field_0x1DA);
    }
}

/**
 * @brief Aim the self entity at whichever entity is most directly "in front" of
 *        it, and stamp that target's delayed-SFX trigger 7 with the pad mode.
 *
 * The self entity is @c D_80085224[D_8005F148]; its position is taken in whole
 * units (fixed-point @c >>12). A @c mode (0/1/2) is derived from the current and
 * previous pad-held bits in @c g_fieldEntity (@c unk150 / @c unk154, bits @c 0x80
 * and @c 0x40). If the field is busy (@c D_800704BD != 0) or @c mode is 0, the
 * scan is skipped entirely.
 *
 * Otherwise every entity @c i is scored into @c angleTable: candidates are
 * skipped when they are the self entity, flagged busy (@c field_0x24B), inactive
 * (@c unk218 == -1), share the self's X/Y cell, lie outside a ±0xFF Z band, or
 * fall outside the combined talk/collision radius. For the rest, the score is how
 * far the entity's bearing (via @ref func_8009A0E8) deviates from the self's
 * facing angle (@c field_0x241), folded to 0..0x80. The entity with the smallest
 * deviation below the 0x40 threshold wins and has its @c triggerSfx7 set to
 * @c mode; a tie or no winner leaves everything untouched.
 */
void func_8009CEE8(void) {
    s32 selfPos[3];
    s32 entPos[3];
    s16 angleTable[48];
    s32 dist;
    SystemState *sys;
    s32 i;
    s32 mode;
    s32 diff;
    s16 best;
    s16 bestIdx;

    selfPos[0] = D_80085224[D_8005F148].posX >> 12;
    selfPos[1] = D_80085224[D_8005F148].posY >> 12;
    selfPos[2] = D_80085224[D_8005F148].posZ >> 12;

    sys = &g_fieldEntity;
    mode = 0;
    if (sys->unk150 & 0x80) {
        mode = ((sys->unk154 & 0x80) == 0) << 1;
    }
    if ((sys->unk150 & 0x40) && !(sys->unk154 & 0x40)) {
        mode = 1;
    }

    if (D_800704BD != 0) return;
    if (mode == 0) return;

    for (i = 0; i < D_80085388; i++) {
        angleTable[i] = 0x100;
        if (i == D_8005F148) continue;
        if (D_80085224[i].field_0x24B != 0) continue;
        if (D_80085224[i].unk218 == -1) continue;
        entPos[0] = D_80085224[i].posX >> 12;
        entPos[1] = D_80085224[i].posY >> 12;
        entPos[2] = D_80085224[i].posZ >> 12;
        if (selfPos[0] == entPos[0] && selfPos[1] == entPos[1]) continue;
        if ((u32)((selfPos[2] - entPos[2]) + 0xFF) >= 0x1FF) continue;
        diff = (D_80085224[D_8005F148].field_0x241 - (func_8009A0E8(selfPos, entPos, &dist) & 0xFF)) & 0xFF;
        angleTable[i] = diff;
        if (diff >= 0x81) {
            angleTable[i] = 0x100 - diff;
        }
        if (dist >= D_80085224[i].talkRadius + D_80085224[D_8005F148].radius) {
            angleTable[i] = 0x100;
        }
    }

    best = 0x40;
    bestIdx = D_8005F148;
    for (i = 0; i < D_80085388; i++) {
        if (D_80085224[i].unk218 == -1) continue;
        if (angleTable[i] < (s16)best) {
            best = (u16)angleTable[i];
            bestIdx = i;
        }
    }

    if (bestIdx != D_8005F148 && best != 0x40) {
        D_80085224[bestIdx].triggerSfx7 = mode;
    }
}

/**
 * Looks up a halfword from the D_800C32A0 table by index.
 *
 * @param a0 Table index (masked to 8 bits).
 * @return The halfword value at D_800C32A0[a0].
 */
s16 func_8009D234(s32 a0) {
    a0 &= 0xFF;
    return *(s16 *)(D_800C32A0 + a0 * 2);
}

/**
 * Looks up a halfword from the D_800C3320 table by index.
 *
 * @param a0 Table index (masked to 8 bits).
 * @return The halfword value at D_800C3320[a0].
 */
s16 func_8009D254(s32 a0) {
    a0 &= 0xFF;
    return *(s16 *)(D_800C3320 + a0 * 2);
}

/**
 * @brief Walk-toward-destination step: arrival test plus one turn increment.
 *
 * Snapshots the entity position and its destination (@c msgTextPtr /
 * @c msgPosX, which double as the walk target) in whole units and takes the
 * squared XY distance between them.
 *
 * When @p pad is non-zero the entity is treated as still travelling unless the
 * remaining squared distance exceeds @c (radius + pad)^2 + 0x1000, in which
 * case 0 is returned. Arrival, squared distance below @c moveSpeed^2 >> 16
 * or below 4, snaps the position onto the destination and returns 0.
 *
 * Otherwise the bearing to the destination is taken (@c func_8009A0E8, which
 * also rewrites @c dist with the true distance) and reduced by
 * @c headingBase to give the target heading. The turn rate is
 * @c field_0x262, or 0 when the entity is already inside its radius or
 * @c field_0x1DA has left the +/-0x100 band. With no rate the heading snaps;
 * otherwise the heading restarts from @c field_0x241 and is stepped by at most
 * @c rate along the shorter way around the 256-unit circle (the four cases
 * offset the compared angles by 0x100 to keep the window continuous across
 * the wrap), snapping when the target already lies inside the step window.
 *
 * @param actor Entity to advance.
 * @param pad  Extra arrival slack added to the radius; 0 disables the test.
 * @return 1 while still travelling, 0 once arrived (or stopped short).
 */
s32 func_8009D274(Actor *actor, s16 pad) {
    VECTOR cur;
    VECTOR dst;
    s32 dist;
    s32 dx;
    s32 dy;
    s32 r;
    s32 rr;
    s32 lim;
    s32 delta;
    u16 rate;
    u16 c;

    cur.vx = actor->posX >> 12;
    cur.vy = actor->posY >> 12;
    dst.vx = actor->msgTextPtr >> 12;
    dx = dst.vx - cur.vx;
    dst.vy = actor->msgPosX >> 12;
    dy = dst.vy - cur.vy;
    r = actor->radius + pad;
    rr = r * r;
    dist = dx * dx + dy * dy;
    lim = rr + 0x1000;
    if (pad != 0) {
        if (lim >= dist) {
            return 0;
        }
    }
    if (dist < (actor->moveSpeed * actor->moveSpeed) >> 16 || dist < 4) {
        actor->posX = actor->msgTextPtr;
        actor->posY = actor->msgPosX;
        return 0;
    }

    delta = (func_8009A0E8(&cur.vx, &dst.vx, &dist) & 0xFF) - actor->headingBase;
    if (dist < actor->radius || actor->field_0x1DA > 0x100 || actor->field_0x1DA < -0x100) {
        rate = 0;
    } else {
        rate = actor->field_0x262;
    }
    if (rate == 0) {
        actor->unk23F = delta;
    } else {
        actor->unk23F = actor->field_0x241;
        c = actor->unk23F;
        if (c == (u8)delta) {
            actor->unk23F = delta;
        } else if ((u16)delta < c) {
            if (c - (u16)delta >= 0x81) {
                delta += 0x100;
                if ((u16)delta < c + rate && c - rate < (u16)delta) {
                    actor->unk23F = delta;
                } else {
                    actor->unk23F += rate;
                    actor->field_0x1DA += rate;
                }
            } else {
                c += 0x100;
                delta += 0x100;
                if ((u16)delta < c + rate && c - rate < (u16)delta) {
                    actor->unk23F = delta;
                } else {
                    actor->unk23F -= rate;
                    actor->field_0x1DA -= rate;
                }
            }
        } else if ((u16)delta - c >= 0x81) {
            c += 0x100;
            if ((u16)delta < c + rate && c - rate < (u16)delta) {
                actor->unk23F = delta;
            } else {
                actor->unk23F -= rate;
                actor->field_0x1DA -= rate;
            }
        } else {
            c += 0x100;
            delta += 0x100;
            if ((u16)delta < c + rate && c - rate < (u16)delta) {
                actor->unk23F = delta;
            } else {
                actor->unk23F += rate;
                actor->field_0x1DA += rate;
            }
        }
    }
    return 1;
}

/**
 * @brief Movement work area in PSX scratchpad memory at @c getScratchAddr(16)
 *        (@c 0x1F800040), shared by @c func_8009D598 and @c func_8009D500.
 *
 * Six 16-byte @c VECTOR slots: the two navmesh-triangle edge vectors that
 * @c func_8009D598 feeds to the GTE @c OP (cross product) instruction, the
 * ground-plane normal that comes back out of it, the source and stepped
 * positions, and the step delta.
 */
typedef struct {
    /* 0x00 */ s32 e0x, e0y, e0z; /**< Triangle edge v1-v0; loaded as the GTE @c OP D-vector. */
    /* 0x0C */ s32 pad0C;
    /* 0x10 */ s32 e1x, e1y, e1z; /**< Triangle edge v2-v1; loaded as the GTE @c OP IR-vector. */
    /* 0x1C */ s32 pad1C;
    /* 0x20 */ s32 nx, ny, nz;    /**< Ground-plane normal (@c OP result), normalised and clamped. */
    /* 0x2C */ s32 pad2C;
    /* 0x30 */ s32 srcX;
    /* 0x34 */ s32 srcY;
    /* 0x38 */ s32 srcZ;        /**< Copied to @c outZ without delta. */
    /* 0x3C */ s32 pad3C;
    /* 0x40 */ s32 outX;        /**< @c outX = @c srcX + @c dx. */
    /* 0x44 */ s32 outY;        /**< @c outY = @c srcY + @c dy. */
    /* 0x48 */ s32 outZ;
    /* 0x4C */ s32 pad4C;
    /* 0x50 */ s32 dx;
    /* 0x54 */ s32 dy;
} FieldStepScratch;

/**
 * @brief Step a position by (@c dx, @c dy, @c 0) and check collision.
 *
 * Writes the stepped position into @c ctx->outX/outY/outZ, calls
 * @c func_8009DF18 to do the per-axis path/extent computation (its
 * return value is captured into @c *out, clever scheduling puts that
 * store in the @c jal @c func_8009E468 delay slot), then runs the
 * collision query @c func_8009E468 against the computed @c outX/Y/Z.
 *
 * @return @c 4 if @c func_8009E468 reported a hit, @c 0 otherwise.
 */
s32 func_8009D500(s32 selfIdx, s32 arg1, FieldStepScratch *ctx, s32 *out) {
    ctx->outX = ctx->srcX + ctx->dx;
    ctx->outY = ctx->srcY + ctx->dy;
    ctx->outZ = ctx->srcZ;
    *out = func_8009DF18(arg1, (Vec3i *)&ctx->outX, &ctx->dx, &ctx->e1x);
    return func_8009E468((s16)selfIdx, (Vec3i *)&ctx->outX) ? 4 : 0;
}

/**
 * @brief Advance one field entity by a step along its facing, sliding around
 *        obstacles by re-aiming until a clear direction is found.
 *
 * Runs once per tick for every walking entity. First it recovers the slope of
 * the navmesh triangle the entity is standing on: the two edge vectors of
 * triangle @c triIdx go through the GTE @c OP (cross product) instruction to
 * give the face normal, which is scaled down, normalised by @ref VectorNormal,
 * re-normalised against Z through @ref SquareRoot12 and clamped to +/-1.0
 * (@c 4096). The magnitudes of its X and Y components become the per-axis
 * scale of the step, so an entity walking across a steep face covers less
 * ground on that axis.
 *
 * It then loops, probing three directions per attempt with @ref func_8009D500 —
 * the facing rotated by @c +0x20, by @c -0x20, and straight ahead, each probed
 * at the entity's collision @c radius. The probes report both a blocking-edge
 * code (via the out parameters) and a push-back result (the return value). If
 * every probe is clear the loop stops and the step is committed. Otherwise the
 * facing is nudged and the probes are repeated: a blocked left flank turns the
 * entity right (@c -8), a blocked right flank turns it left (@c +8), and a
 * head-on block backs the facing off by the reported edge code. Both flanks
 * blocked means a dead end and the walk is abandoned. The player entity gets
 * @c 3 attempts while @c D_8005F102 is armed and @c 0x11 otherwise; every other
 * entity always gets @c 0x11.
 *
 * Whatever the outcome, @ref func_8009DF18 re-walks the navmesh from the
 * entity's triangle to the target point so @c triIdx follows the entity, and
 * for the player (when field control is enabled) the trigger scans run against
 * the new point: @ref func_8009A4C0 for the per-entity line triggers,
 * @ref func_8009AAC8 for the event queue, and @ref func_800A6100 for the field
 * line-trigger table.
 *
 * @param index Entity index into @ref D_80085224.
 * @return @c 1 if the entity moved (position written back), @c 0 if it was
 *         blocked or the navmesh walk failed.
 *
 * @note The event queue is handed to @ref func_8009AAC8 starting four bytes
 *       into entry 0, and that skew is real, @c func_8009AAC8's sentinel test
 *       (@c counter @c == @c 0x7FFF) then lands on @ref EventEntry::field16 and
 *       its armed test (@c spawnTriIdx @c == @c 0xFFFF) on @c field14, which is
 *       exactly what @c opHandler_PREMAPJUMP writes. The likely reading is that
 *       @ref EventEntry 's own field names are off by four and the trigger
 *       segment really starts at @c +0x04; @c opHandler_PREMAPJUMP pins the
 *       array base at @c 0x60, so the offsets are left as they are until a
 *       function that settles the entry layout is decompiled.
 */
s32 func_8009D598(s16 index) {
    FieldStepScratch *sc = (FieldStepScratch *)getScratchAddr(16);
    SVert *a;
    SVert *b;
    SVert *c;
    u16 tri;
    s16 self;
    s32 hitLeft;
    s32 hitRight;
    s32 hitFwd;
    s32 stepX;
    u32 attempt;
    s32 pushFwd;
    s32 pushRight;
    s32 pushLeft;
    s32 stepY;
    s32 blocked;
    /* Held as an integer so the vertex address is an int+int sum with the
       offset first, matching the original's addu operand order. */
    s32 vertBase;

    vertBase = (s32)D_800C71F0;
    tri = D_80085224[index].triIdx;
    a = (SVert *)(tri * 24 + vertBase);
    b = &a[1];
    c = &a[2];
    sc->e0x = b->sx - a->sx;
    sc->e0y = b->sy - a->sy;
    sc->e0z = b->sz - a->sz;
    sc->e1x = c->sx - b->sx;
    sc->e1y = c->sy - b->sy;
    sc->e1z = c->sz - b->sz;
    self = index;

    gte_ldopv1(sc);
    gte_ldopv2(getScratchAddr(20));
    gte_op0();
    gte_stlvnl(getScratchAddr(24));

    sc->nx /= 256;
    sc->ny /= 256;
    sc->nz /= 256;
    VectorNormal((VECTOR *)getScratchAddr(24), (VECTOR *)getScratchAddr(24));

    sc->nx = (sc->nz * 4096)
             / SquareRoot12(sc->nx * sc->nx / 4096 + sc->nz * sc->nz / 4096);
    sc->ny = (sc->nz * 4096)
             / SquareRoot12(sc->ny * sc->ny / 4096 + sc->nz * sc->nz / 4096);

    if (sc->nx > 4096) {
        sc->nx = 4096;
    } else if (sc->nx < -4096) {
        sc->nx = -4096;
    }
    if (sc->ny > 4096) {
        sc->ny = 4096;
    } else if (sc->ny < -4096) {
        sc->ny = -4096;
    }
    if (sc->nz > 4096) {
        sc->nz = 4096;
    } else if (sc->nz < -4096) {
        sc->nz = -4096;
    }

    stepX = sc->nx;
    if (stepX < 0) {
        stepX = -stepX;
    }
    stepY = sc->ny;
    if (stepY < 0) {
        stepY = -stepY;
    }

    attempt = 0;
    while (1) {
        attempt++;
        if (self == D_8005F148 && D_8005F102 == 1) {
            if (attempt >= 3) {
                D_8005F102 = 0;
                break;
            }
        } else if (attempt >= 17) {
            break;
        }

        sc->srcX = func_8009D234(D_80085224[self].unk23F) * stepX / 4096;
        sc->srcY = -(func_8009D254(D_80085224[self].unk23F) * stepY) / 4096;
        sc->srcX = sc->srcX * D_80085224[self].moveSpeed / 256;
        sc->srcY = sc->srcY * D_80085224[self].moveSpeed / 256;
        sc->srcX = D_80085224[self].posX + sc->srcX;
        sc->srcY = D_80085224[self].posY + sc->srcY;
        sc->srcZ = D_80085224[self].posZ;

        sc->dx = func_8009D234((u8)(D_80085224[self].unk23F + 0x20))
                 * D_80085224[self].radius;
        sc->dy = -func_8009D254((u8)(D_80085224[self].unk23F + 0x20))
                 * D_80085224[self].radius;
        pushLeft = func_8009D500(self, (s32)&tri, sc, &hitLeft);

        tri = D_80085224[self].triIdx;
        sc->dx = func_8009D234((u8)(D_80085224[self].unk23F - 0x20))
                 * D_80085224[self].radius;
        sc->dy = -func_8009D254((u8)(D_80085224[self].unk23F - 0x20))
                 * D_80085224[self].radius;
        pushRight = func_8009D500(self, (s32)&tri, sc, &hitRight);

        tri = D_80085224[self].triIdx;
        sc->dx = func_8009D234(D_80085224[self].unk23F) * D_80085224[self].radius;
        sc->dy = -func_8009D254(D_80085224[self].unk23F) * D_80085224[self].radius;
        pushFwd = func_8009D500(self, (s32)&tri, sc, &hitFwd);

        if (hitFwd == 0 && hitLeft == 0 && hitRight == 0 && pushFwd == 0
            && pushLeft == 0 && pushRight == 0) {
            break;
        }

        if (self == D_8005F148 && D_800704BD == 0) {
            if (pushFwd != 0 || pushLeft != 0 || pushRight != 0) {
                break;
            }
        } else if (hitFwd != 0 && hitLeft == 0 && hitRight == 0) {
            D_80085224[self].unk23F -= hitFwd;
        } else if (pushFwd != 0 && pushLeft == 0 && pushRight == 0) {
            D_80085224[self].unk23F -= pushFwd;
        }

        if (hitLeft != 0) {
            if (hitRight != 0) {
                break;
            }
            D_80085224[self].unk23F -= 8;
        } else if (pushLeft != 0) {
            D_80085224[self].unk23F -= 8;
        } else if (hitRight != 0 || pushRight != 0) {
            D_80085224[self].unk23F += 8;
        }
    }

    blocked = func_8009DF18(&D_80085224[self].triIdx, (Vec3i *)&sc->srcX, &sc->dx,
                            (s32 *)sc);
    if (self == D_8005F148 && g_fieldEntity.unk015 == 0) {
        func_8009A4C0(&D_80085224[self], D_8008538C, (VECTOR *)&sc->srcX);
        D_8005F102 = 0;
        if (g_fieldEntity.unk1A2 == 0) {
            func_8009AAC8(&D_80085224[self],
                          (EventEntry *)&D_8005F0F8->entries[0].z0,
                          (Vec3i *)&sc->srcX);
        }
        func_800A6100(&D_80085224[self], D_8005F0F8->segs, (Vec3i *)&sc->srcX);
    }

    if (hitFwd == 0 && hitLeft == 0 && hitRight == 0 && pushFwd == 0
        && pushLeft == 0 && pushRight == 0 && blocked == 0) {
        D_80085224[self].posX = sc->srcX;
        D_80085224[self].posY = sc->srcY;
        D_80085224[self].posZ = sc->srcZ << 12;
        return 1;
    }
    return 0;
}

/**
 * Subtracts two 3-component short vectors, storing result as words.
 *
 * @param out Destination edge vector (widened to words).
 * @param a   Source vertex A.
 * @param b   Source vertex B.
 */
void func_8009DED8(Vec3i *out, SVert *a, SVert *b) {
    out->x = a->sx - b->sx;
    out->y = a->sy - b->sy;
    out->z = a->sz - b->sz;
}

/**
 * @brief GTE outer-product (NCLIP) of the query point against one navmesh edge.
 *
 * Two separate asm statements on purpose: the compiler materializes the
 * result address (@c addiu @c vN,sp,K) between the @c nclip and the @c swc2,
 * and the @c "memory" clobber on the first forces the original's per-edge
 * reloads of the query point and mesh pointer. @c nclip has no GAS mnemonic,
 * hence the @c .word encoding (cop2 0x1400006).
 */
#define gte_nclip(out, sxy0, sxy1, sxy2) \
    __asm__ volatile("mtc2 %0, $12\nmtc2 %2, $14\nmtc2 %1, $13\nnop\nnop\n.word 0x4B400006\n" \
        : : "r"(sxy0), "r"(sxy1), "r"(sxy2) : "memory"); \
    __asm__ volatile("swc2 $24, 0(%0)\n" : : "r"(&(out)))

/**
 * @brief Walk the field navmesh from the current triangle toward a stepped
 *        position, following edge adjacency; classify any blocking edge.
 *
 * Rounds @p out 's fixed-point X/Y up toward zero (@c +0xFFF then @c >>12)
 * into a word vector and a packed SXY vertex, then loops: builds the three
 * edge vectors of the current triangle (@ref func_8009DED8), runs GTE NCLIP
 * of the query point against each directed edge, and
 *  - if the point is inside all three edges (all NCLIPs >= 0), stops and
 *    writes @c out->z from the triangle-plane intersection
 *    (@ref func_8009E338), returning 0 or the last edge classification;
 *  - otherwise, for the first failing edge: moves to that edge's neighbor
 *    triangle (@c *pTriIdx updated) when one exists and its
 *    @c g_fieldEntity.statusBits lock bit is clear, else returns +/-8 by the
 *    sign of the edge direction dotted with the movement delta @p dxy
 *    (which side of the blocking edge the motion crosses).
 *
 * @param pTriIdx In/out current triangle index into @c D_800C71F0.
 * @param out     Stepped position (fixed-point); @c z receives the plane height.
 * @param dxy     Movement delta (dx at [0], dy at [1]).
 * @param aux     Unused.
 * @return 0 when the point settled in a triangle, else +/-8 blocking-edge code.
 */
s32 func_8009DF18(u16 *pTriIdx, Vec3i *out, s32 *dxy, s32 *aux) {
    Vec3i e0;
    Vec3i e1;
    Vec3i e2;
    Vec3i posW;
    SVert posV;
    s32 nc0;
    s32 nc1;
    s32 nc2;
    s32 ret = 0;
    s32 t;
    s32 t2;
    s32 t3;
    s32 t4;
    s16 nb;
    s32 idx3;

    t = out->x;
    if (t < 0) {
        t += 0xFFF;
    }
    t >>= 12;
    posW.x = t;
    t2 = out->y;
    if (t2 < 0) {
        t2 += 0xFFF;
    }
    t2 >>= 12;
    posW.y = t2;
    posW.z = 0;
    t3 = out->x;
    if (t3 < 0) {
        t3 += 0xFFF;
    }
    t3 >>= 12;
    posV.sx = t3;
    t4 = out->y;
    if (t4 < 0) {
        t4 += 0xFFF;
    }
    posV.sy = t4 >> 12;
    posV.sz = 0;

    while (1) {
        func_8009DED8(&e0, &D_800C71F0[*pTriIdx * 3 + 1], &D_800C71F0[*pTriIdx * 3]);
        func_8009DED8(&e1, &D_800C71F0[*pTriIdx * 3 + 2], &D_800C71F0[*pTriIdx * 3 + 1]);
        func_8009DED8(&e2, &D_800C71F0[*pTriIdx * 3], &D_800C71F0[*pTriIdx * 3 + 2]);
        idx3 = *pTriIdx * 3;
        gte_nclip(nc0, *(u32 *)&posV, *(u32 *)&D_800C71F0[idx3 + 1], *(u32 *)&D_800C71F0[idx3]);
        idx3 = *pTriIdx * 3;
        gte_nclip(nc1, *(u32 *)&posV, *(u32 *)&D_800C71F0[idx3 + 2], *(u32 *)&D_800C71F0[idx3 + 1]);
        idx3 = *pTriIdx * 3;
        gte_nclip(nc2, *(u32 *)&posV, *(u32 *)&D_800C71F0[idx3], *(u32 *)&D_800C71F0[idx3 + 2]);
        if (nc0 >= 0 && nc1 >= 0 && nc2 >= 0) {
            break;
        }
        if (nc0 < 0) {
            nb = D_800D5E98[*pTriIdx].neighbor[0];
            if (nb >= 0 && !((g_fieldEntity.statusBits[nb >> 3] >> (nb - ((nb >> 3) << 3))) & 1)) {
                *pTriIdx = D_800D5E98[*pTriIdx].neighbor[0];
                continue;
            }
            ret = (e0.x * dxy[0] + e0.y * dxy[1] >= 0) ? 8 : -8;
            break;
        } else if (nc1 < 0) {
            nb = D_800D5E98[*pTriIdx].neighbor[1];
            if (nb >= 0 && !((g_fieldEntity.statusBits[nb >> 3] >> (nb - ((nb >> 3) << 3))) & 1)) {
                *pTriIdx = D_800D5E98[*pTriIdx].neighbor[1];
                continue;
            }
            ret = (e1.x * dxy[0] + e1.y * dxy[1] >= 0) ? 8 : -8;
            break;
        } else if (nc2 < 0) {
            nb = D_800D5E98[*pTriIdx].neighbor[2];
            if (nb >= 0 && !((g_fieldEntity.statusBits[nb >> 3] >> (nb - ((nb >> 3) << 3))) & 1)) {
                *pTriIdx = D_800D5E98[*pTriIdx].neighbor[2];
                continue;
            }
            ret = (e2.x * dxy[0] + e2.y * dxy[1] >= 0) ? 8 : -8;
            break;
        }
    }

    out->z = func_8009E338(&e0, &e1, &posW, (Vec3s *)&D_800C71F0[*pTriIdx * 3]);
    return ret;
}

/**
 * @brief Plane-cross intersection, compute @c (cross_xyz @c · (a3 @c -
 *        @c a2_partial)) @c / @c cross_z, where @c cross @c = @c a1 @c
 *        × @c a0.
 *
 * Builds the cross product @c a1 @c × @c a0 (stored to a stack array
 * @c sp[3]), then overwrites @c a0 with @c a3's sign-extended values.
 * The return value is the scalar projection of @c (a3 - a2_partial)
 * along the cross-product axis divided by @c cross_z. Note: @c a2's
 * @c z component is intentionally not subtracted.
 *
 * @param a0 Direction vector A (s32 x,y,z), overwritten with @c a3.
 * @param a1 Direction vector B (s32 x,y,z).
 * @param a2 Reference point (s32 x,y,z), only @c .x and @c .y used.
 * @param a3 Target point (s16 x,y,z).
 * @return The intersection parameter @c (s32).
 *
 * The trailing block caches @c sp[0..2] into local @c s32 vars (t0,
 * t1, t2) so gcc 2.7.2 loads each only once, without the cache, gcc
 * reloads them from the stack for each of the 5 uses.
 */
s32 func_8009E338(Vec3i *a0, Vec3i *a1, Vec3i *a2, Vec3s *a3) {
    s32 sp[3];

    sp[0] = -a0->y * a1->z + a1->y * a0->z;
    sp[1] = -a0->z * a1->x + a0->x * a1->z;
    sp[2] = -a0->x * a1->y + a1->x * a0->y;

    a0->x = a3->x;
    a0->y = a3->y;
    a0->z = a3->z;

    {
        s32 t0 = sp[0];
        s32 t1 = sp[1];
        s32 t2 = sp[2];
        return (t0 * a0->x + t1 * a0->y + t2 * a0->z
                - t0 * a2->x - t1 * a2->y) / t2;
    }
}

/**
 * @brief Test if @p selfIdx overlaps with any other active entity at world @p pos.
 *
 * Iterates over the @c D_80085224 entity table, skipping @p selfIdx itself
 * and any entity with @c unk218 == -1 (inactive). For each remaining entity,
 * a quick z-axis bounding-band check is applied (|dz| < 0x7E after shifting
 * the entity's @c posZ down by 12 fixed-point bits and subtracting @p pos->z),
 * then a 2D radius overlap test against the average of the two radii.
 *
 * Side effects:
 *   - When @p selfIdx matches the global player slot at @c D_8005F148, any
 *     overlapping entity with @c unk249 == 0 has its @c unk248 byte set to 1.
 *   - Whenever an overlap is found and the other entity's @c field_0x24C is
 *     zero, the function returns 1.
 *
 * @return 1 if any overlap was found, 0 otherwise (also 0 if @p selfIdx is
 *         itself inactive).
 */
s32 func_8009E468(s16 selfIdx, Vec3i *pos) {
    s32 selfRadius;
    s32 found = 0;
    s32 dx, dy, dz;
    s32 distSq, avgRadius;
    s16 i;

    selfRadius = D_80085224[selfIdx].radius;
    if (D_80085224[selfIdx].unk218 != -1) {
        for (i = 0; i < D_80085388; i++) {
            if (i == selfIdx) continue;
            if (D_80085224[i].unk218 == -1) continue;
            dz = (D_80085224[i].posZ >> 12) - pos->z;
            if ((u32)(dz + 0x7E) >= 0xFE) continue;
            dx = (D_80085224[i].posX - pos->x) >> 12;
            dy = (D_80085224[i].posY - pos->y) >> 12;
            avgRadius = (selfRadius + D_80085224[i].radius) >> 1;
            distSq = dx * dx + dy * dy;
            avgRadius *= avgRadius;
            if (distSq >= avgRadius) continue;
            if (selfIdx == D_8005F148) {
                if (D_80085224[i].unk249 == 0) {
                    D_80085224[i].unk248 = 1;
                }
            }
            if (D_80085224[i].field_0x24C == 0) found = 1;
        }
    }
    return found;
}

/**
 * Extracts position data from two entity structures (offsets 0x190/0x194,
 * right-shifted by 12) and calls func_8009A0E8 with them.
 *
 * @param a First entity pointer.
 * @param a Second entity pointer.
 */
s32 func_8009E604(Actor *a, Actor *b) {
    s32 pos1[4];
    s32 pos2[4];
    s32 result[2];

    pos1[0] = a->posX >> 12;
    pos1[1] = a->posY >> 12;
    pos2[0] = b->posX >> 12;
    pos2[1] = b->posY >> 12;
    return func_8009A0E8(pos1, pos2, result);
}

/**
 * @brief Reset both follower path tables to the player's position on field entry.
 *
 * @c D_80070A60 and @c D_80070760 are the 64-entry breadcrumb rings the two
 * party followers walk along: @c D_8009B74C writes the player's current
 * waypoint at cursor @c D_8005F144 each step, and each follower samples the
 * ring at @c (cursor @c - @c lag), taken modulo @c FIELD_PATH_RING_LEN. Entering a field leaves the ring
 * with no history, so it is refilled here. Every waypoint is marked walkable
 * (@c field_09 / @c unk8 @c = @c 1) and the two follower lag distances are reset
 * to their defaults, current (@c D_8005F118 / @c D_8005F11A) and target
 * (@c D_8005F160 / @c D_8005F162) alike.
 *
 * On a fresh load (@c D_8005F14C @c == @c 0) there is no heading to trail along,
 * so every waypoint just gets the player's position, triangle and facing, the
 * followers stand on top of the player until real movement fills the ring.
 *
 * Otherwise the trail is synthesised backwards from the player. Slot 0 holds the
 * exact position and slots 63..1 each step one trail spacing further back along
 * the player's facing, the step being @c sin / @c cos (@ref func_8009D234 /
 * @ref func_8009D254) scaled by @c SystemState::unk00A. Each stepped point is
 * snapped onto the navmesh by @ref func_8009AC9C and its Z read off that
 * triangle's plane (@ref func_8009DED8 twice, then @ref func_8009E338); if the
 * plane Z lands further than @c 0x136 from the stepped Z the point is off the
 * walkable surface and the slot falls back to the player's own X/Y.
 *
 * @note Slot 0 only receives X and Y here, its Z, triangle and facing keep
 *       whatever the ring already held.
 */
void func_8009E660(void) {
    Vec3i edge0;
    Vec3i edge1;
    Vec3i pos;
    s32 x;
    s32 y;
    s32 z;
    s16 px;
    s16 py;
    s32 pz;
    s32 planeZ;
    s16 i;

    D_8005F144 = 0;
    D_8005F118 = 15;
    D_8005F11A = 30;
    D_8005F160 = 15;
    D_8005F162 = 30;

    for (i = 0; i < 64; i++) {
        D_80070760[i].field_09 = D_80070A60[i].field_09 = 1;
        D_80070760[i].unk8 = D_80070A60[i].unk8 = 1;
    }

    if (D_8005F14C == 0) {
        for (i = 0; i < 64; i++) {
            D_80070760[i].x = D_80070A60[i].x = D_80085224[D_8005F148].posX / 4096;
            D_80070760[i].y = D_80070A60[i].y = D_80085224[D_8005F148].posY / 4096;
            D_80070760[i].z = D_80070A60[i].z = D_80085224[D_8005F148].posZ / 4096;
            D_80070760[i].unk6 = D_80070A60[i].unk6 = D_80085224[D_8005F148].triIdx;
            D_80070760[i].field_0A = D_80070A60[i].field_0A = 0;
            D_80070760[i].field_0B = D_80070A60[i].field_0B = D_80085224[D_8005F148].field_0x241;
        }
    } else {
        x = D_80085224[D_8005F148].posX;
        y = D_80085224[D_8005F148].posY;
        z = D_80085224[D_8005F148].posZ;
        D_80070760[0].x = D_80070A60[0].x = D_80085224[D_8005F148].posX / 4096;
        D_80070760[0].y = D_80070A60[0].y = D_80085224[D_8005F148].posY / 4096;

        for (i = 0; i < 63; i++) {
            pos.x = px = D_80070760[63 - i].x = D_80070A60[63 - i].x = x / 4096;
            pos.y = py = D_80070760[63 - i].y = D_80070A60[63 - i].y = y / 4096;
            pz = z / 4096;
            D_80070760[63 - i].unk6 = D_80070A60[63 - i].unk6 =
                func_8009AC9C(px, py, pz, *D_800C7204);
            func_8009DED8(&edge0, &D_800C71F0[D_80070760[63 - i].unk6 * 3 + 1],
                          &D_800C71F0[D_80070760[63 - i].unk6 * 3]);
            func_8009DED8(&edge1, &D_800C71F0[D_80070760[63 - i].unk6 * 3 + 2],
                          &D_800C71F0[D_80070760[63 - i].unk6 * 3 + 1]);
            planeZ = func_8009E338(&edge0, &edge1, &pos,
                                   (Vec3s *)&D_800C71F0[D_80070760[63 - i].unk6 * 3]);
            if (planeZ < pz + 0x136 && pz - 0x136 < planeZ) {
                D_80070760[63 - i].z = D_80070A60[63 - i].z = planeZ;
            } else {
                D_80070760[63 - i].x = D_80070A60[63 - i].x =
                    D_80085224[D_8005F148].posX / 4096;
                D_80070760[63 - i].y = D_80070A60[63 - i].y =
                    D_80085224[D_8005F148].posY / 4096;
                D_80070760[63 - i].z = D_80070A60[63 - i].z = z / 4096;
            }
            D_80070760[63 - i].field_0A = D_80070A60[63 - i].field_0A = 0;
            D_80070760[63 - i].field_0B = D_80070A60[63 - i].field_0B =
                D_80085224[D_8005F148].field_0x241;
            /* FIELD_CHANNEL_SCALE * 4 >> 9 is the same trail spacing func_800B6738
               applies to D_800704B2; written *4 >> 9 because that is the shift pair
               the original emits. */
            x -= func_8009D234(D_80085224[D_8005F148].field_0x241) *
                 ((g_fieldEntity.unk00A * (FIELD_CHANNEL_SCALE * 4)) >> 9) / 256;
            y -= -(func_8009D254(D_80085224[D_8005F148].field_0x241) *
                   ((g_fieldEntity.unk00A * (FIELD_CHANNEL_SCALE * 4)) >> 9)) / 256;
        }
    }
}

/**
 * @brief Seed both follower breadcrumb trails with a synthetic path running
 *        from the party leader back to where each follower currently stands.
 *
 * Called when the party is (re)placed, after a map jump or a cutscene, so the
 * two followers have a trail to walk instead of snapping to the leader. Both
 * 64-slot tables are cleared, the ring cursor @c D_8005F144 is reset to 0 and
 * the two followers are re-pegged to their default lag of 15 and 30 slots
 * (@c D_8005F118 / @c D_8005F11A, mirrored into @c D_8005F160 / @c D_8005F162).
 *
 * For each follower it takes the bearing and distance from the follower to the
 * leader with @ref func_8009A0E8, converts the distance into a slot count
 * (@c dist scaled by the same @c FIELD_CHANNEL_SCALE step the walk code uses),
 * then walks backwards from the leader laying one waypoint per slot: slot 63 is
 * the leader, and each step moves one stride along the bearing towards the
 * follower. Every waypoint gets its navmesh triangle resolved by
 * @ref func_8009AC9C and its height linearly interpolated between the leader's
 * and the follower's. Any slots left over (down to slot 32) are filled with the
 * follower's own resting position and tagged @c field_0A @c = @c 2.
 *
 * A follower further than 32 slots from the leader is skipped entirely, its
 * table keeps the cleared state.
 *
 * @note @c D_80070760 holds slot 1's trail and @c D_80070A60 slot 2's; the
 *       defaults of 15 and 30 both land inside the 32..63 range this seeds.
 */
void func_8009ECA4(void) {
    s32 i;
    VECTOR lead;
    VECTOR trail1;
    VECTOR trail2;
    s32 slots1;
    s32 slots2;
    s32 dir1;
    s32 dir2;
    s32 stride;

    for (i = 0; i < 64; i++) {
        D_80070760[i].field_09 = D_80070A60[i].field_09 = 1;
        D_80070760[i].unk8 = D_80070A60[i].unk8 = 1;
    }
    D_8005F144 = 0;
    D_8005F118 = 15;
    D_8005F11A = 30;
    D_8005F160 = 15;
    D_8005F162 = 30;

    lead.vx = D_80085224[g_fieldVars->memberSlot[0]].posX / 4096;
    trail1.vx = D_80085224[g_fieldVars->memberSlot[1]].posX / 4096;
    trail2.vx = D_80085224[g_fieldVars->memberSlot[2]].posX / 4096;
    lead.vy = D_80085224[g_fieldVars->memberSlot[0]].posY / 4096;
    trail1.vy = D_80085224[g_fieldVars->memberSlot[1]].posY / 4096;
    trail2.vy = D_80085224[g_fieldVars->memberSlot[2]].posY / 4096;

    stride = (g_fieldEntity.unk00A * (FIELD_CHANNEL_SCALE * 4)) >> 9;
    dir1 = func_8009A0E8(&trail1.vx, &lead.vx, &slots1);
    slots1 = (slots1 << 8) / stride;
    dir2 = func_8009A0E8(&trail2.vx, &lead.vx, &slots2);
    slots2 = (slots2 << 8) / stride;

    trail1.vx = trail2.vx = D_80085224[g_fieldVars->memberSlot[0]].posX;
    trail1.vy = trail2.vy = D_80085224[g_fieldVars->memberSlot[0]].posY;
    trail1.vz = trail2.vz = D_80085224[g_fieldVars->memberSlot[0]].posZ;

    if (slots1 < 32) {
        for (i = 0; i < slots1; i++) {
            D_80070760[63 - i].x = trail1.vx / 4096;
            D_80070760[63 - i].y = trail1.vy / 4096;
            D_80070760[63 - i].unk6 =
                func_8009AC9C((s16)(trail1.vx / 4096), (s16)(trail1.vy / 4096),
                              (s16)(trail1.vz / 4096), *D_800C7204);
            trail1.vx -= func_8009D234((u8)dir1)
                         * ((g_fieldEntity.unk00A * (FIELD_CHANNEL_SCALE * 4)) >> 9)
                         / 256;
            trail1.vy -= -(func_8009D254((u8)dir1)
                           * ((g_fieldEntity.unk00A * (FIELD_CHANNEL_SCALE * 4))
                              >> 9))
                         / 256;
            D_80070760[63 - i].z =
                D_80085224[g_fieldVars->memberSlot[0]].posZ / 4096
                - (D_80085224[g_fieldVars->memberSlot[0]].posZ
                   - D_80085224[g_fieldVars->memberSlot[1]].posZ)
                          / 4096 * i / slots1;
            D_80070760[63 - i].field_0A = 0;
            D_80070760[63 - i].field_0B = dir1;
        }
        for (i = slots1; i < 32; i++) {
            D_80070760[63 - i].x =
                D_80085224[g_fieldVars->memberSlot[1]].posX / 4096;
            D_80070760[63 - i].y =
                D_80085224[g_fieldVars->memberSlot[1]].posY / 4096;
            D_80070760[63 - i].unk6 = func_8009AC9C(
                (s16)(D_80085224[g_fieldVars->memberSlot[1]].posX / 4096),
                (s16)(D_80085224[g_fieldVars->memberSlot[1]].posY / 4096),
                (s16)(D_80085224[g_fieldVars->memberSlot[1]].posZ / 4096),
                *D_800C7204);
            D_80070760[63 - i].z =
                D_80085224[g_fieldVars->memberSlot[1]].posZ / 4096;
            D_80070760[63 - i].field_0A = 2;
            D_80070760[63 - i].field_0B = dir1;
        }
    }

    if (slots2 < 32) {
        for (i = 0; i < slots2; i++) {
            D_80070A60[63 - i].x = trail2.vx / 4096;
            D_80070A60[63 - i].y = trail2.vy / 4096;
            D_80070A60[63 - i].unk6 =
                func_8009AC9C((s16)(trail2.vx / 4096), (s16)(trail2.vy / 4096),
                              (s16)(trail2.vz / 4096), *D_800C7204);
            trail2.vx -= func_8009D234((u8)dir2)
                         * ((g_fieldEntity.unk00A * (FIELD_CHANNEL_SCALE * 4)) >> 9)
                         / 256;
            trail2.vy -= -(func_8009D254((u8)dir2)
                           * ((g_fieldEntity.unk00A * (FIELD_CHANNEL_SCALE * 4))
                              >> 9))
                         / 256;
            D_80070A60[63 - i].z =
                D_80085224[g_fieldVars->memberSlot[0]].posZ / 4096
                - (D_80085224[g_fieldVars->memberSlot[0]].posZ
                   - D_80085224[g_fieldVars->memberSlot[2]].posZ)
                          / 4096 * i / slots2;
            D_80070A60[63 - i].field_0A = 0;
            D_80070A60[63 - i].field_0B = dir2;
        }
        for (i = slots2; i < 32; i++) {
            D_80070A60[63 - i].x =
                D_80085224[g_fieldVars->memberSlot[2]].posX / 4096;
            D_80070A60[63 - i].y =
                D_80085224[g_fieldVars->memberSlot[2]].posY / 4096;
            D_80070A60[63 - i].unk6 = func_8009AC9C(
                (s16)(D_80085224[g_fieldVars->memberSlot[2]].posX / 4096),
                (s16)(D_80085224[g_fieldVars->memberSlot[2]].posY / 4096),
                (s16)(D_80085224[g_fieldVars->memberSlot[2]].posZ / 4096),
                *D_800C7204);
            D_80070A60[63 - i].z =
                D_80085224[g_fieldVars->memberSlot[2]].posZ / 4096;
            D_80070A60[63 - i].field_0A = 2;
            D_80070A60[63 - i].field_0B = dir2;
        }
    }
}

/**
 * @brief Asymmetric overlap test between two Actor entities.
 *
 * Checks whether entity @p b is within entity @p a 's talk-trigger area:
 *   - Z-axis separation must satisfy @c -126 <= (b->posZ - a->posZ)/4096 <= 127.
 *   - XY-distance squared must be less than @c (a->talkRadius + b->radius) squared.
 *
 * Asymmetric: @p a contributes the @c talkRadius (0x1F8) while @p b
 * contributes the @c radius (0x1F6).
 *
 * The 2-iteration outer loop is a quirk of the original source: nothing
 * inside the body depends on @c i, and the Z check is loop-invariant,
 * so the body either succeeds on both iterations or fails on both.
 * Preserved here for byte-match with the original binary.
 *
 * @note The variable-reuse pattern (@c r used as a scratch for the
 *       @c dz pre-shift, then reassigned inside the loop; the chained
 *       @c r = (dy = r * r) inside the comparison) is what coaxes
 *       gcc 2.7.2 into the exact register allocation the original
 *       used. Not "natural" C, it's a deliberate trick that survived
 *       to match.
 */
s32 func_8009F74C(Actor *a, Actor *b) {
    s32 dz;
    s16 i;
    s32 dx;
    s32 dy;
    s32 r;
    r = (b->posZ - a->posZ) >> 12;
    dz = r;
    r = a->talkRadius + r;
    for (i = 0; i < 2; i++) {
        if ((dz < (-126)) || (dz > 127)) {
            continue;
        }
        dx = (b->posX - a->posX) >> 12;
        dy = (b->posY - a->posY) >> 12;
        r = b->radius;
        r = a->talkRadius + r;
        if (((dx * dx) + (dy * dy)) >= (r = (dy = r * r))) {
            continue;
        }
        return 1;
    }
    return 0;
}

/**
 * @brief Compute a move/turn vector and record it as a waypoint.
 *
 * Dispatches @c func_8009B4A8 with one of three calling conventions
 * driven by the signed @p sign byte:
 *   - @c sign == 0 → @c (idx, b, 0,  0)  (no move)
 *   - @c sign  > 0 → @c (idx, b, 1,  1)  (forward step)
 *   - @c sign  < 0 → @c (idx, b, 1, -1)  (reverse step)
 *
 * After that, calls @c func_8009BD50 (path recorder) on the Actor
 * @c D_80085224[idx] with the requested @c mode and @c sign, then
 * @c func_8009BB18 to publish the resulting waypoint.
 */
void func_8009F7F4(s16 idx, s8 sign, u8 b, s16 mode) {
    if (sign == 0) {
        func_8009B4A8(idx, b, 0, 0);
    } else if (sign > 0) {
        func_8009B4A8(idx, b, 1, 1);
    } else {
        func_8009B4A8(idx, b, 1, -1);
    }
    func_8009BD50(&D_80085224[idx], mode, sign, 0);
    func_8009BB18();
}

/**
 * @brief Interpolate an Actor's X/Y/Z position via the safe-lerp helper.
 *
 * Looks up @c D_80085224[idx] and runs @c func_800A0E54 three times to
 * compute @c posX / @c posY / @c posZ from the unk1A8/AC/B0 endpoints
 * and field_0x1C0/C4/C8 targets, using field_0x1D8/DA as the total/step.
 *
 * @note Uses unsubscripted @c D_80085224 to defeat gcc 2.7.2's CSE
 *       of the global pointer load (target reloads after each call).
 */
void func_8009F8D0(s16 idx) {
    D_80085224[idx].posX = func_800A0E54(D_80085224[idx].unk1A8,
                                         D_80085224[idx].field_0x1C0,
                                         (s16)D_80085224[idx].field_0x1D8,
                                         (s16)D_80085224[idx].field_0x1DA);
    D_80085224[idx].posY = func_800A0E54(D_80085224[idx].unk1AC,
                                         D_80085224[idx].field_0x1C4,
                                         (s16)D_80085224[idx].field_0x1D8,
                                         (s16)D_80085224[idx].field_0x1DA);
    D_80085224[idx].posZ = func_800A0E54(D_80085224[idx].unk1B0,
                                         D_80085224[idx].field_0x1C8,
                                         (s16)D_80085224[idx].field_0x1D8,
                                         (s16)D_80085224[idx].field_0x1DA);
}

/**
 * @brief Apply one frame of directional input to the player entity.
 *
 * Only the player (@c D_8005F148) moves, and only while @c D_800704BD is
 * clear; any other entity index, or a set @c D_800704BD, falls through to
 * the passive path at the end.
 *
 * The two direction bit-pairs are handled as a pair of opposite steps whose
 * meaning flips with the entity's message state: with @c windowId @c == @c 1
 * the @c 0xC000 bits step one way and @c 0x3000 the other, and without it the
 * assignment is reversed. Either way the step seeds both path tables at the
 * current write cursor @c D_8005F144 (@c field_0A @c = @c 4 marks the entry a
 * walk step, @c field_0B carries the entity's heading), runs the step through
 * @c func_8009F7F4 / @c func_8009F8D0, and replays both tables into the anim
 * system at their phase offsets (@c func_8009B74C). The two directions then
 * differ only in how they age @c field_0x1DA: one counts it down and reports
 * message state 5 at zero, the other counts it up and reports state 4 on
 * reaching @c field_0x1D8. With no direction bits the step is issued neutral.
 *
 * The passive path just ages @c field_0x1DA toward @c field_0x1D8 (reporting
 * state 2 once there) while advancing the entity's render-slot motion counter
 * @c unk52 by @c field_0x208, wrapping it at @c unk0C.
 *
 * @param idx   Entity index.
 * @param flags Input bits; @c 0x3000 and @c 0xC000 select the two directions.
 *
 * @note The two @c field_0A / @c field_0B stores are chained assignments: the
 *       plain two-statement form lets gcc merge the four otherwise identical
 *       table-seeding blocks, which collapses the shared step tails.
 */
void func_8009F990(s16 idx, s32 flags) {
    s32 p;
    u8 b;

    if (idx == D_8005F148 && D_800704BD == 0) {
        if (D_80085224[idx].windowId == 1) {
            if (flags & 0xC000) {
                p = D_8005F144;
                D_80070760[p].field_0A = D_80070A60[p].field_0A = 4;
                p = D_8005F144;
                D_80070760[p].field_0B = D_80070A60[p].field_0B = D_80085224[idx].field_0x241;
                func_8009F7F4(idx, -1, D_80085224[idx].field_0x253, 1);
                func_8009F8D0(idx);
                func_8009B74C(2, (D_8005F144 - D_8005F11A) & FIELD_PATH_RING_MASK, D_80070A60, 1);
                func_8009B74C(1, (D_8005F144 - D_8005F118) & FIELD_PATH_RING_MASK, D_80070760, 1);
                D_80085224[idx].field_0x1DA--;
                if (D_80085224[idx].field_0x1DA < 0) {
                    D_80085224[idx].field_0x1DA = 0;
                    D_80085224[idx].msgState = 5;
                }
            } else if (flags & 0x3000) {
                p = D_8005F144;
                D_80070760[p].field_0A = D_80070A60[p].field_0A = 4;
                p = D_8005F144;
                D_80070760[p].field_0B = D_80070A60[p].field_0B = D_80085224[idx].field_0x241;
                func_8009F7F4(idx, 1, D_80085224[idx].field_0x253, 1);
                func_8009F8D0(idx);
                func_8009B74C(2, (D_8005F144 - D_8005F11A) & FIELD_PATH_RING_MASK, D_80070A60, 1);
                func_8009B74C(1, (D_8005F144 - D_8005F118) & FIELD_PATH_RING_MASK, D_80070760, 1);
                D_80085224[idx].field_0x1DA++;
                if (D_80085224[idx].field_0x1DA == D_80085224[idx].field_0x1D8) {
                    D_80085224[idx].field_0x1DA = 0;
                    D_80085224[idx].msgState = 4;
                }
            } else {
                func_8009F7F4(idx, 0, D_80085224[idx].field_0x253, 0);
            }
        } else {
            if (flags & 0x3000) {
                p = D_8005F144;
                D_80070760[p].field_0A = D_80070A60[p].field_0A = 4;
                p = D_8005F144;
                D_80070760[p].field_0B = D_80070A60[p].field_0B = D_80085224[idx].field_0x241;
                func_8009F7F4(idx, 1, D_80085224[idx].field_0x253, 1);
                func_8009F8D0(idx);
                func_8009B74C(2, (D_8005F144 - D_8005F11A) & FIELD_PATH_RING_MASK, D_80070A60, 1);
                func_8009B74C(1, (D_8005F144 - D_8005F118) & FIELD_PATH_RING_MASK, D_80070760, 1);
                D_80085224[idx].field_0x1DA--;
                if (D_80085224[idx].field_0x1DA < 0) {
                    D_80085224[idx].field_0x1DA = 0;
                    D_80085224[idx].msgState = 5;
                }
            } else if (flags & 0xC000) {
                p = D_8005F144;
                D_80070760[p].field_0A = D_80070A60[p].field_0A = 4;
                p = D_8005F144;
                D_80070760[p].field_0B = D_80070A60[p].field_0B = D_80085224[idx].field_0x241;
                func_8009F7F4(idx, -1, D_80085224[idx].field_0x253, 1);
                func_8009F8D0(idx);
                func_8009B74C(2, (D_8005F144 - D_8005F11A) & FIELD_PATH_RING_MASK, D_80070A60, 1);
                func_8009B74C(1, (D_8005F144 - D_8005F118) & FIELD_PATH_RING_MASK, D_80070760, 1);
                D_80085224[idx].field_0x1DA++;
                if (D_80085224[idx].field_0x1DA == D_80085224[idx].field_0x1D8) {
                    D_80085224[idx].field_0x1DA = 0;
                    D_80085224[idx].msgState = 4;
                }
            } else {
                func_8009F7F4(idx, 0, D_80085224[idx].field_0x253, 0);
            }
        }
    } else {
        if (D_80085224[idx].field_0x1DA == D_80085224[idx].field_0x1D8) {
            D_80085224[idx].msgState = 2;
        } else {
            D_80085224[idx].field_0x1DA++;
            D_800D9630[idx]->unk52 += D_80085224[idx].field_0x208;
            if (D_800D9630[idx]->unk0C - 1 < D_800D9630[idx]->unk52) {
                D_800D9630[idx]->unk52 = 0;
            }
            func_8009F8D0(idx);
        }
    }
}

/**
 * @brief Advance one entity through the scripted "climb/step onto a ledge"
 *        movement state machine.
 *
 * Dispatches on @c Actor::msgState and, for the moving states, interpolates the
 * entity between two of its stored endpoints while seeding both breadcrumb path
 * tables at the current write cursor @c D_8005F144 (@c field_0A marks the entry
 * type, @c field_0B carries the heading) and replaying them into the animation
 * system at their phase offsets (@c func_8009B74C).
 *
 * States:
 *  - 0: latch the current position as the move origin, widen the two camera
 *       tracking limits, then fall through to state 3.
 *  - 3: step from the origin toward @c unk1A8..@c unk1B0. On arrival, derive the
 *       next leg's duration from the remaining distance and hand over to state 1.
 *  - 1: delegate to @c func_8009F990 (player-steerable walk).
 *  - 2: idle -- the movement is finished.
 *  - 4: step from @c field_0x1C0..@c field_0x1C8 toward @c msgTextPtr..@c msgPosY;
 *       on arrival latch the destination triangle and go to state 6.
 *  - 5: the reverse of state 3; on arrival go to state 6.
 *  - 6: settle in place, narrow the camera limits, and pick a hold time from how
 *       many party followers are present. On expiry go to state 2.
 *
 * @param entIdx Entity index; @c ent is @c D_80085224[entIdx].
 * @param actor    The entity to advance.
 * @param flags  Pad state, forwarded to @c func_8009F990 in state 1.
 *
 * @note @c idx is a working copy of @p entIdx; every use is a 16-bit narrowing,
 *       so the parameter itself is only read once.
 */
void func_8009FE18(s32 entIdx, Actor *actor, s32 flags) {
    s32 idx;
    VECTOR d;
    u16 p;

    idx = entIdx;
    actor->unk258 = 0;
    switch (actor->msgState) {
    case 0:
        actor->headingBase = 0;
        D_8005F160 = 31;
        D_8005F162 = 62;
        actor->moveStartX = actor->posX;
        actor->moveStartY = actor->posY;
        actor->moveStartZ = actor->posZ;
        actor->field_0x1DA = 0;
        actor->msgState = 3;
        /* fallthrough: run the first step of state 3 on this same frame */
    case 3:
        if (actor->windowId == 1) {
            p = D_8005F144;
            D_80070760[p].field_0A = D_80070A60[p].field_0A = 3;
            func_8009F7F4((s16)idx, 1, actor->field_0x252, 1);
        } else {
            p = D_8005F144;
            D_80070760[p].field_0A = D_80070A60[p].field_0A = 5;
            func_8009F7F4((s16)idx, -1, actor->field_0x254, 1);
        }
        actor->field_0x1D8 = (D_800D9630[(s16)idx]->unk0C >> 4) - 2;
        actor->posX = func_800A0E54(actor->moveStartX, actor->unk1A8, actor->field_0x1D8, actor->field_0x1DA);
        actor->posY = func_800A0E54(actor->moveStartY, actor->unk1AC, actor->field_0x1D8, actor->field_0x1DA);
        actor->posZ = func_800A0E54(actor->moveStartZ, actor->unk1B0, actor->field_0x1D8, actor->field_0x1DA);
        func_8009B74C(2, (D_8005F144 - D_8005F11A) & FIELD_PATH_RING_MASK, D_80070A60, 1);
        func_8009B74C(1, (D_8005F144 - D_8005F118) & FIELD_PATH_RING_MASK, D_80070760, 1);
        p = D_8005F144;
        D_80070760[p].field_0B = D_80070A60[p].field_0B = actor->field_0x241;
        actor->field_0x1DA++;
        if (actor->field_0x1DA < actor->field_0x1D8) {
            break;
        }
        d.vx = (actor->field_0x1C0 - actor->unk1A8) / 1024;
        d.vy = (actor->field_0x1C4 - actor->unk1AC) / 1024;
        d.vz = (actor->field_0x1C8 - actor->unk1B0) / 1024;
        actor->field_0x1D8 = SquareRoot0(d.vx * d.vx + d.vy * d.vy + d.vz * d.vz) / D_80070656;
        actor->field_0x1DA = 0;
        actor->msgState = 1;
        break;
    case 1:
        func_8009F990((s16)idx, flags);
        break;
    case 2:
        break;
    case 4:
        if (actor->windowId == 1) {
            p = D_8005F144;
            D_80070760[p].field_0A = D_80070A60[p].field_0A = 5;
            func_8009F7F4((s16)idx, 1, actor->field_0x254, 1);
        } else {
            p = D_8005F144;
            D_80070760[p].field_0A = D_80070A60[p].field_0A = 3;
            func_8009F7F4((s16)idx, -1, actor->field_0x252, 1);
        }
        actor->field_0x1D8 = (D_800D9630[(s16)idx]->unk0C >> 4) - 2;
        actor->posX = func_800A0E54(actor->field_0x1C0, actor->msgTextPtr, actor->field_0x1D8, actor->field_0x1DA);
        actor->posY = func_800A0E54(actor->field_0x1C4, actor->msgPosX, actor->field_0x1D8, actor->field_0x1DA);
        actor->posZ = func_800A0E54(actor->field_0x1C8, actor->msgPosY, actor->field_0x1D8, actor->field_0x1DA);
        func_8009B74C(2, (D_8005F144 - D_8005F11A) & FIELD_PATH_RING_MASK, D_80070A60, 1);
        func_8009B74C(1, (D_8005F144 - D_8005F118) & FIELD_PATH_RING_MASK, D_80070760, 1);
        p = D_8005F144;
        D_80070760[p].field_0B = D_80070A60[p].field_0B = actor->field_0x241;
        actor->field_0x1DA++;
        if (actor->field_0x1DA < actor->field_0x1D8) {
            break;
        }
        actor->field_0x1DA = 0;
        actor->msgState = 6;
        actor->triIdx = actor->field_0x1FC;
        break;
    case 5:
        if (actor->windowId == 1) {
            p = D_8005F144;
            D_80070760[p].field_0A = D_80070A60[p].field_0A = 3;
            func_8009F7F4((s16)idx, -1, actor->field_0x252, 1);
        } else {
            p = D_8005F144;
            D_80070760[p].field_0A = D_80070A60[p].field_0A = 5;
            func_8009F7F4((s16)idx, 1, actor->field_0x254, 1);
        }
        actor->field_0x1D8 = (D_800D9630[(s16)idx]->unk0C >> 4) - 2;
        actor->posX = func_800A0E54(actor->unk1A8, actor->moveStartX, actor->field_0x1D8, actor->field_0x1DA);
        actor->posY = func_800A0E54(actor->unk1AC, actor->moveStartY, actor->field_0x1D8, actor->field_0x1DA);
        actor->posZ = func_800A0E54(actor->unk1B0, actor->moveStartZ, actor->field_0x1D8, actor->field_0x1DA);
        func_8009B74C(2, (D_8005F144 - D_8005F11A) & FIELD_PATH_RING_MASK, D_80070A60, 1);
        func_8009B74C(1, (D_8005F144 - D_8005F118) & FIELD_PATH_RING_MASK, D_80070760, 1);
        p = D_8005F144;
        D_80070760[p].field_0B = D_80070A60[p].field_0B = actor->field_0x241;
        actor->field_0x1DA++;
        if (actor->field_0x1DA < actor->field_0x1D8) {
            break;
        }
        actor->field_0x1DA = 0;
        actor->msgState = 6;
        break;
    case 6:
        D_8005F160 = 15;
        D_8005F162 = 30;
        p = D_8005F144;
        D_80070760[p].field_0A = D_80070A60[p].field_0A = 2;
        func_8009F7F4((s16)idx, -1, actor->field_0x24F, 1);
        if (actor->field_0x1DA == 0) {
            if (g_fieldEntity.entityIndex[2] != 0xFF) {
                actor->field_0x1D8 = 32;
            } else if (g_fieldEntity.entityIndex[1] != 0xFF) {
                actor->field_0x1D8 = 16;
            } else {
                actor->field_0x1D8 = 1;
            }
        }
        func_8009B74C(2, (D_8005F144 - D_8005F11A) & FIELD_PATH_RING_MASK, D_80070A60, 2);
        func_8009B74C(1, (D_8005F144 - D_8005F118) & FIELD_PATH_RING_MASK, D_80070760, 2);
        p = D_8005F144;
        D_80070760[p].field_0B = D_80070A60[p].field_0B = actor->field_0x241;
        actor->unk258 = 1;
        actor->field_0x1DA++;
        if (actor->field_0x1DA < actor->field_0x1D8) {
            break;
        }
        actor->msgState = 2;
        break;
    }
}

/**
 * @brief Transcode the script entry list at @c D_800D5E90->entries into
 *        a buffer of 16x16 sprite (@c SPRT_16) primitives.
 *
 * Walks the list of @ref ScriptEntry records (stride @c 0x10, terminated
 * by @c terminator == @c 0x7FFF) and writes one @c SPRT_16 per entry,
 * returning the advanced @c prim pointer. Each sprite gets the entry's
 * @c u / @c v texture cell and @c clut, gray @c 0x80 colour, and is
 * marked semi-translucent unless @c kind == @c 4 (opaque).
 *
 * Called by @c func_800983F0 as part of the chain that lays out
 * draw-prim regions back-to-back in one growing buffer.
 */
SPRT_16 *func_800A0640(SPRT_16 *prim) {
    ScriptEntry *e = D_800D5E90->entries;
    while (1) {
        if (e->terminator == 0x7FFF) break;
        setSprt16(prim);
        setUV0(prim, e->u, e->v);
        prim->clut = e->clut;
        /* Chain direction is load-bearing: b0 must store first (setRGB0 stores r0 first). */
        prim->r0 = prim->g0 = prim->b0 = 0x80;
        /* Hand-written toggle: setSemiTrans lays its arms out inverted. */
        if (e->kind == 4) {
            prim->code &= ~PRIM_CODE_SEMI_TRANS;
        } else {
            prim->code |= PRIM_CODE_SEMI_TRANS;
        }
        prim++;
        e++;
    }
    return prim;
}

INCLUDE_ASM("asm/field/nonmatchings/fe_object1", func_800A06F0);

/**
 * @brief Restore VRAM regions for the dialog window and 16 strip layers.
 *
 * Two-phase StoreImage transfer:
 *   - First, copies a 256x24 region from the front of @p buf to VRAM
 *     at @c (0, 232) (the dialog header / pause overlay strip).
 *   - Then, advances @p buf by @c 0x3000 and StoreImages 16 strips of
 *     @c 832x16 (stride @c 0x6800 in the buffer), each placed at
 *     @c (0, 256 + i*16), a stack of 16 strips of decoration / VRAM
 *     content.
 *
 * Each transfer is sandwiched by @c DrawSync(1) polls (GPU-busy
 * waits); @c VSync(0) is called once per strip to set up the
 * mode for the upcoming StoreImage.
 *
 * @return Restores VRAM in-place; no return value.
 */
void func_800A0D6C(u8 *buf) {
    RECT rect;
    s16 i;
    rect.x = 0;
    rect.y = 0xE8;
    rect.w = 0x100;
    rect.h = 0x18;
    while (DrawSync(1) != 0) {}
    LoadImage(&rect, (u32 *)buf);
    while (DrawSync(1) != 0) {}
    buf += 0x3000;
    for (i = 0; i < 16; i++) {
        VSync(0);
        rect.x = 0;
        rect.y = i * 16 + 0x100;
        rect.w = 0x340;
        rect.h = 0x10;
        LoadImage(&rect, (u32 *)buf);
        buf += 0x6800;
    }
    while (DrawSync(1) != 0) {}
}

/**
 * @brief Overflow-safe s32 linear interpolation.
 *
 * Computes @c start + ((end - start) * progress) / total without
 * intermediate overflow. The difference @c end - start is checked
 * against signed 20-bit range (@c [-0x7FFFF, 0x7FFFF]):
 *   - When the difference fits, multiplies first then divides, the
 *     precise path that keeps fractional information through the
 *     multiplication.
 *   - When the difference is too large to safely multiply by @p progress
 *     in 32-bit, divides first then multiplies, loses some precision
 *     but avoids overflow.
 *
 * The fit check uses @c (u32)(diff + 0x7FFFF) <= 0xFFFFE, adding the
 * positive bias maps the signed range @c [-0x7FFFF, 0x7FFFF] to the
 * unsigned range @c [0, 0xFFFFE] so a single unsigned compare suffices.
 */
s32 func_800A0E54(s32 start, s32 end, s32 total, s32 progress) {
    s32 diff = end - start;
    if ((u32)(diff + 0x7FFFF) <= 0xFFFFE) {
        start += (diff * progress) / total;
    } else {
        start += (diff / total) * progress;
    }
    return start;
}

/**
 * @brief Sine-eased s32 interpolation between two endpoints.
 *
 * Computes an eased lerp from @p start to @p end using a sin lookup as
 * the easing curve. The phase index for the lookup is derived from
 * @p angle and @p total such that one full sin period spans @p total
 * steps:
 *   - @c (angle << 12) / total scales the angle into a fractional
 *     position of the period.
 *   - @c / 32 narrows that to the sin table's @c 0x100-entry resolution.
 *   - @c - 0x80 shifts the phase so the curve crosses zero at the
 *     midpoint instead of the start.
 *
 * The sin sample is returned in @c [-0x1000, 0x1000]; adding @c 0x1000
 * remaps it to @c [0, 0x2000] and dividing by @c 0x2000 gives a
 * fraction of @c (end - start) to add to @p start.
 *
 * @return Eased value between @p start and @p end at phase
 *         @c angle / total.
 */
s32 func_800A0EB8(s32 start, s32 end, s32 total, s32 angle) {
    s32 idx = ((angle << 12) / total) / 32 - 0x80;
    s32 diff = end - start;
    s16 sin_val = func_8009D254(idx & 0xFF);
    return start + ((sin_val + 0x1000) * diff) / 0x2000;
}

/**
 * @brief Project a 3D point through the current world transform and
 *        return the @c RotTransPers projection result.
 *
 * Pushes the GTE matrix stack, installs the current world transform
 * (rotation and translation) from @c D_800C71F8, resets the geometric
 * offset to @c (0, 0), then projects @p v to screen space, writing the
 * resulting on-screen XY into @c *sxy and discarding the @c p and flag
 * outputs into stack locals. Pops the matrix stack via
 * @c PopMatrix (return discarded) and returns @c RotTransPers 's
 * result, the saved value survives @c PopMatrix by being copied
 * out of @c v0 in the @c jal delay slot.
 */
s32 func_800A0F34(SVECTOR *v, s32 *sxy) {
    s32 result;
    s32 unk_p, unk_flag;
    PushMatrix();
    SetRotMatrix((u8 *)D_800C71F8);
    SetTransMatrix((u8 *)D_800C71F8);
    SetGeomOffset(0, 0);
    result = RotTransPers(v, sxy, &unk_p, &unk_flag);
    PopMatrix();
    return result;
}

/**
 * @brief 2D position clamp, clamp @c out->(x,y) to a rect defined by
 *        @c D_8005F0F8->rect_a[a], shrunk by half the extents of
 *        @c D_8005F0F8->rect_b[b].
 *
 * Computes four "half" values from rect_b's (f0,f2) and (f4,f6) field
 * pairs (signed average with round-toward-zero via @c (x + (x>>31)) >> 1),
 * then clamps @c out->x and @c out->y to the rect_a bounds @c (f4,f6) and
 * @c (f2,f0) minus/plus the appropriate half.
 *
 * @param out  Output position (s16 x, s16 y).
 * @param a    @c rect_a[] index (one of 2 in caller).
 * @param b    @c rect_b[] index (always 0 in caller).
 *
 * @note The first and last `half` expressions are inlined (not assigned
 *       to the @c half local) to match the target's register allocation
 *       cascade, when inlined, gcc reuses the @c a*8 register slot
 *       (a1) for the final half value; when assigned to @c half, gcc
 *       picks a3, leaving a1 holding stale @c a*8 and the function
 *       grows by 4 register-field bit changes.
 */
void func_800A0FB8(Vec2s *out, s16 a, s16 b) {
    s32 half;

    if (D_8005F0F8->rect_a[a].f4 - (D_8005F0F8->rect_b[b].f4 - D_8005F0F8->rect_b[b].f6) / 2 < out->x) {
        out->x = D_8005F0F8->rect_a[a].f4 - (D_8005F0F8->rect_b[b].f4 - D_8005F0F8->rect_b[b].f6) / 2;
    }
    half = (D_8005F0F8->rect_b[b].f4 - D_8005F0F8->rect_b[b].f6) / 2;
    if (out->x < D_8005F0F8->rect_a[a].f6 + half) {
        out->x = D_8005F0F8->rect_a[a].f6 + half;
    }
    half = (D_8005F0F8->rect_b[b].f2 - D_8005F0F8->rect_b[b].f0) / 2;
    if (D_8005F0F8->rect_a[a].f2 - half < out->y) {
        out->y = D_8005F0F8->rect_a[a].f2 - half;
    }
    if (out->y < D_8005F0F8->rect_a[a].f0 + (D_8005F0F8->rect_b[b].f2 - D_8005F0F8->rect_b[b].f0) / 2) {
        out->y = D_8005F0F8->rect_a[a].f0 + (D_8005F0F8->rect_b[b].f2 - D_8005F0F8->rect_b[b].f0) / 2;
    }
}
