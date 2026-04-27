#include "common.h"
#include "menu.h"
#include "gamestate.h"

/** 48-byte ability-menu slot; only @c angle at @c +0x2E is read here. */
typedef struct {
    u8  pad00[0x2E];
    s16 angle;
} MenuSlot;

/** 8-byte ability-menu entry record; the byte at @c +5 is an id/flag
 *  (0xFF = empty, 0x80/0x81 = state-specific — see @c func_801E36AC). */
typedef struct {
    u8 data[8];
} AbilityEntry;

/**
 * @brief Context passed to the ability-list panel configurators.
 *
 * Layout pieced together from @c func_801E3630 (uses @c items as dataPtr)
 * and @c func_801E381C (uses the whole ctx as dataPtr, plus @c pageStart
 * / @c pageEnd for scroll bookkeeping).
 */
typedef struct {
    u8  pad00[0x20];
    u8  items[0x12];   /**< 0x20: item table (strings/pointers, passed as cfg.dataPtr). */
    u16 scrollOff;     /**< 0x32: scroll offset source (copied to cfg.scrollOffset). */
    u8  pad34[2];
    u8  pageStart;     /**< 0x36 */
    u8  pageEnd;       /**< 0x37 */
} AbilityListCtx;

/**
 * @brief Ability menu state struct registered via @c func_801F179C.
 */
typedef struct {
    u8  pad00[0x20];
    s32 dataPtr;       /**< 0x20: cleared at init, filled by state machine. */
    u8  pad24[4];
    s32 abilityMask;   /**< 0x28: result of @c func_801F72B4. */
    s16 state;         /**< 0x2C */
    s16 displaySize;   /**< 0x2E: fixed to 0x1000 at init. */
} AbilityMenuState;

/**
 * @brief Sound/music selection menu state used by @c func_801E2A34.
 *
 * 26-state machine handling track navigation, audio playback, and
 * page transitions. field_3A is the primary grid index, field_3B the
 * secondary; both are computed via div/mod by 11 throughout.
 */
typedef struct {
    u8  pad_00[0x10];
    u16 state;        /**< 0x10: state machine state (0-25). */
    u8  pad_12[0xE];
    s32 field_20;     /**< 0x20 */
    s32 field_24;     /**< 0x24 */
    u8  pad_28[0x4];
    s16 field_2C;     /**< 0x2C: fade scale (0..0x1000). */
    s16 field_2E;     /**< 0x2E: fade scale (secondary). */
    s16 field_30;     /**< 0x30: countdown timer. */
    s16 field_32;     /**< 0x32: page-scroll accumulator. */
    s16 field_34;     /**< 0x34: secondary scroll accumulator. */
    u8  field_36;     /**< 0x36: current page (primary). */
    u8  field_37;     /**< 0x37: previous page (primary). */
    u8  field_38;     /**< 0x38: current page (secondary). */
    u8  field_39;     /**< 0x39: previous page (secondary). */
    u8  field_3A;     /**< 0x3A: grid index (primary). */
    u8  field_3B;     /**< 0x3B: grid index (secondary). */
    u8  field_3C;     /**< 0x3C: mode/sub-state. */
    u8  field_3D;     /**< 0x3D: last track id. */
} SoundMenuState;


extern u8            D_801E3D84[];
extern u8            D_801E3D9C;
extern u8            D_801E3DA4[];
extern u8            D_801E3DB8;
extern u8            g_menuDisplayCfg[];
extern s32           g_menuColor;
extern u16           D_801FA3C8[];
extern AbilityEntry  D_8007CEE0[];

extern void decodeMessage(u8 *src, u8 *dst, s32 mode);
extern s32  getAbilityDesc(s32 id);
extern s32  func_801EF9AC(s32 dl, s32 ot, s32 opaque, s32 color);
extern void func_801EFBB4(s32 dl, s32 ot, s32 callback);
extern void func_801F0A78(s32 ctx, s32 idx, s32 unused, s32 x, s32 y);
extern s32  func_801F0FEC(s32 ctx, s32 state, s32 x, s32 y, u8 *buf, s32 mode);
extern s32  func_801F179C(s32 tickCb, s32 drawCb);
extern s32  func_801F5F30(s32 dl, s32 ot, s32 x, s32 y, s32 color, s32 pageStart);
extern s32  func_801F5F60(s32 dl, s32 ot, s32 color, s32 arrows);
extern s32  func_801F72B4(void);

