#include "common.h"
#include "menu.h"
#include "menumain.h"
#include "menumain2.h"
#include "menumain2b.h"
#include "menu_tint.h"
#include "menusav.h"
#include "gamestate.h"
#include "overlay.h"
#include "snd_sfx.h"
#include "party_stats.h"
#include "cdrom.h"
#include "menumain3.h"
#include "btl_anim.h"
#include "psxsdk/libetc.h"
#include "psxsdk/kernel.h"
#include "memcard.h"
#include "game.h"
#include "render.h"

#ifdef ADD_OMITTED_INCLUDES
#include "snd_init.h"
#endif

extern u8 D_801EC294;
extern u8 D_801EC301;
extern s32 D_8005620C;
extern u8 D_80097424[];
extern u16 D_801EBD24[];
extern s32 D_801EC2E4;

typedef enum {
    SAVE_MENU_INIT = 0x00,
    SAVE_MENU_CARD_SELECT_FADE_IN = 0x01,
    SAVE_MENU_TITLE_INIT = 0x02,
    SAVE_MENU_UNK_03 = 0x03,
    SAVE_MENU_UNK_04 = 0x04,
    SAVE_MENU_UNK_05 = 0x05,
    SAVE_MENU_TITLE_INPUT = 0x06,
    SAVE_MENU_UNK_07 = 0x07,
    SAVE_MENU_CARD_SELECT_ENTER = 0x08,
    SAVE_MENU_CARD_CHECK_FAILED = 0x09,
    SAVE_MENU_UNK_0A = 0x0A,
    SAVE_MENU_UNK_0B = 0x0B,
    SAVE_MENU_CARD_SELECT_INPUT = 0x0C,
    SAVE_MENU_UNK_0D = 0x0D,
    SAVE_MENU_UNK_0E = 0x0E,
    SAVE_MENU_CHOCOBO_WAIT = 0x0F,
    SAVE_MENU_UNK_10 = 0x10,
    SAVE_MENU_UNK_11 = 0x11,
    SAVE_MENU_LIST_OPEN = 0x12,
    SAVE_MENU_LIST_PREPARE = 0x13,
    SAVE_MENU_LIST_CHECK_CARD = 0x14,
    SAVE_MENU_LIST_READ_DIRECTORY = 0x15,
    SAVE_MENU_LIST_NEXT_BLOCK = 0x16,
    SAVE_MENU_LIST_WAIT_CARD = 0x17,
    SAVE_MENU_LIST_WAIT_READ = 0x18,
    SAVE_MENU_UNK_19 = 0x19,
    SAVE_MENU_LIST_LOADED = 0x1A,
    SAVE_MENU_LIST_ENTER = 0x1B,
    SAVE_MENU_UNK_1C = 0x1C,
    SAVE_MENU_LIST_SHOW = 0x1D,
    SAVE_MENU_LIST_INPUT = 0x1E,
    SAVE_MENU_NO_CARD = 0x1F,
    SAVE_MENU_NO_CARD_WAIT = 0x20,
    SAVE_MENU_PAGE_LEFT_START = 0x21,
    SAVE_MENU_PAGE_LEFT_SLIDE = 0x22,
    SAVE_MENU_PAGE_RIGHT_START = 0x23,
    SAVE_MENU_PAGE_RIGHT_SLIDE = 0x24,
    SAVE_MENU_FORMAT_PROMPT_OPEN = 0x25,
    SAVE_MENU_UNK_26 = 0x26,
    SAVE_MENU_UNK_27 = 0x27,
    SAVE_MENU_UNK_28 = 0x28,
    SAVE_MENU_UNK_29 = 0x29,
    SAVE_MENU_UNK_2A = 0x2A,
    SAVE_MENU_UNK_2B = 0x2B,
    SAVE_MENU_FORMAT_OK = 0x2C,
    SAVE_MENU_UNK_2D = 0x2D,
    SAVE_MENU_FORMAT_FAILED = 0x2E,
    SAVE_MENU_UNK_2F = 0x2F,
    SAVE_MENU_NO_EMPTY_BLOCKS = 0x30,
    SAVE_MENU_NO_SAVE_DATA = 0x31,
    SAVE_MENU_MESSAGE_WAIT = 0x32,
    SAVE_MENU_OVERWRITE_OPEN = 0x33,
    SAVE_MENU_OVERWRITE_INPUT = 0x34,
    SAVE_MENU_SAVE_CHECK_FORMAT = 0x35,
    SAVE_MENU_UNK_36 = 0x36,
    SAVE_MENU_UNK_37 = 0x37,
    SAVE_MENU_SAVE_WRITE = 0x38,
    SAVE_MENU_SAVE_WRITE_DONE = 0x39,
    SAVE_MENU_UNK_3A = 0x3A,
    SAVE_MENU_UNK_3B = 0x3B,
    SAVE_MENU_UNK_3C = 0x3C,
    SAVE_MENU_UNK_3D = 0x3D,
    SAVE_MENU_UNK_3E = 0x3E,
    SAVE_MENU_SAVE_WRITE_2 = 0x3F,
    SAVE_MENU_UNK_40 = 0x40,
    SAVE_MENU_SAVE_DONE = 0x41,
    SAVE_MENU_SAVE_FAILED = 0x42,
    SAVE_MENU_SAVE_NO_ROOM = 0x43,
    SAVE_MENU_UNK_44 = 0x44,
    SAVE_MENU_LOAD_OPEN = 0x45,
    SAVE_MENU_UNK_46 = 0x46,
    SAVE_MENU_UNK_47 = 0x47,
    SAVE_MENU_LOAD_WAIT = 0x48,
    SAVE_MENU_LOAD_VERIFY = 0x49,
    SAVE_MENU_LOAD_READ_FAILED = 0x4A,
    SAVE_MENU_LOAD_DAMAGED = 0x4B,
    SAVE_MENU_UNK_4C = 0x4C,
    SAVE_MENU_CHOCOBO_CHANGED = 0x4D,
    SAVE_MENU_UNK_4E = 0x4E,
    SAVE_MENU_EXIT_START = 0x4F,
    SAVE_MENU_EXIT = 0x50,
} SaveMenuState;

typedef enum {
    BLOCK_UNUSED = 0,
    BLOCK_FF8_SAVE = 1,
    BLOCK_DAMAGED = 2,
    BLOCK_OTHER_TITLE = 3,
    BLOCK_NO_CARD = 4,
    BLOCK_CHOCOBO_WORLD = 5,
    BLOCK_CHOCOBO_WORLD_FAILED = 6,
} BlockStatus;

typedef struct {
    /** 0x00 */ s32 task[4];
    /** 0x10 */ u16 state;
    /** 0x12 */ u8 pad12[0xE];
    /** 0x20 */ s32 helpMsg;
    /** 0x24 */ s32 infoMsg;
    /** 0x28 */ s32 fileHandle;
    /** 0x2C */ u8 hasPocketStation[2];
    /** 0x2E */ u8 cardStatus[2];
    /** 0x30 */ u16 cardColumnWidth; // on the memory card select screen, box for the "Final Fantasy VIII" text
    /** 0x32 */ u16 cardColumnX; // x position of the column holding the memory card boxes on memory card select screen
    /** 0x34 */ u16 chocoboColumnWidth; // on memory card select screen, box width for Chocobo world container
    /** 0x36 */ u16 chocoboColumnX; // on memory card select screen, x position of the chocobo world column
    /** 0x38 */ u16 slotLabelWidth; // on the memory card select screen, box for the Slot 1/Slot 2 text
    /** 0x3A */ u8 pad3A[2];
    /** 0x3C */ s16 timer;
    /** 0x3E */ u16 progress;
    /** 0x40 */ u16 progressLimit;
    /** 0x42 */ s16 pageSlide;
    /** 0x44 */ u8 pad44[2];
    /** 0x46 */ s16 brightness;
    /** 0x48 */ u8 layout;
    /** 0x49 */ u8 isLoading;
    /** 0x4A */ u8 selectedBlock;
    /** 0x4B */ u8 anyCardReady;
    /** 0x4C */ u8 unk4C;
    /** 0x4D */ u8 freeBlocks;
    /** 0x4E */ u8 scanBlock;
    /** 0x4F */ u8 yesNoCursor;
    /** 0x50 */ u8 titleCursor;
    /** 0x51 */ u8 showProgress;
    /** 0x52 */ u8 savedPopupTimer;
    /** 0x53 */ u8 cardReadyResult;
    /** 0x54 */ u8 noCardPolls;
    /** 0x55 */ u8 showChocoboColumn;
    /** 0x56 */ u8 columnCursor;
    /** 0x57 */ u8 pollPort;
    /** 0x58 */ u8 selectedCard;
    /** 0x59 */ u8 unk59;
    /** 0x5A */ u16 writeSize;
    /** 0x5C */ u16 titleMenuX;
    /** 0x5E */ u8 readRetries;
} SaveMenuCtx;

typedef struct {
    /** 0x00 */ u8 pad0[0x4C];
    /** 0x4C */ u32 saveSequence;
    /** 0x50 */ u8 pad50[2];
    /** 0x52 */ u8 unk52[4];
} SaveBlockHeader;

typedef struct {
    /** 0x000 */ u16 checksum;
    /** 0x002 */ u8 pad2[8];
    /** 0x00A */ u16 unkA;
    /** 0x00C */ u8 padC[0xCC4];
    /** 0xCD0 */ s32 playTimeSeconds;
    /** 0xCD4 */ u8 padCD4[8];
    /** 0xCDC */ u16 fieldCDC;
    /** 0xCDE */ u8 padCDE[2];
    /** 0xCE0 */ u16 fieldCE0;
    /** 0xCE2 */ u16 fieldCE2;
    /** 0xCE4 */ u8 padCE4[0x6BC];
    /** 0x13A0 */ u16 checksumCopy;
} SaveData;

typedef struct {
    /** 0x00 */ s32 task[4];
    /** 0x10 */ u16 unk10;
    /** 0x12 */ u8 pad12[4];
    /** 0x16 */ u16 unk16;
    /** 0x18 */ u16 unk18;
    /** 0x1A */ u16 unk1A;
    /** 0x1C */ u8 pad1C[4];
    /** 0x20 */ s32 unk20;
    /** 0x24 */ s32 unk24;
    /** 0x28 */ u8 pad28[4];
    /** 0x2C */ s16 unk2C;
    /** 0x2E */ s16 unk2E;
    /** 0x30 */ s16 unk30;
    /** 0x32 */ s16 unk32;
    /** 0x34 */ s16 unk34;
    /** 0x36 */ s16 unk36;
    /** 0x38 */ s16 unk38;
    /** 0x3A */ u8 unk3A;
    /** 0x3B */ u8 unk3B;
    /** 0x3C */ u8 unk3C;
    /** 0x3D */ u8 unk3D;
    /** 0x3E */ u8 unk3E;
    /** 0x3F */ s8 unk3F;
    /** 0x40 */ u8 pad40;
    /** 0x41 */ u8 unk41;
    /** 0x42 */ u8 unk42;
    /** 0x43 */ u8 pad43;
    /** 0x44 */ u8 unk44;
    /** 0x45 */ u8 unk45;
    /** 0x46 */ s8 unk46;
    /** 0x47 */ s8 unk47;
    /** 0x48 */ u8 unk48;
    /** 0x49 */ u8 unk49;
    /** 0x4A */ u8 unk4A;
    /** 0x4B */ u8 unk4B;
    /** 0x4C */ u8 pad4C[2];
    /** 0x4E */ u8 unk4E;
    /** 0x4F */ u8 unk4F;
    /** 0x50 */ u8 unk50[4];
    /** 0x54 */ s16 unk54;
    /** 0x56 */ u8 unk56;
} func_801E7BE8_arg0;

#define FUNC_801E7BE8_UNK45_FLAG_1 (1 << 1)
#define CHOCOBO_WORLD_FLAG_2 (1 << 2)

typedef struct {
    /** 0x00 */ u8 pad0[0x52];
    /** 0x52 */ u8 unk52[4];
    /** 0x56 */ u8 pad56[0x2A];
} func_801E7BE8_sp40;

typedef struct {
    /** 0x00 */ u8 pad0[3];
    /** 0x03 */ u8 unk3;
    /** 0x04 */ char unk4[0xA];
    /** 0x0E */ u8 unkE;
    /** 0x0F */ u8 padF[0x19];
    /** 0x28 */ s32 unk28;
} func_801E7BE8_D_801D1000;

typedef struct {
    /** 0x00 */ u8 pad0[0x17];
    /** 0x17 */ u8 unk17;
    /** 0x18 */ u8 pad18[0x2C];
} D_801EBD5C_Entry;

extern u8 D_80085148[];
extern u32 highestSaveSequence;
extern u8 D_801EC2A0[];
extern u16 D_801EC290;
extern u16 D_801EC292;
extern u16 D_80083792[];
extern char D_801EB9E0[];
extern char D_801EB9E4[];
extern char D_8005624C[];
extern char D_80056260[];
extern u8 D_801EBD3C[];
extern u16 D_801EBD4C[];
extern u16 D_801EBD54[];
extern D_801EBD5C_Entry D_801EBD5C[];
extern u8 D_801EC08C[];
extern struct DIRENTRY D_801EC2B0;
extern s32 D_801EC2D0;
extern u16 D_801EC2E0;
extern u8 D_801EC2F4;
extern s32 D_801EC2F8;

void initCardDataBlock(void);
void initCardEventHandlers(void);
void setModeBit(s32);
void dispatchVsyncCallback(void);
s32 pollCardReady(s32);
s32 pollCardStatus(s32);
s32 openFirstFileRetry(s32, char *, struct DIRENTRY *);
s32 cardNextFile(struct DIRENTRY *);
s32 btlStrlen(char *);
s32 compareBytes(void *, void *, s32);
s32 readCardFileSync(s32, char *, s32, s32, s32);
s32 pollCardEventsDiscard(void);
void closeFileDescriptorWrapper(s32);
void markCardBusy(s32);
s32 createCardFile(s32, char *, s32);
s32 writeCardSector(s32, char *, s32, s32, s32);
void btlMemcpyForward(s32, void *, s32);
static void func_801EAE74(void);
static void func_801EAE4C(void);
s32 func_801F6800(u16, s32, s32);
void func_801F7C98(s32, char *);
void func_801F7954(void);
void func_801F7928(void);
void func_801F7A08(void);
static void func_801E73C0(void);
static void func_801E73E8(void);
static void func_801E7408(void);
void func_801F728C(s32, s32);
static void func_801E283C(void);
static void func_801E2860(void);
s32 func_801E2958(s32, s32);
s32 func_801E2AB0(s32, s32);
void func_801E2DDC(SaveMenuCtx *);
s32 func_801E2E9C(SaveMenuCtx *);
void func_801E2F6C(void);

static void func_801E2CE8(SaveMenuCtx *a0, s32 a1, s32 a2);
static s32 func_801E2ECC(SaveMenuCtx *a0, s32 a1);
static s32 func_801E3098(SaveMenuCtx *ctx);
static void func_801E7B18(s32 a0);
static s32 func_801E7A84(func_801E7BE8_arg0 *a0, func_801E7BE8_D_801D1000 *a1);
static D_801EBD5C_Entry *func_801E7B40(s32 a0);
static void func_801E7BE8(func_801E7BE8_arg0 *arg0);
s32 readCardCreate(s32, char *, void *, s32, s32);
s32 func_801E760C(func_801E7BE8_arg0 *);
s32 func_801E76C4(func_801E7BE8_arg0 *, func_801E7BE8_D_801D1000 *);
void func_801E7830(func_801E7BE8_D_801D1000 *);
void func_801E7938(func_801E7BE8_arg0 *, s32, s32);
s32 func_801E79CC(func_801E7BE8_arg0 *, func_801E7BE8_D_801D1000 *);
void func_801E7B5C(func_801E7BE8_arg0 *);
s32 func_801EAE98(s32);
s32 func_801EB00C(s32, s32, s32, s32);
s32 func_801EB054(s32, s32);

/**
 * @brief Compute save slot data address.
 *
 * Returns a pointer into the D_801D3000 region at offset a0 * 128.
 *
 * @param a0 Save slot index.
 * @return Pointer to save slot data.
 */
s32 func_801E2800(s32 a0) {
    return (s32)0x801D3000 + (a0 << 7);
}

/**
 * @brief Set D_801EC294 flag to 1.
 */
void func_801E2814(void) {
    D_801EC294 = 1;
}

/**
 * @brief Return save data buffer size constant.
 *
 * @return Always 0x180.
 */
s32 func_801E2824(void) {
    return 0x180;
}

/**
 * @brief Read save system state pointer.
 *
 * @return Value of D_8005620C.
 */
s32 func_801E282C(void) {
    return D_8005620C;
}

/**
 * @brief Clear 0x2000 bytes of sprite buffer at 0x801D1000.
 */
static void func_801E283C(void) {
    u8 *ptr = (u8 *)0x801D1000;
    s32 count = 0x2000;
    do {
        *ptr = 0;
        count--;
        ptr++;
    } while (count > 0);
}

