#include "common.h"
#include "menu.h"
#include "psxsdk/libgpu.h"

extern u8 D_801FA278;
extern u8 D_801FA279;
extern u8 D_801FA27A;
extern u8 D_801FA27B;
extern u8 D_801FA27C;
extern s8 D_801FAA11;
extern u8 D_801FAA18[];
extern s16 D_801FAAE0;
extern u8 D_801FAB7A;
extern u8 D_801FAB7B;
extern s16 D_801FAB78;
extern u16 D_80083850;
extern u8 D_801FAA10;
extern u16 D_801FAA12;
extern u16 D_801FAA14;
extern s16 D_801FAB28;
extern s16 D_801FAB2A;
extern s32 D_801FAB2C;
extern u8 D_801FAB30;
extern u8 D_801FAB31;
extern u8 D_801FAAF0;
extern u8 D_801FAAF8;
extern u16 D_801FABB8;
extern s32 D_801FACE8;
extern u16 D_801FACE2;
extern u16 D_801FACE4;
extern s32 D_801FAAE4;
extern s32 D_801FAAE8;
extern u16 D_801FAAF2;
extern u8 D_80077E5F;
extern u16 D_80077E5C;
extern u8 D_801F889C[];
extern u8 D_801F7F98[];
extern u8 D_80077378[];
extern u8 D_80077E6C[];
extern u8 D_801FABC8[];
extern u8 D_80077818[];
extern u8 D_80077EBC[];
extern u8 D_801FAB38[];
extern u8 D_80078720[];
extern u8 D_801F7F78[];
/* D_801FAB00.animCounter is D_801FAB00.animCounter (offset +0x12) */
extern s16 D_801FAA1E;
extern u8 D_801FABC4[];
extern u8 D_801F8BB8[];
extern u16 D_800780E8;
extern u8 D_80077864[];
extern u8 D_80056290[];
extern u8 D_801F7E6C[];
extern u8 D_801FAB88[];
extern u16 D_8007737C;
extern s32 D_80083848;
extern MenuDisplayConfig D_801FAB00;
extern u8 D_801F7FB0[];
extern u8 D_801F7F74[];
extern u8 D_80078D38[];

u8 *func_801F08AC(u8 *, s32);
s32 func_8002E744(s32);
s32 func_801F1A40(s32);
s32 func_801F179C(void *, void *);
s32 func_801F42A4();
s32 func_801F3FE8();
s32 func_801F4744();
void func_801EF9AC(s32, s32, s32, s32);
s32 func_801F6358(s32, s32, s32, s32, s32);
s32 func_801F0FEC(s32, s32, s32, s32, s32, s32);
s32 func_801F6AD0(s32);
s32 func_801F5F60(s32, s32, s32, s32);
void func_801EFBB4(s32, s32, s32);
s32 func_801F64A4(s32, s32, s32, s32, s32, s32, s32);
s32 func_801F3DE4(s32, s32, s32, s32, s32, s32, s32);
s32 func_801F6234(s32, s32, s32, s32, s32);
void func_801F605C(s32, s32, s32, s32, s32);
s32 func_801F776C(s32, s32);
void func_80027F38(s32, s32, s32);

extern u8 D_801FA280[];
extern s32 D_801FA3C0;
extern s32 D_8005F134;
extern s32 D_8005F138;
void VSync(s32);

/* ======================================================================== */
/* Panel/Window Rendering                                                   */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EF800);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EF8D8);

/**
 * @brief Draw inner panel with 1px inset border.
 *
 * Shrinks the rect by 1 pixel on each side, draws the panel via
 * func_801EF800, then restores the original rect dimensions.
 */
void func_801EF934(s32 a0, s32 a1, s32 *a2) {
    s32 save0 = a2[0];
    s32 save1 = a2[1];
    u16 *rect = (u16 *)a2;
    rect[0] += 1;
    rect[1] += 1;
    rect[2] -= 2;
    rect[3] -= 2;
    func_801EF800(a0, a1);
    a2[0] = save0;
    a2[1] = save1;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EF9AC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EFBB4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EFF64);

/* ======================================================================== */
/* Menu State Getters/Setters                                               */
/* ======================================================================== */

/** @brief Get current menu phase/mode. */
s32 func_801EFFB8(void) {
    return D_801FA27C;
}

/** @brief Set menu sub-state. */
void func_801EFFC8(s32 a0) {
    D_801FA27B = a0;
}

/** @brief Get menu sub-state. */
s32 func_801EFFD4(void) {
    return D_801FA27B;
}

/** @brief Set menu state A. */
void func_801EFFE4(s32 a0) {
    D_801FA27A = a0;
}

/** @brief Get menu state A. */
s32 func_801EFFF0(void) {
    return D_801FA27A;
}

/** @brief Set menu state B. */
void func_801F0000(s32 a0) {
    D_801FA278 = a0;
}

/** @brief Get menu state B. */
s32 func_801F000C(void) {
    return D_801FA278;
}

/** @brief Set menu flag. */
void func_801F001C(s32 a0) {
    D_801FA279 = a0;
}

/** @brief Get menu flag. */
s32 func_801F0028(void) {
    return D_801FA279;
}

/* ======================================================================== */
/* Menu Panel/Window Management                                             */
/* ======================================================================== */

/**
 * @brief Open/advance the current menu panel window.
 *
 * If the active window pointer (D_801FA3C0) equals D_801FA280 base,
 * advances it by 0xA0 bytes. Sets up the draw-list entry at
 * offset 0x74 and links the panel into the global list (D_8005F134).
 */
