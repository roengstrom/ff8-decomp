#ifndef FIELD_H
#define FIELD_H

#include "common.h"
#include "snd_cd.h"
#include "psxsdk/libgpu.h"

/*
 * ============================================================================
 * Field entity overlay (Actor / ActorAnim)
 * ============================================================================
 *
 * One field entity slot is 612 bytes (0x264). Two typedefs describe
 * the SAME 612-byte memory at @ref D_80085224, each capturing a
 * different valid type-interpretation of the bytes:
 *
 *   - @ref Actor       — bytecode VM view (@ref ScriptContext header,
 *                        msg* / field_0xNN naming, opcode handlers).
 *   - @ref ActorAnim  — animation slot view (AnimRec rows[4] /
 *                        timers[4] / animOffset / mode at the upper
 *                        half of the stack region; used by
 *                        @c func_800A355C only).
 *
 * Bytes 0x000..0x13F are the stack/animation/movement union — same
 * storage, three valid interpretations depending on which engine
 * subsystem owns the slot at a given moment. Bytes 0x140 onward are
 * named consistently across views and overlap at the same offsets.
 *
 * Cast between views with a plain pointer cast — no union member
 * notation is needed since the layouts agree at every offset that
 * isn't aliased.
 */


/**
 * @brief One slot of the @c SystemState mode-slot table at
 *        @c D_800704A8.slots, stride 28 bytes.
 *
 * Each slot encodes a mode-dispatched operation: @c mode selects which
 * code path runs in the field engine's per-frame poller, @c param
 * carries a target-entity byte (e.g. partyId), @c submode is a
 * sub-state byte cleared on init, @c timer is a halfword countdown,
 * and @c p1 / @c p2 are two halfword parameters consumed by the mode
 * body.
 */
typedef struct {
    /* 0x00 */ u8 mode;
    /* 0x01 */ u8 param;
    /* 0x02 */ u8 submode;
    /* 0x03 */ u8 pad03;
    /* 0x04 */ u16 timer;
    /* 0x06 */ u16 unk06;            /**< Cleared on submode=0 entry by @c func_800A10F4. */
    /* 0x08 */ u16 q1;
    /* 0x0A */ u16 q2;
    /* 0x0C */ u16 savedQ1;          /**< Snapshot of @c q1 captured by @c func_800A10F4. */
    /* 0x0E */ u16 savedQ2;          /**< Snapshot of @c q2 captured by @c func_800A10F4. */
    /* 0x10 */ u16 p1;
    /* 0x12 */ u16 p2;
    /* 0x14 */ u16 p3;
    /* 0x16 */ u16 p4;
    /* 0x18 */ u16 p5;
    /* 0x1A */ u16 p6;
} SystemSubMode; /* 0x1C = 28 bytes */

/**
 * @brief Field oscillator: a small waveform generator the field VM drives.
 *
 * @c mode 1 oscillates continuously; otherwise it runs once and stops.
 * Advanced by @ref func_800A17B8.
 */
typedef struct {
    /* 0x00 */ u8  mode;       /**< Dispatch mode (1 = continuous oscillation). */
    /* 0x01 */ u8  phase;      /**< Sub-state within the current mode. */
    /* 0x02 */ u8  tableIdx;   /**< Cursor into the @c D_800C3520 waveform table. */
    /* 0x03 */ u8  output;     /**< Latest interpolated value from @c func_800A0EB8. */
    /* 0x04 */ s16 amplitude;  /**< Scale applied to the sampled waveform byte. */
    /* 0x06 */ s16 start;      /**< Interpolation start value. */
    /* 0x08 */ s16 end;        /**< Interpolation end (target) value. */
    /* 0x0A */ s16 total;      /**< Interpolation duration in ticks. */
    /* 0x0C */ s16 angle;      /**< Current tick (0..total). */
} Oscillator;  /* 0x0E = 14 bytes */

/* Analog-stick direction bits carried by @c SystemState::padHeld and by the
 * derived pad word @c func_8009BEC8 receives. One bit per direction -- combine
 * them at the use site instead of defining multi-bit masks. */
#define FIELD_PAD_WALK  0x0010 /**< Selects the slow speed and step tables. Set from the
                                    stick deflection each tick by @c func_8009BEC8.
                                    @note Meaning inferred from use: the bit picks the
                                    smaller speed constant and the shorter step delta. */
#define FIELD_PAD_YLOW  0x1000 /**< Stick/d-pad Y-low. */
#define FIELD_PAD_XHIGH 0x2000 /**< Stick/d-pad X-high. */
#define FIELD_PAD_YHIGH 0x4000 /**< Stick/d-pad Y-high. */
#define FIELD_PAD_XLOW  0x8000 /**< Stick/d-pad X-low. */