extern void func_801E2990(void);
extern void func_801E2A34(SoundMenuState *s);
extern void func_801E36AC();
extern void func_801E3AE0();

extern u8 D_8007809A;

extern s32  func_801F6768(u16 flags, s32 max, s32 current);
extern void func_801EFFE4(s32 trackId);
extern void func_801F0BF8(s32 mode);
extern void func_801F0C5C(u8 mode, void *ctx);
extern s32  func_801F0D84(void);
extern void func_801F18FC(s32 *ctx);
extern s32  func_801F0BB0(void);
extern void func_801F7BEC(s32 cfg);
extern void func_801F23D0(s32 a0, s32 size, void *buf);
extern void *func_801F6AA4(s32 id);
extern void *func_801F6AFC(s32 id);
extern void initSfxPlayback(s32 ch, u8 *buf);
extern void sendSpuCommand(s32 cmd);
extern void setSfxPitch(s32 ch, s32 pitch);
extern void startSfxNormal(s32 ch);
extern void fadeOutSfxFast(s32 ch);

/**
 * @brief Render one cell of an ability grid at a per-slot X offset.
 *
 * Computes the cell's screen Y from its list index (row = @p i % 11;
 * @c y = row*13 + 0x42). Reads @p slot->angle, divides by 64 to index
 * @c D_801FA3C8 for a half-width, scales that by @c 240/4096 into MDC pixel
 * space (here written as @c 120*(w*2) to match the compiler's preferred
 * strength-reduction order), and offsets by @c 0xAD to get the final X.
 * Dispatches the cell to @c func_801F0A78 with @c (ctx, 0, unused, x, y).
 *
 * @param ctx    Rendering context, forwarded.
 * @param i      Zero-based list index; row is @c i%11.
 * @param unused Forwarded to @c func_801F0A78 untouched.
 * @param slot   Source object; @c slot->angle drives the X shift.
 */
void func_801E2800(s32 ctx, s32 i, s32 unused, MenuSlot *slot) {
    s32 lookup;
    s32 tableIdx;
    s32 y = (i % 11) * 13 + 0x42;

    lookup   = slot->angle;
    tableIdx = lookup / 64;
    lookup   = D_801FA3C8[tableIdx];
    func_801F0A78(ctx, 0, unused, 120 * (lookup * 2) / 4096 + 0xAD, y);
}

/**
 * @brief Render ability entry label at computed grid position (simple variant).
 *
 * Divides index by 11 to get remainder, computes y = remainder * 13 + 0x42,
 * then calls func_801F0A78 to draw.
 *
 * @param a0 Display context pointer.
 * @param a1 Ability list index.
 * @param a2 Unused.
 */
void func_801E28B4(s32 a0, s32 a1, s32 a2) {
    s32 col = a1 / 11;
    s32 rem = a1 - col * 11;
    s32 y = rem * 13 + 0x42;
    func_801F0A78(a0, 0, a2, 0x23, y);
}

/**
 * @brief Address of the ability entry at index @p idx.
 */
AbilityEntry *func_801E2920(s32 idx) {
    return &D_8007CEE0[idx];
}

/**
 * @brief Read ability menu display count.
 * @return Value of D_801E3DB8.
 */
s32 func_801E2934(void) {
    return D_801E3DB8;
}

/**
 * @brief Look up an ability name string by index.
 *
 * If @p a0 is within bounds (less than D_801E3D9C), uses it to index
 * into D_801E3D84 to get an ability ID, then calls getAbilityDesc
 * to get the corresponding string. Returns NULL if out of bounds.
 *
 * @param a0 Ability list index.
 * @return Pointer to ability name string, or NULL if index out of bounds.
 */
