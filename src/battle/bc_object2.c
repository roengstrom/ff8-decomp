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

/* Inconsistent-ABI symbol: bc_object4.c views D_800EEBC8 as u8[]; here scalar u8. */
extern u8 D_800EEBC8;

#define BATTLE_ENTITY_FLAG_BIT_11  (1 << 11) /* 0x800     */
#define BATTLE_ENTITY_FLAG_BIT_13  (1 << 13) /* 0x2000    */
#define BATTLE_ENTITY_FLAG_BIT_14  (1 << 14) /* 0x4000    */
#define BATTLE_ENTITY_FLAG_BIT_15  (1 << 15) /* 0x8000    */
#define BATTLE_ENTITY_FLAG_BIT_17  (1 << 17) /* 0x20000   */
#define BATTLE_ENTITY_FLAG_BIT_19  (1 << 19) /* 0x80000   */
#define BATTLE_ENTITY_FLAG_BIT_20  (1 << 20) /* 0x100000  */
#define BATTLE_ENTITY_FLAG_BIT_21  (1 << 21) /* 0x200000  */
#define BATTLE_ENTITY_FLAG_BIT_26  (1 << 26) /* 0x4000000 */


extern u8 D_800786D9[];

extern u8 *getMenuString(s32 id);
extern u8 *getStatName(s32 statId);

s32 func_8009D594(s32 unused, s32 arg1);
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
u16 func_8009BAC4(s32 arg0, u16 arg1) {
    s16 result;
    
    D_800EE476 = arg0;
    result = func_800B0F9C(D_80078E00.unk3738[arg0].val) 
           | func_800B0F7C(D_80078E00.unk3738[arg0].val);
    
    if (result & BATTLE_ENTITY_FLAG_BIT_15) {
        return result;
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
        if (D_800ED148.entries[i].unk0 == 250) {
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
    BattleEntry* currentEntry;

    currentEntry = &D_800ED70C[func_8009BB98()];
    D_800EE4C0.unk0 = currentEntry->unk0;
    D_800EE4C0.unk1 = 249;
    D_800EE4C0.unk3 = D_800ED70C[171].unkE; /* 0xD6A */ 

    return currentEntry;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009BC28);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object2", func_8009BCE4);

s32 func_8009BD60(s32 arg0) {
    if ((D_800EEBC4 & BATTLE_ENTITY_FLAG_BIT_26) == 0) {
        if ((D_800ED148.entities[arg0].status & 4) || (D_800ED148.entities[arg0].flags & (BATTLE_ENTITY_FLAG_BIT_11 | BATTLE_ENTITY_FLAG_BIT_19 | BATTLE_ENTITY_FLAG_BIT_20))) {
            return 1;
        }
    }
    
    return 0;
}

s32 func_8009BDD0(s32 arg0, s32 arg1) {
    if ((D_800ED148.entities[arg1].flags & (ENTITY_FLAG_1 | ENTITY_FLAG_4)) || (D_800EEBBB == 255)) {
        return 1;
    }
    
    return 0;
}

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
void func_8009BE24(s32 a0, s32 unused) {
    volatile u8 *base = (volatile u8 *)&D_800ED148;
    u8 *entity = (u8 *)base + a0 * 0xD0;
    s32 val = *(u8 *)(entity + 0xD2) + D_800EEBBC;
    if (func_8009B79C(val, 0xFF)) {
        s32 buf = (s32)&D_800EE4C0;
        *(u8 *)((u8 *)base + 0x1307) = 1;
        *(u8 *)(buf + 6) |= 2;
    } else {
        *(u8 *)((u8 *)base + 0x1307) = 0;
    }
}

s32 func_8009BEA4(s32 arg0, s32 arg1) {
    BattleSystem *bs = &D_800ED148;
    BattleEntity *slotA = &bs->entities[arg0];
    BattleEntity *slotB;
    s32 baseVal;
    s32 result;
    
    if (slotA->status & 8) {
        D_800EEBBB = D_800EEBBB / 4;
    }
    
    baseVal = D_800EEBBB + (slotA[1].stateMachine.bytes.unk2 / 2);
    slotB = &bs->entities[arg1];
    result = baseVal - slotB[1].stateMachine.bytes.unk3 - slotB[1].stateMachine.bytes.unk2;

    if (result < 0) {
        result = 0;
    }
    
    return func_8009B79C(result, 100);
}

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

s32 func_8009BFE0(s32 arg0, u16* arg1, s32* arg2, s32 arg3) {
    if ((D_800EE471 == 0) && (*arg1 & 0x40) && (arg3 & 1)) {
        return 0;
    }
        
    
    if((*arg2 & 0x02000000) && (arg3 & 0x30) != 0) {
        return 0;
    }
    
    if ((*arg2 & 0x400) && (arg3 & 0x40)) {
            *arg2 = *arg2 & ~0x400;
            func_800B0600(arg0, 0x400);
    }
    
    *arg1 |= arg3;
    return 1;
}

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

    if ((arg3 & BATTLE_ENTITY_FLAG_BIT_11) && (arg0 >= 3)) {
        return 0;
    }
    
    if ((a1 & CTRL_FLAG_40) && (arg3 & 0x400)) {
        return 0;
    
    }
    
    if ((*arg2 & 0x02000000) && (arg3 & BATTLE_ENTITY_FLAG_BIT_14)) {
        return 0;
    }
    
    *arg2 |= arg3;
    func_800B0574(arg0, arg3);
    
    return 1;
}

s32 func_8009C104(s32 unused, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7) {
    s32 var_v0;

    if (arg4 == 0) {
        var_v0 = D_800ED148.entities[arg1].status & arg3;
        if (var_v0 != 0) {
            return 0;
        }
    } else {
        var_v0 = D_800ED148.entities[arg1].flags & arg3;
        if (var_v0 != 0) {
            return 0;
        }
    }


    if (arg7 != 255) {
        if (D_800ED148.entities[arg1].unkA0[arg2] >= 200) {
            goto bail;
        }
        if ((arg7 + 300 + (arg5 / 4) - D_800ED148.entities[arg1].unkA0[arg2] - (arg6 / 4) <= 300) || 
            (arg7 < 250 && func_8009B79C((arg7 + (arg5 / 4) - D_800ED148.entities[arg1].unkA0[arg2] - (arg6 / 4)) * 255 / 100, 255) == 0)) {
            return 0;
        }
    }

    if (arg4 == 0) {
        return func_8009BFE0(arg1, &D_800ED148.entities[arg1].status, &D_800ED148.entities[arg1].flags, arg3);
    }

    return func_8009C090(arg1, D_800ED148.entities[arg1].status, &D_800ED148.entities[arg1].flags, arg3);
    
    bail:
    return 0;
}

s32 func_8009C300(s32 arg0, s32 arg1) {
    u8 result;
    s32 index = 1;
    
    if (arg1 == 0) {
        result = D_800ED148.entities[arg0].unkCE;
    } 
    
    else {
        result = (D_800ED148.entities + arg0 + index)->stateMachine.unk0;
    }
    
    if (D_800ED148.entities[arg0].flags & 0x01000000) {
        result = 0;
    }
    
    return result;
}

u8 func_8009C390(s32 arg0, s32 arg1, s32 arg2) {
    s32 temp_s3;
    s32 var_s0;
    s32 var_s1;
    s32 var_s2;
    s32 var_s5;
    s32 sp20;
    BattleEntity* entities;
    
    var_s2 = 0;
    entities = D_800ED148.entities;
    if (entities[arg1 + 1].slot8.byteView.unk09 == 0) { 
        sp20 = entities[arg1].flags;
        
        if (arg2 == 0) {
            var_s5 = entities[arg0].fieldCD;
            temp_s3 = func_8009C300(arg1, 0);
        } 
        
        else {
            var_s5 = entities[arg0].fieldCF;
            temp_s3 = func_8009C300(arg1, 1);
        }
        
        var_s1 = 1;
        for (var_s0 = 0; var_s0 < 7; var_s0++, var_s1 *= 2) {
            if (D_800EEBC2 & var_s1) {
                var_s2 += func_8009C104(arg0, arg1, var_s0, var_s1, 0, var_s5, temp_s3, D_800EEBBA);
            }
        }

        var_s1 = 1;
        for (var_s0 = 8; var_s0 < 40; var_s0++, var_s1 *= 2) {
            if (D_800EEBC4 & var_s1) {
                var_s2 += func_8009C104(arg0, arg1, var_s0, var_s1, 1, var_s5, temp_s3, D_800EEBBA);
            }
        }
        
        func_8009BF70(sp20, &D_800ED148.entities[arg1].flags);
    }
    
    return var_s2;
}

/**
 * @brief Look up entity type byte from a battle table.
 * @param id Entity identifier (masked to 16 bits).
 * @return First byte of the resolved table entry.
 */
u8 func_8009C570(s32 id) {
    u8 buf[8];
    func_800A4FC4(id & 0xFFFF, buf);
    return buf[0];
}

s32 func_8009C598(s32 arg0, s32 arg1) {
    if (D_800ED148.entities[arg0].status & CTRL_FLAG_40) {
        if (arg1 == 7) {
            return 700;
        }
    }
    return D_800ED148.entities[arg0].unk54[arg1];
}

s32 func_8009C610(s32 arg0, s32 arg1, s32 arg2) {    
    if (!(D_800ED148.entities[arg0].status & CTRL_FLAG_40)) {
        if (D_800ED148.entities[arg1].status & CTRL_FLAG_40) {
            arg2 = -arg2;
        }
    } 
    
    else if (!(D_800ED148.entities[arg1].status & CTRL_FLAG_40)) {
        arg2 = -arg2;
    }
    
    arg2 = -arg2;
    if (arg2 < 0) {
        D_800EE4C0.unkA |= 1;
        arg2 = ~arg2 + 1;
    }
    return arg2;
}

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

void func_8009C6E4(s32 arg0, s32 arg1, s32 arg2) {
    s32 temp_v1;
    s32 temp_v2;
    s32 temp_v3;
    
    D_800EE4C0.unk7 = arg0;
    temp_v1 = D_800EEBBA - D_800ED148.entities[arg1].unkB7;
    if (temp_v1 > 0) {
        temp_v2 = (arg2 * temp_v1) / 100;
        temp_v3 = func_8009C610(arg0, arg1, temp_v2);
        D_800EE4C0.unk14 = func_8009C6CC(temp_v3);
        return;
    }
    
    D_800EE4C0.unk14 = 0;
    D_800EE4C0.unkA |= 1;
}

void func_8009C798(s32 arg0, s32 arg1, s32 arg2) {
    s32 var_a2;
    
    if (D_800EE4C0.unkA & 1) {
        var_a2 = ~D_800EE4C0.unk14 + 1;
    } 
    
    else {
        var_a2 = D_800EE4C0.unk14;
    }

    var_a2 += (D_800ED148.entities[arg0].field2C / 10) * (900 - D_800ED148.entities[arg0].unk58) / 100;

    
    if (var_a2 < 0) {
        D_800EE4C0.unkA |= 1;
        var_a2 = ~var_a2 + 1;
    } 
    
    else {
        D_800EE4C0.unkA &= 0xFE;
    }
    
    D_800EE4C0.unk7 = arg0;
    D_800EE4C0.unk14 = func_8009C6CC(var_a2);
}

void func_8009C8B8(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    D_800EEBC4 &= 0xFBFFFFFF;
    
    if (D_800EEBC4 & BATTLE_ENTITY_FLAG_BIT_15) {
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
            D_800EE4C0.flags6 |= 4;
        }
        
    } else if (arg3 == 0) {
        D_800EE4C0.flags5 |= 1;
    }
}