/** @brief System state block (at @c D_800704A8); also aliased as @c g_fieldEntity. */
typedef struct {
    /* 0x000 */ u8 mode;            /**< Top-level engine mode; @c 4 means exit. */
    /* 0x001 */ u8 pad001;
    /* 0x002 */ s16 counter;        /**< Mode countdown, popped by the map-jump opcodes. */

    /*
     * 0x004..0x00E hold where the party lands after a map jump. The
     * opHandler_MAPJUMP family pops them from the script; func_8009AEC0 and
     * func_800BE264 consume them when the destination field comes up.
     * SPAWN_UNSET in position_x or spawnTriIdx means "no override".
     */
    /* 0x004 */ s16 position_x;     /**< Spawn X, or @c SPAWN_UNSET to use the triangle centroid. */
    /* 0x006 */ u16 position_y;     /**< Spawn Y. */
    /* 0x008 */ u16 unk008;         /**< Extra halfword popped only by @c opHandler_MAPJUMP3. */
    /* 0x00A */ s16 unk00A;         /**< Reset to 20 by @c func_8009AEC0 and scaled into the self
                                         entity's @c moveSpeed with the same factor
                                         @c func_800B6738 uses on @c D_800704B2 (134.8046875,
                                         written there as @c *69020>>9 and here as @c *17255>>7), so
                                         the entity starts exactly at that threshold.
                                         @note Both quantities are unnamed; only ever written as 20. */
    /* 0x00C */ s16 spawnTriIdx;    /**< Spawn navmesh triangle — assigned straight to @c Actor::triIdx
                                         by both @c func_8009AEC0 and @c func_800BE264.
                                         @c SPAWN_UNSET means "keep the entity where it is". */
    /* 0x00E */ u16 anim_state;     /**< Spawn animation id, copied to @c Actor::field_0x241. */
    /* 0x010 */ u16 unk010;         /**< Set to 2 by @c func_8009895C on load modes other than 0/1/2. */
    /* 0x012 */ u8 entityIndex[3];  /**< Per-active-slot field-entity index (mirror of g_fieldVars->memberSlot[]). */
    /* 0x015 */ u8 unk015;          /**< Cleared by @c opHandler_UCON along with the trigger flag. */
    /* 0x016 */ u8 pad016[0x02];
    /* 0x018 */ OffsetTable *fieldMessages; /**< The field's message table, latched from @c g_fieldMessages on load. */
    /* 0x01C */ s32 fieldStepDelta; /**< Step delta passed to @c func_800BD804 each field tick. */
    /* 0x020 */ SystemSubMode slots[8]; /**< 8 mode/param slots, stride 28; slot 0 corresponds to the legacy @c unk020..unk032 fields. */
    /* 0x100 */ u16 unk100;         /**< Seeded from @c FieldInfo::unk09 on field entry. */
    /* 0x102 */ u16 unk102;
    /* 0x104 */ u16 unk104;
    /* 0x106 */ u16 unk106;
    /* 0x108 */ volatile u16 dialogState; /**< Dialog state word (0=init, 2=run, 4=force-complete).
                                          @c volatile: the field loop polls it and re-reads it for
                                          every comparison rather than caching one load. */
    /* 0x10A */ u16 dialogTimer;    /**< Dialog timer target. */
    /* 0x10C */ u16 dialogCount;    /**< Dialog countdown — compared against @c dialogTimer to advance state. */
    /* 0x10E */ u16 field_0x10E;
    /* 0x110 */ u16 field_0x110;
    /* 0x112 */ u16 field_0x112;
    /* 0x114 */ u16 field_0x114;
    /* 0x116 */ u16 field_0x116;
    /* 0x118 */ u16 field_0x118;
    /* 0x11A */ u16 field_0x11A;
    /* 0x11C */ u16 field_0x11C;
    /* 0x11E */ u16 field_0x11E;
    /* 0x120 */ u16 field_0x120;    /**< Snapshotted misc halfword; preserved across SaveSnapshot/RestoreSnapshot. */
    /* 0x122 */ Oscillator oscillators[2]; /**< The two field oscillators @c func_800A17B8 advances
                                               each tick; their 14-byte layouts pack back to back
                                               (0x122 and 0x130) and mirror field for field. */
    /* 0x13E */ u8 pad13E[0x02];
    /* 0x140 */ s32 padHeld;        /**< Held input for pad slot 0: @c getAnimFrameParam plus analog-stick direction bits (0x8000 = X-low, 0x2000 = X-high, 0x1000 = Y-low, 0x4000 = Y-high). Built each tick by @c func_80099180. */
    /* 0x144 */ s32 padHeldPrev;    /**< Previous tick's @c padHeld, used by @c func_80099180 for direction edge-detection. */
    /* 0x148 */ s32 padPressed;     /**< Newly-pressed input for pad slot 0 (direction bit set only when not held last tick). */
    /* 0x14C */ u8 pad14C[0x04];
    /* 0x150 */ s32 unk150;         /**< Bit-6/7 source for @c func_8009A7E8 's per-entity trigger7 write; set to @c func_80030F10(padHeld) each tick. */
    /* 0x154 */ s32 unk154;         /**< Bit-6/7 mask gating @c func_8009A7E8 's write (inverse of @c unk150); previous tick's @c unk150. */
    /* 0x158 */ s32 ambientFlags;   /**< Ambient SFX/state flags; bits 6-7 gate the fade-out path in @c func_800BD9C4; set to @c func_80030F10(padPressed). */
    /* 0x15C */ u8 pad15C[0x04];
    /* 0x160 */ s32 field_0x160;    /**< Held input for pad slot 1 (@c getAnimFrameParam(1, 0)). */
    /* 0x164 */ u8 pad164[0x04];
    /* 0x168 */ s32 field_0x168;    /**< Pressed input for pad slot 1 (@c func_80027A58(1, 0)). */
    /* 0x16C */ u8 pad16C[0x14];
    /* 0x180 */ u8 unkActive180[16]; /**< 16-byte active-marker region, cleared on @c func_800BF718 mode 1 init. */
    /* 0x190 */ u8 slotActive[16];
    /* 0x1A0 */ u8 unk1A0;          /**< Mode-6 active marker, set with mode = 6 by fe_object6 opcode. */
    /* 0x1A1 */ u8 unk1A1;          /**< Cleared unconditionally by @c func_800A5700 each dialog tick. */
    /* 0x1A2 */ u8 unk1A2;          /**< Mode-7 reentry guard byte. */
    /* 0x1A3 */ u8 unk1A3;          /**< Set to 1 by @c opHandler_UCOFF on every call (re-arm guard). */
    /* 0x1A4 */ u8 unk1A4;
    /* 0x1A5 */ u8 unk1A5;          /**< Non-zero suppresses the load-mode-1 framebuffer copy. */
    /* 0x1A6 */ u8 unk1A6;          /**< Cleared by @c func_800BFBBC on full reset. */
    /* 0x1A7 */ u8 unk1A7;
    /* 0x1A8 */ u8 unk1A8;
    /* 0x1A9 */ u8 unk1A9;
    /* 0x1AA */ u8 unk1AA;
    /* 0x1AB */ u8 unk1AB;          /**< Sub-mode byte; written together with @c mode by fe_object6 opcodes. */
    /* 0x1AC */ u8 unk1AC;          /**< Passed as the mode argument to @c renderAndUpdateDisplay
                                         and @c func_80042634 on the branch @c func_800BE274 gates.
                                         @note Nothing in the decompiled tree writes it yet, so the
                                         purpose is inferred from those two argument positions only. */
    /* 0x1AD */ u8 unk1AD;          /**< Non-zero makes @c func_80099348 skip the entity-aim, blob-shadow
                                         and shimmer-ribbon passes.
                                         @note Read only there; no writer decompiled yet. */
    /* 0x1AE */ u8 unk1AE;          /**< Script-writable byte (set by opcode handler @c opHandler_COUNTERCLOCKWISETURN2, read by @c func_8009BEC8 as the distance-per-step divisor). */
    /* 0x1AF */ u8 packedFlagSlot;  /**< Last @c getPackedField2Bit result for the active dispatcher slot; written each tick by @c func_800BD9C4. */
    /* 0x1B0 */ u8 unk1B0;          /**< 1 selects the actor-pool install path on engine state 1. */
    /* 0x1B1 */ u8 unk1B1;
    /* 0x1B2 */ u8 pad1B2[0x02];
    /* 0x1B4 */ s32 field1B4;       /**< Initialised to @c 0xFFFFFF by @c func_800BFBBC on full reset. */
    /* 0x1B8 */ u8 statusBits[0x40]; /**< Packed bit-array (512 bits); set by @c opHandler_IDLOCK, cleared by @c opHandler_IDUNLOCK, zeroed during init. */
} SystemState;

extern SystemState D_800704A8;
extern SystemState g_fieldEntity;

/**
 * @brief Field-engine variable block (248 bytes) — held at @c g_gameState+0xD60
 * and aliased through the @c g_fieldVars pointer.
 *
 * The surrounding @c GameState region runs to @c 0xE60 (256 bytes); the 8
 * bytes past this struct are @c GameState.padE58 and are not part of it —
 * @ref SmInitEventAll clears exactly @c sizeof(FieldVars).
 *
 * Despite the legacy "misc3" name for the region (see
 * @ref GAMESTATE_MISC3_OFFSET), it tracks general field/world state — step
 * accumulators that drive periodic ticks, SeeD experience and rank
 * bookkeeping, sound channel handles, the packed 2-bit flag table, party
 * ordering, audio channel state, etc. Only partially mapped; fields are
 * added as their usages are identified.
 */