s32 func_801E2944(s32 a0) {
    if (a0 >= D_801E3D9C) {
        return 0;
    }
    return getAbilityDesc(D_801E3D84[a0]);
}

/**
 * @brief Build list of equipped GF abilities for display.
 *
 * Scans 20 GF slots. For each slot, checks if the GF is unlocked
 * (D_80077CC8 offset 0xD22 bit 0) and whether the ability filter
 * (D_801E3D70) allows it. If valid and the ability count (offset 0x10)
 * is non-zero, adds the slot index to D_801E3DA4 and increments
 * the count in D_801E3DB8.
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2990);

/**
 * @brief Main sound/music menu state machine handler.
 *
 * Handles a 26-case switch for sound menu states including navigation,
 * selection, audio playback, and menu transitions. Uses division by 11
 * for grid position calculations throughout. Manages cursor movement,
 * track preview, and input processing.
 *
 * @param s Sound menu state pointer.
 *
 * @note Contains 4 inline asm statements (FIXME below) needed to force a
 *       specific register-allocation pattern in cases 10 and 19. The
 *       compiler's default allocation chose `$a0` for case 10's slot
 *       result and `$s3` for case 19's sign-extend source; target uses
 *       `$a2` and `$v0` respectively. The asm statements pin the addu's
 *       destination to `$a2` and keep `slot`/`newSlot` alive across the
 *       cast/comparison instructions to prevent register clobbering.
 *       After 134K+ permuter generations no pure-C alternative was
 *       found.
 */
