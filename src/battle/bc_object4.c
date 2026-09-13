#include "common.h"
#include "battle.h"
#include "gf.h"
#include "gamestate.h"
#include "battle/bc_object4.h"
extern void func_800E1850(void);


void func_800A6184(s32 arg0, s32 arg1, s32 arg2, u16 arg3) {
    func_800A5A7C(arg0, arg1, arg2, 0, arg3, 0, &D_800ED148.unk1244[arg0].unk0);
}

void func_800A61CC(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, u16 arg5) {
    func_800A5A7C(arg0, arg1, arg2, arg4, arg5, arg3, &D_800ED148.unk1244[arg0].unk0);
}

/**
 * @brief Check entity flag 0x10 at offset 0x8C and either set bit 0x200 or call display handler.
 *
 * If the entity at D_800ED148[a0] (stride 0xD0) has bit 0x10 set in its
 * word at offset 0x8C, sets bit 0x200 in that word. Otherwise, calls
 * func_800A62DC with display parameters (0x11, 0x80, 0).
 *
 * @param a0 Entity index.
 */
void func_800A6218(s32 arg0) {
    if ((D_800ED148.entities[arg0].controlFlags & CTRL_FLAG_10)) {
        D_800ED148.entities[arg0].controlFlags |= 0x200;
    }
    
    else {
        func_800A62DC(arg0, 17, 128, 0);
    }
}

/**
 * @brief Call func_800D15B4 with display parameters and grid settings.
 *
 * @param a0 First display parameter.
 * @param a1 Brightness or alpha value.
 * @param a2 Size parameter.
 * @param a3 Grid configuration.
 */
void func_800A6288(s32 a0, s32 a1, s32 a2, s32 a3) {
    func_800A62DC(a0, 18, 128, 0);
}

/**
 * @brief Initialize display with default bright settings.
 *
 * Calls func_800A62DC with a0=0, a1=0x41, a2=0x80, a3=0.
 */
void func_800A62B0(void) {
    func_800A62DC(0, 65, 128, 0);
}

void func_800A62DC(s32 a0, s32 a1, s32 a2, s32 a3) {
    func_800D15B4(a0, a1, a2, a3);
}

s32 func_800A62FC(void) {
    return D_800ED148.unk12EB;
}

/**
 * @brief Initialize battle entity state and process active entries.
 *
 * Sets up entity flags, copies status bytes to D_800EE4C0, stores
 * the parameter to D_800EEBC8, then iterates through active entity
 * entries calling func_800A09D0 and func_800A5210 for each.
 *
 * @param a0 Value to store at D_800EEBC8.
 */
void func_800A6310(u8 arg0) {
    s32 i;
    BattleEntry* entry;
    SubEntry* subEntry;

    D_800ED148.unk131E = 1;
    D_800EEBC8 = arg0;
    D_800ED148.unk1300 = 0;
    D_800ED148.unk5C1 = 0;
    entry = D_800ED148.entries;
    D_800EE4C0.unk0 = entry->unk0;
    D_800EE4C0.unk1 = entry->unk1;
    D_800EE4C0.statusCode = entry->unk4;

    for (i = 0; i < entry->unk10; i++) {
        subEntry = &D_800ED148.Array844[i];
        func_800A09D0(subEntry->unk0);
        func_800A5210(subEntry->unk0);
    }
}

void func_800A63C0(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    D_800ED148.unk1290 = arg0;
    D_800ED148.unk1292 = arg1;
    D_800ED148.unk1296 = arg3;
    D_800ED148.unk1294 = arg2;
}

void func_800A63DC(void) {
    BattleEntry* entry;
    SubEntry* subEntry;
    
    if (D_800ED148.unk1290 != 0) {
        entry = &D_800ED148.entries[0];
        D_800EE4C0.unk1 = entry->unk1;
        
        if (D_800ED148.unk1292 == 1) {
            if (entry->unk1 == 12) {
                D_800EE4C0.unk1 = 248;
            } 
            
            else if (entry->unk1 == 28) {
                D_800EE4C0.unk1 = 243;
            }
            
            else {
                D_800EE4C0.unk1 = 253;
            }
        }
            
        else {
            D_800EE4C0.unk1 = 251;
        }
        
        D_800EE4C0.unk0 = entry->unk0;
        subEntry = &D_800ED148.Array844[0];
        
        if ((D_800ED148.unk1292 == 1) && (D_800ED148.entities[D_800EE4C0.unk0].status & 8)) {
            D_800ED148.unk1294 = 1;
        }
        
        D_800ED148.unk5C1 = 0;
        func_800A09D0(subEntry->unk0);
        func_800A5210(subEntry->unk0);
        D_800ED148.unk1290 = 0;
    }
}

/**
 * @brief Initiate battle sequence based on mode flag.
 *
 * If a0 is 0, plays sound 0xED with entity data, runs scene setup,
 * and registers handler func_800E1850. If a0 is non-zero, calls
 * func_800AA4E8 first, plays sound 0xEE, and dispatches event 8.
 *
 * @param a0 Mode flag (0 = player-initiated, non-zero = triggered).
 * @param a1 Sound parameter (masked to u16 in mode 0).
 */
void func_800A64E4(s32 arg0, u16 arg1) {
    if (arg0 == 0) {
        func_800B0754(D_800ED148.entities[0].entityRef, 237, D_800ED148.unk1324, arg1);
        decrementItemByType(D_800ED148.unk1324 + 101);
        func_8009AF14(func_800E1850);
        return;
    }
    
    func_800B0754(D_800ED148.entities[0].entityRef, 238, D_800ED148.unk1324, func_800AA4E8());
    func_8009AE08(8);
}

void func_800A6574(u8 arg0) {
    D_800ED148.unk12EF = arg0;
}

void func_800A6588(void) {
    D_800ED148.unk131C = 0;
    func_8009AE08(8);
}

void func_800A65B0(void) {
    u8 val;

    val = D_800ED148.unk12EF;
    if (val == 255) {
        D_800ED148.unk12EF = 254;
        func_800B0754(D_800ED148.entities[0].entityRef, 241, 65532, D_800ED148.unk12E2);
        func_800A6588();
        return;
    }
    
    if (val == 254) {
       func_8009AF14(func_800A65B0);
        return;
    }

    D_800ED148.unk131A = D_800ED148.unk12EF;
    val = D_80078E00.unk49F8[D_800ED148.unk131A];
    D_800ED148.unk12EF = 254;
    D_800ED148.unk131B = val;
    
    if (val < 6) {
        func_800B0754(D_800ED148.entities[0].entityRef, 241, D_800ED148.unk131B, D_800ED148.unk12E2);
        func_8009AF14(func_800A65B0);
        return;
    }
    
    func_800A30F8(D_800ED148.entities[0].entityRef, 241, 65530, D_800ED148.unk131B, D_800ED148.entities[0].entityRef, D_800ED148.unk12E2, 0);
    func_800B06DC(D_800ED148.unk12E2);
    func_800B0754(D_800ED148.entities[0].entityRef, 239, D_800ED148.unk131B, func_8009BA5C(D_800ED148.unk131B, D_800ED148.unk12E2));
    func_800A6588();
}

void func_800A66D0(s32 arg0) {
    if (func_8009A514(D_800ED148.unkCE3, 7 - arg0) != 0) {
        D_800ED148.unkD5C[arg0] = 1;
    }
}

/**
 * @brief Clear entity active flags and reinitialize all 8 slots.
 *
 * Clears 8 bytes at D_800ED148+0xD5C (entity active flags),
 * then calls func_800A66D0 for each slot 0-7 to re-check
 * and set active status.
 */
void func_800A6724(void) {
    s32 i;
    
   for (i = 0; i < 8; i++) {
        D_800ED148.unkD5C[i] = 0;
    }


    for (i = 0; i < 8; i++) {
        func_800A66D0(i);
    }
}

void func_800A6780(s32 arg0) {
  
    if (!(D_800ED148.entities[arg0].status & 1) && !(D_800ED148.entities[arg0].status & 4)) {
        D_800ED148.entities[arg0].unk24 = D_800ED148.entities[arg0].unk20;
        return;
    }
    
    D_800ED148.entities[arg0].unk24 = 0;
}

