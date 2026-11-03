#include "common.h"
#include "psxsdk/libetc.h"
#include "battle.h"
#include "game.h"
#include "gamestate.h"
#include "kernel.h"
#include "battle/bc_object4.h"
#include "battle/bc_object9.h"
#include "battle/bc_object10.h"
#include "battle/bc_object12.h"
#include "battle/bc_object13.h"
#include "battle/bc_object14.h"
#include "battle/bc_object15.h"
#include "sound.h"

extern u8 D_800EE490[];


void* func_800B02AC(u8* arg0) {
    D_800E3CF0 %= 16;
    func_800B01E8(&D_800EE4C0.something[D_800E3CF0], arg0);
    return &D_800EE4C0.something[D_800E3CF0++]; // fix return value
}

/**
 * @brief Copy a string to D_800EE490 and return the buffer pointer.
 *
 * @param src Source string to copy.
 * @return Pointer to D_800EE490.
 */
u8 *func_800B0328(u8 *src) {
    u8 *dst = D_800EE490;
    func_800B01E8(dst, src);
    return dst;
}

/**
 * @brief Return the description of Rinoa limit break (part 1) @p a0.
 *
 * @param a0 Index into @c g_kernel.rinoaLimitBreaks1.
 */
u8* func_800B0360(s32 a0) {
    return resolveKernelPtr(g_kernel.rinoaLimitBreaks1[a0].descOffset, g_kernel.rinoaLimitBreaks1Text);
}

void* func_800B0398(u8* arg0) {
    D_800E3CF1 %= 16;
    func_800B01E8(&D_800ECC48[D_800E3CF1].unk0, arg0);
    return &D_800ECC48[D_800E3CF1++];
}

u8 func_800B0414(u32 arg0, u8* arg1) {
    s32 i;
    u8* temp_a0;
    u8* temp_v1;
    

    for (i = 0; i < 5; i++) {
        temp_a0 = arg1 + (4 - i);
        *temp_a0 = arg0 % 10;
        arg0 /= 10;
    }
    
    for (i = 0; i < 4; i++) {
        temp_v1 = arg1 + i;
        if (*temp_v1 != 0) {
            return (5 - i);
        }
        
        *temp_v1 = 10;
    }
    
    return 1;
}

/* this function is triggered by doing Draw -> Stock */
u8* func_800B04A0(u32 arg0, u8* arg1) {
    s32 i;
    u8 buffer[8];
    u8* str;

    str = arg1;
    for (i = 5 - func_800B0414(arg0, buffer); i < 5; i++) {
        if (buffer[i] != 10) {
            *str++ = func_800B00E8(buffer[i]);
        } 
    }
    
    *str = 0;
    return arg1;
}

static s32 func_800B054C(u32 arg0) {
    s32 i;

    for (i = 0; i < 32; i++) {
        if (arg0 == 1) {
            return i;
        }

        arg0 >>= 1;
    }
    
    return i;
}

/**
 * @brief Store scaled animation value at entity's bit position offset.
 *
 * Calls func_800B054C to find the lowest set bit in a1. If the result
 * is less than 14, computes a scale factor from @c GameConfig.battleSpeed and the
 * status's kernel timer, multiplies them, and stores the result at the
 * entity's bit-indexed halfword slot.
 *
 * @param a0 Entity index (stride 0xD0).
 * @param a1 Bitmask to find lowest set bit.
 */
void func_800B0574(s32 arg0, u32 arg1) {
    s32 temp_v0;
    
    temp_v0 = func_800B054C(arg1);
    if (temp_v0 < 14) {
        u8 val = g_kernel.misc.statusTimers[temp_v0];
        s32 temp = ((g_gameState.config.battleSpeed + 1) * 4);
        D_800ED148.entities[arg0].perBit[temp_v0] = val * temp;
    }
}

/**
 * @brief Store the reset sentinel @c -0x457 in the entity's per-bit
 *        halfword slot indexed by the lowest set bit of @p a1.
 *
 * @param a0 Entity index into @c D_800ED148.entities.
 * @param a1 Bitmask whose lowest set bit selects the slot in @c timers.
 */
void func_800B0600(s32 a0, s32 a1) {
    s32 bitPos = func_800B054C(a1);
    if (bitPos < 14) {
        D_800ED148.entities[a0].perBit[bitPos] = -0x457;
    }
}

/**
 * @brief Test whether the entity's per-bit halfword slot (selected by the
 *        lowest set bit of @p a1) currently holds the reset sentinel.
 *
 * @param a0 Entity index into @c D_800ED148.entities.
 * @param a1 Bitmask whose lowest set bit selects the slot in @c timers.
 * @return 1 if @c timers[bitPos] == -0x457, 0 otherwise.
 */
