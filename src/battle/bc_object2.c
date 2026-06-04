/**
 * @file bc_object2.c
 * @brief Battle entity status effects and damage calculation.
 *
 * Handles status effect application, hit probability calculations,
 * damage computation, and entity flag management for the battle system.
 */
#include "common.h"
#include "battle.h"
#include "gamestate.h"

extern u8 D_800786D9[];

extern u8 *getMenuString(s32 id);
extern u8 *getStatName(s32 statId);

s32 func_8009D594(void);
s32 func_8009D508(s32 a0, s32 a1);
s32 func_8009DCCC(s32 a0, s32 a1, s32 a2);
void func_8009DD2C(s32 a0, s32 a1, u16 a2, s32 a3);

/**
 * @brief Look up entity ability flags with index-based table lookup.
 *
 * Stores a0 to D_800EE476, computes D_80078E00 + a0 * 0x18 as base,
 * reads byte at base + 0x374E, combines results of func_800B0F9C and
 * func_800B0F7C. Returns combined if bit 15 is set, otherwise returns a1.
 *
 * @param a0 Entity index (stride 0x18).
 * @param a1 Default return value if bit 15 not set.
 * @return Combined ability flags (u16) or a1 (u16).
 */
s16 func_8009BAC4(s32 arg0, s16 arg1) {
    s16 temp_s0_3;
    
    D_800EE476 = arg0;
    temp_s0_3 = func_800B0F9C(D_80078E00.abilities[arg0].val) 
              | func_800B0F7C(D_80078E00.abilities[arg0].val);
    
    if (temp_s0_3 & 0x8000) {
        return temp_s0_3;
    }
    
    return arg1;
}

/**
 * @brief Look up entity ability flags with adjusted index.
 *
 * Subtracts 0x40 from the index, computes entity pointer from
 * D_80078E00 + (a0 - 0x40) * 132, reads byte at offset 0xF81,
 * and combines results of func_800B0F9C and func_800B0F7C.
 *
 * @param a0 Entity index (offset by 0x40, stride 132).
 * @return Combined ability flags (u16).
 */
s32 func_8009BB3C(s32 a0) {
    s32 addr = (s32)&D_80078E00;
    s32 base = addr + (a0 - 0x40) * 132;
    s32 result = func_800B0F9C(*(u8 *)(base + 0xF81));
    result |= func_800B0F7C(*(u8 *)(base + 0xF81));
    return (u16)result;
}

/**
 * @brief Search entity table for entry with status byte 0xFA.
 *
 * Scans up to 32 entries in D_800ED148 (stride 0x14) checking the byte
 * at offset 0x5C5 for value 0xFA. Returns the index of the first match.
 *
 * @return Index of matching entry (0-31), or 0 if none found.
 */
s32 func_8009BB98(void) {
    s32 i;
    for (i = 0; i < 32; i++) {
        if (D_800ED148.entries[i].unk_00 == 0xFA) {
            return i;
        }
    }
}

/**
 * @brief Look up entity status and store it to D_800EE4C0 buffer.
 *
 * Calls func_8009BB98 to get entity index, looks up entry in
 * D_800ED70C (stride 20), copies entry[0] and D_800ED70C[0xD6A]
 * into D_800EE4C0 buffer with command byte 0xF9.
 */
BattleEntry* func_8009BBD0(void) {
    s32 idx;
    BattleEntry* entry;

    idx = func_8009BB98();
    entry = &D_800ED70C[idx];

    D_800EE4C0.unk0 = entry->unk0;
    D_800EE4C0.unk1 = 249;
    D_800EE4C0.unk3 = ((u8*)D_800ED70C)[0xD6A];
    return entry;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009BC28);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009BCE4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009BD60);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009BDD0);

