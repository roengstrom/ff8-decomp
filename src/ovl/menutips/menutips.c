#include "common.h"

/**
 * @brief Render a tips panel entry.
 *
 * Calls func_801F08D4 with mode 1, section 0xD, the provided context,
 * and flag 0.
 *
 * @param a0 Render context pointer.
 */
s32 func_801E5800(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 0);
}

/**
 * @brief Push a value pair into the tips history ring buffer.
 *
 * If the buffer is full (D_801ED630 >= 0x7F), shifts all entries left
 * by one (dropping the oldest) and decrements the count. Then appends
 * (arg0, arg1) at the current end of the buffer and increments the count.
 *
 * @param arg0 First value of the pair to push.
 * @param arg1 Second value of the pair to push.
 */
void func_801E582C(s32 arg0, s32 arg1) {
    extern u8 D_801ED430[];
    extern s16 D_801ED630;
    s32 i;
    s32 new_var;
    s32 next;
    s32 base;
    u16 *new_var2;
    u8 *basePtr;
    s16 *countPtr;
    s16 sv;
    u16 uv;

    if (D_801ED630 >= 0x7F) {
        D_801ED630 = D_801ED630 - 1;
        i = 0;
        base = (s32)D_801ED430;
        do {
            new_var = i * 4;
            new_var = base + new_var;
            next = i + 1;
            *((u16 *)((base + (i * 4)) + 0)) = *((u16 *)((base + (next * 4)) + 0));
            new_var2 = (u16 *)((base + (next * 4)) + 2);
            *((u16 *)(new_var + 2)) = *new_var2;
            i = next;
        } while (i < 0x7F);
    }
    basePtr = D_801ED430;
    countPtr = &D_801ED630;
    sv = *countPtr;
    uv = *((u16 *)countPtr);
    *((u16 *)((((s32)basePtr) + (sv * 4)) + 0)) = (u16)arg0;
    *((u16 *)((((s32)basePtr) + (sv * 4)) + 2)) = (u16)arg1;
    *countPtr = uv + 1;
}

/**
 * @brief Pop the last value pair from the tips history ring buffer.
 *
 * Decrements the counter in D_801ED630 and returns the entry at the
 * new position as a packed s32 (lo | hi << 16). Returns -1 if the
 * buffer is empty (counter <= 0).
 *
 * @return Packed value pair, or -1 if empty.
 */
s32 func_801E58C0(void) {
    extern u8 D_801ED630[];
    extern u8 D_801ED430[];
    s16 sv;
    u16 uv;
    s32 off;
    s32 base;
    s16 *entry;
    s32 lo;
    s32 hi;

    sv = *(s16 *)D_801ED630;
    uv = *(u16 *)D_801ED630;
    if (sv <= 0) {
        return -1;
    }
    uv--;
    base = (s32)D_801ED430;
    off = (s16)uv * 4;
    entry = (s16 *)(base + off);
    lo = *(s16 *)entry;
    hi = *(s16 *)(entry + 1);
    *(u16 *)D_801ED630 = uv;
    return lo | (hi << 16);
}

/**
 * @brief Render a tips entry at a table-derived position.
 *
 * Checks D_801EC410; if positive, looks up coordinates from
 * D_801EC310[a1] (8-byte stride), applies offsets (+0x22, +0x23),
 * and calls func_801F0A34 to render.
 *
 * @param a0 Render context pointer.
 * @param a1 Index into the coordinate table.
 */
/**
 * @brief Render a tips entry at a table-derived position.
 *
 * Checks D_801EC410; if positive, looks up coordinates from
 * D_801EC310[a1] (8-byte stride), applies offsets (+0x22, +0x23),
 * and calls func_801F0A34 to render.
 *
 * @param a0 Render context pointer.
 * @param a1 Index into the coordinate table.
 */
void func_801E590C(s32 a0, s32 a1) {
    extern s16 D_801EC410;
    extern u8 D_801EC310[];
    s32 base;
    int new_var3;
    s16 new_var2;
    s32 entry;
    s16 *new_var;

    if (D_801EC410 > 0) {
        base = (s32)D_801EC310;
        new_var3 = base + (a1 * 8);
        entry = new_var3;
        base = 0;
        new_var = (s16 *)(entry + 0);
        new_var3 = entry + 2;
        new_var2 = *new_var;
        func_801F0A34(a0, base, new_var2 + 0x22, (*((s16 *)new_var3)) + 0x23);
    }
}

