#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "common.h"
#include "character.h"
#include "field.h"
#include "psxsdk/libgpu.h"

typedef enum {
    WORLD_ENTER_FROM_FIELD = 0,
    WORLD_EXIT_TO_FIELD = 1,
    WORLD_ENTER_FROM_BATTLE = 2,
    WORLD_EXIT_TO_BATTLE = 3,
    WORLD_ENTER_FROM_MENU = 4,
    WORLD_EXIT_TO_MENU = 5
} WorldTransition;

typedef struct {
    /* 0x00 */ s8 mode;
    /* 0x01 */ s8 cmd;
    union {
        struct {
            /* 0x02 */ s8 entranceIndex; // index into WORLD_TO_FIELD_ENTRANCES
            /* 0x03 */ s8 facingDirection;
        } entrance;
        struct {
            /* 0x02 */ u8 sceneLow; // these two are 16 bit representing the battle scene id
            /* 0x03 */ u8 sceneHigh;
        } battle;
    } param;
} SceneState;

extern SceneState D_80082C8C;

/* ======================================================================== */
/* GF Save Data                                                             */
/* ======================================================================== */

/**
 * @brief Per-GF save data (stride 68 bytes).
 *
 * 16 entries at g_gameState + 0x50 (0x800773C8).
 * This is the persistent save-file GF data — distinct from the runtime
 * ability tables in g_kernel (0x80078E00, see gf.h).
 */
typedef struct {
    /* 0x00 */ u8 name[12];              /**< GF name (null-terminated). */
    /* 0x0C */ u32 exp;                  /**< Total experience points. */
    /* 0x10 */ u8 unknown10;             /**< Unknown. */
    /* 0x11 */ u8 exists;                /**< GF exists/unlocked flag. */
    /* 0x12 */ u16 hp;                   /**< Current HP. */
    /* 0x14 */ s32 completeAbilities[4]; /**< Learned ability flags (128-bit bitfield, accessed as 4 words). */
    /* 0x24 */ u8 aps[24];              /**< AP per ability slot (22 used + 2 unused). */
    /* 0x3C */ u16 kills;                /**< Kill count. */
    /* 0x3E */ u16 kos;                  /**< KO count. */
    /* 0x40 */ u8 learning;              /**< Currently learning ability index. */
    /* 0x41 */ u8 forgotten1;            /**< Forgotten abilities bitfield byte 0. */
    /* 0x42 */ u8 forgotten2;            /**< Forgotten abilities bitfield byte 1. */
    /* 0x43 */ u8 forgotten3;            /**< Forgotten abilities bitfield byte 2. */
} GfSaveData; /* 0x44 = 68 bytes */

#define GF_COUNT 16

/** @brief GfSaveData.exists bit 0: GF is unlocked/owned. */
#define GF_EXISTS 0x01

/** @brief GF IDs (index into GfSaveData array). */
enum GfId {
    GF_QUEZACOTL = 0,
    GF_SHIVA     = 1,
    GF_IFRIT     = 2,
    GF_SIREN     = 3,
    GF_BROTHERS  = 4,
    GF_DIABLOS   = 5,
    GF_CARBUNCLE = 6,
    GF_LEVIATHAN = 7,
    GF_PANDEMONA = 8,
    GF_CERBERUS  = 9,
    GF_ALEXANDER  = 10,
    GF_DOOMTRAIN  = 11,
    GF_BAHAMUT    = 12,
    GF_CACTUAR    = 13,
    GF_TONBERRY   = 14,
    GF_EDEN       = 15
};

/** @brief Get 24-bit forgotten abilities bitfield from GfSaveData. */
#define GF_GET_FORGOTTEN(gf) \
    ((u32)(gf).forgotten1 | ((u32)(gf).forgotten2 << 8) | ((u32)(gf).forgotten3 << 16))

/* ======================================================================== */
/* Shop Data                                                                */
/* ======================================================================== */

/**
 * @brief Shop inventory entry (stride 20 bytes).
 *
 * 20 entries at g_gameState + 0x950 (0x80077CC8).
 */
typedef struct {
    /* 0x00 */ u8 items[16];   /**< Flags indicating whether each item is visible in the shop. */
    /* 0x10 */ u16 visited;    /**< Shop has been visited flag. */
    /* 0x12 */ u8 pad[2];      /**< Padding. */
} ShopData; /* 0x14 = 20 bytes */

