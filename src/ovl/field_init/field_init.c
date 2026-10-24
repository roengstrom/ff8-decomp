#include "common.h"
#include "cd.h"
#include "battle.h"
#include "main.h"
#include "gf.h"
#include "field_init.h"
#include "field_init_font.h"
#include "psxsdk/libapi.h"

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

/* --- Externs (sorted by address) --- */

extern BattleConfig g_battleConfig;
extern CardDataBlock g_cardData;
extern u8 D_8008369C[];

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
 * @brief Reset the battle config and load kernel.bin into @c g_kernel.
 *
 * Clears the battle config and fills the 3 command slots with 0xFF, then
 * reads kernel.bin from disc into a scratch buffer and copies it to
 * @c g_kernel.
 */
void loadKernel(void) {
    s32 i;

    g_battleConfig.battleSceneId = 0;
    g_battleConfig.unk2 = 0;
    g_battleConfig.sealedAbilities = 0;
    D_8005F170 = 0;

    for (i = 0; i < 3; i++) {
        g_battleConfig.unk4[i] = 0xFF;
    }

    cdReadSync(g_kernelFileDesc.sector, g_kernelFileDesc.size, 0x801A0000, 0);
    memcopy((u8 *)0x801A0000, &g_kernel, g_kernelFileDesc.size);
}

/**
 * @brief Wrapper that calls loadKernel.
 */
void func_800980B0(void) {
    loadKernel();
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
    _bu_init();
    _card_auto(0);
    EnterCriticalSection();

    g_cardData.events[0] = OpenEvent(0xF4000001, 4, 0x2000, 0);
    g_cardData.events[1] = OpenEvent(0xF4000001, 0x8000, 0x2000, 0);
    g_cardData.events[2] = OpenEvent(0xF4000001, 0x100, 0x2000, 0);
    g_cardData.events[3] = OpenEvent(0xF4000001, 0x2000, 0x2000, 0);
    g_cardData.events[4] = OpenEvent(0xF0000011, 4, 0x2000, 0);
    g_cardData.events[5] = OpenEvent(0xF0000011, 0x8000, 0x2000, 0);
    g_cardData.events[6] = OpenEvent(0xF0000011, 0x100, 0x2000, 0);
    g_cardData.events[7] = OpenEvent(0xF0000011, 0x2000, 0x2000, 0);

    EnableEvent(g_cardData.events[0]);
    EnableEvent(g_cardData.events[1]);
    EnableEvent(g_cardData.events[2]);
    EnableEvent(g_cardData.events[3]);
    EnableEvent(g_cardData.events[4]);
    EnableEvent(g_cardData.events[5]);
    EnableEvent(g_cardData.events[6]);
    EnableEvent(g_cardData.events[7]);

    ExitCriticalSection();

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

    EnterCriticalSection();

    for (i = 0; i < 4; i++) {
        EventEntry *entry = &state->entries[i];
        entry->status = 0x404;
    }

    ExitCriticalSection();
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
 * @brief Reset g_engine's two entities, the pad auto-repeat delays and the clip rect.
 */
void func_800983B8(void) {
    s16 clipRect[4];
    s32 i;
    PadPort *port;
    s32 j;
    EngineState *engine = &g_engine;

    engine->repeatDelays.b.lo = 16;
    engine->repeatDelays.b.hi = 5;
    engine->animFlag = 0;

    clipRect[0] = 0;
    clipRect[1] = 0;
    clipRect[2] = 320;
    clipRect[3] = 224;
    setBattleAnimClipRect(clipRect);

    port = engine->controllerPorts;
    i = 0;
    do {
        initPadInput(i);
        port->field19 = 1;
        port->unk10[0] = 0xFFF;
        port->unk10[1] = 0x5000;
        port->unk10[2] = 0xA000;
        port->unk10[3] = 0x900;

        for (j = 0; j < 2; j++) {
            port->motor[j] = 0;
        }

        port->controllerTransmitBuf[0] = 0x40;

        /* The retail code has this loop, with an empty body. */
        for (j = 0; j < 6; j++) {
            ;
        }

        /* volatile keeps this store on port's register, not the loop's port + 0xC2 one. */
        *(volatile u8 *)&port->motor[0] = 0;

        for (j = 0; j < 6; j++) {
            port->fieldBC[j] = 0xFF;
        }

        port->vibrationMask = 0;
        port->field0B = 0;
        port->fieldC3 = 0x31;
        setAnimGlobalCoords(i, 0, 0);
        port->field0C[0] = 0;
        port->field0C[1] = 0;
        port->field0C[2] = 0;
        port->field0C[3] = 0;
        port->linkedIdx = i++;
        port++;
    } while (i < 2);
}
