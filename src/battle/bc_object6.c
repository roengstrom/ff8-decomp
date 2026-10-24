#include "common.h"
#include "ability.h"
#include "battle.h"
#include "gamestate.h"
#include "kernel.h"
#include "game.h"
#include "battle/bc_object1.h"
#include "battle/bc_object2.h"
#include "battle/bc_object4.h"
#include "battle/bc_object5.h"
#include "battle/bc_object6.h"

void func_800A2F54(void); // cant include bc_object3.h


/**
 * @brief Add an action's KO and spread target bits to target mask @p arg2.
 *
 * Magic (command 2) and items (4) take them from the entry's target info;
 * enemy attacks add 0x4000 when they may target KO'd units.
 *
 * @param arg0 Battle command.
 * @param arg1 Spell, item or enemy attack ID.
 * @param arg2 Target mask.
 * @return The extended mask.
 */
u16 func_800AB4A8(s32 arg0, s32 arg1, u16 arg2) {
    if (arg0 == 2) {
        return arg2 | func_800B0F7C(g_kernel.magic[arg1].targetInfo);
    }
    
    if (arg0 == 4) {
        return arg2 | func_800B0F7C(g_kernel.battleItems[arg1].targetInfo);
    }
    

    if ((g_kernel.enemyAttacks[arg1].attackFlags & ATTACK_FLAG_TARGET_KO)) {
        return arg2 | 0x4000;
    }

    return arg2;
}

/**
 * @brief Queue the first pending spell of D_800ED148.array12CC as action 247.
 *
 * Does nothing without pending spells, and clears them afterwards.
 *
 * @param arg0 Acting entity.
 */
void func_800AB570(s32 arg0) {
    u16 result;
    Struct_12CC* cc;
    
    if (D_800ED148.unk130B == 0) {
        return;
    }
    
    cc = &D_800ED148.array12CC[0];
    
    result = func_800B0F7C(g_kernel.magic[cc->unk1].targetInfo);
    result = result | func_800ADC10(cc->unk2);
    func_800A30F8(arg0, 247, cc->unk1, 0, 0, result, 0);
    func_800A4C84(result);
    func_8009AE08(5);
    func_800AE524(D_800ED148.unk5C0 - 1);
    D_800ED148.entries[D_800ED148.unk5C0 - 1].unk11 = 0;
    func_8009AE08(6);
    
    D_800ED148.unk130B = 0;
    D_800ED148.unk130D = 0;
}

u8 func_800AB668(s32 arg0) {
    u8 sp10;
    u8 temp_v0;

    func_800A4FC4(D_800ED148.entities[arg0].hpDisplay, &sp10);
    temp_v0 = func_800B0CC4(sp10, 0);
    D_800ED148.entities[arg0].unkB8[0] = temp_v0;
    
    if (temp_v0 == 255) {
        return 255;
    }
    
    D_800ED148.actionType = 2;
    D_800ED148.actionByte0 = D_800ED148.entities[arg0].unkB8[0];
    return sp10;
}

void func_800AB6F4(s32 arg0) {
    u8 sp18;

    if (arg0 == 1) {        
        func_8009AF3C(
            func_800B02AC(
                func_800B0248(
                    func_800B0248(
                        func_800B0248(getMenuString(0x13), *getMenuString(0xB), func_800B04A0(1, &sp18)),
                        *getMenuString(0xB),
                        getMagicNamePtr(D_800ED148.actionByte0)
                    ),
                    7,
                    getMenuString(0x76)
                )
            ),
            (g_gameState.config.battleMsgSpeed * 8) + 8,
            3,
            128,
            86
        );
    } 
    
    else { 
        func_8009AF3C(
            func_800B02AC(
                func_800B0248(
                    func_800B0248(
                        func_800B0248(getMenuString(0x13), *getMenuString(0xB), func_800B04A0(arg0, &sp18)),
                        *getMenuString(0xB),
                        getMagicNamePtr(D_800ED148.actionByte0)
                    ),
                    7,
                    getMenuString(8)
                )
            ),
            (g_gameState.config.battleMsgSpeed * 8) + 8,
            3,
            128,
            86
        );
    }
}

void func_800AB844(s32 arg0) {
    s32 temp_s1;
    s32 temp_s1_2;

    temp_s1 = func_800B0248(getBattleCharName(arg0), 7, getMenuString(0x1B));
    temp_s1_2 = func_800B0248(temp_s1, *getMenuString(0xB), getMagicNamePtr(D_800ED148.actionByte0));

    func_8009AF3C(func_800B02AC(func_800B0248(temp_s1_2, *getMenuString(0xB), getMenuString(117))), (g_gameState.config.battleMsgSpeed * 8) + 8, 3, 128, 86);
}