void func_801E2A34(SoundMenuState *s) {
    MenuDisplayConfig *cfg = (MenuDisplayConfig *)g_menuDisplayCfg;
    u16 *statePtr = &s->state;
    u16 btnFlags = cfg->inputNew;
    u32 cfgFlags = cfg->inputRepeat;
    u16 state = s->state;
    s32 newSel;
    s32 slot;
    u8 *ptr;
    u8 trackId;
    u8 trackType;

restart:
    switch (state & 0xFFFF) {
    case 0:
        *statePtr = 1;
        break;

    case 1:
        s->field_2C += 0x100;
        if (s->field_2C >= 0x1000) {
            s->field_2C = 0x1000;
            *statePtr = 2;
        }
        func_801E28B4(1, s->field_3A, s->field_2C);
        break;

    case 2:
        *statePtr = 3;
        func_801E28B4(1, s->field_3A, s->field_2C);
        break;

    case 3: {
        s32 page = s->field_3A / 11;
        slot = s->field_3A % 11;
        if (btnFlags & 0x8000) {
            if (D_801E3D9C >= 12) {
                state = 4;
                goto restart;
            }
        }
        if (btnFlags & 0x2000) {
            if (D_801E3D9C >= 12) {
                state = 6;
                goto restart;
            }
        }
        newSel = func_801F6768(btnFlags, 11, slot);
        func_801E28B4(1, s->field_3A, s->field_2C);
        s->field_20 = func_801E2944(s->field_3A);
        s->field_3A = (page * 11) + newSel;
        if (cfgFlags & 0x40) {
            s32 cur = s->field_3A;
            if (cur < D_801E3D9C) {
                u8 *trackPtr = &D_801E3D84[cur];
                trackId = *trackPtr;
                ptr = (u8 *)func_801E2920(trackId);
                trackType = ptr[5];
                if (trackType != 0xFF) {
                    if (trackType == 0x81) {
                        if (D_8007809A & 1) {
                            ptr = (u8 *)func_801F6AA4(0x4F);
                            func_801F23D0(0, 0x68, (void *)ptr);
                            initSfxPlayback(0, ptr);
                            *statePtr = 0x10;
                            break;
                        }
                        sendSpuCommand(2);
                        s->field_3C = 0xC;
                        *statePtr = 0x12;
                        break;
                    }
                    if (trackType == 0x80) {
                        if (func_801E2934() == 0) {
                            ptr = (u8 *)func_801F6AFC(0x36);
                            func_801F23D0(0, 0x68, (void *)ptr);
                            initSfxPlayback(0, ptr);
                            *statePtr = 0x10;
                            break;
                        }
                        sendSpuCommand(2);
                        *statePtr = 8;
                        break;
                    }
                    sendSpuCommand(2);
                    s->field_3D = *trackPtr;
                    *statePtr = 0x17;
                    break;
                }
            }
            sendSpuCommand(5);
        }
        if (cfgFlags & 0x10) {
            sendSpuCommand(3);
            *statePtr = 0x18;
        }
        break;
    }

    case 4: {
        s32 page;
        s32 newPage;
        s32 maxPages;
        s32 slot;
        sendSpuCommand(1);
        s->field_37 = s->field_36;
        s->field_24 = s->field_20;
        page = s->field_3A / 11;
        newPage = page & 0xFF;
        newPage = newPage - 1;
        slot = s->field_3A % 11;
        maxPages = (D_801E3D9C + 10) / 11;
        if (newPage < 0) {
            newPage = maxPages - 1;
        }
        s->field_3A = (newPage * 11) + slot;
        s->field_36 = newPage;
        s->field_32 = -0xE67;
        func_801E28B4(1, s->field_3A, s->field_2C);
        s->field_20 = func_801E2944(s->field_3A);
        *statePtr = 5;
        break;
    }

    case 5:
        func_801E28B4(1, s->field_3A, s->field_2C);
        s->field_32 += 0x199;
        if (s->field_32 >= 0) {
            s->field_32 = 0;
            *statePtr = 3;
        }
        if (cfgFlags & 0x8000) {
            *statePtr = 4;
        }
        if (cfgFlags & 0x2000) {
            *statePtr = 6;
        }
        break;

    case 6: {
        s32 page;
        s32 newPage;
        s32 maxPages;
        s32 slot;
        sendSpuCommand(1);
        s->field_37 = s->field_36;
        s->field_24 = s->field_20;
        page = s->field_3A / 11;
        newPage = page & 0xFF;
        newPage = newPage + 1;
        slot = s->field_3A % 11;
        maxPages = (D_801E3D9C + 10) / 11;
        if (newPage >= maxPages) {
            newPage = 0;
        }
        s->field_3A = (newPage * 11) + slot;
        s->field_36 = newPage;
        s->field_32 = 0xE67;
        func_801E28B4(1, s->field_3A, s->field_2C);
        s->field_20 = func_801E2944(s->field_3A);
        *statePtr = 7;
        break;
    }

    case 7:
        func_801E28B4(1, s->field_3A, s->field_2C);
        s->field_32 -= 0x199;
        if (s->field_32 <= 0) {
            s->field_32 = 0;
            *statePtr = 3;
        }
        if (cfgFlags & 0x8000) {
            *statePtr = 4;
        }
        if (cfgFlags & 0x2000) {
            *statePtr = 6;
        }
        break;

    case 8:
        s->field_2E = 0x1000;
        *statePtr = 9;
        /* fall through */
    case 9:
        s->field_2E -= 0x100;
        if (s->field_2E <= 0) {
            s->field_2E = 0;
            *statePtr = 0xA;
        }
        func_801E28B4(0, s->field_3A, s->field_2C);
        func_801E2800(1, s->field_3B, s->field_2C, (MenuSlot *)s);
        break;

    case 10: {
        s32 page = s->field_3B / 11;
        slot = s->field_3B % 11;
        if (D_801E3DB8 >= 12) {
            if (btnFlags & 0x8000) {
                if (page != 0) {
                    state = 0xC;
                    goto restart;
                }
            }
            if (btnFlags & 0x2000) {
                if (page == 0) {
                    state = 0xE;
                    goto restart;
                }
            }
        }
        newSel = func_801F6768(btnFlags, 11, slot);
        func_801E28B4(0, s->field_3A, s->field_2C);
        func_801E2800(1, s->field_3B, s->field_2C, (MenuSlot *)s);
        {
            /* FIXME: Inline asm needed to force `addu` destination to $a2.
             *        Without this pin, GCC 2.7.2 picks $a0, breaking the
             *        match. The explicit asm("addu...") prevents the
             *        page*11 calculation from cascading through $a2. */
            register s32 newSlot asm("$6");
            asm("addu %0, %1, %2"
                : "=r"(newSlot)
                : "r"(page * 11), "r"(newSel));
            s->field_3B = newSlot;
            if (cfgFlags & 0x40) {
                if (((u8)newSlot) < D_801E3DB8) {
                    /* FIXME: Keep newSlot live past the andi (for the
                     *        upcoming `(u8)newSlot` cast) so the andi
                     *        writes to $v0 instead of clobbering $a2. */
                    asm volatile("" : : "r"(newSlot));
                    sendSpuCommand(2);
                    trackId = D_801E3DA4[s->field_3B];
                    func_801EFFE4(trackId);
                    s->field_3C = 0xB;
                    *statePtr = 0x12;
                    break;
                }
                sendSpuCommand(5);
            }
        }
        if (cfgFlags & 0x10) {
            sendSpuCommand(3);
            *statePtr = 0xB;
            break;
        }
        break;
    }

    case 11:
        s->field_2E += 0x100;
        if (s->field_2E >= 0x1000) {
            s->field_2E = 0x1000;
            *statePtr = 3;
        }
        func_801E28B4(0, s->field_3A, s->field_2C);
        func_801E2800(1, s->field_3B, s->field_2C, (MenuSlot *)s);
        break;

    case 12: {
        s32 page;
        s32 newPage;
        s32 maxPages;
        s32 slot;
        sendSpuCommand(1);
        s->field_39 = s->field_38;
        page = s->field_3B / 11;
        newPage = page & 0xFF;
        newPage = newPage - 1;
        slot = s->field_3B % 11;
        maxPages = (D_801E3DB8 + 10) / 11;
        if (newPage < 0) {
            newPage = maxPages - 1;
        }
        s->field_3B = (newPage * 11) + slot;
        s->field_38 = newPage;
        s->field_34 = -0xE67;
        func_801E28B4(0, s->field_3A, s->field_2C);
        func_801E2800(1, s->field_3B, s->field_2C, (MenuSlot *)s);
        *statePtr = 0xD;
        break;
    }

    case 13:
        func_801E28B4(0, s->field_3A, s->field_2C);
        func_801E2800(1, s->field_3B, s->field_2C, (MenuSlot *)s);
        s->field_34 += 0x199;
        if (s->field_34 < 0) {
            break;
        }
        s->field_34 = 0;
        *statePtr = 0xA;
        break;

    case 14: {
        s32 page;
        s32 newPage;
        s32 maxPages;
        s32 slot;
        sendSpuCommand(1);
        s->field_39 = s->field_38;
        page = s->field_3B / 11;
        newPage = page & 0xFF;
        newPage = newPage + 1;
        slot = s->field_3B % 11;
        maxPages = (D_801E3DB8 + 10) / 11;
        if (newPage >= maxPages) {
            newPage = 0;
        }
        s->field_3B = (newPage * 11) + slot;
        s->field_38 = newPage;
        s->field_34 = 0xE67;
        func_801E28B4(0, s->field_3A, s->field_2C);
        func_801E2800(1, s->field_3B, s->field_2C, (MenuSlot *)s);
        *statePtr = 0xF;
        break;
    }

    case 15:
        func_801E28B4(0, s->field_3A, s->field_2C);
        func_801E2800(1, s->field_3B, s->field_2C, (MenuSlot *)s);
        s->field_34 -= 0x199;
        if (s->field_34 > 0) {
            break;
        }
        s->field_34 = 0;
        *statePtr = 0xA;
        break;

    case 16:
        sendSpuCommand(5);
        s->field_30 = 0x258;
        setSfxPitch(0, 0);
        startSfxNormal(0);
        *statePtr = 0x11;
        /* fall through */
    case 17:
        s->field_30 -= 1;
        if (cfgFlags & 0x50) {
            func_801F7BEC(cfgFlags);
            s->field_30 = 0;
        }
        if (s->field_30 <= 0) {
            fadeOutSfxFast(0);
            *statePtr = 3;
            break;
        }
        break;

    case 18:
        func_801F0BF8(s->field_3C);
        *statePtr = 0x13;
        /* fall through */
    case 19:
        /* FIXME: Keep `slot` alive in $s3 across the function so cases 3
         *        and 10 retain their `addu a2, s3, zero` arg-pass copies.
         *        Without this barrier the compiler eliminates slot's
         *        s-reg allocation and folds it into a temp register. */
        asm volatile("" : : "r"(slot));
        s->field_2C -= 0x100;
        if (s->field_2C <= 0) {
            s->field_2C = 0;
            *statePtr = 0x14;
        }
        if (s->field_3C == 0xB) {
            func_801E28B4(0, s->field_3A, s->field_2C);
            func_801E2800(1, s->field_3B, s->field_2C, (MenuSlot *)s);
            break;
        }
        func_801E28B4(1, s->field_3A, s->field_2C);
        break;

    case 20:
        func_801F0C5C(s->field_3C, (void *)s);
        *statePtr = 0x15;
        break;

    case 21:
        if (func_801F0D84() == 0xE) {
            *statePtr = 0x16;
        }
        break;

    case 22:
        s->field_2C = s->field_2C + 0x100;
        if (s->field_3C != 0xB) {
            func_801E28B4(1, s->field_3A, s->field_2C);
        } else {
            func_801E28B4(0, s->field_3A, s->field_2C);
            func_801E2800(1, s->field_3B, s->field_2C, (MenuSlot *)s);
        }
        if (s->field_2C >= 0x1000) {
            s->field_2C = 0x1000;
            if (s->field_3C != 0xB) {
                *statePtr = 3;
            } else {
                *statePtr = 0xA;
            }
        }
        break;

    case 23:
        s->field_3C = 0x13;
        state = 0x12;
        goto restart;

    case 24:
        *statePtr = 0x19;
        /* fall through */
    case 25:
        s->field_2C -= 0x100;
        if (s->field_2C <= 0) {
            s->field_2C = 0;
            func_801F18FC((s32 *)s);
            func_801F0BB0();
        }
        func_801E28B4(1, s->field_3A, s->field_2C);
        break;
    }
}

