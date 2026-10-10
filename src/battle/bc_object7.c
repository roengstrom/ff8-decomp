#include "common.h"
#include "battle.h"
#include "game.h"
#include "gamestate.h"
#include "kernel.h"
#include "battle/bc_object7.h"


void func_800AF254(void) {
    func_800AF740();
    
    switch (g_battleConfig.result) {
        case 2:
            g_gameState.mainData.fieldCE2++;
            g_vsyncRate = 5;
            break;
            
        case 4:
            g_gameState.mainData.fieldCDC++;
            if (D_800ED148.unkCDD & 0x10) {
                g_vsyncRate = 100;
            }
                
            else {
                g_vsyncRate = 5;
            }
            
            break;
            
        case 1:
        case 3:
            g_gameState.mainData.fieldCE0++;
            g_vsyncRate = 100;
            break;
            
        case 5:
            g_vsyncRate = 100;
            break;
    }
    
    sndCmdF1();
    g_renderMode = 0;
    VSync(2);
    DrawSync(0);
    func_800D0B24();
}

s32 func_800AF358(s32 arg0, s32 arg1, s32 arg2) {
    s32 i;

    if (arg1 == 0) {
        return 0;
    }

    if (arg2 == 0) {
        for (i = 0; i < 32; i++) {
            if (g_battleChars.chars[arg0].magicSlots[i].unk0 == arg1) {
                if (g_battleChars.chars[arg0].magicSlots[i].unk1 < 100) {
                    g_battleChars.chars[arg0].magicSlots[i].unk1++;
                    return 0;
                }
                
                return 1;
            }
        }
        
        for (i = 0; i < 32; i++) {
            if (g_battleChars.chars[arg0].magicSlots[i].unk0 == 0) {
                g_battleChars.chars[arg0].magicSlots[i].unk0 = arg1;
                g_battleChars.chars[arg0].magicSlots[i].unk1++;
                func_800229FC(arg0);
                return 0;
            }
        }
        
        return 1;
    }
    
    for (i = 0; i < 32; i++) {
        if (g_battleChars.chars[arg0].magicSlots[i].unk0 == arg1) {
            if (g_battleChars.chars[arg0].magicSlots[i].unk1 != 0) {
                g_battleChars.chars[arg0].magicSlots[i].unk1--;
                
                if (g_battleChars.chars[arg0].magicSlots[i].unk1 == 0) {
                    g_battleChars.chars[arg0].magicSlots[i].unk0 = 0;
                    func_800229FC(arg0);
                    return 255;
                }
                
                return 0;
            }
            
            return 255;
        }
    }
    
    return 255;
}

s32 func_800AF4BC(s32 arg0, s32 arg1) {
    s32 i;

    if (arg0 == 0) {
        return 0;
    }
    
    if (arg1 == 0) {
        for (i = 0; i < 32; i++) {
            if (D_800EE9E8.animSlots[i].id == arg0) {
                if (D_800EE9E8.animSlots[i].value < 100) {
                    D_800EE9E8.animSlots[i].value++;
                    return 0;
                }
                
                return 1;
            }
        }
      
  
        for (i = 0; i < 32; i++) {
            if (D_800EE9E8.animSlots[i].id == 0) {
                D_800EE9E8.animSlots[i].id = arg0;
                D_800EE9E8.animSlots[i].value++;
                func_800A8578();
                return 0;
            }
        }

        return 1;
    }

    
    else {
        for (i = 0; i < 32; i++) {
            if (D_800EE9E8.animSlots[i].id == arg0) {
                if (D_800EE9E8.animSlots[i].value != 0) {
                    D_800EE9E8.animSlots[i].value--;
            
                    if ((D_800EE9E8.animSlots[i].value) == 0) {
                        D_800EE9E8.animSlots[i].id = 0;
                    }
                    
                    return 0;
                }
                
                return 1;
            }
        }
    }
    
    return 1;
}

void func_800AF5E0(s32 arg0, s32 arg1, ItemSlot* arg2) {
    ItemSlot* slot;
    s32 i;
    
    if (arg0 == 0) {
        return;
    }

    slot = arg2;
    for (i = 0; i < 198; i++, slot++) {
        if (slot->id == arg0) {
            slot->count = arg1;
            return;
        }
    }

    slot = arg2;
    for (i = 0; i < 198; i++, slot++) {
        if (slot->id == 0) {
            slot->id = arg0;
            slot->count = arg1;
            return;
        }
    }    
}

/**
 * @brief Re-init the 32-slot anim init table at @c D_800EE9E8 by feeding
 *        each slot's @c (id, value) pair plus @c D_80077EBC into
 *        @c func_800AF5E0.
 */
void func_800AF654(void) {
    ItemSlot* item = g_gameState.mainData.itemSlots;
    s32 i;
    for (i = 0; i < 32; i++) {
        func_800AF5E0(D_800EE9E8.animSlots[i].id, D_800EE9E8.animSlots[i].value, item);
    }
}