s32 func_800AB914(s32 arg0, s32 arg1) {

    func_800AB3E0();
    if (arg1 == 0) {
        arg1 = func_800AB24C();
    }
    
    func_800A84CC(arg1);
    func_8009A42C(arg1, arg0);
    D_800ED148.unk12FB = arg1;
    D_800ED148.unk12FC = arg0;
    func_8009B134(113, 128, &func_800AB28C);
    return arg1;
}

/**
 * @brief Read a little-endian 16-bit signed value from two bytes.
 *
 * Combines ptr[0] (low byte) and ptr[1] (high byte) into a signed 16-bit value.
 *
 * @param ptr Pointer to two bytes in little-endian order.
 * @return The sign-extended 16-bit value.
 */
s16 func_800AB998(u8 *ptr) {
    return (ptr[0] + (ptr[1] << 8));
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object6", func_800AB9B4);

/**
 * @brief Check if an entity is available (no blocking status flags).
 *
 * Checks halfword at +0x90 for bits 0 and 2 (mask 0x5),
 * word at +0x18 for bits 0, 3, 14 (mask 0x4009),
 * and word at +0x8C for bit 14 (0x4000).
 *
 * @param a0 Entity index (stride 0xD0).
 * @return 1 if entity is available, 0 otherwise.
 */
s32 func_800ACED4(s32 arg0) {
    s32 status = D_800ED148.entities[arg0].status;
    s32 flags  = D_800ED148.entities[arg0].flags;

    if ((status & 5) || (flags & 0x4009)) {
        return 0;
    }

    if (D_800ED148.entities[arg0].controlFlags & 0x4000) {
        return 0;
    }

    return 1;
}

/**
 * @brief Check if an entity is available (stricter status check).
 *
 * Checks halfword at +0x90 for bits 0, 2, 5 (mask 0x25),
 * word at +0x18 for bits 0, 3 (mask 0x9),
 * and word at +0x8C for bit 14 (0x4000).
 *
 * @param a0 Entity index (stride 0xD0).
 * @return 1 if entity is available, 0 otherwise.
 */
s32 func_800ACF2C(s32 arg0) {
    s32 status = D_800ED148.entities[arg0].status;
    s32 flags  = D_800ED148.entities[arg0].flags;
    
    if ((status & 0x25) || (flags & 9)) {
        return 0;
    }
    
    if (D_800ED148.entities[arg0].controlFlags & 0x4000) {
        return 0;
    }

    return 1;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object6", func_800ACF84);

/**
 * @brief Set bit 6 of entity flags at 0x8C, call func_800AE6C0, then set
 * bit 0 of entity halfword at 0x90.
 *
 * Computes entity address at D_800ED148 + a0 * 0xD0, sets bit 6 (0x40)
 * of the word at entity offset 0x8C, calls func_800AE6C0, then sets
 * bit 0 (0x1) of the halfword at entity offset 0x90.
 *
 * @param a0 Entity index (stride 0xD0).
 */
void func_800AD4A4(s32 arg0) {
    D_800ED148.entities[arg0].controlFlags |= 0x40;
    func_800AE6C0();
    D_800ED148.entities[arg0].status |= 1;
}

/**
 * @brief Call func_800AD4A4 then clear bit 0 of entity word at offset 0x8C.
 *
 * Calls func_800AD4A4 with the entity index, then clears the least
 * significant bit of the 32-bit word at D_800ED148 + a0 * 208 + 0x8C.
 *
 * @param a0 Entity index (stride 0xD0).
 */
void func_800AD50C(s32 arg0) {
    func_800AD4A4(arg0);
    D_800ED148.entities[arg0].controlFlags &= ~1;
}

/**
 * @brief Look up animation data for entity and process it.
 *
 * Computes entity pointer from D_800ED148 + a0 * 0xD0. Loads a
 * sub-object pointer at entity+0x14, dereferences it to get a base
 * pointer, then uses relative offsets at base+8 and base+0xC to
 * look up a u16 entry and a data pointer. Calls func_800A9784 with
 * the u16 entry and data pointer, then func_800B0398 with the result.
 *
 * @param a0 Entity index (stride 0xD0).
 * @param a1 Sub-index for u16 table lookup.
 */
void func_800AD564(s32 arg0, s32 arg1) {
    Unk4Struct* temp_v1;

    temp_v1 = *D_800ED148.entities[arg0].monsterAiSection;
    func_800B0398(func_800A9784(*(arg1 + GET_OFFSET(u16, temp_v1, temp_v1->unk8)), GET_OFFSET(s32, temp_v1, temp_v1->unkC)));
}

void func_800AD5D4(s32 partySlot, u8* arg1) {
    s32 i;
    
    if ((arg1 != 0) && (partySlot < 3)) {
        for (i = 0; i < 16; i++) {
            if (g_gameState.gfs[i].exists & 1) {
                g_gameState.chars[g_gameState.mainData.party.partyMembers[partySlot]].gfCompatibility[i] = 
                    g_gameState.chars[g_gameState.mainData.party.partyMembers[partySlot]].gfCompatibility[i] + arg1[i] - 100;
    
                
                if (g_gameState.chars[g_gameState.mainData.party.partyMembers[partySlot]].gfCompatibility[i] > 6000) {
                    g_gameState.chars[g_gameState.mainData.party.partyMembers[partySlot]].gfCompatibility[i] = 6000;
                }
                
                
                if (g_gameState.chars[g_gameState.mainData.party.partyMembers[partySlot]].gfCompatibility[i] < 1000) {
                    g_gameState.chars[g_gameState.mainData.party.partyMembers[partySlot]].gfCompatibility[i] = 1000;
                }
            }
        }
    }
}

void func_800AD6D8(BattleUnkDE8* arg0, s32 arg1, s32 arg2, s32 arg3, u16 arg4) {
    func_800AE4A0(arg0->link.fwd);
    if (arg2 == 4) {
        D_800ED148.entities[arg0->link.fwd].unkB8[0] = arg3;
    }
    
    arg0->link.bwd = arg2;
    arg0->unk4 = arg3;
    arg0->unk6[0] = arg4;
    arg0->unk6[1] = 0;
    D_800ED148.arrayDE8[D_800ED148.unk12F2][arg1][1].link.fwd = 255;
}

void func_800AD7A4(BattleUnkDE8* arg0, s32 arg1) {
    s32 sp18;
    s32 sp1C;
    u16 sp20;
    s32 temp_a1;
    u8 temp_a0;

    sp1C = 0;
    if ((D_800ED148.unk130C != 0) || (D_800ED148.unk12F2 == 0)) {
        return;
    }

    if (D_800ED148.entities[arg0->link.fwd].flags & 0x02000000) {
        func_800B1564(arg0->link.fwd, &sp18, &sp1C, &sp20);
        func_800AD6D8(arg0, arg1, sp18, sp1C, sp20);
        return;
    }
            
    if (D_800ED148.entities[arg0->link.fwd].status & 0x20) {
        sp18 = 1;
        sp20 = (D_800ED148.entities[arg0->link.fwd].flags & 0x4000)? func_800A980C() : func_800A9888();
        func_800AD6D8(arg0, arg1, sp18, sp1C, sp20);
        return;
    }
            
    if (D_800ED148.entities[arg0->link.fwd].flags & 0x4000) {
        func_800B13A0(arg0->link.fwd, &sp18, &sp1C, &sp20);
        func_800AD6D8(arg0, arg1, sp18, sp1C, sp20);
        return;
    }
}

s32 func_800AD8E4(s32 arg0) {
    if (arg0 == 255) {
        return 0;
    }
    
    if (D_800ED148.unk12F2 == 0) {
        return 1;
    }
    
    if ((D_800ED148.entities[arg0].status & 4) || (D_800ED148.entities[arg0].flags & 1) || (D_800ED148.entities[arg0].flags & 8)) {
        return 0;
    } 
     
    return 1;
}

/**
 * @brief Dispatch queued entity action and clear the queue.
 *
 * If the entity action pointer at D_800ED148+0x12DC is non-zero,
 * loads @c GameConfig.battleMsgSpeed, computes palette offset (byte*8+8),
 * calls func_8009AF3C with the entity pointer and palette params,
 * then clears the action pointer.
 */
void func_800AD960(void) {
    if (D_800ED148.unk12DC != 0) {
        func_8009AF3C(D_800ED148.unk12DC, (g_gameState.config.battleMsgSpeed * 8) + 8, 3, 128, 86);
        D_800ED148.unk12DC = 0;
    }
}

void func_800AD9C0(void) {
    s32 i;
    s32 j;

    D_800ED148.unk1304 = 0;
    D_800ED148.unk1306 = 0;
    
    if (D_800ED148.header.unk0 != 0) {
        return;
    }
    
    for (i = 0; i < 3; i++) {
        D_800ED148.unk12F2 = i;
        D_800ED148.unk12F8 = 0;
        
        for(j = 0; j < 11; j++) {
            if (D_800ED148.unkD64[i][j].fwd != 255) {
                continue;
            }
            
            while(1) {
                if (func_800ADAC0(j) == 0) {
                    func_800A57E0(j);
                    return;
                }
                
                j = D_800ED148.unkD64[i][j].bwd;
                if (j == 255) {
                    goto found;
                }
            }
        }
        // amazing
        found:
    }
}

u8 func_800ADAC0(s32 arg0) {
    s32 i;
    BattleUnkDE8* temp_s0;

    D_800ED148.unk1302 = arg0;
    
    temp_s0 = &D_800ED148.arrayDE8[D_800ED148.unk12F2][arg0][0];
    if (func_800AD8E4(temp_s0->link.fwd) != 0) {
        if (temp_s0->link.bwd == 255) {
            func_800ADF08(arg0, 0);
            return 0;
        }
        
        for (i = 0; i < 2; i++) {
            if (func_800ADF08(arg0, i) != 0) {
                return 0;
            }
           
            func_800B17B8(D_800ED148.header.entityRef);
            func_800B1828(D_800ED148.header.entityRef);
            
            if (func_800AE730() == 255 || func_800AE788() == 255) {
                break;
            }
        }
        
        D_800ED148.unk130C = 0;
        func_800B18A0(D_800ED148.header.entityRef);
       
        if (D_800ED148.unk131C == 0 && D_800ED148.unk1325 == 0) {
            func_8009AE08(8);
        }
   
        return 0;
    }
    

    return 1;
}

/**
 * @brief Call one of two processing functions based on entity count.
 *
 * If the input is less than 3, calls func_800A9888; otherwise calls
 * func_800A980C. Returns the lower 16 bits of the result.
 *
 * @param count Entity count.
 * @return Result masked to 16 bits.
 */
u16 func_800ADC10(s32 count) {
    s32 result;
    if (count >= 3) {
        result = func_800A980C();
    } 
    
    else {
        result = func_800A9888();
    }

    return result;
}

/**
 * @brief Record the pending spells of D_800ED148.array12CC as one command.
 *
 * Writes action 247 with each spell's target mask into the current DE8
 * command entry, then clears the pending list.
 *
 * @param arg0 Acting entity.
 */
void func_800ADC48(s32 arg0) {
    s32 i;
    s32 result;
    BattleUnkDE8* temp_s4;

    if (D_800ED148.unk130B == 0) {
        return;
    }
    
    D_800ED148.unk130C = 1;
    temp_s4 = &D_800ED148.arrayDE8[D_800ED148.unk12F2][D_800ED148.unk1302][1];
    temp_s4->link.fwd = arg0;
    temp_s4->link.bwd = 247;
    temp_s4->link.unk2 = 0;
    temp_s4->link.unk3 = 0;

    for (i = 0; i < D_800ED148.unk130B; i++) {
        Struct_12CC* var_s1 = &D_800ED148.array12CC[i];
        temp_s4->unk4 = var_s1->unk1;
        result = func_800B0F7C(g_kernel.magic[var_s1->unk1].targetInfo);
        D_800ED148.arrayDE8[D_800ED148.unk12F2][D_800ED148.unk1302][1].unk6[i] = func_800ADC10(var_s1->unk2) | result;
    }

    
    D_800ED148.unk130B = 0;
    D_800ED148.unk130D = 0;
}

s32 func_800ADDAC(BattleUnkDE8* arg0) {
    s32 i;

    if (arg0->link.bwd == 2) {
        if (D_800ED148.entities[arg0->link.fwd].flags & 0x40000) {
            if (arg0->unk6[2] != 0) {
                return 3;
            }
        }
            
        else {
            arg0->unk6[2] = 0;
        }

        if (D_800ED148.entities[arg0->link.fwd].flags & 0x20000) {
            if (arg0->unk6[1] != 0) {
                return 2;
            }
        }
            
        else {
            arg0->unk6[1] = 0;
        }

        return 1;
    }

    for (i = 0; i < 3; i++) {
        if (arg0->unk6[i] == 0) { 
             break;
        }
    }
 
    return i;
}

s32 func_800ADEA0(s32 arg0, s32 arg1) {
    if ((D_800ED148.unk130C == 0) && (arg1 != 0) && ((D_800ED148.entities[arg0].status & 0x25) || (D_800ED148.entities[arg0].flags & 0x4009))) {
        return 1;
    }
    
    return 0;
}

s32 func_800ADF08(s32 arg0, s32 arg1) {
    BattleUnkDE8* temp_s0;
    s32 result;
    s32 i;
    s32 var_s4;
    s32 temp_s6;
    
    D_800ED148.header.unk8 = NULL; // sets the pointer to null and takes the reference in func_800A30F8
    D_800ED148.unk132B = 0;
    D_800ED148.unk12F1 = arg1;
    temp_s0 = &D_800ED148.arrayDE8[D_800ED148.unk12F2][arg0][arg1];

    if (temp_s0->link.fwd == 255) {
        return 0;
    }
    
    D_800ED148.header.entityRef = temp_s0->link.fwd;
    D_800ED148.unk1310 = temp_s0->link.bwd;
    
    if (temp_s0->link.bwd == 255) {
        func_800ACF84(temp_s0->link.fwd, temp_s0->unk4);
        
        if (temp_s0->unk4 == 1) {
            if (D_800ED148.entities[temp_s0->link.fwd].unk8A == temp_s0->unk4) {
                D_800ED148.entities[temp_s0->link.fwd].flags &= ~(1 << 23);
                func_8009B088(temp_s0->link.fwd, 1, 23, 0);
            }
        }
    }

    else {
        if (func_800ADEA0(temp_s0->link.fwd, arg1) != 0) {
            return 0;
        }
    
        func_8009AE08(5);
        temp_s6 = D_800ED148.unk5C0;
        D_800ED148.unk12F0 = 0;
        result = func_800ADDAC(temp_s0);
        var_s4 = 0;
    
        for (i = 0; i < 3; i++) {
            if (temp_s0->unk6[i] != 0) {
                if (i == 0) {
                    func_800AD7A4(temp_s0, arg0);
                }
            
                if (func_800A30F8(temp_s0->link.fwd, temp_s0->link.bwd, temp_s0->unk4, temp_s0->link.unk2, temp_s0->link.unk3, temp_s0->unk6[i], 0, 0) != 0) {
                    func_8009AE08(7);
                    return 1;
                }
                
                func_800A4C84(temp_s0->unk6[i]);
                D_800ED148.unk12F0++;
                
                if (!(D_800ED148.entities[temp_s0->link.fwd].flags & 0x02000000) && (D_800EE4C0.unk1 == 2) && (D_800ED148.unk12F4 == 0)) {
                    if ((g_battleChars.chars[temp_s0->link.fwd].characterAbilities & CHAR_ABILITY_EXPENDX2_1) && (result == 2)) {
                        var_s4 = 1;
                    } 
                        
                    else if ((g_battleChars.chars[temp_s0->link.fwd].characterAbilities & CHAR_ABILITY_EXPENDX3_1) && (result == 3)) {
                        var_s4 = 1;
                    }
                        
                    else {
                        var_s4 = 0;
                        if (i == (D_800ED148.unk12F0 - 1) && func_800AF358(D_800EE4C0.unk0, D_800ED148.unk132A, 1) == 255) {
                            break;
                        }
                    }
                }
                
                if (D_800ED148.unk132B == 0) {
                    if (D_800ED148.unk130C == 0) {
                        func_800AD5D4(temp_s0->link.fwd, D_800ED148.header.unk8);
                    }            
                }
                    
                else {
                    break;
                }   
            }
                
            else {
                break;
            }
        }
        
        if (var_s4 != 0) {
            func_800AF358(D_800EE4C0.unk0, D_800ED148.unk132A, 1);
        }
        
        D_800ED148.entries[temp_s6].unk11 = D_800ED148.unk12F0 - 1;
        if ((D_800EE4C0.unk1 == 4) || (D_800EE4C0.unk1 == 244)) {
            func_800AE414(temp_s0->link.fwd);
        } 
        
        else {
            func_800AE4A0(temp_s0->link.fwd);
        }
        
        func_800ADC48(temp_s0->link.fwd);
        func_800AE524(temp_s6);
    }
    

    if (D_800ED148.unk131C == 1) {
        func_8009AF14(&func_800A65B0);
    }
    
    else if (D_800ED148.unk1304 != 0) {
        if (D_800ED148.unk132E == 3) {
            func_8009AF14(&func_8009BC28);
        } 
        
        else {
            func_8009AF14(&func_8009BCE4);
        }
      
        D_800ED148.unk1304 = 0;
    } 
    
    else if (D_800ED148.unk1325 == 1) {
        func_8009AF14(&func_800A2F54);
    }
    
    D_800ED148.unk12F0 = 0;
    
    return 0;
}

/**
 * @brief Search D_800EE9E8 table for an entry matching the given value.
 *
 * Iterates 32 entries at stride 5 in D_800EE9E8. If byte[0] matches a0,
 * returns the signed byte at offset 1. Returns 0 if no match found.
 *
 * @param a0 Value to search for.
 * @return Signed byte at offset 1 of matching entry, or 0 if not found.
 */
s32 func_800AE390(s32 arg0) {
    s32 i;
    for (i = 0; i < 32; i++) {
        if (arg0 == D_800EE9E8.animSlots[i].id) {
            return D_800EE9E8.animSlots[i].value;
        }
    }
    
    return 0;
}

/**
 * @brief Clear the entry in D_80077EBC that matches the given value.
 *
 * Searches up to 198 entries at stride 2 in D_80077EBC. If byte[0]
 * matches a0, clears both bytes of that entry and returns.
 *
 * @param a0 Value to search for and clear.
 */
void func_800AE3D4(s32 arg0) {
    s32 i;
    ItemSlot* slot = g_gameState.mainData.itemSlots;
    for (i = 0; i < 198; i++, slot++) {
        if (slot->id == arg0) {
            slot->id = 0;
            slot->count = 0;
            break;
        }
    }
}

void func_800AE414(s32 arg0) {
    if (func_800AE390(D_800ED148.entities[arg0].unkB8[D_800ED148.unk12F1]) == 0) {
        func_800AE3D4(D_800ED148.entities[arg0].unkB8[D_800ED148.unk12F1]);
    }
    
    D_800ED148.entities[arg0].unkB8[D_800ED148.unk12F1] = 0;
}

void func_800AE4A0(s32 arg0) {
    s32 i;

    for (i = 0; i < 2; i++) {
        if (D_800ED148.entities[arg0].unkB8[i] != 0) {
            func_800AF4BC(D_800ED148.entities[arg0].unkB8[i], 0);
            D_800ED148.entities[arg0].unkB8[i] = 0;
        }
    }    
}

void func_800AE524(s32 arg0) {
    func_8009B134(104, 128, &D_800ED148.entries[arg0]);
    func_800AD960();
}

s32 func_800AE568(void) {
    u16 bit;
    s32 i;

    bit = 0;
    for (i = 6; i >= 0; i--) {
        bit <<= 1;
        if ((D_800ED148.entities[i].controlFlags & 1) && !(D_800ED148.entities[i].controlFlags & 0x40) && ((D_800ED148.entities[i].status ^ 1) & 1)) {
            bit |= 1;
        }
    }

    return bit;
}

s32 func_800AE5D8(void) {
    u16 bit;
    s32 i;
    
    bit = 0;
    for (i = 6; i >= 0; i--) {
        bit <<= 1;
        if (D_800ED148.entities[i].controlFlags & 1) {
            if (!(D_800ED148.entities[i].controlFlags & 0x40)) {
                if (i > 2) {
                    bit |= 1;
                }
                
                else if (!(D_800ED148.entities[i].status & 1)) {
                    bit |= 1;
                }
            }
        }   
    }
    
    return bit;
}

s32 func_800AE64C(void) {
    u16 bit;
    s32 i;
    
    bit = 0;
    for (i = 6; i >= 0; i--) {
        bit <<= 1;
        if (D_800ED148.entities[i].controlFlags & 1) {
            if (!(D_800ED148.entities[i].controlFlags & 0x40)) {
                if (i < 3) {
                    bit |= 1;
                }
                
                else if (!(D_800ED148.entities[i].status & 1)) {
                    bit |= 1;
                }
            }
        }   
    }
    
    return bit;
}

/**
 * @brief Store two display values from func_800AE5D8 and func_800AE64C.
 *
 * Calls func_800AE5D8, stores result to g_battleChars[0x570] as u16,
 * then calls func_800AE64C, stores result to g_battleChars[0x572].
 */
void func_800AE6C0(void) {
    g_battleChars.unk570 = func_800AE5D8();
    g_battleChars.unk572 = func_800AE64C();
}

/**
 * @brief Find the first active entity among slots 0-2.
 *
 * Scans up to 3 entities (stride 0xD0) in D_800ED148. Returns
 * the index of the first entity with bit 0 of the word at offset
 * 0x8C set.
 *
 * @return Entity index (0-2) if found.
 */
s32 func_800AE6F8(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (D_800ED148.entities[i].controlFlags & 1) {
            return i;
        }
    }
}

s32 func_800AE730(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if ((D_800ED148.entities[i].controlFlags & 1) && !(D_800ED148.entities[i].status & 1) && !(D_800ED148.entities[i].status & 4)) {
            return i;
        }
    }
    
    return 255;
}