void func_801F0038(void)
{
    s32 val = D_801FA3C0;
    s32 base = D_801FA280;
    s32 new_var;
    s32 temp;
    if (val == base) {
        base = (s32)D_801FA280 + 0xA0;
    }
    D_801FA3C0 = base;
    ClearOTag(base + 0x74, 9);
    new_var = D_801FA3C0;
    temp = *((s32 *) (new_var + 0x98));
    *((s32 *) (new_var + 0x70)) = new_var + 0x74;
    D_8005F134 = new_var + 0x14;
    *((s32 *) (new_var + 0x9C)) = temp;
}

/**
 * @brief Initialize display system and link the active panel.
 *
 * Resets the display pipeline, sets D_801FA3C0 as both
 * the active window (D_8005F138) and draw-list head (D_8005F134),
 * then initializes rendering from the panel's draw-list at offset 0x70.
 */
void func_801F00A0(void)
{
    s32 val;
    s32 arg;
    DrawSync(0);
    VSync(0);
    PutDispEnv(D_801FA3C0);
    PutDrawEnv(D_801FA3C0 + 0x14);
    val = D_801FA3C0;
    D_8005F138 = val;
    arg = *(s32 *)(val + 0x70);
    D_8005F134 = val + 0x14;
    DrawOTag(arg);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F010C);

/**
 * @brief Initialize main menu panel system.
 *
 * Sets display width to 0x180, assigns VRAM buffer pointers at
 * offsets 0x98 and 0x138 in D_801FA280, then resets state.
 */
void func_801F0224(void) {
    s32 base;
    func_801F010C(0x180);
    base = (s32)D_801FA280;
    *(s32 *)(base + 0x98) = (s32)0x801B2000;
    *(s32 *)(base + 0x138) = (s32)0x801B8800;
    func_80027F38(0, 0, 0);
}

/* ======================================================================== */
/* VRAM/Display Setup                                                       */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0274);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F03E8);

/**
 * @brief Upload two display regions to VRAM.
 *
 * Configures two 0x180-wide rectangular regions (at x=0 and x=0x200)
 * and transfers them via ClearImage/DrawSync. Initializes
 * double-buffered VRAM areas for the menu background.
 */
void func_801F0464(s32 a0) {
    s32 buf[2];
    buf[0] = 0;
    *(u16 *)((u8 *)buf + 4) = 0x180;
    *(u16 *)((u8 *)buf + 6) = a0;
    ClearImage(buf, 0, 0, 0);
    DrawSync(0);
    buf[0] = 0x200;
    *(u16 *)((u8 *)buf + 4) = 0x180;
    *(u16 *)((u8 *)buf + 6) = a0;
    ClearImage(buf, 0, 0, 0);
    DrawSync(0);
}

/* ======================================================================== */
/* Text/String Lookup & Rendering                                           */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F04E8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F07D8);

/** @brief Render text string with default style (last param = 0). */
void func_801F0884(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    func_801F07D8(a0, a1, a2, a3, a4, 0);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F08AC);

/**
 * @brief Look up a text string by category and ID.
 *
 * Navigates a two-level string table. Uses 0x801E0000 for type 1,
 * D_801F7FB0 otherwise. Returns D_801F7F74 (fallback) on failure.
 */
u8 *func_801F08D4(s32 a0, s32 a1, s32 a2, s32 a3) {
    u8 *ptr;
    if (a0) {
        ptr = (u8 *)0x801E0000;
    } else {
        ptr = D_801F7FB0;
    }
    if (!ptr) return D_801F7F74;
    ptr = func_801F08AC(ptr, a1);
    if (!ptr) return D_801F7F74;
    ptr = func_801F08AC(ptr, a2 * 2 + a3);
    if (ptr) return ptr;
    return D_801F7F74;
}

/* ======================================================================== */
/* Text Rendering Helpers (blink, color, conditional)                       */
/* ======================================================================== */

/** @brief Set current text color/font parameter for rendering. */
void func_801F0948(s32 a0) {
    D_801FAAE0 = a0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0954);

/** @brief Render text at stored Y with default color. */
void func_801F0994(s32 a0, s32 a1, s32 a2) {
    func_801F0954(a0, D_801FAAE0, a1, a2);
}

/**
 * @brief Render text with blink effect.
 *
 * If (D_801FAB00.animCounter + D_801FAA1E) is odd, sets color to -1 (hidden),
 * creating a blinking text effect for highlighted menu items.
 */
void func_801F09C4(s32 a0, s32 a1, s32 a2, s32 a3) {
    if (!((D_801FAB00.animCounter + D_801FAA1E) & 1)) {
        a0 = -1;
    }
    func_801F0954(a0, a1, a2, a3);
}

/** @brief Render blinking text at stored Y with default color. */
void func_801F0A04(s32 a0, s32 a1, s32 a2) {
    func_801F09C4(a0, D_801FAAE0, a1, a2);
}

/** @brief Render text, normal if a0 is nonzero, blinking otherwise. */
void func_801F0A34(s32 a0, s32 a1, s32 a2, s32 a3) {
    if (a0) {
        func_801F0994(a1, a2, a3);
    } else {
        func_801F0A04(a1, a2, a3);
    }
}

/** @brief Render text with explicit Y, normal if a0 nonzero, blinking otherwise. */
void func_801F0A78(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    if (a0) {
        func_801F0954(a1, a2, a3, a4);
    } else {
        func_801F09C4(a1, a2, a3, a4);
    }
}

/* ======================================================================== */
/* Menu Navigation Stack                                                    */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0AC8);

/**
 * @brief Pop top entry from menu navigation stack.
 *
 * Decrements the stack pointer and returns the popped screen ID.
 * Returns -1 if the stack is empty.
 */
