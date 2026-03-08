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

/** @brief Battle entity structure (stride 612 / 0x264 bytes).
 *  Accessed via pointer at D_80085224. Fields named from func_80011870 usage.
 */
typedef struct {
    u8  pad0[0x190];   /* 0x000..0x18F */
    s32 pos_x;         /* 0x190  fixed-point 20.12 */
    s32 pos_y;         /* 0x194  fixed-point 20.12 */
    u8  pad1[0x62];    /* 0x198..0x1F9 */
    u16 rotation;      /* 0x1FA */
    u8  pad2[0x45];    /* 0x1FC..0x240 */
    u8  anim_state;    /* 0x241 */
    u8  pad3[0x22];    /* 0x242..0x263 */
} BattleEntity;        /* total: 0x264 = 612 */

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

/** @brief Empty stub, called at the very start of main before initialization.
 *  @note Purpose uncertain — may be a debug hook or placeholder that was
 *        never filled in the retail build.
 */
void func_8001152C(void) {
}

/** @brief CRT0 entry point. Clears BSS, sets up GP register, and calls main. */
INCLUDE_ASM("asm/nonmatchings/1D2C", start);

/** @brief Clears the GPU ordering tables and flushes the GPU pipeline.
 *
 *  Builds a minimal OT, submits it via DrawOTag, and spins on DrawSync
 *  until rendering completes. Used to ensure the display is blank during
 *  transitions (e.g. waiting for a fade to finish).
 */

void func_800115F0(void) {
    u32 prim[4];
    u32 ot[2];

    ClearOTag(ot, 2);
    SetDrawStp(prim, 0);
    addPrim(ot, prim);
    DrawOTag(ot);

    do {
    } while (DrawSync(1) != 0);
}

/** @brief VSync callback handler, registered via VSyncCallback in InitHardware.
 *
 *  Dispatches per-frame rendering based on the game mode (g_renderMode):
 *  - 0: no action (rendering idle)
 *  - 1: calls ProcessFade (normal frame draw/swap with fade)
 *  - 2: calls func_80026D8C (battle VSync handler)
 *  - 3: calls func_800D0608 (overlay-loaded VSync handler)
 *  - 4: calls func_800205D0 (game code VSync handler)
 *
 *  Also maintains two fixed-point frame timing accumulators (D_8005F154 and
 *  D_8005F15C) that increment by 0x88F per VSync. On bit-17 rollover (~every
 *  12 frames), one increments a game frame counter at g_gameState+0xCD0
 *  and the other decrements a countdown timer at g_gameState+0xCD4.
 *
 *  @note Non-matching (3 instructions shorter). Two codegen differences:
 *        1. CC1PSX fills beqz delay slot with `addu v0,v0,-1` (countdown
 *           decrement) instead of original `li v0,1` (early-return value),
 *           eliminating a 3-instruction reload sequence.
 *        2. Switch jump table rodata must be at 0x80010000 (before asm
 *           rodata) — requires linker script reorder: move 1D2C.o(.rodata)
 *           first, add `. = ALIGN(8);` after it.
 *
 *  @code
 *  void VsyncHandler(void) {
 *      extern volatile s32 D_8005F154;
 *      extern volatile s32 D_8005F15C;
 *      extern u16 D_8005F11E;
 *      extern u8 g_gameState[];
 *
 *      switch ((s16)g_renderMode) {
 *      case 0:
 *          break;
 *      case 1:
 *          ProcessFade();
 *          break;
 *      case 2:
 *          func_80026D8C();
 *          break;
 *      case 3:
 *          func_800D0608();
 *          break;
 *      case 4:
 *          func_800205D0();
 *          break;
 *      }
 *
 *      if (func_800283CC() != 0) {
 *          D_8005F11E = 1;
 *          return;
 *      }
 *
 *      if (g_vsyncSkip != 0) {
 *          D_8005F11E = 1;
 *          return;
 *      }
 *
 *      D_8005F154 += 0x88F;
 *      if (D_8005F154 >> 17) {
 *          s32 base = g_gameState;
 *          *(s32 *)(base + 0xCD0) += 1;
 *          asm("");
 *          D_8005F154 &= 0xFFFF;
 *      }
 *
 *      D_8005F15C += 0x88F;
 *      if (D_8005F15C >> 17) {
 *          s32 base = (s32)g_gameState;
 *          if (*(s32 *)(base + 0xCD4) == 0) {
 *              D_8005F11E = 1;
 *              return;
 *          }
 *          *(s32 *)(base + 0xCD4) -= 1;
 *          asm("");
 *          D_8005F15C &= 0xFFFF;
 *      }
 *
 *      D_8005F11E = 1;
 *  }
 *  @endcode
 */
