#include "common.h"

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
INCLUDE_ASM("asm/nonmatchings/1D2C", func_800115F0);

/** @brief VSync callback handler, registered via VSyncCallback in func_800117FC.
 *
 *  Dispatches per-frame rendering based on the game mode (D_8005F146):
 *  - 0: calls func_80012B4C (normal frame draw/swap with fade)
 *  - 1: calls func_80026D8C (battle VSync handler)
 *  - 2: calls func_800D0608 (overlay-loaded VSync handler)
 *  - 3/4: calls func_800205D0 (game code VSync handler)
 *
 *  Also maintains two fixed-point frame timing accumulators (D_8005F154 and
 *  D_8005F15C) that increment by 0x88F per VSync. On bit-17 rollover (~every
 *  30 frames at 60Hz), one increments a game frame counter at D_80077378+0xCD0
 *  and the other decrements a countdown timer at D_80077378+0xCD4.
 */
INCLUDE_ASM("asm/nonmatchings/1D2C", func_8001167C);

void InitGeom(void);
void ResetCallback(void);
void ResetGraph(u8 a);
void SetDispMask(u8 a);
void SetGraphDebug(u8 a);
void SetMem(u8 a);
void StopCallback(void);
void VSyncCallback(void (*cb)(void));
void func_8003DE24(void);

extern void D_8001167C(void);
extern s8 D_8005F10C;
extern s16 D_8005F146;

/** @brief Initializes PS1 hardware subsystems.
 *
 *  Called once at the start of main. Sets up memory mode, resets the callback
 *  system, initializes GPU and GTE, starts SPU via func_8003DE24 (SpuInit
 *  wrapper), registers the VSync callback, and disables display output until
 *  the game is ready to show content.
 */
void func_800117FC(void) {
    SetMem(2);
    StopCallback();
    ResetCallback();
    ResetGraph(0);
    func_8003DE24();
    D_8005F10C = 0;
    D_8005F146 = 0;
    VSyncCallback(D_8001167C);
    SetGraphDebug(0);
    SetDispMask(0);
    InitGeom();
}

/** @brief Saves the current battle/field camera state to a snapshot buffer.
 *
 *  Records party member positions (fixed-point 20.12 → integer), rotations,
 *  animation states, and display parameters from the battle entity array
 *  (stride 612 at D_80085224) into D_80077378+0xD38..0xD5C. The snapshot
 *  is later restored by func_800119D4 when returning from battle.
 *
 *  @note Purpose uncertain for some saved fields — offsets 0x190/0x194 are
 *        likely X/Y positions and 0x1FA is likely a facing angle, inferred
 *        from the fixed-point shift and structure layout.
 */
INCLUDE_ASM("asm/nonmatchings/1D2C", func_80011870);

/** @brief Restores a previously saved camera/field state snapshot.
 *
 *  Inverse of func_80011870. Reads the snapshot from D_80077378+0xD38..0xD5C
 *  and writes values back into D_8005F14E (music track), D_800704A8 (field
 *  state structure), D_8005F158 (VSync rate), and fade state variables.
 */
INCLUDE_ASM("asm/nonmatchings/1D2C", func_800119D4);

extern volatile u16 D_8005F14C;
extern u32 D_80097410[];
extern u32 D_800974D0[];

/** @brief Loads field data set A from CD into 0x80098000.
 *
 *  Uses D_80097410[0..1] as the CD sector/size descriptor. Skips loading if
 *  D_8005F14C is 6 (menu) or 0xA (special mode), since field data is not
 *  needed in those states. Calls func_8001F5C8 (sound state reset) afterward.
 */
void func_80011A60(void) {
    if ((s16)D_8005F14C != 6 && (s16)D_8005F14C != 0xA) {
        func_80038868(D_80097410[0], D_80097410[1], 0x80098000, 0);
        while (func_800393C8() != 0)
            ;
    }
    func_8001F5C8();
}

/** @brief Loads field data set B from CD into 0x80098000.
 *
 *  Identical to func_80011A60 but uses D_800974D0[0..1] as the source
 *  descriptor. Same state-skip logic and post-load sound reset.
 *
 *  @note Purpose uncertain — the difference between data sets A and B may
 *        correspond to different field maps or disc configurations.
 */
void func_80011AE0(void) {
    if ((s16)D_8005F14C != 6 && (s16)D_8005F14C != 0xA) {
        func_80038868(D_800974D0[0], D_800974D0[1], 0x80098000, 0);
        while (func_800393C8() != 0)
            ;
    }
    func_8001F5C8();
}