void func_800A67FC(s32 arg0) {
    s32 i;

    if (arg0 == 0) {
        for (i = 0; i < 3; i++) {
            if (!(D_800ED148.entities[i].status & 1)) {
                D_800ED148.entities[i].flags |= 0x800000;
                g_battleChars.chars[i].unk188 = D_800ED148.entities[i].flags;
            }
        }
    }
        
    else {
        for (i = 3; i < 7; i++) {
            if (!(D_800ED148.entities[i].status & 1)) {
                D_800ED148.entities[i].flags |= 0x800000;
            }
        }
    }
}

void func_800A68AC(s32 arg0) {
    s32 i;

    switch (arg0) {
        case 0:
            for (i = 0; i < 3; i++) {
                func_800A6780(i);
            }
            break;

        case 1:
            for (i = 0; i < 3; i++) {
                if (!(g_battleChars.chars[i].statusFlags & 0x10000)) {
                    D_800ED148.entities[i].unk24 = 0;
                }
            }
            break;

        case 2:
            for (i = 3; i < 7; i++) {
                func_800A6780(i);
            }
            break;

        case 3:
            for (i = 3; i < 7; i++) {
                D_800ED148.entities[i].unk24 = 0;
            }
            break;

        default:
            break;
    }
}

void func_800A69BC(void) {
    switch (D_800ED148.unk1308) {
        case 1:
            func_8009AF3C(getMenuString(0x2E), (D_80077E59 * 8) + 8, 3, 128, 86);
            break;
        
        case 2:
            func_8009AF3C(getMenuString(0x2D), (D_80077E59 * 8) + 8, 3, 128, 86);
            break;
        
        case 3:
            func_8009AF3C(getMenuString(0x2C), (D_80077E59 * 8) + 8, 3, 128, 86);
            break;
        
        case 4:
            func_8009AF3C(getMenuString(0x2F), (D_80077E59 * 8) + 8, 3, 128, 86);
            break;
    }
}

void func_800A6A58(void) {
    func_800A68AC(1);
    func_800A68AC(2);
    func_800A67FC(0);
}

void func_800A6A88(void) {
    func_800A68AC(1);
    func_800A68AC(2);
}

void func_800A6AB0(void) {
    func_800A68AC(0);
    func_800A68AC(3);
    func_800A67FC(1);
}

void func_800A6AE0(void) {
    func_800A68AC(0);
    func_800A68AC(3);
}

/**
 * @brief Check if any entity in slots 3-6 has bit 2 set in indirect flag byte.
 *
 * Iterates over entity slots 3 through 6 in the D_800ED148 table (stride 0xD0).
 * For each entity whose status halfword at offset 0x90 does not have bit 0 set,
 * follows the pointer chain at offset 0x10 twice, then checks if bit 2 (0x4)
 * is set in the byte at offset 0xFE of the resulting pointer.
 *
 * @return 1 if any qualifying entity has the flag set, 0 otherwise.
 */
s32 func_800A6B08(void) {
    s32 i;

    for (i = 3; i < 7; i++) {
        if (!(D_800ED148.entities[i].status & 1) && (*D_800ED148.entities[i].entityData)->unkFE & 4) {
            return 1;
        }
    }
    
    return 0;
}

/**
 * @brief Check if all entities in slots 3-6 have a specific flag bit set.
 *
 * Iterates over entity slots 3 through 6 in the D_800ED148 table (stride 0xD0).
 * For each entity whose status halfword at offset 0x90 does not have bit 0 set,
 * follows the pointer chain at offset 0x10 twice, then checks if the bitmask a0
 * is set in the byte at offset 0xFE.
 *
 * @param a0 Bitmask to check against the flag byte.
 * @param a1 Value to return if all entities pass the check.
 * @return 0 if any qualifying entity lacks the flag, a1 if all pass.
 */
s32 func_800A6B6C(s32 arg0, s32 arg1) {
    s32 i;

    for (i = 3; i < 7; i++) {
        if (!(D_800ED148.entities[i].status & 1) && !((*D_800ED148.entities[i].entityData)->unkFE & arg0)) {
            return 0;
        }
    }
    
    return arg1;
}

/**
 * @brief Determine battle result based on condition and ability check.
 *
 * If a0 is 1, returns 0. Otherwise checks func_8009B74C(0x80, 0xFF)
 * to determine the return value. For a0==0, returns 3 or 4; for other
 * values, returns 1 or 2.
 *
 * @param a0 Condition selector.
 * @return Battle result code (0-4).
 */
s32 func_800A6BD0(s32 a0) {
    if (a0 == 1) {
        return 0;
    }

    if (a0 == 0) {
        if (func_8009B74C(128, 255)) {
            return 3;
        }

        return 4;
    }

    if (func_8009B74C(128, 255)) {
        return 1;
    }

    return 2;
}

s32 func_800A6C34(void) {
    s32 var_s0;
    s32 var_s1;


    if (D_80082C0A & 0x80) {
        return 0;
    }
      
    if (D_80082C0A & 0x20) {
        return 1;
    }
    
    if (D_80082C0A & 0x40) {
        return 2;
    }
    
    var_s0 = func_800A6B6C(1, 20);
    var_s0 += func_800A6B6C(2, -20);
    var_s0 += func_8009B15C();
    
    if (D_80078DF8 & 1) {
        var_s0 -= 20;
    }
    
    var_s1 = 0;
    if (var_s0 > 19) {
        var_s1 = 2;
        if (var_s0 < 236) {
            var_s1 = 1;
        }
    }
    
    if ((D_80078DF8 & 1) && (var_s1 == 2)) {
        var_s1 = 1;
    }
    
    if ((func_800A6B08() != 0) && (var_s1 == 0)) {
        var_s1 = 1;
    }
    
    return func_800A6BD0(var_s1);
}

void func_800A6D30(void) {
    D_800ED148.unk1308 = func_800A6C34();
    switch (D_800ED148.unk1308) {
        case 1:
            func_800A6A88();
            return;

        case 2:
            func_800A6A58();
            return;

        case 3:
            func_800A6AE0();
            return;

        case 4:
            func_800A6AB0();
            return;
    }
}

s32 func_800A6DD8(void) {
    s32 i;
    s32 var_a2;
    s32 var_v1;

    var_a2 = 0;
    var_v1 = 0;
    
    for (i = 0; i < 3; i++) {
        if (D_800ED148.entities[i].linkedIdx != 255) {
            var_a2++;
            var_v1 += D_800ED148.entities[i].unkCC;
        }
    }
    
    return var_v1 / var_a2;
}

s32 func_800A6E2C(void) {
    s32 result;

    result = func_800A6DD8();
    
    if (func_8009B15C() & 1) {
        result += (result / 5);
    }
    
    else {
        result -= (result / 5);
    }
    
    if (result < 1) {
        return 1;
    }
        
    if (result > 100) {
        return 100;
    }

    return result;
}

s32 func_800A6EBC(void) {
    s32 result;
    s32 temp_v0;

    result = func_800A6DD8();
    
    if (func_8009B15C() & 1) {
        temp_v0 = func_8009B15C();
        result += (temp_v0 - (temp_v0 / 4 * 4));
    } 
    
    else {
        temp_v0 = func_8009B15C();
        result -= (temp_v0 - (temp_v0 / 4 * 4));
    }
   
    if (result < 1) {
        return 1;
    }
        
    if (result > 65) {
        return 65;
    }

    return result;
}

s32 func_800A6F64(void) {
    s32 result;
  
    result = func_8009B15C() % func_800A6E2C();
    if (result == 0) {
        return 1;
    }
        
    else {
        if (result > 100) {
            return 100;
        }
        
        return result;
    }
}

/**
 * @brief Generate random value in range [1, 100].
 *
 * Calls func_8009B15C to get a random number, computes modulo 100,
 * and adds 1.
 *
 * @return Random value in [1, 100].
 */
s32 func_800A6FB8(void) {
    return func_8009B15C() % 100 + 1;
}

s32 func_800A700C(s32 arg0) {
    return func_800A6E2C() - (200 - arg0);
}