/**
 * @brief Look up a tips data pointer from the entry table.
 *
 * Indexes into D_801EC420 by a0 (4-byte stride), reads the halfword
 * at offset +4, and returns it added to 0x801D1000.
 *
 * @param a0 Tips entry index.
 * @return Pointer to tips data (as s32).
 */
s32 func_801E5958(s32 a0) {
    extern u8 D_801EC420[];
    s32 base = (s32)D_801EC420;
    return *(u16 *)(base + a0 * 4 + 4) + 0x801D1000;
}

/**
 * @brief Initialize tips page data and render entry text.
 *
 * Calls func_801E5958 to get a tips data buffer. Copies the first three
 * halfwords from the buffer header into D_801EC412-D_801EC416, clears
 * D_801EC410, then calls copyString to set up the tips table, measures
 * the text length with btlStrlen, and finalizes via func_801E6018.
 * Stores the result in D_801EC410.
 *
 * @param a0 Tips entry index.
 */
typedef struct {
    u16 unk0;
    u16 unk2;
    u16 unk4;
    u16 unk6;
} TipsBufHeader;

void func_801E597C(s32 a0) {
    extern s16 D_801EC410;
    extern u16 D_801EC412;
    extern u16 D_801EC414;
    extern u16 D_801EC416;
    extern u8 D_801E7B10[];
    extern u8 D_801E8310[];
    extern u8 D_801EC310[];
    TipsBufHeader *ret;
    s32 buf;

    buf = (s32)(ret = (TipsBufHeader *)func_801E5958(a0));
    D_801EC410 = 0;
    D_801EC412 = ret->unk0;
    D_801EC414 = ret->unk2;
    D_801EC416 = ret->unk4;
    buf += 8;
    copyString((s32)D_801E7B10, buf);
    buf += btlStrlen(buf);
    D_801EC410 = func_801E6018(buf + 1, (s32)D_801E8310, (s32)D_801EC310);
}

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E5A10);

/**
 * Number display format parameters, returned by getMenuString.
 * Controls how numeric values are formatted into strings.
 */
typedef struct {
    u8 pad_char; /**< Pad/fill character (leading fill and skip sentinel). */
    u8 fmt_char; /**< Format specifier passed to the number-to-string converter. */
} NumFmtParams;

/**
 * View of CharacterData kills/kos fields (stride 0x98 = sizeof CharacterData).
 * Array base is at g_gameState + 0x520 = g_characters[0] + 0x90.
 * Accesses CharacterData.kills and CharacterData.kos for each character.
 */
typedef struct {
    u16 kills;           /**< Character kill count (CharacterData + 0x90). */
    u16 kos;             /**< Character KO count (CharacterData + 0x92). */
    u8 pad4[0x94];
} CharKillView; /* stride 0x98 = sizeof CharacterData */

/**
 * View of GfSaveData kills/kos fields (stride 0x44 = sizeof GfSaveData).
 * Array base is at g_gameState + 0x8C = g_gameState[GF_COUNT] + 0x3C.
 * Accesses GfSaveData.kills and GfSaveData.kos for each GF.
 */
typedef struct {
    u16 kills;           /**< GF kill count (GfSaveData + 0x3C). */
    u16 kos;             /**< GF KO count (GfSaveData + 0x3E). */
    u8 pad4[0x40];
} GfKillView; /* stride 0x44 = sizeof GfSaveData */

/**
 * Text position marker written for each 0xB opcode in the encoded stream.
 * Used by callers to place annotations (ruby text, etc.) at the right position.
 */
typedef struct {
    s16 x_width;   /**< Accumulated character pixel width at this marker. */
    u16 y_line;    /**< Line y-offset (incremented by 0x10 per line break). */
    s16 char_ofs;  /**< Character-relative pixel offset: (byte0 << 7) - 0x1020 + byte1. */
    s16 pad;
} TextMarker;

/**
 * View over g_gameState (g_gameState) exposing the fields used by tips text
 * variable substitution: GF kill/KO tables, character kill/KO tables, and
 * misc battle counters.
 */
typedef struct {
    char pad00[0x8C];
    GfKillView   gfKills[16];      /**< GF kill/KO counts (GfSaveData.kills/kos x 16). */
    char pad4CC[0x520 - (0x8C + 0x44 * 16)];
    CharKillView charKills[8];     /**< Character kill/KO counts (CharacterData.kills/kos x 8). */
    char pad9E0[0xCDC - (0x520 + 0x98 * 8)];
    s32 miscCount;                 /**< Misc battle counter (g_gameState + 0xCDC). */
    u16 miscCount1;                /**< Misc counter 1 (g_gameState + 0xCE0). */
    u16 miscCount2;                /**< Misc counter 2 (g_gameState + 0xCE2). */
    char padCE4[0xD23 - 0xCE4];
    u8 varFlags[1];                /**< Variable visibility bitfield (g_gameState + 0xD23). */
} GameStatsView;

