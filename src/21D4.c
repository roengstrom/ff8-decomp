#include "common.h"
#include "cd.h"
#include "psxsdk/libgpu.h"

/** @brief Fade effect mode for the rendering system. */
typedef enum {
    FADE_NONE = 0,
    FADE_IN   = 1,
    FADE_OUT  = 2
} FadeMode;

/** @brief VSync rendering dispatch mode. */
typedef enum {
    RENDER_IDLE    = 0,
    RENDER_FADE    = 1,
    RENDER_BATTLE  = 2,
    RENDER_OVERLAY = 3,
    RENDER_GAME    = 4
} RenderMode;

/** @brief Layout of the snapshot region within g_gameState (offsets 0xD40-0xD5C).
 *  Used by func_80011870 (save) and RestoreSnapshot (restore).
 */
typedef struct {
    u8  pad0[0xD40];      /* 0x000..0xD3F */
    u16 vsync_rate;       /* 0xD40 */
    u16 music_track;      /* 0xD42 */
    u16 field_120;        /* 0xD44 */
    u16 positions_x[3];   /* 0xD46..0xD4B */
    u16 positions_y[3];   /* 0xD4C..0xD51 */
    u16 rotations[3];     /* 0xD52..0xD57 */
    u8  anim_states[3];   /* 0xD58..0xD5A */
    u8  fade1;            /* 0xD5B */
    u8  fade0;            /* 0xD5C */
} SnapshotBuf;

/** @brief Field entity state structure at g_fieldEntity (0x800704A8).
 *  Contains party member positions, rotations, and animation states
 *  for the field/overworld. Used by RestoreSnapshot and func_80011870.
 */
typedef struct {
    u8  state;            /* 0x000  exit/state flag (4 = exit) */
    u8  pad0[0x3];        /* 0x001..0x003 */
    u16 position_x;       /* 0x004 */
    u16 position_y;       /* 0x006 */
    u8  pad1[0x4];        /* 0x008..0x00B */
    u16 rotation;         /* 0x00C */
    u16 anim_state;       /* 0x00E */
    u8  pad2[0x2];        /* 0x010..0x011 */
    u8  member_index;     /* 0x012 */
    u8  pad3[0x10D];      /* 0x013..0x11F */
    u16 field_120;        /* 0x120 */
} FieldEntity;

/** @brief Restores a previously saved camera/field state snapshot.
 *
 *  Inverse of func_80011870. Reads the snapshot from g_gameState+0xD40..0xD5C
 *  and writes values back into g_currentMusicTrack (music track), g_fieldEntity (field
 *  state structure), g_vsyncRate (VSync rate), and fade state variables.
 */
void RestoreSnapshot(void) {
    extern u8 g_gameState[];
    extern volatile u16 g_vsyncRate;
    extern u16 g_currentMusicTrack;
    extern u8 D_8005F150;
    extern u8 D_8005F151;
    extern FieldEntity g_fieldEntity;
    SnapshotBuf *buf;
    FieldEntity *entity;

    buf = (SnapshotBuf *)(s32)g_gameState;

    g_vsyncRate = buf->vsync_rate;
    if (g_vsyncRate == 0) {
        g_vsyncRate = 1;
    }

    g_currentMusicTrack = buf->music_track;

    entity = &g_fieldEntity;

    entity->field_120 = buf->field_120;
    entity->position_x = buf->positions_x[0];
    entity->position_y = buf->positions_y[0];
    entity->rotation = buf->rotations[0];
    entity->anim_state = buf->anim_states[0];

    D_8005F151 = buf->fade1;
    D_8005F150 = buf->fade0;
}

extern volatile u16 D_8005F14C;
extern CdFileDesc D_80097410[];
extern CdFileDesc D_800974D0[];

/** @brief Loads field data set A from CD into 0x80098000.
 *
 *  Uses D_80097410[0] as the CD file descriptor (.sector, .size). Skips
 *  loading if D_8005F14C is 6 (menu) or 0xA (special mode), since field data
 *  is not needed in those states. Calls func_8001F5C8 (sound state reset)
 *  afterward.
 */