/**
 * @brief Load sprite data for save menu slot 0x12.
 *
 * Clears the sprite buffer, initiates a DMA read of sprite data
 * to 0x801D1000, waits for completion, then copies the data to
 * the game state region using table entry 0x12 from D_80097424.
 */
static void func_801E2860(void) {
    s32 v0;
    s32 idx = 0x12;

    func_801E283C();
    loadOverlayDirect(idx, 0x801D1000);
    do {
        v0 = pollCdReadStatus();
    } while (v0 != 0);
    {
        s32 base = D_80097424;
        s32 entry = *(s32 *)(base + idx * 8);
        btlMemcpyForward(0x801D1000, g_gameState.gfs, entry);
    }
}

/**
 * @brief Encode a two-digit number as character codes into a buffer.
 *
 * Divides a1 by 10 to get tens and ones digits, adds 0x824F offset
 * to each, then stores as 4 bytes (high/low of tens, high/low of ones)
 * into buffer at a0.
 *
 * @param a0 Output buffer (4 bytes written, pointer advanced to 5th byte).
 * @param a1 Value to encode (0-99).
 * @return Pointer past the last written byte.
 */
s32 func_801E28C8(u8 *a0, s32 a1) {
    s32 v1 = a1 / 10;
    a1 = a1 - v1 * 10;
    v1 += 0x824F;
    a1 += 0x824F;
    *a0 = v1 >> 8;
    a0++;
    *a0 = v1;
    a0++;
    *a0 = a1 >> 8;
    a0++;
    *a0 = a1;
    return (s32)(a0 + 1);
}

/**
 * @brief Check if save slot halfword equals 0x8FF.
 *
 * Calls func_801E2800 to get save slot address, loads halfword
 * at offset +2, and returns whether it equals 0x8FF.
 *
 * @param a0 Save slot index.
 * @return 1 if halfword equals 0x8FF, 0 otherwise.
 */
s32 func_801E292C(s32 a0) {
    s32 result = func_801E2800(a0);
    return *(u16 *)(result + 2) == 0x8FF;
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2958); /* 0x158 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2AB0); /* 0x1E0 */

/** @brief Draw inner panel with section id 0x5 and clear flag. */
s32 func_801E2C90(s32 a0) {
    return func_801F08D4(1, 5, a0, 0);
}

/** @brief Draw inner panel with section id 0x5 and set flag. */
s32 func_801E2CBC(s32 a0) {
    return func_801F08D4(1, 5, a0, 1);
}

/**
 * @brief Render save slot label at computed Y position.
 * @param a0 Save data pointer (halfwords at offsets 0x32 and 0x36)
 * @param a1 Width selector (non-zero = 0x98, zero = 0x64)
 * @param a2 Offset selector (non-zero = use offset 0x36, zero = use 0x32)
 */
static void func_801E2CE8(SaveMenuCtx *a0, s32 a1, s32 a2) {
    s32 width = 0x64;
    s32 ypos;

    if (a1 != 0) {
        width = 0x98;
    }
    if (a2 != 0) {
        ypos = a0->chocoboColumnX + 7;
    } else {
        ypos = a0->cardColumnX + 7;
    }
    func_801F0994(0, ypos, width);
}

/**
 * @brief Render save slot label at computed Y position.
 * @param a0 First parameter passed through to func_801F0954
 * @param a1 Second parameter passed through to func_801F0954
 * @param a2 Row index (multiplied by 24 and offset by 0xA3 for Y position)
 */
void func_801E2D38(s32 a0, s32 a1, s32 a2) {
    func_801F0954(1, a0, a1, a2 * 24 + 0xA3);
}

/**
 * @brief Render a save slot indicator icon at a grid position.
 *
 * Divides @p a0 by 3 to get the remainder, computes a y-coordinate
 * as remainder * 53 + 0x3C, and renders via func_801F0994 at
 * position (0, 0x22, y).
 *
 * @param a0 Linear save slot index.
 */
void func_801E2D78(s32 a0) {
    func_801F0994(0, 0x22, (a0 % 3) * 53 + 0x3C);
}

/**
 * @brief Process memory card read result and update save slot status.
 *
 * Reads card data via func_801EAE98 using a0[0x57] as the read mode.
 * If read succeeds, extracts status bits and stores them:
 * - Mode 0 (first read): stores to a0[0x2E], a0[0x2C], clears
 *   D_80085148 if no data, sets a0[0x57] to 0x10
 * - Mode != 0 (second read): stores to a0[0x2F], a0[0x2D], clears
 *   D_80085149 if no data, sets a0[0x57] to 0
 *
 * @param a0 Save context pointer.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2DDC); /* 0xC0 */

/**
 * @brief Check save slot validity flags and store result.
 *
 * Sets a0[0x4B] to 1 if either a0[0x2E] or a0[0x2F] is less than 2,
 * otherwise stores (a0[0x2E] < 2).
 *
 * @param a0 Save context pointer.
 * @return The stored flag value.
 */
/**
 * @brief Check if either save slot flag indicates availability.
 *
 * If byte at a0+0x2F >= 2, result is 1. Otherwise result is
 * whether byte at a0+0x2E < 2. Stores and returns the result.
 *
 * @param a0 Save context pointer.
 * @return 1 if available, 0 otherwise.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2E9C); /* 0x30 */

/**
 * @brief Select and decode a string ID based on save slot status.
 *
 * Reads the active slot index from a0[0x58], checks the status byte
 * at offset 0x2E of that slot. Depending on a1 (mode selector):
 * - a1 != 0: checks slot status and sub-status byte at 0x2C
 *   to determine string ID (0x20 or 0x25)
 * - a1 == 0: checks slot status and flag at a0[0x49]
 *   to determine string ID (0x28, 0x1F, or 0x0A)
 *
 * @param a0 Save context pointer.
 * @param a1 Mode selector (0 or non-zero).
 * @return Decoded string pointer.
 */
static s32 func_801E2ECC(SaveMenuCtx *a0, s32 a1) {
    s32 idx;
    s32 v0;

    idx = a0->selectedCard;
    v0 = a0->cardStatus[idx];

    if (a1 != 0) {
        if ((u32)v0 < 2) {
            if (a0->hasPocketStation[idx] == 1) {
                return func_801E2CBC(0x20);
            }
            return func_801E2C90(0x25);
        }
        return func_801E2C90(0x25);
    } else {
        if ((u32)v0 < 2) {
            s32 id = 0x1F;
            if (a0->isLoading == 1) {
                id = 0x28;
            }
            return func_801E2CBC(id);
        }
        return func_801E2C90(0x0A);
    }
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2F6C); /* 0x12C */