s32 func_801F0BB0(void) {
    s8 val;

    D_801FAA11--;
    val = D_801FAA11;
    if (val < 0) {
        D_801FAA11 = 0;
        return -1;
    }
    return D_801FAA18[val];
}

/**
 * @brief Play music/sound for a menu screen.
 *
 * Looks up audio entry in D_801F7E6C (stride 8). If the track
 * differs from what is currently playing, loads and plays it.
 */
void func_801F0BF8(s32 a0) {
    u8 *entry = D_801F7E6C + a0 * 8;
    s32 val = *(s32 *)(entry + 4);
    if (val == 0xFF) return;
    if (val <= 0) return;
    if (func_80035E40() == val) return;
    func_80036104(val, 0, 0);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0C5C);

/** @brief Peek at top of menu navigation stack without popping. */
s32 func_801F0D84(void) {
    return D_801FAA18[(s8)D_801FAA11];
}

/** @brief Set current menu screen ID. */
void func_801F0DA4(s32 a0) {
    D_801FAA10 = a0;
}

/** @brief Get current menu screen ID. */
s32 func_801F0DB0(void) {
    return D_801FAA10;
}

/* ======================================================================== */
/* Animation/Transition                                                     */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0DC0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0E5C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0F20);

/** @brief Set menu display dimensions (width defaults to 0x180). */
void func_801F0FD0(s32 a0, s32 a1) {
    if (!a1) {
        a1 = 0x180;
    }
    D_801FAA12 = a1;
    D_801FAA14 = a0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0FEC);

/** @brief Get animation completion flag. */
s32 func_801F1200(void) {
    return D_801FAAF8;
}

/**
 * @brief Start a timed animation/transition.
 *
 * Sets source value, destination value, resets counter, and enables
 * the animation. D_801FAAF8 will be set when the transition completes.
 */
void func_801F1210(s32 a0, s32 a1) {
    D_801FAAE4 = a0;
    D_801FAAE8 = a1;
    D_801FAAF2 = 0;
    D_801FAAF0 = 1;
    D_801FAAF8 = 0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1240);

/** @brief Stop the current animation/transition. */
void func_801F12F0(void) {
    D_801FAAF0 = 0;
}

/* ======================================================================== */
/* Panel Iteration & Callbacks                                              */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F12FC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1584);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F16AC);

/** @brief Process all panel callbacks (wrapper for func_801F16AC). */
void func_801F175C(void) {
    func_801F16AC();
}

/** @brief Clear 22 panel callback entries starting at a0[8]. */
void func_801F177C(s32 *a0) {
    s32 i;

    for (i = 21; i >= 0; i--) {
        a0[i + 8] = 0;
    }
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F179C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1850);

/**
 * @brief Remove a node from the panel doubly-linked list.
 *
 * Unlinks the node, patches prev/next pointers, and clears the
 * node's status byte and data fields.
 */
void func_801F18FC(s32 *a0) {
    s32 *next = (s32 *)a0[1];
    s32 *prev = (s32 *)a0[0];
    *next = (s32)prev;
    prev[1] = (s32)next;
    *(u8 *)((u8 *)a0 + 0x12) = 0;
    a0[2] = 0;
    a0[3] = 0;
    *(s16 *)((u8 *)a0 + 0x10) = 0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1924);

/* ======================================================================== */
/* Cursor/Input Handling                                                    */
/* ======================================================================== */

/** @brief Panel input state machine: advance from idle to active. */
s32 func_801F1A40(s32 a0) {
    s32 ret = 1;
    switch (*(u16 *)(a0 + 0x10)) {
    case 0:
        *(u16 *)(a0 + 0x10) = ret;
        break;
    case 1:
        if (func_801F0D84() != 0) {
            break;
        }
        func_80035C54(1);
        break;
    default:
        break;
    }
    return ret;
}

/** @brief Default panel callback (identity, returns a2 unchanged). */
s32 func_801F1AA4(s32 a0, s32 a1, s32 a2) {
    return a2;
}

/** @brief Iterate all panels and dispatch via callback. */
void func_801F1AAC(void) {
    s32 result = func_801F179C(func_801F1A40, func_801F1AA4);

    if (result != 0) {
        func_801F1A40(result);
    }
}

/* ======================================================================== */
/* Party Member Data & Input                                                */
/* ======================================================================== */

/** @brief Set cursor dimensions (row count, column count). */
void func_801F1AE8(s32 a0, s32 a1) {
    D_801FAB7A = a0;
    D_801FAB7B = a1;
}

/** @brief Snapshot current controller input state. */
void func_801F1AFC(void) {
    D_801FAB78 = D_80083850;
}

/** @brief Restore saved controller input state for menu processing. */
void func_801F1B10(void) {
    func_8002E7C4(D_801FAB78);
    func_80030058(D_801FAB78);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1B4C);

/** @brief Update all 8 party member cursor/panel states. */
void func_801F1CAC(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        func_801F1B4C(i);
    }
}

/** @brief Test if ability bit a1 is set for character a0. */
s32 func_801F1CE8(s32 a0, s32 a1) {
    u32 *base = (u32 *)(D_801FAB38 + a0 * 8);
    return (base[a1 / 32] & (1 << (a1 & 0x1F))) != 0;
}

/* ======================================================================== */
/* Menu Item List                                                           */
/* ======================================================================== */

/** @brief Empty stub (placeholder). */
void func_801F1D2C(void) {
}

/**
 * @brief Initialize menu item list from 0xFF-terminated data.
 *
 * Counts entries (stride 2 bytes) and stores the data pointer,
 * entry count, and resets the selection index.
 */
