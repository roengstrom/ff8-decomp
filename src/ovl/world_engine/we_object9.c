#include "common.h"
#include "world.h"

/* 4-byte slot read as either a full word or just the low halfword */
typedef union {
    u32 word;
    u16 half;
} AngleSlot;

/* 16-byte position descriptor (first 12 bytes = vec3 used by spawner) */
typedef struct {
    u32       pad0;
    AngleSlot pos;
    u32       pad8;
    u32       padC;
} PosDesc;

/* 8-byte unaligned velocity (copied via lwl/lwr to spawner's slot[0x18..0x1F]) */
typedef struct {
    u16 pad0;
    u16 angle;
    u16 height;
    u16 pad6;
} Velocity;

/* Particle source: position + velocity (with 4-byte gap between). Stride 0x28. */
typedef struct {
    PosDesc  pos;
    u32      pad10;
    Velocity vel;
} ParticleSource;

extern AngleSlot D_800C97F4;
extern s32       D_800C4D4C;
extern s32       D_800C5B50;
extern SlotEntry D_800DBFB8[];

extern s32 func_8009CC3C(void);
extern s32 func_800AC0A0(s32 type, PosDesc *pos, Velocity *vel, s32 flags);

#define SPAWN_FLAG_LIFETIME_JITTER 1
#define SPAWN_FLAG_SIZE_JITTER     2

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BA870);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BAC84);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BB150);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BB4E8);

/**
 * @brief Spawn a mirrored particle pair (type 0x13) at ±0x320 angle offsets if visible.
 *
 * Reads world camera angle from D_800C97F4 and computes angular delta dx to the
 * source's position.half. If (rnd + dx/4) < visRange AND |dx| < 0x258, emits two
 * particles via func_800AC0A0 with symmetric angle offsets and per-axis RNG jitter.
 * Overwrites the local copy's 4-byte pos slot with the current world camera word
 * before each spawn, so emitted particles share the world's current frame.
 *
 * @param src Particle source (position descriptor + velocity template).
 * @param visRange Visibility threshold (higher = farther allowed).
 */
void func_800BBD74(ParticleSource *src, s32 visRange) {
    Velocity localVel;
    PosDesc  localPos;
    s32      jitter;
    s32      absdx;
    s16      dx;
    u32      worldCam;

    worldCam = D_800C97F4.word;
    localPos = src->pos;
    dx       = D_800C97F4.half - src->pos.pos.half;
    localPos.pos.word = worldCam;
    if ((func_8009CC3C() + (dx >> 2)) >= visRange) return;
    absdx = (dx >= 0) ? dx : -dx;
    if (absdx >= 0x258) return;

    /* Left-side spawn: +0x320 angle offset + RNG jitter on angle/height */
    localVel = src->vel;
    localVel.angle += 0x320;
    jitter = (func_8009CC3C() - 0x80) * 8;
    localVel.angle += jitter;
    jitter = (func_8009CC3C() - 0x80) * 4;
    localVel.height += jitter;
    func_800AC0A0(0x13, &localPos, &localVel,
                  SPAWN_FLAG_SIZE_JITTER | SPAWN_FLAG_LIFETIME_JITTER);

    /* Right-side spawn: -0x320 angle offset + RNG jitter on angle/height */
    localVel = src->vel;
    localVel.angle -= 0x320;
    jitter = (func_8009CC3C() - 0x80) * 8;
    localVel.angle += jitter;
    jitter = (func_8009CC3C() - 0x80) * 4;
    localVel.height += jitter;
    func_800AC0A0(0x13, &localPos, &localVel,
                  SPAWN_FLAG_SIZE_JITTER | SPAWN_FLAG_LIFETIME_JITTER);
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BBF0C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC09C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC218);

extern void func_800BFFEC(void);

/**
 * @brief Thin wrapper that forwards to func_800BFFEC with no arguments.
 *
 * @note Purpose uncertain — appears to be a dispatch hook into another
 *       subsystem, possibly for resetting or (re)initializing state
 *       owned by func_800BFFEC's module.
 */
void func_800BC44C(void) {
    func_800BFFEC();
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC46C);

/** Copies and negates vector components (rotation variant A). */
void func_800BC51C(s32 *src, s32 *dst) {
    dst[0] = src[0];
    dst[1] = -src[2];
    dst[2] = src[1];
}