/**
 * @brief Compute adjusted value and return the lesser of it and func result.
 *
 * Calls func_800A6E2C, subtracts 100 from a0, and returns the minimum
 * of the two values.
 *
 * @param a0 Input value (100 subtracted before comparison).
 * @return Minimum of (a0 - 100) and func_800A6E2C result.
 */
s32 func_800A703C(s32 a0) {
    s32 limit = func_800A6E2C();
    a0 -= 100;
    if (a0 < limit) {
        return a0;
    }

    return limit;
}

s32 func_800A7080(s32 arg0) {
    if (arg0 < 101) {
        return arg0;
    }
    
    if(arg0 == 255) {
        return func_800A6E2C();
    }
    
    if(arg0 == 254) {
        return func_800A6DD8();
    }
    
    if(arg0 == 253) {
        return func_800A6F64();
    }
    
    if(arg0 == 252) {
        return func_800A6FB8();
    }
    
    if(arg0 == 251) {
        return func_800A6EBC();
    }

    if (arg0 > 200 && arg0 < 251) {
        return func_800A700C(arg0);
    }
    

    if (arg0 > 100 && arg0 < 201) {
        return func_800A703C(arg0);
    }
}

s32 func_800A7154(s32 arg0) {
    s32 val = func_800A7080(arg0);
    if (val > 100) {
        return 100;
    }
    
    return val;
}

/**
 * @brief Clear five bytes around the animation-param region of a battle slot.
 *
 * Zeroes the @c animParam3 halfword (offsets @c 0x88-0x89), the first
 * byte of @c pad8A (@c 0x8A), and bytes @c 0xC7 / @c 0xC8 inside
 * @c padBC[]. Order: @c 0x8A → @c 0x89 → @c 0x88 → @c 0xC7 → @c 0xC8.
 *
 * @param slot Battle slot to clear.
 */
void func_800A7188(BattleEntityData* arg0) {
    arg0->unk8A = 0;
    arg0->unk89 = 0;
    arg0->unk88 = 0;
    arg0->unkC7 = 0;
    arg0->unkC8 = 0;
}

/**
 * @brief Clear 8 consecutive words starting at @c slot+0x24.
 *
 * Zeroes 8 @c s32 words at offsets @c 0x24, @c 0x28, @c 0x2C, @c 0x30,
 * @c 0x34, @c 0x38, @c 0x3C, @c 0x40 — i.e. @c field24, @c field28,
 * @c field2C, and the first 0x14 bytes of @c pad30[].
 *
 * @param slot Battle slot whose mid-region words are zeroed.
 */
void func_800A71A0(BattleEntityData* arg0) {
    s32 i;

    for (i = 0; i < 8; i++) {
        arg0->unk24[i] = 0;
    }
}

void func_800A71C0(s32 idx) {
    BattleCharData* temp_a3;
    BattleEntityData* temp_a0;
    s32 i;

    temp_a3 = &g_battleChars.chars[idx];
    temp_a0 = (BattleEntityData*)&D_800ED148.entities[idx].entityData;
    
    if (temp_a3->characterId == 255) {
        return;
    }
    
    temp_a0->unk1C = temp_a3->hpRegenCap;
    temp_a0->unk18 = temp_a3->unk172;
    temp_a0->unkBC = temp_a3->level;
    temp_a0->unkBD = temp_a3->stats[0];
    temp_a0->unkBE = temp_a3->stats[1];
    temp_a0->unkBF = temp_a3->stats[2];
    temp_a0->unkC0 = temp_a3->stats[3];
    temp_a0->unkC1 = temp_a3->stats[4];
    temp_a0->unkC2 = temp_a3->stats[5];
    temp_a0->unkC4 = temp_a3->stats[7];
    temp_a0->unkC3 = temp_a3->stats[6];
    
    for (i = 0; i < 32; i++) {
        if (temp_a3->magicSlots[i].unk0 != 0) {
            temp_a0->unk8 |= 0x40000000;
            goto found;
        }
    }
    
    temp_a0->unk8 &= ~0x40000000;

    found:
    for (i = 0; i < 8; i++) {
        temp_a0->unk44[i] = temp_a3->elemResistances[i];
    }
    
    
    for (i = 0; i < 40; i++) {
        temp_a0->unk90[i] = 100;
    }

    

    temp_a0->unk90[2] = temp_a0->unk90[20] = temp_a3->statusResistances[2];
    temp_a0->unk90[0] = temp_a3->statusResistances[0];
    temp_a0->unk90[1] = temp_a3->statusResistances[1];
    temp_a0->unk90[3] = temp_a3->statusResistances[3];
    temp_a0->unk90[4] = temp_a3->statusResistances[4];
    temp_a0->unk90[5] = temp_a3->statusResistances[5];
    temp_a0->unk90[6] = temp_a3->statusResistances[6];
    temp_a0->unk90[22] = temp_a3->statusResistances[11];
    temp_a0->unk90[8] = temp_a3->statusResistances[7];
    temp_a0->unk90[10] = temp_a3->statusResistances[8];
    temp_a0->unk90[11] = temp_a3->statusResistances[9];
    temp_a0->unk90[17] = temp_a3->statusResistances[10];
    temp_a0->unk90[23] = temp_a3->statusResistances[12];
    temp_a0->unk20 = temp_a3->abilityFlags;
    temp_a0->unk86 = temp_a3->abilityValue;
    temp_a0->unkBA = temp_a3->atkStatusHit;
    temp_a0->unkC5 = temp_a3->atkElemBase;
    temp_a0->unkC6 = temp_a3->atkElemBonus;
    
    if (temp_a3->statusFlags & 0x8000) {
        temp_a0->unk90[11] = 200;
        temp_a0->unk90[10] = 200;
    }
    
    if (temp_a3->statusFlags & 0x80000) {
        temp_a0->unk90[24] = 200;
        temp_a0->unk90[32] = 200;
        temp_a0->unk90[23] = 200;
        temp_a0->unk90[22] = 200;
        temp_a0->unk90[20] = 200;
        temp_a0->unk90[18] = 200;
        temp_a0->unk90[17] = 200;
        temp_a0->unk90[11] = 200;
        temp_a0->unk90[10] = 200;
        temp_a0->unk90[8] = 200;
        temp_a0->unk90[6] = 200;
        temp_a0->unk90[5] = 200;
        temp_a0->unk90[4] = 200;
        temp_a0->unk90[3] = 200;
        temp_a0->unk90[2] = 200;
        temp_a0->unk90[1] = 200;
        temp_a0->unk90[0] = 200;
    }
    
    func_800A240C(idx, D_800ED148.entities[idx].unk28, &D_800ED148.entities[idx].status);
    if (D_800ED148.entities[0].state.word == 4) {
        if (D_800ED148.entities[idx].status & 0x100) {
            func_8009B088(idx, 0, 8, 1);
        }
        
        else {
            func_8009B088(idx, 0, 8, 0);
        }
    }
}

void func_800A7518(s32 arg0) {
    BattleCharData* temp_s1;
    BattleEntityData* temp_s0;
    s32 i;

    temp_s1 = &g_battleChars.chars[arg0];
    temp_s0 = (BattleEntityData*)&D_800ED148.entities[arg0].entityData;
    
    if (temp_s1->characterId == 255) {
        temp_s0->unk7C = 0;
        temp_s0->unkBB = 255;
        return;
    }
    
    temp_s0->unkBB = temp_s1->characterId;
    temp_s0->unk80 = temp_s1->displayStatus;
    temp_s0->unk7C = 34817;

    if (D_80078E00.array35BD[temp_s1->classId].unk6 & 1) {
        temp_s0->unk7C |= 0x1000;
    }

    if (D_80078E00.array37A6[temp_s0->unkBB].unk1 & 1) {
        temp_s0->unk7C |= 0x100;
    }

    temp_s0->unk8 = 0;
    if (temp_s1->statusFlags & 0x1000) {
        temp_s0->unk8 = 0x80;
    }

    if (temp_s1->statusFlags & 0x4000) {
        temp_s0->unk8 |= 0x20;
    }

    if (temp_s1->statusFlags & 0x2000) {
        temp_s0->unk8 |= 0x40;
    }

    if (temp_s1->statusFlags & 0x8000) {
        temp_s0->unk8 |= 2;
    }
    
    func_800A7188(temp_s0);
    func_800A71A0(temp_s0);
    func_800A554C(arg0);

    if (!(temp_s0->unk80 & 4) && !(temp_s0->unk80 & 1)) {
        if (temp_s1->statusFlags & 0x10000) {
            D_800ED148.entities[arg0].unk24 = D_800ED148.entities[arg0].unk20;
        }
        
        else {
            func_800A559C(arg0);
        }
    }


    for (i = 0; i < 40; i++) {
        temp_s0->unk90[i] = 100;
    }
} 