typedef struct {
    /* 0x00 */ u8 initTag[4];           /**< ASCII @c "FF-8" stamped here by @ref SmInitEventAll on a full reset.
                                             Write-only: no code anywhere in the executable or its overlays reads
                                             these bytes back — verified across all four ways the block is reached
                                             (the @c D_800780D8 symbol, @c g_gameState+0xD60, offset 0 through the
                                             @c g_fieldVars pointer, and the computed @c (u8*)g_fieldVars+n
                                             accesses, which only ever touch @c 0xC5). Not called "magic" for that
                                             reason — contrast @c SoundBank.magic, which @c sndValidateBank
                                             actually validates. Field scripts could read it as M-memory
                                             @c 0x00..0x03 via @c PSHM_B, and it also watermarks the block in RAM
                                             and save dumps; which of those was intended is unknown. */
    /* 0x04 */ u32 stepCounter;         /**< Total step delta accumulator, mirrored to D_80082C14. */
    /* 0x08 */ s32 seedExpStepAcc;      /**< Step accumulator: fires the SeeD level-up tick at @c 0x6000. */
    /* 0x0C */ s32 hpRegenStepAcc;      /**< Step accumulator: fires HP regen ticks at @c 8. */
    /* 0x10 */ u16 seedExp;             /**< SeeD experience (clamped to [100, 3100]; level = exp/100). */
    /* 0x12 */ u16 prevKillSum;         /**< Last frame's total enemy-kill count across all 8 chars. */
    /* 0x14 */ s32 field14;             /**< Copied from @c g_gameState[0xCDC] by @c func_800BFBBC. */
    /* 0x18 */ u16 field18;             /**< Copied from @c g_gameState[0xCE0]. */
    /* 0x1A */ u16 field1A;             /**< Copied from @c g_gameState[0xCE2]. */
    /* 0x1C */ u16 charKills[8];        /**< Snapshot of @c chars[i].kills, refreshed by @c func_800BFBBC. */
    /* 0x2C */ u16 charKos[8];          /**< Snapshot of @c chars[i].kos. */
    /* 0x3C */ u8 pad3C[0x08];
    /* 0x44 */ s32 killSum;             /**< Sum of @c chars[i].kills across all 8 characters. */
    /* 0x48 */ s32 gilMirror;           /**< Mirror of @c g_gameState.mainData.party.gil, kept in sync by fe_object6. */
    /* 0x4C */ s32 dreamGilMirror;      /**< Mirror of @c g_gameState.mainData.party.dreamGil. */
    /* 0x50 */ s32 padInitStatus;       /**< Result of @c func_801E8B58 (pad-init status), updated each field tick. */
    /* 0x54 */ u16 field54;             /**< Mirror of @c D_800704A8.field120, set by @c func_800BFBBC. */
    /* 0x56 */ u8 field56;              /**< Copy of @c D_80082C8D byte, set by @c func_800BFBBC. */
    /* 0x57 */ u8 field57;              /**< Low byte of @c D_8005F14C, set by @c func_800BFBBC. */
    /* 0x58 */ u8 field58;              /**< Used by fe_object7 dispatch (purpose TBD). */
    /* 0x59 */ u8 pad59[0x0F];
    /* 0x68 */ s32 stateFlags;          /**< Field state flags (see FIELD_STATE_*). */
    /* 0x6C */ s32 soundHandle0;        /**< Sound channel 0 handle (music; @ref SND_HANDLE_NONE = inactive). */
    /* 0x70 */ s32 soundHandle1;        /**< Sound channel 1 handle (SFX; @ref SND_HANDLE_NONE = inactive). */
    /* 0x74 */ u8 drawPointFlag[0x40];  /**< Packed 2-bit-per-entry flag table (256 entries, indexed by
                                             8-bit key). Named after the field-init debug trace, which
                                             prints its address as @c "address(DrawPointFlag)". */
    /* 0xB4 */ u16 packedFlagsStepAcc;  /**< Step accumulator: fires packed-flags processing at @c 0x2800. */
    /* 0xB6 */ u16 fieldB6;             /**< Used by fe_object7 dispatch (purpose TBD). */
    /* 0xB8 */ u16 levelUpDisplayTimer; /**< Frames remaining for the SeeD-rank-up notification (set to 150). */
    /* 0xBA */ u16 prevSeedExp;         /**< Snapshot of @c seedExp from the previous tick (for rank-change detection). */
    /* 0xBC */ u8 partyOrderA[3];       /**< Bench list (members not in active party). */
    /* 0xBF */ u8 partyOrderB[3];       /**< Bench list duplicate (initialized identically). */
    /* 0xC2 */ u8 memberSlot[3];        /**< For each active party slot, the Actor index (0xFF = none). */
    /* 0xC5 */ u8 musicVolume;          /**< Music channel volume (0..0x7F). */
    /* 0xC6 */ u8 sfxVolume;            /**< Sound-effects channel volume (0..0x7F). */
    /* 0xC7 */ s8 audioChannel0State;   /**< Audio channel 0 state byte; -1 = reset/inactive. */
    /* 0xC8 */ s8 audioChannel1State;   /**< Audio channel 1 state byte; -1 = reset/inactive. */
    /* 0xC9 */ s8 soundBankSelector;    /**< Sound bank toggle (0 or 1). */
    /* 0xCA */ s8 audioChannel2State;   /**< Audio channel 2 state byte; -1 = reset/inactive. */
    /* 0xCB */ s8 battleMusicId;        /**< Set by @c opHandler_SETBATTLEMUSIC; read sign-extended by the sound-bank loader. */
    /* 0xCC */ u8 expectedDiscId;       /**< Currently inserted disc (1..4). The intro/disc-swap screen waits for @c getDiscId() to match. */
    /* 0xCD */ u8 cameraShakeX;         /**< Camera shake X intensity, popped from stack. */
    /* 0xCE */ u8 cameraShakeY;         /**< Camera shake Y intensity, popped from stack. */
    /* 0xCF */ u8 fieldCF;              /**< Blocks random encounters while set (func_800A5D28 guard); also read by fe_object7 dispatch. */
    /* 0xD0 */ u8 padD0;
    /* 0xD1 */ u8 fieldD1;              /**< Bit 0 toggled by fe_object6 helper. */
    /* 0xD2 */ u8 sfxActiveMask;        /**< Per-slot SFX active bitmask (set on play, cleared on completion). */
    /* 0xD3 */ u8 sfxStartMask;         /**< Per-slot SFX start bitmask (set on play). */
    /* 0xD4 */ u8 sfxEntryMask;         /**< Per-slot SFX entry-table bitmask (set when slot is registered in @c D_80085300). */
    /* 0xD5 */ u8 nextSoundBank;        /**< Sound bank ID staged by MUSICCHANGE; copied into @c audioChannel0State on swap. */
    /* 0xD6 */ u8 soundLoadComplete;    /**< Set to 1 after sound bank loading finishes. */
    /* 0xD7 */ u8 padD7;
    /* 0xD8 */ u16 dialogStateMirror;   /**< Mirror of @c D_800704A8.dialogState (kept in sync by fe_object9). */
    /* 0xDA */ u16 fieldDA;
    /* 0xDC */ u16 fieldDC;
    /* 0xDE */ u16 fieldDE;
    /* 0xE0 */ u16 fieldE0;
    /* 0xE2 */ u16 fieldE2;
    /* 0xE4 */ u16 fieldE4;
    /* 0xE6 */ u16 fieldE6;
    /* 0xE8 */ u16 fieldE8;
    /* 0xEA */ u16 fieldEA;
    /* 0xEC */ u16 fieldEC;
    /* 0xEE */ u16 fieldEE;
    /* 0xF0 */ u8 fieldF0;              /**< Used by fe_object7 dispatch (purpose TBD). */
    /* 0xF1 */ u8 fieldF1;              /**< Used by fe_object7 dispatch (purpose TBD). */
    /* 0xF2 */ u8 fieldF2;              /**< Set to popped field index by fe_object7 dispatch handler. */
    /* 0xF3 */ u8 fieldF3;              /**< Mirrored to D_80082C10 when @ref FIELD_STATE_PARTY_OVERRIDE is set. */
    /* 0xF4 */ s32 angeloLearnStepAcc;  /**< Step accumulator: fires the Angelo trick learn tick at @c 0x250. */
} FieldVars; /* 0xF8 = 248 bytes */


/* FieldVars.stateFlags bits (partial map — bits are named as their usages
 * are identified; several more bits are used as bare hex elsewhere). */
#define FIELD_STATE_TRANSITION     0x8   /**< Bit 3: transition gate — its inverse is pushed to @c setTransitionFlag at field init. Set on new game. */
#define FIELD_STATE_FIELD_READY    0x10  /**< Bit 4: set on new game; returned (with bit 3) by @c getFieldStateFlags. @note Purpose uncertain. */
#define FIELD_STATE_CAMERA_SHAKE   0x40  /**< Bit 6: arms the camera shake/vibrate pass in the field tick tail. */
#define FIELD_STATE_FLAG_200       0x200 /**< Bit 9: toggled by the fe_object6 music/state helper. @note Purpose unknown. */
#define FIELD_STATE_FLAG_400       0x400 /**< Bit 10: staged from @c fieldCF by the music-state machine, then cleared. @note Purpose unknown. */
#define FIELD_STATE_PARTY_OVERRIDE 0x800 /**< Bit 11: while set, @c fieldF3 mirrors into @c g_battleConfig.unk8 / @c GameConfig.sealedFeatures and SETPARTY2 replays at field init. */
#define FIELD_STATE_FLAG_1000      0x1000 /**< Bit 12: set/cleared around the fe_object5 sound-bank swap. @note Purpose unknown. */

/** @brief Field-engine variable block (canonical extern also in gamestate.h). */
extern FieldVars *g_fieldVars;

/**
 * @brief Block C field entity — stride @c 0x18C, dispatched by
 *        @c func_800BD9C4. Same script-VM front-end as @c Eline
 *        but with only two SFX triggers at @c 0x18A / @c 0x18B.
 */
/**
 * @brief The script-VM context every field entity begins with (0x188 bytes).
 *
 * @ref Actor, @ref Eline, @ref Dline and @ref Bganime each carry one as their
 * first member, so a handler can take a @c ScriptContext* whatever kind of
 * entity is running. Handlers that need kind-specific state cast back down.
 */
typedef struct {
    /* 0x000 */ s32 stack[80];      /**< Private evaluation stack, indexed by @c stackPtr. */
    /* 0x140 */ s32 resultSlots[8]; /**< Result-slot register file (opcodes 0x08/0x09 read/write). */
    /* 0x160 */ s32 flags;
    /* 0x164 */ u16 savedPc[8];     /**< Suspended instruction pointer per script slot (@c 0xFFFF = idle). */
    /* 0x174 */ u8  scriptSlot;     /**< Script slot currently executing. */
    /* 0x175 */ u8  activeMask;     /**< One bit per script slot: set = that slot may run. */
    /* 0x176 */ u16 pc;             /**< Instruction pointer of the running slot. */
    /* 0x178 */ u16 rangeLo;        /**< Lower bound of this entity's script range. */
    /* 0x17A */ u16 rangeHi;        /**< Upper bound of this entity's script range. */
    /* 0x17C */ u8  savedStackPtr[8]; /**< Saved stack base per script slot. */
    /* 0x184 */ s8  stackPtr;       /**< Top stack slot; @c -1 when empty, pre-incremented by @c PUSH. */
    /* 0x185 */ u8  pad185[0x03];
} ScriptContext; /* 0x188 */