/** @brief Loads a secondary data block from CD into 0x80097940.
 *
 *  Uses D_80097410[2..3] as the sector/size descriptor with synchronous
 *  CD read (func_8003882C). No state-skip check — always loads.
 *
 *  @note Purpose uncertain — destination is near the file table area,
 *        possibly a secondary asset table used during initialization.
 */
void func_80011B60(void) {
    func_8003882C(D_80097410[2], D_80097410[3], 0x80097940, 0);
    while (func_800393C8() != 0)
        ;
}

/** @brief Initializes the sound/music engine by loading audio assets from CD.
 *
 *  1. Calls func_80012CC8 to initialize the SPU hardware.
 *  2. Loads the sound bank header from CD into D_80067468 and parses it
 *     via func_80012D08.
 *  3. Loads two sample banks from CD into a scratch buffer at 0x801B0000
 *     and uploads each to SPU RAM via func_80013A5C.
 */
void func_80011BA8(void) {
    extern u8 D_80067468[];
    extern u32 D_800974D8[];

    func_80012CC8();

    func_8003882C(D_800974D8[0], D_800974D8[1], (s32)D_80067468, 0);
    while (func_800393C8() != 0)
        ;

    func_80012D08((s32)D_80067468);

    func_8003882C(D_800974D8[2], D_800974D8[3], 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;

    func_80013A5C(0x801B0000, 1);

    func_8003882C(D_800974D8[4], D_800974D8[5], 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;

    func_80013A5C(0x801B0000, 1);
}

extern u32 D_80097400[];

/** @brief Loads an overlay from CD into 0x80098000.
 *
 *  Uses D_80097400[0..1] as the sector/size descriptor. The specific overlay
 *  loaded depends on what D_80097400 was populated with (set by func_80011E18
 *  which loads the master file table).
 */
void func_80011C68(void) {
    func_8003882C(D_80097400[0], D_80097400[1], 0x80098000, 0);
    while (func_800393C8() != 0)
        ;
}

extern u32 D_80097808[];

/** @brief Loads texture data from CD and uploads it to VRAM.
 *
 *  Reads data from CD (D_80097808[0..1]) into scratch buffer 0x801B0000,
 *  then calls func_8002C3AC to process and upload the textures to GPU VRAM.
 *  Spins on DrawSync until the GPU transfer completes.
 */
void func_80011CB0(void) {
    func_8003882C(D_80097808[0], D_80097808[1], 0x801B0000, 0);
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
    extern u32 D_80097808[];
    extern u8 D_8006A468[];
    extern u8 D_8005F188[];

    func_80011C68();
    func_8002AB5C((s32)D_8006A468, 0x6000);
    func_80011CB0();

    func_8003882C(D_80097808[2], D_80097808[3], 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;

    func_80028564(0x801B0000);

    func_8003882C(D_80097808[6], D_80097808[7], 0x80090000, 0);
    while (func_800393C8() != 0)
        ;

    func_80039678((s32)D_8005F188, 0x80090000, 0x200);

    func_8003882C(D_80097808[4], D_80097808[5], 0x80090000, 0);
    while (func_800393C8() != 0)
        ;

    func_8002C100(0x80090000);
}

extern u32 D_80051694[];

/** @brief Loads the master file descriptor table from CD into 0x80097400.
 *
 *  Uses the hard-coded descriptor at D_80051694[0..1]. This must be called
 *  before any other CD loading functions, since all other load descriptors
 *  (D_80097410, D_800974D0, D_800974D8, D_80097808) reside within the
 *  table loaded here.
 */
void func_80011E18(void) {
    func_8003882C(D_80051694[0], D_80051694[1], 0x80097400, 0);
    while (func_800393C8() != 0)
        ;
}

/** @brief Game main function and primary state machine loop.
 *
 *  Initialization sequence: hardware init, CD-ROM init, loads file table,
 *  sound engine, overlays, and field/battle data.
 *
 *  Main loop runs until D_800704A8[0] == 4 (game exit). Uses D_8005F158
 *  as the state variable (decremented per iteration, dispatched via switch):
 *  - State 1: Load field audio, run overlay update.
 *  - State 2: Field-to-battle transition setup.
 *  - State 3: Battle execution and result handling.
 *  - State 4: Post-battle fade-out/transition.
 *  - State 5+: Disc change handling.
 *
 *  On exit (D_800704A8[0] == 4), shuts down sound, resets GPU, and
 *  reinitializes everything from the top.
 */
INCLUDE_ASM("asm/nonmatchings/1D2C", main);

/** @brief Sets up the GPU draw mode for both display buffers.
 *
 *  Computes a TPage value via GetTPage and applies it to both DR_MODE
 *  primitives (D_80070468 and D_80070468+0x20) for the double-buffered
 *  rendering system.
 *
 *  @param a0 Texture page semi-transparency mode (passed to GetTPage).
 *            Value 2 (8-bit) is used from func_80012B4C.
 */
INCLUDE_ASM("asm/nonmatchings/1D2C", func_800127F8);

/** @brief Initializes two full-screen black TILE primitives for screen clearing.
 *
 *  Sets up a 320x224 black fill rectangle in each of the two double-buffered
 *  primitive slots at D_80070474 (32 bytes apart). These are used for screen
 *  clearing during frame rendering.
 */
INCLUDE_ASM("asm/nonmatchings/1D2C", func_80012870);

/** @brief Copies the framebuffer from the current display buffer to the other.
 *
 *  Uses MoveImage to synchronize both framebuffers so they contain the same
 *  content. Reads source/destination coordinates from the DISPENV array at
 *  D_80067440. Spins on DrawSync until the GPU transfer completes.
 */
INCLUDE_ASM("asm/nonmatchings/1D2C", func_800128F8);

/** @brief Builds and chains the per-frame GPU primitive list for rendering.
 *
 *  Clears the ordering table for the current buffer, sets the TILE fill
 *  color to the given brightness, and chains the TILE and DR_MODE primitives
 *  into the OT for submission via DrawOTag.
 *
 *  @param a0 Fill brightness (uniform RGB). 0x10 (dark grey) is used during
 *            fade effects.
 */
INCLUDE_ASM("asm/nonmatchings/1D2C", func_800129A4);

void PutDispEnv(void *);
void PutDrawEnv(void *);
void DrawOTag(void *);

extern volatile u16 D_8005F114;
extern volatile u8 D_8005F116;
extern u8 D_8005F0FC;
extern u8 D_80067440[];
extern u8 D_80067388[];
extern u32 D_8005F128[];

/** @brief Main frame rendering function with double-buffer swap and fade effects.
 *
 *  Called from the VSync callback (func_8001167C, case 0). Toggles the
 *  double buffer index, manages fade-in/fade-out frame counters, and submits
 *  the GPU primitive list for the current frame.
 *
 *  Fade behavior depends on D_8005F116:
 *  - 0: rendering disabled, returns immediately.
 *  - 1: fade-in mode — updates every 4th frame for 128 frames total.
 *  - 2+: fade-out mode — updates every frame for 34 frames total.
 *
 *  When the fade counter expires, sets D_8005F146=0 and D_8005F116=0 to
 *  signal completion to the main loop.
 */
void func_80012B4C(void) {
    if (D_8005F116 == 0) {
        return;
    }

    D_8005F114 = D_8005F114 + 1;
    D_8005F114 = D_8005F114 & 1;

    if (D_8005F116 == 1) {
        s32 fc;
        fc = D_8005F0FC + 1;
        D_8005F0FC = fc;
        if ((fc & 0xFF) == 0x80) {
            D_8005F146 = 0;
            D_8005F116 = 0;
        }
        if (fc & 0x6) {
            return;
        }
    } else {
        D_8005F0FC = D_8005F0FC + 1;
        if (D_8005F0FC == 0x22) {
            D_8005F146 = 0;
            D_8005F116 = 0;
        }
    }

    func_800127F8(2);
    func_800129A4(0x10);
    PutDispEnv(&D_80067440[(s16)D_8005F114 * 20]);
    PutDrawEnv(&D_80067388[(s16)D_8005F114 * 92]);
    DrawOTag(&D_8005F128[(s16)D_8005F114]);
}

/** @brief Initializes the SPU (Sound Processing Unit).
 *
 *  Wrapper that calls func_80014C30 (which runs the full SPU init sequence:
 *  SpuStart, SpuInitMalloc, transfer mode setup, IRQ, and root counter timer)
 *  and returns 0.
 */
INCLUDE_ASM("asm/nonmatchings/1D2C", func_80012CC8);

/** @brief Shuts down the SPU and cleans up sound resources.
 *
 *  Wrapper that calls func_80014D20 (which stops the root counter timer,
 *  disables/closes the timer event, resets the SPU IRQ address to 0xFFFFFF,
 *  and reinitializes the SPU) and returns 0.
 */
INCLUDE_ASM("asm/nonmatchings/1D2C", func_80012CE8);