/**
 * @brief Configure and draw an ability menu panel border.
 *
 * Sets up g_menuDisplayCfg with no icon, position from a2/a3,
 * fixed size (0xF4 x 0x12), then draws via func_801EF9AC.
 *
 * @param a0 Display list pointer
 * @param a1 OT pointer
 * @param a2 X position
 * @param a3 Y position
 */
void func_801E3530(s32 a0, s32 a1, s16 a2, s16 a3) {
    MenuDisplayConfig *cfg = (MenuDisplayConfig *)g_menuDisplayCfg;

    cfg->iconType    = 0;
    cfg->iconSubType = 0;
    cfg->x           = a2;
    cfg->w           = 0xF4;
    cfg->h           = 0x12;
    cfg->y           = a3;
    func_801EF9AC(a0, a1, 0x1000, g_menuColor);
}

/**
 * @brief Render an ability-list entry's name from @c g_menuDisplayCfg.dataPtr.
 *
 * Loads @c dataPtr[idx] from the list config (offset @c 0x20). If non-NULL,
 * decodes the name string via @c decodeMessage into a local buffer, then
 * renders it via @c func_801F0FEC at @c (cfg.x + yOff + 0xA, cfg.y + 9).
 * Returns the new display state from @c func_801F0FEC, or @p state unchanged
 * if the entry was NULL.
 *
 * @param ctx   Rendering context.
 * @param state Current display state (returned unchanged on NULL).
 * @param idx   Entry index into @c dataPtr.
 * @param unk3  Unused 4th register arg.
 * @param yOff  Extra Y offset (5th arg, passed on stack).
 */
