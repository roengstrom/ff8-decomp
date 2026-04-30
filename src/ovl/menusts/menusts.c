#include "common.h"

extern u16 D_801FA3C8[];
extern u8 g_menuDisplayCfg[];
extern s32 g_menuColor;
extern s32 D_801E961C[];

/** @brief Look up value from D_801FA3C8 table by dividing input by 64. */
u16 func_801E5800(s32 a0) {
    return D_801FA3C8[a0 / 64];
}

/**
 * @brief Look up status display type byte.
 *
 * Indexes into D_801E9964 (8-byte stride) and returns the byte
 * at offset 4.
 *
 * @param a0 Status entry index.
 * @return Type byte.
 */
INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E582C);

/**
 * @brief Compute status display x-position.
 *
 * Indexes into D_801E9964 (8-byte stride), loads halfword at offset 0,
 * and adds D_801E99AC base.
 *
 * @param a0 Status entry index.
 * @return Computed address.
 */
INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E5848);

/**
 * @brief Compute status display y-position.
 *
 * Indexes into D_801E9964 (8-byte stride), loads halfword at offset 2,
 * and adds D_801E99AC base.
 *
 * @param a0 Status entry index.
 * @return Computed address.
 */
INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E586C);

/**
 * @brief Wrapper that calls getMagicNamePtr with a0 offset by 0x33.
 *
 * @param a0 Base pointer to a menu entry structure.
 */
void func_801E5890(u8 *a0) {
    getMagicNamePtr(a0 + 0x33);
}

/**
 * @brief Wrapper that calls getSpellEntityData with a0 offset by 0x33.
 *
 * @param a0 Base pointer to a menu entry structure.
 */
void func_801E58B0(u8 *a0) {
    getSpellEntityData(a0 + 0x33);
}

/** @brief Draw inner panel with section id 0x7 and clear flag. */
s32 func_801E58D0(s32 a0) {
    return func_801F08D4(1, 7, a0, 0);
}

/** @brief Draw inner panel with section id 0x7 and set flag. */
s32 func_801E58FC(s32 a0) {
    return func_801F08D4(1, 7, a0, 1);
}

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E5928);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E5D28);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E61B0);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E6254);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E62A8);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E68A4);

/**
 * @brief Configure and draw a status menu panel border.
 *
 * Sets up g_menuDisplayCfg with no icon, position from a2/a3,
 * fixed size (0xF4 x 0x12), then draws via func_801EF9AC.
 *
 * @param a0 Display list pointer
 * @param a1 OT pointer
 * @param a2 X position
 * @param a3 Y position
 */
void func_801E6994(s32 a0, s32 a1, s16 a2, s16 a3) {

    s32 cfg = (s32)g_menuDisplayCfg;

    *(u8 *)(cfg + 0x10) = 0;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)cfg = a2;
    *(s16 *)(cfg + 0x4) = 0xF4;
    *(s16 *)(cfg + 0x6) = 0x12;
    *(s16 *)(cfg + 0x2) = a3;
    func_801EF9AC(a0, a1, 0x1000, g_menuColor);
}

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E69E4);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E6B84);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E6C68);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E6EA0);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E709C);

/** @brief Build status bitfield from character data flags. */
s32 func_801E7278(u8 *a0) {
    s32 result = *(u16 *)(a0 + 0x1B4) & 0x7F;
    s32 flags = *(s32 *)(a0 + 0x18C);

    if (flags & 0x1) result |= 0x80;
    if (flags & 0x4) result |= 0x100;
    if (flags & 0x8) result |= 0x200;
    if (flags & 0x200) result |= 0x400;
    if (flags & 0x4000) result |= 0x800;
    if (flags & 0x8000) result |= 0x1000;
    return result;
}

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E72D8);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E750C);

/**
 * @brief Compute centered X position after subtracting rendered width.
 * @param a0 Base X position
 * @param a1 Total available width
 * @param a2 Text or item to measure via getGlyphStatusU16
 * @return Base X + (available width - rendered width) / 2
 */