static s32 func_801E3098(SaveMenuCtx *ctx) {
    struct DIRENTRY dirEntry;
    char fileNames[16][32];
    char searchPattern[32];
    char slotName[32];
    char slotName2[32];
    char slotName3[32];
    char slotName4[32];
    char slotName5[32];
    MenuDisplayConfig *cfg;
    SaveData *save;
    s32 result;
    u16 *stepPtr;
    u8 *slotStates;
    s32 hasSharedFile;
    s32 sum0;
    s32 sum1;
    char (*nameTable)[32];
    s32 blocksLeft;
    s32 row;
    u16 state;
    u16 inputRepeat;
    u16 inputNew;
    u16 repeat;
    s32 one;
    s32 slotAvailable;
    // One result variable per case: sharing a single variable changes the register allocation.
    s32 poll1;
    s32 poll2;
    s32 poll3;
    s32 poll4;
    s32 poll5;
    s32 poll6;
    s32 poll7;
    s32 poll8;
    s32 poll9;
    s32 poll10;
    s32 poll11;
    s32 poll12;
    s32 poll13;
    s32 poll14;
    s32 poll15;
    s32 poll16;
    s32 slotStatus;
    s32 blockSize;
    s32 writeSize;
    s32 writeSize2;
    s32 fileFound;
    s32 usedBytes;
    s32 checksum;
    s32 fileCount;
    s32 matchCount;
    s32 usedCount;
    s32 slotB;
    s32 freeCount;
    s32 partial;
    s32 i;
    s32 slotIndex;
    s32 clearSlot;
    s32 slot;
    s32 fillSlot;
    s32 j;
    s32 rowUp;
    s32 rowDown;
    s32 moved;
    s32 colUp;
    s32 colDown;

    result = 1;
    stepPtr = &ctx->state;
    cfg = &g_menuDisplayCfg;
    inputRepeat = cfg->inputRepeat;
    inputNew = cfg->inputNew;
    state = *stepPtr;

dispatch:
    switch (state) {
    case SAVE_MENU_INIT:
        if (ctx->isLoading == 1) {
            ctx->selectedBlock = 0xFF;
            *stepPtr = SAVE_MENU_TITLE_INIT;
            break;
        }
        ctx->selectedBlock = 0xFF;
        *stepPtr = SAVE_MENU_CARD_SELECT_FADE_IN;
        ctx->cardStatus[0] = 3;
        ctx->cardStatus[1] = 3;
        ctx->hasPocketStation[0] = 0;
        ctx->hasPocketStation[1] = 0;
        ctx->selectedCard = 0;
        initCardDataBlock();
        func_801EAE74();
        break;
    case SAVE_MENU_CARD_SELECT_FADE_IN:
        ctx->layout = 1;
        ctx->brightness += 0x199;
        func_801E2DDC(ctx);
        ctx->helpMsg = func_801E2ECC(ctx, ctx->columnCursor);
        if (ctx->brightness >= 0x1000) {
            ctx->brightness = 0x1000;
            *stepPtr = SAVE_MENU_CARD_SELECT_ENTER;
        }
        func_801F0948(ctx->brightness);
        func_801E2CE8(ctx, ctx->selectedCard, ctx->columnCursor);
        break;
    case SAVE_MENU_TITLE_INIT:
        one = 1;
        initCardEventHandlers();
        if (ctx->isLoading == one) {
            func_801F1DB0(0);
        }
        ctx->pollPort = 0;
        ctx->layout = 0;
        ctx->titleCursor = one;
        initCardDataBlock();
        func_801EAE74();
        func_801E2D38(0, ctx->titleMenuX, ctx->titleCursor);
        *stepPtr = SAVE_MENU_UNK_03;
        break;
    case SAVE_MENU_UNK_03:
        func_801E2DDC(ctx);
        func_801E2E9C(ctx);
        if (pollCdReadStatus() == 0) {
            *stepPtr = SAVE_MENU_UNK_04;
        }
        func_801E2D38(0, ctx->titleMenuX, ctx->titleCursor);
        break;
    case SAVE_MENU_UNK_04:
        func_801E2DDC(ctx);
        func_801E2E9C(ctx);
        ctx->brightness += 0x199;
        if (ctx->brightness >= 0x1000) {
            ctx->brightness = 0x1000;
            state = SAVE_MENU_UNK_05;
            goto dispatch;
        }
        func_801E2D38(ctx->brightness, ctx->titleMenuX, ctx->titleCursor);
        break;
    case SAVE_MENU_UNK_05:
        func_801E2DDC(ctx);
        func_801E2E9C(ctx);
        *stepPtr = SAVE_MENU_TITLE_INPUT;
    case SAVE_MENU_TITLE_INPUT:
        func_801E2DDC(ctx);
        slotAvailable = func_801E2E9C(ctx);
        ctx->helpMsg = func_801E2ECC(ctx, ctx->columnCursor);
        if (inputRepeat & (PADLup | PADLdown)) {
            ctx->unk4C = 0xFF;
            ctx->titleCursor = func_801F6768(inputRepeat, 2, ctx->titleCursor);
        }
        func_801E2D38(ctx->brightness, ctx->titleMenuX, ctx->titleCursor);
        if (inputNew & PADRdown) {
            if (ctx->titleCursor != 0) {
                if (slotAvailable != 0) {
                    sendSpuCommand(2);
                    func_801EAE4C();
                    initCardEventHandlers();
                    *stepPtr = SAVE_MENU_CARD_SELECT_ENTER;
                } else {
                    sendSpuCommand(5);
                }
            } else {
                *stepPtr = SAVE_MENU_UNK_07;
            }
        }
        break;
    case SAVE_MENU_UNK_07:
        playSoundEffect(0xC);
        playSoundEffect(0xD);
        playSoundEffect(0xE);
        playSoundEffect(0xF);
        func_801E73C0();
        func_801E73E8();
        func_801E2860();
        func_801E7408();
        func_801E2F6C();
        setModeBit(1);
        func_801F7954();
        func_801F798C();
        func_801F7928();
        func_801E2D38(ctx->brightness, ctx->titleMenuX, ctx->titleCursor);
        D_80085148[0] = 0;
        D_80085148[1] = 0;
        *stepPtr = SAVE_MENU_EXIT_START;
        break;
    case SAVE_MENU_CARD_SELECT_ENTER:
        ctx->pollPort = 0;
        ctx->layout = 1;
        ctx->showProgress = 0;
        ctx->savedPopupTimer = 0;
        initCardDataBlock();
        func_801EAE74();
        *stepPtr = SAVE_MENU_CARD_SELECT_INPUT;
        func_801E2CE8(ctx, ctx->selectedCard, ctx->columnCursor);
        break;
    case SAVE_MENU_CARD_CHECK_FAILED:
        sendSpuCommand(5);
        ctx->layout = 3;
        ctx->showProgress = 0;
        ctx->timer = 60;
        ctx->helpMsg = 0;
        initCardDataBlock();
        func_801EAE74();
        *stepPtr = SAVE_MENU_UNK_0A;
    case SAVE_MENU_UNK_0A:
        func_801E2DDC(ctx);
        ctx->timer--;
        if (inputNew & (PADRup | PADRdown)) {
            func_801F7BEC(inputNew);
            ctx->timer = 0;
        }
        if (ctx->timer <= 0) {
            *stepPtr = SAVE_MENU_UNK_0B;
        }
        break;
    case SAVE_MENU_UNK_0B:
        ctx->layout = 1;
        ctx->cardStatus[ctx->selectedCard] = 3;
        ctx->hasPocketStation[ctx->selectedCard] = 0;
        D_80085148[ctx->selectedCard] = 0;
        ctx->selectedBlock = 0xFF;
        func_801E2DDC(ctx);
        state = SAVE_MENU_CARD_SELECT_ENTER;
        goto dispatch;
    case SAVE_MENU_CARD_SELECT_INPUT:
        func_801E2DDC(ctx);
        // The mask is redundant on a u16, but the original has this andi.
        repeat = inputRepeat & 0xFFFF;
        ctx->selectedCard = func_801F6768(repeat, 2, ctx->selectedCard);
        if (ctx->showChocoboColumn != 0) {
            ctx->columnCursor = func_801F6800(repeat, 2, ctx->columnCursor);
        }
        func_801E2CE8(ctx, ctx->selectedCard, ctx->columnCursor);
        ctx->helpMsg = func_801E2ECC(ctx, ctx->columnCursor);
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            if (ctx->isLoading == 0) {
                if (D_801EC294 != 0) {
                    *stepPtr = SAVE_MENU_CHOCOBO_CHANGED;
                } else {
                    *stepPtr = SAVE_MENU_EXIT_START;
                }
            } else {
                *stepPtr = SAVE_MENU_TITLE_INIT;
            }
        }
        if (inputNew & PADRdown) {
            if (ctx->cardStatus[ctx->selectedCard] < 2) {
                if (ctx->columnCursor != 0) {
                    if (ctx->hasPocketStation[ctx->selectedCard] == 1) {
                        sendSpuCommand(2);
                        func_801EAE4C();
                        initCardEventHandlers();
                        dispatchVsyncCallback();
                        loadSubOverlay(7, 0x801CD000);
                        loadSubOverlay(0x5A, 0x801CE800);
                        loadOverlayWithTimCallback(0xC, 0x801D3000);
                        *stepPtr = SAVE_MENU_UNK_0D;
                    } else {
                        sendSpuCommand(5);
                    }
                } else {
                    func_801EAE4C();
                    initCardEventHandlers();
                    sendSpuCommand(2);
                    *stepPtr = SAVE_MENU_LIST_OPEN;
                }
            } else {
                sendSpuCommand(5);
            }
        }
        break;
    case SAVE_MENU_UNK_0D:
        ctx->brightness -= 0x199;
        if (ctx->brightness <= 0) {
            ctx->brightness = 0;
            *stepPtr = SAVE_MENU_UNK_0E;
        }
        func_801F0948(ctx->brightness);
        func_801E2CE8(ctx, ctx->selectedCard, ctx->columnCursor);
        break;
    case SAVE_MENU_UNK_0E:
        func_801F0C5C(0x1B, ctx);
        *stepPtr = SAVE_MENU_CHOCOBO_WAIT;
        break;
    case SAVE_MENU_CHOCOBO_WAIT:
        if (func_801F0D84() == 6 || func_801F0D84() == 0x10) {
            *stepPtr = SAVE_MENU_UNK_10;
        }
        break;
    case SAVE_MENU_UNK_10:
        initCardDataBlock();
        func_801EAE74();
        *stepPtr = SAVE_MENU_UNK_11;
        break;
    case SAVE_MENU_UNK_11:
        ctx->brightness += 0x199;
        func_801E2DDC(ctx);
        ctx->helpMsg = func_801E2ECC(ctx, ctx->columnCursor);
        if (ctx->brightness >= 0x1000) {
            ctx->brightness = 0x1000;
            if (pollCdReadStatus() == 0) {
                *stepPtr = SAVE_MENU_CARD_SELECT_ENTER;
            }
        }
        func_801F0948(ctx->brightness);
        func_801E2CE8(ctx, ctx->selectedCard, ctx->columnCursor);
        break;
    case SAVE_MENU_LIST_OPEN:
        ctx->layout = 2;
        ctx->scanBlock = 0;
        ctx->readRetries = 0;
        slotStatus = ctx->cardStatus[ctx->selectedCard];
        highestSaveSequence = 0;
        ctx->infoMsg = func_801E2C90(ctx->selectedCard + 0x22);
        if (slotStatus == 1) {
            ctx->helpMsg = func_801E2C90(0x19);
            D_80085148[ctx->selectedCard] = 0;
            ctx->selectedBlock = 0xFF;
        } else {
            ctx->helpMsg = func_801E2C90(0x10);
        }
        *stepPtr = SAVE_MENU_LIST_PREPARE;
        break;
    case SAVE_MENU_LIST_PREPARE:
        *stepPtr = SAVE_MENU_LIST_CHECK_CARD;
        break;
    case SAVE_MENU_LIST_CHECK_CARD:
        result = pollCardReady(ctx->selectedCard << 4);
        switch (result) {
        case 0:
            ctx->showProgress = 1;
            ctx->progress = 0;
            ctx->cardReadyResult = 0;
            break;
        case 2:
            ctx->selectedBlock = 0xFF;
            ctx->cardReadyResult = result;
            break;
        case 3:
        default:
            ctx->selectedBlock = 0xFF;
            state = SAVE_MENU_CARD_CHECK_FAILED;
            goto dispatch;
        }
        *stepPtr = SAVE_MENU_LIST_READ_DIRECTORY;
        break;
    case SAVE_MENU_LIST_READ_DIRECTORY:
        ctx->showProgress = 1;
        ctx->progress = 0;
        ctx->helpMsg = func_801E2C90(0x10);
        ctx->progress = 0x100;
        if (ctx->cardReadyResult == 2) {
            for (i = 0; i < 15; i++) {
                D_801EC2A0[i] = BLOCK_UNUSED;
            }
            ctx->freeBlocks = i;
        } else {
            copyString(searchPattern, D_801EB9E0);
            fileCount = 0;
            dirEntry.name[0] = 0;
            fileNames[0][0] = 0;
            fileFound = openFirstFileRetry(ctx->selectedCard << 4, searchPattern, &dirEntry);
            nameTable = fileNames;
            if (fileFound != 0) {
                copyString(*nameTable, dirEntry.name);
            }
            usedBytes = 0;
            if (fileFound != 0) {
                while (1) {
                    fileCount++;
                    usedBytes += dirEntry.size;
                    dirEntry.name[0] = 0;
                    fileFound = cardNextFile(&dirEntry);
                    nameTable[fileCount][0] = 0;
                    if (fileFound == 0) {
                        break;
                    }
                    copyString(nameTable[fileCount], dirEntry.name);
                }
            }
            ctx->freeBlocks = 15 - usedBytes / 8192;
            matchCount = 0;
            hasSharedFile = 0;
            for (i = 0; i < 15; i++) {
                D_801EC2A0[i] = BLOCK_UNUSED;
                for (j = 0; j < fileCount; j++) {
                    func_801F7C98(i, searchPattern);
                    if (compareBytes(fileNames[j], D_8005624C, btlStrlen(D_8005624C)) != 0) {
                        hasSharedFile = 1;
                    }
                    if (compareBytes(searchPattern, fileNames[j], btlStrlen(searchPattern)) != 0) {
                        D_801EC2A0[i] = BLOCK_FF8_SAVE;
                        matchCount++;
                        break;
                    }
                }
            }
            blocksLeft = (15 - ctx->freeBlocks) - matchCount;
            ctx->unk59 = hasSharedFile;
            if (hasSharedFile == 1) {
                blocksLeft -= 7;
            }
            matchCount = 14;
            while (blocksLeft > 0) {
                if (D_801EC2A0[matchCount] == BLOCK_UNUSED) {
                    D_801EC2A0[matchCount] = BLOCK_OTHER_TITLE;
                    blocksLeft--;
                }
                matchCount--;
            }
            if (hasSharedFile != 0) {
                blocksLeft = 7;
                while (blocksLeft > 0) {
                    if (D_801EC2A0[matchCount] == BLOCK_UNUSED) {
                        D_801EC2A0[matchCount] = BLOCK_CHOCOBO_WORLD;
                        blocksLeft--;
                    }
                    matchCount--;
                }
            }
        }
        initCardDataBlock();
        *stepPtr = SAVE_MENU_LIST_NEXT_BLOCK;
        break;
    case SAVE_MENU_LIST_NEXT_BLOCK:
        if (ctx->scanBlock >= 15) {
            state = SAVE_MENU_UNK_19;
            goto dispatch;
        }
        ctx->progress += 0x40;
        ctx->progressLimit = (ctx->scanBlock + 2) << 8;
        if (ctx->progress > ctx->progressLimit) {
            ctx->progress = ctx->progressLimit;
        }
        slotStates = D_801EC2A0;
        if (slotStates[ctx->scanBlock] == BLOCK_FF8_SAVE) {
            func_801F7C98(ctx->scanBlock, slotName);
            result = readCardFileSync(ctx->selectedCard << 4, slotName, func_801E2800(ctx->scanBlock), 0x80, 0x180);
            ctx->fileHandle = result;
            if (result == -1) {
                slotStates[ctx->scanBlock] = BLOCK_UNUSED;
                ctx->selectedBlock = 0xFF;
                ctx->showProgress = 0;
                state = SAVE_MENU_CARD_CHECK_FAILED;
                goto dispatch;
            }
            ctx->timer = 60;
            *stepPtr = SAVE_MENU_LIST_WAIT_READ;
            break;
        }
        if (slotStates[ctx->scanBlock] == BLOCK_CHOCOBO_WORLD && ctx->unk59 != 0) {
            result = readCardFileSync(ctx->selectedCard << 4, D_8005624C, func_801E2800(ctx->scanBlock), 0x80, 0);
            ctx->fileHandle = result;
            if (result == -1) {
                slotStates[ctx->scanBlock] = BLOCK_UNUSED;
                ctx->selectedBlock = 0xFF;
                ctx->showProgress = 0;
                state = SAVE_MENU_CARD_CHECK_FAILED;
                goto dispatch;
            }
            ctx->timer = 60;
            *stepPtr = SAVE_MENU_LIST_WAIT_READ;
            break;
        }
        initCardDataBlock();
        poll1 = pollCardStatus(ctx->selectedCard << 4);
        if (poll1 != -1) {
            if (poll1 != 0) {
                if (poll1 != 1 || ctx->cardReadyResult != 2) {
                    state = SAVE_MENU_CARD_CHECK_FAILED;
                    goto dispatch;
                }
            }
        }
        *stepPtr = SAVE_MENU_LIST_WAIT_CARD;
        break;
    case SAVE_MENU_LIST_WAIT_CARD:
        poll2 = pollCardStatus(ctx->selectedCard << 4);
        if (poll2 != -1) {
            if (poll2 != 0) {
                if (poll2 != 1 || ctx->cardReadyResult != 2) {
                    state = SAVE_MENU_CARD_CHECK_FAILED;
                    goto dispatch;
                }
            }
        }
        ctx->progress += 0x40;
        if (ctx->progress > ctx->progressLimit) {
            ctx->progress = ctx->progressLimit;
            ctx->scanBlock++;
            *stepPtr = SAVE_MENU_LIST_NEXT_BLOCK;
        }
        break;
    case SAVE_MENU_LIST_WAIT_READ:
        poll3 = pollCardEventsDiscard();
        ctx->progress += 0x40;
        if (ctx->progress > ctx->progressLimit) {
            ctx->progress = ctx->progressLimit;
        }
        switch (poll3) {
        case 0:
            closeFileDescriptorWrapper(ctx->fileHandle);
            if (D_801EC2A0[ctx->scanBlock] == BLOCK_CHOCOBO_WORLD && ctx->unk59 != 0) {
                ctx->unk59 = 0;
                if (compareBytes(((SaveBlockHeader *)func_801E2800(ctx->scanBlock))->unk52, D_801EB9E4, 4) == 0) {
                    for (slotIndex = 0; slotIndex < 15; slotIndex++) {
                        if (D_801EC2A0[slotIndex] == BLOCK_CHOCOBO_WORLD) {
                            D_801EC2A0[slotIndex] = BLOCK_CHOCOBO_WORLD_FAILED;
                        }
                    }
                }
            } else {
                SaveBlockHeader *header = (SaveBlockHeader *)func_801E2800(ctx->scanBlock);

                if (highestSaveSequence < header->saveSequence) {
                    highestSaveSequence = header->saveSequence;
                    D_80085148[ctx->selectedCard] = ctx->scanBlock;
                }
                if (func_801E292C(ctx->scanBlock) == 0) {
                    D_801EC2A0[ctx->scanBlock] = BLOCK_DAMAGED;
                }
            }
            ctx->readRetries = 0;
            ctx->scanBlock++;
            *stepPtr = SAVE_MENU_LIST_NEXT_BLOCK;
            break;
        case 1:
        case 2:
        case 3:
            closeFileDescriptorWrapper(ctx->fileHandle);
            if (ctx->readRetries < 2) {
                ctx->readRetries++;
                state = SAVE_MENU_LIST_NEXT_BLOCK;
                goto dispatch;
            }
            ctx->showProgress = 0;
            state = SAVE_MENU_CARD_CHECK_FAILED;
            goto dispatch;
        default:
            ctx->timer--;
            if (ctx->timer != 0) {
                break;
            }
            closeFileDescriptorWrapper(ctx->fileHandle);
            ctx->showProgress = 0;
            state = SAVE_MENU_CARD_CHECK_FAILED;
            goto dispatch;
        }
        break;
    case SAVE_MENU_UNK_19:
        initCardEventHandlers();
        initCardDataBlock();
        initCardEventHandlers();
        result = pollCardReady(ctx->selectedCard << 4);
        switch (result) {
        case 0:
            ctx->progress = 0x1000;
            *stepPtr = SAVE_MENU_LIST_LOADED;
            break;
        case 2:
            ctx->progress = 0x1000;
            ctx->cardReadyResult = result;
            for (clearSlot = 0; clearSlot < 15; clearSlot++) {
                D_801EC2A0[clearSlot] = BLOCK_UNUSED;
            }
            ctx->freeBlocks = 15;
            *stepPtr = SAVE_MENU_LIST_LOADED;
            break;
        case 3:
            sendSpuCommand(5);
            ctx->selectedBlock = 0xFF;
            state = SAVE_MENU_CARD_CHECK_FAILED;
            goto dispatch;
        default:
            sendSpuCommand(5);
            break;
        }
        break;
    case SAVE_MENU_LIST_LOADED:
        ctx->showProgress = 0;
        if (ctx->isLoading == 1) {
            freeCount = 0;
            for (slot = 0; slot < 15; slot++) {
                if (D_801EC2A0[slot] == BLOCK_FF8_SAVE) {
                    freeCount++;
                }
            }
            if (freeCount == 0) {
                state = SAVE_MENU_NO_SAVE_DATA;
                goto dispatch;
            }
        }
        if (ctx->isLoading == 0) {
            usedCount = 0;
            for (slotB = 0; slotB < 15; slotB++) {
                if (D_801EC2A0[slotB] == BLOCK_OTHER_TITLE) {
                    usedCount++;
                }
            }
            if (usedCount == 15) {
                state = SAVE_MENU_NO_EMPTY_BLOCKS;
                goto dispatch;
            }
        }
        playSoundEffect(0x10);
        *stepPtr = SAVE_MENU_LIST_ENTER;
        break;
    case SAVE_MENU_LIST_ENTER:
        initCardEventHandlers();
        initCardDataBlock();
        if (ctx->selectedBlock == 0xFF) {
            ctx->selectedBlock = D_80085148[ctx->selectedCard];
        }
        *stepPtr = SAVE_MENU_UNK_1C;
    case SAVE_MENU_UNK_1C:
        if (pollCdReadStatus() != 0) {
            break;
        }
        state = SAVE_MENU_LIST_SHOW;
        goto dispatch;
    case SAVE_MENU_LIST_SHOW:
        ctx->layout = 5;
        if (ctx->isLoading == 0) {
            ctx->helpMsg = func_801E2C90(4);
        } else {
            ctx->helpMsg = func_801E2C90(0x1D);
        }
        initCardEventHandlers();
        initCardDataBlock();
        ctx->noCardPolls = 0;
        func_801E2D78(ctx->selectedBlock);
        *stepPtr = SAVE_MENU_LIST_INPUT;
        break;
    case SAVE_MENU_NO_CARD:
        ctx->savedPopupTimer = 0;
        ctx->helpMsg = func_801E2C90(0xA);
        for (fillSlot = 0; fillSlot < 15; fillSlot++) {
            D_801EC2A0[fillSlot] = BLOCK_NO_CARD;
        }
        D_80085148[ctx->selectedCard] = 0;
        markCardBusy(ctx->selectedCard << 4);
        *stepPtr = SAVE_MENU_NO_CARD_WAIT;
    case SAVE_MENU_NO_CARD_WAIT:
        if ((u32)pollCardStatus(ctx->selectedCard << 4) < 2) {
            *stepPtr = SAVE_MENU_LIST_OPEN;
        }
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            ctx->savedPopupTimer = 0;
            ctx->selectedBlock = 0xFF;
            initCardDataBlock();
            func_801EAE74();
            state = SAVE_MENU_UNK_0B;
            goto dispatch;
        }
        break;
    case SAVE_MENU_LIST_INPUT:
        poll4 = pollCardStatus(ctx->selectedCard << 4);
        if (poll4 != -1) {
            if (poll4 == 3) {
                ctx->noCardPolls++;
                if (ctx->noCardPolls >= 3) {
                    state = SAVE_MENU_NO_CARD;
                    goto dispatch;
                }
            } else {
                ctx->noCardPolls = 0;
            }
        }
        // The (u8) casts on the row and column are needed for the match.
        row = (u8)(ctx->selectedBlock / 3);
        moved = func_801F6768(inputRepeat, 3, (u8)(ctx->selectedBlock % 3));
        if (inputRepeat & (PADLup | PADLdown)) {
            ctx->savedPopupTimer = 0;
        }
        if (inputRepeat & PADLleft) {
            ctx->savedPopupTimer = 0;
            *stepPtr = SAVE_MENU_PAGE_LEFT_START;
        }
        if (inputRepeat & PADLright) {
            ctx->savedPopupTimer = 0;
            *stepPtr = SAVE_MENU_PAGE_RIGHT_START;
        }
        ctx->selectedBlock = moved + row * 3;
        func_801E2D78(ctx->selectedBlock);
        if (inputNew & PADRdown) {
            ctx->savedPopupTimer = 0;
            if (ctx->isLoading == 1) {
                if (D_801EC2A0[ctx->selectedBlock] == BLOCK_FF8_SAVE) {
                    sendSpuCommand(2);
                    initCardEventHandlers();
                    *stepPtr = SAVE_MENU_LOAD_OPEN;
                } else {
                    sendSpuCommand(5);
                }
            } else {
                switch (D_801EC2A0[ctx->selectedBlock]) {
                case BLOCK_FF8_SAVE:
                case BLOCK_DAMAGED:
                    sendSpuCommand(2);
                    initCardEventHandlers();
                    *stepPtr = SAVE_MENU_OVERWRITE_OPEN;
                    break;
                case BLOCK_UNUSED:
                    sendSpuCommand(2);
                    initCardEventHandlers();
                    *stepPtr = SAVE_MENU_SAVE_CHECK_FORMAT;
                    break;
                default:
                    sendSpuCommand(5);
                    break;
                }
            }
        }
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            ctx->savedPopupTimer = 0;
            ctx->selectedBlock = 0xFF;
            state = SAVE_MENU_CARD_SELECT_ENTER;
            goto dispatch;
        }
        break;
    case SAVE_MENU_PAGE_LEFT_START:
        // The (u8) casts on the row and column are needed for the match.
        colUp = (u8)(ctx->selectedBlock % 3);
        rowUp = (u8)(ctx->selectedBlock / 3);
        rowUp--;
        if (rowUp < 0) {
            rowUp = 4;
        }
        ctx->selectedBlock = colUp + rowUp * 3;
        sendSpuCommand(1);
        ctx->pageSlide = -0xF80;
        func_801E2D78(ctx->selectedBlock);
        poll5 = pollCardStatus(ctx->selectedCard << 4);
        if (poll5 != -1) {
            if (poll5 == 3) {
                ctx->noCardPolls++;
                if (ctx->noCardPolls >= 3) {
                    ctx->pageSlide = 0;
                    state = SAVE_MENU_NO_CARD;
                    goto dispatch;
                }
            } else {
                ctx->noCardPolls = 0;
            }
        }
        *stepPtr = SAVE_MENU_PAGE_LEFT_SLIDE;
        break;
    case SAVE_MENU_PAGE_LEFT_SLIDE:
        poll6 = pollCardStatus(ctx->selectedCard << 4);
        if (poll6 != -1) {
            if (poll6 == 3) {
                ctx->noCardPolls++;
                if (ctx->noCardPolls >= 3) {
                    ctx->pageSlide = 0;
                    state = SAVE_MENU_NO_CARD;
                    goto dispatch;
                }
            } else {
                ctx->noCardPolls = 0;
            }
        }
        func_801E2D78(ctx->selectedBlock);
        ctx->pageSlide += 0x80;
        if (ctx->pageSlide >= 0) {
            ctx->pageSlide = 0;
            *stepPtr = SAVE_MENU_LIST_SHOW;
        }
        if (inputNew & PADLright) {
            *stepPtr = SAVE_MENU_PAGE_RIGHT_START;
        }
        if (inputNew & PADLleft) {
            *stepPtr = SAVE_MENU_PAGE_LEFT_START;
        }
        break;
    case SAVE_MENU_PAGE_RIGHT_START:
        // The (u8) casts on the row and column are needed for the match.
        colDown = (u8)(ctx->selectedBlock % 3);
        rowDown = (u8)(ctx->selectedBlock / 3);
        rowDown++;
        if (rowDown >= 5) {
            rowDown = 0;
        }
        ctx->selectedBlock = colDown + rowDown * 3;
        sendSpuCommand(1);
        ctx->pageSlide = 0xF80;
        func_801E2D78(ctx->selectedBlock);
        poll7 = pollCardStatus(ctx->selectedCard << 4);
        if (poll7 != -1) {
            if (poll7 == 3) {
                ctx->noCardPolls++;
                if (ctx->noCardPolls >= 3) {
                    ctx->pageSlide = 0;
                    state = SAVE_MENU_NO_CARD;
                    goto dispatch;
                }
            } else {
                ctx->noCardPolls = 0;
            }
        }
        *stepPtr = SAVE_MENU_PAGE_RIGHT_SLIDE;
        break;
    case SAVE_MENU_PAGE_RIGHT_SLIDE:
        poll8 = pollCardStatus(ctx->selectedCard << 4);
        if (poll8 != -1) {
            if (poll8 == 3) {
                ctx->noCardPolls++;
                if (ctx->noCardPolls >= 3) {
                    ctx->pageSlide = 0;
                    state = SAVE_MENU_NO_CARD;
                    goto dispatch;
                }
            } else {
                ctx->noCardPolls = 0;
            }
        }
        func_801E2D78(ctx->selectedBlock);
        ctx->pageSlide -= 0x80;
        if (ctx->pageSlide <= 0) {
            ctx->pageSlide = 0;
            *stepPtr = SAVE_MENU_LIST_SHOW;
        }
        if (inputNew & PADLright) {
            *stepPtr = SAVE_MENU_PAGE_RIGHT_START;
        }
        if (inputNew & PADLleft) {
            *stepPtr = SAVE_MENU_PAGE_LEFT_START;
        }
        break;
    case SAVE_MENU_NO_EMPTY_BLOCKS:
        sendSpuCommand(5);
        initCardDataBlock();
        ctx->layout = 0xA;
        ctx->timer = 300;
        *stepPtr = SAVE_MENU_MESSAGE_WAIT;
        break;
    case SAVE_MENU_NO_SAVE_DATA:
        sendSpuCommand(5);
        initCardDataBlock();
        ctx->layout = 0xB;
        ctx->timer = 300;
        *stepPtr = SAVE_MENU_MESSAGE_WAIT;
        break;
    case SAVE_MENU_MESSAGE_WAIT:
        ctx->noCardPolls = 0;
        ctx->timer--;
        poll9 = pollCardStatus(ctx->selectedCard << 4);
        if (poll9 != -1) {
            if (poll9 == 3) {
                ctx->noCardPolls++;
                if (ctx->noCardPolls >= 3) {
                    state = SAVE_MENU_UNK_0B;
                    goto dispatch;
                }
            } else {
                ctx->noCardPolls = 0;
            }
        }
        if (inputNew & (PADRup | PADRdown)) {
            func_801F7BEC(inputNew);
            ctx->timer = 0;
        }
        if (ctx->timer <= 0) {
            *stepPtr = SAVE_MENU_CARD_SELECT_ENTER;
        }
        break;
    case SAVE_MENU_OVERWRITE_OPEN:
        ctx->layout = 4;
        ctx->yesNoCursor = 1;
        func_801F728C(func_801E2C90(5), 0x54);
        initCardDataBlock();
        ctx->noCardPolls = 0;
        *stepPtr = SAVE_MENU_OVERWRITE_INPUT;
    case SAVE_MENU_OVERWRITE_INPUT:
        poll10 = pollCardStatus(ctx->selectedCard << 4);
        if (poll10 != -1) {
            if (poll10 == 3) {
                ctx->noCardPolls++;
                if (ctx->noCardPolls >= 4) {
                    ctx->layout = 5;
                    state = SAVE_MENU_NO_CARD;
                    goto dispatch;
                }
            } else {
                ctx->noCardPolls = 0;
            }
        }
        ctx->yesNoCursor = func_801F6768(inputRepeat, 2, ctx->yesNoCursor);
        func_801F6F88(ctx->yesNoCursor);
        if (inputNew & PADRdown) {
            sendSpuCommand(2);
            if (ctx->yesNoCursor == 0) {
                *stepPtr = SAVE_MENU_SAVE_CHECK_FORMAT;
            } else {
                *stepPtr = SAVE_MENU_LIST_SHOW;
            }
        }
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            *stepPtr = SAVE_MENU_LIST_SHOW;
        }
        break;
    case SAVE_MENU_SAVE_CHECK_FORMAT:
        if (ctx->cardReadyResult == 2) {
            *stepPtr = SAVE_MENU_FORMAT_PROMPT_OPEN;
            break;
        }
    case SAVE_MENU_UNK_36:
        initCardDataBlock();
        if (D_801EC2A0[ctx->selectedBlock] == BLOCK_UNUSED) {
            blockSize = func_801E2AB0(ctx->selectedBlock, 0x801D1000);
            if (ctx->freeBlocks < (blockSize + 0x1FFF) / 8192) {
                state = SAVE_MENU_SAVE_NO_ROOM;
                goto dispatch;
            }
        }
        ctx->layout = 7;
        ctx->helpMsg = func_801E2C90(9);
        *stepPtr = SAVE_MENU_UNK_37;
        break;
    case SAVE_MENU_UNK_37:
        initCardDataBlock();
        *stepPtr = SAVE_MENU_SAVE_WRITE;
        break;
    case SAVE_MENU_SAVE_WRITE:
        ctx->progress = 0;
        func_801F7C98(ctx->selectedBlock, slotName);
        func_801F7A08();
        writeSize = func_801E2AB0(ctx->selectedBlock, 0x801D1000);
        ctx->writeSize = writeSize;
        if (D_801EC2A0[ctx->selectedBlock] == BLOCK_UNUSED) {
            if (createCardFile(ctx->selectedCard << 4, slotName, writeSize) == -1) {
                state = SAVE_MENU_SAVE_FAILED;
                goto dispatch;
            }
        }
        poll11 = writeCardSector(ctx->selectedCard << 4, slotName, 0x801D1000, writeSize, 0);
        ctx->timer = 300;
        ctx->fileHandle = poll11;
        if (poll11 != writeSize) {
            state = SAVE_MENU_SAVE_FAILED;
            goto dispatch;
        }
        *stepPtr = SAVE_MENU_SAVE_WRITE_DONE;
        break;
    case SAVE_MENU_SAVE_WRITE_DONE:
        *stepPtr = SAVE_MENU_UNK_3E;
        break;
    case SAVE_MENU_UNK_3A:
        func_801F7C98(ctx->scanBlock, slotName2);
        result = readCardFileSync(ctx->selectedCard << 4, slotName2, 0x801D1000, 0x80, 0x180);
        ctx->fileHandle = result;
        if (result == -1) {
            state = SAVE_MENU_SAVE_FAILED;
            goto dispatch;
        }
        ctx->progress += 0x2A;
        *stepPtr = SAVE_MENU_UNK_3B;
    case SAVE_MENU_UNK_3B:
        poll12 = pollCardEventsDiscard();
        ctx->progress += 0x2A;
        if (ctx->progress >= 0x1000) {
            ctx->progress = 0xFD6;
        }
        switch (poll12) {
        case 0:
            closeFileDescriptorWrapper(ctx->fileHandle);
            // The original reads these words of the save buffer through hard-coded addresses.
            D_801EC290 = *(u16 *)0x801D1000;
            *stepPtr = SAVE_MENU_UNK_3C;
            break;
        case 1:
        case 2:
        case 3:
            sendSpuCommand(5);
            closeFileDescriptorWrapper(ctx->fileHandle);
            state = SAVE_MENU_SAVE_FAILED;
            goto dispatch;
        default:
            ctx->timer--;
            if (ctx->timer != 0) {
                break;
            }
            sendSpuCommand(5);
            closeFileDescriptorWrapper(ctx->fileHandle);
            state = SAVE_MENU_SAVE_FAILED;
            goto dispatch;
        }
        break;
    case SAVE_MENU_UNK_3C:
        ctx->progress += 0x2A;
        if (ctx->progress >= 0x1000) {
            ctx->progress = 0xFD6;
        }
        func_801F7C98(ctx->scanBlock, slotName3);
        result = readCardFileSync(ctx->selectedCard << 4, slotName3, 0x801D2380, 0x80, 0x1A7);
        ctx->fileHandle = result;
        if (result == -1) {
            state = SAVE_MENU_SAVE_FAILED;
            goto dispatch;
        }
        ctx->progress += 0x2A;
        *stepPtr = SAVE_MENU_UNK_3D;
    case SAVE_MENU_UNK_3D:
        poll13 = pollCardEventsDiscard();
        ctx->progress += 0x2A;
        if (ctx->progress >= 0x1000) {
            ctx->progress = 0xFD6;
        }
        switch (poll13) {
        case 0:
            closeFileDescriptorWrapper(ctx->fileHandle);
            if (D_801EC290 != (D_801EC292 = *(u16 *)0x801D23A0)) {
                state = SAVE_MENU_SAVE_FAILED;
                goto dispatch;
            }
            *stepPtr = SAVE_MENU_UNK_3E;
            break;
        case 1:
        case 2:
        case 3:
            sendSpuCommand(5);
            closeFileDescriptorWrapper(ctx->fileHandle);
            state = SAVE_MENU_SAVE_FAILED;
            goto dispatch;
        default:
            ctx->timer--;
            if (ctx->timer != 0) {
                break;
            }
            sendSpuCommand(5);
            closeFileDescriptorWrapper(ctx->fileHandle);
            state = SAVE_MENU_SAVE_FAILED;
            goto dispatch;
        }
        break;
    case SAVE_MENU_UNK_3E:
        ctx->progress += 0x2A;
        if (ctx->progress >= 0x1000) {
            ctx->progress = 0xFD6;
        }
        *stepPtr = SAVE_MENU_SAVE_WRITE_2;
    case SAVE_MENU_SAVE_WRITE_2:
        ctx->progress += 0x2A;
        if (ctx->progress >= 0x1000) {
            ctx->progress = 0xFD6;
        }
        func_801F7C98(ctx->selectedBlock, slotName4);
        writeSize2 = func_801E2958(ctx->selectedBlock, 0x801D1000);
        poll14 = writeCardSector(ctx->selectedCard << 4, slotName4, 0x801D1000, writeSize2, 0);
        ctx->writeSize = writeSize2;
        ctx->timer = 300;
        ctx->fileHandle = poll14;
        if (poll14 != writeSize2) {
            state = SAVE_MENU_SAVE_FAILED;
            goto dispatch;
        }
        playSoundEffect(0x13);
        D_801EC294 = 0;
        *stepPtr = SAVE_MENU_UNK_40;
        break;
    case SAVE_MENU_UNK_40:
        ctx->progress += 0x2A;
        if (ctx->progress >= 0x1000) {
            ctx->progress = 0xFD6;
        }
        *stepPtr = SAVE_MENU_SAVE_DONE;
        break;
    case SAVE_MENU_SAVE_DONE:
        ctx->showProgress = 0;
        ctx->savedPopupTimer = 0x40;
        *stepPtr = SAVE_MENU_LIST_ENTER;
        break;
    case SAVE_MENU_SAVE_FAILED:
        sendSpuCommand(5);
        ctx->showProgress = 0;
        ctx->layout = 9;
        *stepPtr = SAVE_MENU_UNK_44;
        break;
    case SAVE_MENU_SAVE_NO_ROOM:
        sendSpuCommand(5);
        ctx->showProgress = 0;
        ctx->layout = 0xA;
        *stepPtr = SAVE_MENU_UNK_44;
        break;
    case SAVE_MENU_LOAD_OPEN:
        ctx->layout = 6;
        ctx->helpMsg = func_801E2C90(8);
        ctx->showProgress = 1;
        ctx->progress = 0;
        *stepPtr = SAVE_MENU_UNK_46;
    case SAVE_MENU_UNK_46:
        *stepPtr = SAVE_MENU_UNK_47;
    case SAVE_MENU_UNK_47:
        func_801F7C98(ctx->selectedBlock, slotName5);
        if (D_801EC2A0[ctx->selectedBlock] == BLOCK_UNUSED) {
            *stepPtr = SAVE_MENU_LOAD_READ_FAILED;
        }
        func_801E283C();
        func_801E283C();
        poll15 = readCardFileSync(ctx->selectedCard << 4, slotName5, 0x801D1000, 0x1400, 0x180);
        ctx->timer = 300;
        ctx->fileHandle = poll15;
        if (poll15 == -1) {
            *stepPtr = SAVE_MENU_LOAD_READ_FAILED;
        }
        *stepPtr = SAVE_MENU_LOAD_WAIT;
        break;
    case SAVE_MENU_LOAD_WAIT:
        poll16 = pollCardEventsDiscard();
        ctx->progress += 0x33;
        if (ctx->progress >= 0x1000) {
            ctx->progress = 0xFCD;
        }
        switch (poll16) {
        case 0:
            closeFileDescriptorWrapper(ctx->fileHandle);
            *stepPtr = SAVE_MENU_LOAD_VERIFY;
            break;
        case 1:
        case 2:
        case 3:
            closeFileDescriptorWrapper(ctx->fileHandle);
            *stepPtr = SAVE_MENU_LOAD_READ_FAILED;
        default:
            ctx->timer--;
            if (ctx->timer != 0) {
                break;
            }
            closeFileDescriptorWrapper(ctx->fileHandle);
            *stepPtr = SAVE_MENU_LOAD_READ_FAILED;
            break;
        }
        break;
    case SAVE_MENU_LOAD_VERIFY:
        save = (SaveData *)0x801D1000;
        checksum = func_800377B4(0x1350, (u8 *)0x801D1050);
        // The first compare uses the raw return value, the second the narrowed one.
        if (checksum == save->checksum && (u16)checksum == save->checksumCopy) {
            sum0 = save->playTimeSeconds;
            sum1 = save->fieldCDC + sum0;
            partial = save->unkA + sum1;
            D_80083792[0] = partial + save->fieldCE0 + save->fieldCE2;
            ctx->progress = 0x1000;
            btlMemcpyForward(0x801D1000, &g_gameState, 0x13A4);
            recalcPartyStats();
            func_801F7954();
            func_801F798C();
            func_801F7928();
            playSoundEffect(0x13);
            D_801EC294 = 0;
            *stepPtr = SAVE_MENU_EXIT_START;
            break;
        }
        sendSpuCommand(5);
        *stepPtr = SAVE_MENU_LOAD_DAMAGED;
        break;
    case SAVE_MENU_LOAD_READ_FAILED:
        sendSpuCommand(5);
        ctx->showProgress = 0;
        ctx->layout = 0xC;
        *stepPtr = SAVE_MENU_UNK_4C;
        break;
    case SAVE_MENU_LOAD_DAMAGED:
        sendSpuCommand(5);
        ctx->showProgress = 0;
        ctx->layout = 0xD;
        *stepPtr = SAVE_MENU_UNK_4C;
        break;
    case SAVE_MENU_FORMAT_PROMPT_OPEN:
        func_801F728C(func_801E2C90(ctx->selectedCard + 0xB), 0x42);
        ctx->layout = 8;
        ctx->yesNoCursor = 1;
        initCardDataBlock();
        *stepPtr = SAVE_MENU_UNK_26;
        break;
    case SAVE_MENU_UNK_26:
        *stepPtr = SAVE_MENU_UNK_27;
    case SAVE_MENU_UNK_27:
        ctx->helpMsg = 0;
        *stepPtr = SAVE_MENU_UNK_28;
    case SAVE_MENU_UNK_28:
        result = pollCardStatus(ctx->selectedCard << 4);
        if (result == 3) {
            initCardEventHandlers();
            state = SAVE_MENU_UNK_0B;
            goto dispatch;
        }
        ctx->yesNoCursor = func_801F6768(inputRepeat, 2, ctx->yesNoCursor);
        func_801F6F88(ctx->yesNoCursor);
        if (inputNew & PADRdown) {
            sendSpuCommand(2);
            if (ctx->yesNoCursor == 0) {
                initCardEventHandlers();
                *stepPtr = SAVE_MENU_UNK_29;
            } else {
                *stepPtr = SAVE_MENU_LIST_ENTER;
            }
        }
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            *stepPtr = SAVE_MENU_LIST_ENTER;
        }
        break;
    case SAVE_MENU_UNK_29:
        ctx->helpMsg = 0;
        ctx->layout = 0xF;
        *stepPtr = SAVE_MENU_UNK_2A;
        break;
    case SAVE_MENU_UNK_2A:
        *stepPtr = SAVE_MENU_UNK_2B;
        break;
    case SAVE_MENU_UNK_2B:
        result = func_800379AC(ctx->selectedCard << 4);
        if (result == 1) {
            ctx->cardReadyResult = 0;
            *stepPtr = SAVE_MENU_FORMAT_OK;
            break;
        }
        *stepPtr = SAVE_MENU_FORMAT_FAILED;
        break;
    case SAVE_MENU_FORMAT_OK:
        ctx->layout = 0x11;
        ctx->timer = 60;
        playSoundEffect(0x10);
        initCardDataBlock();
        *stepPtr = SAVE_MENU_UNK_2D;
        break;
    case SAVE_MENU_UNK_2D:
        result = pollCardStatus(ctx->selectedCard << 4);
        ctx->timer--;
        if (result != -1) {
            if (result == 3) {
                ctx->noCardPolls++;
                if (ctx->noCardPolls >= 4) {
                    state = SAVE_MENU_UNK_0B;
                    goto dispatch;
                }
            } else {
                ctx->noCardPolls = 0;
            }
        }
        if (inputNew & PADRdown) {
            sendSpuCommand(2);
            ctx->cardStatus[ctx->selectedCard] = 1;
            *stepPtr = SAVE_MENU_UNK_36;
        }
        if (ctx->timer < 0) {
            ctx->cardStatus[ctx->selectedCard] = 1;
            *stepPtr = SAVE_MENU_UNK_36;
        }
        break;
    case SAVE_MENU_FORMAT_FAILED:
        ctx->layout = 0x10;
        sendSpuCommand(5);
        *stepPtr = SAVE_MENU_UNK_2F;
        break;
    case SAVE_MENU_UNK_2F:
    case SAVE_MENU_UNK_44:
    case SAVE_MENU_UNK_4C:
        if (inputNew & (PADRup | PADRdown)) {
            func_801F7BEC(inputNew);
            state = SAVE_MENU_UNK_0B;
            goto dispatch;
        }
        break;
    case SAVE_MENU_CHOCOBO_CHANGED:
        sendSpuCommand(5);
        ctx->showProgress = 0;
        ctx->layout = 0xE;
        initCardDataBlock();
        func_801EAE74();
        *stepPtr = SAVE_MENU_UNK_4E;
        break;
    case SAVE_MENU_UNK_4E:
        func_801E2DDC(ctx);
        ctx->helpMsg = func_801E2ECC(ctx, ctx->columnCursor);
        if (inputNew & (PADRup | PADRdown)) {
            func_801F7BEC(inputNew);
            D_801EC294 = 0;
            *stepPtr = SAVE_MENU_CARD_SELECT_ENTER;
        }
        break;
    case SAVE_MENU_EXIT_START:
        func_801EAE4C();
        initCardEventHandlers();
        loadOverlayWithTimCallback(9, 0x801CD000);
        loadOverlayWithTimCallback(0xA, 0x801D5000);
        *stepPtr = SAVE_MENU_EXIT;
    case SAVE_MENU_EXIT:
        ctx->brightness -= 0x199;
        if (ctx->brightness < 0) {
            ctx->brightness = 0;
            clearCdBusyFlag();
            func_801F18FC(ctx->task);
            func_801F0BB0();
            if (ctx->isLoading == 1) {
                func_801F1D34(NULL);
            }
        }
        if (ctx->isLoading == 1) {
            if (ctx->titleCursor == 0) {
                func_801E2D38(ctx->brightness, ctx->titleMenuX, ctx->titleCursor);
                break;
            }
            func_801F1DB0(ctx->brightness);
            break;
        }
        func_801F0948(ctx->brightness);
        func_801E2CE8(ctx, ctx->selectedCard, ctx->columnCursor);
        break;
    }
    if (ctx->savedPopupTimer != 0) {
        ctx->savedPopupTimer--;
    }
    if (func_801EFFB8() == 0x18) {
        func_801F1DB0(ctx->brightness);
    }
    return result;
}


INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5464); /* 0xAC */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5510); /* 0xB4 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E55C4); /* 0xC4 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5688); /* 0x114 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E579C); /* 0x124 */

/**
 * @brief Render save menu entry with string ID 0x26 (save slot label).
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E58C0(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x26);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x0F.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E591C(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x0F);
    func_801F4274(a1, a2, v0, 0xC0, 0x87, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x12.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E5978(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x12);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x18.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E59D4(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x18);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x27.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E5A30(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x27);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x13.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E5A8C(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x13);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

/**
 * @brief Conditionally render a save menu entry based on status.
 *
 * Checks the signed halfword at a0[0x3C]. If positive, selects
 * string ID 0x29 (if a0[0x53] == 2) or 0x1C (otherwise), and
 * renders via func_801E2C90 + func_801F4274.
 *
 * @param a0 Save context pointer.
 * @param a1 X position for rendering.
 * @param a2 Y position for rendering.
 * @return Render result, or a2 if skipped.
 */
s32 func_801E5AE8(u8 *a0, s32 a1, s32 a2) {
    s32 s0 = a2;
    if (*(s16 *)(a0 + 0x3C) > 0) {
        s32 id = 0x1C;
        if (a0[0x53] == 2) {
            id = 0x29;
        }
        s0 = func_801F4274(a1, s0, func_801E2C90(id), 0xC0, 0x6B, 0x1000);
    }
    return s0;
}