s32 func_801E3580(s32 ctx, s32 state, s32 idx, s32 unk3, s32 yOff) {
    MenuDisplayConfig *cfg = (MenuDisplayConfig *)g_menuDisplayCfg;
    u8 **table = (u8 **)cfg->dataPtr;
    u8  buf[0x80];
    u8 *entry = table[idx];
    s32 result = state;
    s32 x, y;
    if (entry != NULL) {
        s32 adj = yOff + 0xA;
        x = cfg->x + adj;
        y = cfg->y + 9;
        decodeMessage(entry, buf, -1);
        result = func_801F0FEC(ctx, state, x, y, buf, 7);
    }
    return result;
}

/**
 * @brief Configure and draw an ability list panel with scrolling.
 *
 * Sets up g_menuDisplayCfg with icon type 0x55, position and scroll
 * parameters from the source data, then registers func_801E3580
 * as the rendering callback via func_801EFBB4.
 *
 * @param a0 Source data pointer (ability list state)
 * @param a1 X position for callback
 * @param a2 Y position for callback
 * @param a3 Panel X position
 * @param stackArg Panel Y position (5th arg on stack)
 */
void func_801E3630(s32 a0, s32 a1, s32 a2, s32 a3, s32 stackArg) {
    MenuDisplayConfig *cfg = (MenuDisplayConfig *)g_menuDisplayCfg;
    AbilityListCtx    *ctx = (AbilityListCtx *)a0;

    cfg->iconType     = 0x55;
    cfg->iconSubType  = 0;
    cfg->x            = a3;
    cfg->w            = 0x144;
    cfg->h            = 0x1A;
    cfg->columnCount  = 1;
    cfg->pageStart    = 0;
    cfg->pageEnd      = 1;
    cfg->y            = stackArg;
    cfg->scrollOffset = ctx->scrollOff;
    cfg->dataPtr      = (s32)ctx->items;
    func_801EFBB4(a1, a2, (s32)func_801E3580);
}