/**
 * @brief Actor — opcode handler / script-VM view of a field entity.
 *
 * Every field entity is a script thread: the first 320 bytes are its private
 * evaluation stack, and it holds 8 script slots, each with its own suspended
 * @c savedPc and @c savedStackPtr so one can be resumed after another runs.
 * Slot 0 is the script installed at reset; the engine enters the others on
 * events (@c func_800AE8B4 starts slot 7 and slot 6 from the per-frame tick).
 * @c actor, @c eline, @c dline and @c bganime all share this header and are
 * dispatched through the same @c g_fieldOpcodeTable.
 *
 * Every field entity is a script thread: the first 320 bytes are its private
 * evaluation stack, and it runs up to 8 concurrent scripts held in priority
 * slots (0 = highest), each with its own suspended @ref savedPc and
 * @ref savedStackPtr. @c actor, @c eline, @c dline and @c bganime all share
 * this header and are dispatched through the same @ref g_fieldOpcodeTable.
 *
 * Named for the game's own term: the field-init debug trace prints
 * @c "sizeof(actor)" for this 612-byte structure.

 */
typedef struct {
    /* 0x000 */ ScriptContext context; // Has to be first field in struct so that Actor instance can be cast as ScriptContext
    /* 0x188 */ u8 unk188;          /**< Script parameter byte. */
    /* 0x189 */ u8 unk189;          /**< Script parameter byte. */
    /* 0x18A */ u16 unk18A;         /**< Step delta added to unk188 each movement tick. */
    /* 0x18C */ u16 unk18C;         /**< Halfword saved during async msg state. */
    /* 0x18E */ u16 unk18E;         /**< Halfword saved during async msg state. */
    /* 0x190 */ s32 posX;           /**< Entity X position (fixed-point). */
    /* 0x194 */ s32 posY;           /**< Entity Y position (fixed-point). */
    /* 0x198 */ s32 posZ;           /**< Entity Z position (fixed-point). */
    /* 0x19C */ s32 moveStartX;    /**< Snapshot of @c posX taken when a move begins; the "from"
                                        end of the @c func_800A0E54 interpolation in @c func_8009BEC8. */
    /* 0x1A0 */ s32 moveStartY;    /**< Snapshot of @c posY; twin of @c moveStartX. */
    /* 0x1A4 */ s32 moveStartZ;    /**< Snapshot of @c posZ; twin of @c moveStartX. */
    /* 0x1A8 */ s32 unk1A8;
    /* 0x1AC */ s32 unk1AC;
    /* 0x1B0 */ s32 unk1B0;
    /* 0x1B4 */ s32 msgTextPtr;     /**< Message text pointer (fixed-point). */
    /* 0x1B8 */ s32 msgPosX;        /**< Message X position (fixed-point). */
    /* 0x1BC */ s32 msgPosY;        /**< Message Y position (fixed-point). */
    /* 0x1C0 */ s32 field_0x1C0;    /**< Saved message text pointer. */
    /* 0x1C4 */ s32 field_0x1C4;    /**< Saved message X position. */
    /* 0x1C8 */ s32 field_0x1C8;    /**< Saved message Y position. */
    /* 0x1CC */ u8 pad1CC[0x08];
    /* 0x1D4 */ s32 arcVelZ;       /**< Initial vertical velocity of the hop in @c func_8009BEC8 's
                                        @c msgActive @c == @c 2 arc; gravity is a fixed @c -0x3E80. */
    /* 0x1D8 */ s16 field_0x1D8;   /**< Total step count paired with @c field_0x1DA (read signed). */
    /* 0x1DA */ s16 field_0x1DA;   /**< Signed turn accumulator; @c func_8009D274 only steps the heading while it is within +/-0x100. */
    /* 0x1DC */ s16 field_0x1DC;
    /* 0x1DE */ s16 field_0x1DE;
    /* 0x1E0 */ s16 posOfsX;        /**< Display/world position offset X, added to @c posX>>12 (func_800A1CFC render + bearing origin). */
    /* 0x1E2 */ u16 field_0x1E2;
    /* 0x1E4 */ u16 field_0x1E4;
    /* 0x1E6 */ s16 posOfsY;        /**< Display/world position offset Y. */
    /* 0x1E8 */ u16 field_0x1E8;
    /* 0x1EA */ u16 field_0x1EA;
    /* 0x1EC */ s16 posOfsZ;        /**< Display/world position offset Z. */
    /* 0x1EE */ u16 field_0x1EE;
    /* 0x1F0 */ u16 field_0x1F0;
    /* 0x1F2 */ u16 field_0x1F2;
    /* 0x1F4 */ u16 field_0x1F4;
    /* 0x1F6 */ u16 radius;         /**< Collision radius (used by @c func_8009E468 overlap test). */
    /* 0x1F8 */ u16 talkRadius;     /**< Set by @c opHandler_TALKRADIUS; read alongside @c radius by @c func_8009F74C 's asymmetric overlap test. */
    /* 0x1FA */ u16 triIdx;         /**< Navmesh triangle the entity stands on; indexes @c g_fieldWalkmeshVerts.
                                         Set from a path-table entry's @c unk6 by @c func_8009BB18 and
                                         from @c D_800704A8.spawnTriIdx on field entry by @c func_8009AEC0. */
    /* 0x1FC */ u16 field_0x1FC;
    /* 0x1FE */ s16 moveSpeed;      /**< Per-tick movement speed, 8.8 fixed point (@c 256 @c = 1.0):
                                         @c func_8009D598 scales the sin/cos step vector by it before
                                         adding it to the position. Doubled on movie entry and halved
                                         on exit (@c opHandler_MOVIE / @c func_800B14C8) along with
                                         @c msgChannel and @c field_0x208, and saved/restored with the
                                         position, @c triIdx and @c field_0x241 by fe_object5. */
    /* 0x200 */ u16 msgChannel;     /**< Current message channel. */
    /* 0x202 */ u16 field_0x202;    /**< Saved channel for async restore. */
    /* 0x204 */ s16 field_0x204;
    /* 0x206 */ u16 field_0x206;
    /* 0x208 */ u16 field_0x208;
    /* 0x20A */ u16 field_0x20A;
    /* 0x20C */ u16 field_0x20C;
    /* 0x20E */ u16 field_0x20E;
    /* 0x210 */ u16 field_0x210;
    /* 0x212 */ u16 field_0x212;
    /* 0x214 */ u16 field_0x214;
    /* 0x216 */ u16 field_0x216;
    /* 0x218 */ s16 unk218;         /**< -1 = inactive (skipped by collision tests in @c func_8009E468). */
    /* 0x21A */ s16 windowId;       /**< Message window ID (read signed; @c 1 selects the inverted input mapping in @c func_8009F990). */
    /* 0x21C */ u16 field_0x21C;    /**< Saved window ID for async restore. */
    /* 0x21E */ s16 msgState;       /**< Message state (0=init, 2=complete). */
    /* 0x220 */ u16 field_0x220;
    /* 0x222 */ s16 turnTgtX;       /**< Turn/look target point X (world units). */
    /* 0x224 */ s16 turnTgtY;       /**< Turn/look target point Y. */
    /* 0x226 */ s16 turnTgtZ;       /**< Turn/look target point Z. */
    /* 0x228 */ s16 turnPitchCur;   /**< Committed pitch angle (8-bit BAM scaled); interpolation start. */
    /* 0x22A */ s16 turnPitchDst;   /**< Pitch destination, from elevation bearing (func_800A1CFC), rate-clamped. */
    /* 0x22C */ s16 turnRollCur;    /**< Committed roll angle. */
    /* 0x22E */ s16 turnRollDst;    /**< Roll destination. */
    /* 0x230 */ s16 turnYawCur;     /**< Committed yaw angle; interpolation start. */
    /* 0x232 */ s16 turnYawDst;     /**< Yaw destination, from XY bearing to @c turnTgt, rate-clamped. */
    /* 0x234 */ u16 turnLen;        /**< Turn duration in ticks (opcode-set). */
    /* 0x236 */ u16 turnTick;       /**< Turn progress tick, incremented by @c func_800A1CFC; == @c turnLen commits Dst->Cur. */
    /* 0x238 */ u8 turnPitchRate;   /**< Max per-update pitch step (BAM). */
    /* 0x239 */ u8 field_0x239;
    /* 0x23A */ u8 turnYawRate;     /**< Max per-update yaw step (BAM). */
    /* 0x23B */ u8 turnMode;        /**< 0 = idle (emits ops 0x15/0x16 when done), 1 = tracking a target. */
    /* 0x23C */ u8 msgActive;       /**< Message active flag. */
    /* 0x23D */ u8 pad23D;
    /* 0x23E */ u8 headingBase;     /**< Heading reference subtracted from the bearing to the
                                         destination (@c func_8009D274 walk-toward step). */
    /* 0x23F */ u8 unk23F;          /**< Facing/heading angle (8-bit BAM, sin/cos source per FF8 wiki entity-struct). @c func_8009A4C0 / @c func_8009A7E8 compare a trigger's bearing (@c Eline.unk19C) against it in a +/-facing window. */
    /* 0x240 */ u8 field_0x240;
    /* 0x241 */ u8 field_0x241;
    /* 0x242 */ u8 field_0x242;
    /* 0x243 */ u8 field_0x243;
    /* 0x244 */ u8 field_0x244;
    /* 0x245 */ u8 unk245;
    /* 0x246 */ u8 pad246[0x02];
    /* 0x248 */ u8 unk248;          /**< Set to 1 by @c func_8009E468 when colliding with self entity; also acts as delayed-SFX trigger 6 in @c func_800BD9C4. */
    /* 0x249 */ u8 unk249;          /**< @c 0 = enable @c unk248 update path in @c func_8009E468. */
    /* 0x24A */ u8 triggerSfx7;     /**< Delayed-SFX trigger 7 (consumed by @c func_800BD9C4); @c 2 also flips @c flags bit @c 0x20. */
    /* 0x24B */ u8 field_0x24B;
    /* 0x24C */ u8 field_0x24C;
    /* 0x24D */ u8 field_0x24D;
    /* 0x24E */ u8 field_0x24E;
    /* 0x24F */ u8 field_0x24F;
    /* 0x250 */ u8 field_0x250;
    /* 0x251 */ u8 field_0x251;
    /* 0x252 */ u8 field_0x252;
    /* 0x253 */ u8 field_0x253;
    /* 0x254 */ u8 field_0x254;
    /* 0x255 */ u8 field_0x255;
    /* 0x256 */ u8 field_0x256;
    /* 0x257 */ u8 field_0x257;
    /* 0x258 */ u8 unk258;          /**< Set from path-table entry's @c unk8 by @c func_8009BB18. */
    /** 0x259: Blob-shadow radius per octagon direction — entry @c k is the heading
     *  @c k*32 sampled by @c func_800A222C. Set by @c SHADESET (all eight alike) or
     *  @c SHADEFORM (one per direction); both scale the script value by @c 1/4. */
    /* 0x259 */ u8 shadowRadius[8];
    /* 0x261 */ u8 shadowLevel;     /**< Blob-shadow grey level, set by @c SHADELEVEL. */
    /* 0x262 */ u8 field_0x262;
    /* 0x263 */ u8 field_0x263;
} Actor;