s32 func_800B0668(s32 a0, s32 a1) {
    s32 bitPos = func_800B054C(a1);
    if (bitPos < 14) {
        if (D_800ED148.entities[a0].perBit[bitPos] == -0x457) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Process entity ability and trigger state transitions.
 *
 * Masks @p arg0 to 16 bits and calls @c func_800A4C84. If @c sys->unkE
 * is zero, transitions to state 5, calls @c func_800AE524 with the
 * preceding entry index (@c sys->unk5C0 - 1), clears that entry's
 * @c unk10 byte, then transitions to state 6.
 *
 * @param arg0 Entity bitmask (16-bit).
 */
void func_800B06DC(u16 arg0) {
    func_800A4C84(arg0);
    if (D_800ED148.header.unkE == 0) {
        func_8009AE08(5);
        func_800AE524(D_800ED148.unk5C0 - 1);
        D_800ED148.entries[D_800ED148.unk5C0 - 1].unk11 = 0;
        func_8009AE08(6);
    }
}

/**
 * @brief Set up extended parameters and call two processing functions.
 *
 * Saves the 16-bit truncation of a3, calls func_800A30F8 with 7 args
 * (a0, a1, a2 passed through, a3 zeroed, plus a0, truncated a3, and 0
 * on the stack), then calls func_800B06DC with the truncated a3 value.
 *
 * @param a0 First parameter (also passed as 5th arg).
 * @param a1 Second parameter passed through.
 * @param a2 Third parameter passed through.
 * @param a3 Fourth parameter (16-bit truncated, passed as 6th arg).
 */
void func_800B0754(s32 a0, s32 a1, s32 a2, u16 a3) {
    func_800A30F8(a0, a1, a2, 0, a0, a3, 0);
    func_800B06DC(a3);
}

/**
 * @brief Handle special battle action flags for an entity.
 *
 * If @p a1 has bit @c 0x400 set, calls @c func_800A59AC with mode 5 and
 * returns 1. If @p a1 has bit @c 0x1000 set, sets bit 2 in the entity's
 * @c status field and calls @c func_800A2520. Otherwise returns 0.
 *
 * @param a0 Entity index into @c D_800ED148.entities.
 * @param a1 Action flags bitmask.
 * @return 1 if bit @c 0x400 action taken, 0 otherwise.
 */
s32 func_800B0794(s32 a0, s32 a1) {
    if (a1 & 0x400) {
        func_800A59AC(a0, 5, 0);
        return 1;
    }

    if (a1 & 0x1000) {
        D_800ED148.entities[a0].status |= 4;
        func_800A2520(a0);
    }

    return 0;
}

void func_800B0808(s32 arg0, u8* arg1) {
    s32 temp_s0;
    s32 temp_s1;

    temp_s0 = func_800B0248(arg0, 7, getMenuString(0x1B));
    temp_s1 = func_800B0248(temp_s0, *getMenuString(0xB), arg1);

    func_800A4320(func_800B02AC(func_800B0248(temp_s1, *getMenuString(0xB), getMenuString(0x74))));
    func_800AD960();
}

void func_800B08AC(s32 arg0, s32 arg1) {
    s32 var_s0;
    
    if (arg0 < 3) {
        var_s0 = getBattleCharName(arg0);
    } 
    
    else {
        var_s0 = func_800B0074(arg0);
    }

    if (arg1 & 0x40) {
        func_800B0808(var_s0, getMenuString(0x4F));
    }
    
    if (arg1 & 0x20) {
        func_800B0808(var_s0, getMenuString(0x4E));
    }
    
    if (arg1 & 0x80) {
        func_800B0808(var_s0, getMenuString(0x50));
    }
}

/**
 * @brief Call func_800A59AC with a1=6 and a2=0.
 *
 * @param a0 First argument passed through.
 */
void func_800B095C(s32 a0) {
    func_800A59AC(a0, 6, 0);
}

void func_800B0980(s32 arg0, s16 arg1, s32 arg2, s16 arg3) {
    if ((arg0 & 0x10) && (arg3 != 0)) {
        s16 div = arg1 / arg3;
        if ((div % (60 / arg3)) == 0) {
            func_800B095C(arg2);
        }
    }
}

void func_800B09F0(s32 arg0) {
    s32 i;
    s32 val;
    s32 flag;
    BattleEntity* entity;

    entity = &D_800ED148.entities[arg0];
    for (i = 0; i < 14; i++) {
        if (entity->perBit[i] == -0x457) {
            continue;
        }
        
        flag = 1;
        flag <<= i;
        if (entity->perBit[i] <= 0) {
            entity->perBit[i] = -0x457;
            if (func_800B0794(arg0, flag) != 0) {
                entity->flags &= ~flag;
                return;
            }
            
            func_800B08AC(arg0, flag);
            entity->flags &= ~flag;
            func_800A240C(arg0, D_800ED148.entities[arg0].currentHp, &D_800ED148.entities[arg0].status);
            func_8009AFF0(arg0);
            
            if (arg0 < 3) {
                func_800A1AB8(arg0, D_800ED148.entities[arg0].status, D_800ED148.entities[arg0].flags);
            } 
            
            else {
                func_800A1CFC(arg0);
            }
            
            func_8009AF98(arg0);
        }
            
        else {
            val = 2;
            if (entity->flags & 2) {
                val = 3;
            }
            
            if (entity->flags & 4) {
                val = 1;
            }
            
            if ((entity->flags & 9) == 9) {
                if (i != 3) {
                    val = 0;
                }
            }
            
            else {
                if (entity->flags & 8) {
                    if (i != 3) {
                        val = 0;
                    }
                }
                
                if (entity->flags & 1) {
                    if (i != 0) {
                        val = 0;
                    }
                }
            }
            
            func_800B0980(flag, entity->perBit[i], arg0, val);
            entity->perBit[i] -= val;
        }  
    }
}

/**
 * @brief Process entities whose @c status has neither bit 0 nor bit 2 set.
 *
 * Walks @c D_800ED148.entities[0..6]; for each slot whose @c status & 5
 * is zero, calls @c func_800B09F0(i).
 */
void func_800B0C08(void) {
    s32 i;
    for (i = 0; i < 7; i++) {
        if ((D_800ED148.entities[i].status & 5) == 0) {
            func_800B09F0(i);
        }
    }
}

s8 func_800B0C68(s32 arg0, s32 arg1) {
    BattleCharData* var_v1;
    s32 i;
    
    for (i = 0; i < 32; i++) {
        var_v1 = &g_battleChars.chars[arg0];
        if (var_v1->magicArgs[i].id == arg1) {
            return var_v1->magicArgs[i].count;
        }
    }
    
    return 0;
}

s32 func_800B0CC4(s32 arg0, s32 arg1) {
    s32 i;
    s32 count;
    BattleCharData* character;
    
    count = 0;
    character = &g_battleChars.chars[arg0];
    
    if (arg1 == 0) {
        for (i = 0; i < 32; i++) {
            if (character->magicArgs[i].id != 0) {
                count++;
            }
        }
        
        if (count == 0) {
            arg1 = 255;
        }
            
        else {
            arg1 = func_8009B15C() % count;
            
            while(1) {
                if (character->magicArgs[arg1].id != 0) {
                    arg1 = character->magicArgs[arg1].id;
                    break;
                    
                }
                
                arg1++;
                arg1 &= 0x1F;
            }
        }
    }
    
    return arg1;
}

s32 func_800B0D8C(s32 arg0, s32 arg1) {
    BattleCharData* var_v1;
    s32 i;

    for (i = 0; i < 4; i++) {
        var_v1 = &g_battleChars.chars[arg0];
        if (var_v1->abilityCommands[i].commandId == arg1) {
            return 0;
        }
    }

    return 255;
}

/**
 * @brief Dispatch call based on @ref SEALED_MAGIC in g_battleConfig.unk8.
 *
 * If @ref SEALED_MAGIC is set, passes 0xFF to func_800B0CC4.
 * Otherwise calls func_800B0D8C with a0 and mode 2, then passes
 * the result to func_800B0CC4.
 *
 * @param a0 Entity parameter for func_800B0D8C and func_800B0CC4.
 */
s32 func_800B0DDC(s32 a0) {
    s32 val;
    if (g_battleConfig.sealedAbilities & SEALED_MAGIC) {
        val = 255;
    } 
    
    else {
        val = func_800B0D8C(a0, 2);
    }
    
    return func_800B0CC4(a0, val);
}

s32 func_800B0E30(s32 arg0) {
    s32 count;
    s32 i;
    s32 id;

    count = 0;
    if (arg0 == 0) {
        for (i = 0; i < 32; i++) {
            if ((D_800EE9E8.animSlots[i].id != 0) && (g_kernel.battleItems[D_800EE9E8.animSlots[i].id].randomSelect & 1)) {
                count++;
            }
        }
        
        if (count == 0) {
            arg0 = 255;
        }
            
        else {
            arg0 = func_8009B15C() % count;
            while (1) {
                if ((D_800EE9E8.animSlots[arg0].id != 0) && (g_kernel.battleItems[D_800EE9E8.animSlots[arg0].id].randomSelect & 1)) {
                    arg0 = D_800EE9E8.animSlots[arg0].id;
                    break;
                }
                
                arg0++;
                arg0 &= 0x1F;
            }
        }  
    }

    return arg0;
}

/**
 * @brief Dispatch call based on @ref SEALED_ITEM in g_battleConfig.unk8.
 *
 * If @ref SEALED_ITEM is set, passes 0xFF to func_800B0E30.
 * Otherwise calls func_800B0D8C with a0 and mode 4, then passes
 * the result to func_800B0E30.
 *
 * @param a0 Entity parameter for func_800B0D8C.
 */
s32 func_800B0F3C(s32 a0) {
    s32 val;
    if (g_battleConfig.sealedAbilities & SEALED_ITEM) {
        val = 255;
    } 
    
    else {
        val = func_800B0D8C(a0, 4);
    }

    return func_800B0E30(val);
}

/**
 * @brief Convert ability flag bits to GF compatibility bitmask.
 *
 * Bit 0 of the input maps to bit 14 (0x4000) of the result,
 * and bit 1 maps to bit 13 (0x2000).
 *
 * @param arg0 Ability flags.
 * @return Bitmask with bits 14 and/or 13 set.
 */
u16 func_800B0F7C(u8 arg0) {
    u16 flags;

    flags = (arg0 & 1) << 14;
    
    if (arg0 & 2) {
        flags |= 0x2000;
    }
    
    return flags;
}

u16 func_800B0F9C(s32 arg0) {
    switch (arg0 & 0x30) {
        case 0:
            if (arg0 & 0x40) {
                return func_800AA4E8();
            }
            return func_800AA4E0();
            
        case 16:
            if (arg0 & 0x40) {
                return func_800A9888();
            }
            return func_800A980C();
        
        case 32:
            return func_800AA4F0();
    }
}

u16 func_800B1050(s32 arg0) {
    switch (arg0 & 0x30) {
        case 0:
            if (arg0 & 0x40) {
                return func_800AA4E0();
            }
            return func_800AA4E8();

        case 16:
            if (arg0 & 0x40) {
                return func_800A980C();
            }
            return func_800A9888();

        case 32:
            return func_800AA4F0();
    }
}

/**
 * @brief Compute combined ability flags for the spell record at the given ID.
 *
 * Reads the spell's target info, passes it to func_800B1050 and
 * func_800B0F7C, and returns the OR of both results masked to 16 bits.
 *
 * @param a0 Spell ID (index into g_kernel.magic).
 * @return Combined 16-bit ability flags.
 */
u16 func_800B1104(s32 a0) {
    return func_800B1050(g_kernel.magic[a0].targetInfo) | func_800B0F7C(g_kernel.magic[a0].targetInfo);
}

/**
 * @brief Resolve the action ID and flags for one of the player's command slots.
 *
 * Picks a deterministic-but-pseudorandom variant via func_8009B15C() % 3 and
 * dispatches on the command type stored at g_battleChars.chars[selfIdx].abilityCommands[cmdIdx].
 *
 * - cmd 1 / 12 (Attack-like): writes only *outFlags (no ID resolved).
 * - cmd 2 (Magic): resolves a spell ID via func_800B0DDC; combined element/status flags
 *   are read from g_kernel.magic[id].targetInfo via func_800B0F9C/F7C/1104.
 * - cmd 4 (GF/Item): resolves an ability ID via func_800B0F3C, calls func_800AF4BC(id, 1)
 *   to consume a charge, then reads flags from g_kernel.battleItems[id].targetInfo via
 *   func_800B1050/F9C/F7C.
 *
 * @param selfIdx     Party slot index into g_battleChars.chars (0..2).
 * @param cmdIdx      Command slot index (0..3) within the chosen char.
 * @param outId       Output: resolved action ID, or 0xFF on lookup failure.
 * @param outFlags    Output: combined 16-bit element/status flags.
 * @return The command type that was dispatched, or 0 if no match / lookup failed.
 */
s32 func_800B115C(s32 selfIdx, s32 cmdIdx, s32 *outId, u16 *outFlags) {
    u8 var = func_8009B15C() % 3;
    s32 cmd = g_battleChars.chars[selfIdx].abilityCommands[cmdIdx].commandId;
    s32 result;

    *outId = 0;

    switch (cmd) {
        case 1:
        case 12:
            if (var != 0) {
                *outFlags = func_800A980C();
            } 
            
            else {
                *outFlags = func_800A9888();
            }
            
            return cmd;
        case 2:
            result = func_800B0DDC(selfIdx);
            *outId = result;
            if (result == 255) {
                return 0;
            }

            if (var != 0) {
                *outFlags = func_800B1104(result);
            } 
            
            else {
                *outFlags = func_800B0F9C(g_kernel.magic[result].targetInfo) | func_800B0F7C(g_kernel.magic[*outId].targetInfo);
            }
            return cmd;

        case 4:
            result = func_800B0F3C(selfIdx);
            *outId = result;
            if (result == 255) {
                return 0;
            }
            func_800AF4BC(result, 1);
            if (var != 0) {
                *outFlags = func_800B1050(g_kernel.battleItems[*outId].targetInfo) | func_800B0F7C(g_kernel.battleItems[*outId].targetInfo);
            } 
            
            else {
                *outFlags = func_800B0F9C(g_kernel.battleItems[*outId].targetInfo) | func_800B0F7C(g_kernel.battleItems[*outId].targetInfo);
            }
            
            return cmd;
    }

    return 0;
}

void func_800B13A0(s32 arg0, s32* arg1, s32* arg2, u16* arg3) {
    u32 result;
    
    result = func_8009B15C() % 4;
    while((*arg1 = func_800B115C(arg0, result, arg2, arg3)) == 0) {
        result++;
        result %= 4;
    }
}

s32 func_800B1438(s32 arg0) {
    s32 i;
    s32 result;
    s32 count;
    BattleCharData* character;

    count = 0;
    character = &g_battleChars.chars[arg0];
    
    for (i = 0; i < 32; i++) {
        if ((character->magicArgs[i].id != 0) && (g_kernel.magic[character->magicArgs[i].id].targetInfo & 0x40)) {
            count++;
        }
    }
    
    if (count == 0) {
        result = 255;
    }
        
    else {
        result = func_8009B15C() % 32;
        while (1) {            
            if ((character->magicArgs[result].id != 0) && (g_kernel.magic[character->magicArgs[result].id].targetInfo & 0x40)) {
                result = character->magicArgs[result].id;
                break;
            }
            
            result++;
            result &= 0x1F;
        }
    }
        
    return result;
}

void func_800B1564(s32 arg0, s32* arg1, s32* arg2, s16* arg3) {
    if ((*arg2 = func_800B1438(arg0)) != 255) {
        *arg1 = 2;
        *arg3 = func_800B0F9C(g_kernel.magic[*arg2].targetInfo) 
              | func_800B0F7C(g_kernel.magic[*arg2].targetInfo);
        return;
    }
    
    *arg1 = 1;
    *arg2 = 0;
    *arg3 = func_800A9888();
}

/* new */
void func_800B1624(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (D_800ED148.entities[arg0].controlFlags & 0x10) {
        D_800ED148.entities[arg0].controlFlags |= 0x200;
        return;
    }
    
    func_800A6288(arg0, arg1, arg2, arg3);
    func_800A5F24(0, arg0, 1, 0, func_800A9888());
}

void func_800B16C0(s32 arg0) {
    s32 i;

    if (D_800ED148.unk130C != 0) {
        return;
    }
    
    if (D_800ED148.entities[arg0].status & 1) {
        return;
    }
    
    D_800ED148.entities[arg0].unkCC = 0;

    for (i = 0; i < 3; i++) {
        if (D_800ED148.entities[i].unkCC != 0) {
            D_800ED148.entities[arg0].unkCC += D_800ED148.entities[i].unkCC;
            D_800ED148.entities[i].unkCC = 0;
        }  
    }
    
    if ((D_800ED148.unk1309 != 3) && (D_800ED148.entities[arg0].unkCC != 0)) {
        func_800B0754(arg0, 0, 5, func_800A97FC(arg0));
    }
}


/**
 * @brief Conditionally trigger entity action based on D_800ED148 flags.
 *
 * If D_800ED148[0x130C] is non-zero or D_800ED148[0x1326] is zero,
 * returns immediately. Otherwise calls func_800A97FC to get a value,
 * passes it to func_800B0754, and clears D_800ED148[0x1326].
 *
 * @param a0 Entity parameter for func_800A97FC and func_800B0754.
 */
void func_800B17B8(s32 arg0) {
    if ((D_800ED148.unk130C == 0) && (D_800ED148.unk1326 != 0)) {
        func_800B0754(arg0, 0, 10, func_800A97FC(arg0));
        D_800ED148.unk1326 = 0;
    }
}

/**
 * @brief Check GF compatibility bytes and trigger special attack if matched.
 */
void func_800B1828(s32 arg0) {
    if (D_800ED148.unk130C != 0) {
        return;
    }
    
    if (D_800EE4C0.unk1 != 244) {
        return;
    }

    if (D_800EE4C0.unk3 != 31) {
        return;
    }
    
    func_800B0754(arg0, 0, 8, func_800AA4E0() | 0x4000);
}

/**
 * @brief Conditionally trigger limit break action based on entity and GF state.
 *
 * Returns immediately if D_800ED148[0x130C] is non-zero, or if the entity's
 * status halfword at offset 0x90 has bit 0 set, or if D_800EE4C1 is not 0x1F.
 * Otherwise calls func_800A97FC to get a value and passes it to func_800B0754.
 *
 * @param a0 Entity index (stride 0xD0 in D_800ED148).
 */
void func_800B18A0(s32 arg0) {
    if (D_800ED148.unk130C != 0) {
        return;
    }

    if (D_800ED148.entities[arg0].status & 1) {
        return;    
    }

    if (D_800EE4C0.unk1 != 31) {
        return;
    }

    func_800B0754(arg0, 0, 7, func_800A97FC(arg0));
}

s32 func_800B1930(s32 arg0) {
    s32 val;

    val = 0;
    if ((D_800ED148.unk130C == 0) && !(D_800ED148.entities[arg0].status & 1)) {
        if ((D_800EE4C0.unk1 == 28) || (D_800EE4C0.unk1 == 243)) {
            func_800B0754(arg0, 0, 6, func_800A97FC(arg0));
            val = 1;
        }

        if (!(D_800ED148.entities[arg0].status & 1) && (D_800ED148.entities[arg0].status & 2)) {
            func_800B0754(arg0, 0, 2, func_800A97FC(arg0));
            val++;
        }
    }
    
    if (val != 0) {
        func_8009AE08(9);
    }
    
    return val;
}

/**
 * @brief Get entity from func_800AE6F8 and call func_800A59AC with mode 7.
 */
void func_800B1A48(void) {
    func_800A59AC(func_800AE6F8(), 7, 0);
}

/**
 * @brief Check if all enemy entities have HP below threshold 0xC8.
 *
 * @return 0 if a live enemy has value >= 0xC8, 1 otherwise.
 */
s32 func_800B1A78(void) {
    s32 i;

    for (i = 3; i < 7; i++) {
        if (!(D_800ED148.entities[i].status & 1) && (D_800ED148.entities[i].mentalRes[24] >= 200)) {
            return 0;
        }
    }
    
    return 1;
}

/**
* @brief Check battle conditions and trigger entity action sequence.
*
* Checks bit 1 of D_8007809A. If set, calls func_800B1A78 to validate.
* If valid, calls func_8009B79C(0x20, 0xFF) to test entity availability.
* If available, clears D_800EE45C and calls func_800B1A48 to start action.
*/

void func_800B1ACC(void) {
    if (!(g_gameState.mainData.partyLockFlag & 2)) {
        return;
    }
    
    if (func_800B1A78() == 0) {
        return;
    }
    
    if (func_8009B79C(32, 255) == 0) {
        return;
    }
    
    D_800ED148.unk1314 = 0;
    func_800B1A48();
}

/**
 * @brief Determine quadrant index from a rotation value.
 *
 * Calls func_8009B15C to get a rotation value, then maps it to a quadrant:
 * 0-63 → 0, 64-127 → 1, 128-191 → 2, 192+ → 3.
 *
 * @return Quadrant index (0-3).
 */
s32 func_800B1B1C(void) {
    s32 result = func_8009B15C();
    if (result < 64) {
        return 0;
    }
    
    if (result < 128) {
        return 1;
    }

    if (result < 192) {
        return 2;
    }

    return 3;
}

/**
 * @brief Process battle entity: compute type, store to D_800ED148[0x1314],
 *        trigger entity action, then set flag at D_800ED148[0x131D].
 */
void func_800B1B68(void) {
    D_800ED148.unk1314 = func_800B1B1C() + 7;
    func_800B1A48();
    D_800ED148.unk131D = 1;
}

/**
 * @brief Store battle type and value, then trigger entity action mode 8.
 *
 * @param a0 Type byte to store at D_800ED148[0x1314].
 * @param a1 Value to store at D_800ED148[0x12E6] as halfword.
 */
void func_800B1BA8(u8 arg0, u16 arg1) {
    D_800ED148.unk1314 = arg0;
    D_800ED148.unk12E6 = arg1;
    func_800A59AC(func_800AE6F8(), 8, 0);
}

/**
 * @brief Check if any other party entity has specific status flags.
 *
 * @param a0 Entity index to skip.
 * @param a1 Status mask to check.
 * @return 1 if a matching entity is found, 0 otherwise.
 */
s32 func_800B1BE4(s32 arg0, s32 arg1) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if ((i != arg0) && (D_800ED148.entities[i].controlFlags & 1) && (D_800ED148.entities[i].status & arg1)) {
            return 1;
        }  
    }
    
    return 0;
}