void func_8009CA14(s32 arg0) {
    s32 i;

    if (D_800ED148.actionType != 1) {
        if (D_800ED148.actionType == 2) {
            for(i = 0; i < D_800ED148.actionByte1; i++){
                func_800AF358(arg0, D_800ED148.actionByte0, 1);
            }
        }
    } else {
        func_800AF654();
        addItemToInventory(D_800ED148.actionByte0, D_800ED148.actionByte1);
        func_800A864C();
    }
    D_800ED148.actionType = 0;
    D_800ED148.actionByte1 = 0;
    D_800ED148.actionByte0 = 0;
}

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

    if (entityDst->controlFlags & BATTLE_ENTITY_FLAG_BIT_11) {
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
            *(volatile s32 *)&entityDst->controlFlags |= BATTLE_ENTITY_FLAG_BIT_11;

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
    if ((D_800EE456 & 3) == 0) {
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
    if ((D_800EE456 & 3) == 0) {
        func_8009B924(a0, 0, 0x800000);
    }
}

s32 func_8009CD78(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 var;
    
    if ((D_800ED148.entities[arg1].flags & CTRL_FLAG_20) && (arg3 != 0)) {
        arg3 = arg3 >> 1;
        D_800EE4C0.flags5 |= CTRL_FLAG_10;
    }
    
    if (D_800ED148.entities[arg1].flags & 0x800000) {
        arg3 *= 2;
    }
    
    if (D_800ED148.unk1307 != 0) {
        arg3 *= 2;
    }
    
    if (D_800ED148.entities[arg1].status & CTRL_FLAG_40) {
        arg3 = arg3 >> 1;
    }
    
    if (D_800EEBB9 != 0) {
        var = func_8009C570(D_800EEBB9);
        arg3 += arg3 * D_800EEBB8 * (800 - func_8009C598(arg1, var)) / 10000;
    }
    
    func_8009C8B8(0, arg0, arg1, arg2, arg3);
    if (arg3 < 0) {
        D_800EE4C0.flags6 |= 1;
        arg3 = ~arg3 + 1;
    }
    
    return arg3;
}

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
s32 func_8009CF38(s32 attackerIdx, s32 targetIdx, s32 power, s32 type) {
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

    return func_8009CD78(attackerIdx, targetIdx, power, dmg);
}

s32 func_8009D174(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (func_8009BD60(arg1) == 0) {
        if ((func_8009BDD0(arg0, arg1) == 0) && (func_8009BEA4(arg0, arg1) == 0)) {
            D_800EE4C0.flags6 |= 4;
            return 0;
        }
        
        func_8009BE24(arg0, arg1);
        return func_8009CF38(arg0, arg1, arg2, arg3);
    }
    return 0;
}

s32 func_8009D228(s32 arg0, s32 arg1, s32 arg2) {
    s32 result1;
    s32 result2;
    s32 var1;
    s32 var2;
    s32 var3;
    s32 var_v1;

    
    if (D_800ED148.unk1292 != D_800ED148.unk1290) {
        D_800ED148.unk1300 = 1;
        D_800EE4C4 = 255;
    } 
    
    else if (D_800ED148.unk1303 != 0) {
        D_800ED148.unk1300 = 1;
        D_800ED148.unk1303 = 0;
        D_800EE4C4 = 255;
    } 
    
    else {
        D_800ED148.unk1300 = 0;
        if (D_800EE4C0.unk1 == 0xFB) {
            if (!(D_800ED148.entities[arg1].status & 1)) {
                D_800EE4C0.unk4 = 0xFF;
            }
                
            else{
                D_800EE4C0.unk4 = 4;
            }
        }
    }
    
    if (func_8009BD60(arg1) == 0) {
        D_800ED148.unk1307 = 0;
        result1 = func_8009C300(arg1, 0);
        result2 = func_8009CF18();
        
        var_v1 = D_800ED148.entities[arg0].fieldCD * D_800ED148.entities[arg0].fieldCD;
        if (var_v1 < 0) {
            var_v1 += 0xF;
        }
        
        var2 = ((var_v1 >> 4) + D_800ED148.entities[arg0].fieldCD) * (0x109 - result1);
        if (var2 < 0) {
            var2 += 0xFF;
        }
        
        var1 = (var2 >> 8) * arg2;
        if (var1 < 0) {
            var1 += 0x1F;
        }
        
        var3 = (var1 >> 5) * ((D_800ED148.unk1294 / 20) + 2) * result2;
        if (var3 < 0) {
            var3 += 0xFF;
        }
        
        return func_8009CD78(arg0, arg1, arg2, var3 >> 8);
    }
    return 0;
}

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
    if (!(D_800EEBC4 & BATTLE_ENTITY_FLAG_BIT_26)) {
        base = (s32)&D_800ED148;
        flags = *(u16 *)(base + a1 * 0xD0 + 0x90);
        if (flags & 0x4) {
            return 1;
        }
    }
    return 0;
}

s32 func_8009D474(s32 arg0, s32 arg1) {
    if ((D_800EEBC4 & BATTLE_ENTITY_FLAG_BIT_26) == 0) {
        if ((D_800ED148.entities[arg1].flags & (BATTLE_ENTITY_FLAG_BIT_21 | BATTLE_ENTITY_FLAG_BIT_11)) || (D_800ED148.entities[arg1].status & 1)) {
            return 1;
        }
        if ((D_800EEBB9 & 8) && (((D_800ED148.entities[arg1].flags & BATTLE_ENTITY_FLAG_BIT_13) != 0))) {
            return 1;
        }
    }
    
    return 0;
}

s32 func_8009D508(s32 arg0, s32 arg1) {
    if (!(D_800EEBC4 & BATTLE_ENTITY_FLAG_BIT_26)) { 
        if (D_800ED148.entities[arg1].flags & BATTLE_ENTITY_FLAG_BIT_21) {
            return 1;
        }
    
        if (D_800ED148.entities[arg1].status & 1) {
            return 1;
        }

        if (D_800ED148.entities[arg1].status & 1 || ((D_800EEBB9 & 8) && (D_800ED148.entities[arg1].flags & BATTLE_ENTITY_FLAG_BIT_13))) {
            return 1;
        }
    }

    return 0;
}