/** @brief Push one s32 onto the actor's bytecode stack. */
#define PUSH(ctx, val) (((s32 *)(ctx))[(s8)(++(ctx)->stackPtr)] = (val))

/** @brief Pop one s32 from the actor's bytecode stack. */
#define POP(ctx) (((s32 *)(ctx))[(s8)(ctx)->stackPtr--])

/** @brief Pop one s32 then read low byte only. */
#define POP_BYTE(ctx) (*(u8 *)&POP(ctx))

/** @brief Pop one s32 then read low signed halfword (sign-extended to s32). */
#define POP_HALF(ctx) (*(s16 *)&POP(ctx))

/** @brief Read the top s32 from the actor's bytecode stack without popping. */
#define PEEK(ctx) (((s32 *)(ctx))[(ctx)->stackPtr])

/** @brief SeeD salary lookup table indexed by SeeD level (exp / 100). */
extern u16 g_seedSalaryTable[];

/**
 * @brief Field script-VM opcode dispatch table.
 *
 * 392-entry function-pointer table at @c 0x800C6760. Indices 0-17
 * are a 18-entry stack-arithmetic sub-table (ADD, SUB, MUL, DIV, MOD,
 * NEG, EQ, ..., AND, OR, XOR, ...) accessed by @c opHandler_CAL
 * (the "meta-dispatcher") which receives the sub-opcode in @c a1 and
 * tail-calls @c g_fieldOpcodeTable[a1].
 *
 * Indices 18-391 are the main opcode dispatch table. The runtime
 * dispatcher (@c func_800BEBD0 / @c func_800BD9C4) loads
 * @c g_fieldOpcodeTable + 0x48 (i.e. our index 18) as its base, so
 * wiki opcode @c N (0x000-0x175) corresponds to our index @c N + 0x12.
 * See @c src/field/opcodes.c for the full opcode-to-handler mapping.
 */

/** @brief Read the 2-bit packed flag at the given key (256-entry table). */
extern s32 getPackedField2Bit(s32 key);

/** @brief Field-only party-member status predicate (used by dialog opcodes). */
extern s32 func_800211B4(s32 partyMember, s32 code);

/** @brief Field-engine PRNG; consumed by random encounter / step ticks. */
extern s32 fieldRandom(void);

/**
 * @brief Per-entity render slot (rotational/animation state).
 *
 * One slot per field-engine entity, addressed via @ref D_800D9630. The
 * full structure is around 0x64 bytes; only the fields touched by the
 * opcode handlers are named so far.
 */
/**
 * @brief One animation frame: destination X/Y for the sprite rect.
 */
typedef struct {
    u16 x;
    u16 y;
} EntityAnimFrame; /* 4 bytes */

/**
 * @brief Animation descriptor referenced by @ref EntityDef.
 *
 * A short header (sprite rect size, frame-reload count, source X) followed
 * by the per-frame destination-coordinate table. The animation loop period
 * is stored in the first frame's @c y slot (@c frames[0].y).
 */
typedef struct {
    u8 pad0[3];
    u8 rectW;                 /**< 0x03 — sprite rect width. */
    u8 rectH;                 /**< 0x04 — sprite rect height. */
    u8 frameReload;           /**< 0x05 — value reloaded into @c slot->frameIdx. */
    u16 srcX;                 /**< 0x06 — source X passed to the frame loader. */
    EntityAnimFrame frames[1];/**< 0x08 — per-frame dest coords; @c frames[0].y doubles as the loop period. */
} EntityAnimData;

/**
 * @brief Entity definition/template referenced by @ref EntityRenderSlot.
 */
typedef struct {
    u8 pad00[0x14];
    EntityAnimData *anim;     /**< 0x14 — animation descriptor. */
    u8 pad18[0x47];
    u8 unk5F;                 /**< 0x5F — bit 0 set = entity is animated. */
} EntityDef;

/**
 * @brief The four consecutive @c s32 transform words at offset @c 0x20 of an
 *        @ref EntityRenderSlot, modelled as one 16-byte block.
 *
 * @c func_800A74B4 mode 0 overwrites all four at once via a single aggregate
 * copy; the compiler emits that as a 4-word load-all/store-all block, which is
 * why the region is a struct rather than four scalar fields.
 */
typedef struct {
    s32 field20;     /**< Scale factor; init to @c 0x1000. */
    s32 field24;     /**< Scale factor; init to @c 0x1000. */
    s32 field28;     /**< Scale factor; init to @c 0x1000. */
    s32 field2C;     /**< Mode-0 only (mode-1 leaves it untouched). */
} EntityRenderXform;

typedef struct {
    EntityDef *def;  /**< 0x00 — entity definition/template. */
    u8 pad04[0x08];
    u16 unk0C;
    u8 pad0E[0x02];
    u16 unk10;       /**< Cleared on init by @c func_800A8CDC. */
    u16 unk12;       /**< Init to @c 0x190 by @c func_800A8CDC. */
    u16 unk14;       /**< Cleared on init. */
    u8 pad16[0x02];
    u16 unk18;       /**< Cleared on init. */
    u16 unk1A;       /**< Cleared on init. */
    u16 unk1C;       /**< Cleared on init. */
    u8 pad1E[0x02];
    EntityRenderXform xform;  /**< 0x20 — transform block (field20..field2C); see @c func_800A74B4. */
    u8 pad30[0x20];
    u16 unk50;       /**< Cleared on init. */
    u16 unk52;       /**< Current motion halfword (mirror of Actor @c field_0x206). */
    u8 pad54[0x0C];
    u8 unk60;
    u8 unk61;
    u16 unk62;
    u8 pad64[0x02];
    u8 frameTimer;   /**< 0x66 — per-frame countdown; on expiry the frame advances. */
    u8 frameIdx;     /**< 0x67 — current frame index (counts down; reloaded from anim->frameReload). */
    u8 pad68[0x0A];
    u8 animFlags;    /**< 0x72 — bit0x80 = retrigger, bit0x10 = active, low nibble reused. */
    u8 pad73;
    u8 timerReload;  /**< 0x74 — reload value for frameTimer when the period is non-negative. */
    u8 pad75[0x09];
    u16 unk7E;       /**< 0x7E — animation id currently playing; @c func_8009B4A8 resets @c unk52 when it changes. */
    u8 pad80[0x04];
    u16 field84;     /**< 0x84 — snapshot of @c unk10 (dirty-check by @c func_800A7224). */
    u16 field86;     /**< 0x86 — snapshot of @c unk12. */
    s16 field88;     /**< 0x88 — snapshot of @c unk14. */
    u8 pad8A[0x02];
    u16 field8C;     /**< 0x8C — snapshot of @c unk18 (dirty-check by @c func_800A736C). */
    u16 field8E;     /**< 0x8E — snapshot of @c unk1A. */
    s16 field90;     /**< 0x90 — snapshot of @c unk1C. */
    u8 pad92[0x06];
    s32 subBuffer;   /**< @c 0x98 — caller of @c func_800A8CDC uses the returned @c &subBuffer pointer. */
} EntityRenderSlot;