/**
 * @brief Test a bit in the g_gameState bitfield at offset 0xD04.
 *
 * If a0 is nonzero, computes bit position (a0 - 1), divides by 32 to
 * find the word index and remainder, then tests that bit in the bitfield
 * at g_gameState + word_index * 4 + 0xD04.
 *
 * @param a0 1-based bit position to test. If 0, returns undefined.
 * @return 1 if the bit is set, 0 if clear.
 */
s32 func_800A774C(s32 idx) {
    if (idx != 0) {
        s32 val = idx - 1;
        s32 idx = val / 32;
        s32 bit = val - idx * 32;
        
        return (g_gameState.mainData.array210[idx] & (1 << bit))? 1 : 0;
    }
}

/**
 * @brief Set a bit in the g_gameState bitfield at offset 0xD04.
 *
 * If a0 is nonzero, computes bit position (a0 - 1), divides by 32 to
 * find the word index and remainder, then sets that bit in the bitfield
 * at g_gameState + word_index * 4 + 0xD04.
 *
 * @param a0 1-based bit position to set. If 0, does nothing.
 */
void func_800A779C(s32 arg0) {
    s32 idx;
    s32 bit;
    s32 temp_v0;
    
    temp_v0 = arg0 - 1;
    if (arg0 != 0) {
        idx = temp_v0 / 32;
        bit = temp_v0 - (idx * 32);

        g_gameState.mainData.array210[idx] |= 1 << bit;
    }
}

void func_800A77E8(void) {
    s32 j;
    s32 i;
    u8 idx;
    
    for (i = 0; i < 3; i++) {
        idx = g_gameState.mainData.party.party[i]; // uses party.party as the index for chars
        if (idx != 255) { // not party member 3
            for (j = 0; j < 32; j++) {
                func_800A779C(g_gameState.chars[idx].magic[j].magicId);
            }
        }
    }
}

void func_800A7884(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        g_battleChars.chars[i].unk180 = 0;
        g_battleChars.chars[i].unk184 = 0;
    }

    func_800A77E8();
    D_80078DF8 = 0;

    for (i = 0; i < 3; i++) {
        func_80022E08(g_gameState.mainData.party.party[i], i);
        func_800231E0(g_gameState.mainData.party.party[i], i);
        func_800A7518(i);
        func_800A71C0(i);
    }
    
    recalcAllGfStats();
}

void func_800A7934(void) {
    s32 i;
    
    for (i = 0; i < 3; i++) {
        GameState* gs = &g_gameState;
        if (D_800ED148.entities[i].linkedIdx != 255) {
            CharacterData* character = gs->chars;
            character[g_gameState.mainData.party.party[i]].currentHp = D_800ED148.entities[i].unk28;
        }
    }
}

void func_800A79A0(void) {
    s32 i;

    func_800A7934();
    func_800AF7C4();
    func_800A77E8();
    func_800A8794();
    func_800D8A94();
    
    if (D_800ED148.unk1318 != 0) {
        func_800D8A78();
        D_800ED148.unk1318 = 0;
    }

    for (i = 0; i < 3; i++) {
        func_800231E0(g_gameState.mainData.party.party[i], i);
        func_800A71C0(i);
    }
}

/**
 * @brief Look up the per-class @c field09 byte for the entity at @p a0.
 *
 * Reads @c BattleCharData[a0].classId, then returns
 * @c g_gfData.levelCurve12[classId].field09 (offset @c 0x35C1 in the
 * shared @c D_80078E00 / @c g_gfData block).
 *
 * @param a0 Entity index (stride 0x1D0 in @c g_battleChars).
 * @return The @c field09 byte for that entity's class.
 */
u8 func_800A7A44(s32 arg0) {
    return D_80078E00.array35BD[g_battleChars.chars[arg0].classId].unk4;
}

/**
 * @brief Look up a byte attribute from g_battleChars entity table (stride 0x1D0).
 *
 * @param idx Entity index.
 * @return Byte at offset 0x1B9 within the entity entry.
 */
u8 func_800A7A8C(s32 arg0) {
    return g_battleChars.chars[arg0].unk1B9;
}

/**
 * @brief Get a byte field from an entity's nested pointer chain.
 *
 * Looks up D_800ED148[a0] (stride 208), follows the pointer at +0x10,
 * dereferences it, then returns the byte at offset +0xF6.
 *
 * @param a0 Entity index.
 * @return Byte value at the end of the pointer chain.
 */
u8 func_800A7AB8(s32 arg0) {
    return (*D_800ED148.entities[arg0].entityData)->unkF6;
}

/**
 * @brief Traverse a multi-level pointer chain from entity data.
 *
 * Indexes into D_800ED148 by a0*208, follows pointers at offsets
 * 0x14, 0x0, 0x4 (added back), 0xC (added back), and returns
 * the first byte at the final address.
 *
 * @param a0 Entity index (stride 208).
 * @return Byte value at the end of the pointer chain.
 */
s32 func_800A7AF4(s32 a0) {
    s32 base = (s32)&D_800ED148;
    s32 ptr;
    s32 p2;
    ptr = *(s32 *)(base + a0 * 208 + 0x14);
    p2 = *(s32 *)ptr;
    ptr = *(s32 *)(p2 + 4);
    ptr += p2;
    p2 = *(s32 *)(ptr + 0xC);
    ptr += p2;
    return *(u8 *)ptr;
}

void func_800A7B48(void) {
    s32 temp_s1;
    s32 i;
    s32 j;

    for (j = 3, i = 0; i < 8; i++) {
        if (D_800ED148.unkD5C[i] != 0) {
            func_800A7FD0(j, D_800ED148.unkD54[i], D_800ED148.unkD14[i]);
            func_800A8430(j);
            
            temp_s1 = 7 - i;
            if (func_8009A514(D_800ED148.unkCE0, temp_s1) != 0) {
                D_800ED148.entities[j].controlFlags |= 2;
            }
            
            if (func_8009A514(D_800ED148.unkCE2, temp_s1) != 0) {
                D_800ED148.entities[j].controlFlags |= 0x40;
            }
            
            if (func_8009A514(D_800ED148.unkCE1, temp_s1) != 0) {
                D_800ED148.entities[j].controlFlags |= 0x80;
            }
            
        
            j++;
        }
    }
    
    func_800A8948();
    func_800A8794();
}

void func_800A7C64(s32 arg0, s32 arg1) {
    BattleEntityData* temp_v1;
    u8 result;

    temp_v1 = *D_800ED148.entities[arg0].entityData;
    result = 0;
    
    if (arg1 >= temp_v1->unkF4) {
        result = 2;
        if (arg1 < temp_v1->unkF5) {
            result = 1;
        }
    }
    
    D_800EE9E8.subEntries[arg0 - 3].unk46 = result;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object4", func_800A7CEC);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object4", func_800A7D8C);

u8 func_800A7EE0(BattleEntityData* arg0, s32 arg1) {
    switch (arg1) {
        case 0 ... 6:
            return arg0->unk168[arg1];

        case 8:
            return arg0->unk168[7];

        case 9:
            return arg0->unk168[8];

        case 10:
            return arg0->unk168[9];

        case 11:
            return arg0->unk168[10];

        case 12:
            return arg0->unk168[11];

        case 15:
            return arg0->unk168[12];

        case 18:
            return arg0->unk168[13];

        case 20:
            return arg0->unk168[14];
            
        case 21:
            return arg0->unk168[15];
            
        case 22:
            return arg0->unk168[16];
            
        case 23:
            return arg0->unk168[17];
            
        case 24:
            return arg0->unk168[18];
            
        default:
            return 100;
    }
}
 