/**
 * @brief Render save menu entry with string ID 0x17.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E5B70(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x17);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

/**
 * @brief Look up save slot data and render with func_801F3824.
 *
 * Calls func_801E2800 with the 5th argument to get a save slot pointer,
 * loads the word at offset 0xC, then passes it as the 5th stack arg
 * to func_801F3824 along with adjusted copies of the original arguments.
 *
 * @param a0 First render parameter (passed through).
 * @param a1 Second render parameter (passed through).
 * @param a2 Third render parameter (0x100 added).
 * @param a3 Fourth render parameter (0x14 added).
 * @param stack_arg Save slot index for func_801E2800.
 */
/**
 * @brief Look up save slot data and render with func_801F3824.
 *
 * Calls func_801E2800 with the 5th argument to get a save slot pointer,
 * loads the word at offset 0xC, then passes it as the 5th stack arg
 * to func_801F3824 along with adjusted copies of the original arguments.
 *
 * @param a0 First render parameter (passed through).
 * @param a1 Second render parameter (passed through).
 * @param a2 Third render parameter (0x100 added).
 * @param a3 Fourth render parameter (0x14 added).
 * @param stack_arg Save slot index for func_801E2800.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5BCC); /* 0x6C */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5C38); /* 0xCC */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5D04); /* 0xC0 */

/**
 * @brief Look up save slot data and render with func_801F3270.
 *
 * Calls func_801E2800 with the 5th argument to get a save slot pointer,
 * loads the word at offset 0x10, then passes it as the 5th stack arg
 * to func_801F3270 along with adjusted copies of the original arguments.
 *
 * @param a0 First render parameter (passed through).
 * @param a1 Second render parameter (passed through).
 * @param a2 Third render parameter (0x100 added).
 * @param a3 Fourth render parameter (0x8 added).
 * @param stack_arg Save slot index for func_801E2800.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5DC4); /* 0x6C */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5E30); /* 0xB0 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5EE0); /* 0x180 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6060); /* 0xDC */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E613C); /* 0x84 */

/**
 * @brief Build a parameter struct on the stack and call func_8002B898.
 *
 * Constructs a 3-field struct: {u16 a2+0x80, u16 a3+0x1F, u32 0x1600D0}
 * and passes it along with g_menuTint[MENU_TINT_NORMAL] to func_8002B898.
 *
 * @param a0 First parameter (passed through).
 * @param a1 Second parameter (passed through).
 * @param a2 Width value (0x80 added).
 * @param a3 Height value (0x1F added).
 */
void func_801E61C0(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 buf[2];
    *(u16 *)((u8 *)buf + 0) = a2 + 0x80;
    *(u16 *)((u8 *)buf + 2) = a3 + 0x1F;
    *(s32 *)((u8 *)buf + 4) = 0x1600D0;
    func_8002B898(a0, a1, (s32)buf, g_menuTint[MENU_TINT_NORMAL]);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6204); /* 0x338 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E653C); /* 0x218 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6754); /* 0xB0 */

/**
 * @brief Render save menu entry with string ID 0x0D.
 * @param a0 X position for rendering
 * @param a1 Y position for rendering
 */
