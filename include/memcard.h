#ifndef MEMCARD_H
#define MEMCARD_H

#include "common.h"

#define MEMORY_CARD_FRAME_SIZE 0x80 // The minimum amount of bytes that can be managed on a memory card
#define MEMORY_CARD_BLOCK_SIZE 0x2000 // 8kb per block
#define MEMORY_CARD_TOTAL_CAPACITY 0x1E000 // 120KB (15 blocks)
#define MEMORY_CARD_OPEN_CREATE 0x200
#define MEMORY_CARD_OPEN_READWRITE 1

extern s32 func_800377B4(s32 len, u8 *buf);
extern void mcZeroFrame(u8 *ptr);
extern s32 func_800379AC(s32 arg0);

typedef enum {
    VERSION_JAPAN = 0,
    VERSION_USA = 1,
    VERSION_GERMANY = 2,
    VERSION_FRANCE = 3,
    VERSION_SPAIN = 4,
    VERSION_ITALY = 5,
    VERSION_EUROPE = 6
} GameVersion;

extern void InitGameVersion(void);
extern GameVersion GetGameVersion(void);

#endif