/**
 * @brief Check entity stat against limit and set overflow flag.
 *
 * Reads byte at entity[0xD2] (index a0, stride 0xD0 in D_800ED148),
 * adds D_800EEBBC, and checks if the sum exceeds 0xFF via func_8009B79C.
 * If it does, sets D_800ED148[0x1307] = 1 and sets bit 2 in D_800EE4C0[6].
 * Otherwise clears D_800ED148[0x1307] = 0.
 *
 * @param a0 Entity index (stride 0xD0).
 */
void func_8009BE24(s32 a0) {
    volatile u8 *base = (volatile u8 *)&D_800ED148;
    u8 *entity = (u8 *)base + a0 * 0xD0;
    s32 val = *(u8 *)(entity + 0xD2) + *(u8 *)D_800EEBBC;
    if (func_8009B79C(val, 0xFF)) {
        s32 buf = (s32)&D_800EE4C0;
        *(u8 *)((u8 *)base + 0x1307) = 1;
        *(u8 *)(buf + 6) |= 2;
    } else {
        *(u8 *)((u8 *)base + 0x1307) = 0;
    }
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009BEA4);

/**
 * @brief Conditionally XOR masked flag bits.
 *
 * Compares masked versions of newFlags and *flagsPtr. If they differ,
 * returns the masked current flags XORed with masked new flags.
 *
 * @param newFlags New flag value to compare.
 * @param flagsPtr Pointer to current flags word.
 * @param mask Bit mask for comparison.
 * @return Masked current value, XORed with masked newFlags if different.
 */
s32 func_8009BF50(s32 newFlags, s32 *flagsPtr, s32 mask) {
    s32 current = *flagsPtr;
    newFlags &= mask;
    current &= mask;
    if (current != newFlags) {
        current ^= newFlags;
    }
    return current;
}

void func_8009BF70(s32 newFlags, s32 *flagsPtr) {
    s32 current = *flagsPtr;
    if (current != newFlags) {
        s32 masked = current & ~(0xE | 0x300);
        *flagsPtr = masked
                  | func_8009BF50(newFlags, flagsPtr, 0xE)
                  | func_8009BF50(newFlags, flagsPtr, 0x300);
    }
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009BFE0);

/**
 * @brief Apply a status flag to an entity if not blocked by guards.
 *
 * Performs cascading guard checks against the proposed flag (a3):
 * - If bit 0x800 in a3 is set and a0 >= 3, rejects (returns 0)
 * - If bit 0x40 in a1 is set and bit 0x400 in a3 is set, rejects
 * - If bit 0x2000000 in *a2 is set and bit 0x4000 in a3 is set, rejects
 * If all checks pass, ORs a3 into *a2 and calls func_800B0574.
 *
 * @param a0 Entity index.
 * @param a1 Entity status byte.
 * @param a2 Pointer to entity flags word.
 * @param a3 Proposed status flag to apply.
 * @return 1 if flag was applied, 0 if rejected.
 */
s32 func_8009C090(s32 arg0, s32 arg1, s32* arg2, u32 arg3) {
    s32 a1 = arg1;

    if ((arg3 & 0x800) && (arg0 >= 3)) {
        return 0;
    }
    
    if ((a1 & 0x40) && (arg3 & 0x400)) {
        return 0;
    
    }
    
    if ((*arg2 & 0x02000000) && (arg3 & 0x4000)) {
        return 0;
    }
    
    *arg2 |= arg3;
    func_800B0574(arg0, arg3);
    
    return 1;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009C104);

u8 func_8009C300(s32 arg0, s32 arg1) {
    u8 var_a1;
    s32 index = 1;
    
    if (arg1 == 0) {
        var_a1 = (D_800ED148.entities + arg0)->unkCE;
    } 
    
    else {
        var_a1 = (D_800ED148.entities + arg0 + index)->unk0;
    }
    
    if (D_800ED148.entities[arg0].flags & 0x01000000) {
        var_a1 = 0;
    }
    
    return var_a1;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009C390);

/**
 * @brief Look up entity type byte from a battle table.
 * @param id Entity identifier (masked to 16 bits).
 * @return First byte of the resolved table entry.
 */