/**
 * @brief Call func_800B0754 with rearranged parameters.
 *
 * @param a0 First parameter (passed through as a0).
 * @param a1 Becomes a2 in the callee.
 * @param a2 Masked to 16 bits, becomes a3 in the callee.
 */
void func_800B1C3C(s32 a0, s32 a1, u16 a2) {
    func_800B0754(a0, 240, a1, a2);
}

void func_800B1C68(s32 arg0) {
    if (g_gameState.mainData.partyLockFlag & 0x10) {
        return;
    }
    
    if ((g_gameState.mainData.limitBreaks.angeloCompleted & 2) && (D_800ED148.entities[arg0].status & 0x100) && func_8009B79C(16, 255) != 0) {
        func_800B1C3C(arg0, 12, func_800A97FC(arg0));
    }

    else if ((g_gameState.mainData.limitBreaks.angeloCompleted & 1) && (func_8009B79C(16, 255) != 0)) {
        func_800B1C3C(arg0, 11, func_800A9904(arg0));
    }
}

s32 func_800B1D4C(s32 arg0, s32 arg1) {
    if (g_gameState.mainData.partyLockFlag & 0x10) {
        return 0;
    }

    if (arg1 < 3) {
        return 0;
    }
    
    if (D_800ED148.entities[arg0].comFileId != 4) {
        return 0;
    }
    
   if ((g_battleConfig.battleSceneId != 317) && (g_gameState.mainData.limitBreaks.angeloCompleted & 4) && (func_8009B79C(32, 255) != 0)) {
         return 1;
    }

    return 0;
}