/**
 * @brief Render ability entry with conditional highlight in ability list.
 *
 * Computes x/y position from g_menuDisplayCfg base coordinates plus column/row
 * offsets derived from a2 (column * 11) and a3 (row * 13). If the computed
 * index is within bounds (D_801E3D9C), looks up the ability from D_801E3D84,
 * checks its type (0xFF = disabled, 0x80 = conditional, 0x81 = flag-based),
 * and determines the color code. Draws the entry via func_801F0FEC with
 * the ability name from getAbilityName.
 *
 * @param a0 Display context pointer.
 * @param a1 OT pointer.
 * @param a2 Column offset.
 * @param a3 Row offset.
 * @param stackArg X base offset.
 * @return Updated display list pointer.
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E36AC);

/**
 * @brief Configure ability list panel with character ability data and render.
 *
 * Sets up g_menuDisplayCfg with icon type 0x5E, size 0x8A x 0xA0, 11 rows,
 * copies scroll parameters from source data (offsets 0x36, 0x37),
 * stores the total ability count and source pointer. If the count exceeds
 * 12, calls func_801F5F30/func_801F5F60 for scrollbar rendering.
 * Finally registers func_801E36AC as the rendering callback.
 *
 * @param a0 Source data pointer (character ability state).
 * @param a1 OT pointer for rendering.
 * @param a2 Column position parameter.
 * @param a3 Panel X position.
 * @param stackArg Panel Y position (5th arg on stack).
 */
void func_801E381C(s32 a0, s32 a1, s32 a2, s32 a3, s32 stackArg) {
    MenuDisplayConfig *cfg = (MenuDisplayConfig *)g_menuDisplayCfg;
    AbilityListCtx    *ctx = (AbilityListCtx *)a0;

    cfg->iconType     = 0x5E;
    cfg->iconSubType  = 0;
    cfg->x            = a3;
    cfg->w            = 0x8A;
    cfg->h            = 0xA0;
    cfg->columnCount  = 0xB;
    cfg->y            = stackArg;
    cfg->pageStart    = ctx->pageStart;
    cfg->pageEnd      = ctx->pageEnd;
    cfg->scrollOffset = ctx->scrollOff;
    cfg->dataPtr      = (s32)ctx;

    if (D_801E3D9C >= 0xC) {
        s32 scrollbar = func_801F5F30(a1, a2, a3 + 0x28, stackArg, g_menuColor, ctx->pageStart);
        a2 = func_801F5F60(a1, scrollbar, g_menuColor, 3);
    }
    func_801EFBB4(a1, a2, (s32)func_801E36AC);
}

