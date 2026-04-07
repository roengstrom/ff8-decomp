#include "common.h"
#include "cd.h"
#include "battle.h"
#include "field_init_font.h"

extern s8 D_8005F170;
extern u8 D_80078E00[];
extern u8 D_80082C08[];
extern CardDataBlock g_cardData;
extern u8 D_8008369C[];
extern CdFileDesc D_80097800;

/** @brief Field initialization entry: call setup then init step. */
void func_80098000(void) {
    func_8001F5C8();
    func_800980B0();
}

/**
 * @brief Initialize battle command state and load battle data from CD.
 *
 * Clears the battle command descriptor at D_80082C08, resets the
 * D_8005F170 flag, fills 3 process-state bytes with 0xFF, then
 * reads battle data from disc (D_80097800) into a scratch buffer
 * and copies it to the battle data buffer (D_80078E00).
 */
void func_80098028(void) {
    s32 fillVal = 0xFF;
    s32 i = 2;
    u8 *base = D_80082C08;
    u8 *ptr = base + 2;

    *(u16 *)D_80082C08 = 0;
    *(u16 *)(base + 2) = 0;
    *(u8 *)(base + 8) = 0;
    D_8005F170 = 0;

    do {
        *(u8 *)(ptr + 4) = fillVal;
        i--;
        ptr--;
    } while (i >= 0);

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
    s32 *events;
    s32 i;
    s32 j;

    setCardFlag(-1);
    func_8004D8C4(0);
    func_8004D930();
    events = (s32 *)&g_cardData;
    func_800471A4();
    func_8004D844(0);
    j = 0;
    func_800472E4(); /* EnterCriticalSection */

    events[0] = func_80047204(0xF4000001, 4, 0x2000, 0);
    events[1] = func_80047204(0xF4000001, 0x8000, 0x2000, 0);
    events[2] = func_80047204(0xF4000001, 0x100, 0x2000, 0);
    events[3] = func_80047204(0xF4000001, 0x2000, 0x2000, 0);
    events[4] = func_80047204(0xF0000011, 4, 0x2000, 0);
    events[5] = func_80047204(0xF0000011, 0x8000, 0x2000, 0);
    events[6] = func_80047204(0xF0000011, 0x100, 0x2000, 0);
    events[7] = func_80047204(0xF0000011, 0x2000, 0x2000, 0);

    func_80047244(events[0]);
    func_80047244(events[1]);
    func_80047244(events[2]);
    func_80047244(events[3]);
    func_80047244(events[4]);
    func_80047244(events[5]);
    func_80047244(events[6]);
    func_80047244(events[7]);

    func_800472F4(); /* ExitCriticalSection */

    do {
        i = 0;
        do {
            s32 entity = packCardId(i, j);
            markCardBusy(entity);
            setCardStatusSecondary(entity, 0);
        } while (++i < 2);
        i = 0;
    } while (++j < 4);
}

/**
 * @brief Wrapper that calls func_8004DF84 (memory card initialization).
 */
void func_800982B8(void) {
    func_8004DF84();
}

/**
 * @brief Set memory card event status to "ready" (0x404) for 4 card slots.
 *
 * Reads the event table base from ECB+0x10, then writes 0x404 (EvStACTIVE |
 * EvMdNOINTR) to the status field (+0x94) of each of 4 card event entries
 * (stride 0xC0) within a critical section.
 */
void func_800982D8(void) {
    s32 ecb_base = 0x100;

    func_800472E4(); /* EnterCriticalSection */

    {
        s32 i = 0;
        s32 val = 0x404;
        s32 offset = i;
        s32 base;

        top:
        base = *(s32 *)(ecb_base + 0x10);
        i++;
        *(s32 *)(base + offset + 0x94) = val;
        offset += 0xC0;
        if (i < 4) goto top;
    }

    func_800472F4(); /* ExitCriticalSection */
}

/**
 * @brief Decode table data from D_80098960 into D_8008369C.
 *
 * First byte is a start index, second is the limit. Copies pairs of
 * bytes from the source data into the destination buffer until the
 * index exceeds the limit.
 */
void func_80098330(void) {
    u8 *src = D_80098960;
    u8 *dst = D_8008369C;
    s32 count;
    u8 byte;
    s32 limit;

    src++;
    count = D_80098960[0];
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

/** @brief Top-level animation data block (D_80082DD0). */
typedef struct {
    AnimEntity entities[2];
    u8 pad188[0x58];
    u8 field_1E0;
    u8 field_1E1;
    u8 pad1E2;
    u8 field_1E3;
} AnimData;

extern AnimData D_80082DD0;

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
    AnimEntity *entity;
    s32 i, j;

    D_80082DD0.field_1E0 = 16;
    D_80082DD0.field_1E1 = 5;
    D_80082DD0.field_1E3 = 0;

    clipRect[0] = 0;
    clipRect[1] = 0;
    clipRect[2] = 320;
    clipRect[3] = 224;
    setBattleAnimClipRect(clipRect);

    entity = D_80082DD0.entities;
    i = 0;

    do {
        do { entity++; entity--; } while (0); /* Regalloc: boost entity priority */
        initAnimEntityColor(i);

        entity->field_19 = 1;
        {
            s16 *p = &entity->field_10;
            p[0] = 0xFFF;
            p[1] = 0x5000;
            *(u16 *)&p[2] = 0xA000;
            p[3] = 0x900;
        }

        for (j = 0; j < 2; j++) {
            entity->field_06[j] = 0;
        }

        entity->flags = 0x40;

        for (j = 0; j < 6; j++) {
            /* empty spin */
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