s32 func_8009D594(s32 unused, s32 arg1) {
    Struct_12CC* temp_v0;
    
    if ((D_800EE4C0.unk1 != 247) && (D_800ED148.unk130E & CTRL_FLAG_10)) {
        if (D_800ED148.entities[arg1].flags & CTRL_FLAG_80) {
            temp_v0 = &D_800ED148.array12CC[D_800ED148.unk130B];
            temp_v0->unk0 = D_800EE4C0.unk1;
            temp_v0->unk1 = D_800EE4C0.statusCode;
            temp_v0->unk2 = arg1;
            D_800ED148.unk130B += 1;
            D_800ED148.entities[arg1].controlFlags |= BATTLE_ENTITY_FLAG_BIT_14;
            D_800EE4C0.flags6 |= 4;
            D_800EE4C0.flags5 |= 49;
            return 1;
        }
    }
    D_800ED148.entities[arg1].controlFlags &= ~BATTLE_ENTITY_FLAG_BIT_14;
    return 0;
}

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

s32 func_8009D6C4(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (!(D_800ED148.entities[arg1].status & 4) && !(D_800ED148.entities[arg1].flags & BATTLE_ENTITY_FLAG_BIT_11)) {
        switch (arg3) {
            case 11:
                arg0 = (arg2 * 100) - D_800EEBBB;
                break;
            case 12:
                arg0 = D_800ED148.entities[arg1].field28 - 1;
                break;
            case 13:
                arg0 = ((D_800EEBBF * arg2 / 1000) + 1) * 1000;
                break;
            case 18:
                arg0 = 1;
                break; 
        } 
        return arg0;
    }

    return 0;
}

s32 func_8009D7D8(s32 arg0, s32 arg1, s32 arg2, u32 arg3) {
    s32 var_s0;
    s32 var_a0;
    s32 var_v0_2;
    s32 var_v1;
    s32 temp_v1;

    if (func_8009D594(arg0, arg1) != 0) {
        return 0;
    }

    if (func_8009D420(arg0, arg1) != 0 || func_8009D474(arg0, arg1) != 0) {
        D_800EE4C0.flags6 |= 4;
        return 0;
    }

    var_s0 = func_8009C300(arg1, 1);
    
    switch (arg3) {
        
    case 10:
        if ((D_800ED148.entities[arg1].unkCC % D_800EEBBB) != 0) {
            D_800EE4C0.flags6 |= 4;
            return 0;
        }
        
        goto case_0;
    case 6:
        
        var_s0 = 0;
        /* Fallthrough */
    case 0: case_0:
    
        temp_v1 = func_8009CF18();
        
        var_a0 = (D_800ED148.entities[arg0].fieldCF + arg2) * (265 - var_s0);
        
        var_v0_2 = ((var_a0 / 4 )* arg2) / 256;

        var_v1 = var_v0_2 * temp_v1 / 256;

        var_s0 = var_v1;
        if (arg0 >= 3) {
            var_s0 = var_v1 >> 1;
        }
        break;

    case 1:
        
        if (!(D_800ED148.entities[arg1].controlFlags & 0x10000)) {
            var_s0 = (D_800ED148.entities[arg1].field28 * arg2) / 16;
            break;
        } 
        
        var_s0 = 0;
        D_800EE4C0.flags6 |= 4;
        break;



    case 5:
        
        var_s0 = 0;
        /* Fallthrough */
    case 2:

       var_s0 = (((((((((D_800EEBBD + ((D_800EEBBE * D_800EEBBF) / 10) + arg2) * (265 - var_s0)) / 8) * arg2) / 256) * D_800EEBC8) / 100) * (D_800EEBC0 + 100) / 100) * func_8009CF18()) / 256;
        
        func_8009D68C();
        break;    

    case 4:
        
        if (D_800ED148.entities[arg1].controlFlags & 0x10000) {
            var_s0 = 0;
            D_800EE4C0.flags6 |= 4;
            break;
        }
        
        var_s0 = (D_800ED148.entities[arg1].field2C * D_800EEBBF) / ((D_800EEBBD + 100) - D_800EEBBE);
        break;

    case 17:
        var_s0 = ((u32)D_80082C14 / 1000) * arg2;
        break;

    default: 
        var_s0 = 0; 
        break;
    }
    
    if (((D_800ED148.unk130E & 3) == 1) && (var_s0 != 0) && (D_800ED148.entities[arg1].flags & CTRL_FLAG_40)) {
        var_s0 = var_s0 >> 1;
        D_800EE4C0.flags5 |= CTRL_FLAG_20;
    }
    
    if (D_800ED148.entities[arg1].flags & 0x80000) {
        var_s0 = var_s0 >> 1;
    }
    
    if (D_800EEBB9 != 0) {
        var_s0 = (var_s0 * (900 - func_8009C598(arg1, func_8009C570(D_800EEBB9)))) / 100;
    }

    func_8009C8B8(1, arg0, arg1, arg2, var_s0);
    
    if (var_s0 < 0) {
        D_800EE4C0.flags6 |= 1;
        var_s0 = ~var_s0 + 1;
    }
    
    return var_s0;
}

s32 func_8009DCCC(s32 unused, s32 arg1, s32 arg2) {
    if (D_800ED148.entities[arg1].status & CTRL_FLAG_40) {
        arg2 = -arg2;
    }
    
    if (arg2 < 0) {
        D_800EE4C0.flags6 &= 0xFE;
        arg2 = ~arg2 + 1;
    }
    
    return arg2;
}

void func_8009DD2C(s32 arg0, s32 arg1, u16 sp10, s32 arg3) {
    s32 i;
    BattleEntity* entities;
    BattleEntity* entity;
    s32 var_s0;
    s32 var_s2;


    entities = D_800ED148.entities;
    if (entities[arg0 + 1].slot8.byteView.unk09 == 0) {
        func_8009B878(arg0, &sp10, &D_800EEBC4, 0);
        var_s2 = 0;
        
        var_s0 = 1;
        for(i = 0; i < 7; i++) {
            if ((sp10 & var_s0) && (entities[arg0].status & var_s0)) {
                var_s2 = 1;
            }
            
            var_s0 *= 2;
        }

        var_s0 = 1;
        for(i = 8; i < 40; i++) {
            if ((arg3 & var_s0) && (D_800ED148.entities[arg0].flags & var_s0) && (func_800B0668(arg0, var_s0) == 0)) {
                var_s2 = 1;
            }
            
            var_s0 *= 2;
        }

        
        if ((sp10 == 0) && (arg3 == 0)) {
            var_s2 = 1;
        }

        
        if (var_s2 == 0) {
            if (arg1 == 0) {
                D_800EE4C0.flags6 |= 4;
            }
        } else if (arg1 == 0) {
            D_800EE4C0.flags5 |= 1;
        }
        func_8009B924(arg0, sp10, arg3);
    }
}

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

    if (func_8009D594(arg0, arg1) != 0) {
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

s32 func_8009E110(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 var_s1;
    u8 temp_s0;
    u8 temp_s0_2;

    D_800EE4C0.flags6 |= 1;
    temp_s0 = D_800EEBBB;
    if (func_8009B7BC(100) < temp_s0) {
        switch (arg3) {
        case 14:
            var_s1 = arg2 * 50;
            break;
        case 15:
            var_s1 = (D_800ED148.entities[arg1].field2C * arg2) / 16;
            break;
        case 9:
            var_s1 = D_800ED148.entities[arg0].field2C - g_battleChars.chars[arg0].field172;
            break;
        }
        if (((D_800EE456 & 3) == 2) && (arg0 < 3)) {
            if (g_battleChars.chars[arg0].statusFlags & 2) {
                var_s1 *= 2;
            }
        }
        
        var_s1 = func_8009DCCC(arg0, arg1, var_s1);
        temp_s0_2 = D_800EEBBA;
        if (func_8009B7BC(100) < temp_s0_2) {
            func_8009DD2C(arg1, arg2, D_800EEBC2, D_800EEBC4);
        }
        return var_s1;
    }
    D_800EE4C0.flags6 |= 4;
    return 0;
}

s32 func_8009E33C(s32 arg0, s32 arg1, s32 arg2) {
    D_800ED148.unk1329 = 1;
    if (D_800ED148.entities[arg1].status & CTRL_FLAG_40) {
        return func_8009D7D8(arg0, arg1, arg2, 0);
    }
    
    D_800EE4C0.flags6 |= 1;
    
    if (!(D_800ED148.entities[arg1].status & 1)) {
        D_800EE4C0.flags6 |= 5;
        return 0;
    }
    
    if (D_80082C10 & CTRL_FLAG_40) {
        D_800EE4C0.flags6 |= 5;
    }
    else {
        D_800ED148.entities[arg1].status &= 0xFFFE;
    }
    
    D_800EE4C0.flags5 |= 4;
    return -100000;
}

s32 func_8009E418(s32 arg0, s32 arg1, s32 arg2) {
    s32 temp_a0;
    s32 var_a1;
    s32 ret;

     if (func_8009D594(arg0, arg1) != 0) {
         return 0;
     }

    ret = func_8009E33C(arg0, arg1, arg2);
    if (ret != -100000) {
        return ret;
    }
    
    temp_a0 = D_800ED148.entities[arg1].field2C;

    var_a1 = temp_a0 / 8;
    if ((D_800EE4C1 == 4 || D_800EE4C1 == 13) && arg0 < 3 && g_battleChars.chars[arg0].statusFlags & 2) {
        var_a1 = temp_a0 / 4;
    }
    
    if (var_a1 == 0) {
        return 1;
    }
        
    return var_a1;
}

s32 func_8009E528(s32 arg0, s32 arg1, s32 arg2) {
    s32 ret;
    
    if (func_8009D594(arg0, arg1) != 0) {
        return 0;
    }

    ret = func_8009E33C(arg0, arg1, arg2);
    if (ret != -100000) {
        return ret;
    }
    
    if (D_800ED148.entities[arg1].field2C == 0) {
        return 1;
    }
    
    return D_800ED148.entities[arg1].field2C;
}

void func_8009E5C0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 sp10;
    s32 var_s1;
    s32 temp_s0_2;
    s32 temp_s1;
    s32 var_s2;
    
    if (arg2 == 0) {
        var_s2 = arg1 - arg0;    
        var_s1 = 0x5F;
    }
        
    else {
        var_s2 = arg0 - arg1;
        var_s1 = 0x59;
    }
    
    temp_s0_2 = func_800B0248(func_800B0074(arg3), 7U, getMenuString(0x58));
    temp_s1 = func_800B0248(temp_s0_2, 7U, getMenuString(var_s1));
    func_800A4320(func_800B02AC(func_800B0248(temp_s1, *getMenuString(0xB), func_800B04A0(var_s2, &sp10))));
}