void func_801E6804(s32 a0, s32 a1) {
    s32 v0 = func_801E2C90(0x0D);
    func_801F4274(a0, a1, v0, 0xC0, 0x64, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x0E.
 * @param a0 X position for rendering
 * @param a1 Y position for rendering
 */
void func_801E6860(s32 a0, s32 a1) {
    s32 v0 = func_801E2C90(0x0E);
    func_801F4274(a0, a1, v0, 0xC0, 0x64, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x16.
 * @param a0 X position for rendering
 * @param a1 Y position for rendering
 */
void func_801E68BC(s32 a0, s32 a1) {
    s32 v0 = func_801E2C90(0x16);
    func_801F4274(a0, a1, v0, 0xC0, 0x64, 0x1000);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6918); /* 0x168 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6A80); /* 0x9C */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6B1C); /* 0xC4 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6BE0); /* 0x42C */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E700C); /* 0x184 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7190); /* 0xD8 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7268); /* 0x158 */

/** @brief Call memzero16 with g_gameState and 0x13A. */
static void func_801E73C0(void) {
    memzero16((s32 *)&g_gameState, 0x13A);
}

/**
 * @brief Wrapper that calls func_801E7428 (save menu handler).
 */
static void func_801E73E8(void) {
    func_801E7428();
}

/**
 * @brief Wrapper that calls recalcPartyStats (save menu input handler).
 */
static void func_801E7408(void) {
    recalcPartyStats();
}

/**
 * @brief Initialize save menu handler state.
 *
 * Gets the save handler context, clears the first 0x6E bytes and bytes
 * 0x6E-0x72, clears bit 24 of the flags word at offset 0x70, sets
 * the word at 0x7C to 1, then registers 0x21 menu items via
 * modifyItemQuantity.
 */
void func_801E7428(void) {
    s32 s0;
    u8 *a1 = (u8 *)getTripleTriadData();
    s32 v0;

    s0 = 0x6D;
    v0 = (s32)a1 + 0x6D;
    do {
        *(u8 *)v0 = 0;
        s0--;
        v0--;
    } while (s0 >= 0);

    s0 = 4;
    v0 = (s32)a1 + 4;
    do {
        *(u8 *)(v0 + 0x6E) = 0;
        s0--;
        v0--;
    } while (s0 >= 0);

    *(s32 *)(a1 + 0x70) = *(s32 *)(a1 + 0x70) & (s32)0xFEFFFFFF;
    *(s32 *)(a1 + 0x7C) = 1;

    s0 = 0;
    do {
        modifyItemQuantity(s0 + 0x4D, s0 + 0xC8);
        s0++;
    } while (s0 < 0x21);
}

/**
 * @brief Read save menu checksum value.
 * @return Value of D_801EC2E4.
 */
s32 func_801E74BC(void) {
    return D_801EC2E4;
}

/** @brief Draw inner panel with section id 0xE and clear flag. */
s32 func_801E74CC(s32 a0) {
    return func_801F08D4(1, 0xE, a0, 0);
}

/** @brief Draw inner panel with section id 0xE and set flag. */
s32 func_801E74F8(s32 a0) {
    return func_801F08D4(1, 0xE, a0, 1);
}

/**
 * @brief Compute save menu state from input flags.
 *
 * Returns -1 if input is -1, clears bit 4 and returns the result
 * if bit 4 is set, otherwise returns 4.
 *
 * @param a0 Input flags value.
 * @return Processed state value.
 */
s32 func_801E7524(s32 a0) {
    if (a0 == -1) {
        return -1;
    }
    if ((a0 & 0x10) == 0) {
        return 4;
    }
    return a0 & ~0x10;
}

/**
 * @brief Compute XOR checksum from game state fields.
 *
 * XORs three values from the g_gameState region (at offsets 0xCD0, 0xB0C,
 * and 0xCDC with the latter shifted left 16), then XORs with the result
 * of func_801E74BC.
 *
 * @return XOR checksum value.
 */
s32 func_801E7550(void) {
    s32 result;

    result = g_gameState.mainData.playTimeSeconds ^ g_gameState.mainData.party.gil ^ (g_gameState.mainData.fieldCDC << 0x10);
    return result ^ func_801E74BC();
}

/**
 * @brief Compute and store save data checksum fields.
 *
 * Gets save data pointer, checks bit 15 of halfword at offset 0xC.
 * If not set, calls several computation functions and stores results
 * at offsets 0x28 (XOR checksum) and 0x2C (combined checksum byte).
 */
void func_801E7598(void) {
    s32 s0;
    u8 *s1 = (u8 *)getChocoboWorldPtr();
    if ((*(u16 *)(s1 + 0xC) & 0x8000) == 0) {
        func_801EB928();
        func_801EB890();
        *(s32 *)(s1 + 0x28) = func_801E7550();
        s0 = func_801EB0B8();
        s0 += func_801E74BC();
        s1[0x2C] = s0;
    }
}

/**
 * @brief Process save menu input and update state flags.
 *
 * Reads a gamepad input byte from s0[0x3C], shifts left 4,
 * passes through func_801EAE98 then func_801E7524. Based on result:
 * - 0: checks flags at s1[0] and updates s0[0x45] with bit manipulation
 * - -1: stores 8 to s0[0x45]
 * - positive: stores result to s0[0x45]
 *
 * @param a0 Save menu context pointer.
 * @return Processed input result.
 */
/**
 * @brief Process save menu input and update state flags.
 *
 * Reads a gamepad input byte from a0[0x3C], shifts left 4,
 * passes through func_801EAE98 then func_801E7524. Based on result:
 * - 0: checks flags at save data and updates a0[0x45] with bit manipulation
 * - -1: stores 8 to a0[0x45]
 * - positive: stores result to a0[0x45]
 *
 * @param a0 Save menu context pointer.
 * @return Processed input result.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E760C); /* 0xB8 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E76C4); /* 0x16C */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7830); /* 0xCC */

/**
 * @brief Initialize save data structure with checksum and set active flag.
 *
 * Calls getChocoboWorldPtr to get the save data pointer, then func_801E74BC to
 * compute a checksum/value which is stored at offset 0x14. Sets bit 0x2
 * in the flags byte at offset 0x0.
 */
void func_801E78FC(void) {
    u8 *s0 = (u8 *)getChocoboWorldPtr();
    s32 v0 = func_801E74BC();
    *(s32 *)(s0 + 0x14) = v0;
    s0[0] |= 2;
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7938); /* 0x94 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E79CC); /* 0xB8 */

/**
 * @brief Poll for card data availability, up to 8 attempts.
 *
 * Tries func_8004F200 up to 8 times. If it succeeds, extracts
 * data via sfxUpdate and stores status 2. Returns the
 * result from the stack local.
 *
 * @param a0 Save context pointer (byte at 0x3C used as port).
 * @param a1 Second parameter for func_8004F200.
 * @return Card data result, or 2 if successful.
 */
static s32 func_801E7A84(func_801E7BE8_arg0 *a0, func_801E7BE8_D_801D1000 *a1) {
    s32 s0 = 0;
    s32 result;

    do {
        if (func_8004F200(a0->unk3C << 4, a1, 0x280, 0x80) != 0) {
            sfxUpdate(0, 0, &result);
            break;
        }
        s0++;
    } while (s0 < 8);

    if (s0 == 8) {
        result = 2;
    }

    return result;
}

/** @brief Call loadSubOverlay with a0 + 0xB4 and 0x801D3000. */
static void func_801E7B18(s32 a0) {
    loadSubOverlay(a0 + 0xB4, 0x801D3000);
}

/**
 * @brief Compute pointer to save slot data entry by index.
 * @param a0 Save slot index (stride 68).
 * @return Address of D_801EBD5C + a0 * 68.
 *
 */
static D_801EBD5C_Entry *func_801E7B40(s32 a0) {
    D_801EBD5C_Entry *base = D_801EBD5C;
    return &base[a0];
}

/**
 * @brief Advance save data loading state machine.
 *
 * Polls pollCdReadStatus for DMA completion. When complete, advances
 * the state byte at a0[0x44]:
 * - State 0: transition to 1
 * - State 1: initiate read via loadSubOverlay, transition to 2
 * - State 2: transition to 3
 * - State 3+: no-op
 *
 * @param a0 Save context pointer.
 */
/**
 * @brief Advance save data loading state machine.
 *
 * Polls pollCdReadStatus for DMA completion. When complete, advances
 * the state byte at a0[0x44]:
 * - State 0: transition to 1
 * - State 1: initiate read via loadSubOverlay, transition to 2
 * - State 2: transition to 3
 * - State 3+: no-op
 *
 * @param a0 Save context pointer.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7B5C); /* 0x8C */

static void func_801E7BE8(func_801E7BE8_arg0 *arg0) {
    u16 temp_fp;
    u16 temp_s2;
    u16 var_v0;
    u16 *temp_s6;

    temp_fp = g_menuDisplayCfg.inputRepeat;
    temp_s2 = g_menuDisplayCfg.inputNew;
    temp_s6 = &arg0->unk10;
    var_v0 = *temp_s6;
loop_1:
    switch (var_v0) {
    case 0x0:
        arg0->unk20 = func_801E74CC(0);
        *temp_s6 = 1;
        break;
    case 0x1:
        arg0->unk2C += 0x100;
        if (arg0->unk2C >= 0x1000) {
            arg0->unk2C = 0x1000;
            if (pollCdReadStatus() == 0) {
                *temp_s6 = 2;
            }
        }
        break;
    case 0x2:
        arg0->unk56 = 0;
        arg0->unk20 = func_801E74CC(0);
        *temp_s6 = 3;
        break;
    case 0x3:
        *temp_s6 = 4;
        break;
    case 0x4: {
        s32 temp_v0;

        func_801EAE74();
        do {
            temp_v0 = func_801EAE98(arg0->unk3C << 4);
        } while (temp_v0 == -1);
        temp_v0 = func_801E7524(temp_v0);
        if (temp_v0 < 0 || temp_v0 >= 2) {
            arg0->unk24 = func_801E74CC(0xA);
            arg0->unk16 = 0x46;
            arg0->unk18 = 0x46;
            *temp_s6 = 0x4A;
        } else {
            arg0->unk3E = 0;
            initCardEventHandlers();
            temp_v0 = pollCardReady(arg0->unk3C << 4);
            switch (temp_v0) {
            case 2:
                arg0->unk3E = 1;
            case 0:
                *temp_s6 = 5;
                break;
            default:
                arg0->unk24 = func_801E74CC(0xA);
                arg0->unk16 = 0x46;
                arg0->unk18 = 0x46;
                *temp_s6 = 0x4A;
                break;
            }
        }
        break;
    }
    case 0x5: {
        s32 temp_v0;

        if (arg0->unk3E == 0) {
            D_801EC2B0.name[0] = 0;
            temp_v0 = openFirstFileRetry(arg0->unk3C << 4, D_8005624C, &D_801EC2B0);
        } else {
            temp_v0 = 0;
        }
        if (temp_v0 == 0) {
            D_801EC2D0 = 0xFFFF;
            *temp_s6 = 6;
        } else {
            *temp_s6 = 0x1E;
        }
        break;
    }
    case 0x6: {
        struct DIRENTRY sp18;
        s32 var_s0;
        s32 temp_v0;

        if (arg0->unk3E == 0) {
            sp18.name[0] = 0;
            temp_v0 = openFirstFileRetry(arg0->unk3C << 4, "*", &sp18);
            for (var_s0 = 0; temp_v0 != 0; temp_v0 = cardNextFile(&sp18)) {
                var_s0 += sp18.size;
            }
        } else {
            var_s0 = 0;
        }
        arg0->unk42 = 0xF - var_s0 / 0x2000;
        if (arg0->unk42 >= 7) {
            *temp_s6 = 7;
        } else {
            arg0->unk24 = func_801E74CC(2);
            arg0->unk16 = 0x46;
            arg0->unk18 = 0x46;
            *temp_s6 = 0x4A;
        }
        break;
    }
    case 0x7:
        func_801EAE74();
        playSoundEffect(0x10);
        arg0->unk3F = 0;
        func_801F728C(func_801E74CC(1), 0x46);
        arg0->unk3D = 1;
        loadOverlayWithTimCallback(0xB4, 0x801D3000);
        D_801EC2F4 = 1;
        arg0->unk44 = 0;
        *temp_s6 = 8;
    case 0x8: {
        u32 temp_s0;

        temp_s0 = func_801E7524(func_801EAE98(arg0->unk3C << 4));
        func_801E7B5C(arg0);
        // The result is held unsigned and compared signed. A signed variable makes gcc merge the two compares.
        if ((s32)temp_s0 >= 2 || (s32)temp_s0 < -1) {
            arg0->unk24 = func_801E74CC(0xA);
            arg0->unk16 = 0x46;
            arg0->unk18 = 0x46;
            *temp_s6 = 0x4A;
            break;
        }
        arg0->unk3F = func_801F6768(temp_fp, 2, arg0->unk3F);
        func_801F6F88(arg0->unk3F);
        if (temp_s2 & PADRdown) {
            sendSpuCommand(2);
            arg0->unk3D = 0;
            if (arg0->unk3F != 0) {
                *temp_s6 = 0x46;
            } else {
                *temp_s6 = 0xA;
            }
        }
        if (temp_s2 & PADRup) {
            sendSpuCommand(3);
            *temp_s6 = 0x46;
        }
        break;
    }
    case 0x9:
        loadOverlayWithTimCallback(0xB4, 0x801D3000);
        D_801EC2F4 = 1;
        arg0->unk44 = 0;
        *temp_s6 = 0xA;
        break;
    case 0xA:
        func_801EAE4C();
        *temp_s6 = 0xB;
        break;
    case 0xB: {
        s32 temp_v0;

        func_801E7B5C(arg0);
        if ((temp_v0 = func_801E7524(func_801EAE98(arg0->unk3C << 4))) >= 2 || temp_v0 < -1) {
            arg0->unk24 = func_801E74CC(0xA);
            arg0->unk16 = 0x46;
            arg0->unk18 = 0x46;
            *temp_s6 = 0x4A;
            break;
        }
        if (arg0->unk44 == 3) {
            if (arg0->unk3E == 0) {
                *temp_s6 = 0x13;
            } else {
                *temp_s6 = 0xC;
            }
        }
        break;
    }
    case 0xC: {
        s32 temp_s0;

        arg0->unk20 = 0;
        playSoundEffect(0x10);
        temp_s0 = func_801E74CC(arg0->unk3C + 8);
        func_801EAE74();
        arg0->unk3F = 1;
        func_801F728C(temp_s0, 0x37);
        arg0->unk3D = 1;
        *temp_s6 = 0xD;
    }
    case 0xD: {
        s32 temp_v0;

        if ((temp_v0 = func_801E7524(func_801EAE98(arg0->unk3C << 4))) >= 2 || temp_v0 < -1) {
            arg0->unk24 = func_801E74CC(0xA);
            arg0->unk16 = 0x46;
            arg0->unk18 = 0x46;
            *temp_s6 = 0x4A;
            break;
        }
        arg0->unk3F = func_801F6768(temp_fp, 2, arg0->unk3F);
        func_801F6F88(arg0->unk3F);
        if (temp_s2 & PADRdown) {
            sendSpuCommand(2);
            arg0->unk3D = 0;
            if (arg0->unk3F != 0) {
                *temp_s6 = 0x46;
            } else {
                arg0->unk24 = func_801E2C90(0xD);
                arg0->unk30 = 1;
                *temp_s6 = 0xE;
            }
            if (temp_s2 & PADRup) {
                sendSpuCommand(3);
                *temp_s6 = 0x46;
            }
        }
        break;
    }
    case 0xE:
        *temp_s6 = 0xF;
        break;
    case 0xF:
        *temp_s6 = 0x10;
        break;
    case 0x10:
        if (func_800379AC(arg0->unk3C << 4) == 1) {
            *temp_s6 = 0x11;
        } else {
            arg0->unk24 = func_801E74CC(0x2A);
            arg0->unk16 = 0x46;
            arg0->unk18 = 0x46;
            *temp_s6 = 0x4A;
        }
        break;
    case 0x11:
        playSoundEffect(0x10);
        arg0->unk24 = func_801E2C90(0xE);
        arg0->unk30 = 0x3C;
        *temp_s6 = 0x12;
        break;
    case 0x12:
        arg0->unk30--;
        if (temp_s2 & (PADRup | PADRdown)) {
            func_801F7BEC(temp_s2);
            arg0->unk30 = 0;
        }
        if (arg0->unk30 <= 0) {
            *temp_s6 = 0x13;
        }
        break;
    case 0x13:
        arg0->unk4A = 0;
        arg0->unk20 = func_801E74CC(0x14);
        arg0->unk41 = 1;
        arg0->unk2E = 0;
        arg0->unk4B = 0;
        *temp_s6 = 0x14;
        break;
    case 0x14:
        arg0->unk2E += 0x199;
        if (arg0->unk2E >= 0x1000) {
            arg0->unk2E = 0x1000;
            *temp_s6 = 0x15;
        }
        break;
    case 0x15:
        *temp_s6 = 0x16;
        break;
    case 0x16:
        *temp_s6 = 0x17;
        break;
    case 0x17: {
        func_801E7BE8_sp40 sp40;
        s32 temp_s0;
        s32 temp_s0_2;
        s32 temp_s0_3;
        s32 var_s3;
        s32 var_s3_2;
        s32 var_s5;
        s32 temp_v0;

        var_s5 = arg0->unk4B << 0xE;
        var_s3 = 0x4000;
        if (arg0->unk4B == 2) {
            var_s3 = 0x6000;
        }
        if (arg0->unk4B == 0) {
            temp_s0 = func_801E2824();
            temp_v0 = createCardFile(arg0->unk3C << 4, D_8005624C, 0xE000);
            if (temp_v0 == -1) {
                var_v0 = 0x1D;
                goto loop_1;
            }
            if (temp_v0 == 2 && readCardCreate(arg0->unk3C << 4, D_8005624C, &sp40, 0x80, 0) == 0x80 && compareBytes(sp40.unk52, "MCX0", 4) != 0) {
                arg0->unk41 = 0;
                arg0->unk20 = 0;
                arg0->unk4A = 1;
                loadOverlayWithTimCallback(9, 0x801D5000);
                func_801EAE74();
                *temp_s6 = 0x20;
                break;
            }
            btlMemcpyForward(func_801E282C(), (void *)0x801D1000, temp_s0);
            ((func_801E7BE8_D_801D1000 *)0x801D1000)->unk3 = 7;
            copyString(((func_801E7BE8_D_801D1000 *)0x801D1000)->unk4, D_80056260);
            func_801EB00C(1, arg0->unk3C << 4, 1, 0x36);
            temp_v0 = writeCardSector(arg0->unk3C << 4, D_8005624C, 0x801D1000, temp_s0, 0);
            var_s3 -= temp_v0;
            if (temp_v0 != temp_s0) {
                func_801EB054(1, arg0->unk3C << 4);
                var_v0 = 0x1D;
                goto loop_1;
            }
            var_s5 += temp_v0;
        }
        if (writeCardSector(arg0->unk3C << 4, D_8005624C, var_s5 + 0x801D1800, var_s3, var_s5) != var_s3) {
            func_801EB054(1, arg0->unk3C << 4);
            var_v0 = 0x1D;
            goto loop_1;
        }
        if (arg0->unk4B == 2) {
            var_s3_2 = 0x80;
            temp_s0_2 = func_801E2824() - 0x80;
            if (writeCardSector(arg0->unk3C << 4, D_8005624C, 0x801D1880, temp_s0_2, var_s3_2) != temp_s0_2) {
                func_801EB054(1, arg0->unk3C << 4);
                var_v0 = 0x1D;
                goto loop_1;
            }
            temp_s0_3 = writeCardSector(arg0->unk3C << 4, D_8005624C, 0x801D1800, 0x80, 0);
            func_801EB054(1, arg0->unk3C << 4);
            if (temp_s0_3 != var_s3_2) {
                var_v0 = 0x1D;
                goto loop_1;
            }
            arg0->unk20 = func_801E74CC(0x1A);
            playSoundEffect(0x13);
        }
        func_801EAE74();
        *temp_s6 = 0x18;
        break;
    }
    case 0x18: {
        s32 temp_v0;

        temp_v0 = func_801E7524(func_801EAE98(arg0->unk3C << 4));
        if (temp_v0 != -1 && temp_v0 != 0) {
            var_v0 = 0x1D;
            goto loop_1;
        }
        arg0->unk54 = D_801EBD54[arg0->unk4B];
        *temp_s6 = 0x19;
        break;
    }
    case 0x19: {
        s32 temp_v0;

        temp_v0 = func_801E7524(func_801EAE98(arg0->unk3C << 4));
        if (temp_v0 != -1 && temp_v0 != 0) {
            var_v0 = 0x1D;
            goto loop_1;
        }
        if (arg0->unk4B == 2 && (temp_s2 & PADRdown)) {
            sendSpuCommand(2);
            arg0->unk54 = 0;
        }
        if (--arg0->unk54 < 0) {
            *temp_s6 = 0x1A;
        }
        break;
    }
    case 0x1A: {
        s32 temp_v0;

        temp_v0 = func_801E7524(func_801EAE98(arg0->unk3C << 4));
        if (temp_v0 != -1 && temp_v0 != 0) {
            var_v0 = 0x1D;
            goto loop_1;
        }
        arg0->unk2E -= 0x199;
        if (arg0->unk2E <= 0) {
            arg0->unk2E = 0;
            arg0->unk4B++;
            if (arg0->unk4B >= 3) {
                arg0->unk41 = 0;
                *temp_s6 = 0x1C;
            } else {
                *temp_s6 = 0x1B;
            }
        }
        break;
    }
    case 0x1B: {
        s32 temp_v0;

        temp_v0 = func_801E7524(func_801EAE98(arg0->unk3C << 4));
        if (temp_v0 != -1 && temp_v0 != 0) {
            var_v0 = 0x1D;
            goto loop_1;
        }
        arg0->unk2E += 0x199;
        if (arg0->unk2E >= 0x1000) {
            arg0->unk2E = 0x1000;
            func_801EAE4C();
            *temp_s6 = 0x16;
        }
        break;
    }
    case 0x1C: {
        s32 temp_s0;

        temp_s0 = func_801E74CC(0x1A);
        arg0->unk4A = 1;
        arg0->unk20 = temp_s0;
        if (D_801EC2F4 != 0) {
            loadOverlayWithTimCallback(9, 0x801D5000);
            D_801EC2F4 = 0;
        }
        *temp_s6 = 2;
        break;
    }
    case 0x1D:
        arg0->unk41 = 0;
        arg0->unk20 = 0;
        arg0->unk24 = func_801E74CC(7);
        arg0->unk16 = 0x46;
        arg0->unk18 = 0x46;
        *temp_s6 = 0x4A;
        break;
    case 0x1E: {
        u32 spC0;
        s32 var_a1;

        initCardEventHandlers();
        func_801EAE4C();
        if (func_800509C0(arg0->unk3C << 4, &D_801EC2E0) == 0) {
            arg0->unk24 = func_801E74CC(0xA);
            arg0->unk16 = 0x46;
            arg0->unk18 = 0x46;
            *temp_s6 = 0x4A;
        } else {
            sfxUpdate(0, 0, &spC0);
            var_a1 = D_801EC2E0;
            switch (spC0) {
            case 0:
            case 3:
                if (var_a1 != 0 && var_a1 != D_801EC2D0 / 64) {
                    arg0->unk24 = func_801E74CC(4);
                    arg0->unk16 = 0x46;
                    arg0->unk18 = 0x46;
                    *temp_s6 = 0x4A;
                } else {
                    *temp_s6 = var_a1 == 0 ? 0x21 : 0x24;
                }
                break;
            case 1:
            case 2:
            default:
                arg0->unk24 = func_801E74CC(0xA);
                arg0->unk16 = 0x46;
                arg0->unk18 = 0x46;
                *temp_s6 = 0x4A;
                break;
            }
        }
        break;
    }
    case 0x1F:
        *temp_s6 = 0x46;
        break;
    case 0x20: {
        s32 temp_v0;

        if ((temp_v0 = func_801E7524(func_801EAE98(arg0->unk3C << 4))) >= 2 || temp_v0 < -1) {
            arg0->unk24 = func_801E74CC(0xA);
            arg0->unk16 = 0x46;
            arg0->unk18 = 0x46;
            *temp_s6 = 0x4A;
            break;
        }
        if (pollCdReadStatus() == 0) {
            *temp_s6 = 2;
        }
        break;
    }
    case 0x21: {
        s32 var_s0;
        s32 var_s1;

        playSoundEffect(0x10);
        var_s1 = func_801E74CC(5);
        func_801EAE74();
        arg0->unk3F = 0;
        var_s0 = 0x42;
        if (GetGameVersion() == VERSION_SPAIN) {
            var_s0 = 0x38;
        }
        func_801F728C(var_s1, var_s0);
        arg0->unk3D = 1;
        *temp_s6 = 0x22;
        break;
    }
    case 0x22: {
        s32 temp_v0;

        if ((temp_v0 = func_801E7524(func_801EAE98(arg0->unk3C << 4))) >= 2 || temp_v0 < -1) {
            arg0->unk24 = func_801E74CC(0xA);
            arg0->unk16 = 0x46;
            arg0->unk18 = 0x46;
            *temp_s6 = 0x4A;
            break;
        }
        arg0->unk3F = func_801F6768(temp_fp, 2, arg0->unk3F);
        func_801F6F88(arg0->unk3F);
        if (temp_s2 & PADRdown) {
            sendSpuCommand(2);
            arg0->unk3D = 0;
            if (arg0->unk3F != 0) {
                *temp_s6 = 0x46;
            } else {
                *temp_s6 = 0x23;
            }
        }
        if (temp_s2 & PADRup) {
            sendSpuCommand(3);
            *temp_s6 = 0x46;
        }
        break;
    }
    case 0x23: {
        func_801E7BE8_sp40 spC8;
        u32 sp148;
        s32 temp_v0;

        temp_v0 = readCardCreate(arg0->unk3C << 4, D_8005624C, &spC8, 0x80, 0);
        sp148 = 2;
        if (temp_v0 == 0x80) {
            if (compareBytes(spC8.unk52, "MCX0", 4) != 0) {
                func_8004EC64(arg0->unk3C << 4, D_801EC2D0 / 64, 0);
                sfxUpdate(0, 0, &sp148);
            } else {
                func_801F728C(func_801E74CC(0x1B), 0x42);
                arg0->unk16 = 9;
                arg0->unk18 = 0x46;
                arg0->unk1A = 0x46;
                *temp_s6 = 0x48;
                break;
            }
        }
        if (sp148 == 0) {
            VSync(4);
            *temp_s6 = 2;
        } else {
            arg0->unk24 = func_801E74CC(0x13);
            arg0->unk16 = 0x46;
            arg0->unk18 = 0x46;
            *temp_s6 = 0x4A;
        }
        break;
    }
    case 0x24:
        func_801E7598();
        func_801EAE74();
        arg0->unk32 = 0x1000;
        arg0->unk45 = 8;
        *temp_s6 = 0x25;
        break;
    case 0x25:
        if (pollCdReadStatus() == 0) {
            var_v0 = *temp_s6 = 0x26;
            goto loop_1;
        }
        break;
    case 0x26:
        arg0->unk32 -= 0xAA;
        func_801E760C(arg0);
        if (arg0->unk45 & FUNC_801E7BE8_UNK45_FLAG_1) {
            arg0->unk46 = 1;
        } else {
            arg0->unk46 = 0;
        }
        if (arg0->unk32 <= 0) {
            arg0->unk32 = 0;
            *temp_s6 = 0x27;
        }
        func_801E7938(arg0, 1, arg0->unk46);
        break;
    case 0x27: {
        s32 temp_s0;

        arg0->unk56 = 0;
        arg0->unk46 = func_801F6768(temp_fp, 4, arg0->unk46);
        func_801E7938(arg0, 1, arg0->unk46);
        temp_s0 = func_801E760C(arg0);
        arg0->unk20 = func_801E74F8(D_801EBD4C[arg0->unk46]);
        if (temp_s0 == 1) {
            arg0->unk32 = 0x1000;
            *temp_s6 = 2;
            break;
        }
        if (temp_s2 & PADRup) {
            sendSpuCommand(3);
            *temp_s6 = 0x46;
        }
        if (temp_s2 & PADRdown) {
            if ((arg0->unk45 >> arg0->unk46) & 1) {
                switch (arg0->unk46) {
                case 0:
                    var_v0 = 0x28;
                    goto loop_1;
                case 1:
                    sendSpuCommand(2);
                    var_v0 = 0x2D;
                    goto loop_1;
                case 2:
                    sendSpuCommand(2);
                    var_v0 = 0x2A;
                    goto loop_1;
                case 3:
                    sendSpuCommand(2);
                    var_v0 = 0x36;
                    goto loop_1;
                }
            } else {
                sendSpuCommand(5);
            }
        }
        break;
    }
    case 0x28: {
        func_801E7BE8_D_801D1000 *var_s0;

        var_s0 = (func_801E7BE8_D_801D1000 *)0x801D1000;
        func_801EAE4C();
        if (func_801E79CC(arg0, var_s0) == 0) {
            if (var_s0->unkE != 0) {
                sendSpuCommand(5);
                arg0->unk24 = func_801E74CC(0x15);
                arg0->unk16 = 0x27;
                arg0->unk18 = 2;
                *temp_s6 = 0x4A;
                break;
            }
        } else {
            func_801EAE74();
            arg0->unk24 = func_801E74CC(0xC);
            arg0->unk16 = 0x27;
            arg0->unk18 = 2;
            *temp_s6 = 0x4A;
            break;
        }
        sendSpuCommand(2);
        func_801E7830(var_s0);
        if (func_801E7A84(arg0, var_s0) != 0) {
            func_801EAE74();
            arg0->unk24 = func_801E74CC(0xC);
            arg0->unk16 = 0x24;
            arg0->unk18 = 2;
            *temp_s6 = 0x4A;
        } else {
            func_801E2814();
            func_801E78FC();
            *temp_s6 = 0x29;
        }
        break;
    }
    case 0x29:
        func_801EAE74();
        var_v0 = *temp_s6 = 0x27;
        goto loop_1;
    case 0x2A: {
        s32 temp_v0;
        func_801E7BE8_D_801D1000 *var_s0;

        var_s0 = (func_801E7BE8_D_801D1000 *)0x801D1000;
        func_801EAE4C();
        arg0->unk56 = 1;
        if (func_801E79CC(arg0, var_s0) == 0) {
            temp_v0 = 0x16;
            if (var_s0->unkE != 0) {
                temp_v0 = 0x17;
            }
            func_801F728C(func_801E74CC(temp_v0), 0x3E);
            arg0->unk16 = 0x2B;
            arg0->unk18 = 0x27;
            arg0->unk1A = 0x27;
            *temp_s6 = 0x48;
        } else {
            arg0->unk24 = func_801E74CC(0xC);
            arg0->unk16 = 0x27;
            arg0->unk18 = 2;
            *temp_s6 = 0x4A;
        }
        break;
    }
    case 0x2B: {
        s32 temp_s0;

        arg0->unk56 = 0;
        func_801E7830((func_801E7BE8_D_801D1000 *)0x801D1000);
        temp_s0 = func_801E7550();
        ((func_801E7BE8_D_801D1000 *)0x801D1000)->unk28 = temp_s0;
        if (func_801E7A84(arg0, (func_801E7BE8_D_801D1000 *)0x801D1000) != 0) {
            func_801EAE74();
            arg0->unk24 = func_801E74CC(0xC);
            arg0->unk16 = 0x24;
            arg0->unk18 = 2;
            *temp_s6 = 0x4A;
        } else {
            func_801E2814();
            func_801E78FC();
            func_801E760C(arg0);
            ((ChocoboWorldData *)getChocoboWorldPtr())->associatedSaveId = temp_s0;
            *temp_s6 = 0x2C;
        }
        break;
    }
    case 0x2C:
        func_801EAE74();
        *temp_s6 = 0x27;
        break;
    case 0x2D: {
        s32 temp_s0;
        func_801E7BE8_D_801D1000 *var_s1;

        func_801E7938(arg0, 1, arg0->unk46);
        var_s1 = (func_801E7BE8_D_801D1000 *)0x801D1000;
        temp_s0 = func_801E79CC(arg0, var_s1);
        arg0->unk24 = func_801E74CC(0xC);
        if (temp_s0 == 0) {
            if (func_801E76C4(arg0, var_s1) == 0) {
                arg0->unk24 = func_801E74CC(0x18);
            } else {
                var_s1->unkE = 3;
                if (func_801E7A84(arg0, var_s1) == 0) {
                    var_s1->unk28 = func_801E7550();
                    var_s1->unkE = 0;
                    func_801E2814();
                    *temp_s6 = 0x2E;
                    break;
                }
            }
        }
        arg0->unk16 = 0x27;
        arg0->unk18 = 2;
        *temp_s6 = 0x4A;
        break;
    }
    case 0x2E: {
        ChocoboWorldData *sp14C;
        u8 *temp_v1;
        u8 *var_s1;
        u8 *var_s2;
        s32 var_s3;
        u8 *var_s5;
        u8 *var_s8;
        s32 var_s0;
        s32 var_s1_2;

        sp14C = (ChocoboWorldData *)getChocoboWorldPtr();
        func_801E7938(arg0, 1, arg0->unk46);
        var_s5 = (u8 *)0x801D3000;
        for (var_s0 = 0; var_s0 < 0xC7; var_s0++) {
            *var_s5 = 0;
            var_s5++;
        }
        var_s3 = 0;
        var_s8 = D_801EC08C;
        var_s5 = (u8 *)0x801D3000;
        for (var_s0 = 0; var_s0 < 4; var_s0++) {
            for (var_s1_2 = 0; var_s1_2 < arg0->unk50[var_s0]; var_s1_2++) {
                temp_v1 = &var_s5[(var_s8 + var_s0)[(func_801EB0B8() % 128) * 4]];
                (*temp_v1)++;
                if (*temp_v1 >= 100) {
                    *temp_v1 = 100;
                }
                var_s3++;
            }
        }
        if (arg0->unk49 != 0) {
            var_s3++;
            var_s5[0x41]++;
        }
        if (var_s3 == 0) {
            VSync(4);
            *temp_s6 = 0x27;
        } else {
            var_s2 = (u8 *)0x801D3000;
            var_s1 = (u8 *)0x801D3100;
            var_s3 = 0;
            for (var_s0 = 0; var_s0 < 0xC7; var_s0++) {
                if (*var_s2 != 0) {
                    if (var_s0 == 0x64) {
                        setFieldFlag(0x7B);
                    }
                    if (var_s0 == 0x41 && !(sp14C->flags & CHOCOBO_WORLD_FLAG_2)) {
                        var_s0 = 0x6E;
                    }
                    *var_s1 = var_s0;
                    var_s1++;
                    var_s3++;
                    *var_s1 = *var_s2;
                    var_s1++;
                }
                var_s2++;
            }
            arg0->unk48 = var_s3;
            *temp_s6 = 0x2F;
        }
        break;
    }
    case 0x2F:
        arg0->unk20 = func_801E74CC(0x12);
        arg0->unk3A = 0;
        arg0->unk34 = 0x1000;
        func_801E760C(arg0);
        *temp_s6 = 0x30;
    case 0x30:
        func_801E7938(arg0, 0, arg0->unk46);
        func_801E760C(arg0);
        arg0->unk34 -= 0xAA;
        if (arg0->unk34 <= 0) {
            arg0->unk34 = 0;
            *temp_s6 = 0x31;
        }
        break;
    case 0x31:
        func_801E760C(arg0);
        func_801E7938(arg0, 0, arg0->unk46);
        *temp_s6 = 0x32;
        break;
    case 0x32:
        func_801E760C(arg0);
        func_801E7938(arg0, 0, arg0->unk46);
        if (temp_s2 & PADRdown) {
            sendSpuCommand(8);
            if ((arg0->unk3A + 1) * 11 >= arg0->unk48) {
                *temp_s6 = 0x34;
            } else {
                arg0->unk3B = arg0->unk3A;
                arg0->unk38 = 0xE67;
                arg0->unk3A++;
                *temp_s6 = 0x33;
            }
        }
        break;
    case 0x33:
        func_801E760C(arg0);
        arg0->unk38 -= 0x199;
        if (arg0->unk38 <= 0) {
            arg0->unk38 = 0;
            *temp_s6 = 0x32;
        }
        if (temp_s2 & PADRdown) {
            arg0->unk38 = 0;
            var_v0 = 0x32;
            goto loop_1;
        }
        func_801E7938(arg0, 0, arg0->unk46);
        break;
    case 0x34: {
        u8 temp_a0;
        u8 temp_a1;
        u8 *var_s0;
        s32 var_s1;

        var_s0 = (u8 *)0x801D3100;
        arg0->unk20 = 0;
        for (var_s1 = 0; var_s1 < arg0->unk48; var_s1++) {
            temp_a0 = *var_s0;
            var_s0++;
            temp_a1 = *var_s0;
            var_s0++;
            addItemToInventory(temp_a0, temp_a1);
        }
        func_801E760C(arg0);
        func_801F6888();
        *temp_s6 = 0x35;
    }
    case 0x35:
        func_801E7938(arg0, 0, arg0->unk46);
        func_801E760C(arg0);
        arg0->unk34 += 0xAA;
        if (arg0->unk34 >= 0x1000) {
            arg0->unk34 = 0x1000;
            *temp_s6 = 0x27;
        }
        break;
    case 0x36:
        arg0->unk4F = 0;
        *temp_s6 = 0x37;
        func_801E7938(arg0, 1, arg0->unk46);
        break;
    case 0x37:
        func_801E7B18(func_801E7B40(D_801EBD3C[arg0->unk4F])->unk17);
        D_801EC2F4 = 1;
        *temp_s6 = 0x38;
    case 0x38:
        func_801E760C(arg0);
        arg0->unk36 -= 0x199;
        if (arg0->unk36 <= 0) {
            arg0->unk36 = 0;
            arg0->unk4B = D_801EBD3C[arg0->unk4F];
            *temp_s6 = 0x39;
        }
        func_801E7938(arg0, 1, arg0->unk46);
        break;
    case 0x39:
        if (pollCdReadStatus() == 0) {
            *temp_s6 = 0x3A;
        }
        break;
    case 0x3A:
        loadTimImage((u8 *)0x801D3000);
        arg0->unk41 = 1;
        arg0->unk20 = func_801E74CC(0x25);
        *temp_s6 = 0x3B;
        break;
    case 0x3B:
        func_801E760C(arg0);
        arg0->unk2E += 0x199;
        if (arg0->unk2E >= 0x1000) {
            arg0->unk2E = 0x1000;
            *temp_s6 = 0x3C;
        }
        break;
    case 0x3C:
        *temp_s6 = 0x3D;
    case 0x3D: {
        s32 temp_s0;

        temp_s0 = arg0->unk4F;
        arg0->unk4F = func_801F6800(temp_fp, arg0->unk4E, temp_s0);
        func_801E760C(arg0);
        if (arg0->unk4F != temp_s0) {
            *temp_s6 = 0x3E;
        }
        if (temp_s2 & PADRup) {
            sendSpuCommand(3);
            *temp_s6 = 0x41;
        }
        break;
    }
    case 0x3E:
        func_801E7B18(func_801E7B40(D_801EBD3C[arg0->unk4F])->unk17);
        *temp_s6 = 0x3F;
        break;
    case 0x3F:
        func_801E760C(arg0);
        arg0->unk2E -= 0x199;
        if (arg0->unk2E <= 0) {
            arg0->unk2E = 0;
            arg0->unk4B = D_801EBD3C[arg0->unk4F];
            *temp_s6 = 0x40;
        }
        break;
    case 0x40:
        func_801E760C(arg0);
        if (pollCdReadStatus() == 0) {
            *temp_s6 = 0x3A;
        }
        break;
    case 0x41:
        loadSubOverlay(9, 0x801D5000);
        *temp_s6 = 0x42;
    case 0x42:
        func_801E760C(arg0);
        arg0->unk2E -= 0x199;
        if (arg0->unk2E <= 0) {
            arg0->unk2E = 0;
            *temp_s6 = 0x43;
        }
        break;
    case 0x43:
        func_801E760C(arg0);
        arg0->unk41 = 0;
        *temp_s6 = 0x44;
        break;
    case 0x44:
        func_801E760C(arg0);
        if (pollCdReadStatus() == 0) {
            loadTimImage((u8 *)0x801D5000);
            D_801EC2F4 = 0;
            arg0->unk20 = func_801E74F8(D_801EBD3C[arg0->unk4F]);
            if (arg0->unk45 == 8) {
                arg0->unk32 = 0x1000;
                arg0->unk36 = 0x1000;
                arg0->unk4A = 1;
                *temp_s6 = 2;
            } else {
                *temp_s6 = 0x45;
            }
        }
        break;
    case 0x45:
        func_801E760C(arg0);
        arg0->unk36 += 0x199;
        if (arg0->unk36 >= 0x1000) {
            arg0->unk36 = 0x1000;
            *temp_s6 = 0x27;
        }
        func_801E7938(arg0, 1, arg0->unk46);
        break;
    case 0x46:
        arg0->unk3D = 0;
        sndSeqPlayPan7bit(D_801EC2F8, 0, 0x10, 0);
        func_801EAE4C();
        if (D_801EC2F4 != 0) {
            loadOverlayWithTimCallback(9, 0x801D5000);
            D_801EC2F4 = 0;
        }
        *temp_s6 = 0x47;
        if (arg0->unk36 != 0) {
            func_801E7938(arg0, 1, arg0->unk46);
        }
        break;
    case 0x47:
        arg0->unk2C -= 0x100;
        if (arg0->unk2C <= 0) {
            arg0->unk2C = 0;
            sndCmd21(D_801EC2F8);
            dispatchDrawCallback();
            func_801F18FC(arg0->task);
            func_801F0BB0();
        }
        if (arg0->unk36 != 0) {
            func_801E7938(arg0, 1, arg0->unk46);
        }
        break;
    case 0x48:
        arg0->unk3F = 1;
        arg0->unk3D = 1;
        func_801EAE74();
        *temp_s6 = 0x49;
        break;
    case 0x49: {
        s32 temp_v0;

        if ((temp_v0 = func_801E7524(func_801EAE98(arg0->unk3C << 4))) >= 2 || temp_v0 < -1) {
            arg0->unk3D = 0;
            *temp_s6 = arg0->unk1A;
            break;
        }
        arg0->unk3F = func_801F6768(temp_fp, 2, arg0->unk3F);
        if (temp_s2 & PADRup) {
            sendSpuCommand(3);
            arg0->unk3D = 0;
            *temp_s6 = arg0->unk18;
        } else if (temp_s2 & PADRdown) {
            sendSpuCommand(2);
            arg0->unk3D = 0;
            if (arg0->unk3F != 0) {
                *temp_s6 = arg0->unk18;
            } else {
                *temp_s6 = arg0->unk16;
            }
        } else {
            func_801F6F88(arg0->unk3F);
        }
        break;
    }
    case 0x4A:
        arg0->unk3D = 0;
        arg0->unk30 = 0x258;
        func_801EAE74();
        arg0->unk47 = 0;
        *temp_s6 = 0x4B;
        break;
    case 0x4B: {
        s32 temp_v0;

        temp_v0 = func_801E760C(arg0);
        if (temp_v0 != -1) {
            if (temp_v0 == 1) {
                arg0->unk47 = 1;
            }
            if (temp_v0 == 3) {
                arg0->unk47 = 1;
            }
        }
        arg0->unk30--;
        if (temp_s2 & (PADRup | PADRdown)) {
            func_801F7BEC(temp_s2);
            arg0->unk30 = 0;
        }
        if (arg0->unk30 <= 0) {
            arg0->unk30 = 0;
            if (arg0->unk47 != 0) {
                arg0->unk32 = 0x1000;
                arg0->unk34 = 0x1000;
                *temp_s6 = arg0->unk18;
            } else {
                *temp_s6 = arg0->unk16;
            }
        }
        break;
    }
    }
    func_801F0948(arg0->unk2C * arg0->unk36 / 4096);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9804); /* 0xC4 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E98C8); /* 0xC0 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9988); /* 0xA8 */

/**
 * @brief Render a centered text string in the save menu.
 *
 * If the string pointer a2 is non-null, computes the pixel width
 * via measureMessage and func_801F7394, centers it within 0xD8 pixels,
 * and renders via func_801F4274.
 *
 * @param a0 First render parameter (passed through).
 * @param a1 Second render parameter / default return value.
 * @param a2 String pointer (null to skip rendering).
 * @return Render result from func_801F4274, or a1 if a2 is null.
 */
s32 func_801E9A30(s32 a0, s32 a1, s32 a2) {
    s32 s0 = a1;
    if (a2 != 0) {
        s32 v0 = measureMessage(a2);
        v0 = func_801F7394((u32)v0 >> 16);
        s0 = func_801F4274(a0, s0, a2, 0xC0, (u32)(0xD8 - v0) >> 1, 0x1000);
    }
    return s0;
}

/**
 * @brief Look up sprite data address from table at 0x801CE800.
 *
 * Reads halfword offset at index a0 from the table, adds to base.
 *
 * @param a0 Sprite index.
 * @return Base address + halfword offset from table entry.
 */
s32 func_801E9AB4(s32 a0) {
    s32 base = 0x801CE800;
    return *(u16 *)(base + a0 * 2 + 2) + base;
}

/** @brief Return base address of save sprite data (0x801CD000). */
s32 func_801E9AD0(void) {
    return 0x801CD000;
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9ADC); /* 0xE8 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9BC4); /* 0x110 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9CD4); /* 0xB0 */

/**
 * @brief Copy rectangle parameters with offset and dispatch rendering.
 *
 * Copies 4 halfwords from the 5th arg pointer into g_menuDisplayCfg,
 * adding a2 to the X coordinate and a3 to the Y coordinate.
 * Then calls func_801E9CD4 with the modified display config.
 *
 * @param a0 First render parameter (passed through).
 * @param a1 Second render parameter (passed through).
 * @param a2 X offset added to source X coordinate.
 * @param a3 Y offset added to source Y coordinate.
 * @param src Pointer to source rectangle (4 halfwords: x, y, w, h).
 */
void func_801E9D84(s32 a0, s32 a1, s32 a2, s32 a3, u16 *src) {
    g_menuDisplayCfg.rect.x = src[0] + a2;
    g_menuDisplayCfg.rect.y = src[1] + a3;
    g_menuDisplayCfg.rect.w = src[2];
    g_menuDisplayCfg.rect.h = src[3];
    func_801E9CD4(a0, a1, (s32)&g_menuDisplayCfg, g_menuTint[MENU_TINT_NORMAL], (s32)src);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9DE8); /* 0x128 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9F10); /* 0x124 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA034); /* 0x14C */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA180); /* 0xF8 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA278); /* 0x104 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA37C); /* 0xC8 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA444); /* 0x9C */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA4E0); /* 0x160 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA640); /* 0x2C4 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA904); /* 0xA8 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA9AC); /* 0x37C */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EAD28); /* 0x124 */