/**
 * @brief Find first available entity in slots 3-6.
 *
 * Scans entities 3-6 (stride 0xD0) at D_800ED148+0x270. Returns
 * the index of the first entity where bits 0 and 2 of the halfword
 * at offset 0x90 are both clear.
 *
 * @return Entity index (3-6), or 0xFF if none available.
 */
s32 func_800AE788(void) {
    s32 i;

    for (i = 3; i < 7; i++) {
        if (!(D_800ED148.entities[i].status & 1) && !(D_800ED148.entities[i].status & 4)) {
           return i;
        }
    }
    
    return 255;
}

u8 func_800AE7D0(void) {
    s32 i;

    for (i = 0; i < 11; i++) {
        if (D_800ED148.unkD64[0][i].fwd == 255) {
            BattleUnkDE8* var_a0 = &D_800ED148.arrayDE8[0][i][0];
            if ((var_a0->link.bwd == 255) && (var_a0->unk4 == 3)) {
                return 1;
            }
        }
    }
    
    return 0;
}

s32 func_800AE83C(s32 arg0, s32 arg1) {
    s32 i;

    for (i = 3; i < 7; i++) {
        if (!(D_800ED148.entities[i].status & 1) && ((*D_800ED148.entities[i].entityData)->unkFE & arg0)) {
            return arg1;
        }
    }
    
    return 0;
}