/**
 * @brief Copy entity animation data to lookup table and clear a flag.
 *
 * Computes entity pointer from D_800ED158 + a0*0xD0. Reads an index
 * byte from g_gameState + a0 + 0xAF4, multiplies by 0x98 to find a
 * table entry at g_gameState + 0x490. Copies entity halfword at 0x18
 * to the table entry. Clears bit 5 of entity halfword at 0x80 and
 * stores the result at table entry + 0x96.
 *
 * @param a0 Entity index (stride 0xD0).
 */
void func_800AF6BC(s32 arg0) {
    CharacterData* partyMember;
    BattleEntity* entity;

    entity = &D_800ED148.entities[arg0];
    partyMember = &g_gameState.chars[g_gameState.mainData.party.partyMembers[arg0]];
    
    partyMember->currentHp = entity->currentHp;
    partyMember->statusFlags = entity->status &= ~STATUS_BERSERK;
    func_800AE4A0(arg0);
}

/**
 * @brief For each of the 3 party slots, mirror the entity's display status
 *        into the matching @c BattleCharData and refresh its anim table entry.
 *
 * Walks @c D_800ED148.entities[0..2] (BattleSystem block) — for any slot whose
 * @c comFileId is not 0xFF, calls @c func_800AF6BC(i) (which copies the
 * entity's animation halfwords into the per-character anim cache) and then
 * mirrors @c entity->status into @c g_battleChars.chars[i].displayStatus.
 * Finishes by calling @c func_800AF654 to rebuild the global anim list.
 */
void func_800AF740(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (D_800ED148.entities[i].comFileId != 255) {
            func_800AF6BC(i);
            g_battleChars.chars[i].displayStatus = D_800ED148.entities[i].status;
        }
    }
    
    func_800AF654();
}

void func_800AF7C4(void) {
    s32 i;
    s32 j;
    s32 k;
    CharacterData* temp_a3;
    BattleCharData* character;
    
    for (i = 0; i < 3; i++) {
        if (D_800ED148.entities[i].comFileId == 255) {
            continue;
        }
        
        temp_a3 = &g_gameState.chars[g_gameState.mainData.party.partyMembers[i]];
        character = &g_battleChars.chars[i];
        
        for (j = 0; j < 32; j++) {
            temp_a3->magic[j].magicId  = character->magicSlots[j].unk0;
            temp_a3->magic[j].quantity = character->magicSlots[j].unk1;
        }
        
        for (k = 0; k < 20; k++) {
            for (j = 0; j < 32; j++) {
                if (temp_a3->junctions[k] == temp_a3->magic[j].magicId) {
                    goto skip;
                }
            }
             
            temp_a3->junctions[k] = 0;
            skip:
        }
    }
}

/**
 * @brief Clear two flag bits in the entity's @c controlFlags and bracket
 *        the update with @c func_800A565C / @c func_800A5778 calls.
 *
 * @param a0 Entity index into @c D_800ED148.entities.
 */
void func_800AF8A4(s32 a0) {
    func_800A565C(a0);
    D_800ED148.entities[a0].controlFlags &= ~0x8;
    D_800ED148.entities[a0].controlFlags &= ~0x4;
    func_800A5778(a0);
}

s32 func_800AF918(s32 arg0, u8* arg1) {
    s32 bit;
    s32 i;
    s32 count;

    count = 0;
    bit = 1;

    for (i = 0; i < 16; i++) {
        if (g_gameState.chars[g_gameState.mainData.party.partyMembers[arg0]].junctedGfs & bit) {
            *arg1++ = i;
            count++; // counts how many elements of arg1 have a value inside
        }
        
        bit <<= 1;
    }
        
    return count;
}

/**
 * @brief Read the byte at offset 0x14F of the entity's linked data block.
 *
 * @param a0 Entity index into @c D_800ED148.entities.
 * @return Byte at @c (*entities[a0].linkedPtr)[0x14F].
 */
s32 func_800AF988(s32 a0) {
    return (*D_800ED148.entities[a0].entityData)->unk14F;
}

/**
 * @brief Clamp a 16-bit unsigned value to a maximum of 60000.
 *
 * @param a0 Input value (low 16 bits used).
 * @return min(a0 & 0xFFFF, 60000).
 */
u32 func_800AF9C4(u16 arg0) {
    
    if (arg0 > 60000) {
        return 60000;
    }
    
    return arg0;
}

u16 func_800AF9E8(s32 arg0, s32 arg1) {
    s32 result;
    s32 temp_v1;
    BattleEntity* entity; 
    
    temp_v1 = (*D_800ED148.entities[arg0].entityData)->unk100;
    entity = &D_800ED148.entities[arg0];
    result = (entity->level * 5 * temp_v1) / arg1 - temp_v1;
    
    if (temp_v1 == 0) {
        result = 0;
    } 
    
    else if (result < 1) {
        result = 1;
    }
    
    return result;
}