void func_801F1D34(u8 *a0) {
    s32 count;
    s32 sentinel;

    D_801FAB2C = (s32)a0;
    D_801FAB31 = 0;
    if (a0 == 0) {
        D_801FAB30 = 0;
    } else {
        count = 0;
        sentinel = 0xFF;
    top:
        if (*a0 == sentinel) goto out;
        a0 += 2;
        count++;
        goto top;
    out:
        D_801FAB30 = count;
    }
}

/** @brief Reset all menu item list state (scroll, pointer, count, index). */
void func_801F1D84(void) {
    D_801FAB28 = 0;
    D_801FAB2A = 0;
    D_801FAB2C = 0;
    D_801FAB30 = 0;
    D_801FAB31 = 0;
}

/** @brief Set menu list scroll offset. */
void func_801F1DB0(s32 a0) {
    D_801FAB28 = a0;
}

/**
 * @brief Select a menu item list by ID.
 *
 * Searches D_801F7F78 for the matching entry, then initializes the
 * list and sets the selection index to the found position.
 */
void func_801F1DBC(s32 a0) {
    u8 *ptr = D_801F7F78;
    s32 i;

    for (i = 0; ; i++, ptr += 2) {
        if (*ptr == 0xFF) {
            return;
        }
        if (*ptr == a0) {
            func_801F1D34(D_801F7F78);
            D_801FAB31 = i;
            return;
        }
    }
}

/* ======================================================================== */
/* Party Member Switch                                                      */
/* ======================================================================== */

/** @brief Save 3 active party slot IDs from D_80077E6C to buffer at a0+0x35. */
void func_801F1E20(u8 *a0) {
    u8 *src = D_80077E6C;
    u8 *dst = a0 + 0x35;
    s32 i;
    for (i = 0; i < 3; i++) {
        *dst++ = *src++;
    }
}

/** @brief Restore 3 active party slot IDs from buffer at a0+0x35 to D_80077E6C. */
void func_801F1E54(u8 *a0) {
    u8 *dst = D_80077E6C;
    u8 *src = a0 + 0x35;
    s32 i;
    for (i = 0; i < 3; i++) {
        *dst++ = *src++;
    }
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1E88);

/** @brief Swap party members (forward direction). */
void func_801F1F58(s32 a0, s32 a1) {
    func_801F1E88(a0, a1, 1);
}

/** @brief Swap party members (backward direction). */
void func_801F1F78(s32 a0, s32 a1) {
    func_801F1E88(a0, a1, 0);
}

/**
 * @brief Build sorted available character list from party data.
 *
 * Fills D_801FAB88 with 0xFF, then collects valid (non-0xFF) entries
 * from 3 active slots (a0+0x35) and 8 reserve slots (a0+0x38).
 */
void func_801F1F98(u8 *a0) {
    u8 *dst = D_801FAB88;
    s32 i;
    u8 *p;
    s32 fill;

    fill = 0xFF;
    i = 8;
    p = dst + 8;
    do {
        *p = fill;
        i--;
        p--;
    } while (i >= 0);

    for (i = 0; i < 3; i++) {
        u8 val = *(a0 + i + 0x35);
        if (val != 0xFF) {
            *dst++ = val;
        }
    }

    for (i = 0; i < 8; i++) {
        u8 val = *(a0 + i + 0x38);
        if (val != 0xFF) {
            *dst++ = val;
        }
    }
}

/* ======================================================================== */
/* Character/Entity Data Access                                             */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F202C);

/** @brief Get character status flags (u16 at D_80077378 + a0*152 + 0x526). */
s32 func_801F21D0(s32 a0) {
    s32 base = (s32)D_80077378;
    return *(u16 *)(base + a0 * 152 + 0x526);
}

/** @brief Set character status flags and sync to secondary table. */
void func_801F21FC(s32 a0, s32 a1) {
    s32 base = (s32)D_80077378;
    s32 base2 = (s32)D_801FABC8;
    *(s16 *)(base + a0 * 152 + 0x526) = a1;
    *(s16 *)(base2 + a0 * 32 + 0xE) = a1;
}

/** @brief Get GF status (stub, always returns 0). */
s32 func_801F2238(s32 a0) {
    return 0;
}

/**
 * @brief Get entity health condition from D_80078D38 table.
 *
 * Returns 1 if dead (HP <= 0), 0x100 if critical (HP < 25% max),
 * or 0 for normal health.
 */
s32 func_801F2240(s32 a0) {
    u8 *entry = D_80078D38 + a0 * 12;
    s16 val = *(s16 *)(entry);
    s32 result = 0;
    if (val <= 0) {
        result = 1;
    } else {
        s32 limit = (s16)(*(u16 *)(entry + 2)) >> 2;
        if (val < limit) {
            result = 0x100;
        }
    }
    return result;
}

/** @brief Get party presence bitmask. */
s32 func_801F2298(void) {
    return D_801FABB8;
}

/**
 * @brief Compute party presence bitmask.
 *
 * Iterates 3 active party slots at D_80077378 + 0xAF4, sets a bit
 * for each valid (non-0xFF) member ID. Stores result in D_801FABB8.
 */
void func_801F22A8(void) {
    s32 result = 0;
    s32 i;
    s32 base;
    REGALLOC_BARRIER(result);
    i = 0;
    base = (s32)D_80077378;

    for (; i < 3; i++) {
        u8 val = *(u8 *)(i + base + 0xAF4);
        if (val != 0xFF) {
            result |= (1 << val);
        }
    }
    D_801FABB8 = result;
}