/**
 * @brief Search first 3 entities for one matching a condition.
 *
 * Iterates entities 0-2 at stride 0xD0 from D_800ED148. For each
 * entity with bit 0 of the word at offset 0x8C set, calls
 * func_800ACED4. Returns 1 on the first match, 0 if none found.
 *
 * @return 1 if a matching entity was found, 0 otherwise.
 */
s32 func_800AE8A0(void) {
    s32 i;

    for (i = 0; i < 3; i++){
        if ((D_800ED148.entities[i].controlFlags & 1) && (func_800ACED4(i) != 0)) {
            return 1;
        }
    }
    
    return 0;
}

void func_800AE90C(void) {
    s32 var_s0;
    s32 i;
    s32 var_a1;

    switch (D_800ED148.unk1308) {
        case 1:
        case 2:
            var_s0 = 16;
            break;
        case 3:
        case 4:
            var_s0 = 255;
            break;
        
        default:
            
        var_a1 = 0;
        for (i = 3; i < 7; i++) {
            if (!(D_800ED148.entities[i].status & 0x25) && !(D_800ED148.entities[i].flags & 0x4001)) {
                var_a1++;
            }
        }
    
        if (var_a1 != 0) {
            if (func_800AE83C(16, 100) == 100) {
                var_s0 = 16;
            } 
            
            else {
                if (func_800AE83C(8, 100) == 0) {
                    var_s0 = 64;
                }
                    
                else {
                    var_s0 = 128;
                }
            }
        }
        
        else {
            var_s0 = 255;
        }
    }

    if ((func_800AE8A0() != 0) && (func_8009B79C(var_s0, 255) != 0)) {
        D_800ED148.unk12E8 = 1;
    }
}