s32 func_800A7FB4(BattleEntityData* arg0, s32 arg1) {
    return arg0->unk160[arg1] * 10;
}

void func_800A7FD0(s32 arg0, s32 arg1, s32 arg2) {
    s32 temp_v0;
    s32 i;
    BattleEntityData* temp_s1;
    BattleEntityData* temp_s3;


    temp_s3 = *D_800ED148.entities[arg0].entityData;
    temp_s1 = (BattleEntityData*)&D_800ED148.entities[arg0].entityData;
    D_800EE9E8.subEntries[arg0 - 3].unk10 = 0;
    temp_s1->unkBC = func_800A7154(arg1);
    temp_s1->unk7C = 17;
    if (temp_s3->immunityFlags & 0x10) {
        temp_s1->unk7C |= 0x2000;
    }
    
    if (temp_s3->immunityFlags & 8) {
        temp_s1->unk7C |= 0x8000;
    }
    
    if (temp_s3->unkFE & 0x40) {
        temp_s1->unk7C |= 0x10000;
    }
    
    if (func_800A7AF4(arg0) != 0) {
        temp_s1->unk7C |= 0x20;
    }
    
    temp_s1->unkBB = arg2;
    temp_v0 = func_800A7CEC(temp_s1->unkBC, temp_s3);
    temp_s1->unk18 = temp_v0;
    temp_s1->unk1C = temp_v0;
    temp_s1->unkC4 = 0;
    temp_s1->unkC2 = 0;
    temp_s1->unk80 = 0;
    temp_s1->unk8 = 0;
    temp_s1->unkC6 = 100;
    for (i = 0; i < 8; i++) {
        temp_s1->unk44[i] = func_800A7FB4(temp_s3, i);
    }
    
    for (i = 0; i < 40; i++) {
        temp_s1->unk90[i] = func_800A7EE0(temp_s3, i);
    }
    
    if (temp_s3->immunityFlags & 1) {
        temp_s1->unk90[6] = 255;
        temp_s1->unk80 |= 0x40;
    }
    
    if (temp_s3->immunityFlags & 2) {
        temp_s1->unk8 |= 0x2000;
    }
    
    if (temp_s3->immunityFlags & 0x20) {
        temp_s1->unk8 |= 0x80;
    }
    
    if (temp_s3->immunityFlags & 0x80) {
        temp_s1->unk8 |= 0x20;
    }
    
    if (temp_s3->immunityFlags & 0x40) {
        temp_s1->unk8 |= 0x40;
    }
    
    for (i = 0; i < 6; i++) {
        D_800EE9E8.subEntries[arg0 - 3].unk40[i] = 10;
    }
    
    func_800A7188(temp_s1);
    func_800A71A0(temp_s1);
    func_800A554C(arg0);
    func_800A559C(arg0);
}

s32 func_800A82A0(BattleAnimSubEntry* arg0, BattleEntityData* arg1, BattleEntityData* arg2, s32 arg3) {
    s32 result;

    result = (arg0->unk40[arg3] * func_800A7D8C(arg2->unkBC, arg1, arg3)) / 10;

    if (result > 255) {
        result = 255;
    }
    
    return result;
}

void func_800A8320(s32 arg0) {
    BattleEntityData* temp_s2;
    BattleEntityData* temp_s0;
    BattleAnimSubEntry* temp_s1;

    temp_s1 = &D_800EE9E8.subEntries[arg0 - 3];
    temp_s2 = *D_800ED148.entities[arg0].entityData;
    temp_s0 =  (BattleEntityData*)&D_800ED148.entities[arg0].entityData;
    
    temp_s0->unkBD = func_800A82A0(temp_s1, temp_s2, temp_s0, 0);
    temp_s0->unkBE = func_800A82A0(temp_s1, temp_s2, temp_s0, 1);
    temp_s0->unkBF = func_800A82A0(temp_s1, temp_s2, temp_s0, 2);
    temp_s0->unkC0 = func_800A82A0(temp_s1, temp_s2, temp_s0, 3);
    temp_s0->unkC1 = func_800A82A0(temp_s1, temp_s2, temp_s0, 4);
    temp_s0->unkC3 = func_800A82A0(temp_s1, temp_s2, temp_s0, 5);
}

void func_800A8430(s32 arg0) {
    BattleEntityData* temp_s0;
    BattleEntityData* temp_s1;
    s32 result;
    
    temp_s0 = *D_800ED148.entities[arg0].entityData;
    temp_s1 = (BattleEntityData*)&D_800ED148.entities[arg0].entityData;
    
    func_800A7C64(arg0, temp_s1->unkBC);
    result = func_800A7CEC(temp_s1->unkBC, temp_s0);
    temp_s1->unk1C = result;
    
    if (result < temp_s1->unk18) {
        temp_s1->unk18 = result;
    }

    func_800A8320(arg0);
}

void func_800A84CC(s32 arg0) {
    s32 i;
    BattleEntityData* temp_v0 = (BattleEntityData*)&D_800ED148.entities[arg0].entityData;
    
    temp_v0->unkBB  = 255;
    temp_v0->unk7C  = 0;
    temp_v0->unkB9  = 0;
    temp_v0->unkB8  = 0;
    temp_v0->unk84  = 0;
    temp_v0->unkCA  = 0;
    temp_v0->unkCC  = 0;
    temp_v0->unk80 |= 1;
    
    for (i = 0; i < 16; i++) {
        temp_v0->unk54[i] = -1111;
    }
}

void func_800A853C(void) {
    s32 i;
    
    for (i = 0; i < 7; i++) {
        func_800A84CC(i);
    }
}

void func_800A8578(void) {
    s32 i;

    for (i = 0; i < 32; i++) {
        D_800EE9E8.animSlots[i].unk4 = 0;
        
        if (D_80078E00.abilities[D_800EE9E8.animSlots[i].id].unk2 & 0x80) {
            D_800EE9E8.animSlots[i].unk4 = 1;
        }
        
        if (!(D_80078E00.abilities[D_800EE9E8.animSlots[i].id].unk2 & 0x20)) {
            D_800EE9E8.animSlots[i].unk4 |= 2;
        }

        D_800EE9E8.animSlots[i].unk3 = D_80078E00.abilities[D_800EE9E8.animSlots[i].id].abilityId;
        D_800EE9E8.animSlots[i].unk2 = D_80078E00.abilities[D_800EE9E8.animSlots[i].id].unk0;
    }
}

void func_800A864C(void) {
    BattleAnimSlot* temp_v0;
    s32 i;
    s32 idx;

    for (i = 0; i < 32; i++) {
        D_800EE9E8.animSlots[i].value = 0;
        D_800EE9E8.animSlots[i].id    = 0;
    }

    for (i = 0; i < 198; i++) {
        idx = g_gameState.mainData.itemSlots[i].id;
        
        if ((idx != 0) && (idx < 33)) {
            temp_v0 = &D_800EE9E8.animSlots[g_gameState.mainData.limitBreaks.angeloPoints[idx+7]]; // not much sense, size 8
            temp_v0->value = g_gameState.mainData.itemSlots[i].count;
            temp_v0->id = idx;
        }
    }
    
    func_800A8578();
}

void func_800A86F0(s32 arg0) {
    s32 i;

    func_8009B208(&D_800ED148.unkD64[arg0][0], &D_800ED148.unk1100[arg0], 11);
    
    for (i = 0; i < 11; i++) {
        func_800A5948(i, arg0);
    }

    for (i = 0; i < 3; i++) {
        D_800ED148.unk1244[i].unk0[2].unk7 = 0;
        D_800ED148.unk1244[i].unk0[1].unk7 = 0;
        D_800ED148.unk1244[i].unk0[0].unk7 = 0;
    }
}