s32 func_8009E684(s32 unused, s32 arg1, s32 arg2, s32 arg3) {
    s32 var_a0;
    u8 temp_a1;
    u8 temp_s0;

    if (!(D_800ED148.entities[arg1].controlFlags & BATTLE_ENTITY_FLAG_BIT_15)) {
        temp_s0 = D_800EEBBA;
        
        if (func_8009B15C() < temp_s0) {
            temp_a1 = D_800ED148.entities[arg1].unkCC;
            
            var_a0 = temp_a1 * arg2 / 8; 
            if (var_a0 == 0) {
                var_a0 = 1;
            }

            if (var_a0 >= 101) {
                var_a0 = 100;
            }
            
            D_800ED148.entities[arg1].unkCC = var_a0;
            func_8009E5C0(var_a0, temp_a1, arg3, arg1);
            func_800A8430(arg1);
            D_800ED148.unk1318 = 1;
            D_800EE4C0.flags5 |= 1;
        } 
        
        else {
            D_800EE4C0.flags6 |= 4;
        }
    } 
        
    else {
        D_800EE4C0.flags6 |= 4;
    }
   
    return 0;
}

s32 func_8009E7B0(void) {
    s32 var_v0_2;
    u8 temp_v1_2;
    s32 temp_v1_3;
    
    temp_v1_2 = func_80042634(-1);

    if (temp_v1_2 < 128) {
        var_v0_2 = func_8009B15C();
        
        temp_v1_3 = var_v0_2;
        if (temp_v1_3 < 0) {
            var_v0_2 = temp_v1_3 + 7;
        }
        
        return (temp_v1_3 - ((var_v0_2 >> 3) * 8)) + 1;
    }

    if (temp_v1_2 < 160) {
        return (func_8009B15C() % 98) + 101;
    }

    if (temp_v1_2 < 176) {
        return (func_8009B15C() % 23) + 9;
    }

    if (temp_v1_2 < 192) {
        return (func_8009B15C() % 34) + 66;
    }

    if (temp_v1_2 < 200) {
        var_v0_2 = func_8009B15C();
        
        temp_v1_3 = var_v0_2;
        if (temp_v1_3 < 0) {
            var_v0_2 = temp_v1_3 + 31;
        }
        
        return temp_v1_3 - ((var_v0_2 >> 5) << 5) + 33;
    } 
    
    return func_8009B15C() % 7 + 33;
}

s32 func_8009E95C(void) {
    s32 temp_s0;

    D_800ED148.actionType = 1;
    D_800ED148.actionByte0 = func_8009E7B0();
    D_800ED148.actionByte1 = 1;
    D_800EE4C0.flags5 |= 1;

    temp_s0 = func_800B0248(getMenuString(0x47), 7, getStatName(D_800ED148.actionByte0));
    func_800A4320(func_800B02AC(func_800B0248(temp_s0, 7, getMenuString(0x40))));
    return 0;
}

s32 func_8009EA08(s32 unused, s32 arg1) {
    s32 var_a1;
    s32 index;
    TaskEntry sp10; // not confirmed to be this type
    
    D_800EE4C0.flags5 |= 1;

    if (!(D_800ED148.entities[arg1].status & 1)) {
        
        var_a1 = func_800AF918(arg1, &sp10);
        
        if (var_a1 != 0) {
            for (index = 0; index < var_a1; index++){
                D_800ED148.entities[arg1].controlFlags |= 0x20000;
            }
        }
    }
    
    if (D_800ED148.unk1322 == 0) {
        func_800A4320(getMenuString(0x36));
        D_800ED148.unk1322 += 1;
    }
    
    return 0; 
}

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

    if (!(D_800ED148.entities[charIdx].controlFlags & BATTLE_ENTITY_FLAG_BIT_17)) {
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
    D_800ED148.entities[charIdx].controlFlags &= ~BATTLE_ENTITY_FLAG_BIT_17;
}



s32 func_8009ED2C(s32 arg0) {
    BattleEntityData* temp_s0;
    
    temp_s0 = D_800ED148.entities[arg0].linkedPtr->data;
    if((temp_s0->unkF9 == 255) && (temp_s0->unkFA == temp_s0->unkF9)) {
        return 0;
    }
    
    if (func_8009B79C(256 - (D_800ED148.entities[arg0].field28 * 255 / D_800ED148.entities[arg0].field2C), 255) != 0) {
        if (func_8009B15C() < 16) {
            D_800ED148.unk1315 = temp_s0->unkFA;
        }
        
        else {
            D_800ED148.unk1315 = temp_s0->unkF9;
        }

        if (markItemPresent(D_800EE45D) != 0) {
            return 0;
        }
        
    }
        
    else {
        D_800ED148.unk1315 = 255;
    }
    return 1;
}

void func_8009EE44(s32 unused, s32 arg1) {
    u8* temp_s0_2;
    u8* temp_s1_2;

    if (D_800ED148.unk1315 == 255) {
        D_800EE4C0.flags6 |= 4;
        return;
    }
    D_800ED148.entities[arg1].flags |= 0x10000;
    D_800EE4C0.flags5 |= 1;
    temp_s0_2 = func_800B0248(getMenuString(9), *getMenuString(0xB), getMenuString(0x47));
    temp_s1_2 = func_800B0248(temp_s0_2, 7, func_80023A54(D_800ED148.unk1315));
    func_800A4320(func_800B02AC(func_800B0248(temp_s1_2, *getMenuString(0xB), getMenuString(0x10))));
}

s32 func_8009EF64(s32 arg0, s32 unused, s32 unused2, s32 arg3) {
    BattleEntityData* curr;

    curr = D_800ED148.entities[arg0].linkedPtr->data;
    switch (D_800EE9E8.subEntries[arg0 - 3].unk43) {
    case 0:
        arg3 = curr->unkFB;
        break;
    case 1:
        arg3 = curr->unkFC;
        break;
    case 2:
        arg3 = curr->unkFD;
        break;
    }
    
    if (arg3 == 255) {
       D_800EE46E = 0;
        return 0; 
    }
    
    D_800ED148.unk1326 = 1;
    D_800ED148.unk1327 = arg3;
    D_800ED148.unk1328 = curr->unkFF;
    return 1; 
}

s32 func_8009F040(s32 arg0, s32 arg1, s32 unused) {
    s32 entity0;
    s32 entity1;
    

    entity0 = D_800ED148.entities[arg0].field28;
    entity1 = D_800ED148.entities[arg1].field28;
    
    if (entity0 >= entity1) {
        if (func_8009B79C(((entity0 - entity1) * 255 / entity0), 255) != 0) {
            D_800ED148.entities[arg1].flags |= 0x10000;
            D_800ED148.unk1326 = 1;
            D_800EE4C0.flags5 |= 1;
            
        
            func_800A4320(resolveKernelPtr(
                D_80078E00.unk4C0C[D_800ED148.unk1327].lookupId, 
                D_80078E00.unk4C0CArg, 
                &D_80078E00
            ));
        }
            
        else {
            D_800ED148.unk1326 = 0;
            D_800ED148.unk1328 = 8;
            D_800EE4C0.flags6 |= 4;
        }
    }
        
    else {
        D_800ED148.unk1326 = 0;
        D_800ED148.unk1328 = 8;
        D_800EE4C0.flags6 |= 4;
    }

    return D_800EE470;
}