INCLUDE_ASM("asm/nonmatchings/1D2C", VsyncHandler);

void InitGeom(void);
void ResetCallback(void);
void SetMem(u8 a);
void StopCallback(void);
void VSyncCallback(void (*cb)(void));
void InitSpu(void);

extern void VsyncHandler(void);
extern u8 g_vsyncSkip;
extern volatile u16 g_renderMode;

/** @brief Initializes PS1 hardware subsystems.
 *
 *  Called once at the start of main. Sets up memory mode, resets the callback
 *  system, initializes GPU and GTE, starts SPU via InitSpu (SpuInit
 *  wrapper), registers the VSync callback, and disables display output until
 *  the game is ready to show content.
 */
void InitHardware(void) {
    SetMem(2);
    StopCallback();
    ResetCallback();
    ResetGraph(0);
    InitSpu();
    g_vsyncSkip = 0;
    g_renderMode = RENDER_IDLE;
    VSyncCallback(VsyncHandler);
    SetGraphDebug(0);
    SetDispMask(0);
    InitGeom();
}

/** @brief Saves the current battle/field camera state to a snapshot buffer.
 *
 *  Records party member positions (fixed-point 20.12 -> integer), rotations,
 *  animation states, and display parameters from the battle entity array
 *  (stride 612 at D_80085224) into g_gameState+0xD40..0xD5C. The snapshot
 *  is later restored by RestoreSnapshot when returning from battle.
 *
 *  @note Non-matching: same instruction count (89) but different register
 *  allocation and scheduling. Pre-loop: compiler skips "through $v0" copies
 *  for g_gameState/g_fieldEntity addresses (addiu directly into target reg
 *  instead of same-reg addiu + addu copy). Can be fixed with register
 *  asm("$2") temp + REGALLOC_BARRIER, but causes scheduling side effects.
 *  Loop body: all 4 temp registers shifted (idx=$a0 vs $a1, eoff=$a3 vs
 *  $a2, stp=$a2 vs $a0, entityPtr=$a1 vs $a3). Loop increment: direct
 *  addiu instead of through-$a0 copy pattern (addiu $a0,$t0,1; addu
 *  $t0,$a0,$zero), preventing $v0 from staying free for delay slot sb.
 *  Epilogue: uses 2 registers (sequential) instead of 3 (interleaved).
 *  Permuter best score: ~4850 (base 6025), not trending toward 0.
 *
 *  @code
 *  void func_80011870(void) {
 *      extern volatile u16 D_8005F14C;
 *      extern u16 g_currentMusicTrack;
 *      extern u8 D_8005F150;
 *      extern u8 D_8005F151;
 *      extern u16 D_800780B8;
 *      extern u8 g_gameState[];
 *      extern u8 g_fieldEntity[];
 *      extern BattleEntity *D_80085224;
 *      s32 i;
 *      s32 buf;
 *      s32 src;
 *
 *      if ((s16)D_8005F14C == 2) {
 *          D_800780B8 = 2;
 *      } else {
 *          D_800780B8 = 1;
 *      }
 *
 *      i = 0;
 *      buf = g_gameState;
 *      src = g_fieldEntity;
 *      *(u16 *)(buf + 0xD42) = g_currentMusicTrack;
 *      *(u16 *)(buf + 0xD44) = *(u16 *)(src + 0x120);
 *
 *  top:
 *      {
 *          s16 idx = (s16)i;
 *          s32 eoff = idx + src;
 *          s32 stp = idx * 2 + buf;
 *
 *          *(u16 *)(stp + 0xD46) = D_80085224[*(u8 *)(eoff + 0x12)].pos_x >> 12;
 *          *(u16 *)(stp + 0xD4C) = D_80085224[*(u8 *)(eoff + 0x12)].pos_y >> 12;
 *          *(u16 *)(stp + 0xD52) = D_80085224[*(u8 *)(eoff + 0x12)].rotation;
 *          i++;
 *          *(u8 *)(idx + buf + 0xD58) = D_80085224[*(u8 *)(eoff + 0x12)].anim_state;
 *      }
 *      if ((s16)i < 3) goto top;
 *
 *      buf = g_gameState;
 *      *(u8 *)(buf + 0xD5B) = D_8005F151;
 *      *(u8 *)(buf + 0xD5C) = D_8005F150;
 *  }
 *  @endcode
 */