void func_800AEA0C(void) {
    s32 result;

    D_800ED148.header.control = 0;
    D_800ED148.unk12ED = func_800CED4C();
    if (D_800ED148.unk12ED != 0) {
        if (g_battleConfig.unk2 & 1) {
            D_800ED148.header.control = 1;
            return;
        }

        
        result = func_800CED3C();
        if (!(result % 60)) {
            func_800AE90C();
        }
    } 
    
    else if (D_800ED148.unk12E8 != 2) {
        D_800ED148.unk12E8 = 0;
    }
}



/**
 * @brief Start battle sequence with optional character animation.
 *
 * Sets D_800EE446 to 1, calls func_8009B134(0x70, 0x80, 0) to
 * allocate a message entry, then func_8009AE08(0xA) to set mode.
 * If a0 is not -1, calls getMenuString to look up the character,
 * then func_8009AF3C with animation parameters derived from
 * @c GameConfig.battleMsgSpeed and a stack argument of 0x56.
 *
 * @param a0 Character index, or -1 to skip animation setup.
 */
void func_800AEACC(s32 arg0) {
    D_800ED148.unk12FE = 1;
    func_8009B134(112, 128, 0);
    func_8009AE08(10);
    
    if (arg0 != -1) {
        func_8009AF3C(getMenuString(arg0), g_gameState.config.battleMsgSpeed * 8 + 8, 3, 128, 86);
    }
}