void func_8009F168(s32 arg0, s32 arg1) {
    u8 temp_s0;
     
    temp_s0 = D_80078E00.unk4C0C[arg1].flags;
    if (temp_s0 & 1) {
        func_8002153C(arg0, 0);
    }

    if (temp_s0 & 2) {
        func_8002153C(arg0, 1);
    }

    if (temp_s0 & 4) {
        func_8002153C(arg0, 2);
    }

    if (temp_s0 & 8) {
        func_8002153C(arg0, 3);
    }

    if (temp_s0 & CTRL_FLAG_10) {
        func_8002153C(arg0, 4);
    }

    if (temp_s0 & CTRL_FLAG_20) {
        func_8002153C(arg0, 5);
    }

    addCharMaxHp(arg0, D_80078E00.unk4C0C[arg1].maxHP);
}

s32 func_8009F23C(s32 arg0, s32 arg1) {
    s32 var_s2;
    s32 temp_a3;

    temp_a3 = D_80078E00.unk4C0C[D_800ED148.unk1327].unk4;
    var_s2 = D_800ED148.entities[arg1].field2C * temp_a3 / 16;
    
    switch (D_80078E00.unk4C0C[D_800ED148.unk1327].unk3) {
    case 30:
        D_800EE4C0.flags6 |= 1;
        func_8009DD2C(arg1, temp_a3, D_800EEBC2, D_800EEBC4);
        func_8009F168(arg1, D_800ED148.unk1327);
        break;
    case 31:
        func_8009C8B8(1, arg0, arg1, temp_a3, var_s2);
        break;
    }
    
    return var_s2;
}

s32 func_8009F350(s32 arg0) {
    s32 val;
    s32 calc;

    val = func_8009B15C();
    D_800EE4C4 = 5;

    val = (val % 5);

    do {} while(0); // hack
    
    return D_800ED148.entities[arg0].field2C * (val + 5) / 100;
}

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

s32 func_8009F4BC(s32 arg0) {
    if (!(D_800ED148.entities[arg0].status & CTRL_FLAG_40)) {
        D_800EE4C0.flags6 |= 1;
    }
    return D_800ED148.entities[arg0].field2C / 20;
}

/**
 * @brief Test a bit in the battle flag array at g_gameState+0xD0C.
 *
 * Computes the word index and bit position from the given bit index,
 * then tests whether the corresponding bit is set.
 *
 * @param bitIndex The bit index to test.
 * @return 1 if the bit is set, 0 otherwise.
 */
 
s32 func_8009F52C(s32 arg0) {
    s32 var2;
    s32 var;

    var = arg0 / 32;
    var2 = arg0 % 32;
    
    if ((g_gameState.mainData.array[var] & 1 << var2) != 0) {
        return 1;
    }
    
    return 0;
}

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

void func_8009F5B4(s32 arg0) {
    if (!(D_80077E5C & CTRL_FLAG_100) && (func_8009F52C(D_800ED148.entities[arg0].linkedIdx) != 0)) {
        D_800EE4C0.flags5 |= 2;
    }
    func_8009F570(D_800ED148.entities[arg0].linkedIdx);
}

s32 func_8009F65C(s32 arg0, s32 arg1) {
    s32 offset;
    s32 i;
    BattleEntityData* data;
    
    data = D_800ED148.entities[arg0].linkedPtr->data;
    offset = D_800EE9E8.subEntries[arg0 - 3].unk43 * 8;

    for (i = 0; i < 4; i++, offset += 2) {
        if (arg1 == data->unk104[offset]) {
            return data->unk104[offset + 1];
        }
    }
    
    return 1;
}

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

s32 func_8009F718(s32 arg0, s32 arg1, s32 arg2) {
    s32 temp_s3;
    s32 temp_s4;
    s32 result;
    s32 temp;
    s32 var;
    s32 var2;
    s32 var3;
    
    temp_s4 = func_8009B7BC(32);
    temp_s3 = func_8009F6F4(arg2);
    temp = func_8009F65C(arg1, arg2);
    var = 10;
    
    var2 = D_800ED148.entities[arg0].unkCC - (D_800ED148.entities[arg1].unkCC - var);
    var3 = (var2 >> 1) + temp_s4 + D_800ED148.entities[arg0].fieldCF;
    result = ((var3 - temp_s3) / 5) - temp;
    
    if (result < 0) {
        result = 0;
    }
    
    if (result > 9) {
        result = 9;
    }
    
    return result;
}

void func_8009F824(void) {
    if (D_800EE4C0.flags6 & 4) {
        if (D_800ED148.entities[D_800ED148.unk12F3].unk99 == 0) {
            D_800EE4C0.unk4 = 9;
        } 
        
        else {
            D_800EE4C0.unk4 = 0;
        }
    }
    
    if ((D_800EE4C0.flags6 & 2) && (D_800EE4C0.unk4 != 255)) {
        D_800EE4C0.unk4 = 6;
    }
    if (!((D_800EE4C0.flags5 & 1) || (D_800EE4C0.flags6 & 4))) {
        if (!(D_800EE4C0.flags6 & 1)) {
            if (D_800EE4C0.unk4 == 0) {
                D_800EE4C0.unk4 = 5;
            }
        } 
            
        else {
            D_800EE4C0.unk4 = 0;
        }
    }
    
    if (D_800EE4C0.unk4 == 255) {
        D_800EE4C0.unk4 = 0;
    }
}

s32 func_8009F930(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 var_s1;
    
    func_8009CD10(arg2);
    switch (arg0) {     
        case 10:
            var_s1 = func_8009D228(arg1, arg2, arg3);
            break;
        
        case 1:
            var_s1 = func_8009D174(arg1, arg2, arg3, 0);
            break;
            
        case 2:
            var_s1 = func_8009D7D8(arg1, arg2, arg3, 0);
            break;
            
        case 3:
            var_s1 = func_8009DEF0(arg1, arg2, arg3, 7);
            break;
            
        case 4:
            var_s1 = func_8009E110(arg1, arg2, arg3, 14);
            break;
            
        case 5:
            var_s1 = func_8009E418(arg1, arg2, arg3);
            break;
            
        case 6:
            var_s1 = func_8009E528(arg1, arg2, arg3);
            break;
            
        case 7:
            var_s1 = func_8009D174(arg1, arg2, arg3, 1);
            break;
            
        case 8:
            var_s1 = func_8009D7D8(arg1, arg2, arg3, 1);
            break;
            
        case 22:
            var_s1 = func_8009D7D8(arg1, arg2, arg3, 6);
            break;
            
        case 9:
            var_s1 = func_8009D174(arg1, arg2, arg3, 0);
            break;
            
        case 20:
            var_s1 = func_8009D7D8(arg1, arg2, arg3, 4);
            break;
            
        case 11:
            var_s1 = func_8009D7D8(arg1, arg2, arg3, 2);
            break;
            
        case 15:
            var_s1 = func_8009D7D8(arg1, arg2, arg3, 5);
            break;
            
        case 12:
            func_8009F5B4(arg2);
            /* fallthrough */
        case 0:
            var_s1 = 0;
            D_800EE4C0.flags5 |= 1;
            break;
            
        case 13:
            var_s1 = func_8009E684(arg1, arg2, arg3, 0);
            break;
            
        case 16:  
            var_s1 = D_800ED148.entities[arg2].field2C - D_800ED148.entities[arg2].field28;
            func_8009E684(arg1, arg2, arg3, 1);
            D_800ED148.entities[arg2].field28 = D_800ED148.entities[arg2].field2C - var_s1;
            var_s1 = 0;
            break;
            
        case 18:
            var_s1 = func_8009D174(arg1, arg2, arg3, 3);
            break;
            
        case 19:
            var_s1 = func_8009F040(arg1, arg2, arg3);
            break;
            
        case 21:
            var_s1 = func_8009DEF0(arg1, arg2, arg3, 8);
            break;
            
        case 17:
            var_s1 = 0;
            func_8009EE44(arg1, arg2);
            break;
            
        case 23:
            var_s1 = func_8009E95C();
            break;
            
        case 25:
            var_s1 = func_8009E110(arg1, arg2, arg3, 9);
            break;
            
        case 24:
            var_s1 = func_8009EA08(arg1, arg2);
            break;
    
        case 26:
            var_s1 = func_8009D7D8(arg1, arg2, arg3, 10);
            break;
            
        case 27:
            var_s1 = func_8009D6C4(arg1, arg2, arg3, 11);
            break;
            
        case 28:
            var_s1 = func_8009D6C4(arg1, arg2, arg3, 12);
            break;
            
        case 29:
            var_s1 = func_8009D6C4(arg1, arg2, arg3, 13);
            break;
            
        case 32:
            var_s1 = func_8009E110(arg1, arg2, arg3, 15);
            break;
            
        case 33:
            var_s1 = func_8009D7D8(arg1, arg2, arg3, 17);
            break;
            
        case 34:
            var_s1 = func_8009D174(arg1, arg2, arg3, 16);
            break;
            
        case 35:
            var_s1 = func_8009D6C4(arg1, arg2, arg3, 18);
            break;
            
        case 36:
            var_s1 = func_8009D174(arg1, arg2, arg3, 19);
            break;
            
        default:
            var_s1 = 0;
            break;      
    }
    
    func_8009CD44(arg2);
    return var_s1;
}

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