void func_800A8794(void) {
    BattleEntityData* temp_s2;
    s32 j;
    s32 i;
    s32 idx; 
    drawSlot* arr0;
    
    for (i = 0; i < 4; i++) {   
        if (D_800ED148.entities[i + 3].controlFlags & 1) {
            temp_s2 = *D_800ED148.entities[i + 3].entityData;
            arr0 = D_800EE9E8.subEntries[i].array0;

            for (j = 0; j < 4; j++) {
                idx = temp_s2->unk104[D_800EE9E8.subEntries[i].unk46].unk0[j].unk0;
                if (idx < 64) {
                    arr0[j].unk0 = idx;
                    arr0[j].unk2 = 0;
                    
                    if (func_800A774C(arr0[j].unk0) == 0) {
                        arr0[j].unk1 |= 8;
                    }
                        
                    else {
                        arr0[j].unk1 &= 0xF7;
                    }
                }
                    
                else {
                    arr0[j].unk2 = 0;
                    if (!(g_gameState.gfs[idx - 64].exists & 1)) {
                        arr0[j].unk0 = idx;
                    }
                        
                    else {
                        arr0[j].unk0 = 0;
                    }
                }  
            }
        }
    }
}

void func_800A890C(s32 arg0) {
    s32 i;
    drawSlot* var_v0 = D_800EE9E8.subEntries[arg0 - 3].array0;

    for (i = 0; i < 4; i++) {
        var_v0[i].unk1 = 0;
    }
}

void func_800A8948(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        if (D_800ED148.entities[i+3].controlFlags & 1) {
            func_800A890C(i + 3);
        }
    }
}

s32 func_800A89B8(s32 arg0, s32 arg1) {
    BattleCharData* temp_a0;
    s32 i;
    
    temp_a0 = &g_battleChars.chars[arg0];
  
    for (i = 0; i < 32; i++) {
        if (temp_a0->magicSlots[i].unk0 == arg1) {
            return temp_a0->magicSlots[i].unk1 == 100;
        }
    }
  
    for (i = 0; i < 32; i++) {
        if (temp_a0->magicSlots[i].unk0 == 0) {
            return 0;
        }
    }
    
    return 1;
}

void func_800A8A48(BattleCharData* arg0, s32 arg1, u8 arg2, s32 arg3) {
    arg0->unkSlots[arg1].unk0 = arg2;
    arg0->unkSlots[arg1].unk2 = D_80078E00.unkE8[arg0->unkSlots[arg1].unk0].unk2;
    
    if (arg3 == 0) {
        arg0->unkSlots[arg1].unk1 = arg0->cmdSlots[findCommandSlot(arg0, 13)].unk1;
        arg0->unkSlots[arg1].unk3 = arg0->cmdSlots[findCommandSlot(arg0, 13)].unk3;
    }
    
    else {
        arg0->unkSlots[arg1].unk1 = arg0->cmdSlots[findCommandSlot(arg0, 2)].unk1;
        arg0->unkSlots[arg1].unk3 = arg0->cmdSlots[findCommandSlot(arg0, 2)].unk3;
    }
}

s32 func_800A8AFC(s32 arg0) {
    BattleCharData* ptr = &g_battleChars.chars[arg0];

    func_800A8A48(ptr, 0, 35, 0);
    func_800A8A48(ptr, 1, 36, 0);
    func_800A8A48(ptr, 2, 0, 0);
    
    return 2;
}

#include "battle/bc_object7.h"

s32 func_800A8B7C(s32 arg0) {
    BattleCharData* temp_s0;

    temp_s0 = &g_battleChars.chars[arg0];
    func_800A8A48(temp_s0, 2, 0, 1);

    switch (temp_s0->unk188 & 0x60000) {
        case 0:
            return 0;
        
        case 0x60000:
            func_800A8A48(temp_s0, 0, 0x23, 1);
            func_800A8A48(temp_s0, 1, 0x24, 1);
            func_800A8A48(temp_s0, 2, 0x25, 1);
            return 3;
            
        case 0x20000:
            func_800A8A48(temp_s0, 0, 0x23, 1);
            func_800A8A48(temp_s0, 1, 0x24, 1);
            return 2;
            
        case 0x40000:
            func_800A8A48(temp_s0, 0, 0x23, 1);
            func_800A8A48(temp_s0, 1, 0x25, 1);
            return 2;
    }
}

void func_800A8CA4(s32 characterId, s32 targetId) {
    BattleAnimSubEntry* var_s0;
    s32 i;

    if (D_800ED148.entities[targetId].controlFlags & 1) {
        for (i = 0; i < 4; i++) {
            var_s0 = &D_800EE9E8.subEntries[targetId - 3];
            if (var_s0->array0[i].unk0 < 64) {
                if (hasJunctionedAbility(characterId, var_s0->array0[i].unk0) != 0) {
                    var_s0->array0[i].unk1 |= 4;
                } 
                
                else {
                    var_s0->array0[i].unk1 &= 0xFB;
                }
            }
        }
    }
}

void func_800A8D7C(s32 arg0, s32 arg1) {
    BattleCharData* temp_s0;

    temp_s0 = &g_battleChars.chars[arg0];
    temp_s0->pad0[0] = 10;
    temp_s0->pad0[1] = D_80078E00.unk0139;
    temp_s0->pad0[2] = D_80078E00.unk013A;
    temp_s0->pad0[3] = 0;
    if (arg1 < 64) {
        temp_s0->pad0[4] = 9;
        temp_s0->pad0[5] = D_80078E00.spells[arg1].unk5;
        temp_s0->pad0[6] = D_80078E00.spells[arg1].magicId;
        temp_s0->pad0[7] = 0;
        
        if (D_80078E00.spells[arg1].unk7 & 0x80) {
            temp_s0->pad0[7] = 1;
            
            if (D_80078E00.spells[arg1].unk7 & 0x80) {
                temp_s0->pad0[3] |= 1;
            }
        }
        
        if (func_800A89B8(arg0, arg1) != 0) {
            temp_s0->pad0[3] |= 2;
        }
    } 
    
    else {
        temp_s0->pad0[4] = 9;
        temp_s0->pad0[5] = 0;
        temp_s0->pad0[6] = 0;
        temp_s0->pad0[7] = 2;
    }
}

void func_800A8E90(BattleCharData* arg0, s32 arg1) {
    arg0->testSlots[0].unk0 = arg1;
    arg0->testSlots[0].unk2 = D_80078E00.array4484[arg1].unk5;
    arg0->testSlots[0].unk3 = D_80078E00.array4484[arg1].unk6;
    arg0->testSlots[0].unk4 = 0;
    arg0->testSlots[0].unk1 = 1;
    
    if (D_80078E00.array4484[arg1].unk7 & 0x80) {
        arg0->testSlots[0].unk4 |= 1;
    }
}

s32 func_800A8EFC(BattleCharData* arg0) {
    s32 bit;
    s32 i;
    s32 val;

    val = 0; 
    bit = 1; 

    for (i = 0; i < 16; i++) {
        if (g_gameState.mainData.limitBreaks.quistisLimits & bit) {
            arg0->testSlots[val].unk0 = i;
            arg0->testSlots[val].unk2 = D_80078E00.array44FC[i].unk4;
            arg0->testSlots[val].unk3 = D_80078E00.array44FC[i].unk5;
            arg0->testSlots[val].unk4 = 0;
            arg0->testSlots[val].unk1 = 1;
            arg0->testSlots[val].unk4 &= 0xEF;
            
            if (D_80078E00.array44FC[i].unk6 & 0x80) {
                arg0->testSlots[val].unk4 |= 1;
            }
            
            val++;
        }

        bit <<= 1;
    }
    
    return val;
}

void func_800A8F98(BattleCharData* arg0, s32 arg1, s32 arg2) {
    arg0->testSlots[arg2].unk0 = arg1;
    arg0->testSlots[arg2].unk2 = D_80078E00.rows8[arg1].unk2;
    arg0->testSlots[arg2].unk3 = D_80078E00.rows8[arg1].unk3;
    arg0->testSlots[arg2].unk4 = 0;
    arg0->testSlots[arg2].unk1 = 1;
}

s32 func_800A8FDC(BattleCharData* arg0) {
    s32 current;
    s32 val = 0;

    if (!(g_gameState.mainData.partyLockFlag & 0x10)) {
        val++;
        func_800A8F98(arg0, 0, 0);
    }
    
    current = val;
    if (g_gameState.mainData.partyLockFlag & 0x20) {
        val++;
        func_800A8F98(arg0, 1, current);
    }
    
    return val;
}