s32 func_8009C570(s32 id) {
    u8 buf[8];
    func_800A4FC4(id & 0xFFFF, buf);
    return buf[0];
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009C598);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009C610);

/**
 * @brief Clamp a damage value to a maximum of 9999.
 * @param val Value to clamp.
 * @return val if < 10000, otherwise 9999.
 */
s32 func_8009C6CC(s32 val) {
    if (val >= 10000) {
        val = 9999;
    }
    return val;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009C6E4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009C798);

void func_8009C8B8(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    D_800EEBC4 &= 0xFBFFFFFF;
    
    if (D_800EEBC4 & 0x8000) {
        func_8009C6E4(arg1, arg2, arg4);
        D_800EEBC4 &= 0xFFFF7FFF;
    }
    
    if (D_800ED148.entities[arg2].flags & 0x400000) {
        if (D_800ED148.entities[arg1].controlFlags & 0x1000) {
            if (arg0 == 0) {
                func_8009C798(arg1, arg2, arg3);
            }
        }
    }
    
    if (!(func_8009C390(arg1, arg2, arg0) & 0xFF)) {
        if (arg3 == 0) {
            D_800EE4C0.unk6 |= 4;
        }
        
    } else if (arg3 == 0) {
        D_800EE4C0.unk5 |= 1;
    }
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009CA14);

/**
 * @brief Apply a single-stat boost effect from src entity onto dst entity.
 *
 * Clears the queued action; bails if dst already has the 0x800 control flag.
 * Calls @c func_800AF134 with a "type byte" read from past entities[srcIdx]
 * (offset 0xD1 — into the next entity's region) plus two output bytes.
 * On result == 1, queues an actionType=1 message with the two bytes, sets
 * the dst's 0x800 control flag, and renders a formatted message string
 * via @c func_800B0248 chained calls. The "count == 1" branch produces a
 * single-stat phrase; otherwise produces a multi-stat phrase with an extra
 * @c getMenuString(0x77) clause. On result 0/2 it shows a generic message.
 *
 * @param srcIdx Source entity slot index.
 * @param dstIdx Target entity slot index.
 */
void func_8009CAD8(s32 srcIdx, s32 dstIdx) {
    u8 stackbuf[8];
    u8 stat;
    u8 count;
    u8 *p;
    u8 *str;
    s16 chA1;
    u32 chA2;
    u8 ch;
    s16 chB2;
    u8 *strCacheB2;
    s16 chB1;
    u8 *strCacheB1;
    BattleEntity *entityDst;

    entityDst = &D_800ED148.entities[dstIdx];
    D_800ED148.actionType = 0;

    if (entityDst->controlFlags & 0x800) {
        func_800A432C(0x12);
        return;
    }

    {
        s32 src_off = srcIdx * 0xD0 + 0xD1;
        switch (func_800AF134(dstIdx, &stat, &count, ((u8 *)&D_800ED148)[src_off])) {
        case 1:
            D_800ED148.actionType = 1;
            D_800ED148.actionByte0 = stat;
            D_800ED148.actionByte1 = count;
            *(volatile s32 *)&entityDst->controlFlags |= 0x800;

            if (count == 1) {
                {
                    u8 *prefix1 = getMenuString(0x13);
                    u8 *prefix2;
                    str = getMenuString(0xB);
                    ch = *str;
                    chA1 = ch;
                    prefix2 = func_800B0248(prefix1, chA1, func_800B04A0(count, stackbuf));
                    str = getMenuString(0xB);
                    ch = *str;
                    chA2 = ch;
                    p = func_800B0248(prefix2, chA2, getMenuString(0x47));
                }
                {
                    u8 *prefix3 = func_800B0248(p, 7, getStatName(stat));
                    p = func_800B0248(prefix3, 7, getMenuString(0x73));
                }
                func_800A4320(func_800B02AC(p));
                break;
            } else {
                {
                    u8 *prefix1 = getMenuString(0x13);
                    u8 *prefix2;
                    strCacheB2 = getMenuString(0xB);
                    str = strCacheB2;
                    ch = *str;
                    chB2 = ch;
                    prefix2 = func_800B0248(prefix1, chB2, func_800B04A0(count, stackbuf));
                    strCacheB1 = getMenuString(0xB);
                    str = strCacheB1;
                    ch = *str;
                    chB1 = ch;
                    p = func_800B0248(prefix2, chB1, getMenuString(0x47));
                }
                {
                    u8 *prefix3 = func_800B0248(p, 7, getStatName(stat));
                    u8 *prefix4 = func_800B0248(prefix3, 7, getMenuString(0x77));
                    p = func_800B0248(prefix4, 7, getMenuString(0x73));
                }
                func_800A4320(func_800B02AC(p));
            }
            break;
        case 0:
            func_800A432C(0x11);
            break;
        case 2:
            func_800A432C(0x12);
            break;
        }
    }
}