void func_800B1DFC(void) {
    s32 idx;

    if (g_gameState.mainData.partyLockFlag & 8) {
        if ((D_800ED148.unk131D == 0) && (func_8009B79C(12, 255) != 0)) {
            func_800B1B68();
            return;
        }
    }

    if (g_gameState.mainData.partyLockFlag & 0x10) {
        return;
    }
    
    idx = func_800AA4F8(4);
    if (idx == 255) {
        return;
    }
    
    if ((g_gameState.mainData.limitBreaks.angeloCompleted & 2) && (func_800B1BE4(idx, 256) != 0) && (func_8009B79C(8, 255) != 0)) {
        func_800B1BA8(12, func_800AA034(0, 200, 0, 8, 0));
        return;
    }
    
    if (g_gameState.mainData.limitBreaks.angeloCompleted & 4) {
        if (D_800ED148.entities[idx].status & 1) {
            if (func_8009B79C(8, 255) != 0) {
                func_800B1BA8(13, func_800A97FC(idx) | 0x4000);
                return;
            }
        }

        if ((func_800B1BE4(idx, 1) != 0) && (func_8009B79C(2, 255) != 0)) {
        
            func_800B1BA8(13, func_800AA034(0, 200, 0, 0, 1) | 0x4000);
            return;
        }
    }
    
    if (!(g_gameState.mainData.limitBreaks.angeloCompleted & 8) || (D_800ED148.entities[idx].status & 5) || (D_800ED148.entities[idx].flags & 0x4009) || (func_8009B79C(8, 255) == 0)) {
        return; 
    }
    
    func_800B1BA8(14, func_800A97FC(idx));
}