void func_800AEB50(void) {
    if ((g_battleConfig.result != 0) || (func_800AE7D0() != 0) || (D_800ED148.unk12E8 != 1) || (func_800AE8A0() == 0)) {
        return;
    }
    
    func_800AEACC(1);
    func_8009B134(116, 128, 0);
    g_battleConfig.result = 2;
    func_800AFD0C();
    D_800ED148.unk1301 = 2;
    func_8009AF14(&func_8009AD7C);
}

/**
 * @brief Check conditions and trigger callback mode 3 for entity system.
 *
 * Returns early if g_battleConfig[7] is non-zero, or if
 * @ref BATTLE_FLAG_COUNTDOWN is clear in g_battleConfig.unk2, or if the
 * countdown has not reached 0, or if the halfword at g_battleConfig equals 0x13D.
 * Otherwise calls func_800AEACC(-1), sets mode to 3, stores 3 in
 * D_800EE449, and registers func_8009AD7C as callback.
 */
void func_800AEC04(void) {
    if ((g_battleConfig.result == 0) && (g_battleConfig.unk2 & BATTLE_FLAG_COUNTDOWN) && (g_gameState.mainData.countdownTimer == 0) && (g_battleConfig.battleSceneId != 317)) {
        func_800AEACC(-1);
        g_battleConfig.result = 3;
        D_800ED148.unk1301 = 3;
        func_8009AF14(&func_8009AD7C);
    }
}