/* ======================================================================== */
/* Character Panel Rendering                                                */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F22F4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F2370);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F23D0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F2458);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F2FAC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3270);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3464);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F36E8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3824);

/**
 * @brief Draw character name panel with icon.
 *
 * Looks up the character name via func_801F6AD0, renders it with
 * func_801F0FEC, configures D_801FAB00 (icon 0x55), and draws
 * the panel border via func_801EF9AC.
 */
void func_801F38F8(s32 a0, s32 a1, s32 a2) {
    s32 ret1;
    s32 ret2;

    ret1 = func_801F6AD0(*(u8 *)(a0 + 0x46));
    ret2 = func_801F0FEC(a1, a2, 0x22, 0xF, ret1, 7);
    D_801FAB00.iconType = 0x55;
    D_801FAB00.iconSubType = 0;
    D_801FAB00.x = 0x18;
    D_801FAB00.y = 7;
    *(s32 *)&D_801FAB00.w = 0x001900F4; /* w=0xF4, h=0x19 packed */
    func_801EF9AC(a1, ret2, 0x1000, D_80083848);
}

/** @brief Render text with explicit parameters (arg-reorder wrapper for func_801F0FEC). */
void func_801F3994(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F0FEC(a1, a2, a3, a4, a0, a5);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F39D0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3ABC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3B64);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3CE0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3DE4);

/**
 * @brief Map status flags to display text color.
 *
 * Returns: 7 (white/normal), 2 (yellow/critical HP),
 * 5 (red/status ailment), 1 (gray/dead).
 */
s32 func_801F3FB4(s32 a0) {
    s32 v1 = 7;

    if (a0 & 0x100) {
        v1 = 2;
    }
    if (a0 & 0xFE) {
        v1 = 5;
    }
    if (a0 & 1) {
        v1 = 1;
    }
    return v1;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3FE8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4168);

/** @brief Draw ability/status list line (default mode, last param = 0). */
void func_801F4274(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F4168(a0, a1, a2, a3, a4, a5, 0);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F42A4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4454);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4744);

/**
 * @brief Draw character command abilities panel.
 *
 * If a0[0x34] == 0, renders 3 command slots via func_801F42A4;
 * otherwise renders via func_801F3FE8 plus func_801F4744.
 */
s32 func_801F486C(u8 *a0, s32 a1) {
    s32 i;
    s32 result;

    if (a0[0x34] == 0) {
        for (i = 0; i < 3; i++) {
            result = func_801F42A4(a0, a1, result, i);
        }
    } else {
        for (i = 0; i < 3; i++) {
            result = func_801F3FE8(a0, a1, result, i);
        }
        result = func_801F4744(a0, a1);
    }
    return result;
}

/**
 * @brief Draw Gil (money) display panel.
 *
 * Renders the Gil label string, sets up D_801FAB00 rendering params,
 * and draws the panel decoration via func_801EF9AC.
 */
void func_801F4918(s32 a0, s32 a1, s32 a2) {
    s32 ret;
    ret = func_801F6358(a1, a2, 0x22, 0xC6, (s32)D_8007737C);
    D_801FAB00.iconType = 0;
    D_801FAB00.iconSubType = 0;
    D_801FAB00.x = 0x18;
    D_801FAB00.y = 0xBE;
    D_801FAB00.w = 0xF4;
    D_801FAB00.h = 0x1A;
    func_801EF9AC(a1, ret, 0x1000, D_80083848);
}

/* ======================================================================== */
/* Party Status & HP Display                                                */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F49A4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4A98);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4C60);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4CE8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4D70);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4EA8);

/** @brief Convert raw ability ID to menu display index (subtract 100). */
s32 func_801F5104(s32 a0) {
    return a0 - 0x64;
}

/**
 * @brief Compute ability learn percentage (0-100).
 *
 * Subtracts 800 or 900 threshold depending on value, then clamps
 * the result to the 0-100 range.
 */
s32 func_801F510C(s32 a0) {
    s32 v1;

    if (a0 < 0x385) {
        a0 -= 0x320;
    } else {
        a0 -= 0x384;
    }
    v1 = 0;
    if (a0 >= 0) {
        v1 = 0x64;
        if (a0 < 0x65) {
            v1 = a0;
        }
    }
    return v1;
}

/** @brief Check if ability is fully mastered (value >= 901). */
s32 func_801F5144(s32 a0) {
    return a0 >= 0x385;
}

/**
 * @brief Compute HP color flags from current HP and max HP.
 *
 * Clears low bits of a2, sets 0x200 if HP < 50% max, 0x300 if
 * HP < 25% max. Returns 1 if HP <= 0 (dead).
 */
s32 func_801F5150(s32 a0, s32 a1, s32 a2) {
    a2 &= ~0x301;
    if (a0 < (a1 >> 1)) {
        a2 |= 0x200;
    }
    if (a0 < (a1 >> 2)) {
        a2 |= 0x300;
    }
    if (a0 <= 0) {
        a2 = 1;
    }
    return a2;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5190);

/** @brief Save 3 active party slots to D_801FABC4 and clear originals to 0xFF. */
void func_801F5300(void) {
    s32 i = 0;
    s32 dstBase = (s32)D_801FABC4;
    s32 srcBase = (s32)D_80077378;

    for (; i < 3; i++) {
        *(u8 *)(i + dstBase) = *(u8 *)(i + srcBase + 0xAF4);
        *(u8 *)(i + srcBase + 0xAF4) = 0xFF;
    }
}

