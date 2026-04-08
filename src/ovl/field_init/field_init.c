#include "common.h"
#include "cd.h"
#include "battle.h"
#include "field_init_font.h"

/** @brief Kernel event control block entity (stride 0xC0). */
typedef struct {
    u8 pad00[0x94];
    s32 status;         /* 0x94 */
    u8 pad98[0x28];
} EventEntry; /* 0xC0 bytes */

/** @brief Kernel script/event state at fixed address 0x100. */
typedef struct {
    u8 pad00[0x10];
    EventEntry *entries; /* 0x10 */
} EventState;

/** @brief Entity identity at tail of AnimEntity. */
typedef struct {
    u8 entityId;
    u8 entityType;
} AnimEntityTail;

/** @brief Single animation entity (0xC4 bytes). */
typedef struct {
    u8 flags;            /* 0x00 */
    u8 pad01[5];         /* 0x01-0x05 */
    u8 field_06[2];      /* 0x06-0x07 */
    u8 pad08[3];         /* 0x08-0x0A */
    u8 field_0B;         /* 0x0B */
    u8 field_0C;         /* 0x0C */
    u8 field_0D;         /* 0x0D */
    u8 field_0E;         /* 0x0E */
    u8 field_0F;         /* 0x0F */
    s16 field_10;        /* 0x10 */
    s16 field_12;        /* 0x12 */
    u16 field_14;        /* 0x14 */
    s16 field_16;        /* 0x16 */
    u8 pad18;            /* 0x18 */
    u8 field_19;         /* 0x19 */
    u8 pad1A;            /* 0x1A */
    u8 field_1B;         /* 0x1B */
    AnimFrame frames[8]; /* 0x1C-0xBB */
    u8 colors[6];        /* 0xBC-0xC1 */
    AnimEntityTail tail;  /* 0xC2-0xC3 */
} AnimEntity;

/** @brief Top-level animation data block (g_battleAnims). */
typedef struct {
    AnimEntity entities[2];
    u8 pad188[0x58];
    u8 field_1E0;
    u8 field_1E1;
    u8 pad1E2;
    u8 field_1E3;
} AnimData;

/* --- Externs (sorted by address) --- */

extern s8 D_8005F170;
extern u8 D_80078E00[];
extern BattleConfig g_battleConfig;
extern CardDataBlock g_cardData;
extern u8 D_8008369C[];
extern CdFileDesc D_80097800;

/* --- Forward declarations --- */
void func_800983B8(void);

/* --- Helpers --- */

static inline EventState *getEventState(void) {
    return (EventState *)0x100;
}

/* --- Functions --- */

/** @brief Field initialization entry: call setup then init step. */
void func_80098000(void) {
    func_8001F5C8();
    func_800980B0();
}

/**
 * @brief Initialize battle command state and load battle data from CD.
 *
 * Clears the battle config, fills the 3 command slots with 0xFF,
 * then reads battle data from disc into a scratch buffer and copies
 * it to the battle data buffer (D_80078E00).
 */
void func_80098028(void) {
    s32 i;

    g_battleConfig.battleSceneId = 0;
    g_battleConfig.unk2 = 0;
    g_battleConfig.unk8 = 0;
    D_8005F170 = 0;

    for (i = 0; i < 3; i++) {
        g_battleConfig.unk4[i] = 0xFF;
    }

    cdReadSync(D_80097800.sector, D_80097800.size, 0x801A0000, 0);
    memcopy((u8 *)0x801A0000, D_80078E00, D_80097800.size);
}

/**
 * @brief Wrapper that calls func_80098028 (field init step).
 */
void func_800980B0(void) {
    func_80098028();
}

/**
 * @brief Initialize memory card events and battle entity slots.
 *
 * Sets up the battle/entity system, initializes the memory card driver,
 * opens and enables 8 PsyQ card events (HwCARD + SwCARD for IOE, ERROR,
 * TIMEOUT, NEW specs), then creates 4x2 battle entity slots.
 */