/**
 * @brief Decode an encoded tips text segment into a flat byte string.
 *
 * Walks the encoded text stream for @p text_id, expanding variable-substitution
 * opcodes (0xA) into formatted numeric strings from game stats, copying literal
 * character bytes to @p out, and recording a TextMarker for each position-marker
 * opcode (0xB) encountered.
 *
 * @note NON-MATCHING (hand-assembled): The original binary contains two
 *       `addi $gp, $gp, ±0x80` instructions (opcode 0x08) that adjust the
 *       scratchpad pointer held in $gp. PsyQ CC1PSX always emits `addiu`
 *       (opcode 0x09, no overflow trap) for arithmetic — `addi` is never
 *       generated by the compiler. These instructions were inserted by hand
 *       after compilation. See improvement.c for the fully documented C
 *       reference implementation.
 *
 * Opcode summary:
 *   0x00, 0x01, 0x07 - end of stream, stop.
 *   0x02             - line break: reset x_width, advance y_line by 0x10, restart segment.
 *   0x0A param       - variable: set show flag (0x3F=always show) or substitute a game stat.
 *   0x0B b0 b1       - marker: record TextMarker at current position.
 *   0x05 byte        - glyph with advance: copy byte, accumulate width.
 *   0x10-0x1F byte   - two-byte glyph: copy both bytes, accumulate width.
 *   0x19-0x1F byte   - extended two-byte glyph with 0x400 flag.
 *   0x20+            - single-byte glyph: accumulate width.
 *
 * @param text_id      Handle identifying the encoded text to decode.
 * @param out          Output buffer receiving decoded bytes (null-terminated).
 * @param markers      Array of TextMarker structs populated with one entry per 0xB opcode.
 * @return             Number of TextMarker entries written into @p markers.
 */
INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E6018);

/**
 * @brief Check tutorial availability and save current selection.
 *
 * Calls pollCdReadStatus to check if the tips system is ready. If not ready
 * (returns 0), copies D_801ED422 to D_801ED420 (saves current selection)
 * and returns 1. Otherwise returns 0.
 *
 * @return 1 if tips system was not ready (selection saved), 0 otherwise.
 */
s32 func_801E6474(void) {
    extern u16 D_801ED422;
    extern u16 D_801ED420;
    if (pollCdReadStatus() == 0) {
        D_801ED420 = D_801ED422;
        return 1;
    }
    return 0;
}

/**
 * @brief Load tips page data if the current selection has changed.
 *
 * Indexes into D_801EC420 to find the entry for the given page index.
 * If the entry's ID differs from D_801ED420, calls loadSubOverlay to
 * load the new page data from 0x801D1000, then updates D_801ED422 with
 * the new ID.
 *
 * @param a0 Page index into the tips entry table.
 */
void func_801E64B0(s32 a0) {
    extern u8 D_801EC420[];
    extern s16 D_801ED420;
    extern u16 D_801ED422;
    s32 base = (s32)D_801EC420;
    u16 *entry;
    u16 id;
    a0 = a0 * 4 + 4;
    entry = (u16 *)(a0 + base);
    id = entry[1];
    if (D_801ED420 != id) {
        loadSubOverlay(id + 0x80, 0x801D1000);
        D_801ED422 = entry[1];
    }
}

/**
 * @brief Render tips navigation panel with scroll indicators.
 *
 * @param a0 Display list pointer.
 * @param a1 OT pointer.
 * @return Updated OT pointer from func_801EF9AC.
 */