/** @brief Restore 3 active party slots from D_801FABC4 backup. */
void func_801F5340(void) {
    s32 i = 0;
    s32 dstBase = (s32)D_80077378;
    s32 srcBase = (s32)D_801FABC4;

    for (; i < 3; i++) {
        *(u8 *)(i + dstBase + 0xAF4) = *(u8 *)(i + srcBase);
    }
}

typedef struct { s32 w0, w1, w2, w3; } CopyBlock16;

/** @brief Recalculate stats for a party slot and copy result table to dst. */
void func_801F537C(s32 a0, CopyBlock16 *a1) {
    CopyBlock16 *src;
    CopyBlock16 *end;

    func_801F5300();
    func_801F5190(a0);

    src = (CopyBlock16 *)D_80078720;
    end = (CopyBlock16 *)((u8 *)D_80078720 + 0x1D0);
    do {
        *a1++ = *src++;
    } while (src != end);

    func_801F5340();
    func_80023888();
}

/** @brief Recalculate stats for a single party slot after swap. */
void func_801F5400(s32 a0) {
    func_801F5300();
    func_801F5190(a0);
    func_801F5340();
    func_80023888();
}

/** @brief Recalculate stats for all 8 party slots. */
void func_801F5440(void) {
    s32 i;
    func_801F5300();
    for (i = 0; i < 8; i++) {
        func_801F5190(i);
    }
    func_801F5340();
    func_80023888();
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5490);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F565C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F56E4);

/** @brief Set entity status flags, dispatched by type (character < 16, GF >= 16). */
void func_801F576C(s32 a0, s32 a1) {
    if (a0 >= 16) {
        func_801F2238(a0 - 16);
    } else {
        func_801F21FC(a0, a1);
    }
}

/** @brief Get entity status flags, dispatched by type (character < 16, GF >= 16). */
s32 func_801F57A4(s32 a0) {
    if (a0 >= 16) {
        return func_801F2240(a0 - 16);
    } else {
        return func_801F21D0(a0);
    }
}

/**
 * @brief Get entity current HP.
 *
 * For GF (a0 >= 16): reads from D_80077378 at stride 68 + 0x62.
 * For character: returns 0 if dead, else reads at stride 152 + 0x490.
 */
u16 func_801F57DC(s32 a0) {
    if (a0 >= 16) {
        s32 base = (s32)D_80077378;
        a0 -= 16;
        return *(u16 *)(base + a0 * 68 + 0x62);
    } else {
        if (func_801F57A4(a0) & 1) {
            return 0;
        }
        {
            s32 base = (s32)D_80077378;
            return *(u16 *)(base + a0 * 152 + 0x490);
        }
    }
}

/** @brief Set entity current HP (updates both primary and cache tables). */
void func_801F5868(s32 a0, s16 a1) {
    if (a0 >= 16) {
        s32 idx = a0 - 16;
        s32 base1 = (s32)D_80077378;
        s32 base2 = (s32)D_80078720;
        *(s16 *)(base1 + idx * 68 + 0x62) = a1;
        *(s16 *)(base2 + idx * 12 + 0x618) = a1;
    } else {
        s32 base1 = (s32)D_80077378;
        s32 base2 = (s32)D_801FABC8;
        *(s16 *)(base1 + a0 * 152 + 0x490) = a1;
        *(s16 *)(base2 + a0 * 32 + 0x8) = a1;
    }
}

/** @brief Get entity max HP. */
s32 func_801F58EC(s32 a0) {
    if (a0 < 16) {
        s32 base = (s32)D_801FABC8;
        return *(s16 *)(base + a0 * 32 + 0xA);
    } else {
        s32 idx = a0 - 16;
        s32 base = (s32)D_80078720;
        return *(s16 *)(base + idx * 12 + 0x61A);
    }
}

/** @brief Get entity level. */
s32 func_801F5938(s32 a0) {
    if (a0 < 16) {
        s32 base = (s32)D_801FABC8;
        return *(u8 *)(base + a0 * 32 + 0xC);
    } else {
        s32 idx = a0 - 16;
        s32 base = (s32)D_80078720;
        return *(u8 *)(base + idx * 12 + 0x620);
    }
}

/**
 * @brief Build cumulative pixel-width table for menu item strings.
 *
 * Iterates a -1-terminated u16 source list, measures each string's
 * pixel width via func_8002E744, and accumulates offsets into dst.
 * Returns the item count.
 */
s32 func_801F5984(u16 *src, u16 *dst, s32 a2) {
    s32 accum = 0;
    s32 count = 0;
    s16 val;
    s32 ret;
    *dst++ = 0;
    while (1) {
        val = (s16)*src++;
        if (val == -1) break;
        ret = func_801F08D4(1, a2, val, 0);
        ret = func_8002E744(ret) + 12;
        accum += ret;
        *dst++ = accum;
        count++;
    }
    return count;
}

/* ======================================================================== */
/* Number & String Rendering                                                */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5A38);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5B54);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5C84);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5D5C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5E0C);

/** @brief Render number right-aligned (last param = 1). */
void func_801F5EFC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F5E0C(a0, a1, a2, a3, a4, a5, 1);
}

/** @brief Render number left-aligned (last param = 0). */
s32 func_801F5F30(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F5E0C(a0, a1, a2, a3, a4, a5, 0);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5F60);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F605C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6234);

/* ======================================================================== */
/* String Resource Lookup                                                   */
/* ======================================================================== */

/** @brief Look up string resource pointer by index from D_801F8BB8 table. */
s32 func_801F6324(s32 a0) {
    u16 *table = (u16 *)D_801F8BB8;

    if (a0 >= table[0]) {
        a0 = 0;
    }
    a0 += 1;
    return table[a0] + (s32)table;
}