/** @brief Call sfxInit with all arguments set to zero. */
static void func_801EAE4C(void) {
    sfxInit(0, 0, 0);
}

/**
 * @brief Call func_801EAE4C then clear D_801EC301.
 */
/**
 * @brief Call func_801EAE4C then clear D_801EC301.
 */
static void func_801EAE74(void) {
    func_801EAE4C();
    D_801EC301 = 0;
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EAE98); /* 0x174 */

/**
 * @brief Conditionally read card data and extract value.
 * @param a0 Enable flag (non-zero to proceed)
 * @param a1 First parameter for func_800502C8
 * @param a2 Second parameter for func_800502C8
 * @param a3 Third parameter for func_800502C8
 * @return Extracted card data value, or 0 if disabled or read fails
 */
s32 func_801EB00C(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result = 0;
    if (a0 != 0) {
        if (func_800502C8(a1, a2, a3) != 0) {
            sfxUpdate(0, 0, &result);
        }
    }
    return result;
}

/**
 * @brief Conditionally check card status and extract value.
 * @param a0 Enable flag (non-zero to proceed)
 * @param a1 Parameter for func_800503F8
 * @return Extracted card status value, or 0 if disabled or check fails
 */
s32 func_801EB054(s32 a0, s32 a1) {
    s32 result = 0;
    if (a0 != 0) {
        if (func_800503F8(a1) != 0) {
            sfxUpdate(0, 0, &result);
        }
    }
    return result;
}