/** @brief Entity render-slot pointer table; indexed by @c actor->field_0x256. */
extern EntityRenderSlot *D_800D9630[];

/** @brief Entity Actor pointer table; indexed by raw field-entity id. */
extern Actor *D_80085230[];

/** @brief Actor entity array (count @c D_80085388, stride 612). */
extern Actor *D_80085224;

/** @brief Number of entries in the @c D_80085224 entity array. */
extern u8 D_80085388;

/**
 * @brief Block B field entity — stride @c 0x1A0, dispatched by
 *        @c func_800BD9C4 alongside the full-sized @c Actor pool.
 *
 * Shares the script-VM state layout with @c Actor at offsets
 * @c 0x160..0x178, but with a 7-byte SFX trigger region at @c 0x194..0x19B
 * instead of the full @c Actor body. Used by the field engine for compact
 * actors that don't need the full 612-byte slot.
 */
typedef struct {
    /* 0x000 */ ScriptContext context; // Has to be first field in struct so that Eline instance can be cast as ScriptContext
    /* 0x188 */ s16 x0, y0, z0;         /**< Trigger line-segment start; set by @c SETLINE. */
    /* 0x18E */ s16 x1, y1, z1;         /**< Trigger line-segment end; set by @c SETLINE. */
    /* 0x194 */ u8  activeMarker;       /**< Block-active gate; @c func_8009A4C0 / @c func_8009A7E8 process the record only when @c == 1. */
    /* 0x195 */ u8  charMarker;      /**< Script-character marker; @c SETLINE stamps @c D_800DE4FC here. */
    /* 0x196 */ u8  trigger4;       /**< In-range latch: @c func_8009A4C0 sets it while the query point projects inside @c actor->radius, clears it when out; cleared with @c unk19D by @c func_8009A8E0. */
    /* 0x197 */ u8  trigger5;       /**< Edge-straddle: @c func_8009A4C0 sets it when self and the query point fall on opposite sides of the segment edge (2D cross-product signs differ). */
    /* 0x198 */ u8  trigger6;       /**< Facing hit: @c func_8009A4C0 sets it when self coincides with the projected point or lies within a +/-64 facing window. */
    /* 0x199 */ u8  trigger7;       /**< Set to 1/2 by @c func_8009A4C0 / @c func_8009A7E8 from the current pad-hold mode (@c D_800704A8 unk150/unk154 bits 0x40/0x80) when the active-marker test + diff window pass. */
    /* 0x19A */ u8  trigger2;       /**< Entered: @c func_8009A4C0 sets it on the frame the record first comes into range. */
    /* 0x19B */ u8  trigger3;       /**< Exited: @c func_8009A4C0 sets it on the frame the record leaves range. */
    /* 0x19C */ u8  unk19C;         /**< Facing angle to the projected point, written by @c func_8009A4C0 (@ref func_8009A0E8); compared (with diff bias) against @c actor->unk23F by @c func_8009A4C0 / @c func_8009A7E8. */
    /* 0x19D */ u8  unk19D;         /**< Fired-this-frame flag: @c func_8009A4C0 raises it with @c trigger6; @c func_8009A7E8 gates the pad-mode write on it; cleared by @c func_8009A8E0 alongside @c trigger4. */
    /* 0x19E */ u8  pad19E[0x02];
} Eline; /* 0x1A0 = 416 bytes — the game's "eline" (per the field-init trace). */



typedef struct {
    /* 0x000 */ ScriptContext context; // Has to be first field in struct so that Dline instance can be cast as ScriptContext
    /* 0x188 */ u8  activeMarker;
    /* 0x189 */ u8  pad189;
    /* 0x18A */ u8  trigger6;
    /* 0x18B */ u8  trigger7;
} Dline; /* 0x18C = 396 bytes — the game's "dline" (per the field-init trace). */

/**
 * @brief Block D field entity — stride @c 0x1B4, dispatched by
 *        @c func_800BD9C4. Same script-VM front-end; no SFX triggers,
 *        each tick ends with a call to @c func_800B2BA0.
 */
typedef struct {
    /* 0x000 */ ScriptContext context; // Has to be first field in struct so that Bganime instance can be cast as ScriptContext
    /* 0x188 */ s16 unk188;
    /* 0x18A */ u16 unk18A;
    /* 0x18C */ u16 unk18C;         /**< Movement/animation end value, set by @c func_800B2D40. */
    /* 0x18E */ u16 unk18E;         /**< Movement/animation start value, set by @c func_800B2D40. */
    /* 0x190 */ u16 animSpeed;      /**< Animation speed; @c BGANIMESPEED sets it, @c BGDRAW / @c BGOFF force it to 1. */
    /* 0x192 */ u16 animSpeed2;     /**< Reload partner of @ref animSpeed; written with the same value. */
    /* 0x194 */ u16 animFlag;       /**< Set by @c BGANIMEFLAG from the script stack. */
    /* 0x196 */ u8  pad196[0x06];
    /* 0x19C */ u16 shadeCounter;   /**< Shade wait counter; @c BGSHADE pops it, @c BGSHADESTOP zeros it, the opcode finishes when it reaches 0. */
    /* 0x19E */ u16 unk19E;         /**< Zeroed whenever a shade sequence starts or stops. */
    /* 0x1A0 */ u16 unk1A0;         /**< Loop counter popped by @c RBGSHADELOOP. */
    /* 0x1A2 */ u16 unk1A2;         /**< Timer popped by @c RBGSHADELOOP. */
    /* 0x1A4 */ u8  unk1A4;         /**< Per-tick counter, primed from @ref unk1A5. */
    /* 0x1A5 */ u8  unk1A5;         /**< Saved tick total. */
    /* 0x1A6 */ u8  unk1A6;
    /* 0x1A7 */ u8  unk1A7;         /**< Shade parameter block (@c 0x1A7..0x1AC), popped by @c BGSHADE. */
    /* 0x1A8 */ u8  unk1A8;
    /* 0x1A9 */ u8  unk1A9;
    /* 0x1AA */ u8  unk1AA;
    /* 0x1AB */ u8  unk1AB;
    /* 0x1AC */ u8  unk1AC;
    /* 0x1AD */ u8  unk1AD;         /**< Saved mirror of the shade block (@c 0x1AD..0x1B2). */
    /* 0x1AE */ u8  unk1AE;
    /* 0x1AF */ u8  unk1AF;
    /* 0x1B0 */ u8  unk1B0;
    /* 0x1B1 */ u8  unk1B1;
    /* 0x1B2 */ u8  unk1B2;
    /* 0x1B3 */ u8  pad1B3;
} Bganime; /* 0x1B4 = 436 bytes — the game's "bganime" (per the field-init trace). */

/** @brief Self/anchor pointer used by @c func_8009A8E0 after the
 *         party-member copy in @c opHandler_COPYINFO / @c opHandler_PCOPYINFO; also
 *         the base of the Block B entity array dispatched by @c func_800BD9C4. */
extern Eline *D_8008538C;

/** @brief Number of entries in the @c D_8008538C entity array. */
extern u8 D_800852F8;

/** @brief Block C entity-array base; count is @c D_80085228. */
extern Dline *D_80085384;

/** @brief Number of entries in the @c D_80085384 entity array. */
extern u8 D_80085228;

/**
 * @brief Per-entity latch for the line-trigger dispatcher, indexed by
 *        @c FieldLineTrigger::marker (the @ref D_80085384 entity index).
 *
 * @c func_800A5FA4 uses it to make each trigger edge fire once: the even
 * selectors only act while the byte is @c 0 (then set it), the odd ones only
 * while it is @c 1 (then clear it).
 */
extern u8 D_80070628[];

/** @brief Block D entity-array base; count is @c D_80085391. */
extern Bganime *D_800852F4;

/** @brief Number of entries in the @c D_800852F4 entity array. */
extern u8 D_80085391;

/** @brief Per-block iteration counter used by @c func_800BD9C4 inner loops. */
extern u8 D_800DE4FC;

/** @brief Field-engine opcode argument table base; indexed by @c pc. */
extern s32 *D_80085380;

/** @brief Field-engine bit-0 lock byte read by Block C of @c func_800BD9C4. */
extern u8 D_800704BD;

extern void func_80037B7C(s32 *base, s32 *outA, s32 *outB);
extern s32  func_801E8B58(void);