s32 func_801E7644(s32 a0, s32 a1, s32 a2) {
    s32 rendered = getGlyphStatusU16(a2);
    return a0 + (a1 - rendered) / 2;
}

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E7690);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E7B20);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E7D04);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E811C);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E81B8);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8598);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E86F0);

/**
 * @brief Status menu render wrapper with stat type 6.
 * @param a0 Unused parameter
 * @param a1 First render parameter
 * @param a2 Second render parameter
 * @param a3 Third render parameter
 * @param arg5 Fourth render parameter
 * @param arg6 Fifth render parameter
 */
void func_801E8850(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {
    func_801E750C(a1, a2, a3, arg5, arg6, 6);
}

/**
 * @brief Status menu render wrapper with stat type 7.
 * @param a0 Unused parameter
 * @param a1 First render parameter
 * @param a2 Second render parameter
 * @param a3 Third render parameter
 * @param arg5 Fourth render parameter
 * @param arg6 Fifth render parameter
 */
void func_801E8890(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {
    func_801E750C(a1, a2, a3, arg5, arg6, 7);
}

/**
 * @brief Status menu render wrapper with stat type 8.
 * @param a0 Unused parameter
 * @param a1 First render parameter
 * @param a2 Second render parameter
 * @param a3 Third render parameter
 * @param arg5 Fourth render parameter
 * @param arg6 Fifth render parameter
 */
void func_801E88D0(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {
    func_801E750C(a1, a2, a3, arg5, arg6, 8);
}

/**
 * @brief Status menu render wrapper with stat type 9.
 * @param a0 Unused parameter
 * @param a1 First render parameter
 * @param a2 Second render parameter
 * @param a3 Third render parameter
 * @param arg5 Fourth render parameter
 * @param arg6 Fifth render parameter
 */
void func_801E8910(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {
    func_801E750C(a1, a2, a3, arg5, arg6, 9);
}

/**
 * @brief Status menu render wrapper with stat type 0xA.
 * @param a0 Unused parameter
 * @param a1 First render parameter
 * @param a2 Second render parameter
 * @param a3 Third render parameter
 * @param arg5 Fourth render parameter
 * @param arg6 Fifth render parameter
 */
void func_801E8950(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {
    func_801E750C(a1, a2, a3, arg5, arg6, 0xA);
}

/**
 * @brief Dispatch a status rendering operation through a function table.
 *
 * Looks up a function pointer from D_801E961C[a1], calls it with
 * (a0, a2, a3, stackArg1, stackArg2), then passes the result and
 * other parameters to func_801EF9AC for GPU display.
 *
 * @param a0 Status entry data pointer
 * @param a1 Function table index
 * @param a2 Parameter passed as a1 to table function, then as a0 to func_801EF9AC
 * @param a3 Parameter passed as a2 to table function
 * @param stackArg1 Passed as a3 to table function, then as stack arg to func_801EF9AC
 * @param stackArg2 Passed as stack arg to table function, then as a2 to func_801EF9AC
 */
void func_801E8990(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6, s32 arg7) {
    s32 (*fn)(s32, s32, s32, s32, s32, s32) = (void *)D_801E961C[a1];
    s32 result = fn(a0, a2, a3, arg5, arg6, arg7);
    func_801EF9AC(a2, result, arg7, g_menuColor);
}

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8A08);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8BAC);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8CE4);

/**
 * @brief Conditionally transform a value before passing to the status renderer.
 *
 * If the signed byte at a0+0x3B is non-negative, calls func_801F6FE4
 * with (a1, a2, 1, 0x1000) and uses the result as a2. Then calls
 * func_801E8CE4 with the original a0, a1, and the (possibly modified) a2.
 *
 * @param a0 Pointer to status entry structure.
 * @param a1 First render parameter.
 * @param a2 Second render parameter (may be replaced by func_801F6FE4 result).
 */
void func_801E9118(u8 *a0, s32 a1, s32 a2) {
    s8 val = *(s8 *)(a0 + 0x3B);
    if (val >= 0) {
        a2 = func_801F6FE4(a1, a2, 1, 0x1000);
    }
    func_801E8CE4(a0, a1, a2);
}

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E9178);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E923C);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E936C);