void func_80011A60(void) {
    if ((s16)D_8005F14C != 6 && (s16)D_8005F14C != 0xA) {
        func_80038868(D_80097410[0].sector, D_80097410[0].size, 0x80098000, 0);
        while (func_800393C8() != 0)
            ;
    }
    func_8001F5C8();
}

/** @brief Loads field data set B from CD into 0x80098000.
 *
 *  Identical to func_80011A60 but uses D_800974D0[0] as the source
 *  descriptor. Same state-skip logic and post-load sound reset.
 *
 *  @note Purpose uncertain — the difference between data sets A and B may
 *        correspond to different field maps or disc configurations.
 */
void func_80011AE0(void) {
    if ((s16)D_8005F14C != 6 && (s16)D_8005F14C != 0xA) {
        func_80038868(D_800974D0[0].sector, D_800974D0[0].size, 0x80098000, 0);
        while (func_800393C8() != 0)
            ;
    }
    func_8001F5C8();
}

/** @brief Loads a secondary data block from CD into 0x80097940.
 *
 *  Uses D_80097410[1] as the CD file descriptor (.sector, .size) with
 *  synchronous CD read (func_8003882C). No state-skip check — always loads.
 *
 *  @note Purpose uncertain — destination is near the file table area,
 *        possibly a secondary asset table used during initialization.
 */
void func_80011B60(void) {
    func_8003882C(D_80097410[1].sector, D_80097410[1].size, 0x80097940, 0);
    while (func_800393C8() != 0)
        ;
}

/** @brief Initializes the sound/music engine by loading audio assets from CD.
 *
 *  1. Calls func_80012CC8 to initialize the SPU hardware.
 *  2. Loads the sound bank header from CD (D_800974D8[0]) into D_80067468
 *     and parses it via func_80012D08.
 *  3. Loads two sample banks from CD (D_800974D8[1], D_800974D8[2]) into a
 *     scratch buffer at 0x801B0000 and uploads each to SPU RAM via
 *     func_80013A5C.
 */