void func_800980D0(void) {
    s32 i, j;

    setCardFlag(-1);
    func_8004D8C4(0);
    func_8004D930();
    func_800471A4();
    func_8004D844(0);
    func_800472E4();

    g_cardData.events[0] = func_80047204(0xF4000001, 4, 0x2000, 0);
    g_cardData.events[1] = func_80047204(0xF4000001, 0x8000, 0x2000, 0);
    g_cardData.events[2] = func_80047204(0xF4000001, 0x100, 0x2000, 0);
    g_cardData.events[3] = func_80047204(0xF4000001, 0x2000, 0x2000, 0);
    g_cardData.events[4] = func_80047204(0xF0000011, 4, 0x2000, 0);
    g_cardData.events[5] = func_80047204(0xF0000011, 0x8000, 0x2000, 0);
    g_cardData.events[6] = func_80047204(0xF0000011, 0x100, 0x2000, 0);
    g_cardData.events[7] = func_80047204(0xF0000011, 0x2000, 0x2000, 0);

    func_80047244(g_cardData.events[0]);
    func_80047244(g_cardData.events[1]);
    func_80047244(g_cardData.events[2]);
    func_80047244(g_cardData.events[3]);
    func_80047244(g_cardData.events[4]);
    func_80047244(g_cardData.events[5]);
    func_80047244(g_cardData.events[6]);
    func_80047244(g_cardData.events[7]);

    func_800472F4();

    for (j = 0; j < 4; j++) {
        for (i = 0; i < 2; i++) {
            s32 id = packCardId(i, j);
            markCardBusy(id);
            setCardStatusSecondary(id, 0);
        }
    }
}

/**
 * @brief Wrapper that calls func_8004DF84 (memory card initialization).
 */
void func_800982B8(void) {
    func_8004DF84();
}

/**
 * @brief Set memory card event status to "ready" for 4 event slots.
 *
 * Reads the event table from the kernel ECB at 0x100, then writes
 * 0x404 (EvStACTIVE | EvMdNOINTR) to the status field of each of
 * 4 entries within a critical section.
 */
void func_800982D8(void) {
    s32 i;
    EventState *state = getEventState();

    func_800472E4();

    for (i = 0; i < 4; i++) {
        EventEntry *entry = &state->entries[i];
        entry->status = 0x404;
    }

    func_800472F4();
}

/**
 * @brief Decode table data from g_fieldFontGlyphs into D_8008369C.
 *
 * First byte is a start index, second is the limit. Copies pairs of
 * bytes from the source data into the destination buffer until the
 * index exceeds the limit.
 */
void func_80098330(void) {
    u8 *src = g_fieldFontGlyphs;
    u8 *dst = D_8008369C;
    s32 count;
    u8 byte;
    s32 limit;

    src++;
    count = g_fieldFontGlyphs[0];
    limit = *src;
    src++;

    if (limit < count) return;

    do {
        *dst++ = *src++;
        count++;
        *dst++ = (byte = *src++);
    } while (!(limit < count));
}

/** @brief Calls func_80098330 then func_800983B8 in sequence. */
void func_80098390(void) {
    func_80098330();
    func_800983B8();
}

/**
 * @brief Initialize the two animation entities to default state.
 *
 * Sets global display parameters (color intensity=16, blend mode=5),
 * configures the screen clip rect to 320x224, then initializes each
 * entity with default rendering params, full-brightness colors, and
 * zeroed animation state.
 */
void func_800983B8(void) {
    s16 clipRect[4];
    s32 i;
    AnimEntity *entity;
    s32 j;
    AnimData *anim = (AnimData *)&g_battleAnims;

    anim->field_1E0 = 16;
    anim->field_1E1 = 5;
    anim->field_1E3 = 0;

    clipRect[0] = 0;
    clipRect[1] = 0;
    clipRect[2] = 320;
    clipRect[3] = 224;
    setBattleAnimClipRect(clipRect);

    entity = anim->entities;
    i = 0;
    do {
        do { entity++; entity--; } while (0);
        initAnimEntityColor(i);
        entity->field_19 = 1;
        entity->field_10 = 0xFFF;
        entity->field_12 = 0x5000;
        entity->field_14 = 0xA000;
        entity->field_16 = 0x900;

        for (j = 0; j < 2; j++) {
            entity->field_06[j] = 0;
        }

        entity->flags = 0x40;

        for (j = 0; j < 6; j++) {
            ;
        }

        *(volatile u8 *)&entity->field_06[0] = 0;

        for (j = 0; j < 6; j++) {
            entity->colors[j] = 0xFF;
        }

        entity->field_1B = 0;
        entity->field_0B = 0;
        entity->tail.entityType = 0x31;
        setAnimGlobalCoords(i, 0, 0);
        entity->field_0C = 0;
        entity->field_0D = 0;
        entity->field_0E = 0;
        entity->field_0F = 0;
        entity->tail.entityId = i++;
        entity++;
    } while (i < 2);
}