/** @brief Decode and render an indexed string resource to screen. */
s32 func_801F6358(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    u8 buf[256];
    s32 ptr = func_801F6324(a4);
    func_8002F688(ptr, buf, -1);
    func_801F0FEC(a0, a1, a2, a3, buf, 7);
}

/** @brief Draw icon/sprite at position (arg-reorder wrapper for func_800375A0). */
void func_801F63DC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_800375A0(a1, a2, a0, a3, a4, a5);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6418);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F64A4);

/**
 * @brief Draw full character info panel.
 *
 * Chains: func_801F64A4 (header/portrait), func_801F3DE4 (stats block),
 * func_801F6234 (HP/status bar), func_801F605C (ability/junction summary).
 */
void func_801F65F0(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    s32 ret;

    ret = func_801F64A4(a0, a1, a2, a3, a4, a5, 1);
    ret = func_801F3DE4(*(s32 *)a5, *(s32 *)(a5 + 4), a0, ret, a2 + 0x20, a3 + 0x7C, 7);
    ret = func_801F6234(a0, ret, a2 + 0xD0, a3 + 0x39, *(u16 *)(a5 + 0xE));
    func_801F605C(a0, ret, a2 + 0x10E, a3 + 0x38, a5);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F66B0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6768);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6800);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6888);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6934);

/** @brief Advance pseudo-random number generator (LCG: val*125+14 mod 32768). */
s32 func_801F6A5C(void) {
    extern u8 g_battleAnims[];
    s32 base = (s32)g_battleAnims;
    s32 val = *(u16 *)(base + 0x9C2);
    val = (val * 125 + 14) % 32768;
    *(u16 *)(base + 0x9C2) = val;
    return val;
}

/** @brief Look up ability/command name string (category 3). */
void func_801F6AA4(s32 a0) {
    func_801F08D4(1, 3, a0, 0);
}

/** @brief Look up character name string. */
s32 func_801F6AD0(s32 a0) {
    func_801F08D4(0, 0, a0, 1);
}

/** @brief Look up character description string. */
void func_801F6AFC(s32 a0) {
    func_801F08D4(0, 0, a0, 0);
}

/** @brief Map ability index to display category via double indirection. */
s32 func_801F6B28(s32 a0) {
    return D_801F7F98[D_801F889C[a0 * 4]];
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6B54);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6C9C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6D88);

/* ======================================================================== */
/* Config/Settings Display                                                  */
/* ======================================================================== */

/** @brief Get packed cursor coordinates (D_801FACE4 << 16 | D_801FACE2). */
s32 func_801F6F6C(void) {
    return (D_801FACE4 << 16) | D_801FACE2;
}

/** @brief Render menu item at grid position derived from hi/lo nibbles of a0. */
void func_801F6F88(s32 a0) {
    s32 hi = a0 >> 4;
    s32 lo = a0 & 0xF;
    s32 val = func_801F6F6C();

    func_801F0994(lo | hi, (u16)val, (val >> 16) + lo * 13);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6FE4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7148);

/** @brief Render config option (simplified wrapper for func_801F7148). */
void func_801F728C(s32 a0, s32 a1) {
    func_801F7148(a0, 0, 0, a1);
}

/** @brief Get current config value. */
s32 func_801F72B4(void) {
    return D_801FACE8;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F72C4);

/* ======================================================================== */
/* Junction/Ability Management                                              */
/* ======================================================================== */

/** @brief Convert character index to GF table index (add 0x10). */
s32 func_801F738C(s32 a0) {
    return a0 + 0x10;
}

/** @brief Convert character index to GF table index + 1 (add 0x11). */
s32 func_801F7394(s32 a0) {
    return a0 + 0x11;
}

/**
 * @brief Render scrollable panel with footer.
 *
 * Sets up D_801FAB00 rendering params (icon 0x4A), then renders
 * the list, scroll indicator, and footer/help text.
 */
void func_801F739C(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    s32 base = (s32)&D_801FAB00;
    s32 ret1;
    s32 ret2;

    *(u8 *)(base + 0x10) = 0x4A;  /* iconType */
    *(u8 *)(base + 0x11) = 0;     /* iconSubType */
    *(s16 *)&D_801FAB00 = a2;     /* x */
    *(s16 *)(base + 4) = 0x9A;    /* w */
    *(s16 *)(base + 2) = a3;      /* y */
    *(s16 *)(base + 6) = 0x40;    /* h */
    *(u8 *)(base + 0x13) = 4;     /* columnCount */
    *(u8 *)(base + 0x1E) = a4;    /* itemId */

    ret1 = func_801F5F30(a0, a1, a2 + 0x24, a3, D_80083848, *(u8 *)(base + 0x16) /* pageStart */);
    ret2 = func_801F5F60(a0, ret1, D_80083848, 3);
    func_801EFBB4(a0, ret2, a5);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7454);

/** @brief Apply junction change, then update cursor and recalculate party stats. */
s32 func_801F75A4(s32 a0) {
    s32 result = func_801F7454(a0);
    func_801F1B4C(a0);
    func_801F5400(a0);
    return result;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F75EC);

/** @brief Remove junction, then update cursor and recalculate party stats. */
void func_801F76A8(s32 a0) {
    func_801F75EC(a0);
    func_801F1B4C(a0);
    func_801F5400(a0);
}

/**
 * @brief Handle left/right D-pad input for value adjustment.
 *
 * If right pressed (0x2000), increments; if left (0x8000), decrements.
 * Plays a sound effect if the value changed.
 */