u16 func_800AFA64(s32 arg0) {
    BattleEntity* temp_s0;
    BattleEntityData* temp_s1;
    s32 result;

    temp_s1 = *D_800ED148.entities[arg0].entityData;
    temp_s0 = &D_800ED148.entities[arg0];
    
    if (temp_s1->unk102 == 0) {
        result = 0;
    } 
    
    else if (temp_s0->maxHp == temp_s0->currentHp) {
        result = 0;
    } 
    
    else {
        result = ((temp_s0->level * 5 * temp_s1->unk102) / func_800A6DD8() - temp_s1->unk102) * (temp_s0->maxHp - temp_s0->currentHp) / temp_s0->maxHp;

        if (temp_s1->unk102 == 0) {
            result = 0;
        } 
        
        else if (result < 1) {
            result = 1;
        }
    }
    
    return result;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800AFB5C);

void func_800AFD0C(void) {
    s32 i;
    s32 j;
    s32 idx;
    u16 tmp;
    u16 temp_s2;
    u8 sp10[16];
    
    for (i = 3; i < 7; i++) {
        if (!(D_800ED148.entities[i].status & 1)) {
            g_battleChars.unk574[0] = func_800AF9C4(g_battleChars.unk574[0] + func_800AF9C4(func_800AFA64(i)));
        }
    }
    
    tmp = g_battleChars.unk574[0];
    for (i = 0; i < 3; i++) {
        if ((D_800ED148.entities[i].status & 1) || (D_800ED148.entities[i].status & 4)) {
            g_battleChars.unk574[i] = 0;
            g_battleChars.unk57A[i] = 0;
        } 
        
        else {
            g_battleChars.unk574[i] = tmp;
        }
    }

    temp_s2 = g_battleChars.unk5C0[0];
    g_battleChars.unk5C0[0] = 0;
    for (i = 0; i < 3; i++) {
        if (!(D_800ED148.entities[i].controlFlags & 1) || ((D_800ED148.entities[i].status & 1) && (D_800ED148.entities[i].status & 4))) {
              continue;  
        }
        
        idx = func_800AF918(i, sp10);
        if (idx != 0) {
            u16 div = tmp / idx;
            for (j = 0; j < idx; j++) {
                if (g_gameState.gfs[sp10[j]].hp != 0) {
                    g_battleChars.unk580[sp10[j]] = div;
                    g_battleChars.unk5C0[sp10[j]] = temp_s2;
                } 
                
                else {
                    g_battleChars.unk5A0[sp10[j]] = 0;
                }
            }
        }
    }
    
    if (g_battleConfig.unk2 & 8) {
        func_800A9490();
    }
}

/**
 * @brief Return the name of non-junctionable GF attack @p a0.
 *
 * @param a0 Index into @c g_kernel.nonJunctionableGfAttacks.
 */
u8* func_800AFF30(s32 a0) {
  return resolveKernelPtr(g_kernel.nonJunctionableGfAttacks[a0].nameOffset, g_kernel.nonJunctionableGfAttacksText);
}

/**
 * @brief Return the name of junctionable GF @p a0 - 0x40.
 *
 * @param a0 Index into @c g_kernel.junctionableGfs, offset by @c 0x40.
 */
u8* func_800AFF70(s32 a0) {
    return resolveKernelPtr(g_kernel.junctionableGfs[a0 - 64].nameOffset, g_kernel.junctionableGfsText);
}

/**
 * @brief Return the name of enemy attack @p a0.
 *
 * @param a0 Index into @c g_kernel.enemyAttacks.
 */
u8* func_800AFFB4(s32 a0) {
    return resolveKernelPtr(g_kernel.enemyAttacks[a0].nameOffset, g_kernel.enemyAttacksText);
}

/**
 * @brief Call getMenuString with argument 0xA.
 */
void func_800AFFF4(void) {
    getMenuString(0xA);
}

/**
 * @brief Call getMenuString with argument 0xC.
 */
void func_800B0014(void) {
    getMenuString(0xC);
}

/**
 * @brief Call getMenuString with argument 0xD.
 */
void func_800B0034(void) {
    getMenuString(0xD);
}

/**
 * @brief Call getMenuString with argument 0xE.
 */
void func_800B0054(void) {
    getMenuString(0xE);
}

/**
 * @brief Return the first word of the data linked from a battle entity.
 *
 * @param idx Entity index into D_800ED148.entities.
 * @return First s32 word at @c entities[idx].linkedPtr.
 */
BattleEntityData* func_800B0074(s32 idx) {
    return *D_800ED148.entities[idx].entityData;
}

/**
 * @brief Call getMenuString with argument 0xF.
 */
void func_800B00A8(void) {
    getMenuString(0xF);
}

/**
 * @brief Call getMenuString with argument 0x10.
 */
void func_800B00C8(void) {
    getMenuString(0x10);
}