/**
 * @brief Convert a BCD-encoded byte to binary.
 *
 * Extracts high nibble, multiplies by 10, adds low nibble.
 *
 * @param a0 BCD-encoded byte value.
 * @return Binary value (0-99).
 */
s32 func_801EB094(s32 a0) {
    s32 hi = (u32)(a0 & 0xFF) >> 4;
    return (hi * 10 + (a0 & 0xF)) & 0xFF;
}

/**
 * @brief Get low byte of save data value via signed modulo.
 * @return Low byte (0-255) of the value returned by func_801F6A5C
 */
s32 func_801EB0B8(void) {
    return (u8)(func_801F6A5C() % 256);
}

/**
 * @brief Convert a value to variable-radix digit string using D_801EBCF8 table.
 *
 * For each entry in the radix table, repeatedly subtracts the divisor from
 * a0 to extract a digit, adds the character offset a2, and stores the
 * result into output buffer a1. Null-terminates the output.
 *
 * @param a0 Value to convert.
 * @param a1 Output character buffer.
 * @param a2 Character offset added to each digit.
 */
/**
 * @brief Convert a value to variable-radix digit string using D_801EBCF8 table.
 *
 * For each entry in the radix table, repeatedly subtracts the divisor from
 * a0 to extract a digit, adds the character offset a2, and stores the
 * result into output buffer a1. Null-terminates the output.
 *
 * @param a0 Value to convert.
 * @param a1 Output character buffer.
 * @param a2 Character offset added to each digit.
 */
/**
 * @brief Convert a value to variable-radix digit string using D_801EBCF8 table.
 *
 * For each entry in the radix table, repeatedly subtracts the divisor from
 * a0 to extract a digit, adds the character offset a2, and stores the
 * result into output buffer a1. Null-terminates the output.
 *
 * @param a0 Value to convert.
 * @param a1 Output character buffer.
 * @param a2 Character offset added to each digit.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB0F4); /* 0x5C */

/**
 * @brief Convert a value to variable-radix digit string using D_801EBD14 table.
 *
 * Same algorithm as func_801EB0F4 but uses the D_801EBD14 radix table.
 *
 * @param a0 Value to convert.
 * @param a1 Output character buffer.
 * @param a2 Character offset added to each digit.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB150); /* 0x5C */

/**
 * @brief Convert a 32-bit value to 8 hex digit characters.
 *
 * Extracts nibbles from a0 (LSB first) and writes them as characters
 * (with offset a2) into buffer a1, right-to-left. Null-terminates.
 *
 * @param a0 Value to convert.
 * @param a1 Output buffer pointer (writes 9 bytes: 8 chars + null).
 * @param a2 Character offset added to each nibble value.
 */
void func_801EB1AC(s32 a0, u8 *a1, s32 a2) {
    s32 i;
    a1 += 8;
    *a1 = 0;
    i = 7;
    do {
        a1--;
        *a1 = (a0 & 0xF) + a2;
        i--;
        a0 = (u32)a0 >> 4;
    } while (i >= 0);
}

/**
 * @brief Accumulate 3 nibbles into a 12-bit value from func_801EB150 output.
 *
 * Calls func_801EB150 to fill a 3-byte buffer, then combines the bytes as:
 * result = (byte0 << 8) | (byte1 << 4) | byte2.
 *
 * @param a0 Parameter passed to func_801EB150
 * @return 12-bit accumulated value
 *
 */
s32 func_801EB1DC(s32 a0) {
    u8 buf[8];
    s32 result;
    s32 i;
    u8 *ptr;

    func_801EB150(a0, buf, 0);
    ptr = buf;
    result = 0;
    i = 2;
    do {
        result = result << 4;
        result = result + *ptr;
        ptr++;
        i--;
    } while (i >= 0);
    return result;
}

/**
 * @brief Convert BCD value, add offset, clamp to 99, and encode.
 *
 * Converts a0 from BCD (upper nibble * 10 + lower nibble), adds a1,
 * clamps to maximum 99 (0x63), then encodes via func_801EB1DC.
 *
 * @param a0 BCD-encoded input value
 * @param a1 Offset to add
 * @return Encoded result masked to byte
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB224); /* 0x4C */

/**
 * @brief Convert BCD value, subtract offset, clamp to 0, and encode.
 *
 * Converts a0 from BCD (upper nibble * 10 + lower nibble), subtracts a1,
 * clamps to minimum 0, then encodes via func_801EB1DC.
 *
 * @param a0 BCD-encoded input value
 * @param a1 Offset to subtract
 * @return Encoded result masked to byte
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB270); /* 0x48 */

/**
 * @brief Convert BCD-encoded byte to decimal, or return 100 if zero.
 *
 * If a0 is zero, returns 100 (0x64). Otherwise extracts high nibble,
 * multiplies by 10, adds low nibble for BCD-to-binary conversion.
 *
 * @param a0 BCD-encoded value.
 * @return Decimal value, or 100 if input is zero.
 */
/**
 * @brief Convert BCD-encoded byte to decimal, or return 100 if zero.
 *
 * If a0 is zero, returns 100 (0x64). Otherwise extracts high nibble,
 * multiplies by 10, adds low nibble for BCD-to-binary conversion.
 *
 * @param a0 BCD-encoded value.
 * @return Decimal value, or 100 if input is zero.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB2B8); /* 0x2C */

/**
 * @brief Convert save checksum to packed color value.
 *
 * Calls func_801E74BC to get checksum, masks to 24 bits, converts
 * via func_801EB0F4 to RGB bytes, then packs as (R << 8) + (G << 4) + B.
 *
 * @return Packed 12-bit color value.
 *
 */
s32 func_801EB2E4(void) {
    u8 buf[16];
    s32 v0;

    v0 = func_801E74BC();
    func_801EB0F4(v0 & 0xFFFFFF, buf, 0);
    return (buf[7] << 8) + (buf[8] << 4) + buf[9];
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB334); /* 0xD4 */

/**
 * @brief Look up save icon display value from table.
 *
 * Converts a0 to BCD via func_801EB2B8 (masked to byte), shifts right by 2
 * and adds 6, then adds the halfword from D_801EBD24[a1] to compute
 * the final value passed to func_801EB1DC.
 *
 * @param a0 Input value (masked to byte)
 * @param a1 Table index into D_801EBD24
 * @return Result of func_801EB1DC
 */
s32 func_801EB408(s32 a0, s32 a1) {
    s32 v0;
    s32 a0_new;

    v0 = func_801EB2B8(a0 & 0xFF);
    a0_new = D_801EBD24[a1];
    v0 = (v0 >> 2) + 6;
    return func_801EB1DC(a0_new + v0);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB458); /* 0x120 */

/**
 * @brief Find a unique coordinate pair by repeated random sampling.
 *
 * Extracts target x from a0>>10 and target y from a1>>10. Repeatedly
 * calls func_801EB0B8 to get random values, converting to coordinates
 * via (val * 9) >> 7. Loops until the generated coordinates differ
 * from the targets.
 *
 * @param a0 Target x coordinate (encoded in bits 10+).
 * @param a1 Target y coordinate (encoded in bits 10+).
 * @return Combined coordinate: x | (y << 8).
 */
s32 func_801EB578(s32 a0, s32 a1) {
    s32 targetX = a0 >> 10;
    s32 targetY = a1 >> 10;
    s32 x, y;

    do {
        x = (u32)((func_801EB0B8() & 0xFF) * 9) >> 7;
        y = (u32)((func_801EB0B8() & 0xFF) * 9) >> 7;
    } while (x == targetX && y == targetY);

    return x | (y << 8);
}

/**
 * @brief Search a byte-pair table for a matching entry.
 *
 * Iterates through @p count consecutive byte pairs in @p table.
 * Returns 0 if a pair matching (@p key1, @p key2) is found,
 * or 1 if no match exists.
 *
 * @param key1 First byte to match.
 * @param key2 Second byte to match.
 * @param count Number of pairs to search.
 * @param table Pointer to byte pair array.
 * @return 0 if match found, 1 otherwise.
 */
s32 func_801EB5F0(s32 key1, s32 key2, s32 count, u8 *table) {
    s32 i;

    for (i = 0; i < count; i++) {
        u8 first = *table++;
        u8 second = *table++;

        if (key1 == first && key2 == second)
            return 0;
    }

    return 1;
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB630); /* 0x84 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB6B4); /* 0xB4 */

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB768); /* 0xE8 */

/**
 * @brief Update save data byte at offset 3 based on bytes at offsets 1 and 6.
 *
 * Gets save data pointer via getChocoboWorldPtr, refreshes via func_801EB6B4,
 * then computes a new value from bytes at offsets 1 and 6 using func_801EB408.
 */
void func_801EB850(void) {
    u8 *s0 = (u8 *)getChocoboWorldPtr();
    func_801EB6B4();
    s0[3] = func_801EB408(s0[1], s0[6]);
}

/**
 * @brief Compute and store save data timing values.
 *
 * Gets save data pointer, computes values from func_801EB408 and
 * func_801EB0B8, transforms with multiply-shift (val * 9 >> 7),
 * and stores at offsets 0x03, 0x10, 0x12, 0x32 in the save data.
 * Finally calls func_801EB850 to update checksum.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB890); /* 0x98 */

/**
 * @brief Initialize save data structure with default values.
 *
 * Gets save data pointer, computes initial color and timing values
 * via func_801EB2E4, func_801EB334, func_801EB408, func_801EB458.
 * Sets various flag bytes and clears timing fields.
 */
void func_801EB928(void) {
    s32 s0 = 1;
    u8 *s1 = (u8 *)getChocoboWorldPtr();
    s32 v0;
    s32 a0;

    v0 = func_801EB2E4();
    v0 |= 0x8000;
    *(u16 *)(s1 + 0xC) = v0;
    a0 = *(u16 *)(s1 + 0xC);
    s1[1] = s0;
    v0 = func_801EB334(a0);
    s1[6] = v0;
    v0 = func_801EB408(s1[1], s1[6]);
    s1[3] = v0;
    s1[2] = v0;
    v0 = func_801EB458(s1[3]);
    *(u16 *)(s1 + 4) = v0;
    s1[7] = 4;
    s1[0xF] = 2;
    s1[0] = s0;
    s1[0x2F] = 7;
    s1[0x2C] = 0;
    {
        s32 i = 3;
        u8 *p = s1 + 3;
        do {
            *(p + 0x14) = 0;
            i--;
            p--;
        } while (i >= 0);
    }
    s1[0x2E] = 0;
    *(s32 *)(s1 + 8) = 0;
    s1[0x2D] = 0;
}
