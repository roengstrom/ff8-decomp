#include "common.h"

extern s16 D_801E7ABC;
extern u8 D_801E7ABE;
extern s32 D_801E7ACC;
extern u8 D_801E7ADC;
extern u8 D_801E69BC;
extern u8 D_801E71BC;
extern u8 D_801E79BC;
extern u16 D_801FA3C8;
extern u8 D_801FAB00;
extern u8 D_801FABD4;
extern s32 D_80083848;
extern u8 D_800780AB;
extern u8 D_80077378;
extern u32 D_801E69B8;

void func_801E5D74(s32 a0);

/**
 * Allocates a menu resource of type 0xD.
 * @param a0 Subtype parameter passed as third argument
 * @return Result from func_801F08D4
 */
s32 func_801E5800(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 0);
}

/**
 * Computes a scroll-adjusted Y position from D_801E79BC table entries and
 * D_801FA3C8 animation table, then calls func_801F0A34 to apply it.
 * @param a0 Display mode parameter (passed through to func_801F0A34)
 * @param a1 Menu state structure pointer
 * @note Non-matching: register allocation mismatch — compiler assigns t0/a2
 *       differently in the multiply chain (tblVal * 3 << 7 / 4096).
 *       Instruction count and structure match, but register numbers differ.
 * @code
 * void func_801E582C(s32 a0, s32 a1) {
 *     s32 base = (s32)&D_801E79BC;
 *     s32 idx = *(u8 *)(a1 + 0x2F);
 *     s16 t0 = *(s16 *)(base + idx * 8) + 0x22;
 *     s16 height = *(s16 *)(base + idx * 8 + 2) + 0x23;
 *     s16 scroll = *(s16 *)(a1 + 0x2A);
 *     s32 v;
 *     if (scroll < 0) scroll += 0x3F;
 *     v = *(u16 *)((s32)&D_801FA3C8 + (scroll >> 6) * 2);
 *     v = (v * 3) << 7;
 *     if (v < 0) v += 0xFFF;
 *     v >>= 12;
 *     func_801F0A34(a0, 0, t0 - v, height);
 * }
 * @endcode
 */
INCLUDE_ASM("asm/ovl/menutest/nonmatchings/menutest", func_801E582C);

/**
 * Decodes a string using func_801F7A54 key and func_8002F294/func_8002F2EC,
 * then copies it to D_801E7ADC via func_8002A2A8.
 */
void func_801E58B8(void) {
    u8 buf[0x20];
    u8 *p = buf;
    s32 v0;

    v0 = func_801F7A54();
    if (v0 == 0x1F) {
        v0 = func_801F6AFC(0x39);
        func_8002A2A8((s32)&D_801E7ADC, v0);
        return;
    }
    v0 = func_801F7A54();
    func_8002F294(v0, (s32)p, 1);
    func_8002F2EC((s32)p, 4, 1, 0x10);
    if (*p == 0x10) {
        s32 c = 0x10;
        do {
            p++;
        } while (*p == c);
    }
    {
        u8 *q = p;
        if (*p != 0) {
            do {
                v0 = func_80020F84(0xB);
                v0 = *(u8 *)(v0 + 1) + 0xFF;
                *q += v0;
                q++;
            } while (*q != 0);
        }
    }
    func_8002A2A8((s32)&D_801E7ADC, (s32)p);
}

/**
 * Text/string parser that processes encoded character data into display buffers.
 * Redirects GP to PS1 scratchpad (0x1F800300) for fast temporary storage,
 * then iterates over encoded input, handling control codes (0x00=end, 0x01=end,
 * 0x02=reset, 0x07=end, 0x0A=special decode, 0x0B=new line), character decoding
 * via func_8002F294/func_8002F2EC, and glyph width accumulation via func_8002E428.
 * @param a0 Pointer to encoded input string
 * @param a1 Output text buffer
 * @param a2 Output glyph position buffer (8 bytes per entry)
 * @return Number of lines processed (via $fp counter)
 * @note Non-decompilable: handwritten assembly with direct GP manipulation
 *       (`addi gp, gp, 0x80` / `addi gp, gp, -0x80`). The C compiler cannot
 *       produce GP register manipulation instructions.
 */
INCLUDE_ASM("asm/ovl/menutest/nonmatchings/menutest", func_801E59B4);

/**
 * Reads a string descriptor from a table and dispatches to func_801E59B4.
 * @param a0 Base pointer to string table
 * @param a1 Index into the table
 */
void func_801E5D18(s32 a0, s32 a1) {
    s32 offset;
    u8 code;

    offset = *(u16 *)(a0 + a1 * 2 + 2);
    code = *(u8 *)(a0 + offset);
    *(u8 *)&D_801E7ABE = code;
    *(s16 *)&D_801E7ABC = func_801E59B4(a0 + offset + 1, &D_801E69BC, &D_801E79BC);
}