s32 func_801F76E0(s32 flags, s32 a1, s32 a2) {
    s32 result = a2;
    s32 orig = a2;

    if (flags & 0x2000) {
        result = func_80035B28(a1, result);
    }
    if (flags & 0x8000) {
        result = func_80035B70(a1, orig);
    }
    if (result != orig) {
        func_80030D48(1);
    }
    return result;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F776C);

/**
 * @brief Find ability slot by ID in character data.
 *
 * Searches 19 ability slots in D_80077864 for value a1.
 * Returns the slot index if found, or a2 (default) if not.
 */
s32 func_801F77F8(s32 a0, s32 a1, s32 a2) {
    s32 offset = a0 * 152;
    u8 *ptr = (u8 *)((s32)D_80077864 + offset);
    s32 i;
    for (i = 0; i < 19; i++) {
        if (*ptr++ != a1) continue;
        return i;
    }
    return a2;
}

/** @brief Equip ability a1 to character a0's junction slot. */
void func_801F784C(s32 a0, s32 a1, s32 a2) {
    s32 ret;
    s32 base;
    ret = func_801F776C(a1, a2);
    if (ret) {
        ret = func_801F77F8(a0, a1, a2);
        base = (s32)D_80077378;
        *(u8 *)(base + a0 * 152 + ret + 0x4EC) = a1;
    }
    func_801F1B4C(a0);
}

/** @brief Remove ability a1 from character a0's junction slots. */
void func_801F78D8(s32 a0, s32 a1) {
    s32 offset = a0 * 152;
    u8 *ptr = (u8 *)((s32)D_80077864 + offset);
    s32 i;
    for (i = 0; i < 19; i++) {
        if (*ptr == a1) {
            *ptr = 0;
        }
        ptr++;
    }
}

/* ======================================================================== */
/* Misc/Utility                                                             */
/* ======================================================================== */

/** @brief Apply vibration config setting from D_80077E5C bit 1. */
void func_801F7928(void) {
    s32 val = D_80077E5C & 2;
    func_8001327C(val != 0);
}

/** @brief Apply ATB/screen brightness setting from D_80077E5C bit 6. */
void func_801F7954(void) {
    s32 a1 = 0;
    if (D_80077E5C & 0x40) {
        a1 = 0xFF;
    }
    func_80027EF8(0, a1);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F798C);

/** @brief Test config flag bits (returns D_80077E5F & a0). */
s32 func_801F79F8(s32 a0) {
    return D_80077E5F & a0;
}

/** @brief Update party-data flag bit 0x40 based on slot 0 status. */
void func_801F7A08(void) {
    if (func_80027EC8(0) == 0xFF) {
        s32 base = (s32)D_80077378;
        *(u16 *)(base + 0xAE4) |= 0x40;
    } else {
        s32 base = (s32)D_80077378;
        *(u16 *)(base + 0xAE4) &= 0xFFBF;
    }
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7A54);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7AD4);

/**
 * @brief Clear magic slots with zero quantity.
 *
 * Iterates 32 magic entries (stride 2) for character a0. If the
 * quantity byte is 0, clears the magic ID byte to 0.
 */
void func_801F7B10(s32 a0) {
    s32 offset = a0 * 152;
    s32 base = (s32)D_80077818;
    u8 *ptr = (u8 *)(base + offset);
    s32 i;
    for (i = 0; i < 32; i++) {
        if (ptr[1] == 0) {
            ptr[0] = 0;
        }
        ptr += 2;
    }
}

/**
 * @brief Sync magic IDs with quantities (bidirectional zero-clear).
 *
 * Iterates 198 entries in D_80077EBC. If quantity is 0, clears the
 * ID; if ID is 0, clears the quantity.
 */
void func_801F7B60(void) {
    u8 *a0 = D_80077EBC;
    s32 a2 = 0;
    u8 *v1 = a0 + 1;
    do {
        u8 b = *v1;
        u8 a = *a0;
        if (b == 0) {
            *a0 = 0;
        }
        if (a == 0) {
            *v1 = 0;
        }
        a2++;
        v1 += 2;
        a0 += 2;
    } while (a2 < 198);
}

/** @brief Convert 0-255 value to 0-100 percentage. */
s32 func_801F7BAC(s32 a0) {
    return a0 * 100 / 255;
}

/** @brief Identity function (returns a0 unchanged, compatibility placeholder). */
s32 func_801F7BE4(s32 a0) {
    return a0;
}

/** @brief Play toggle sound effect (sound 2 if bit 6 set, else sound 3). */
void func_801F7BEC(s32 a0) {
    if (a0 & 0x40) {
        func_80030D48(2);
    } else {
        func_80030D48(3);
    }
}

/**
 * @brief Expand compressed status flags to rendering icon bitmask.
 *
 * Maps individual status bits from a compact internal format to a
 * wider bitmask with spacing for display icon positioning.
 */
s32 func_801F7C20(s32 a0) {
    s32 result = a0 & 0x7FF;
    if (a0 & 0x800)   result |= 0x800;
    if (a0 & 0x2000)  result |= 0x1800;
    if (a0 & 0x4000)  result |= 0x7800;
    if (a0 & 0x1000)  result |= 0x8000;
    if (a0 & 0x8000)  result |= 0x18000;
    if (a0 & 0x10000) {
        s32 mask = 0x10000;
        result |= 0x8000;
        result |= mask;
        result |= 0x20000;
        result |= 0x40000;
    }
    return result;
}

/** @brief Render save/card-related text (convert value to string, draw at position). */
void func_801F7C98(s32 a0, s32 a1) {
    u8 buf[16];
    func_8002F294(a0, buf, 0x30);
    func_8002A2A8(a1, D_80056290);
    func_8002A2C4(a1, &buf[3]);
}