/**
 * @brief Conditionally queue a status effect clear command.
 *
 * If D_800EE456 bits 0-1 are clear, queues a battle command
 * to clear status flag 0x4001 on the given entity.
 *
 * @param a0 Entity slot index.
 */
void func_8009CD10(s32 a0) {
    if ((*(u8 *)D_800EE456 & 3) == 0) {
        func_8009B924(a0, 0, 0x4001);
    }
}

/**
 * @brief Conditionally queue a high-bit status clear command.
 *
 * If D_800EE456 bits 0-1 are clear, queues a battle command
 * to clear status flag 0x800000 on the given entity.
 *
 * @param a0 Entity slot index.
 */
void func_8009CD44(s32 a0) {
    if ((*(u8 *)D_800EE456 & 3) == 0) {
        func_8009B924(a0, 0, 0x800000);
    }
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009CD78);

/**
 * @brief Query battle state 0x21 and add offset 0xEF to the result.
 *
 * @return func_8009B7BC(0x21) + 0xEF.
 */
s32 func_8009CF18(void) {
    return func_8009B7BC(0x21) + 0xEF;
}

/**
 * @brief Compute damage for a battle action based on type and route to dispatcher.
 *
 *   - case 0/19: scale formula using attacker @c fieldCD (squared), defense,
 *                power, and the @c func_8009CF18 modifier. Case 19 forces
 *                @c defense to 0 first.
 *   - case 1:    if target's @c controlFlags bit 0x10000 is set, mark
 *                @c D_800EE4C0[6] (bit 2) and report 0; else scale @c field28.
 *   - case 3:    multiply attacker's @c field2C by 5.
 *   - case 16:   active party member's kill count (selected via the party
 *                slot at @c targetIdx) times power. If @c targetIdx >= 3,
 *                the damage is forced to 0.
 *
 * @param attackerIdx Battle entity index of the attacker (case 0/3 path).
 * @param targetIdx   Battle entity index of the target (case 1) or party slot (case 16).
 * @param power       Damage multiplier.
 * @param type        Action type, indexes the case dispatch.
 */
void func_8009CF38(s32 attackerIdx, s32 targetIdx, s32 power, s32 type) {
    s32 defense;
    s32 dmg;
    s32 stat, mod, sq;

    defense = func_8009C300(targetIdx, 0);

    if ((u32)type < 20) {
        switch (type) {
        case 19:
            defense = 0;
        case 0:
            mod = func_8009CF18();
            stat = D_800ED148.entities[attackerIdx].fieldCD;
            sq = stat * stat / 16 + stat;
            dmg = sq * (0x109 - defense) / 256 * power / 16 * mod / 256;
            break;

        case 1:
            if (D_800ED148.entities[targetIdx].controlFlags & 0x10000) {
                dmg = 0;
                D_800EE4C0.flags6 |= 4;
            } else {
                dmg = D_800ED148.entities[targetIdx].field28 * power / 16;
            }
            break;

        case 3:
            dmg = D_800ED148.entities[attackerIdx].field2C * 5;
            break;

        case 16:
            if (targetIdx < 3) {
                dmg = g_gameState.chars[g_gameState.mainData.party.party[targetIdx]].kills * power;
            } else {
                dmg = 0;
            }
            break;
        }
    }

    func_8009CD78(attackerIdx, targetIdx, power, dmg);
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009D174);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009D228);