/** Copies and negates vector components (rotation variant B). */
void func_800BC544(s32 *src, s32 *dst) {
    dst[0] = src[0];
    dst[1] = src[2];
    dst[2] = -src[1];
}

/** Two-word state blob assembled from @c D_800C9718 / @c D_800C9710. */
typedef struct {
    s32 a;
    s32 b;
} U64;

extern s32 D_800C9718;
extern s32 D_800C9710;
extern s32 D_800C96CC;

extern void func_800C2C00(s32 a, s32 b, s32 c, s32 d, s32 e, U64 *f, s32 g, U64 *h);

/**
 * @brief Dispatch to @c func_800C2C00 with the current two-word state blob.
 *
 * Packs @c D_800C9718 / @c D_800C9710 into a pair of 8-byte buffers on the
 * caller stack; forwards @p arg1..@p arg3 alongside @c D_800C96CC and the
 * magic @c 0x80122000 command code. The same pointer is passed as @c arg6
 * and @c arg8, with @c 0 as @c arg7 — @c func_800C2C00 reads both slots.
 */
void func_800BC570(s32 arg1, s32 arg2, s32 arg3) {
    U64 buf;
    U64 tmp;

    tmp.a = D_800C9718;
    tmp.b = D_800C9710;
    buf = tmp;

    func_800C2C00(arg3, D_800C96CC, 0x80122000, arg2, arg1, &buf, 0, &buf);
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC5E0);

/**
 * @brief Linear scan of @c D_800DBFB8 starting at @p start for a slot whose
 *        @c marker equals @p key.
 *
 * Returns the matching slot index on hit, or -1 on miss / when @p start is
 * already past @c D_800C5B50.
 *
 * @param key Marker byte value to search for.
 * @param start First index to scan.
 * @return Matching slot index, or -1 on no match.
 */