/**
 * @brief Set callback mode 3 and register func_8009AD7C.
 *
 * Stores 1 to g_battleConfig.result, stores 3 to D_800EE449, then calls
 * func_8009AF14 with func_8009AD7C as the callback.
 */
void func_800AEC98(void) {
    g_battleConfig.result = 1;
    D_800ED148.unk1301 = 3;
    func_8009AF14(&func_8009AD7C);
}

/**
 * @brief Check entity state and trigger retreat if conditions met.
 *
 * Returns early if g_battleConfig.result is non-zero, or if entity at D_800ED148
 * offset 0x12F9 equals 1, or if byte at 0x132D is zero. Otherwise
 * calls func_800AEACC(-1) and func_800AEC98.
 */
void func_800AECD4(void) {
    if ((g_battleConfig.result == 0) && (D_800ED148.unk12F9 != 1) && (D_800ED148.unk132D != 0)) {
        func_800AEACC(-1);
        func_800AEC98();
    }
}

/**
 * @brief Check entity state and trigger action if conditions met.
 *
 * Returns early if g_battleConfig.result is non-zero, or if D_800EE441 equals 1.
 * Calls func_800AE730 and checks if result is 0xFF. If not, returns.
 * Calls func_800B2128 and if result is non-zero, returns. Otherwise
 * calls func_800AEACC(0) and func_800AEC98.
 */