/**
 * @brief Copy a byte from g_kernel.misc.summonInterval to D_800ED148.unk12E4 as a halfword.
 */
void func_800B2024(void) {
    D_800ED148.unk12E4 = g_kernel.misc.summonInterval;
}

/**
 * @brief Decrement entity timer at D_800ED148+0x12E4 or trigger completion.
 *
 * If the halfword timer at D_800ED148+0x12E4 is zero, calls func_800B1DFC
 * and func_800B2024 to handle completion. Otherwise decrements the timer.
 */
void func_800B2038(void) {
    if (D_800ED148.unk12E4 == 0) {
        func_800B1DFC();
        func_800B2024();
        return;
    }
    
    D_800ED148.unk12E4--;
}

/**
* @brief Check battle mode flag and conditionally trigger entity processing.
*
* Checks bit 3 of D_8007809A. If set, calls func_8009B79C(8, 0xFF) to test
* entity availability. If available, calls func_800B1B68 and returns.
* Otherwise (bit not set or entity unavailable), clears D_800ED148.unk131D.
*/

void func_800B2084(void) {
    if (g_gameState.mainData.partyLockFlag & 8 && func_8009B79C(8, 255) != 0) {
        func_800B1B68();
        return;
    }
    
    D_800ED148.unk131D = 0;
}