INCLUDE_ASM("asm/nonmatchings/1D2C", func_80011870);

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
 *
 *  @note Non-matching due to ASPSX vs GAS assembler differences.
 *        CC1PSX output is identical, but ASPSX (SN Systems assembler) performs
 *        optimizations that GAS/maspsx cannot replicate:
 *        1. %hi caching: ASPSX caches %hi(g_fieldEntity) in $fp and
 *           %hi(D_80082C08) in $s7, reusing them across accesses (~8 insns).
 *        2. Constant substitution: ASPSX replaces `li $v0,N; beq $v1,$v0`
 *           with `beq $v1,$sN` when an s-register holds the constant (~3 insns).
 *        3. Delay slot filling around pseudo-instruction expansions.
 *        This only affects files without -G0 (1D2C.c), where CC1PSX emits
 *        assembler pseudo-instructions instead of explicit lui/lw sequences.
 */
#ifdef NON_MATCHING
/** Extended field entity with fields accessed by GameMain beyond the base struct. */
typedef struct {
    u8  state;            /* 0x000: 0 = running, 4 = exit */
    u8  pad001;           /* 0x001 */
    u16 scene_id;         /* 0x002: current field/scene ID */
    u16 position_x;       /* 0x004 */
    u16 position_y;       /* 0x006 */
    u8  pad008[4];        /* 0x008..0x00B */
    u16 rotation;         /* 0x00C */
    u16 anim_state;       /* 0x00E */
    u8  pad010[0x190];    /* 0x010..0x19F */
    u8  disc_change_mode; /* 0x1A0: overlay reinit mode */
    u8  pad1A1;           /* 0x1A1 */
    u8  transition_flag;  /* 0x1A2 */
    u8  disc_change_flag; /* 0x1A3 */
    u8  pad1A4[7];        /* 0x1A4..0x1AA */
    u8  field_param;      /* 0x1AB: field-specific parameter */
} FieldEntityFull;

/** Battle scene descriptor (24-byte stride, loaded at 0x80097940). */
typedef struct {
    u16 position_x;   /* 0x00 */
    u16 position_y;   /* 0x02 */
    u16 rotation;     /* 0x04 */
    u16 music_track;  /* 0x06 */
    u8  anim_state;   /* 0x08 */
    u8  pad[15];      /* 0x09..0x17 */
} BattleSceneEntry;   /* 0x18 = 24 bytes */

/** First 8 bytes of the battle config block at D_80082C08. */
typedef struct {
    u16 scene_id;          /* 0x00: battle scene ID to load */
    u8  pad02[2];          /* 0x02 */
    u8  overlay_assets[3]; /* 0x04: asset IDs to load (0xFF = none) */
    u8  result;            /* 0x07: battle result code */
} BattleConfig;