/**
 * @brief Check if entity at index a1 has ability flag bit 2 set.
 *
 * @param a0 Unused.
 * @param a1 Entity index.
 * @return 1 if entity has bit 2 set and global flag is clear, 0 otherwise.
 */
s32 func_8009D420(s32 a0, s32 a1) {
    s32 base;
    s32 flags;
    if (!(D_800EEBC4 & 0x4000000)) {
        base = (s32)&D_800ED148;
        flags = *(u16 *)(base + a1 * 0xD0 + 0x90);
        if (flags & 0x4) {
            return 1;
        }
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009D474);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009D508);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009D594);

/**
 * @brief Conditionally set control flag bit 0x10 in D_800EE4C0.
 *
 * If bit 2 of D_800EE4C0[6] is clear and the u16 at D_800EE4C0+0x1C
 * equals 0x49, sets bit 0x10 in D_800EE4C0[6].
 */
void func_8009D68C(void) {
    s32 base = (s32)&D_800EE4C0;
    u8 val = *(u8 *)(base + 6);
    if (!(val & 4)) {
        if (*(u16 *)(base + 0x1C) == 0x49) {
            *(u8 *)(base + 6) = val | 0x10;
        }
    }
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009D6C4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009D7D8);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009DCCC);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009DD2C);

/**
 * @brief Compute and dispatch a battle-action effect value.
 *
 * Returns early (0) if the action is gated by @c func_8009D594. Otherwise
 * sets @c D_800EE4C0.flags6 bit 0x01, calls @c func_8009D508; on failure
 * raises bit 0x04 in @c flags6 and returns 0. Then by mode (@p arg3):
 *
 *   - mode 7: take @c func_8009CF18 as a scale @c cf, average the
 *     attacker's @c fieldCF with @p arg2, scale by @p arg2 and @c cf, and
 *     divide by 256 to get the action's effect value @c s1.
 *   - mode 8: scale the target's @c field2C by @p arg2 and divide by 16.
 *
 * After computing @c s1, if the target has @c flags & 0x40 and @c s1 is
 * non-zero, raise @c D_800EE4C0.flags5 bit 0x20 and halve @c s1. If the
 * target's @c status has bit 0x4 set, clear @c s1 entirely. Finally hand
 * off to @c func_8009DCCC (which returns the final value) and dispatch
 * via @c func_8009DD2C with @c D_800EEBC2 / @c D_800EEBC4 as the status
 * code / flags word, then return the computed @c s1.
 *
 * @param arg0 Attacker entity index (used by @c func_8009D508, the
 *             mode-7 attacker-side scale, and @c func_8009DCCC).
 * @param arg1 Target entity index.
 * @param arg2 Scale value.
 * @param arg3 Mode (7 or 8 produce a non-zero @c s1; other values leave
 *             @c s1 uninitialized, matching the original codegen).
 * @return     The post-dispatch effect value from @c func_8009DCCC.
 */