#define SHOP_COUNT 20

/* ======================================================================== */
/* Config                                                                   */
/* ======================================================================== */

/**
 * @brief Game configuration (20 bytes).
 *
 * At g_gameState + 0xAE0 (0x80077E58).
 */
typedef struct {
    /* 0x00 */ u8 battleSpeed;     /**< Battle speed setting. */
    /* 0x01 */ u8 battleMsgSpeed;  /**< Battle message speed. */
    /* 0x02 */ u8 fieldMsgSpeed;   /**< Field message speed. */
    /* 0x03 */ u8 analogVolume;    /**< Analog volume level. */
    /* 0x04 */ u16 flags;           /**< Config bitfield: bit0=ATB, bit1=sound, bit2=cursor,
                                        bit5=controller, bit6=vibration, bit7=analog,
                                        bit8=scan mode. */
    /* 0x06 */ u8 camera;          /**< Camera mode. */
    /* 0x07 */ u8 sealedAbilities; // abilities sealed in Ultimecia's Castle, see @c SealedAbilityFlag
    /* 0x08 */ u8 buttons[12];      /**< Button remapping table (L2,R2,L1,R1,Tri,Cir,X,Sq,Sel,?,?,Start). */
} GameConfig; /* 0x14 = 20 bytes */

enum SealedAbilityFlag {
    SEALED_ITEM = 0x01, // Item and command 13
    SEALED_MAGIC = 0x02,
    SEALED_GF = 0x04,
    SEALED_DRAW = 0x08,
    SEALED_COMMAND_ABILITIES = 0x10, // every other junctioned command
    SEALED_LIMIT_BREAK = 0x20,
    SEALED_RESURRECTION = 0x40
};

/** @brief GameConfig.flags bitfield. */
#define CONFIG_ATB        0x001  /**< ATB mode (0=active, 1=wait). */
#define CONFIG_SOUND      0x002  /**< Sound (0=stereo, 1=mono). */
#define CONFIG_CURSOR     0x004  /**< Cursor (0=initial, 1=memory). */
#define CONFIG_UNK_08     0x008  /**< Unknown. */
#define CONFIG_UNK_10     0x010  /**< Unknown. */
#define CONFIG_CONTROLLER 0x020  /**< Controller (0=normal, 1=customize). */
#define CONFIG_VIBRATION  0x040  /**< Vibration (0=off, 1=on). */
#define CONFIG_ANALOG     0x080  /**< Unknown (bit 7). */
#define CONFIG_SCAN       0x100  /**< Scan (0=once, 1=always). */

/** @brief Button indices into GameConfig.buttons[]. */
#define BTN_L2       0
#define BTN_R2       1
#define BTN_L1       2
#define BTN_R1       3
#define BTN_TRIANGLE 4
#define BTN_CIRCLE   5
#define BTN_CROSS    6
#define BTN_SQUARE   7
#define BTN_SELECT   8
#define BTN_START    11

/* ======================================================================== */
/* Misc1 — Party, Gil, Griever Name                                        */
/* ======================================================================== */

/**
 * @brief Miscellaneous save data block 1 (32 bytes).
 *
 * At g_gameState + 0xAF4 (0x80077E6C).
 * Contains active party members, gil, unlocked weapons, and the Griever name.
 */
typedef struct {
    /* 0x00 */ u8 partyMembers[4];     /**< Active party member IDs (slot 3 always 0xFF). */
    /* 0x04 */ u32 unlockedWeapons;    /**< Bitfield of unlocked weapon upgrades. */
    /* 0x08 */ u8 grieverName[12];     /**< Player-chosen name for Griever. */
    /* 0x14 */ u8 gfIndex0;            /**< GF index for character slot 0 (party locked mode). */
    /* 0x15 */ u8 gfIndex1;            /**< GF index for character slot 1 (party locked mode). */
    /* 0x16 */ u8 gfIndex2;            /**< GF index for character slot 2 (party locked mode). */
    /* 0x17 */ u8 trickLearning;       /**< Index (0-7) of Angelo trick currently being learned. */
    /* 0x18 */ u32 gil;                /**< Current gil. */
    /* 0x1C */ u32 dreamGil;           /**< Gil held during Laguna dream sequences. */
} PartyData; /* 0x20 = 32 bytes */

