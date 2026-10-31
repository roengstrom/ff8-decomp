#ifndef INTRO_H
#define INTRO_H

#include "common.h"
#include "intro_assets.h"
#include "intro_state.h"
#include "psxsdk/libgpu.h"

/**
 * @brief LZSS-decoded intro frame in staging RAM.
 *
 * Every entry in @c g_introAssetTable is an LZSS-compressed bitmap. After
 * @c cdReadAsyncSync (mode 7) decompresses one into staging RAM, the
 * layout starts with this 8-byte header followed by row-major 16bpp
 * BGR555 pixel data.
 */
typedef struct {
    u16 hdr0;
    u16 hdr1;
    u16 width;
    u16 height;
    u16 pixels[1];          /**< Flexible array — actually @c width × @c height entries. */
} IntroFrame;

/** @brief Pre-loaded 580x406 "CAUTION WRONG DISC" frame at @c 0x8017D000. */
#define g_wrongDiscFrame      ((IntroFrame *)0x8017D000)
#define WRONG_DISC_FRAME_W    580
#define WRONG_DISC_FRAME_H    406

/** @brief Per-stage scratch buffer where @c loadIntroSlide decodes a slide. */
#define g_introStagedFrame    ((IntroFrame *)0x80100000)

/* --- Intro overlay entry points (defined in src/intro.c) ----------------- */
void initIntroOverlay(void);
void func_8009818C(void);
void loadIntroSlide(s32 stage);
void func_80098378(s32 mode, s32 x, s32 y, s32 width, s32 height);
void func_80098440(s32 brightness, s32 mode, RECT *rect);
void loadWrongDiscWarning(void);
void func_800985EC(void);
void func_8009869C(void);
void playBootIntro(void);
void waitForCorrectDisc(void);
void func_80098FD4(s32 mode);

/* --- External helpers called by the intro overlay ------------------------ */
extern s32  getDiscId(void);

#endif /* INTRO_H */