s32 func_8009DEF0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 s1;
    s32 t;
    s32 cf;

    if (func_8009D594() != 0) {
        return 0;
    }
    D_800EE4C0.flags6 |= 1;
    if (func_8009D508(arg0, arg1) != 0) {
        D_800EE4C0.flags6 |= 4;
        return 0;
    }

    switch (arg3) {
    case 7:
        cf = func_8009CF18();
        t = (D_800ED148.entities[arg0].fieldCF + arg2) / 2;
        t *= arg2;
        s1 = (t * cf) / 256;
        break;
    case 8:
        s1 = (D_800ED148.entities[arg1].field2C * arg2) / 16;
        break;
    }

    if ((D_800ED148.entities[arg1].flags & 0x40) && s1 != 0) {
        D_800EE4C0.flags5 |= 0x20;
        s1 >>= 1;
    }
    if (D_800ED148.entities[arg1].status & 0x4) {
        s1 = 0;
    }

    s1 = func_8009DCCC(arg0, arg1, s1);
    func_8009DD2C(arg1, arg2, D_800EEBC2, D_800EEBC4);
    return s1;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009E110);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009E33C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009E418);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009E528);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009E5C0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009E684);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009E7B0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009E95C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009EA08);

/**
 * @brief Apply damage-over-time tick to GF targets and clear pending flag.
 *
 * If the entity's @c controlFlags bit 0x20000 is set, queries the current
 * target list (up to 16) and for each target:
 *   - Scales their @c gfs[].hp by @c effectMult % of their battle hp,
 *   - Triggers a battle event via @c func_8002363C,
 *   - Clears bit 1 in the matching itemSlot flag byte.
 *
 * After the pass, if @c flags is negative, the entity is restored from its
 * GF max HP (via @c characterId − 0x40 lookup), and the value is mirrored
 * into @c hpDisplay. The pending bit (0x20000) is then cleared.
 *
 * @param charIdx Battle entity index.
 */
void func_8009EAEC(s32 charIdx) {
    u8 targets[16];
    BattleEntity *entry;
    s32 count, i, j;
    s32 id, amount;

    if (!(D_800ED148.entities[charIdx].controlFlags & 0x20000)) {
        return;
    }

    count = func_800AF918(charIdx, targets);

    if (count != 0) {
        for (i = 0; i < count; i++) {
            g_gameState.gfs[targets[i]].hp +=
                (D_800ED148.effectMult * (s16)g_battleChars.gfEntries[targets[i]].hp) / 100;

            func_8002363C(targets[i]);

            for (j = 0; j < 16; j++) {
                if (g_battleChars.chars[charIdx].itemSlots[j].field0 == targets[i] + 0x40) {
                    g_battleChars.chars[charIdx].itemSlots[j].field4 &= 0xFD;
                    break;
                }
            }
        }

        if (D_800ED148.entities[charIdx].flags < 0) {
            id = g_battleChars.chars[charIdx].field01D - 0x40;
            g_battleChars.chars[charIdx].currentHp = g_battleChars.gfEntries[id].maxHp;
            D_800ED148.entities[charIdx].hpDisplay = g_battleChars.chars[charIdx].currentHp;
        }
    }
    D_800ED148.entities[charIdx].controlFlags &= ~0x20000;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009ED2C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009EE44);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009EF64);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009F040);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009F168);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009F23C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009F350);

/**
 * @brief Read and clear an entity's pending damage value.
 *
 * Reads a u16 at entity offset 0xDC, clears it, and returns the
 * value right-shifted by 2.
 *
 * @param entityIdx Entity index.
 * @return Pending damage value divided by 4.
 */
s32 func_8009F3F8(s32 entityIdx) {
    u8 *base = (u8 *)&D_800ED148;
    u8 *entity;
    s32 val;
    asm("");
    entity = base + entityIdx * 0xD0;
    val = *(u16 *)(entity + 0xDC);
    *(u16 *)(entity + 0xDC) = 0;
    return val >> 2;
}

/**
 * @brief Read entity field at offset 0x2C and divide by 5.
 *
 * Indexes into D_800ED148 by a0*208, reads the word at offset 0x2C,
 * and returns the result divided by 10.
 *
 * @param a0 Entity index (stride 208).
 * @return Entity field value divided by 5.
 */
s32 func_8009F428(s32 a0) {
    u8 *base = (u8 *)&D_800ED148;
    s32 val = *(s32 *)(base + a0 * 208 + 0x2C);
    return val / 10;
}