s32 func_801E6514(s32 a0, s32 a1) {
    extern u8 g_menuDisplayCfg[];
    extern s16 D_801EC410;
    extern u16 D_801EC414;
    extern u16 D_801EC416;
    extern s32 g_menuColor;
    s32 ot = a1;
    s32 resource;
    s32 tw;
    s32 width;
    s32 xPos = 0x18;
    s32 yPos;
    u8 *cfg = g_menuDisplayCfg;
    yPos = 0xC4;
    if (D_801EC410 != 0) {
        resource = func_801E5800(0xE);
    } else {
        resource = func_801E5800(0xF);
        if ((D_801EC416 != 0xFFFF) || (D_801EC414 != D_801EC416)) {
            resource = func_801E5800(0x20);
        }
    }
    if (D_801EC416 != 0xFFFF) {
        s32 arg = 0x1F;
        if (D_801EC410 == 0) {
            arg = 0x1D;
        }
        resource = func_801E5800(arg);
    }
    tw = func_8002E680(resource);
    tw = (0x150 - (u16)tw) / 2;
    func_8002EAD0(a0, xPos + tw, yPos + 4, resource);
    width = 0x150;
    cfg[0x10] = 0;
    cfg[0x11] = 0;
    *(s16 *)(cfg + 0x0) = xPos;
    *(s16 *)(cfg + 0x2) = yPos;
    *(s16 *)(cfg + 0x4) = width;
    *(s16 *)(cfg + 0x6) = 0x14;
    return func_801EF9AC(a0, ot, 0x1000, g_menuColor);
}

typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    u8 pad8[8];
    u8 unk10;
    u8 unk11;
} WorkData;

/**
 * @brief Render the tips content panel with scroll state.
 *
 * Saves the current palette context, renders the tips text region via
 * func_8002EAD0, restores the palette, then configures the g_menuDisplayCfg
 * display panel and calls func_801F5F60 (if either scroll indicator is
 * active) followed by func_801EF9AC to draw the panel.
 *
 * @param arg0 Display list pointer.
 * @param arg1 OT pointer.
 * @return Updated OT pointer from func_801EF9AC.
 */
s32 func_801E6668(s32 arg0, s32 arg1) {
    extern WorkData g_menuDisplayCfg;
    extern s8 D_801E8310;
    extern u16 D_801EC414;
    extern u16 D_801EC416;
    extern s32 g_menuColor;
    s32 temp_s0;
    s32 var_a3;
    s32 var_s1;

    var_s1 = arg1;
    temp_s0 = getDisplayListHead();
    storeGpuPacket(var_s1);
    func_8002EAD0(arg0, 0x22, 0x23, &D_801E8310);
    var_s1 = getDisplayListHead();
    storeGpuPacket(temp_s0);
    g_menuDisplayCfg.unk10 = 0;
    g_menuDisplayCfg.unk11 = 0;
    g_menuDisplayCfg.unk0 = 0x18;
    g_menuDisplayCfg.unk2 = 0x1D;
    g_menuDisplayCfg.unk4 = 0x150;
    g_menuDisplayCfg.unk6 = 0xA7;
    var_a3 = D_801EC414 != 0xFFFF;
    if (D_801EC416 != 0xFFFF) {
        var_a3 |= 2;
    }
    if (var_a3 != 0) {
        var_s1 = func_801F5F60(arg0, var_s1, g_menuColor, var_a3);
    }
    return func_801EF9AC(arg0, var_s1, 0x1000, g_menuColor);
}

/**
 * @brief Render tips header panel.
 *
 * Calls func_8002EAD0 to set up the display region using the D_801E7B10
 * data table, then configures g_menuDisplayCfg with fixed panel dimensions
 * and calls func_801EF9AC to render.
 *
 * @param a0 Display list pointer.
 * @param a1 OT pointer.
 * @return Updated OT pointer from func_801EF9AC.
 */
s32 func_801E6768(s32 a0, s32 a1) {
    extern u8 D_801E7B10[];
    extern u8 g_menuDisplayCfg[];
    extern s32 g_menuColor;
    u8 *new_var;
    s32 ot;
    s32 disp;
    s32 cfg;
    if (1) {
        disp = a0;
        ot = a1;
        new_var = D_801E7B10;
        func_8002EAD0(disp, 0x24, 0xC, (((s32)new_var) + ot) - ot);
        cfg = (s32)g_menuDisplayCfg;
        *(u8 *)(cfg + 0x10) = 0;
        *(u8 *)(cfg + 0x11) = 0;
        *(s16 *)&g_menuDisplayCfg[0] = 0x18;
        *(s16 *)(cfg + 2) = 6;
    }
    *(s16 *)(cfg + 4) = 0xF4;
    *(s16 *)(cfg + 6) = 0x16;
    return func_801EF9AC(disp, ot, 0x1000, g_menuColor);
}

/**
 * @brief Render tips display pipeline.
 *
 * If the tips state has an active display (field 0x2A non-zero),
 * sets up rendering context, draws the header, content, and navigation
 * panels in sequence.
 *
 * @param a0 Tips state pointer.
 * @param a1 Display list pointer.
 * @param a2 OT pointer.
 * @return Updated OT pointer, or a2 if no active display.
 */