void func_80011BA8(void) {
    extern u8 D_80067468[];
    extern CdFileDesc D_800974D8[];

    func_80012CC8();

    func_8003882C(D_800974D8[0].sector, D_800974D8[0].size, (s32)D_80067468, 0);
    while (func_800393C8() != 0)
        ;

    func_80012D08((s32)D_80067468);

    func_8003882C(D_800974D8[1].sector, D_800974D8[1].size, 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;

    func_80013A5C(0x801B0000, 1);

    func_8003882C(D_800974D8[2].sector, D_800974D8[2].size, 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;

    func_80013A5C(0x801B0000, 1);
}

extern CdFileDesc D_80097400[];

/** @brief Loads an overlay from CD into 0x80098000.
 *
 *  Uses D_80097400[0] as the CD file descriptor (.sector, .size). The
 *  specific overlay loaded depends on what D_80097400 was populated with
 *  (set by func_80011E18 which loads the master file table).
 */
void func_80011C68(void) {
    func_8003882C(D_80097400[0].sector, D_80097400[0].size, 0x80098000, 0);
    while (func_800393C8() != 0)
        ;
}

extern CdFileDesc D_80097808[];

/** @brief Loads texture data from CD and uploads it to VRAM.
 *
 *  Reads data from CD using D_80097808[0] (.sector, .size) into scratch
 *  buffer 0x801B0000, then calls func_8002C3AC to process and upload the
 *  textures to GPU VRAM. Spins on DrawSync until the GPU transfer completes.
 */
void func_80011CB0(void) {
    func_8003882C(D_80097808[0].sector, D_80097808[0].size, 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;
    func_8002C3AC(0x801B0000, 0);
    while (DrawSync(1) != 0)
        ;
}

/** @brief Loads an overlay and initializes the field/battle module.
 *
 *  Calls func_80011C68 to load the overlay at 0x80098000, then runs its
 *  init function (func_80098390) and calls func_80028444 for additional setup.
 */
void func_80011D0C(void) {
    func_80011C68();
    func_80098390();
    func_80028444();
}

/** @brief Extended battle/visual initialization — loads multiple CD assets.
 *
 *  1. Loads overlay via func_80011C68.
 *  2. Initializes a 0x6000-byte buffer at D_8006A468 via func_8002AB5C.
 *  3. Loads and uploads textures to VRAM via func_80011CB0.
 *  4. Loads battle entity/model data and processes it via func_80028564.
 *  5. Loads a lookup table and copies 0x200 bytes via func_80039678.
 *  6. Loads additional data and stores its pointer via func_8002C100.
 */
void func_80011D3C(void) {
    extern CdFileDesc D_80097808[];
    extern u8 D_8006A468[];
    extern u8 D_8005F188[];

    func_80011C68();
    func_8002AB5C((s32)D_8006A468, 0x6000);
    func_80011CB0();

    func_8003882C(D_80097808[1].sector, D_80097808[1].size, 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;

    func_80028564(0x801B0000);

    func_8003882C(D_80097808[3].sector, D_80097808[3].size, 0x80090000, 0);
    while (func_800393C8() != 0)
        ;

    func_80039678((s32)D_8005F188, 0x80090000, 0x200);

    func_8003882C(D_80097808[2].sector, D_80097808[2].size, 0x80090000, 0);
    while (func_800393C8() != 0)
        ;

    func_8002C100(0x80090000);
}

extern CdFileDesc g_fileTableDesc[];

/** @brief Loads the master file descriptor table from CD into 0x80097400.
 *
 *  Uses the hard-coded bootstrap descriptor g_fileTableDesc[0] (.sector,
 *  .size). This must be called before any other CD loading functions, since
 *  all other load descriptors (D_80097410, D_800974D0, D_800974D8,
 *  D_80097808) reside within the table loaded here.
 */
void func_80011E18(void) {
    func_8003882C(g_fileTableDesc[0].sector, g_fileTableDesc[0].size, 0x80097400, 0);
    while (func_800393C8() != 0)
        ;
}

/** @brief Game main function and primary state machine loop.
 *
 *  Initialization sequence: hardware init, CD-ROM init, loads file table,
 *  sound engine, overlays, and field/battle data.
 *
 *  Main loop runs until g_fieldEntity[0] == 4 (game exit). Uses g_vsyncRate
 *  as the state variable (decremented per iteration, dispatched via switch):
 *  - State 1: Load field audio, run overlay update.
 *  - State 2: Field-to-battle transition setup.
 *  - State 3: Battle execution and result handling.
 *  - State 4: Post-battle fade-out/transition.
 *  - State 5+: Disc change handling.
 *
 *  On exit (g_fieldEntity[0] == 4), shuts down sound, resets GPU, and
 *  reinitializes everything from the top.
 */
INCLUDE_ASM("asm/nonmatchings/21D4", main);

/** @brief Sets up the GPU draw mode for both display buffers.
 *
 *  Computes a TPage value via GetTPage and applies it to both DR_MODE
 *  primitives (D_80070468 and D_80070468+0x20) for the double-buffered
 *  rendering system.
 *
 *  @param a0 Texture page semi-transparency mode (passed to GetTPage).
 *            Value 2 (8-bit) is used from ProcessFade.
 */
void SetupDrawMode(s32 a0) {
    extern u8 D_80070468[];
    u16 tpage;

    tpage = GetTPage(0, a0, 0, 0);
    SetDrawMode(D_80070468, 0, 0, tpage, 0);
    SetDrawMode(D_80070468 + 0x20, 0, 0, tpage, 0);
}

/** @brief Initializes two full-screen black TILE primitives for screen clearing.
 *
 *  Sets up a 320x224 black fill rectangle in each of the two double-buffered
 *  primitive slots at g_clearTiles (32 bytes apart). These are used for screen
 *  clearing during frame rendering.
 */
void InitClearTiles(void) {
    extern TILE g_clearTiles[];
    s32 i = 0;
    TILE *tile = &g_clearTiles[0];
top:
    SetTile(tile);
    SetSemiTrans(tile, 1);
    SetShadeTex(tile, 1);
    tile->w = 320;
    tile->r0 = 0;
    tile->g0 = 0;
    tile->b0 = 0;
    tile->h = 224;
    tile->x0 = 0;
    tile->y0 = 0;
    tile += 2;
    if (++i < 2) goto top;
}

/** @brief Copies the framebuffer from the current display buffer to the other.
 *
 *  Uses MoveImage to synchronize both framebuffers so they contain the same
 *  content. Reads source/destination coordinates from the DISPENV array at
 *  g_dispEnvs. Spins on DrawSync until the GPU transfer completes.
 *
 *  Logically equivalent to:
 *    DISPENV *src = &g_dispEnvs[g_bufferIndex];
 *    DISPENV *dst = &g_dispEnvs[(g_bufferIndex + 1) & 1];
 *    MoveImage(&src->disp, dst->disp.x, dst->disp.y);
 *  Raw pointer arithmetic is used for codegen matching (volatile triple-load,
 *  implicit ptr-to-int, explicit stride).
 */
void func_800128F8(void) {
    extern DISPENV g_dispEnvs[];
    extern volatile u16 g_bufferIndex; /* volatile for codegen match */
    s32 base;
    s32 ofs1;

    ofs1 = (s16)g_bufferIndex * 20;
    base = g_dispEnvs;

    MoveImage(
        (void *)(ofs1 + base),
        *(s16 *)(((s16)g_bufferIndex + 1 & 1) * 20 + base),
        *(s16 *)(((s16)g_bufferIndex + 1 & 1) * 20 + base + 2)
    );

    do {
    } while (DrawSync(1) != 0);
}

/** @brief Builds and chains the per-frame GPU primitive list for rendering.
 *
 *  Clears the ordering table for the current buffer, sets the TILE fill
 *  color to the given brightness, and chains the TILE and DR_MODE primitives
 *  into the OT for submission via DrawOTag.
 *
 *  @param a0 Fill brightness (uniform RGB). 16 (dark grey) is used during
 *            fade effects.
 */
INCLUDE_ASM("asm/nonmatchings/21D4", BuildPrimList);

extern volatile u16 g_bufferIndex; /* volatile for codegen match (forces sign extension, prevents CSE) */
extern volatile u8 g_fadeMode; /* volatile for codegen match (forces reload each access) */
extern volatile u16 g_renderMode;
extern u8 g_fadeCounter;
extern DISPENV g_dispEnvs[];
extern DRAWENV g_drawEnvs[];
extern u32 g_orderingTablePtrs[];

/** @brief Main frame rendering function with double-buffer swap and fade effects.
 *
 *  Called from the VSync callback (VsyncHandler, case 0). Toggles the
 *  double buffer index, manages fade-in/fade-out frame counters, and submits
 *  the GPU primitive list for the current frame.
 *
 *  Fade behavior depends on g_fadeMode:
 *  - FADE_NONE: rendering disabled, returns immediately.
 *  - FADE_IN: fade-in mode — updates every 4th frame for 128 frames total.
 *  - FADE_OUT: fade-out mode — updates every frame for 34 frames total.
 *
 *  When the fade counter expires, sets g_renderMode=RENDER_IDLE and g_fadeMode=FADE_NONE to
 *  signal completion to the main loop.
 */
void ProcessFade(void) {
    if (g_fadeMode == FADE_NONE) {
        return;
    }

    g_bufferIndex = g_bufferIndex + 1;
    g_bufferIndex = g_bufferIndex & 1;

    if (g_fadeMode == FADE_IN) {
        s32 fc;
        fc = g_fadeCounter + 1;
        g_fadeCounter = fc;
        if ((fc & 0xFF) == 0x80) {
            g_renderMode = RENDER_IDLE;
            g_fadeMode = FADE_NONE;
        }
        if (fc & 0x6) {
            return;
        }
    } else {
        g_fadeCounter = g_fadeCounter + 1;
        if (g_fadeCounter == 0x22) {
            g_renderMode = RENDER_IDLE;
            g_fadeMode = FADE_NONE;
        }
    }

    SetupDrawMode(2);
    BuildPrimList(16);
    PutDispEnv(&g_dispEnvs[(s16)g_bufferIndex]);
    PutDrawEnv(&g_drawEnvs[(s16)g_bufferIndex]);
    DrawOTag(&g_orderingTablePtrs[(s16)g_bufferIndex]);
}