/**
 * @brief Mark entity for processing and set a control flag.
 *
 * Sets bit 0 in D_800EE4C0[5] and sets bit 0x10000 in the entity's
 * flags word at offset 0x18.
 *
 * @param entityIdx Entity index.
 * @return Always 0.
 */
s32 func_8009F46C(s32 entityIdx) {
    s32 ctrl = (s32)&D_800EE4C0;
    u8 *base;
    u8 *entity;
    *(u8 *)(ctrl + 5) |= 1;
    base = (u8 *)&D_800ED148;
    asm("");
    entity = base + entityIdx * 0xD0;
    *(s32 *)(entity + 0x18) |= 0x10000;
    return 0;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009F4BC);

/**
 * @brief Test a bit in the battle flag array at g_gameState+0xD0C.
 *
 * Computes the word index and bit position from the given bit index,
 * then tests whether the corresponding bit is set.
 *
 * @param bitIndex The bit index to test.
 * @return 1 if the bit is set, 0 otherwise.
 */
INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009F52C);

/**
 * @brief Set a bit in the battle flag array at g_gameState+0xD0C.
 *
 * Computes the word index and bit position from the given bit index,
 * then ORs the corresponding bit into the flag array.
 *
 * @param bitIndex The bit index to set.
 */
void func_8009F570(s32 bitIndex) {
    s32 wordIdx = bitIndex / 32;
    s32 bitPos = bitIndex % 32;
    s32 base = (s32)&g_gameState;
    *(s32 *)(base + wordIdx * 4 + 0xD0C) |= (1 << bitPos);
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009F5B4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009F65C);

/**
 * @brief Look up a byte attribute from D_80078E00 table (stride 0x3C).
 * @param idx Entry index.
 * @return Byte at offset 0x228 within the table entry.
 */
s32 func_8009F6F4(s32 idx) {
    u8 *base = (u8 *)&D_80078E00;
    u8 *entry;
    asm("");
    entry = base + idx * 0x3C;
    return entry[0x228];
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009F718);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009F824);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009F930);

/**
 * @brief Store a command byte to entity data.
 *
 * Stores the full command value at D_800ED148[0x130E], then stores the
 * low 2 bits at entity[0x99] where the entity index is read from
 * D_800ED148[0x12F3].
 *
 * @param cmd Command byte value.
 */
void func_8009FCF4(s32 cmd) {
    u8 *base = (u8 *)&D_800ED148;
    s32 idx = base[0x12F3];
    base[0x130E] = cmd;
    cmd &= 3;
    *(u8 *)(base + idx * 0xD0 + 0x99) = cmd;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009FD28);

/**
 * @brief Check if a value is at least 20.
 * @param val Value to test.
 * @return 1 if val >= 20, 0 otherwise.
 */
s32 func_8009FDD4(s32 val) {
    return val >= 20;
}

/**
 * @brief Compute an entity table offset from a byte field.
 *
 * Looks up byte at offset 0xDA in entity a0's table entry (stride 208),
 * then returns (a1 * 4 - 1) + that byte value.
 *
 * @param a0 Entity index.
 * @param a1 Multiplier input.
 * @return Computed offset value.
 */
s32 func_8009FDE0(s32 a0, s32 a1) {
    s32 base;
    a1 = a1 * 4;
    base = (s32)&D_800ED148;
    a1 -= 1;
    return a1 + *(u8 *)(base + a0 * 208 + 0xDA);
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009FE14);

s32 func_800A085C(void) {
    s32 i;
    u16 total = 0;
    s32 count = 0;

    for (i = 0; i < 16; i++) {
        if (g_gameState.chars[i].flags & 1) {
            count++;
            total += g_battleChars.levelEntries[i].level;
        }
    }

    return total / count;
}

/**
 * @brief Get the active entity count, defaulting to 1 if unset.
 *
 * @return The value of D_800786D9, or 1 if it is zero.
 */
s32 func_800A08C0(void) {
    u8 val = *(u8 *)D_800786D9;
    if (val == 0) {
        return 1;
    }
    return val;
}