/* ======================================================================== */
/* Limit Breaks                                                             */
/* ======================================================================== */

/**
 * @brief Limit break progress data (16 bytes).
 *
 * At g_gameState + 0xB14 (0x80077E8C).
 */
typedef struct {
    /* 0x00 */ u16 quistisLimits;      /**< Quistis Blue Magic learned bitfield. */
    /* 0x02 */ u16 zellLimits;         /**< Zell Duel combos learned bitfield. */
    /* 0x04 */ u8 irvineLimits;        /**< Irvine Shot ammo unlocked. */
    /* 0x05 */ u8 selphieLimits;       /**< Selphie Slot config. */
    /* 0x06 */ u8 angeloCompleted;     /**< Angelo tricks completed bitfield. */
    /* 0x07 */ u8 angeloKnown;         /**< Angelo tricks known bitfield. */
    /* 0x08 */ u8 angeloPoints[8];     /**< Angelo trick learning points (8 tricks). */
} LimitBreakData; /* 0x10 = 16 bytes */

/* ======================================================================== */
/* Items                                                                    */
/* ======================================================================== */

/** @brief Single item slot (2 bytes). */
typedef struct {
    u8 id;       /**< Item ID (0 = empty). */
    u8 count;    /**< Item quantity (0-100). */
} ItemSlot; /* 2 bytes */


#define ITEM_SLOT_COUNT 198
#define BATTLE_ITEM_COUNT 0x20
#define BATTLE_ITEM_ID_LIMIT (BATTLE_ITEM_COUNT + 1)

/* ======================================================================== */
/* Triple Triad Cards                                                       */
/* ======================================================================== */

/**
 * @brief Triple Triad card collection (128 bytes).
 *
 * At g_gameState + 0x12E0 (0x80078658).
 */
typedef struct {
    /* 0x00 */ u8 cards[77];          /**< Common card slots (one per card type).
                                           Bit 7 = "owned / has been acquired" flag,
                                           bits 0..6 = count (0..127). Byte is 0
                                           when the card has never been obtained;
                                           becomes 0x80 | count once acquired. */
    /* 0x4D */ u8 cardLocations[33];  /**< Current location ID for each rare card (77..109).
                                           Specifies where in the world the card sits;
                                           the Queen of Cards uses this to relocate
                                           cards between regions. Confirmed values:
                                              0x00 — Used up (consumed by Card Mod)
                                              0x01 — Queen of Cards (in transit)
                                              0x10 — Garden hallway
                                              0xF0 — Squall (in the player's deck)
                                           Remaining IDs map to specific NPCs / rooms
                                           and are not yet fully cataloged. */
    /* 0x6E */ u8 rareCards[5];       /**< Rare card flags. */
    /* 0x73 */ u8 pad73;              /**< Padding. */
    /* 0x74 */ u16 victories;         /**< TT win count. */
    /* 0x76 */ u16 defeats;           /**< TT loss count. */
    /* 0x78 */ u16 draws;             /**< TT draw count. */
    /* 0x7A */ u16 pad7A;             /**< Unknown. */
    /* 0x7C */ s32 rngState;           /**< PRNG state (LCG: seed * 69069 + 1). */
} TripleTriadData; /* 0x80 = 128 bytes */

/** @brief Returns a pointer to the Triple Triad save-data block (win/loss tallies, etc.). */
extern TripleTriadData *getTripleTriadData(void);

/* ======================================================================== */
/* Chocobo World                                                            */
/* ======================================================================== */

/**
 * @brief Chocobo World save data (64 bytes).
 *
 * At g_gameState + 0x1360 (0x800786D8).
 */