s32 func_800BC688(s32 key, s32 start) {
    s32 i;

    for (i = start; i < D_800C5B50; i++) {
        if (key == D_800DBFB8[i].marker) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Two back-to-back offset arrays at the start of the string-table blob
 *        @c D_800C97D4 points to. Each entry in @c first / @c second is a
 *        byte offset (from the table base) to a null-terminated string stored
 *        later in the same blob.
 */
typedef struct {
    s32 first[30];
    s32 second[1];
} StringTable;

extern StringTable *D_800C97D4;

/**
 * @brief Copy a null-terminated string from the first offset table to @p dst.
 *
 * Looks up the byte offset for string @p idx in @c table->first, computes the
 * string's address (@c table + offset), and copies the null-terminated bytes
 * into @p dst.
 *
 * @param dst Destination buffer (must have room for the string + null).
 * @param idx Index into @c table->first.
 * @return @p dst (unchanged).
 */
u8 *func_800BC6E8(u8 *dst, s32 idx) {
    StringTable *table = D_800C97D4;
    s32 *offsets = table->first;
    u8 *src = (u8 *)table + offsets[idx];
    s32 i;

    i = 0;
    while ((dst[i] = src[i]) != 0) {
        i++;
    }
    return dst;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC744);

/**
 * @brief Copy a null-terminated string from the second offset table to @p dst.
 *
 * Variant of @c func_800BC6E8 that indexes @c table->second (the second
 * offset array, starting 0x78 bytes into the blob) instead of @c first.
 */
u8 *func_800BC7D0(u8 *dst, s32 idx) {
    StringTable *table = D_800C97D4;
    u8 *src = (u8 *)table + *(table->second + idx);
    s32 i;

    i = 0;
    while ((dst[i] = src[i]) != 0) {
        i++;
    }
    return dst;
}

/**
 * @brief Append a string from the second offset table to @p dst (strcat variant).
 *
 * Computes the length of the existing null-terminated string in @p dst, looks
 * up the source string at @c table->second[idx], and appends it (including the
 * terminating null) to @p dst.
 *
 * @param dst Destination buffer with an existing null-terminated string.
 * @param idx Index into @c table->second.
 * @return @p dst (unchanged).
 */
u8 *func_800BC82C(u8 *dst, s32 idx) {
    u8 new_var;
    StringTable *table = D_800C97D4;
    u8 *src = (u8 *)table + *(table->second + idx);
    s32 len;
    s32 i;
    u8 c;

    len = 0;
    if (dst[0] != 0) {
        do {
            len++;
        } while (dst[len] != 0);
    }

    c = src[0];
    dst[len] = c;
    new_var = c != 0;
    if (c != 0) {
        i = 0;
        do {
            len++;
            i++;
            new_var = src[i];
            c = new_var;
            dst[len] = c;
        } while (new_var);
    }
    return dst;
}

extern u8 *func_800BC8D8(u8 *buf, s32 magicId);

/**
 * @brief Zero-terminate buf and fill it with a magic spell's display name.
 *
 * Clears the first byte of @p buf before delegating to func_800BC8D8, which
 * resolves the magic name (via getMagicNamePtr) and copies glyphs into @p buf.
 *
 * @param buf Destination string buffer; first byte is zeroed before filling.
 * @param magicId Magic spell ID whose name to look up.
 */
void func_800BC8B8(u8 *buf, s32 magicId) {
    buf[0] = 0;
    func_800BC8D8(buf, magicId);
}

/**
 * @brief Append a magic-name string to @p dst (strcat variant).
 *
 * Looks up the magic name pointer via @c getMagicNamePtr and appends it
 * (including terminating null) to the existing null-terminated string in
 * @p dst.
 *
 * @param dst     Destination buffer with an existing null-terminated string.
 * @param magicId Magic ID to look up via @c getMagicNamePtr.
 * @return @p dst (unchanged).
 */
u8 *func_800BC8D8(u8 *dst, s32 magicId) {
    u8 new_var;
    u8 *src = getMagicNamePtr(magicId);
    s32 len;
    s32 i;
    u8 c;

    len = 0;
    if (dst[0] != 0) {
        do {
            len++;
        } while (dst[len] != 0);
    }

    c = src[0];
    dst[len] = c;
    new_var = c != 0;
    i = 0;
    if (c != 0) {
        do {
            len++;
            i++;
            new_var = src[i];
            c = new_var;
            dst[len] = c;
        } while (new_var);
        len--;
    }
    return dst;
}

/**
 * @brief Append the decimal string of @p num to @p dst.
 *
 * Converts @p num to its decimal digits, looks up the digit-0 character from
 * @c table->first[29] (the game's digit-character base), and appends the
 * digits (most significant first) to the null-terminated string in @p dst.
 * Null-terminates the result.
 *
 * @param dst Destination buffer with an existing null-terminated string.
 * @param num Integer to convert to decimal.
 * @return @p dst (unchanged).
 */
u8 *func_800BC974(u8 *dst, s32 num) {
    StringTable *table = D_800C97D4;
    u8 *digitBase = (u8 *)table + table->first[29];
    u8 digits[8];
    s32 len;
    s32 count;

    len = 0;
    if (dst[0] != 0) {
        do {
            len++;
        } while (dst[len] != 0);
    }

    count = 0;
    if (num != 0) {
        do {
            digits[count++] = num % 10;
            num /= 10;
        } while (num != 0);
    }

    if (count == 0) {
        digits[0] = 0;
        count = 1;
    }

    for (count--; count >= 0; count--, len++) {
        dst[len] = digitBase[0] + digits[count];
    }

    dst[len] = 0;
    return dst;
}

extern u8 *func_800BC974(u8 *buf, s32 id);

/**
 * @brief Zero-terminate buf and fill it with a name string via func_800BC974.
 *
 * Mirrors func_800BC8B8 but forwards to a different name resolver
 * (func_800BC974 reads from the table referenced by D_800C97D4).
 *
 * @param buf Destination string buffer; first byte is zeroed before filling.
 * @param id Identifier whose name to look up.
 */
void func_800BCA54(u8 *buf, s32 id) {
    buf[0] = 0;
    func_800BC974(buf, id);
}

/**
 * @brief Second relocatable offset-table pointing at template strings.
 *
 * Same layout as @c StringTable but used by @c func_800BCA74 for template
 * format strings containing @c 0x0A marker bytes.
 */
typedef struct {
    s32 offsets[1];
} TemplateTable;

extern TemplateTable *D_800C9FE8;

extern void func_80047C74(u8 *dst, u8 *src);

/**
 * @brief Format a template string with up to four parameter substitutions.
 *
 * Fetches the template at @c D_800C9FE8 index @p idx and walks it character
 * by character. Each byte that isn't an @c 0x0A marker is looked up in the
 * @c D_800C97D4 character table and appended to @p dst. The two-byte sequence
 * @c 0x0A/0xFF terminates the template. When the byte following an @c 0x0A
 * is in the range @c 0x20..0x23, the corresponding parameter
 * (@p p1 / @p p2 / @p p3 / @p p4) is appended via @c func_80047C74.
 *
 * @param dst Destination buffer.
 * @param idx Template index into @c D_800C9FE8.
 * @param p1  Substitution string for marker @c 0x0A @c 0x20.
 * @param p2  Substitution string for marker @c 0x0A @c 0x21.
 * @param p3  Substitution string for marker @c 0x0A @c 0x22.
 * @param p4  Substitution string for marker @c 0x0A @c 0x23.
 * @return @p dst (unchanged).
 */
u8 *func_800BCA74(u8 *dst, s32 idx, u8 *p1, u8 *p2, u8 *p3, u8 *p4) {
    TemplateTable *tpl = D_800C9FE8;
    s32 *toff = (s32 *)tpl;
    u8 *src = (u8 *)tpl + toff[idx];
    s32 i = 0;

    while (src[i] != 0x0A || src[i + 1] != 0xFF) {
        if (src[i] == 0x0A) {
            i++;
            if (src[i] >= 0x20) {
                s32 code = src[i] - 0x20;
                if (code == 0) {
                    if (p1 != 0) func_80047C74(dst, p1);
                } else if (code == 1) {
                    if (p2 != 0) func_80047C74(dst, p2);
                } else if (code == 2) {
                    if (p3 != 0) func_80047C74(dst, p3);
                } else if (code == 3) {
                    if (p4 != 0) func_80047C74(dst, p4);
                }
            }
        } else {
            u8 c = src[i];
            StringTable *chtable = D_800C97D4;
            s32 *offsets = chtable->first;
            u8 *s = (u8 *)chtable + offsets[c];
            s32 len = 0;
            s32 j;
            u8 ch;
            if (dst[0] != 0) {
                len = 1;
                len--;
                do {
                    len++;
                } while (dst[len] != 0);
            }
            j = 0;
            do {
                ch = s[j];
                dst[len] = s[j];
                if (ch == 0) break;
                len++;
                j++;
            } while (1);
        }
        i++;
    }
    return dst;
}

/**
 * @brief Variant of @c func_800BCA74 that clears @c dst[0] before formatting.
 *
 * Identical to @c func_800BCA74 except it writes @c '\0' to @p dst before
 * walking the template, so the destination starts empty regardless of any
 * prior contents.
 */
u8 *func_800BCC70(u8 *dst, s32 idx, u8 *p1, u8 *p2, u8 *p3, u8 *p4) {
    TemplateTable *tpl;
    s32 *toff;
    u8 *src;
    s32 i = 0;

    dst[0] = 0;
    tpl = D_800C9FE8;
    toff = (s32 *)tpl;
    src = (u8 *)tpl + toff[idx];

    while (src[i] != 0x0A || src[i + 1] != 0xFF) {
        if (src[i] == 0x0A) {
            i++;
            if (src[i] >= 0x20) {
                s32 code = src[i] - 0x20;
                if (code == 0) {
                    if (p1 != 0) func_80047C74(dst, p1);
                } else if (code == 1) {
                    if (p2 != 0) func_80047C74(dst, p2);
                } else if (code == 2) {
                    if (p3 != 0) func_80047C74(dst, p3);
                } else if (code == 3) {
                    if (p4 != 0) func_80047C74(dst, p4);
                }
            }
        } else {
            u8 c = src[i];
            StringTable *chtable = D_800C97D4;
            s32 *offsets = chtable->first;
            u8 *s = (u8 *)chtable + offsets[c];
            s32 len = 0;
            s32 j;
            u8 ch;
            if (dst[0] != 0) {
                len = 1;
                len--;
                do {
                    len++;
                } while (dst[len] != 0);
            }
            j = 0;
            do {
                ch = s[j];
                dst[len] = s[j];
                if (ch == 0) break;
                len++;
                j++;
            } while (1);
        }
        i++;
    }
    return dst;
}

extern u8 *getStatName(s32 statId);

/**
 * @brief Append a stat-name string to @p dst (strcat variant).
 *
 * Looks up the stat name via @c getStatName and appends it (including the
 * terminating null) to the existing null-terminated string in @p dst.
 *
 * @param dst    Destination buffer with an existing null-terminated string.
 * @param statId Stat ID passed to @c getStatName.
 * @return @p dst (unchanged).
 */
u8 *func_800BCE74(u8 *dst, s32 statId) {
    u8 new_var;
    u8 *src = getStatName(statId);
    s32 len;
    s32 i;
    u8 c;

    len = 0;
    if (dst[0] != 0) {
        do {
            len++;
        } while (dst[len] != 0);
    }

    c = src[0];
    dst[len] = c;
    new_var = c != 0;
    i = 0;
    if (c != 0) {
        do {
            len++;
            i++;
            new_var = src[i];
            c = new_var;
            dst[len] = c;
        } while (new_var);
        len--;
    }
    return dst;
}

extern u8 *func_800BCE74(u8 *buf, s32 statId);

/**
 * @brief Zero-terminate buf and fill it with a stat name via func_800BCE74.
 *
 * Mirrors func_800BC8B8 / func_800BCA54 but targets the stat-name resolver
 * (func_800BCE74 dispatches to getStatName internally).
 *
 * @param buf Destination string buffer; first byte is zeroed before filling.
 * @param statId Stat identifier whose name to look up.
 */
void func_800BCF10(u8 *buf, s32 statId) {
    buf[0] = 0;
    func_800BCE74(buf, statId);
}

/** @brief 3D vector used by func_800BCF30's angle-to-position conversion. */
typedef struct { s32 x, y, z; } Vec3;

/**
 * @brief Convert a 16-bit angle into a Vec3 position on a 128x48 tile grid.
 *
 * Treats @p angle as a signed 16-bit value and splits it into a section
 * index (angle / 128) and an offset within the section (angle % 128).
 * Writes the resulting world-space coordinates to @p out:
 *   - @c x = (offset - 0x40) * 2048     (centered, shifted by 11 bits)
 *   - @c z = (0x30 - section) * 2048
 *   - @c y = 0                          (flat on the XZ plane)
 *
 * Returns silently when @p out is NULL.
 *
 * @note @c angle is declared s32 (not s16) and the @c section=angle prelude
 *       is deliberate — it steers gcc's register allocation to match the
 *       target's @c v1/@c a0 pattern.
 *
 * @param angle s16-style angle (low 16 bits are significant).
 * @param out Output position vector; no-op when NULL.
 */
void func_800BCF30(s32 angle, Vec3 *out) {
    s32 section;
    s32 a;
    s16 offset;
    section = angle;
    if (out != 0) {
        a = (s16)section;
        section = a / 128;
        offset = a - section * 128;
        out->x = (offset - 0x40) << 11;
        out->z = (0x30 - section) << 11;
        out->y = 0;
    }
}

/** Lookup table entry: matches an (id, param) pair. */
typedef struct {
    s16 id;
    u8 param;
    u8 pad3;
} LookupEntry;

/** Range into the lookup-entry pool (offsets from blob base). */
typedef struct {
    s32 start_off;
    s32 end_off;
} SubRange;

/** Scatter table: 5-bit scatter key -> up to 4 sub-range indices. */
typedef struct {
    s8 ranges[4];
} ScatterEntry;

/** Lookup blob at @c D_800C96F8: range directory, entry-pool offset, then pool. */
typedef struct {
    SubRange ranges[5];       /* 0x00: table ranges, indexed by scatter entry */
    s32      entry_base_off;  /* 0x28: offset from blob base to first pool entry */
} LookupBlob;

extern u8 D_800D2440;                 /* scatter key (5 bits) */
extern ScatterEntry D_800C5D70[];     /* scatter table, 32 entries */
extern LookupBlob *D_800C96F8;        /* pointer to lookup blob */

/**
 * @brief Look up an (id, param) pair in a scattered multi-subtable index.
 *
 * Uses the 5-bit scatter key at @c D_800D2440 to select up to 4 sub-ranges
 * from @c D_800C5D70. For each non-negative sub-range index, searches the
 * corresponding entry pool for a matching (id, param) pair.
 *
 * @param id Entry id to match (compared as halfword).
 * @param param Entry param byte to match.
 * @return Entry index (offset from pool base, divided by entry stride) on match,
 *         or -1 if no matching entry was found in any scanned sub-range.
 */
s32 func_800BCF84(s32 id, s32 param) {
    s8 *scatter = D_800C5D70[D_800D2440 & 0x1F].ranges;
    LookupBlob *blob = D_800C96F8;
    u8 *pool_base;
    u8 *entry_base = (u8 *)blob + blob->entry_base_off;
    s32 i;
    s32 t;
    LookupEntry *e;
    LookupEntry *e_end;

    pool_base = (u8 *)blob;
    for (i = 0; i < 4; i++) {
        t = scatter[i];
        if (t < 0) break;
        e = (LookupEntry *)(pool_base + (&blob->ranges[t])->start_off);
        e_end = (LookupEntry *)(pool_base + (&blob->ranges[t])->end_off);
        while (e < e_end) {
            if (id == e->id && param == e->param) {
                return ((u8 *)e - entry_base) >> 2;
            }
            e++;
        }
    }
    return -1;
}

extern u32 D_80077E84;

/** Checks if a counter has reached threshold 0xBB8. */
s32 func_800BD040(void) {
    return D_80077E84 >= 0xBB8;
}

/**
 * @brief Subtract a clamped value from the HP-like counter at g_gameState+0xB0C.
 */
s32 func_800BD058(s32 amount) {
    extern u8 g_gameState[];
    s32 base = (s32)g_gameState;
    s32 max;

    if (amount >= 0) {
        max = *(s32 *)(base + 0xB0C);
        if ((u32)max < (u32)amount) {
            amount = max;
        }
    } else {
        amount = 0;
    }
    *(s32 *)(base + 0xB0C) = *(s32 *)(base + 0xB0C) - amount;
    return amount;
}

extern s32 D_800C4D38;

extern s32 func_800A4670(u32 a, s32 b);
extern s32 func_800A358C(s32 a, SlotEntry *b, u8 *c, s32 d);
extern s32 func_800B00D8(s32 a);

/**
 * @brief Test if a candidate slot fits the current command and insert it.
 *
 * Rejects the slot early when its @c angle is more than 0xC8 away from the
 * world camera @p worldAngle. Then requires both cmd-tag checks (against
 * @p arg1 and @c D_800C4D38) to pass via @c func_800A4670. On all checks
 * passing, dispatches @c func_800B00D8 on the slot's marker and forwards
 * the result to @c func_800A358C for insertion. Returns 1 when insertion
 * reports "not already present" (negative), 0 otherwise.
 *
 * @note Purpose uncertain — the return semantic is inverted from the
 * surrounding early-exit 0s, suggesting @c func_800A358C returns a
 * negative sentinel for "new".
 *
 * @param slot Candidate slot entry (read-only in this function).
 * @param arg1 First tag selector passed to the cmd check.
 * @param cmd Command descriptor (type/flag/param packed into 24-bit key).
 * @param worldAngle Current world camera angle for proximity test.
 * @return 1 if the slot was newly inserted, 0 otherwise or on rejection.
 */
s32 func_800BD09C(SlotEntry *slot, s32 arg1, CmdDesc *cmd, s32 worldAngle) {
    s32 delta = slot->y - worldAngle;
    s32 insertResult;
    s32 trigger;

    if (delta > 0) {
        if (delta >= 0xC8) return 0;
    } else if (worldAngle - slot->y >= 0xC8) {
        return 0;
    }

    trigger = 1;
    if (func_800A4670(cmd->type | (cmd->flag << 8) | (cmd->param << 16), arg1) == 0) {
        return 0;
    }
    if (func_800A4670(cmd->type | (cmd->flag << 8) | (cmd->param << 16), D_800C4D38) == 0) {
        if (trigger) return 0;
    }
    insertResult = func_800A358C(func_800B00D8(slot->marker), slot, slot->data14, trigger);
    if (insertResult < 0) return trigger;
    return 0;
}

/**
 * @brief World-space 3-vector used for translation/position state.
 */
typedef struct {
    s32 x;             /* 0x00 */
    s32 y;             /* 0x04: source z, negated (PS1 y-down convention) */
    s32 z;             /* 0x08: source y */
} WorldPos;

/**
 * @brief Source-side 16-byte vector blob at @c D_800DD680.
 */
typedef struct {
    s32 x;             /* 0x00 */
    s32 y;             /* 0x04 */
    s32 z;             /* 0x08 */
    s32 pad0C;         /* 0x0C */
} WorldVec;

extern s32      D_800C4DC8;
extern s32      D_800C4D38;
extern s32      D_800C4D3C;
extern u8       D_800C9770[0x10];
extern WorldPos D_800C9868;
extern WorldVec D_800DD680;
extern u8       D_800DD690[8];
extern s32      D_800DD698;
extern s32      D_800DD69C;

extern void func_800A40F8(WorldVec *src, u8 *dst);
extern void *memcpy(void *dst, const void *src, u32 n);

/**
 * @brief Initialize scene-relative state from the template at @c D_800DD680 et al.
 *
 * No-op when the @c D_800C4DC8 flag is zero. Otherwise copies:
 *   - @c D_800DD698 → @c D_800C4D38 (primary cmd byte)
 *   - @c D_800DD680.x → @c D_800C9868.x
 *   - @c D_800DD680.y → @c D_800C9868.z  (Y/Z swap for PS1 coord system)
 *   - -@c D_800DD680.z → @c D_800C9868.y (Z negated)
 *   - @c D_800DD690[0..8] → @c D_800C9770[8..0xF] (unaligned 8 bytes)
 *   - @c D_800DD69C → @c D_800C4D3C (secondary cmd byte)
 * and calls @c func_800A40F8 with the source vector and @c D_800C9770 buffer.
 */
void func_800BD180(void) {
    if (D_800C4DC8 != 0) {
        D_800C4D38 = D_800DD698;
        D_800C9868.x = D_800DD680.x;
        D_800C9868.z = D_800DD680.y;
        D_800C9868.y = -D_800DD680.z;
        func_800A40F8(&D_800DD680, D_800C9770);
        memcpy(&D_800C9770[0x8], D_800DD690, 8);
        D_800C4D3C = D_800DD69C;
    }
}

/**
 * @brief Arm scene state: stash source data into @c D_800DD680.. for later apply.
 *
 * Counterpart to @c func_800BD180. Sets the "armed" flag at @c D_800C4DC8,
 * copies the caller's 16-byte vector block into @c D_800DD680, the 8-byte
 * rotation blob into @c D_800DD690 (unaligned), and the two cmd bytes into
 * @c D_800DD698 / @c D_800DD69C. @c func_800BD180 consumes this state on
 * its next call.
 *
 * @param src 16-byte vector block (@c x, @c y, @c z, @c pad0C).
 * @param rot 8-byte rotation blob (caller may pass an unaligned address).
 * @param cmd Primary cmd byte, stored at @c D_800DD698.
 * @param cmd2 Secondary cmd byte, stored at @c D_800DD69C.
 */
void func_800BD22C(WorldVec *src, u8 *rot, s32 cmd, s32 cmd2) {
    D_800C4DC8 = 1;
    D_800DD698 = cmd;
    D_800DD680 = *src;
    memcpy(D_800DD690, rot, 8);
    D_800DD69C = cmd2;
}

extern s32 D_800C5C2C;

extern s32 func_800A5DC8(s32 x, s32 y);

/**
 * @brief Compute two coarse-angle outputs for the current active slot.
 *
 * Looks up the active slot index at @c D_800C5C2C; returns 0 if no slot
 * is active (index < 0). Otherwise converts the slot's (x, -z) pair to an
 * angle via @c func_800A5DC8, then splits the s16 result into a "fine"
 * (mod 128, doubled) and "coarse" (div 128, doubled) halfword output.
 * Either output pointer may be NULL to skip that write.
 *
 * @param outLow  Destination for the fine angle component (may be NULL).
 * @param outHigh Destination for the coarse angle component (may be NULL).
 * @return 1 when the slot is active, 0 otherwise.
 */
s32 func_800BD2A0(s16 *outLow, s16 *outHigh) {
    s32 slotIdx = D_800C5C2C;
    s32 vec[3];
    s32 angle;
    s16 s16angle;
    s32 result = 0;

    if (slotIdx >= 0) {
        SlotEntry *slot = &D_800DBFB8[slotIdx];
        s32 *vp;
        vec[0] = slot->x;
        vp = vec;
        vp[1] = -slot->z;
        vp[2] = slot->y;
        angle = func_800A5DC8(vec[0], vec[1]);

        s16angle = (s16)angle;
        if (outLow != 0) {
            s16angle = (s16)angle;
            *outLow = (s16angle % 128) << 1;
        }
        if (outHigh != 0) {
            *outHigh = (s16angle / 128) << 1;
        }
        result = 1;
    }
    return result;
}

extern s32 D_800C5C28;

/**
 * @brief Twin of @c func_800BD2A0 that reads the secondary slot index.
 *
 * Identical logic to @c func_800BD2A0 except the slot index is sourced
 * from @c D_800C5C28 (vs @c D_800C5C2C). Used for the companion slot in
 * the dual-slot tracking scheme.
 *
 * @param outLow  Destination for the fine angle component (may be NULL).
 * @param outHigh Destination for the coarse angle component (may be NULL).
 * @return 1 when the slot is active, 0 otherwise.
 */
s32 func_800BD380(s16 *outLow, s16 *outHigh) {
    s32 slotIdx = D_800C5C28;
    s32 vec[3];
    s32 angle;
    s16 s16angle;
    s32 result = 0;

    if (slotIdx >= 0) {
        SlotEntry *slot = &D_800DBFB8[slotIdx];
        s32 *vp;
        vec[0] = slot->x;
        vp = vec;
        vp[1] = -slot->z;
        vp[2] = slot->y;
        angle = func_800A5DC8(vec[0], vec[1]);

        s16angle = (s16)angle;
        if (outLow != 0) {
            s16angle = (s16)angle;
            *outLow = (s16angle % 128) << 1;
        }
        if (outHigh != 0) {
            *outHigh = (s16angle / 128) << 1;
        }
        result = 1;
    }
    return result;
}

extern s32 D_800C5C24;

/**
 * @brief Third variant of the slot angle splitter, sourcing @c D_800C5C24.
 *
 * Same logic as @c func_800BD2A0 / @c func_800BD380 but reads the slot
 * index from @c D_800C5C24.
 */
s32 func_800BD460(s16 *outLow, s16 *outHigh) {
    s32 slotIdx = D_800C5C24;
    s32 vec[3];
    s32 angle;
    s16 s16angle;
    s32 result = 0;

    if (slotIdx >= 0) {
        SlotEntry *slot = &D_800DBFB8[slotIdx];
        s32 *vp;
        vec[0] = slot->x;
        vp = vec;
        vp[1] = -slot->z;
        vp[2] = slot->y;
        angle = func_800A5DC8(vec[0], vec[1]);

        s16angle = (s16)angle;
        if (outLow != 0) {
            s16angle = (s16)angle;
            *outLow = (s16angle % 128) << 1;
        }
        if (outHigh != 0) {
            *outHigh = (s16angle / 128) << 1;
        }
        result = 1;
    }
    return result;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BD540);

/**
 * @brief 32-byte record with two signed sentinel bytes near its tail.
 */
typedef struct {
    u8  pad00[3];
    u8  byte3;          /**< 0x03: zone byte, must be 1 or 2 for a match. */
    u8  pad04[0x1A];
    s8  sb1E;           /**< 0x1E: signed sentinel byte (-1 = open slot). */
    s8  sb1F;           /**< 0x1F: signed marker byte (0, 1, ...). */
} SlotTarget;

/**
 * @brief Classify a @c SlotTarget against an expected @p kind.
 *
 * Returns a small result code:
 *   - -1: @p kind is 0xFF, mismatch, or out-of-zone.
 *   - For @p kind == 0: 0 when @c sb1F == 1; 1 when @c sb1E == -1 && @c sb1F == 0.
 *   - For @p kind == 1: 2 when @c sb1F == 1.
 *   - Any other @p kind: -1.
 *
 * The @c byte3 "zone" byte must be 1 or 2 for any non-default return.
 */
s32 func_800BD640(u8 kind, SlotTarget *t) {
    s32 result = -1;

    if (kind == 0xFF) {
        return -1;
    }
    if (kind == 0) {
        if ((u32)(t->byte3 - 1) < 2) {
            if (t->sb1F == 1) {
                result = 0;
            } else if (t->sb1E == -1 && t->sb1F == 0) {
                result = 1;
            }
        }
    } else if (kind == 1) {
        if ((u32)(t->byte3 - 1) < 2 && t->sb1F == 1) {
            result = 2;
        }
    }
    return result;
}