void func_800AED30(void) {
    if (g_battleConfig.result != 0) {
        return;
    }

    if (D_800ED148.unk12F9 == 1) {
        return;
    }

    if (func_800AE730() != 255) {
        return;
    }

    if (func_800B2128() != 0) {
        return;
    }

    func_800AEACC(0);
    func_800AEC98();
}

void func_800AED9C(void) {
    if ((g_battleConfig.result != 0) || (D_800ED148.unk12F9 == 1) || (func_800AE788() != 255)) {
        return;       
    }
    
    func_800AEACC(-1);
    g_battleConfig.result = 4;
    func_800AFD0C();
    
    if (!(g_battleConfig.unk2 & 2)) {
        func_8009B134(115, 128, 0);
        D_800ED148.unk1301 = 0;
    }
    
    else {
        func_8009B134(109, 128, 0);
        D_800ED148.unk1301 = 1;
    }
    
    func_8009AF14(&func_8009AD7C);
}

/**
 * @brief Trigger battle end sequence.
 *
 * Calls SetDispMask(0) to stop processing, sets g_battleConfig.result to 5,
 * clears byte at D_800ED148 + 0xC, then calls sndStopAll and
 * resetCdDrive for cleanup.
 */
/**
 * @brief Trigger battle end sequence.
 *
 * Calls SetDispMask(0) to stop processing, sets g_battleConfig.result to 5,
 * clears byte at D_800ED148 + 0xC, then calls sndStopAll and
 * resetCdDrive for cleanup.
 */
void func_800AEE64(void) {
    SetDispMask(0);
    g_battleConfig.result = 5;
    D_800ED148.header.timer = 0;
    sndStopAll();
    resetCdDrive();
}


/**
 * @brief Classify the current animation frame into a range bucket.
 *
 * Calls func_8009B15C to get the current frame value. Checks bit 2
 * of D_80078DF8 to select between two sets of range thresholds.
 * Returns 0-3 based on which range the frame falls into.
 *
 * @return Range classification: 0 (low), 1 (mid), 2 (high), 3 (very high).
 */
s32 func_800AEEAC(void) {
    s32 result;
    
    result = func_8009B15C();
    if (g_battleChars.abilityFlags & 2) {
        if (result < 128) return 0;
        if (result < 242) return 1;
        if (result < 261) return 2;
        return 3;
    }
    
    else {
        if (result < 178) return 0;
        if (result < 229) return 1;
        if (result < 244) return 2;
        return 3;
    }
}

void func_800AEF34(s32 arg0) {
    BattleEntityData* temp_s1;
    s32 result;

    if ((D_800ED148.entities[arg0].controlFlags & 0x800)) {
        return;
    }
    
    temp_s1 = *D_800ED148.entities[arg0].entityData;

    if ((temp_s1->unk14D >= func_8009B15C()) && (D_800ED148.unk12FA < 24)) {
        result = func_800AEEAC();
        g_battleChars.unk5E0[D_800ED148.unk12FA].unk0   = temp_s1->unk104[2][D_800EE9E8.subEntries[arg0 - 3].unk46].sub[result].unk0;
        g_battleChars.unk5E0[D_800ED148.unk12FA++].unk1 = temp_s1->unk104[2][D_800EE9E8.subEntries[arg0 - 3].unk46].sub[result].unk1;
    } 
}

void func_800AF068(s32 arg0) {
    BattleEntityData* temp_s1;
    s32 var_s0;
    
    temp_s1 = *D_800ED148.entities[arg0].entityData;
    
    var_s0 = 8;
    if (temp_s1->unkFE & 0x80) {
        var_s0 = 255;
    }
    
    if (var_s0 >= func_8009B15C()) {
        if ((D_800ED148.unk1316 < 8) && (temp_s1->unkF8 != 255)) {
            g_battleChars.unk610[D_800ED148.unk1316++] = temp_s1->unkF8;
        }
    }
}

s32 func_800AF134(s32 arg0, u8* arg1, u8* arg2, s32 arg3) {
    BattleEntityData* temp_s1;
    s32 result;
    
    temp_s1 = *D_800ED148.entities[arg0].entityData;
    result  = temp_s1->unk14C;
    
    if (result != 0) {
        if (func_8009B15C() <= (result + (arg3 / 2))) {
            result = func_800AEEAC();
            *arg1 = temp_s1->unk104[1][D_800EE9E8.subEntries[arg0 - 3].unk46].sub[result].unk0;
            *arg2 = temp_s1->unk104[1][D_800EE9E8.subEntries[arg0 - 3].unk46].sub[result].unk1;
            return 1;
        }
        
        return 0;
    }
    
    return 2;
}