void func_800A08E0(void) {
    u8 temp_a0;

    if (D_800ED148.unk1319 != 0xFF) {
        temp_a0 = D_800EE4C0.flags6 & 0x3F;
    
        switch (D_800ED148.unk1319) {
            case 0:
                D_800EE4C0.flags6 = temp_a0;
                break;
            case 1:
                D_800EE4C0.flags6 = temp_a0 | CTRL_FLAG_40;
                break;
            case 2:
                D_800EE4C0.flags6 = temp_a0 | CTRL_FLAG_80;
                break;
        }
        
        D_800EE4C0.unk4 = 22;
        D_800EE4C0.flags6 |= 8;
    }
}

/**
 * @brief Apply 1.5x multiplier to battle control value if entity has status bit 0x20.
 *
 * If the entity at entityIdx has status bit 0x20 set, multiplies
 * the word at D_800EE4C0[0xC] by 3/2 (rounds down).
 *
 * @param entityIdx Index into the battle entity array.
 */
void func_800A0978(s32 entityIdx) {
    u8 *base = (u8 *)&D_800ED148;
    u8 *entity;
    asm("");
    entity = base + entityIdx * 0xD0;
    if (*(u16 *)(entity + 0x90) & 0x20) {
        s32 ctrl = (s32)&D_800EE4C0;
        u32 val = *(u32 *)(ctrl + 0xC);
        *(u32 *)(ctrl + 0xC) = (val * 3) >> 1;
    }
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_800A09D0);

void func_800A1760(s32 arg0, BattleCharData* arg1) {
    s32 i;
    
    for (i = 0; i < 4; i++) {
        switch (arg1->cmdSlots[i].cmdType) {
            case 2:
                if (arg0 == 0) {
                    if (!(g_battleConfig.unk8 & 2)) {
                        arg1->cmdSlots[i].field3 &= ~2;
                    }
                } else {
                    arg1->cmdSlots[i].field3 |= 2;
                }
            break;
            case 3:
                if (arg0 == 0) {
                    if (!(g_battleConfig.unk8 & 4)) {
                        arg1->cmdSlots[i].field3 &= ~2;
                    }
                } else {
                    arg1->cmdSlots[i].field3 |= 2;
                }
            break;
            case 6:
                if (arg0 == 0) {
                    if (!(g_battleConfig.unk8 & 8)) {
                        arg1->cmdSlots[i].field3 &= ~2;
                    }
                } else {
                    arg1->cmdSlots[i].field3 |= 2;
                }
            break;
        }
    }
}

/**
 * @brief Set masked attribute values on a g_battleChars table entry (stride 0x1D0).
 *
 * Stores a masked u16 at offset 0x1B2 and a masked u32 at offset 0x188.
 *
 * @param idx Entry index.
 * @param attr Attribute value (masked to 7 bits).
 * @param flags Flag value (masked to 0x30E7FFF).
 */
void func_800A184C(s32 idx, s32 attr, s32 flags) {
    u8 *entry = (u8 *)&g_battleChars.chars[idx];
    *(u16 *)(entry + 0x1B2) = attr & 0x7F;
    *(s32 *)(entry + 0x188) = flags & 0x30E7FFF;
}

/**
 * @brief Check entity attribute bit and call func_800A1760.
 *
 * Computes g_battleChars + a0 * 0x1D0, checks bit 0x10 in the
 * halfword at offset 0x1B2. Calls func_800A1760 with 1 if the
 * bit is set, or 0 if clear.
 *
 * @param arg0 Entity index (stride 0x1D0).
 */
 void func_800A1888(s32 arg0) {
    BattleCharData* temp_a1 = &g_battleChars.chars[arg0];
    
    if (temp_a1->displayStatus & 0x10) {
        func_800A1760(1, temp_a1);
    }
        
    else {
        func_800A1760(0, temp_a1);
    }   
}