#define BATTLE_SCENE_TABLE  ((BattleSceneEntry *)0x80097940)
#define BATTLE_MAP_ADDR     0x801D0000
#define OVERLAY_ADDR        0x80098000

/** Game phase values (D_8005F14C sub-state). */
#define PHASE_IDLE          0
#define PHASE_FIELD         1
#define PHASE_TRANSITION    2
#define PHASE_BATTLE        3
#define PHASE_DISC_CHANGE   6
#define PHASE_DISC_RETURN   0xA

/** Field entity exit state. */
#define STATE_EXIT          4

/** Music track IDs. */
#define MUSIC_DEFAULT_FIELD 0x49
#define MUSIC_NEW_GAME      0x4A
#define MUSIC_TRANSITION    0x4B

/** Memory card check result flags (func_8003646C). */
#define CARD_NEW_GAME       0x02
#define CARD_DISC_CHANGED   0x04
#define CARD_LOAD_OK        0x01

/** Battle transition types (D_80082C8C). */
#define BTRANS_FROM_FIELD   0
#define BTRANS_ENTER_SCENE  1
#define BTRANS_FROM_BATTLE  2
#define BTRANS_FROM_DISC    4
#define BTRANS_FROM_IDLE    6

/** Battle result codes (BattleConfig.result). */
#define BRESULT_SCRIPTED    1
#define BRESULT_FLAG        3
#define BRESULT_GAME_OVER   5