/**
 * @brief Check if all active party entities have status bit 2 set.
 *
 * @return 0 if an active entity without bit 2 is found, 1 otherwise.
 */
s32 func_800B20D8(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if ((D_800ED148.entities[i].controlFlags & 1) && !(D_800ED148.entities[i].status & 4)) {
            return 0;
        } 
    }
    
    return 1;
}


/**
* @brief Check if conditions are met to initiate an auto-battle action.
*
* Checks a chain of conditions: whether func_800AE788 returns the sentinel
* 0xFF, whether func_800B20D8 indicates busy, bit 2 of D_8007809A flags,
* whether g_battleConfig matches 0x13D, and whether entity slot 0x40 is
* available via func_8009B79C. If all pass, sets D_800EE45C to 1 and
* calls func_800B1A48 to start the action.
*
* @return 1 if action was initiated, 0 otherwise.
*/

s32 func_800B2128(void) {
    if (func_800AE788() == 255) {
        return 0;
    }
    
    if (func_800B20D8() != 0) {
        return 0;
    }
    
    if (!(g_gameState.mainData.partyLockFlag & 4)) {
        return 0;
    }
    
    if (g_battleConfig.battleSceneId == 317) {
        return 0;
    }
    
    if (func_8009B79C(64, 255) == 0) { 
        return 0;
    }
    
    D_800ED148.unk1314 = 1;
    func_800B1A48();
    return 1;
}

/**
 * @brief Copy 3 bytes from g_gameState to D_800EE9E8 at stride 0x47.
 *
 * Copies bytes from g_gameState[i+0xAF4] to D_800EE9E8[i*0x47+0xA3]
 * for i = 0, 1, 2.
 */
void func_800B21B4(void) {
    s32 i;
    
    for (i = 0; i < 3; i++) {
        D_800EE9E8.subEntries[i].array0[0].unk3 = g_gameState.mainData.party.partyMembers[i];
    }
}

/**
 * @brief Search D_800EE9E8 for an entry matching a given value.
 *
 * Scans 3 entries at stride 0x47 in D_800EE9E8. If byte at offset 0xA3
 * matches a0, returns 0 (found). Returns 1 if no match found.
 *
 * @param a0 Value to search for.
 * @return 0 if found, 1 if not found.
 */
s32 func_800B21EC(s32 arg0) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (D_800EE9E8.subEntries[i].array0[0].unk3 == arg0) {
            return 0;
        }
    }

    return 1;
}

s32 func_800B2224(u8* arg0, s32 size) {
    s32 idx;
    s32 tmp;
    s32 ret;
    s32 i;

    tmp = -1;

    for (i = 0; i < size; i++) {
        idx = *arg0++;
        if (g_gameState.chars[idx].experience < tmp) {
            ret = idx;
            tmp = g_gameState.chars[idx].experience;
        }
    }
 
    return ret;
}

s32 func_800B228C(void) {
    u8 buffer[8];
    s32 size;
    s32 i;

    size = 0;
    for (i = 0; i < 8; i++) {
        if ((g_gameState.chars[i].exists & 1) && !(g_gameState.chars[i].statusFlags & STATUS_KO) && (func_800B21EC(i) != 0)) {
            buffer[size++] = i;
        }
    }
        
    if (size == 0) {
        return 255;
    }

    return func_800B2224(buffer, size);
}

/**
 * @brief Process current battle entity: update effects and activate ability.
 *
 * Reads the entity index from D_800ED148[0x1301], calls func_800AE6C0
 * and func_800D3090 with it, then calls func_800D0530 and func_800AB3C4.
 */
void func_800B2338(void) {
    func_800AE6C0(D_800ED148.unk1301);
    func_800D3090(D_800ED148.unk1301, 1);
    func_800D0530();
    func_800AB3C4();
}

void func_800B2388(void) {
    s32 idx;

    idx = D_800ED148.unk1301;
    g_gameState.mainData.party.partyMembers[idx] = D_800ED148.unk1328;
    
    func_80022E08(g_gameState.mainData.party.partyMembers[idx], idx);
    func_800231E0(g_gameState.mainData.party.partyMembers[idx], idx);
    recalcAllGfStats();
    func_800A7518(idx);
    func_800A71C0(idx);
    func_8009A8B4(idx);
    func_8009A6A8(idx);
    func_8009B134(14, 128, 0);
    func_8009B134(112, 128, 0);
    func_8009AF14(&func_800B2338);
}

void func_800B243C(s32 arg0) {
    s32 result;

    if (g_battleConfig.battleSceneId != 511) {
        return;
    }
    
    func_800B21B4();
    result = func_800B228C();
    if (result == 255) {
        return;
    }

    func_800AB3E0();
    func_800D3090(arg0, 0);
    D_800ED148.unk1301 = arg0;
    D_800ED148.unk1328 = result;
    func_8009AF14(&func_800B2388);
}

// functions seem to become weird from here, most likely a split occurs