/**
 * @brief Animation slot record (one of four per actor).
 *
 * Used by @c func_800A355C to drive per-actor animation playback. Lives
 * inside the @c ActorAnim view (see below) at offset @c 0x80, which
 * shares storage with @c Actor.stack[32..49] — the bytecode VM only
 * uses stack slots @c 0..31 during script execution, so the upper
 * half of the stack region doubles as animation state.
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x10];
    /* 0x10 */ s16 id;              /**< Animation ID, -1 = empty slot. */
} AnimRec; /* 0x12 = 18 bytes */

/**
 * @brief Animation-mode view of a field entity (alternate typedef
 *        over the same memory as @ref Actor).
 *
 * The bytecode VM's @c stack[80] region (offsets @c 0x00..0x13F of
 * @c Actor) is repurposed as animation state once the script reaches
 * a movement opcode and surrenders its stack frame: @c rows[4]
 * occupies @c 0x80..0xC7, @c timers[4] occupies @c 0xC8..0xCF, and
 * @c animOffset / @c mode appear at @c 0xF4 / @c 0xFC. The rest of
 * the struct is identical to @c Actor from offset @c 0x140 onward.
 *
 * Use a cast (@c (ActorAnim *)&D_80085224[idx]) only inside the
 * animation engine; everywhere else, access via @c Actor.
 */
typedef struct {
    /* 0x000 */ u8 pad000[0x80];
    /* 0x080 */ AnimRec rows[4];    /**< Four animation slots, stride 0x12. */
    /* 0x0C8 */ s16 timers[4];      /**< Per-slot tick counters. */
    /* 0x0D0 */ u8 padD0[0x24];
    /* 0x0F4 */ s16 animOffset;     /**< Byte offset from @c rows[] to source row table. */
    /* 0x0F6 */ u8 padF6[0x06];
    /* 0x0FC */ s16 mode;           /**< Dispatch mode (1/2/3 = different sources). */
    /* 0x0FE */ u8 padFE[0x166];    /**< Rest of the 612-byte slot mirrors @c Actor. */
} ActorAnim; /* 0x264 = 612 bytes — second view of the same slot as @ref Actor. */

/**
 * @brief Per-slot ribbon prim buffer: the five @c LINE_G4 strips that make up
 *        one shimmer object's four-segment trail.
 */
typedef struct {
    /* 0x00 */ LINE_G4 lines[5];
} FieldRibbonPrims;  /* 0xB4 = 180 bytes */

/** @brief Per-slot tpage commands, one for each of the four ribbon segments. */
typedef struct {
    /* 0x00 */ DR_TPAGE tpages[4];
} FieldRibbonTPages;  /* 0x20 = 32 bytes */

/**
 * @brief One frame's GPU work area: the ordering table plus every prim the field
 *        render chain builds into it.
 */
typedef struct {
    /* 0x0000 */ u32 ot[0x1000];               /**< Ordering table, cleared each frame by @c ClearOTagR. */
    /* 0x4000 */ POLY_G3 shadowPrims[128];     /**< Character blob-shadow triangles (@ref func_800A222C).
                                                    128 is the region's capacity; the function walks
                                                    the array without a cap of its own. */
    /* 0x4E00 */ DR_TPAGE shadowTPages[16];    /**< Their tpage commands (again a region capacity). */
    /* 0x4E80 */ DR_ENV drawEnvPrim;           /**< Built by @c SetDrawEnv; linked into @c ot[0xFFF]. */
    /* 0x4EC0 */ u8 pad4EC0[0x40];
    /* 0x4F00 */ DR_ENV unk4F00;               /**< Second env prim; linked into @c ot[1]. */
    /* 0x4F40 */ u8 pad4F40[0x40];
    /* 0x4F80 */ u8 unk4F80[0x1018];           /**< Output block @ref func_800A06F0 fills. */
    /* 0x5F98 */ FieldRibbonPrims ribbonPrims[8];   /**< Shimmer-ribbon line strips (@ref func_800A5224). */
    /* 0x6538 */ FieldRibbonTPages ribbonTPages[8]; /**< Their tpage commands. */
} FieldFrameBuf; /* 0x6638 = 26168 bytes */


/** @brief Update one packed-flag table slot from a step tick. */
extern void func_800383B8(s32 key, s32 status);

/** @brief Trigger SeeD rank-up notification (palette transition phase 7). */
extern void setTransitionPhase7(void);

/* ======================================================================== */
/* Field-overlay SFX / animation entry tables                                */
/* ======================================================================== */

/**
 * @brief One slot in the field SFX-shadow table @ref D_80085300.
 *
 * Populated by the field-script VM when an SFX instance is registered.
 * @c rect is a 4-halfword on-screen rectangle (used by text/balloon
 * SFX), @c payload typically holds the SFX data pointer cast to s32,
 * and @c volume / @c type mirror the values previously set via
 * @c setSfxEntryVolume / @c setSfxEntityType for the slot. Distinct
 * from the runtime @c SfxEntry in @c battle.h, which is the active
 * playback state — this is just the script-VM's last-set shadow.
 */
typedef struct {
    /* 0x0 */ u16 rect[4];
    /* 0x8 */ s32 payload;
    /* 0xC */ u16 volume;
    /* 0xE */ u16 type;
} FieldSfxSlot;

/** @brief Per-slot SFX shadow table populated by field-VM SFX opcodes. */
extern FieldSfxSlot D_80085300[];

/**
 * @brief One slot in the field anim-shadow table @ref D_80085398.
 *
 * Eight halfwords of opcode-supplied animation parameters. @c flag at
 * @c 0x0 is the slot-active marker (cleared by @c func_800BD1A4),
 * @c field2..fieldE are the per-opcode arg shadow (the same values
 * forwarded to @c setupAnimEntry / @c setupAnimEntryFull / @c updateAnimEntry).
 */
typedef struct {
    /* 0x0 */ s16 flag;
    /* 0x2 */ s16 field2;
    /* 0x4 */ s16 field4;
    /* 0x6 */ s16 field6;
    /* 0x8 */ s16 field8;
    /* 0xA */ s16 fieldA;
    /* 0xC */ u16 fieldC;
    /* 0xE */ u16 fieldE;
} FieldAnimSlot;

/** @brief Per-slot anim shadow table populated by field-VM anim opcodes. */
extern FieldAnimSlot D_80085398[];

/** @brief Small on-screen rectangle in halfword coords (used by SFX balloons). */
typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Rect;

/** @brief The field's message table; the same word as @c SystemState::fieldMessages. */
extern OffsetTable *g_curFieldMessages;

/** @brief Spatial-entity dispatch context word (passed to @c func_800A8DAC). */

/** @brief Field-side dialog companion scalar. */
extern s32 D_800DE4DC;

/** @brief Stashed SFX global flag, saved/restored around dialog SFX. */
extern s32 D_800DE4D8;

/** @brief Global SFX-status flags packed scalar (tested with 0xC0 etc.). */
extern s32 D_80070600;

/** @brief Active field-script entity index (mirrors @c actor->field_0x256). */
extern u8 D_800DE4FC;
extern u8 D_800DE4FD[];
extern u8 D_80085390;
extern u8 D_800704BB;
extern u8 D_800704BC;

/**
 * @brief Field-engine status word array; @c D_800DE8C8[1] aliases
 *        @ref D_800DE8CC. The two are different views of the same
 *        memory — target asm picks one form per call site (the array
 *        form folds the base+4 into a single lhu, the scalar form
 *        uses a separate lui+addiu).
 */
extern s32 D_800DE8C8[];

/** @brief Status word at @c D_800DE8C8[1] (separate symbol for codegen). */
extern s32 D_800DE8CC;

/** @brief Dialog-companion flag byte tested by some opcodes. */
extern u8 D_800DE8D0;

/** @brief SFX/sample-load ready flag — flipped on by the @c func_800B2188
 *         load-complete callback and polled by @c EFFECTLOAD. */
extern u8 D_800DE8D5;

/** @brief Misc 4-byte field-engine slot; stash for @c func_801E8104's return
 *         value in @c MOVIEREADY / @c SPUREADY. */
extern s32 D_800DE4EC;

/** @brief Movie subsystem state pointer (or buffer base) — stored to as
 *         a single u32 by the battle-load opcodes. */
extern u8 D_800DE878[];

/* ======================================================================== */
/* Battle encounter params (populated by field-VM opcode 0x14C)             */
/* ======================================================================== */

/* EncounterParams / D_80082C90 moved to common.h (resident, shared with the
 * Triple Triad overlay's AI setup). */

/** @brief Mirrored from @ref FieldVars.fieldF3 when @c stateFlags & 0x800 is set. */
extern u8 D_80082C10;

/** @brief Stashed sound-bank selector across the battle transition. */
extern u8 D_80082C11;