/**
 * Menu test state machine — drives all menu navigation and display logic.
 * Uses a 28-case switch on *(u16 *)(a0 + 0x10) as state variable, with cases
 * handling: initial display (0), VSync wait (1), page transition checks (2),
 * resource allocation (3,6), fade-in animation (4,7,10,15), input polling (5,8,11),
 * text decode (9), scroll animation (13,14,19,20,25,26), page navigation (16,17),
 * confirmation (21,22), and cleanup/exit (27).
 * Reads input via func_801F0948/func_80035E00, updates scroll position at +0x24,
 * manages page index at +0x2D and +0x2E, and dispatches rendering via func_801E582C,
 * func_801E5D18, func_801E58B8, func_801F6800.
 * @param a0 Menu state structure pointer (s1=a0, s2=a0+0x10, s3/s0 from D_801FAB00)
 * @note Non-matching: scrambled prologue (s1,s2,ra,s3,s0) with s-reg assignment
 *       locked by graph coloring. Also contains a 28-entry jump table
 *       (jtbl_801E6948) requiring rodata placement.
 */
INCLUDE_ASM("asm/ovl/menutest/nonmatchings/menutest", func_801E5D74);

/**
 * Sets up a GPU frame with centered text display for the header area.
 * Measures text width via func_8002E680, centers it within 0xF4 pixels,
 * draws via func_8002EAD0, configures D_801FAB00 display struct
 * (0x18, 0x6, 0xF4, 0x16), and submits via func_801EF9AC.
 * @param a0 Display list pointer
 * @param a1 OT pointer
 * @return Result of func_801EF9AC
 * @note Non-matching: register asm("$16/$17/$18") gets correct s-reg assignment
 *       (text→s0, ot→s1, disp→s2) but the scheduler places the lui/addiu for
 *       &D_801E71BC (text init) BEFORE the ot save/init, while the original has
 *       ot save/init first. The prologue instruction order is s2→s0→s1 (compiled)
 *       vs s2→s1→s0 (original). All register numbers and body instructions match.
 * @code
 * s32 func_801E64B4(s32 a0, s32 a1) {
 *     register s32 disp asm("$18") = a0;
 *     register s32 ot asm("$17") = a1;
 *     register s32 text asm("$16") = (s32)&D_801E71BC;
 *     s32 maxW;
 *     s32 v0;
 *     s32 buf;
 *     v0 = func_8002E680(text);
 *     REGALLOC_BARRIER(v0);
 *     maxW = 0xF4;
 *     v0 = (maxW - v0) / 2;
 *     func_8002EAD0(disp, v0 + 0x18, 0xC, text);
 *     buf = (s32)&D_801FAB00;
 *     *(u8 *)(buf + 0x10) = 0;
 *     *(u8 *)(buf + 0x11) = 0;
 *     *(s16 *)&D_801FAB00 = 0x18;
 *     *(s16 *)(buf + 2) = 6;
 *     *(s16 *)(buf + 4) = maxW;
 *     *(s16 *)(buf + 6) = 0x16;
 *     return func_801EF9AC(disp, ot, 0x1000, D_80083848);
 * }
 * @endcode
 */
INCLUDE_ASM("asm/ovl/menutest/nonmatchings/menutest", func_801E64B4);

/**
 * Sets up GPU display for the scrollable text body area.
 * Computes scroll offset from D_801FA3C8 animation table (same pattern as
 * func_801E582C), draws text at scroll-adjusted Y position via func_8002EAD0,
 * then configures two D_801FAB00 display regions:
 * first (0x1C, 0x21, 0x148, 0x9F) submitted via func_801EF800,
 * second (0x18, 0x1D, 0x150, 0xA7) submitted via func_801EF9AC.
 * @param a0 Menu state structure
 * @param a1 Display list pointer
 * @param a2 OT pointer
 * @return Result of func_801EF9AC
 * @note Non-matching: scrambled prologue (s3,s0,s1,s4,ra,s2) with 6 callee-saved
 *       registers. register asm("$19/$16") gets disp→s3 and ot→s0 correct, and
 *       the compiler naturally assigns yPos→s1, buf→s2, %hi(FAB00)→s4. However,
 *       the prologue save ordering still differs, and the scroll computation has
 *       register differences: compiler uses $3 for sll result (original uses $2),
 *       and fills the bgez delay slot with sra (original has nop). Use s32 for
 *       scroll variable (not s16) to get correct lh+sra pattern.
 * @code
 * s32 func_801E6570(s32 a0, s32 a1, s32 a2) {
 *     register s32 disp asm("$19") = a1;
 *     register s32 ot asm("$16") = a2;
 *     s32 yPos = 0x22;
 *     s32 buf = (s32)&D_801FAB00;
 *     s32 scroll = *(s16 *)(a0 + 0x2A);
 *     s32 v0;
 *     if (scroll < 0) scroll += 0x3F;
 *     v0 = *(u16 *)((s32)&D_801FA3C8 + (scroll >> 6) * 2);
 *     v0 = (v0 * 3) << 7;
 *     if (v0 < 0) v0 += 0xFFF;
 *     yPos -= v0 >> 12;
 *     ot = func_801EF8D8(disp, ot);
 *     func_8002EAD0(disp, yPos, 0x23, (s32)&D_801E69BC);
 *     *(s16 *)&D_801FAB00 = 0x1C;
 *     *(s16 *)(buf + 2) = 0x21;
 *     *(s16 *)(buf + 4) = 0x148;
 *     *(s16 *)(buf + 6) = 0x9F;
 *     v0 = func_801EF800(disp, ot, buf);
 *     *(u8 *)(buf + 0x10) = 0;
 *     *(u8 *)(buf + 0x11) = 0;
 *     *(s16 *)&D_801FAB00 = 0x18;
 *     *(s16 *)(buf + 2) = 0x1D;
 *     *(s16 *)(buf + 4) = 0x150;
 *     *(s16 *)(buf + 6) = 0xA7;
 *     return func_801EF9AC(disp, v0, 0x1000, D_80083848);
 * }
 * @endcode
 */