extern u8 *D_800EEED8;
extern u8 D_800E3D0C;
extern s32 D_800EEEB8;
extern s32 D_800EEEBC;
extern s32 D_800EEEC0;
extern u8  D_800EEEC4;
extern s32 D_800EEEC8;
extern s32 D_800EEECC;
extern u8 D_800EE45C[];
extern u8 D_800EEDD8[];
extern u8 D_800EEDE8[];
extern u8 D_800EF020;
extern u8* D_800EEED0;
extern u8 D_800EEED4;
extern u8 D_800EEC68[];
extern u8 D_800EEDC8[];

extern s32 D_800EEC38;
extern s32 D_800EEC44;
extern s32 D_800EEC48;
extern s32 D_800EEC4C;
extern s32 D_800EEC50;
extern s8 D_800EEC54;
extern s32 D_800EEC58;
extern s8 D_800EEC60;
extern s8 D_800F05D8;
extern s8 D_800F1B88;

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B24C8);


s32 func_800B25E4(void) {
    D_800EEC5C = 0;
    func_800B36D8();
    func_800B7C48();
    func_800C65A8();
    func_800BAE6C();
    func_800B2EDC();
    func_800B650C();
    func_800C42DC();
    func_800B3470();
    D_800EEC44 = func_800B84C8();
    D_800EEC38 = func_800B4248();
    D_800EEC48 = func_800B2C14();
    D_800EEC4C = func_800C749C();
    D_800EEC50 = func_800C0134();
    D_800EEC58 = 0;
    D_800EEC60 = 0;
    D_800EEC54 = 0;
    D_800F05D8 = 0;
    D_800F1B88 = 4 - g_gameState.config.camera;
    return 0;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B26B8);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B27AC);

/**
 * @brief Advance a two-phase process based on state byte at offset 0xD.
 *
 * State 0: Calls func_800B3128 with data at offset 0xE, increments state.
 * State 1: If byte at 0xE is non-zero, writes 0xFF to target+1 and returns 2.
 * Otherwise returns 0.
 *
 * @param a0 Pointer to process state structure.
 * @return 0 if still in progress, 2 if complete.
 */
s32 func_800B2848(u8 *a0) {
    u8 state = a0[0xD];
    s32 target = *(s32 *)(a0 + 0x10);

    if (state == 0) goto case0;
    if (state == 1) goto case1;
    goto ret0;

case0:
    func_800B3128(a0 + 0xE);
    a0[0xD] = a0[0xD] + 1;
    goto ret0;

case1:
    if (a0[0xE] == 0) goto ret0;
    *(u8 *)(target + 1) = 0xFF;
    return 2;

ret0:
    return 0;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B28C8);

/**
 * @brief Initialize a table header and zero its entries.
 *
 * Sets up a table header at a0 with pointer to data, entry stride,
 * and entry count. Then zeros the first halfword of each entry.
 *
 * @param a0 Pointer to table header.
 * @param data Pointer to table data.
 * @param stride Byte stride between entries.
 * @param count Number of entries to zero.
 */
void func_800B2A00(void *header, void *data, s32 stride, s32 count) {
    s32 i = 0;
    u8 *a0 = header;
    u8 *entry = data;
    *(s32 *)a0 = 0;
    *(s32 *)(a0 + 4) = 0;
    *(s32 *)(a0 + 8) = (s32)entry;
    *(u16 *)(a0 + 0xC) = stride;
    *(u16 *)(a0 + 0xE) = count;
    if (count > 0) {
        do {
            *(u16 *)entry = 0;
            i++;
            entry += stride;
        } while (i < count);
    }
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B2A38);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B2A84);

/**
 * @brief Allocate a node, initialize it, and prepend to linked list.
 *
 * Allocates a node via func_800B2A38, sets bit 0 of its flags,
 * clears halfword at offset 2, stores callback at offset 8,
 * and prepends the node to the linked list at a0.
 *
 * @param list Pointer to linked list head pointer.
 * @param callback Value to store at node offset 8.
 * @return Pointer to the new node, or NULL if allocation failed.
 */
s32 func_800B2B00(u8 *list, s32 callback) {
    u8 *node = (u8 *)func_800B2A38(list);
    if (node != 0) {
        u16 flags = *(u16 *)node;
        *(u16 *)(node + 2) = 0;
        *(s32 *)(node + 8) = callback;
        *(u16 *)node = flags | 1;
        *(s32 *)(node + 4) = *(s32 *)list;
        *(s32 *)list = (s32)node;
    }
    return (s32)node;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B2B68);

/**
 * @brief Initialize D_800EEDC8 buffer via func_800B2A00 and return it.
 *
 * Calls func_800B2A00 with D_800EEDC8, D_800EEC68, size 0x2C, and count 8.
 *
 * @return Pointer to D_800EEDC8.
 */
u8* func_800B2C14(void) {
    func_800B2A00(D_800EEDC8, D_800EEC68, 44, 8);
    return D_800EEDC8;
}


/**
 * @brief Call func_800B2A84 with D_800EEDC8 and the given parameter.
 *
 * @param task Per-frame step installed on the new task.
 * @return Result from func_800B2A84.
 */
void *func_800B2C58(void *task) {
    return func_800B2A84(D_800EEDC8, task);
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B2C80);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B2D0C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B2E04);

/**
 * @brief Initialize sound system resources and configuration.
 *
 * Sets up sound data via func_800B2A00 with D_800EEDD8 and D_800EEDE8,
 * then initializes sound channels via sequential calls.
 */
void func_800B2EDC(void) {
    func_800B2A00(D_800EEDD8, D_800EEDE8, 0x18, 8);
    func_800DF904();
    func_800DF8E4(3, 0x57);
    func_800DF8C4(3, 0);
    func_800DF8A4(3, 0);
}

void func_800B2F3C(void);
INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B2F3C);

/**
 * @brief Allocate handler for func_800B2F3C and store entity pointers.
 *
 * Registers func_800B2F3C as callback via func_800B2C58. If allocation
 * succeeds, clears byte at +0xC, stores a0 at +0x10 and a1 at +0x14,
 * then clears the first byte of a1.
 *
 * @param a0 Entity pointer stored at result offset 0x10.
 * @param a1 Pointer to completion flag byte, stored at result offset 0x14.
 */