void GameMain(void)
{
    extern volatile u16 g_vsyncRate;
    extern u8 g_fieldEntity[];
    extern u8 g_gameState[];
    extern u16 g_currentMusicTrack;
    extern s16 D_8005F14A;              /* field audio pending flag */
    extern s16 D_8005F100;              /* current field/area ID */
    extern s32 D_8005F104;              /* field audio CD sector */
    extern s32 D_8005F13C;              /* field map load address */
    extern u8 D_80082C08[];             /* battle config block (see BattleConfig) */
    extern s8 D_80082C8C;              /* battle transition type */
    extern s8 D_80082C8E;              /* battle scene index (low byte) */
    extern u8 D_80082C8F;              /* battle scene index (high byte) */
    extern u8 *D_800562C4;             /* global context pointer */

    FieldEntityFull *fe;
    BattleConfig *bc;
    s32 result;
    s32 mapAddr;
    s16 i;

    /* ================================================================
     * ONE-TIME INITIALIZATION
     * Hardware, CD-ROM, file table, sound engine, overlays.
     * ================================================================ */
    func_8001152C();                        /* Pre-init stub */
    InitHardware();                         /* GPU, SPU, GTE, VSync callback */
    fe = (FieldEntityFull *)g_fieldEntity;
    bc = (BattleConfig *)D_80082C08;
    func_80038668(1);                       /* CD-ROM hardware init */
    func_80011E18();                        /* Load master file table */
    func_80011BA8();                        /* Load sound samples to SPU */
    func_80011C68();                        /* Load game overlay from CD */
    func_80098000();                        /* Run overlay entry point */
    func_80011D0C();                        /* Load + init field/battle module */
    func_80011B60();                        /* Load battle scene table */

    /* ================================================================
     * RESTART POINT
     * Re-entered after game over or end-of-game via goto at bottom.
     * ================================================================ */
restart:
    func_80038868(D_80097400[1].sector, D_80097400[1].size, OVERLAY_ADDR, 0);
    while (func_800393C8() != 0) { }
    func_80098FD4(0);                       /* Init overlay (mode 0 = fresh) */
    func_80011D3C();                        /* Extended battle/visual init */

    /* Reset all game state */
    g_vsyncRate = 1;
    D_8005F14C = PHASE_IDLE;
    fe->disc_change_flag = 0;
    D_8005F14A = 0;
    D_8005F100 = MUSIC_DEFAULT_FIELD;
    fe->state = 0;
    fe->transition_flag = 0;

    /* Check memory card for save data */
    result = func_8003646C(25);

    if (!(result & CARD_NEW_GAME)) {
        /* Continue from saved game */
        RestoreSnapshot();
        func_80011A60();                    /* Load field data set A */
        func_800C00C8(0);                   /* Resume saved field */
    } else {
        /* Start new game */
        s32 gs = g_gameState;
        g_currentMusicTrack = MUSIC_NEW_GAME;
        *((u8 *)(gs + 0xD38)) = 0;         /* Party lead: Squall */
        *((u8 *)(gs + 0xD39)) = 0xFF;      /* Party slot 2: empty */
        *((u8 *)(gs + 0xD3A)) = 0xFF;      /* Party slot 3: empty */
        *((u8 *)(gs + 0xD3B)) = 0xFF;      /* Party slot 4: empty */
        func_80011A60();                    /* Load field data set A */
        func_800C00C8(1);                   /* Start new field */
    }

    /* Validate current disc matches expected disc */
    i = *((u8 *)(D_800562C4 + 0xCC));
    if (i != func_80038CE0()) {
        func_80038868(D_80097400[1].sector, D_80097400[1].size, OVERLAY_ADDR, 0);
        while (func_800393C8() != 0) { }
        func_80098FD4(1);                   /* Reinit overlay (mode 1 = reload) */
        func_80011E18();                    /* Reload master file table */
        func_80011B60();                    /* Reload battle scene table */
    }

    /* ================================================================
     * MAIN GAME LOOP
     * Runs until fe->state == STATE_EXIT.
     * g_vsyncRate selects the current state (switched on value - 1):
     *   1 = field update          5 = (unused)
     *   2 = field-to-battle       6 = disc change (field/battle)
     *   3 = battle map init       7 = (unused)
     *   4 = battle execution      8 = post-battle cleanup
     *   (cases 3/4 share code)    9 = disc reload
     *                            10 = disc change alt (shares w/ 6)
     * ================================================================ */
    do {
        switch ((s16)(g_vsyncRate - 1)) {

        /* ---- State 1: Field Update -------------------------------- */
        case 0:
            func_80011A60();                /* Load field data */
            func_8009895C();                /* Run field overlay update */
            break;

        /* ---- State 2: Field-to-Battle Transition ------------------ */
        case 1:
            /* Select transition type based on current phase */
            switch ((s16)D_8005F14C) {
            case PHASE_FIELD:
                func_800B4F40();            /* Init battle sequence */
                D_80082C8C = BTRANS_FROM_FIELD;
                break;
            case PHASE_BATTLE:
                D_80082C8C = BTRANS_FROM_BATTLE;
                break;
                /* Unreachable dead code (after break) */
                fe->state = 0;
                func_80011BA8();
            case PHASE_DISC_CHANGE:
                D_80082C8C = BTRANS_FROM_DISC;
                break;
            case PHASE_IDLE:
                D_80082C8C = BTRANS_FROM_IDLE;
                break;
            }

            /* Update field audio if a new track is pending */
            if ((s16)D_8005F14C == PHASE_FIELD
                && D_8005F14A != 0
                && D_8005F100 < MUSIC_NEW_GAME) {
                while (func_800393C8() != 0) { }
                func_80038490(D_8005F104, OVERLAY_ADDR);
            } else {
                func_80011AE0();            /* Load field data set B */
            }
            D_8005F14A = 0;

            /* Check if game should exit */
            result = func_800987D8();
            if (result == 1) {
                fe->scene_id = 0;
                fe->state = STATE_EXIT;
                fe->rotation = 0x7FFF;
                func_80012D5C();            /* Shutdown sound */
                break;
            }

            /* Advance to transition phase */
            D_8005F14C = PHASE_TRANSITION;

            /* Dispatch on battle transition type */
            switch ((s8)D_80082C8C) {
            case BTRANS_ENTER_SCENE: {
                BattleSceneEntry *scene = &BATTLE_SCENE_TABLE[(s8)D_80082C8E];
                u16 track = scene->music_track;
                fe->position_x = scene->position_x;
                fe->position_y = scene->position_y;
                fe->rotation = scene->rotation;
                g_currentMusicTrack = track;
                fe->anim_state = scene->anim_state;
                g_vsyncRate = 1;            /* Return to field */
                func_80013100(-1, 0);       /* Field transition */
                break;
            }
            case PHASE_BATTLE:
                g_vsyncRate = 3;            /* Go to battle init */
                break;
            case 5:
                g_vsyncRate = 6;            /* Go to disc change */
                break;
            }
            break;

        /* ---- State 8: Post-Battle Cleanup ------------------------- */
        case 7:
            func_80021358();                /* Finalize battle */
            D_8005F14C = PHASE_BATTLE;
            g_vsyncRate = 1;                /* Return to field */
            break;

        /* ---- States 3/4: Battle Init + Execution ------------------ */
        /*       Case 2 sets battleAddr and falls through to case 3.   */
        case 2: {
            s32 battleAddr = BATTLE_MAP_ADDR;
            /* FALLTHROUGH */
        case 3:
            if ((s16)D_8005F14C == PHASE_FIELD) {
                /* From field: use scene ID and field map */
                bc->scene_id = fe->scene_id;
                mapAddr = D_8005F13C;
            } else {
                /* From battle/disc: use battle scene indices */
                bc->scene_id = ((u8)D_80082C8E) | (((u8)D_80082C8F) << 8);
                mapAddr = battleAddr;
            }

            func_80038030(mapAddr);         /* Set battle map address */
            func_80021358();                /* Execute battle tick */
            if ((s16)g_renderMode != RENDER_GAME) {
                func_800115F0();            /* Clear GPU OT + flush */
            }

            /* --- Process battle result --- */
            if (bc->result == BRESULT_GAME_OVER) {
                fe->scene_id = 0;
                fe->state = STATE_EXIT;
                fe->rotation = 0x7FFF;
                break;
            }
            if (bc->result == BRESULT_SCRIPTED) {
                if (!(*((u16 *)(D_800562C4 + 0xB6)) & 0x200)) {
                    g_currentMusicTrack = MUSIC_TRANSITION;
                    D_8005F14C = PHASE_IDLE;
                    g_vsyncRate = 1;
                    break;
                }
            }
            if (bc->result == BRESULT_FLAG) {
                if (*((u16 *)(D_800562C4 + 0xB6)) & 0x100) {
                    *((s32 *)(D_800562C4 + 0x68)) &= ~0x40;
                } else {
                    g_currentMusicTrack = MUSIC_TRANSITION;
                    D_8005F14C = PHASE_IDLE;
                    g_vsyncRate = 1;
                    break;
                }
            }

            /* Transition to battle phase */
            D_8005F14C = PHASE_BATTLE;
            g_vsyncRate = ((s16)D_8005F14C == PHASE_FIELD) ? 1 : 2;

            /* Load up to 3 overlay assets */
            for (i = 0; i < 3; i++) {
                if (bc->overlay_assets[i] != 0xFF) {
                    while ((s16)g_renderMode == RENDER_GAME) { }
                    while (DrawSync(1) != 0) { }
                    func_8003646C(bc->overlay_assets[i] + 5);
                }
            }
            break;
        }

        /* ---- States 6/10: Disc Change ----------------------------- */
        case 5:
        case 9:
            while (g_renderMode != 0) { }   /* Wait for render idle */
            SetDispMask(0);                  /* Disable display */
            func_80011870();                 /* Save snapshot */

            if ((s16)D_8005F14C == PHASE_FIELD) {
                /* Field path: prompt disc swap for field scene */
                result = func_8003646C(fe->scene_id, fe->field_param);
                if (result & CARD_LOAD_OK) {
                    func_80038308();         /* Clear battle entity flags */
                    D_8005F14C = ((s16)g_vsyncRate == 6)
                        ? PHASE_DISC_RETURN : PHASE_DISC_CHANGE;
                } else {
                    D_8005F14C = PHASE_DISC_CHANGE;
                }
                g_vsyncRate = 1;
            } else {
                /* Battle path: prompt disc swap for battle */
                result = func_8003646C((s32)0x80000000,
                                       *((u8 *)(D_800562C4 + 0xD1)) | 1);
                D_8005F14C = PHASE_DISC_CHANGE;
                g_vsyncRate = 2;
            }

            if (!(result & CARD_DISC_CHANGED)) { break; }

            /* Disc was changed: reload and run battle map */
            D_8005F14C = g_vsyncRate;
            g_vsyncRate = 3;
            bc->scene_id = (u16)(result >> 16);

            if ((s16)g_vsyncRate == 1) {
                mapAddr = D_8005F13C;
            } else {
                mapAddr = BATTLE_MAP_ADDR;
            }

            func_80038030(mapAddr);
            func_80021358();
            if ((s16)g_renderMode != RENDER_GAME) {
                func_800115F0();
            }

            /* --- Process battle result (same logic as states 3/4) --- */
            if (bc->result == BRESULT_GAME_OVER) {
                fe->scene_id = 0;
                fe->state = STATE_EXIT;
                fe->rotation = 0x7FFF;
                break;
            }
            if (bc->result == BRESULT_SCRIPTED) {
                if (!(*((u16 *)(D_800562C4 + 0xB6)) & 0x200)) {
                    g_currentMusicTrack = MUSIC_TRANSITION;
                    D_8005F14C = PHASE_IDLE;
                    g_vsyncRate = 1;
                    break;
                }
            }
            if (bc->result == BRESULT_FLAG) {
                if (*((u16 *)(D_800562C4 + 0xB6)) & 0x100) {
                    *((s32 *)(D_800562C4 + 0x68)) &= ~0x40;
                } else {
                    g_currentMusicTrack = MUSIC_TRANSITION;
                    D_8005F14C = PHASE_IDLE;
                    g_vsyncRate = 1;
                    break;
                }
            }

            D_8005F14C = PHASE_BATTLE;
            g_vsyncRate = ((s16)D_8005F14C == PHASE_FIELD) ? 1 : 2;

            for (i = 0; i < 3; i++) {
                if (bc->overlay_assets[i] != 0xFF) {
                    while ((s16)g_renderMode == RENDER_GAME) { }
                    while (DrawSync(1) != 0) { }
                    func_8003646C(bc->overlay_assets[i] + 5);
                }
            }
            break;

        /* ---- State 9: Disc Reload --------------------------------- */
        case 8:
            func_80038868(D_80097400[1].sector, D_80097400[1].size, OVERLAY_ADDR, 0);
            while (func_800393C8() != 0) { }
            func_80098FD4(fe->disc_change_mode);
            i = fe->disc_change_mode;
            if (i == 1) {
                func_80011E18();             /* Reload file table */
                func_80011B60();             /* Reload scene table */
                g_currentMusicTrack = fe->scene_id;
                D_8005F14C = PHASE_IDLE;
                g_vsyncRate = i;             /* Return to field */
            }
            break;
        }
    } while (fe->state != STATE_EXIT);

    /* ================================================================
     * SHUTDOWN AND RESTART
     * ================================================================ */
    func_80012D5C();                        /* Shutdown sound engine */
    func_800283DC(0);                       /* Battle cleanup */
    func_80027448();                        /* Reset battle state */
    ResetGraph(3);                          /* Reset GPU */
    goto restart;
}
#else
INCLUDE_RODATA("src/asm", 1D2C_rodata);
INCLUDE_ASM("asm/nonmatchings/1D2C", main);
#endif

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
INCLUDE_ASM("asm/nonmatchings/1D2C", BuildPrimList);

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