/* ======================================================================== */
/* Field-side scalars consumed by fe_object7 / fe_object8 / fe_object9       */
/* ======================================================================== */

/** @brief Sound-load handshake byte for the battle-fade sequence. */
/** @brief Run-disable gate, written by @c RUNDISABLE / @c RUNENABLE.
 *         Meaning inferred from those opcode names; no reader decompiled yet. */
extern u8 D_8007064C;

/** @brief Misc menu/field share scalar. */
extern u16 D_8007737C;

/** @brief Field-side post-battle flag byte. */
extern u8 D_800773C0;

/** @brief Resident table of text strings read by the field-VM string opcodes. */
extern OffsetTable D_8005630C;

/** @brief Sound-init parameter passed to @c func_80037FB0. */
extern s32 D_8005F13C;

/** @brief Field/battle load sub-state halfword driving the @c ff8main state
 *         machine (values 0/1/2/3/6/0xA). Read as a signed halfword there
 *         (@c switch dispatch) and copied into @c FieldVars.field57 (low byte)
 *         on full field reset by @c func_800BF718. */
extern volatile s16 D_8005F14C;

/** @brief Secondary field-load flag consumed by the @c ff8main state machine
 *         (non-zero enables the incremental @c func_80038490 reload path). */
extern s16 D_8005F14A;

/** @brief Field-load music/threshold halfword (initialised to 0x49; the reload
 *         path is taken only while @c D_8005F100 < 0x4A). */
extern s16 D_8005F100;

/** @brief Cleared by @c func_8009AEC0 when the entities are placed on the
 *         navmesh; no other decompiled code reads or writes it yet. */
extern u8 D_8005F102;

/** @brief Field-load CD descriptor index passed to @c func_80038490. */
extern s32 D_8005F104;

/** @brief Misc field byte; copied into @c FieldVars.field56 on full reset. */
extern u8  D_80082C8D;

/** @brief Bitfield words tested by fe_object4 opcode handlers. */
extern s32 D_800705E8;
extern s32 D_800705F0;
extern s32 D_800705F8;

/** @brief @c &g_gameState.fieldVars exposed as a byte array for fe_object4's
 *         script-VM M-memory load/store opcodes (offsets are popped from the
 *         actor stack). */
/** @brief The field script-VM's M-memory block — the same storage as
 *         @c *g_fieldVars, addressed by byte offset by the @c PSHM_ / @c POPM_
 *         opcodes (M-offset @c N is @c FieldVars + @c N).
 *  @note A second view of memory that @ref FieldVars already describes; the
 *        opcode handlers use this byte-array form because the offset is a
 *        runtime script argument, not a named field. */
extern u8  D_800780D8[];

/** @brief Field-side status flag byte; bitfield (0x1, 0x2, 0x4, 0x8, 0x10, 0x20). */
extern u8  D_8007809A;

/** @brief Mirror of @c g_fieldVars->stepCounter (s32). */
extern u32 D_80082C14;

/** @brief Pool sizer for entity/script tables; called from @c fe_object10. */
extern s32 func_80037AEC(u8 *header, u16 *table, s32 **outBase);

/** @brief Reset to 20 on field entry by @c func_8009AEC0 and scaled by
 *         134.8046875 (@c *69020>>9) in @c func_800B6738 to form the threshold
 *         the entity's @c moveSpeed is compared against — the same scale
 *         @c func_8009AEC0 applies to @c SystemState::unk00A when seeding it.
 *  @note Both quantities are unnamed; the pair only ever holds 20. */
extern s16 D_800704B2;

/** @brief Dialog companion halfword (mirrors @c D_800DE4DC s32 view). */
extern s16 D_800DE4D0;

/** @brief Per-script slot scratch bytes used by fe_object7 dialog opcodes. */
extern s8 D_800DE4D2;
extern s8 D_800DE4D3;
extern s8 D_800DE4D4;

/** @brief Actor-pointer slots used by fe_object7 to remember last-active entities. */
extern Actor *D_800DE4F0;
extern Actor *D_800DE4F4;
extern Actor *D_800DE4F8;

/** @brief Per-text status array consumed by fe_object7 dialog flow. */
extern u8 D_800DE880[];

/**
 * @brief Field-VM event header — script blob preamble parsed by @c func_800BE30C.
 *
 * Each script blob begins with this header. Three byte flags at the
 * start, a 4th flag at offset 3, then two 16-bit offsets that point
 * to sub-tables inside the blob. The actual bytecode starts at
 * @c +0x8.
 */
typedef struct {
    /* 0x0 */ u8 field0;
    /* 0x1 */ u8 field1;
    /* 0x2 */ u8 field2;
    /* 0x3 */ u8 field3;
    /* 0x4 */ u16 offset4;
    /* 0x6 */ u16 offset6;
} EventHeader;

extern u8 D_800DE8D8;       /**< Mirror of @c EventHeader.field2 from the active script. */
extern u8 D_800DE8D9;       /**< Mirror of @c EventHeader.field0. */
extern u8 D_800DE8DA;       /**< Mirror of @c EventHeader.field1. */
extern u8 D_800DE8C0;       /**< Mirror of @c EventHeader.field3. */
extern u16 *D_800DE4E0;     /**< Pointer to script bytecode start (header base + 8); read as packed u16s. */
extern u8 *D_800DE4E4;      /**< Pointer to first sub-table (header base + offset4). */
extern u8 *D_800DE4E8;      /**< Pointer to second sub-table (header base + offset6). */

/* ======================================================================== */
/* Field-side scalars consumed by fe_object6 (SFX / camera / GF opcodes)    */
/* ======================================================================== */

/** @brief The field's SFX table, latched from @c g_fieldSfxTable on load. */
extern OffsetTable *g_curFieldSfx;

/** @brief Misc field-side scratch bytes used by fe_object6. */
extern u8 D_8007064A;
extern u8 D_8007064B;
extern u8 D_8007064D;
extern u8 D_8007064E;
extern u8 D_8007064F[];
extern u8 D_80070656;      /**< Distance-per-step divisor: @c func_8009FE18 divides the remaining
                                distance by it to get the leg's frame count. */
extern u8 D_8007065C[];
extern u8 D_80070652;
extern u8 D_800704CA;

/** @brief Field-side rotation/orientation halfword consumed by camera opcodes. */
extern u16 D_800704AA;

/** @brief Dialog dispatch mode shared with @ref D_800704A8.dialogState. */
extern u8 D_800DE8D2;

/* ======================================================================== */
/* Spatial / text / SFX helpers (defined in main binary)                    */
/* ======================================================================== */

/**
 * @brief Per-entity spatial dispatch on @ref D_800D9630 render slots.
 *
 * Dispatched on @c cmd; selected cmds:
 *   - @c 0x1E writes 4 grid-cell halfwords to @c out.
 *   - @c 0x20 writes 3 relative-offset halfwords to @c out.
 *   - @c 0x1F returns a per-entity matrix-buffer pointer (returned in @c v0).
 *   - @c 0x2F / @c 0x2E set per-entity flags / queue voice/SFX.
 *
 * Returns a per-entity pointer in @c v0 (used by callers of @c 0x1F);
 * callers that ignore the return value just drop it.
 */

/** @brief Measure a text string, returning width|height packed as one s32. */
extern s32 func_8002E680(u8 *text);

/** @brief Stash an SFX-slot scalar (paramY/Z/W/V signature). */
extern void func_8002D784(s32 sfxIdx, u8 *data, s32 paramY, s32 paramZ, s32 paramW, s32 paramV);

/** @brief Read the per-slot SFX status word at the table offset 0x?. */
extern s32 func_8002CE84(s32 idx);

/* ======================================================================== */
/* fe_object5 movie-load tables and movie-overlay (0x801E0000) entry points */
/* ======================================================================== */

/** @brief Battle-encounter scratch buffer; fe_object5 hands the pointer to
 *         @c loadBattleCmd as the asset-payload base. */
extern u8 D_800C5FB0[];

/** @brief CD-entry @c {LBA,size} pairs for movie load opcodes (op04F MOVIE). */
extern u32 D_800C2D14[];
/** @brief CD-entry @c {LBA,size,_,_} quadruples for the alt movie loader. */
extern u32 D_800C2E14[];
/** @brief CD-entry @c {LBA,size} pairs for the SPU-stream loader (op056). */
extern u32 D_800C2E1C[];

/** @brief Movie overlay (loaded at @c 0x801E0000) entry points called by
 *         the field-VM movie / SPU-stream opcodes. */
extern void func_801E8000(s32 priority);
extern s32  func_801E8104(s32 a, s32 b, s32 c, s32 d);
extern s32  func_801E82CC(void);
extern void func_801E870C(void);
extern s32  func_801E8B98(void);

#endif /* FIELD_H */