void func_800B2FF8(s32 a0, u8 *a1) {
    u8 *result = func_800B2C58(func_800B2F3C);
    if (result != 0) {
        result[0xC] = 0;
        *(s32 *)(result + 0x10) = a0;
        *(s32 *)(result + 0x14) = (s32)a1;
        *a1 = 0;
    }
}

void func_800B304C(void);
INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B304C);

/**
 * @brief Allocate a handler for func_800B304C, initialize fields, and clear target byte.
 *
 * @param a0 Pointer whose first byte will be cleared; stored at result[0x14].
 */
void func_800B3128(u8 *a0) {
    u8 *result = func_800B2C58(func_800B304C);
    result[0xC] = 0;
    *(s32 *)(result + 0x14) = (s32)a0;
    *a0 = 0;
}

void func_800B3164(void);
INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B3164);

/**
 * @brief Set up audio stream from descriptor and register playback callback.
 *
 * Computes buffer start and end from the descriptor's offset fields,
 * stores them to D_800EEEB8/D_800EEEBC/D_800EEEC0, then registers
 * func_800B3164 as a callback and links the control byte.
 *
 * @param a0 Audio descriptor with offsets at +4, +8, +0xC.
 * @param a1 Pointer to control byte (cleared after setup).
 */
void func_800B3270(s32 *a0, u8 *a1) {
    u8 *result;
    D_800EEEB8 = (s32)a0 + a0[1];
    D_800EEEBC = (s32)a0 + a0[2];
    D_800EEEC0 = a0[3] - a0[2];
    result = func_800B2C58(func_800B3164);
    result[0xC] = 0;
    *(s32 *)(result + 0x10) = (s32)a1;
    *a1 = 0;
}

/**
 * @brief Set D_800EEEC4 to 1, D_800E3D0C to 3, and store a0/a1 to D_800EEEC8/CC.
 *
 * @param a0 Value to store to D_800EEEC8.
 * @param a1 Value to store to D_800EEECC.
 */
void func_800B32E0(s32 a0, s32 a1) {
    D_800EEEC4 = 1;
    D_800E3D0C = 3;
    D_800EEEC8 = a0;
    D_800EEECC = a1;
}

/**
 * @brief Set D_800EEEC4 to 1, D_800E3D0C to 4, and store a0 to D_800EEEC8.
 *
 * @param a0 Value to store to D_800EEEC8.
 */
void func_800B330C(s32 a0) {
    D_800EEEC4 = 1;
    D_800E3D0C = 4;
    D_800EEEC8 = a0;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B3330);

/**
 * @brief Initialize sound data pointer based on g_battleConfig.unk9 flag.
 *
 * Clears D_800EEEC4 and D_800EEED4, then sets D_800EEED0 to either
 * D_8005F388 (if g_battleConfig.unk9 is zero) or D_80063388 (if non-zero).
 */
void func_800B3470(void) {
    D_800EEEC4 = 0;
    if (g_battleConfig.unk9 == 0) {
        D_800EEED0 = D_8005F388;
    } 
    
    else {
        D_800EEED0 = D_80063388;
    }
    
    D_800EEED4 = 0;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B34B0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B3534);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B3574);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B3650);

/**
 * @brief Allocate aligned memory from the scratchpad buffer.
 *
 * Aligns the requested size up to 4 bytes, advances the D_800EEED8
 * pointer, and returns the old (pre-advance) pointer.
 *
 * @param size Number of bytes to allocate.
 * @return Pointer to the allocated region.
 */
void *func_800B3698(s32 size) {
    u8 *ptr = D_800EEED8;
    D_800EEED8 = ptr + ((size + 3) & ~3);
    return ptr;
}

/**
 * @brief Free aligned memory back to the scratchpad buffer.
 *
 * Aligns the requested size up to 4 bytes and moves the D_800EEED8 pointer
 * back by that much. The subtraction folded into the store is what leaves the
 * new pointer in v0 the way the original does; a two-statement spelling swaps
 * v0 and v1.
 *
 * @param size Number of bytes to free.
 */
void func_800B36B8(s32 size) {
    u8 *ptr = D_800EEED8;

    D_800EEED8 = ptr - ((size + 3) & ~3);
}

/**
 * @brief Set D_800EEED8 to the scratchpad base address 0x1F800000.
 */
void func_800B36D8(void) {
    D_800EEED8 = (u8 *)getScratchAddr(0);
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B36E8);

void func_800B3738(void) {
    s32 i;

    D_800EF724 &= 0x8000;
    for (i = 0; i < 7; i++) {
        if ((D_800EF2D0[i].flags & 2) && !(D_800EF2D0[i].unk02 & 4) && !(D_800EF2D0[i].flags & 0x10)) {
            D_800EF724 |= 1 << i;
        }
    }
}

/**
 * @brief Initialize SFX entry fields, setting pitch based on address range.
 *
 * If the entry pointer is below D_800EF2D0[3], pitch is cleared to 0.
 * Otherwise pitch is set to 0x800. Both paths clear volume and rate fields.
 *
 * @param entry Pointer to SFX entry structure.
 */
void func_800B37B4(BattleEffectSlot* arg0) {
    if (arg0 < &D_800EF2D0[3]) {
        arg0->facing = 0;
    } 
    
    else {
        arg0->facing = 0x800;
    }
    
    arg0->unkC = 0;
    arg0->unk10 = 0;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B37E0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B3960);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B3AE8);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B3B54);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B3C2C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B3E54);

/**
 * @brief Initialize sound effect table and clear entries.
 *
 * Clears the flags of all 7 battle slots, then initializes D_800EF020
 * via func_800B2A00
 * with D_800EEFB0, stride 0x10, and count 7. Clears D_800EF724.
 *
 * @return Pointer to D_800EF020.
 */
u8* func_800B4248(void) {
    s32 i;

    for(i = 0; i < 7; i++) {
        D_800EF2D0[i].flags = 0;
    }
    
    func_800B2A00(&D_800EF020, &D_800EEFB0, 16, 7);
    D_800EF724 = 0;
    return &D_800EF020;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B42B4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B44D8);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object9", func_800B4920);