typedef struct {
    /* 0x00 */ u8 flags;              /**< Flags: bit0=enabled, bit1=in world, bit2=MiniMog found,
                                           bit3=Demon King defeated, bit4=Koko kidnapped,
                                           bit5=Hurry!, bit6=Koko met, bit7=Event Wait off. */
    /* 0x01 */ u8 level;              /**< Boko's level. */
    /* 0x02 */ u8 currentHp;          /**< Boko's current HP. */
    /* 0x03 */ u8 maxHp;              /**< Boko's max HP. */
    /* 0x04 */ u16 weapon;            /**< Weapon bitfield (4 bits per weapon). */
    /* 0x06 */ u8 rank;               /**< Boko's rank. */
    /* 0x07 */ u8 move;               /**< Move type (1-6). */
    /* 0x08 */ u32 saveCount;         /**< Chocobo World save count. */
    /* 0x0C */ u16 idRelated;         /**< Unknown ID field. */
    /* 0x0E */ u8 pad0E[6];           /**< Unknown. */
    /* 0x14 */ u8 itemClassACount;    /**< Class A items found. */
    /* 0x15 */ u8 itemClassBCount;    /**< Class B items found. */
    /* 0x16 */ u8 itemClassCCount;    /**< Class C items found. */
    /* 0x17 */ u8 itemClassDCount;    /**< Class D items found. */
    /* 0x18 */ u8 pad18[16];          /**< Unknown. */
    /* 0x28 */ u32 associatedSaveId;  /**< Associated main save ID. */
    /* 0x2C */ u8 pad2C;              /**< Unknown. */
    /* 0x2D */ u8 bokoAttack;         /**< Boko attack level (star count bitfield). */
    /* 0x2E */ u8 pad2E;              /**< Unknown. */
    /* 0x2F */ u8 homeWalking;        /**< Home walking distance. */
    /* 0x30 */ u8 pad30[16];          /**< Unknown. */
} ChocoboWorldData; /* 0x40 = 64 bytes */

/* ======================================================================== */
/* Full Game State Layout                                                   */
/* ======================================================================== */

/**
 * @brief Game state section offsets from g_gameState (0x80077378).
 *
 * The first 0x50 bytes are header/padding before MAIN data begins.
 */
#define GAMESTATE_GFS_OFFSET        0x050  /**< GfSaveData[16] (1088 bytes). */
#define GAMESTATE_PERSOS_OFFSET     0x490  /**< CharacterData[8] (1216 bytes). */
#define GAMESTATE_SHOPS_OFFSET      0x950  /**< ShopData[20] (400 bytes). */
#define GAMESTATE_CONFIG_OFFSET     0xAE0  /**< GameConfig (20 bytes). */
#define GAMESTATE_PARTY_DATA_OFFSET 0xAF4  /**< PartyData (32 bytes). */
#define GAMESTATE_LIMITB_OFFSET     0xB14  /**< LimitBreakData (16 bytes). */
#define GAMESTATE_ITEMS_OFFSET      0xB24  /**< ItemData (428 bytes). */
#define GAMESTATE_MISC2_OFFSET      0xCD0  /**< Battle vars / misc (144 bytes). */
#define GAMESTATE_MISC3_OFFSET      0xD60  /**< Steps, SeeD rank, counters: 256-byte region holding the 248-byte @ref FieldVars plus @c padE58. */
#define GAMESTATE_FIELD_OFFSET      0xE60  /**< Field script vars, TT rules (1024 bytes). */
#define GAMESTATE_WORLDMAP_OFFSET   0x1260 /**< World map position/vehicles (128 bytes). */
#define GAMESTATE_TTCARDS_OFFSET    0x12E0 /**< TripleTriadData (128 bytes). */
#define GAMESTATE_CHOCOBO_OFFSET    0x1360 /**< ChocoboWorldData (64 bytes). */
#define GAMESTATE_TOTAL_SIZE        0x13A0 /**< Total: 80 + 4944 = 5024 bytes. */

/**
 * @brief Party, inventory, and battle state data (0x244 = 580 bytes).
 *
 * Covers GameState offsets 0xAF4–0xD38. Copied as a single block
 * during tutorial save/restore.
 */