s32 func_800A9064(BattleCharData* arg0) {
    arg0->testSlots[0].unk0 = 65;
    arg0->testSlots[1].unk0 = 67;
    arg0->testSlots[2].unk0 = 66;
    
    return 3;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object4", func_800A9084);

s32 func_800A9240(s32 arg0) {
    s32 i;
    
    for (i = 0; i < 198; i++) {
        if (g_gameState.mainData.itemSlots[i].id == arg0) {
            return g_gameState.mainData.itemSlots[i].count;
        }
    }
    
    return 0;
}

s32 func_800A9284(BattleCharData* arg0) {
    s32 i;
    u16 mask;
    s32 val;
    s32 temp_a0;
    s32 var_s1;

    val = 0;
    mask = 1;
    var_s1 = 101;
        
    for (i = 0; i < 8; i++) {
        temp_a0 = func_800A9240(var_s1);
        if ((g_gameState.mainData.limitBreaks.irvineLimits & mask) || (temp_a0 != 0)) {
            arg0->testSlots[val].unk0 = var_s1;
            arg0->testSlots[val].unk2 = 0x80;
            arg0->testSlots[val].unk3 = D_80078E00.unk015A;
            arg0->testSlots[val].unk4 = 0;
            arg0->testSlots[val].unk1 = temp_a0;
            arg0->testSlots[val].unk4 &= 0xEF;
            
            if (temp_a0 == 0) {
                arg0->testSlots[val].unk4 |= 2;
            }
          
            val++;
        }
        
        var_s1++;
        mask <<= 1;
    }
    
    return val;
}

s32 func_800A9370(s32 arg0) {
    BattleCharData* temp_a0;
    s32 i;

    temp_a0 = &g_battleChars.chars[arg0];
    
    for (i = 0; i < 16; i++) {
        temp_a0->testSlots[i].unk4 = 0;
        temp_a0->testSlots[i].unk1 = 0;
        temp_a0->testSlots[i].unk3 = 0;
        temp_a0->testSlots[i].unk2 = 0;
        temp_a0->testSlots[i].unk0 = 0;
        temp_a0->testSlots[i].unk4 |= 0x10;
    }
    
    switch (temp_a0->characterId) {
        case 0:
        case 1:
            return 0;
            
        case 2:
            return func_800A9284(temp_a0);
            
        case 3:
            return func_800A8EFC(temp_a0);
            
        case 4:
            return func_800A8FDC(temp_a0);
            
        case 5:
            return func_800A9064(temp_a0);
            
        case 6:
            func_800A8E90(temp_a0, 0);
            return 1;
            
        case 7:         
            func_800A8E90(temp_a0, 1);
            return 1;
            
        case 8:         
            func_800A8E90(temp_a0, 2);
            return 1;
            
        case 9:   
            func_800A8E90(temp_a0, 3);
            return 1;
            
        case 10:
            func_800A8E90(temp_a0, 4);
            return 1;
    }
}

void func_800A9490(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        g_battleChars.unk57A[i] = 0;
        g_battleChars.unk574[i] = 0;
    }

    for (i = 0; i < 16; i++) {
        g_battleChars.unk5A0[i] = 0;
        g_battleChars.unk580[i] = 0;
    }
}

void func_800A94E0(void) {
    s32 i;

    func_800A9490();
    
    for (i = 0; i < 16; i++) {
        g_battleChars.unk5C0[i] = 0;
    }

    for (i = 0; i < 24; i++) {
        g_battleChars.unk5E0[i].unk1 = 0;
        g_battleChars.unk5E0[i].unk0 = 0;
    }



    for (i = 0; i < 8; i++) {
        g_battleChars.gfEntries[0].unk0[i] = 255;
    }
}

s32 func_800A9568(s32 arg0) {
    s32 i;

    for (i = 0; i < 32; i++) {
        if (D_800EE9E8.animSlots[i].id == arg0) {
            return 1; 
        }
    }
    
    return 0;
}

void func_800A95A0(s32 arg0, s32 arg1) {
    func_800B0754(arg0, 4, arg1, func_800A97FC(arg0));
    func_800AF4BC(arg1, 1);
    
    if (func_800AE390(arg1) == 0) {
        func_800AE3D4(arg1);
    }
}

void func_800A960C(s32 arg0) {
    s32 result;

    if (D_800ED148.entities[arg0].status & 0x200) {
        result = D_800ED148.entities[arg0].unk2C - D_800ED148.entities[arg0].unk28;
        if (result <= 200) {
            return;
        }
        
        if (result > 1000) {
            if (func_800A9568(3) != 0) {
                func_800A95A0(arg0, 3);
            } 
            
            else if (func_800A9568(1) != 0) {
                func_800A95A0(arg0, 1);
            } 
            
            else if (func_800A9568(2) != 0) {
                func_800A95A0(arg0, 2);
            } 
            
            else if (func_800A9568(4) != 0) {
                func_800A95A0(arg0, 4);
            } 
            
            else if (func_800A9568(5) != 0) {
                func_800A95A0(arg0, 5);
            } 
            
            else if (func_800A9568(9) != 0) {
                func_800A95A0(arg0, 9);
            }
        }
            
        else {
            if (func_800A9568(1) != 0) {
                func_800A95A0(arg0, 1);
            }
        }
    }
}


s32 func_800A972C(s32 arg0) {
    return func_800B0F9C(D_80078E00.entriesA0[arg0].unk7) | func_800B0F7C(D_80078E00.entriesA0[arg0].unk7);
}

u8* func_800A9784(u16 arg0, s32 arg1) {
    if (arg0 == 65535) {
        return D_800E3CEC;
    }
    
    return arg0 + arg1; // weird
}

s32 func_800A97A4(s32 arg0) {
    return func_8009B15C() % arg0;
}

void func_800A97D4(void) {
    s32 i;
    
    for (i = 0; i < 8; i++) {
        D_800ED148.unk1298[i] = 0;
    }
}

s32 func_800A97FC(s32 arg0) {
    return (u16)(1 << arg0); // functions in this folder break with u16 return prototype
}

u16 func_800A980C(void) {
    s32 idx;

    if (func_800AE730() == 255) {
        return 0;
    }

    do {
        idx = func_800A97A4(3);
    } while (D_800ED148.entities[idx].status & 1);
    
    return (1 << idx);
}

u16 func_800A9888(void) {
    s32 idx;

    if (func_800AE788() == 255) {
        return 8;
    }
    
    do {
        idx = func_800A97A4(4) + 3;
    } while (D_800ED148.entities[idx].status & 1);
    
    return (1 << idx);
}

u16 func_800A9904(s32 arg0) {
    return (1 << D_800ED148.entities[arg0].unk98);
}

void func_800A9938(void) {
    s32 i;

    for (i = 0; i < 7; i++) {
        D_800EEBE0[i] = ~D_800EEBE0[i] & 1;
    }
}

void func_800A9970(s32 arg0) {
    s32 i;

    for (i = 0; i < 7; i++) {
        if (D_800ED148.entities[i].controlFlags & 0x100) {
            D_800EEBE0[i] = 1;
        } 
        
        else {
            D_800EEBE0[i] = 0;
        }
    }
    
    if (arg0 == 200) {
        func_800A9938();
    }
}

void func_800A99E8(s32 arg0) {
    s32 i;

    for (i = 0; i < 7; i++) {
        if (arg0 < 16) {
            if ((D_800ED148.entities[i].status >> arg0) & 1) {
                D_800EEBE0[i] = 1;
            } 
            
            else {
                D_800EEBE0[i] = 0;
            }
        }
            
        else {
            s32 result = 1 << (arg0 - 16);
            if (D_800ED148.entities[i].flags & result) {
                D_800EEBE0[i] = 1;
            } 
            
            else {
                D_800EEBE0[i] = 0;
            }
        }
    }
}