s32 func_8009FD28(s32 arg0, s32 arg1) {
    u8 var;

    func_8009FCF4(D_80078E00.spells[D_800EE4C0.statusCode].unk7);
    var = D_80078E00.spells[D_800EE4C0.statusCode].unk4;
    D_800EE4C0.unk4 = D_80078E00.spells[D_800EE4C0.statusCode].unk2;
    return func_8009F930(D_80078E00.spells[D_800EE4C0.statusCode].unk3, arg1, arg0, var);
}

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

void func_8009FE14(s32 arg0) {
    BattleEntity* temp_v0_2;
    BattleSystem* bs;
    s32 index;
    
    D_800EEBBB = 255;
    D_800EEBBC = 0;
    D_800EEBB8 = 0;
    
    switch (D_800EE4C1) {
    case 239:
        D_800EEBB8 = D_80078E00.array48BC[D_800EE4C0.statusCode].unk48C6;
        D_800EEBB9 = D_80078E00.array48BC[D_800EE4C0.statusCode].unk48C5;
        D_800EEBBA = D_80078E00.array48BC[D_800EE4C0.statusCode].unk48C7;
        D_800EEBC2 = D_80078E00.array48BC[D_800EE4C0.statusCode].unk48D2;
        D_800EEBC4 = D_80078E00.array48BC[D_800EE4C0.statusCode].unk48D4;
        break;

    case 241:
    {
        s32 val_min = 65530;
        s32 val_max = 65532;
        if ((D_800EE4DC <= val_max) && (D_800EE4DC >= val_min)) {
            D_800EEBB8 = 0;
            D_800EEBB9 = 0;
            D_800EEBBA = 0;
            D_800EEBC2 = 0;
            D_800EEBC4 = 0;
        }
            
        else { 
            D_800EEBB8 = D_80078E00.array48BC[D_800EE4C0.statusCode].unk48C6;
            D_800EEBB9 = D_80078E00.array48BC[D_800EE4C0.statusCode].unk48C5;
            D_800EEBBA = D_80078E00.array48BC[D_800EE4C0.statusCode].unk48C7;
            D_800EEBC2 = D_80078E00.array48BC[D_800EE4C0.statusCode].unk48D2;
            D_800EEBC4 = D_80078E00.array48BC[D_800EE4C0.statusCode].unk48D4;
        }
        
        break;
    }
        
    case 240:
    case 244:
    case 245:
        D_800EEBC0 = 0;
        D_800EEBBB = D_80078E00.entriesA0[D_800EE4C0.statusCode].unk13;
        D_800EEBB9 = D_80078E00.entriesA0[D_800EE4C0.statusCode].unkB;
        D_800EEBBA = D_80078E00.entriesA0[D_800EE4C0.statusCode].unk6;
        D_800EEBC2 = D_80078E00.entriesA0[D_800EE4C0.statusCode].unk10;
        D_800EEBC4 = D_80078E00.entriesA0[D_800EE4C0.statusCode].unkC;
        break;
        
    case 254:
        D_800EEBC0 = g_battleChars.levelEntries[D_800EE4C0.statusCode - 64].unk1;
        D_800EEBB9 = D_80078E00.rows132[D_800EE4C0.statusCode - 64].unkD;
        D_800EEBBA = D_80078E00.rows132[D_800EE4C0.statusCode - 64].unk1B;
        D_800EEBC2 = D_80078E00.rows132[D_800EE4C0.statusCode - 64].unkE;
        D_800EEBC4 = D_80078E00.rows132[D_800EE4C0.statusCode - 64].unk10;
        break;
        
    case 7:  
    case 23: 
    case 24: 
    case 25: 
    case 26: 
    case 27: 
    case 29: 
    case 30: 
    case 31: 
    case 32: 
    case 33: 
    case 34: 
    case 38: 
        D_800EEBB9 = D_80078E00.array4020[D_800EE4C0.statusCode].unk0;
        D_800EEBBA = D_80078E00.array4020[D_800EE4C0.statusCode].unk1;
        D_800EEBC2 = D_80078E00.array4020[D_800EE4C0.statusCode].unk2;
        D_800EEBC4 = D_80078E00.array4020[D_800EE4C0.statusCode].unk4;
        break;
        
    case 2:
    case 6:
    case 16:
    case 247:
        D_800EEBB9 = D_80078E00.spells[D_800EE4C0.statusCode].unkA;
        D_800EEBBA = D_80078E00.spells[D_800EE4C0.statusCode].unk12;
        D_800EEBC2 = D_80078E00.spells[D_800EE4C0.statusCode].unk10;
        D_800EEBC4 = D_80078E00.spells[D_800EE4C0.statusCode].unkC;
        break;
        
    case 19:
        D_800EEBB8 = D_80078E00.array4A70[D_800EE4C0.unk3].unk4;
        D_800EEBB9 = D_80078E00.array4A70[D_800EE4C0.unk3].unk3;
        D_800EEBBA = D_80078E00.array4A70[D_800EE4C0.unk3].unk5;
        D_800EEBC2 = D_80078E00.array4A70[D_800EE4C0.unk3].unk6;
        D_800EEBC4 = D_80078E00.array4A70[D_800EE4C0.unk3].unk8;
        break;
        
    case 17:
    case 18:
    case 20:
    case 21:
    case 22:
        D_800EEBB8 = D_80078E00.array4484[D_800EE4C0.statusCode].unk2;
        D_800EEBB9 = D_80078E00.array4484[D_800EE4C0.statusCode].unk1;
        D_800EEBBA = D_80078E00.array4484[D_800EE4C0.statusCode].unk3;
        D_800EEBC2 = D_80078E00.array4484[D_800EE4C0.statusCode].unk4;
        D_800EEBC4 = D_80078E00.array4484[D_800EE4C0.statusCode].unk8;
        break;
        
    case 15:
        D_800EEBB9 = D_80078E00.array44FC[D_800EE4C0.statusCode].unk0;
        D_800EEBBA = D_80078E00.array44FC[D_800EE4C0.statusCode].unk1;
        index = func_8009FDE0(arg0, D_800EE4C0.statusCode);
        D_800EEBC2 = D_80078E00.array45F8[index].unk4;
        D_800EEBC4 = D_80078E00.array45F8[index].unk0;
        D_800EEBBB = D_80078E00.array45F8[index].unk7;
        D_800EEBBC = D_80078E00.array44FC[D_800EE4C0.statusCode].unk2;
        break;
        
    case 14:
    case 237:
    case 238:
        D_800EEBB8 = D_80078E00.array47FC[D_800ED148.unk1324].unk2;
        D_800EEBB9 = D_80078E00.array47FC[D_800ED148.unk1324].unk1;
        D_800EEBBA = D_80078E00.array47FC[D_800ED148.unk1324].unk3; 
        D_800EEBC2 = D_80078E00.array47FC[D_800ED148.unk1324].unk4;
        D_800EEBC4 = D_80078E00.array47FC[D_800ED148.unk1324].unk8;
        D_800EEBBC = D_80078E00.array47FC[D_800ED148.unk1324].unk7;
        break;
        
    case 4:
    case 13:
        D_800EEBBB = D_80078E00.abilities[D_800EE4C0.statusCode].unkC;
        D_800EEBB9 = D_80078E00.abilities[D_800EE4C0.statusCode].unk11;
        D_800EEBBA = D_80078E00.abilities[D_800EE4C0.statusCode].unk5;
        D_800EEBC2 = D_80078E00.abilities[D_800EE4C0.statusCode].unk09;
        D_800EEBC4 = D_80078E00.abilities[D_800EE4C0.statusCode].unkA;
        break;
        
    case 8:
    case 236:
        D_800EEBB9 = D_80078E00.entries17[D_800EE4C0.statusCode].unkA;
        D_800EEBBA = D_80078E00.entries17[D_800EE4C0.statusCode].unkC; 
        D_800EEBC2 = D_80078E00.entries17[D_800EE4C0.statusCode].unkE;       
        D_800EEBC4 = D_80078E00.entries17[D_800EE4C0.statusCode].unk10;
        D_800EEBBB = D_80078E00.entries17[D_800EE4C0.statusCode].unkD;
        D_800EEBBC = D_80078E00.entries17[D_800EE4C0.statusCode].unkB;
        break;
    
    case 0:
        switch (D_800EE4DC) {
        case 10:
            D_800EEBB9 = 0;    
            D_800EEBBA = 200;
            D_800EEBC2 = D_80078E00.unk4C0C[D_800ED148.unk1327].unk8;
            D_800EEBC4 = D_80078E00.unk4C0C[D_800ED148.unk1327].unk5;
            break;
            
        case 8:
            D_800EEBB9 = 0;
            D_800EEBBA = 255;
            D_800EEBC2 = 1;
            D_800EEBC4 = 0;
            break;
            
        }
        
        break;
        
    case 249:
        D_800EEBB8 = D_80078E00.array3750[D_800EE4C0.unk3].unk2;
        D_800EEBB9 = D_80078E00.array3750[D_800EE4C0.unk3].unk1;
        D_800EEBBA = D_80078E00.array3750[D_800EE4C0.unk3].unk3;
        D_800EEBC2 = D_80078E00.array3750[D_800EE4C0.unk3].unk6;
        D_800EEBC4 = D_80078E00.array3750[D_800EE4C0.unk3].unk8;
        break;

    case 251:
        D_800EEBB9 = 0;
        D_800EEBBA = 0;
        D_800EEBC2 = 0;
        D_800EEBC4 = 0;
        break;
        
    default:
        bs = &D_800ED148;
        temp_v0_2 = &bs->entities[arg0];
        D_800EEBB8 = temp_v0_2[1].state.bytes.b2;
        D_800EEBB9 = temp_v0_2[1].state.bytes.b1;
        D_800EEBBA = temp_v0_2[0].unkCA;
        D_800EEBC2 = temp_v0_2[0].unk96;
        D_800EEBC4 = temp_v0_2[0].unk30;
        D_800EEBBB = temp_v0_2[1].state.bytes.b0;
        D_800EEBBC = D_80078E00.array35BD[g_battleChars.chars[arg0].classId].unk0;
        break;
    }
}