typedef struct {
    /* 0x00 */ PartyData     party;                       /**< Party/gil/Griever (32 bytes). */
    /* 0x20 */ LimitBreakData limitBreaks;                /**< Limit break progress (16 bytes). */
    /* 0x30 */ u8            battleOrder[32];              /**< Battle item menu ordering. */
    /* 0x50 */ ItemSlot      itemSlots[198];  /**< Item inventory (198 slots). */
    /* 0x1DC */ volatile s32 playTimeSeconds;             /**< @c 0xCD0: play time in seconds; @ref VsyncHandler steps it every ~59.8 vsyncs (a second on NTSC). */
    /* 0x1E0 */ volatile s32 countdownTimer; /**< @c 0xCD4: countdown in seconds. Set/get by field event opcodes, decremented by @ref VsyncHandler at the play time's rate while nonzero; drawn as MM:SS through @c g_engine.countdown. */
    /* 0x1E4 */ u8           pad1E4[0x04];
    /* 0x1E8 */ s32          fieldCDC;                     /**< Snapshotted by @c func_800BFBBC into @c FieldVars.field14. */
    /* 0x1EC */ u16          fieldCE0;                     /**< Snapshotted by @c func_800BFBBC into @c FieldVars.field18. */
    /* 0x1EE */ u16          fieldCE2;                     /**< Snapshotted by @c func_800BFBBC into @c FieldVars.field1A. */
    /* 0x1F0 */ u8           pad1F0[0x20];                 /**< Battle vars / misc (continued). */
    /* 0x210 */ u32          array210[2];
    /* 0x218 */ u32          array[1];                      /* used in func_8009F52C */
    /* 0x21C */ u8           pad21C[0x10];                 /**< Battle vars / misc (continued). */  
    /* 0x22C */ u16          fieldD20;                    /**< Unknown (zeroed on save init). */
    /* 0x22E */ u8           partyLockFlag;               /**< Party/menu lock bits (see PARTY_LOCK_*). */
    /* 0x22F */ u8           unk2F[16];                 /**< Battle vars / misc (continued). (SIZE NOT CONFIRMED) */
    /* 0x23F */ u8           tutoEntryCount;              /**< Current tutorial section entry count.
                                                               Read/written as g_gameState.mainData.tutoEntryCount in menututo.
                                                               0 greys out the section in the list;
                                                               divided by 10 gives the page count. */
    /* 0x240 */ u8           pad40[4];                    /**< Battle vars / misc (continued). */
} SaveMainData; /* 0x244 = 580 bytes */

typedef struct {
    /* 0x00 */ u16 gameMode;
    /* 0x02 */ u16 fieldId;
    /* 0x04 */ u16 previousFieldId;
    /* 0x06 */ u16 positionsX[3];    /**< Party member X positions (>>12 integer). */
    /* 0x0C */ u16 positionsY[3];    /**< Party member Y positions (>>12 integer). */
    /* 0x12 */ u16 triIdxs[3]; /**< Navmesh triangle each party member stands on. */
    /* 0x18 */ u8 facingDirection[3];
    /* 0x1B */ u8  fade1;
    /* 0x1C */ u8  fade0;
} ReturnState; /* 0x1E = 30 bytes (29 data + 1 trailing pad, u16-aligned) */

/**
 * @brief Full game state (g_gameState at 0x80077378, 0x13A0 bytes).
 *
 * Sections not yet mapped to named structs are left as padding arrays.
 * Use this for struct-based field access; for raw pointer arithmetic
 * use (s32)&g_gameState to prevent CC1PSX symbol+constant folding.
 */
typedef struct {
    /* 0x000 */ u8                pad000[4];                   /**< Save header prefix. */
    /* 0x004 */ u16               unk004;
    /* 0x006 */ u8                pad006[0x018 - 0x006];
    /* 0x018 */ u8                squallName[12];              /**< Squall's name (null-terminated). */
    /* 0x024 */ u8                rinoaName[12];               /**< Rinoa's name (null-terminated). */
    /* 0x030 */ u8                angeloName[12];              /**< Angelo's name (null-terminated). */
    /* 0x03C */ u8                bokoName[12];                /**< Boko's name (null-terminated). */
    /* 0x048 */ u8                unk048;
    /* 0x049 */ u8                pad049[7];
    /* 0x050 */ GfSaveData        gfs[GF_COUNT];               /**< GF save data (16 × 68 bytes). */
    /* 0x490 */ CharacterData     chars[CHARACTER_COUNT];      /**< Character data (8 × 152 bytes). */
    /* 0x950 */ ShopData          shops[SHOP_COUNT];           /**< Shop inventory (20 × 20 bytes). */
    /* 0xAE0 */ GameConfig        config;                      /**< Game config (20 bytes). */
    /* 0xAF4 */ SaveMainData      mainData;                     /**< Party/items/battle state (580 bytes). */
    /* 0xD38 */ u8                battleParty[4];              /**< Battle party member IDs (mirrors party.partyMembers). */
    /* 0xD3C */ u8                padD3C[4];        /**< Battle vars / misc. */
    /* 0xD40 */ ReturnState returnState;
    /* 0xD5E */ u8                padD5D[2];        /**< Battle vars / misc (continued). */
    /* 0xD60 */ FieldVars         fieldVars;                   /**< Steps, SeeD rank, counters (@c &g_gameState.fieldVars == @c g_fieldVars). */
    /* 0xE58 */ u8                padE58[0x08];                /**< Trailing 8 bytes of the 0xD60 region; not part of @ref FieldVars (the field-reset wipe stops at 0xE58) and read by nothing so far. */
    /* 0xE60 */ u8                padE60[0x400];               /**< Field script vars, TT rules. */
    /* 0x1260 */ u8               pad1260[0x80];               /**< World map position/vehicles. */
    /* 0x12E0 */ TripleTriadData  cards;                    /**< Triple Triad data (128 bytes). */
    /* 0x1360 */ ChocoboWorldData chocobo;                 /* D_800786D8 *< Chocobo World data (64 bytes). */
} GameState; /* 0x13A0 = 5024 bytes */

