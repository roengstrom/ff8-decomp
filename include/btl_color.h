#ifndef BTL_COLOR_H
#define BTL_COLOR_H

#include "common.h"
#include "battle.h"

/* --- Battle camera / palette transition state --- */

typedef struct {
    u16 intensity;      /* 0x00: shake intensity */
    u8 direction;       /* 0x02: shake direction */
    u8 enable;          /* 0x03: vibration enable flag */
    u16 zoom;           /* 0x04: zoom/distance (default 0x1000) */
    u8 counter;         /* 0x06: vibration timer (clamped to 0x40) */
    u8 stateSnapshot;   /* 0x07: battle state byte for change detection */
} BattleCameraState;

/** @brief Palette transition state machine (D_80083754). */
typedef struct {
    u16 state;      /* 0x00 */
    u16 pad02;      /* 0x02 */
    s16 brightness; /* 0x04 */
    s16 fade;       /* 0x06 */
    u8 srcPalette;  /* 0x08 */
    u8 dstPalette;  /* 0x09 */
    u8 timer;       /* 0x0A */
    u8 lineCount;   /* 0x0B */
    u8 oldName[3];  /* 0x0C */
    u8 newName[3];  /* 0x0F */
    u8 oldName2[6]; /* 0x12 */
    u8 newName2[6]; /* 0x18 */
} PaletteTransition;

/**
 * @brief 0x280-byte scratch region immediately before the PaletteTransition
 * at D_80083754 (a separate global). vibrateIntensity is read as the block
 * before the transition base to keep the retail lhu -640($s5) form.
 */
typedef struct {
    /* 0x000 */ u16 vibrateIntensity; /* g_cameraVibrateIntensity */
    /* 0x002 */ u8  pad[0x27E];       /* to D_80083754 */
} CameraTransitionScratch;            /* sizeof == 0x280 */

/* Load-bearing layout: size must stay 0x280 (see lhu -640($s5) access).
 * Negative-sized array typedefs fail compilation if it drifts. */
typedef char camera_transition_scratch_vibrate_ok[
    (((u32)&((CameraTransitionScratch *)0)->vibrateIntensity) == 0x000) ? 1 : -1];
typedef char camera_transition_scratch_size_ok[
    (sizeof(CameraTransitionScratch) == 0x280) ? 1 : -1];

/* --- Data externs (sorted by address) --- */

extern s32              g_gpuColor;           /* 0x800834C8 */
extern BattleCameraState g_cameraShake;       /* 0x800834D0 */
extern u16              g_cameraVibrateIntensity; /* 0x800834D4 */
extern PaletteTransition D_80083754;          /* 0x80083754 */

extern void clearEntityColor(SfxEntry *entry);
extern void buildGrayscaleGpuColor(s32 intensity);
extern void buildRgbGpuColor(s32 r, s32 g, s32 b);
extern void setDefaultGpuColor(void);
extern void btlColorStub0234(void);
extern void setCameraVibrateIntensity(s32 val);
extern void setCameraVibrateState(u32 enable);
extern void setCameraShakeParams(s32 intensity, s32 direction);
extern void updateCameraVibrate(void);
extern void resetBattleCameraState(void);
extern BattleCmdEntry *getBattleCmdTable(void);
extern BattleCmdEntry *findBestBattleCmd(s32 threshold);
extern s32  isAnyBattleCmdActive(void);
extern s32  checkBattleCmdSource(s32 cmd);
extern void deactivateBattleCmd(s32 id);
extern s32  loadBattleCmd(u8 *data, s32 idx, s32 priority);
extern void advanceBattleTimer(s32 delta);
extern void initBattleCmdEntries(void);
extern void sendSpuCommand(s32 idx);
extern void playSoundEffect(s32 idx);
extern void enableSoundReverb(s32 mask);
extern void disableSoundReverb(s32 mask);
extern u16  remapControllerInput(u16 bitmask);
extern s32  remapButtonIndex(s32 index);
extern s32  reverseButtonRemap(s32 index);
extern void btlColorStub1044(void);
extern void updatePaletteTransition(s32 arg0, s32 arg1);
extern u8  *renderBattleString(P_TAG *ot, u8 *pkt, u8 *str, s32 y, s32 width, s32 color);
extern u8  *func_80031224(P_TAG *ot, u8 *cursor, s32 leftWidth, s32 rightX);
extern void setTransitionPhase7(void);
extern void setTransitionFlag(s32 val);
extern void initBattleTransition(void);
extern s32  lerpRange(s32 rangeStart, s32 rangeEnd, s32 input, s32 maxOut);
extern void stepAnimEntries(void);
extern void clearAnimEntryActive(s32 idx);
extern void updateAnimEntry(s32 idx, s32 value);
extern void copyAnimEntryField(s32 idx, u8 *src);
extern void initAnimEntry(s32 idx, s32 flags, s32 src, s32 start, s32 end, s32 inStart, s32 inEnd);
extern void setupAnimEntry(s32 idx, s32 flags, s32 src, s32 start, s32 end, s32 inStart);
extern void setupAnimEntryFull(s32 idx, s32 flags, s32 src, s32 start, s32 end, s32 inStart, s32 inEnd);
extern void clearAnimEntries(void);
extern u8  *getBattleBuffer1(void);
extern u8  *getBattleBuffer2(void);
extern void waitBattleVSync(void);
extern u32  getBattleAllocBase(void);
extern s32  getBattleAllocSize(void);
extern void flipBattleOtBuffer(void);

void *func_8002FF34(void *ot, void *pkt, s32 stringId, s32 x, s32 y, s32 color);
s32 func_800300F8(s32 renderCtx, s32 x, s32 w, s32 y, s32 color, s32 menuColor, s32 selColor);
s32 func_800302DC(P_TAG *ot, u8 *pkt);
s32 func_80030A54(CmdStream *stream);
s32 func_80031364(P_TAG *ot, u8 *pkt);
void func_800316D4(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 renderAnimOverlay(P_TAG *ot, u8 *pkt);

#endif