/**
 * @brief Render GF ability name at computed position in ability list.
 *
 * Computes x/y position from g_menuDisplayCfg base coordinates plus column/row
 * offsets. If the computed index is within bounds (D_801E3DB8), looks up
 * the ability ID from D_801E3DA4, gets the name via func_801F6AA4,
 * and draws it using func_801F0FEC.
 *
 * @param a0 Display context pointer.
 * @param a1 OT pointer.
 * @param a2 Column index.
 * @param a3 Row index (multiplied by 13 for y offset).
 * @param stackArg X base offset added to g_menuDisplayCfg.x.
 * @return Updated display list pointer.
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3904);

/**
 * @brief Configure GF ability list panel with scroll support and render.
 *
 * Sets up g_menuDisplayCfg with icon type 0x49, size 0xC6 x 0xA0, 11 rows,
 * copies scroll parameters from source data (offsets 0x38, 0x39),
 * stores the GF ability count and source pointer. If the count exceeds
 * 12, renders scrollbar via func_801F5F30/func_801F5F60. Scrollbar
 * arrow count depends on whether src+0x38 is zero (2 arrows) or not (1).
 * Registers func_801E3904 as the rendering callback.
 *
 * @param a0 Source data pointer (GF ability state).
 * @param a1 OT pointer for rendering.
 * @param a2 Column position parameter.
 * @param a3 Panel X position.
 * @param stackArg Panel Y position (5th arg on stack).
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E39E0);

/**
 * @brief Render complete ability menu page with panels and scrollbars.
 *
 * Called when the menu state is 0xE (active display). Saves GPU state,
 * loads character data, draws three panels (header, ability list, GF list),
 * with optional GF ability scrollbar if the GF ability count is under 0x1000.
 * Restores GPU state after rendering.
 *
 * @param a0 Menu state pointer (reads 0x2C, 0x2E offsets).
 * @param a1 OT/display list pointer.
 * @param a2 Column offset parameter.
 * @return Updated display list pointer, or a2 if state != 0xE.
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3AE0);

/**
 * @brief Scan ability bitmask and build list of available ability indices.
 *
 * Calls func_801F72B4 to get an ability bitmask, then iterates bits 0-23.
 * For each set bit, stores (bit_index + 0x5C) into D_801E3D84 array and
 * increments the count in D_801E3D9C.
 */
void func_801E3C28(void) {
    s32 mask = func_801F72B4();
    u8 *dst = D_801E3D84;
    D_801E3D9C = 0;
    {
        s32 i = 0;
        s32 one = 1;

        do {
            if (mask & (one << i)) {
                *dst = i + 0x5C;
                D_801E3D9C = D_801E3D9C + 1;
                dst++;
            }
            i++;
        } while (i < 0x18);
    }
}

/**
 * @brief Initialize ability menu: register callbacks and set up display state.
 *
 * Registers func_801E2A34 and func_801E3AE0 as callbacks via func_801F179C.
 * If registration succeeds, clears state fields (0x20, 0x2C), stores the
 * ability bitmask (0x28) and display size (0x2E = 0x1000), builds the ability
 * list, initializes data, and enters via func_801E2A34.
 */
void func_801E3C9C(void) {
    AbilityMenuState *st = (AbilityMenuState *)func_801F179C(
        (s32)func_801E2A34, (s32)func_801E3AE0);

    if (st != NULL) {
        st->state       = 0;
        st->dataPtr     = 0;
        st->abilityMask = func_801F72B4();
        st->displaySize = 0x1000;
        func_801E3C28();
        func_801E2990();
        func_801E2A34(st);
    }
}