/* SaveMainData.partyLockFlag bits. Only bit 0 has a confirmed meaning; the
 * rest are placeholder names pending identification. */
#define PARTY_LOCK_LOCKED  0x01  /**< Party composition is locked (fixed story party). */
#define PARTY_LOCK_FLAG_02 0x02  /**< Purpose unknown; set by the field engine. */
#define PARTY_LOCK_FLAG_10 0x10  /**< Purpose unknown; set and cleared by the field engine. */
#define PARTY_LOCK_FLAG_20 0x20  /**< Purpose unknown; set and cleared by the field engine. */

/** @brief Main game state (BSS at 0x80077378). */
extern GameState g_gameState;

/** @brief Pointer to the SeeD/world sub-region of @c g_gameState (@c &g_gameState.fieldVars). */
extern FieldVars *g_fieldVars;

/** @brief Dialog text speed for each Config "Message speed" slider position
 * (@c GameConfig.fieldMsgSpeed): 0x1C00 (1.75 characters a frame)
 * down to 0x0C00. */
extern u16 g_textSpeeds[];

typedef enum {
    RENDER_IDLE = 0,
    RENDER_FADE = 1,
    RENDER_BATTLE_TRANSITION = 2,
    RENDER_BATTLE = 3,
    RENDER_BATTLE_RESULTS = 4
} RenderMode;

extern volatile s16 g_renderMode;
extern u8 D_8005F150;
extern u8 D_8005F151;

typedef enum {
    GAME_MODE_NONE = 0,
    GAME_MODE_FIELD = 1,
    GAME_MODE_WORLD = 2,
    GAME_MODE_BATTLE = 3,
    GAME_MODE_BATTLE_DEBUG_MENU = 4,
    GAME_MODE_BATTLE_RESULTS = 5,
    GAME_MODE_MENU = 6,
    GAME_MODE_CARD = 8,
    GAME_MODE_DISC_CHANGE = 9,
    GAME_MODE_SCRIPT_MENU = 10,
    GAME_MODE_EXIT = 100
} GameMode;

/* Display double-buffer state (main-binary; shared with tripletriad). */
extern volatile s16 g_gameMode;
extern DISPENV      g_dispEnvs[2];
extern DRAWENV      g_drawEnvs[2];
extern s8           g_fadeCounter; /* signed: counts toward 0 */

/** @brief Bit @c 0x10 mirrors into @c FieldVars.field58 on full field reset. */
extern u8  D_80078DF8;

/** @brief Per-area sub-table pointer (snapshot of @c D_800DE4E4); written by @c func_800BFBBC. */
extern u16 *D_800852F0;

/* --- Save / GF / chocobo-world state setters --- */
extern void setGfExists(s32 gfId);

/** @brief Card / character refresh hook: invoked when a party member
 *         changes; refreshes derived character data. */
extern void func_80036B90(s32 charIndex);

extern void func_80036C74(void);

/** @brief Companion to @ref func_80036B90 — applies a bitmask of flags
 *         to the active-party char records. */
extern void func_80036D44(s32 mask);

/** @brief Set the party leader to @p charId. */
extern void setPartyLeader(s32 charId);

/** @brief Bitmask of characters currently available to the party. */
extern u32 func_80036EC0(void);

extern void func_800370AC(s32 arg0);
extern u8 *getChocoboWorldPtr(void);
extern void enableChocoboWorld(void);

#endif /* GAMESTATE_H */