s32 func_801E67F4(s32 a0, s32 a1, s32 a2) {
    s32 state = a0;
    s32 disp = a1;
    s32 ot = a2;

    if (*(s16 *)(state + 0x2A) != 0) {
        func_801F1AFC();
        setMenuColorIntensity(*(s16 *)(state + 0x24));
        buildGrayscaleGpuColor(*(s16 *)(state + 0x24));
        ot = func_801E6768(disp, ot);
        ot = func_801E6668(disp, ot);
        ot = func_801E6514(disp, ot);
        func_801F1B10();
    }
    return ot;
}

typedef struct {
    u8 id;
    u8 valueA;
    u8 valueB;
    u8 pad;
} TipsEntry;

typedef struct {
    u8 flags;
    u8 unk[0x2C];
    u8 value2D;
} TipsState;

extern TipsEntry D_801E6AC0[];

/**
 * @brief Scan tips entry table and invoke unlock/locked callbacks, then check state flags.
 *
 * Iterates D_801E6AC0 until an entry with id 0xFF is found. For each entry,
 * checks bit (id - 0x5C) in the flags returned by func_801F72B4(). If set,
 * calls clearFieldFlag(valueA) and uses valueB as the arg to setFieldFlag;
 * otherwise calls clearFieldFlag(valueB) and uses valueA. After the loop,
 * calls getChocoboWorldPtr() to get a state pointer, and if its flags field has
 * bit 0 set, returns value2D.
 *
 * @return value2D from the state struct if flags bit 0 is set; undefined otherwise.
 */
u8 func_801E688C(void) {
    s32 temp_s4;
    u8 temp_s1;
    u8 temp_s2;
    int temp_v0;
    u8 var_a0;
    TipsState *temp_v0_2;
    TipsEntry *var_s3;

    var_s3 = D_801E6AC0;
    temp_s4 = func_801F72B4();
    while (1) {
        temp_v0 = (*var_s3).id;
        temp_s2 = var_s3->valueA;
        temp_s1 = var_s3->valueB;
        if (0xFF == temp_v0) {
            break;
        }
        if (((unsigned long)temp_s4) & (1 << (temp_v0 - 0x5C))) {
            clearFieldFlag(temp_s2);
            var_a0 = temp_s1;
        } else {
            clearFieldFlag(temp_s1);
            var_a0 = temp_s2;
        }
        setFieldFlag(var_a0);
        var_s3++;
    }

    temp_v0_2 = getChocoboWorldPtr();
    if (temp_v0_2->flags & 1) {
        return temp_v0_2->value2D;
    }
}

extern void *func_801E5A10(void *);

typedef struct {
    u8 pad00[0x24];
    s16 unk24;
    s16 unk26;
    s16 unk28;
    s16 unk2A;
} WorkStruct;

/**
 * @brief Initialize the tips overlay state and kick off the work loop.
 *
 * Resets the selection trackers, waits for pollCdReadStatus to clear, sets up
 * audio/display, allocates a WorkStruct via func_801F179C, zeroes the
 * D_801E6B10 name buffer, runs the entry-scan, then initialises all overlay
 * state variables and starts the main work function.
 */
void func_801E696C(void) {
    extern s8 D_801E6B0C;
    extern s8 D_801E6B10[];
    extern s8 D_801E7B10;
    extern s8 D_801E8310;
    extern s16 D_801EC410;
    extern u8 D_801EC420;
    extern s16 D_801ED420;
    extern s16 D_801ED422;
    extern s16 D_801ED630;
    WorkStruct *work;
    s32 i;

    D_801ED420 = -1;
    D_801ED422 = -1;
    while (pollCdReadStatus() != 0) {
        ;
    }

    func_80048C50(0);
    func_80042634(0);
    work = (WorkStruct *)func_801F179C(func_801E5A10, &func_801E67F4);
    for (i = 0; i < 0x1000; i++) {
        D_801E6B10[i] = 0;
    }

    func_801E688C();
    func_801F0948(0);
    if (work == 0) {
        return;
    }
    D_801E7B10 = 0;
    D_801E8310 = 0;
    loadSubOverlay(0x7F, &D_801EC420);
    work->unk24 = (work->unk28 = 0);
    work->unk26 = 0;
    work->unk2A = 0;
    D_801EC410 = 0;
    func_801F1D34(&D_801E6B0C);
    func_801F1DB0(0);
    func_801E5A10(work);
    D_801ED630 = 0;
}