INCLUDE_ASM("asm/ovl/menutest/nonmatchings/menutest", func_801E6570);

/**
 * Sets up GPU display for a secondary text area, similar to func_801E64B4.
 * Centers text from *(a0 + 0x20) within 0x150 pixels, draws via func_8002EAD0
 * at Y=0xC8, configures D_801FAB00 (0x18, 0xC4, 0x150, 0x14), and submits
 * via func_801EF9AC.
 * @param a0 Menu state structure
 * @param a1 Display list pointer
 * @param a2 OT pointer
 * @return Result of func_801EF9AC
 */
s32 func_801E66A8(s32 a0, s32 a1, s32 a2) {
    s32 state = a0;
    s32 disp = a1;
    s32 ot = a2;
    s32 maxW;
    s32 v0;
    s32 buf;

    v0 = func_8002E680(*(s32 *)(state + 0x20) + ot - ot);
    do { maxW = 0x150; v0 = (maxW - v0) / 2; break; } while (1);
    func_8002EAD0(disp, v0 + 0x18, 0xC8, *(s32 *)(state + 0x20));
    buf = (s32)&D_801FAB00;
    *(u8 *)(buf + 0x10) = 0;
    *(u8 *)(buf + 0x11) = 0;
    *(s16 *)&D_801FAB00 = 0x18;
    *(s16 *)(buf + 2) = 0xC4;
    *(s16 *)(buf + 4) = maxW;
    *(s16 *)(buf + 6) = 0x14;
    return func_801EF9AC(disp, ot, 0x1000, D_80083848);
}

/**
 * Sets up menu display rendering pipeline.
 * @param a0 Menu state structure
 * @param a1 Display buffer 1
 * @param a2 Display buffer 2
 * @return Final display buffer pointer from func_801F1B10
 */
s32 func_801E6760(s32 a0, s32 a1, s32 a2) {
    s32 v0;

    func_801F1AFC();
    func_8002E7C4(*(s16 *)(a0 + 0x24));
    func_80030058(*(s16 *)(a0 + 0x24));
    v0 = func_801E64B4(a1, a2);
    v0 = func_801E6570(a0, a1, v0);
    v0 = func_801E66A8(a0, a1, v0);
    func_801F1B10();
    return v0;
}

/**
 * Main entry point for the menu test overlay.
 * Initializes menu state, loads resources, and runs the main menu loop.
 */
void func_801E67F0(void) {
    s32 s0;
    s32 v0;

    s0 = func_801F179C((s32)func_801E5D74, (s32)func_801E6760);
    func_801F0948(0);
    func_801F5440();
    do {
        v0 = func_80035E00();
    } while (v0 != 0);
    if (s0 == 0) {
        return;
    }
    *(s16 *)(s0 + 0x26) = 0;
    func_801F1D34((s32)&D_801E69B8);
    func_801F1DB0(0);
    v0 = func_801E28D4();
    if (v0 == 0xFF) {
        *(u8 *)(s0 + 0x2C) = 0;
        *(u8 *)(s0 + 0x2D) = *(u8 *)&D_800780AB;
        v0 = func_801E5800(0x11);
        func_801E59B4(v0, (s32)&D_801E71BC, (s32)&D_801E79BC);
        v0 = func_801E5800(0x1A);
    } else {
        *(u8 *)(s0 + 0x2C) = 1;
        v0 = func_801E28D4();
        *(u8 *)(s0 + 0x2D) = v0;
        *(s32 *)&D_801E7ACC = *(u8 *)(s0 + 0x2D);
        *(s32 *)((s32)&D_801E7ACC + 4) = *(u8 *)(s0 + 0x2D) + 1;
        v0 = func_801E5800(0x16);
        func_801E59B4(v0, (s32)&D_801E71BC, (s32)&D_801E79BC);
        v0 = func_801E5800(0x1B);
    }
    *(s32 *)(s0 + 0x20) = v0;
    *(u8 *)(s0 + 0x2E) = 0;
    *(s16 *)(s0 + 0x26) = 0;
    *(u8 *)&D_801E69BC = 0;
    if (*(u8 *)(s0 + 0x2D) < 0x1E) {
        func_800360D0(*(u8 *)(s0 + 0x2D) + 0x60, 0x801D1000);
    }
    *(s16 *)(s0 + 0x2A) = 0;
    func_801E5D74(s0);
}