void func_800A9A6C(s32 arg0) {
    switch (arg0) {
        case 200:
            D_800EEBD8 = 0;
            D_800EEBDC = 3;
            break;
        
        case 201:
            D_800EEBD8 = 3;
            D_800EEBDC = 7;
            break;
        
        default:
            D_800EEBD8 = 0;
            D_800EEBDC = 7;
            break;
    }
}

void func_800A9AC0(s32 arg0, s32 arg1) {
    s32 i;
    s32 val;
    s32 idx;

    func_800A9A6C(arg1);
    
    if (arg0 < 213) {
        val = 0;
        idx = arg0 - 0xCD;
        for (i = D_800EEBD8; i < D_800EEBDC; i++) {
            if (!(D_800ED148.entities[i].status & 1)) {
                u8* cd = D_800ED148.entities[i].unkCD;
                if (val < cd[idx]) {
                    val = cd[idx];
                }
            }
        }
    }
    
    else {
        val = 255;
        idx = arg0 - 0xD5;
        for (i = D_800EEBD8; i < D_800EEBDC; i++) {
            if (!(D_800ED148.entities[i].status & 1)) {
                u8* cd = D_800ED148.entities[i].unkCD;
                if (cd[idx] < val){
                    val = cd[idx];
                }
            }
        }
    }

    for (i = 0; i < 7; i++) {
        u8* cd = D_800ED148.entities[i].unkCD;
        if (val == cd[idx]) {
            D_800EEBE0[i] = 1;
        }
        
        else {
            D_800EEBE0[i] = 0;
        }
    }
}

void func_800A9C68(s32 arg0, s32 arg1) {
    s32 i;
    u16 val;
    s32 idx;

    func_800A9A6C(arg1);
    
    if (arg0 < 229) {
        val = 0;
        idx = arg0 - 221;
        for (i = D_800EEBD8; i < D_800EEBDC; i++) {
            if (!(D_800ED148.entities[i].status & 1)) {
                if (val < D_800ED148.entities[i].unk54[idx]) {
                    val = D_800ED148.entities[i].unk54[idx];
                }
            }
        }
    }
    
    else {
        val = 65535;
        idx = arg0 - 229;
        for (i = D_800EEBD8; i < D_800EEBDC; i++) {
            if (!(D_800ED148.entities[i].status & 1)) {
                if (D_800ED148.entities[i].unk54[idx] < val) {
                    val = D_800ED148.entities[i].unk54[idx];
                }
            }
        }       
    }


    for (i = 0; i < 7; i++) {
        if (val == D_800ED148.entities[i].unk54[idx]) {
            D_800EEBE0[i] = 1;
        }
        
        else {
            D_800EEBE0[i] = 0;
        }
    }
}

void func_800A9E08(s32 arg0, s32 arg1) {
    s32 i;
    s32 val;

    func_800A9A6C(arg1);
    
    if (arg0 == 203) {
        val = 0;
        for (i = D_800EEBD8; i < D_800EEBDC; i++) {
            if (!(D_800ED148.entities[i].status & 1)) {
                if (val < D_800ED148.entities[i].unk28) {
                    val = D_800ED148.entities[i].unk28;
                }
            }
        }
    }
    
    else {
        val = 0x7FFFFFFF;
        for (i = D_800EEBD8; i < D_800EEBDC; i++) {
            if (!(D_800ED148.entities[i].status & 1)) {
                if ((D_800ED148.entities[i].unk28 < val) && (D_800ED148.entities[i].unk28 != 0)) {
                    val = D_800ED148.entities[i].unk28;
                }
            }
        }
    }

    for (i = 0; i < 7; i++) {
        if (val == D_800ED148.entities[i].unk28) {
            D_800EEBE0[i]= 1;
        } 
        
        else {
             D_800EEBE0[i] = 0;
        }
    }
}

void func_800A9F98(void) {
    s32 i;
    
    for (i = 0; i < 7; i++) {
        if (!(D_800ED148.entities[i].controlFlags & 1)) {
            D_800EEBE0[i] = 0;
        }
    }
}

void func_800A9FDC(void) {
    s32 i;

    for (i = 0; i < 7; i++) {
        if (!(D_800ED148.entities[i].controlFlags & 1) || (D_800ED148.entities[i].status & 1)) {
            D_800EEBE0[i] = 0;
        }
    }
}

u16 func_800AA034(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 i;
    s32 result;
    s32 var_s0;

    var_s0 = 0;
    
    switch (arg3) {
        case 200: case 201:
            func_800A9970(arg3);
            break;
        
        case 0:
            var_s0 = 1;
            /* fallthrough */
        case 1  ... 9:
        case 16 ... 42:
        case 46 ... 47:
            func_800A99E8(arg3);
            break;
        
        case 205 ... 220:
            func_800A9AC0(arg3, arg1);
            break;

        case 221 ... 236:
            func_800A9C68(arg3, arg1);
            break;
        
        case 203 ... 204:
            func_800A9E08(arg3, arg1);
            break;
        
        default:
            break;
    }
    
    if (arg2 == 3) {
        func_800A9938();
    }
    
    if (var_s0 == 0) {
        func_800A9FDC();
    }
        
    else {
        func_800A9F98();
    }
    
    if (arg0 == 0) {
        switch (arg1) {
            case 200:
                do {
                    i = func_800A97A4(3);
                } while (D_800EEBE0[i] == 0);
                result = func_800A97FC(i);
                break;
            
            case 201:
                do {
                    i = func_800A97A4(4) + 3;
                } while (D_800EEBE0[i] == 0);
                result = func_800A97FC(i);
                break;
                
            default:
                do {
                    i = func_800A97A4(7);
                } while ((D_800EEBE0[i] == 0) || D_800ED148.entities[i].linkedIdx != arg1);
                result = func_800A97FC(i);
                break;
        }
    }
        
    else {
        result = 32768;
        switch (arg1) {
            case 200:
                for (i = 0; i < 3; i++) {
                    if (D_800EEBE0[i] != 0) {
                        result |= func_800A97FC(i);
                    }
                }
                break;
            
            case 201:
                for (i = 3; i < 7; i++) {
                    if (D_800EEBE0[i] != 0) {
                        result |= func_800A97FC(i);
                    }
                }
                break;
            
            default:
                for (i = 0; i < 7; i++) {
                    if ((D_800ED148.entities[i].linkedIdx == arg1) && (D_800EEBE0[i] != 0)) {
                        result |= func_800A97FC(i);
                    }
                }
                break;
        }
    }
    
    return result;
}

s32 func_800AA368(s32 arg0) {
    s32 i;
    s32 val;

    val = 0;
    switch(arg0) {
        case 200:
            for (i = 0; i < 3; i++) {
                if (!(D_800ED148.entities[i].status & 1)) {
                    val++;
                }
            }
            break;
            
        case 201:
            for (i = 3; i < 7; i++) {
                if (!(D_800ED148.entities[i].status & 1)) {
                    val++;
                }
            }
            break;
            
        default:
            for (i = 0; i < 7; i++) {
                if ((D_800ED148.entities[i].linkedIdx == arg0) && !(D_800ED148.entities[i].status & 1)) {
                    val++;
                }
            }   
            break;
    }
    
    return val;
}

u16 func_800AA44C(s32 arg0) {
    s32 bit;

    if (func_800AA368(201) == 1) {
        bit = arg0;
    }
    
    else {
        do {
            bit = func_800A97A4(4) + 3;
        } while (bit == arg0 || (D_800ED148.entities[bit].status & 1));
    }
    
    return (1 << bit);
}

s32 func_800AA4E0(void) {
    return 32775;
}

s32 func_800AA4E8(void) {
    return 33016;
}

s32 func_800AA4F0(void) {
    return 33023;
}

s32 func_800AA4F8(s32 arg0) {
    s32 i;

    for (i = 0; i < 7; i++) {
        if (D_800ED148.entities[i].linkedIdx == arg0) {
            return i;
        }
    }

    return 255;    
}

s32 func_800AA530(s32 arg0) {
    s32 i;

    for (i = 0; i < 7; i++) {
        if (!(D_800ED148.entities[i].status & 1) && ((D_800ED148.entities[i].linkedIdx == arg0))) {
            return i;
        }
    }
    
    return 255;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object4", func_800AA57C);