s32 func_800A085C(void) {
    s32 i;
    u16 total = 0;
    s32 count = 0;

    for (i = 0; i < 16; i++) {
        if (g_gameState.gfs[i].exists & 1) {
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

void func_800A08E0(s32 a0, s32 a1) {
    u8 temp_a0;

    if (D_800ED148.unk1319 != 0xFF) {
        temp_a0 = D_800EE4C0.flags6 & 0x3F;
    
        switch (D_800ED148.unk1319) {
            case 0:
                D_800EE4C0.flags6 = temp_a0;
                break;
            case 1:
                D_800EE4C0.flags6 = temp_a0 | CTRL_FLAG_40;;
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

void func_800A09D0(u32 arg0) {
    BattleSystem* bs;
    BattleEntity* temp_a0;
    s32 temp_s4;
    u8 var_a0;
    u8 var_s3;

    bs = &D_800ED148;
    temp_a0 = &bs->entities[arg0];
    temp_a0->controlFlags &= ~0x4000;
    if (temp_a0[1].slot8.byteView.unk09 != 0) {
        temp_a0[1].slot8.byteView.unk09-= 1;
    }
    
    D_800ED148.unk1329 = 0;
    D_800ED148.unk12F3 = arg0;
    func_8009FCF4(3);
    D_800EE4C0.unk18 = 0;
    D_800EE4C0.unk20 = 0;
    D_800EE4C0.unkA = 0;
    D_800EE4C0.unk09 = 0;
    D_800EE4C0.unk14 = 0;
    D_800EE4C0.unk08 = 0;
    D_800EE4C0.unk4 = 0;
    D_800EE4C0.unk10 = 0;
    D_800EE4C0.unk1E = 0;
    D_800EE4C0.flags6 = 0;
    D_800EE4C0.flags5 = 0;
    D_800EE4C0.unk0C = 0;
    temp_s4 = D_800EE4C0.unk0;
    D_800EE4C0.unk7 = 255;
    func_8009FE14(temp_s4);
    
    switch (D_800EE4C0.unk1) {
    case 0x0:        
        D_800EE43D = 1;
        switch (D_800EE4DC) {
        case 10:
            D_800EE4CC = func_8009F23C(temp_s4, arg0);
            break;
            
        case 8:
            D_800EE4CC = func_8009E418(temp_s4, arg0, 0);
            break;
            
        case 4:
            D_800EE4CC = func_8009F4BC(arg0);
            break;
            
        case 2:
            D_800EE4CC = func_8009F350(arg0);
            break;
            
        case 5:
            D_800EE4CC = func_8009F3F8(arg0);
            break;
            
        case 6:
            D_800EE4CC = func_8009F428(arg0);
            break;
            
        case 7:
            D_800EE4CC = func_8009F46C(arg0);
            break;
            
        case 3:
            D_800EE4C0.flags5 |= 1;
            D_800EE4C0.unk4 = D_800EE4C0.unk2;
            break;         
        }
        
    default:         
        break;
        
    case 0x5:        
    case 0xB:        
    case 0xFA:       
        D_800EE448 = 1;
        break;
        
    case 0xEF:       
        D_800EE448 = 0;
        goto not_another_goto;
        
    case 0xF1:       
        switch (D_800EE4C0.statusCode) {     
        case 0xFFFA:
            D_800EE448 = 1;
            D_800EE4C0.flags5 |= 1;
            break;
            
        case 0xFFFC:
            D_800EE4C0.unk7 = temp_s4;
            D_800EE448 = 0;
            D_800EE4C0.flags5 |= 1;
            D_800EE4C0.unk09 |= 1;
            break;
            
        default:    
            D_800EE448 = 1;
            goto not_another_goto;
        }       
        break;
        
        not_another_goto:
        func_8009FCF4(D_80078E00.array48BC[D_800EE4C0.statusCode].unk48C3);
        var_s3 = D_80078E00.array48BC[D_800EE4C0.statusCode].unk48BF;
    
        D_800EE4C0.unk4 = D_80078E00.array48BC[D_800EE4C0.statusCode].unk48C0;
        D_800EE4C0.unk0C = func_8009F930(D_80078E00.array48BC[D_800EE4C0.statusCode].unk48BE, temp_s4, arg0, var_s3);
        break;
    
    case 0xC:        
        func_8009CAD8(temp_s4, arg0);
        /* fallthrough */
    case 0x1:        
    case 0x1C:
        if (D_800ED148.entities[temp_s4].linkedIdx != 0 && D_800ED148.entities[temp_s4].linkedIdx != 6) {
            func_8009FCF4(0);
            var_s3 = D_80078E00.array35BD[g_battleChars.chars[temp_s4].classId].unk2;
            D_800EE4C0.unk4 = 4;
            var_a0 = D_80078E00.array35BD[g_battleChars.chars[temp_s4].classId].unk1;
            D_800EE4C0.unk0C = func_8009F930(var_a0, temp_s4, arg0, var_s3);
        } 
            
        else {
            D_800EE448 = 1;
        }
        
        func_800A0978(temp_s4);
        break;
        
    case 0x1D:       
        func_8009FCF4(D_80078E00.array4020[D_800EE4C0.statusCode].unk13);
        var_s3 = D_80078E00.array4020[D_800EE4C0.statusCode].unk12;
        D_800EE4C0.unk4 = D_80078E00.array4020[D_800EE4C0.statusCode].unk10;
        var_a0 = D_80078E00.array4020[D_800EE4C0.statusCode].unk11;
        D_800EE4C0.unk0C = func_8009F930(var_a0, temp_s4, arg0, var_s3);
        break;
        
    case 0x6:        
        switch (D_800EE4C0.unk3) {           
        case 9:     
            D_800EE4C0.unk0C = (func_8009FD28(arg0, temp_s4) * (func_8009B15C() + 0xA)) / 150;
            break;
            
        case 10:    
            D_800EE4C0.unk0C = 0;
            D_800EE4C0.flags5 |= 1;
            break;
        }
        break;
        
    case 0xF4:       
        if (D_800EE469 != 0) {
            D_800EEBBF = func_800A08C0();
        } 
        
        else {
        case 0x26:   
        case 0xF0:   
        case 0xF5:   
            D_800EEBBF = func_800A085C();
        }
        
        D_800EEBC8 = 100;
        D_800EEBBD = D_80078E00.entriesA0[D_800EE4C0.statusCode].unk12;
        D_800EEBBE = D_80078E00.entriesA0[D_800EE4C0.statusCode].unk13;
        func_8009FCF4(D_80078E00.entriesA0[D_800EE4C0.statusCode].unk8);
        var_s3 = D_80078E00.entriesA0[D_800EE4C0.statusCode].unk5;
        D_800EE4C0.unk4 = D_80078E00.entriesA0[D_800EE4C0.statusCode].unk9;
        var_a0 = D_80078E00.entriesA0[D_800EE4C0.statusCode].unk4;
        D_800EE4C0.unk0C = func_8009F930(var_a0, temp_s4, arg0, var_s3);
        break;
        
    case 0xF6:       
        D_800ED148.entities[arg0].status |= 1;
        D_800EE4C0.unk0C = 0;
        D_800EE4C0.flags5 |= 1;
        break;
        
    case 0x2:        
    case 0x10:       
    case 0xF7:       
        D_800EE4CC = func_8009FD28(arg0, temp_s4);
        break;
        
    case 0x13:       
        func_8009FCF4(D_80078E00.array4A70[D_800EE4C0.unk3].unkunk);
        var_s3 = D_80078E00.array4A70[D_800EE4C0.unk3].unk1;
        D_800EE4C0.unk4 = D_80078E00.array4A70[D_800EE4C0.unk3].unk2;
        var_a0 = D_80078E00.array4A70[D_800EE4C0.unk3].unk0;
        D_800EE4C0.unk0C = func_8009F930(var_a0, temp_s4, arg0, var_s3);
        break;
        
    case 0x11:       
    case 0x12:       
    case 0x14:       
    case 0x15:       
    case 0x16:       
        func_8009FCF4(D_80078E00.array4484[D_800EE4C0.statusCode].var3);
        var_s3 = D_80078E00.array4484[D_800EE4C0.statusCode].var1;
        D_800EE4C0.unk4 = D_80078E00.array4484[D_800EE4C0.statusCode].var2;
        var_a0 = D_80078E00.array4484[D_800EE4C0.statusCode].var;
        D_800EE4C0.unk0C = func_8009F930(var_a0, temp_s4, arg0, var_s3);
        break;
        
    case 0xF:        
        func_8009FCF4(D_80078E00.array44FC[D_800EE4C0.statusCode].unk13);
        var_s3 = D_80078E00.array45F8[func_8009FDE0(temp_s4, D_800EE4C0.statusCode)].unk6;
        D_800EE4C0.unk4 = D_80078E00.array44FC[D_800EE4C0.statusCode].unk09;
        var_a0 = D_80078E00.array44FC[D_800EE4C0.statusCode].unk10;
        D_800EE4C0.unk0C = func_8009F930(var_a0, temp_s4, arg0, var_s3);
        break;
        
    case 0x4:        
    case 0xD:        
        func_8009FCF4(D_80078E00.abilities[D_800EE4C0.statusCode].unk2);
        var_s3 = D_80078E00.array3920[D_800EE4C0.statusCode].unk18;
        D_800EE4C0.unk4 = D_80078E00.abilities[D_800EE4C0.statusCode].unk3;
        var_a0 = D_80078E00.array3920[D_800EE4C0.statusCode].unk17;
        D_800EE4C0.unk0C = func_8009F930(var_a0, temp_s4, arg0, var_s3);
        break;
        
    case 0x7:        
    case 0x17:       
    case 0x18:       
    case 0x19:       
    case 0x1A:       
    case 0x1B:       
    case 0x1E:       
    case 0x1F:       
    case 0x20:       
    case 0x21:       
    case 0x22:       
        func_8009FCF4(D_80078E00.array4020[D_800EE4C0.statusCode].unk13);
        var_s3 = D_80078E00.array4020[D_800EE4C0.statusCode].unk12;
        D_800EE4C0.unk4 = D_80078E00.array4020[D_800EE4C0.statusCode].unk10;
        var_a0 = D_80078E00.array4020[D_800EE4C0.statusCode].unk11;
        D_800EE4C0.unk0C = func_8009F930(var_a0, temp_s4, arg0, var_s3);
        break;
        
    case 0xE:        
        D_800ED148.unk1300 = 1;
        D_800EE4C4 = 0;
        D_800ED148.unk12F5 = 1;
        goto ohno_plz_halp;

    case 0xEE:       
        D_800ED148.unk1300 = 0;
        if (D_800ED148.entities[arg0].status & 1) {
            D_800EE4C4 = D_80078E00.array47FC[D_800ED148.unk1324].var2;
        } 
        
        else {
            D_800EE4C4 = 0;
        }
        
        ohno_plz_halp:
        D_800EE4C0.unk0C = 0;
        D_800EE4C0.flags5 |= 1;
        break;
        
    case 0xED:       
        D_800ED148.unk1300 = 1;
        func_8009FCF4(D_80078E00.array47FC[D_800ED148.unk1324].var3);
        var_s3 = D_80078E00.array47FC[D_800ED148.unk1324].var1;
        D_800EE4C0.unk4 = D_80078E00.array47FC[D_800ED148.unk1324].var2;
        var_a0 = D_80078E00.array47FC[D_800ED148.unk1324].var;
        D_800EE4C0.unk0C = func_8009F930(var_a0, temp_s4, arg0, var_s3); 
        break;
        
    case 0x8:        
    case 0xEC:       
        func_8009FCF4(D_80078E00.entries17[D_800EE4C0.statusCode].unk8);
        var_s3 = D_80078E00.entries17[D_800EE4C0.statusCode].unk7;
        D_800EE4C0.unk4 = D_80078E00.entries17[D_800EE4C0.statusCode].unk5;
        var_a0 = D_80078E00.entries17[D_800EE4C0.statusCode].unk6;
        D_800EE4C0.unk0C = func_8009F930(var_a0, temp_s4, arg0, var_s3);
        break;
        
    case 0xFC:       
        D_800EE4C0.flags5 |= 1;
        break;
        
    case 0xFE:       
        if (D_800EE466 != 0) {
            D_800EEBBD = D_80078E00.rows132[D_800EE4C0.statusCode - 64].unk83;
            D_800EEBBE = D_80078E00.rows132[D_800EE4C0.statusCode - 64].unk84;
            D_800EEBBF = g_battleChars.levelEntries[D_800EE4C0.statusCode - 64].level;
            func_8009FCF4(D_80078E00.rows132[D_800EE4C0.statusCode - 64].unk8);
            var_s3 = D_80078E00.rows132[D_800EE4C0.statusCode - 64].unk5;
            D_800EE4C0.unk4 = D_80078E00.rows132[D_800EE4C0.statusCode - 64].unk9;
            var_a0 = D_80078E00.rows132[D_800EE4C0.statusCode - 64].unk4;
            D_800EE4C0.unk0C = func_8009F930(var_a0, temp_s4, arg0, var_s3); 
            break; 
        }       
        break;
        
    case 0xF9:       
        D_800EE448 = 0;
        func_8009FCF4(D_80078E00.unk3738[D_800EE4C0.unk3].unk18);
        var_s3 = D_80078E00.unk3738[D_800EE4C0.unk3].unk15;
        D_800EE4C0.unk4 = D_80078E00.unk3738[D_800EE4C0.unk3].unk16;
        var_a0 = D_80078E00.unk3738[D_800EE4C0.unk3].unk13;
        D_800EE4C0.unk0C = func_8009F930(var_a0, temp_s4, arg0, var_s3); 
        break;
        
    case 0xFB:       
        var_s3 = D_80078E00.array37A9[g_gameState.chars[g_gameState.mainData.party.party[temp_s4]].characterId].unk0;
        goto wait_oh_god_make_it_stop;
        
    case 0xF3:       
    case 0xF8:       
    case 0xFD:       
        var_s3 = D_80078E00.array35BD[g_battleChars.chars[temp_s4].classId].unk2;
        wait_oh_god_make_it_stop:
        func_8009FCF4(0);
        D_800EE4C0.unk4 = 4;
        if (func_8009FDD4(D_800EE3DC) != 0) {
            D_800EE4C0.flags6 |= 2;
        }
        
        D_800EE4C0.unk0C = func_8009F930(10, temp_s4, arg0, var_s3);
        func_800A0978(temp_s4);
        break;   
    }
    
    if ((D_800EE4C0.unk1 == 243) || (D_800EE4C0.unk1 == 28)) {
        D_800EE4C0.unk0C *= 3;
    }
    
    if (D_800ED148.entities[temp_s4].flags & 0x02000000) {
        if (D_800EE4C0.unk1 == 2) {
            D_800EE4C0.unk0C *= 5;
        }
    }
    
    if (D_800EE461 != 255) {
        D_800EE4C0.unk0C = D_800EE4C0.unk0C / 2;
    }
    
    func_8009F824();
    D_800EE4C0.unk0C = func_8009B7F4(D_800EE4C0.unk0C, arg0);
    func_800A08E0(temp_s4, arg0);
    func_800A2724(arg0, D_800EE4C0.unk0C, &D_800EE4C0.flags6, &D_800EE4C0.flags5, D_800EE4C0.unk0, &D_800EE4C0.unk4, &D_800EE4C0.unk1E, &D_800EE4C0.unk10, 0);
    
    if (D_800EE4C0.unk7 != 255) {
        func_800A2724(temp_s4, D_800EE4C0.unk14, &D_800EE4C0.unkA, &D_800EE4C0.unk09, D_800EE4C0.unk0, &D_800EE4C0.unk08, &D_800EE4C0.unk20, &D_800EE4C0.unk18, 1);
    }
}

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
 * @param a0 Entity index (stride 0x1D0).
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
