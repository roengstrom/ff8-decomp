#include "common.h"
#include "battle.h"
#include "gamestate.h"
#include "battle/bc_object5.h"


u16 func_800AA034(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 unused) {
    s32 i;
    s32 result;
    s32 val;

    val = 0;
    
    switch (arg3) {
        case 200 ... 201:
            func_800A9970(arg3);
            break;
        
        case 0:
            val = 1;
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
    
    if (val == 0) {
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
                } while ((D_800EEBE0[i] == 0) || D_800ED148.entities[i].comFileId != arg1);
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
                    if ((D_800ED148.entities[i].comFileId == arg1) && (D_800EEBE0[i] != 0)) {
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
                if ((D_800ED148.entities[i].comFileId == arg0) && !(D_800ED148.entities[i].status & 1)) {
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

/**
* @brief Search entity table for a matching byte at offset 0xCB.
*
* @param a0 Value to search for.
* @return Entity index (0-6) if found, or 0xFF if not found.
*/

s32 func_800AA4F8(s32 arg0) {
    s32 i;

    for (i = 0; i < 7; i++) {
        if (D_800ED148.entities[i].comFileId == arg0) {
            return i;
        }
    }

    return 255;    
}

/**
 * @brief Search entity table for active entity linked to index a0.
 *
 * @param a0 Link index to search for.
 * @return Entity index if found, 0xFF otherwise.
 */

s32 func_800AA530(s32 arg0) {
    s32 i;

    for (i = 0; i < 7; i++) {
        if (!(D_800ED148.entities[i].status & 1) && ((D_800ED148.entities[i].comFileId == arg0))) {
            return i;
        }
    }
    
    return 255;
}

BattleEntity* func_800AA57C(s32 arg0, s32 arg1) {
    switch (arg0) {
        case 203:
            return &D_800ED148.entities[D_800ED148.entities[arg1].unk88];
        
        case 200:
            return &D_800ED148.entities[arg1];

        case 220 ... 227:
            return &D_800ED148.entities[D_800ED148.entities[arg1].unk24[arg0 - 220]];

        case 0 ... 199:
        default:
            return &D_800ED148.entities[func_800AA4F8(arg0)];
    }
}

s32 func_800AA68C(s32 arg0, u32 arg1, u32 arg2) {
    switch (arg1) {
        case 0:
            if (arg0 == arg2) {
                return 1;
            }
            break;
            
        case 1:
            if (arg0 < arg2) {
                return 1;
            }
            break;
            
        case 2:
            if (arg2 < arg0) {
                return 1;
            }
            break;
            
        case 3:
            if (arg0 != arg2) {
                return 1;
            }
            break;
            
        case 4:
            if (arg2 >= arg0) {
                return 1;
            }
            break;
            
        case 5:
            if (arg0 >= arg2) {
                return 1;
            }
            break;
    }

    return 0;
}

/**
* @brief Conditionally search entity table and check for 0xFF result.
*
* If a0 is 0, returns whether func_800AA4F8(a1) is not 0xFF.
* If a0 is 3, returns whether func_800AA4F8(a1) is 0xFF.
*
* @param a0 Mode selector (0 = found check, 3 = not-found check).
* @param a1 Search value passed to func_800AA4F8.
* @return Boolean result based on mode.
*/

s32 func_800AA71C(s32 arg0, s32 arg1) {
    if (arg0 == 0) {
        return func_800AA4F8(arg1) != 255;
    }
    
    if (arg0 == 3) {
        return func_800AA4F8(arg1) == 255;
    }
}

/**
* @brief Conditionally search via func_800AA530 and check for 0xFF result.
*
* If a0 is 0, returns whether func_800AA530(a1) is not 0xFF.
* If a0 is 3, returns whether func_800AA530(a1) is 0xFF.
*
* @param a0 Mode selector (0 = found check, 3 = not-found check).
* @param a1 Search value passed to func_800AA530.
* @return Boolean result based on mode.
*/

s32 func_800AA768(s32 arg0, s32 arg1) {
    if (arg0 == 0) {
        return func_800AA530(arg1) != 255;
    }

    if (arg0 == 3) {
        return func_800AA530(arg1) == 255;
    }
}

s32 func_800AA7B4(s32 unused, s32 arg1) {
    s32 i;
    
    for (i = 0; i < 3; i++) {
        if ((D_800ED148.entities[i].controlFlags & 1) && !(D_800ED148.entities[i].status & 1)) {
            if (arg1 == 202) {
                if (!(D_800ED148.entities[i].controlFlags & CTRL_FEMALE)) {
                    return i;
                }
            }
                
            else {
                if ((D_800ED148.entities[i].controlFlags & CTRL_FEMALE)) {
                     return i;
                }
            }
        }
    }
    
    return 255;
}

/**
* @brief Conditionally search via func_800AA7B4 and check for 0xFF result.
*
* If a0 is 0, returns whether func_800AA7B4(a1) is not 0xFF.
* If a0 is 3, returns whether func_800AA7B4(a1) is 0xFF.
*
* @param a0 Mode selector (0 = found check, 3 = not-found check).
* @param a1 Search value passed to func_800AA7B4.
* @return Boolean result based on mode.
*/

s32 func_800AA840(s32 arg0, s32 arg1) {
    if (arg0 == 0) {
        return func_800AA7B4(arg1, arg1) != 255;
    }

    if (arg0 == 3) {
        return func_800AA7B4(arg1, arg1) == 255;
    }
}

s32 func_800AA88C(s32 arg0, s32 arg1) {
    if (arg0 == 0) {
        if ((D_800ED148.entities[arg1].controlFlags & 1) && !(D_800ED148.entities[arg1].status & 1)) {
            return 1;
        }
        
        return 0;
    }
    
    if (arg0 == 3) {
        if ((D_800ED148.entities[arg1].controlFlags & 1) && !(D_800ED148.entities[arg1].status & 1)) {
            return 0;
        }

        return 1;
    }
}

s32 func_800AA930(s32 arg0) {
    drawSlot* var_v1;
    s32 i;

    
    var_v1 = D_800EE9E8.subEntries[arg0 - 3].array0;
    for (i = 0; i < 4; i++) {
        if (var_v1[i].unk0 > 63) {
           return 1;
        }
    }
    
    return 0;
}

/**
* @brief Conditionally call func_800AA930 and optionally invert the result.
*
* If a0 is 0, calls func_800AA930 with a1 and returns its result.
* If a0 is 3, calls func_800AA930 with a1 and returns the inverted
* lowest bit of the result.
*
* @param a0 Mode selector (0 = normal, 3 = inverted).
* @param a1 Parameter passed to func_800AA930.
* @return Result of func_800AA930, possibly with bit 0 inverted.
*/

s32 func_800AA980(s32 arg0, s32 arg1) {
    if (arg0 == 0) {
        return func_800AA930(arg1);
    }

    if (arg0 == 3) {
        return (~func_800AA930(arg1)) & 1;
    }
}

/**
* @brief Test a specific bit in D_8007809A, optionally inverting.
*
* If a0 is 0, returns bit a1 of D_8007809A.
* If a0 is 3, returns the inverse of bit a1 of D_8007809A.
* Otherwise returns 0 (falls through without setting return value).
*
* @param a0 Mode selector (0 = normal, 3 = inverted).
* @param a1 Bit position to test.
* @return Bit value (0 or 1).
*/

s32 func_800AA9C8(s32 arg0, s32 arg1) {
    if (arg0 == 0) {
        return (g_gameState.mainData.partyLockFlag >> arg1) & 1;
    }

    if (arg0 == 3) {
        return ((g_gameState.mainData.partyLockFlag >> arg1) & 1) ^ 1;
    }
}

/**
 * @brief Test countdownTimer, optionally inverted.
 *
 * If a0 == 0, returns 1 when countdownTimer is zero (no battle active).
 * If a0 == 3, returns 1 when countdownTimer is nonzero (battle active).
 * Other values fall through with no explicit return.
 *
 * @param a0 Query mode (0 = test no-battle, 3 = test in-battle).
 * @return 1 or 0 based on the flag value.
 */

s32 func_800AAA10(s32 arg0) {
    if (arg0 == 0) {
        return g_gameState.mainData.countdownTimer == 0;
    }

    if (arg0 == 3) {
        return g_gameState.mainData.countdownTimer != 0;
    }
}

s32 func_800AAA50(s32 arg0, s32 arg1, s32 bit) {
    if (arg1 == 0) {
        s32 mask = (1 << bit);
        if (arg0 & mask) {
            return 1;
        }
    }

    if (arg1 == 3) {
        s32 mask = (1 << bit);
        if (!(arg0 & mask)) {
            return 1;
        }
    }

    return 0;
}

s32 func_800AAA9C(s32 arg0, u32 arg1, BattleEntity* arg2) {
    if (arg1 < 16) {
        if (arg0 == 0 && ((arg2->status >> arg1) & 1)) {
            return 1;
        }
        
        if (arg0 == 3 && !((arg2->status >> arg1) & 1)) {
            return 1;
        }
    }
    
    else if (arg0 == 0 && (arg2->flags & (1 << (arg1 - 16)))) {
        return 1;
    } 
    
    else if (arg0 == 3 && !(arg2->flags & (1 << (arg1 - 16)))) {
        return 1;  
    }
        
    
    return 0;
}

s32 func_800AAB50(s32 arg0, u32 arg1, BattleEntity* arg2) {
    switch (arg1) {
        case 1 ... 9:
            return func_800AA68C(arg2->currentHp, arg0, (arg2->maxHp / 10) * arg1);
        
        case 10:
            return func_800AA68C(arg2->currentHp, arg0, arg2->maxHp >> 2);        

        default:
            return func_800AA68C(arg2->currentHp, arg0, arg1);
    }
}

s32 func_800AABEC(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    if (arg4 != 0 && arg1 == 0) {
        if (D_800ED148.entities[arg3].controlFlags & 1) {
            BattleEntity* var_a2 = &D_800ED148.entities[arg3];
            arg2 += func_800AAA9C(arg0, arg1, var_a2);
        }
        
        return arg2;
    }

    else if (D_800ED148.entities[arg3].controlFlags & 1) {
        if (!(D_800ED148.entities[arg3].status & 1)) {
            BattleEntity* var_a2 = &D_800ED148.entities[arg3];
            if (arg4 == 0) {
                arg2 += func_800AAB50(arg0, arg1, var_a2);
            } 
            
            else {
                arg2 += func_800AAA9C(arg0, arg1, var_a2);
            }      
        }
    }
    
    return arg2;
}

s32 func_800AACD0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 result;
    s32 var_s2;
    s32 i;
    
    var_s2 = 0;
    if (arg3 == 0) {
        var_s2 = arg1;
    }
    
    result = 0;
    switch(arg0) {
        case 200:
            for (i = 0; i < 3; i++) {
                result = func_800AABEC(var_s2, arg2, result, i, arg3);
            }
            break;

        case 201:
            for (i = 3; i < 7; i++) {
                result = func_800AABEC(var_s2, arg2, result, i, arg3);
            }
            break;

        default:
            for (i = 0; i < 7; i++) {
                if (D_800ED148.entities[i].comFileId == arg0) {
                    result = func_800AABEC(var_s2, arg2, result, i, arg3);
                }
            }
            break;
    }
    
    if (arg3 == 0) {
        return result != 0;
    }
    
    if (result == 0) {
        return arg1 != 0;
    }
    
    return arg1 == 0;
}

s32 func_800AAE10(s32 arg0, s32 arg1, s32 arg2) {
    if (D_800ED148.entities[arg2].controlFlags & 1) {
        if ((D_800ED148.entities[arg2].status & 1)) {
            return arg1;  
        }
        
         arg1 += func_800AAA9C(0, arg0, &D_800ED148.entities[arg2]);
    }
    

    return arg1;
}

s32 func_800AAE98(s32 arg0) {
    s32 check1;
    s32 check2;
    s32 i;

    check1 = 0;
    check2 = 0;

    for (i = 0; i < 3; i++) {
        if ((D_800ED148.entities[i].controlFlags & 1) && !(D_800ED148.entities[i].status & 1)) {
            check2++;
        }
    }
    
    for (i = 0; i < 3; i++) {
        check1 = func_800AAE10(arg0, check1, i);

    }
   
    return check1 == check2;
}

/**
* @brief Allocate structure via func_800AA57C and copy field +0x1C to +0x18.
*
* @param a0 Second parameter to func_800AA57C (first is 0xC8).
*/

void func_800AAF48(s32 arg0) {
    BattleEntity* temp_v0;

    temp_v0 = func_800AA57C(200, arg0);
    temp_v0->currentHp = temp_v0->maxHp;
}

/**
* @brief Allocate entry and add signed offset to its field at 0x18.
*
* Calls func_800AA57C with 0xC8 and a0, then sign-extends a1 to 16 bits
* and adds it to the word at offset 0x18 of the returned entry.
*
* @param a0 Second parameter to func_800AA57C.
* @param a1 Signed 16-bit offset to add.
*/

void func_800AAF70(s32 arg0, s16 hpQuantity) {
    BattleEntity* temp_v0;

    temp_v0 = func_800AA57C(200, arg0);
    temp_v0->currentHp += hpQuantity;
}

/**
* @brief Look up entity entry by index, call animation init, set active flag.
*
* Computes D_800EE28C + a0 * 16 to get the entry, calls func_8009AF3C
* with entry[4], duration 0x1E, mode 3, size 0x80, and zero flag.
* Then sets entry[0xF] to 1.
*
* @param a0 Entity index (stride 16).
*/

void func_800AAFB8(s32 arg0) {
    TaskEntry* data;

    data = &D_800ED148.taskData[arg0];
    func_8009AF3C(data->unk4, 30, 3, 128, 0);
    data->done = 1;
}

void func_800AB008(s32 arg0) {
    s32 idx;
    
    TaskEntry* task = &D_800ED148.taskData[func_8009B3D0(&func_800AAFB8)];
    task->unk4 = func_800B0398(arg0);
}

void func_800AB054(s32 arg0) {
    TaskEntry* td = &D_800ED148.taskData[arg0];
    
    if (td->timer == 0) {
        func_8009AF3C(td->unk4, 30, 3, 240, 0);
        td->done = 1;
    }
    
    td->timer--;
}

/**
* @brief Allocate entry via func_8009B3D0 and store battle data.
*
* Allocates an entry using func_800AB054 as callback, computes the
* entry address from D_800EE28C + index * 16, stores the result of
* func_800B0398(a0) at offset 4, and a1 as halfword at offset 8.
*
* @param a0 Parameter passed to func_800B0398.
* @param a1 Halfword value stored at entry offset 8.
*/

void func_800AB0C0(s32 arg0, u16 arg1) {
    TaskEntry* currentEntry;

    currentEntry = &D_800ED148.taskData[func_8009B3D0(func_800AB054)];
    currentEntry->unk4 = func_800B0398(arg0);
    currentEntry->timer = arg1;
}


/**
* @brief Clear control flags and reset animation state for a battle entity.
*
* Clears CTRL_FLAG_40, CTRL_FLAG_80, and CTRL_FLAG_02 from the entity's
* controlFlags field, then calls func_800AE6C0 and func_800A59AC to
* reset animation state.
*
* @param idx Entity index (stride 0xD0 in D_800ED148).
*/

void func_800AB11C(s16 arg0) {
    D_800ED148.entities[arg0].controlFlags &= ~0x40;
    D_800ED148.entities[arg0].controlFlags &= ~0x80;
    D_800ED148.entities[arg0].controlFlags &= ~2;
    
    func_800AE6C0();
    func_800A59AC(arg0, 0, 0);
}

void func_800AB1AC(s32 arg0) {
    TaskEntry* temp_a0;

    temp_a0 = &D_800ED148.taskData[arg0];
    if (temp_a0->timer == 0) {
        D_800ED148.unk5C2 = 1;
        D_800ED148.unk12FD = 0;
        temp_a0->done = 1;
        return;
    }
    

    D_800ED148.unk5C2 = 0;
    D_800ED148.unk12FD = 1;
    temp_a0->timer--;
}

/**
* @brief Allocate entry via func_8009B3D0 and store value at offset 8.
*
* Calls func_8009B3D0 with func_800AB1AC as callback, shifts result
* left by 4 (multiply by 16), adds D_800EE28C as base, then stores a0
* as halfword at offset 8 of the computed entry.
*
* @param a0 Value to store as halfword.
*/

void func_800AB208(u16 arg0) {
    TaskEntry* currentEntry = &D_800ED148.taskData[func_8009B3D0(func_800AB1AC)];
    
    currentEntry->timer = arg0;
}

/**
* @brief Find the first active entity (bit 0 set at offset 0x8C).
*
* Scans entities 3-6 in the D_800ED148 array (offset 0x270, stride 0xD0).
* Returns the index of the first entity whose word at offset 0x8C has bit 0 set.
* Returns 0xFF if no active entity is found.
*
* @return Entity index (3-6), or 0xFF if none active.
*/

s32 func_800AB24C(void) {
    s32 i;

    for(i = 3; i < 7; i++) {
        if (!(D_800ED148.entities[i].controlFlags & 1)) {
            return i;
        }
    }
        
    return 255;
}

void func_800AB28C(void) {
    u8 entityIdx;
    u8 idx;

    idx = D_800ED148.unk12FC;
    entityIdx = D_800ED148.unk12FB;
    func_800A7FD0(entityIdx, D_800ED148.unkD54[idx], D_800ED148.unkD14[idx]);
    func_800A8430(entityIdx);
    

    
    if (func_8009A514(D_800ED148.unkCE0, 7 - idx) != 0) {
        D_800ED148.entities[entityIdx].controlFlags |= 2;
    }
    
    if (func_8009A514(D_800ED148.unkCE2, 7 - idx) != 0) {
        D_800ED148.entities[entityIdx].controlFlags |= 0x40;
    }
    
    if (func_8009A514(D_800ED148.unkCE1, 7 - idx) != 0) {
        D_800ED148.entities[entityIdx].controlFlags |= 0x80;
    }
    
    func_800A890C(entityIdx);
    func_800A8794();
    func_800D8A94();
    func_8009A528(entityIdx, idx);
}

/**
* @brief Set battle flag 0x5C2 and clear status bytes 0x12F9 and 0x12FD.
*/

void func_800AB3C4(void) {
    D_800ED148.unk5C2 = 1;
    D_800ED148.unk12F9 = 0;
    D_800ED148.unk12FD = 0;
}

/**
* @brief Clear battle flag 0x5C2 and set status bytes 0x12F9 and 0x12FD.
*/

void func_800AB3E0(void) {
    D_800ED148.unk5C2 = 0;
    D_800ED148.unk12F9 = 1;
    D_800ED148.unk12FD = 1;
}

/**
* @brief Resolve animation data and display it with position and timing.
*
* Looks up animation data from the entity's sub-object table at offset 0x14,
* resolves it through func_800A9784 and func_800B0398, then calls
* func_8009AF3C to display with the given Y position, fixed params.
*
* @param a0 Entity index (stride 0xD0 in D_800ED148).
* @param a1 Sub-animation index (multiplied by 2 for table lookup).
* @param a2 Y position for display.
*/

void func_800AB3FC(s32 arg0, s32 arg1, s32 arg2) {
    Unk4Struct* temp_v1;

    temp_v1 = *D_800ED148.entities[arg0].monsterAiSection;
    func_8009AF3C(func_800B0398(func_800A9784(*(arg1 + GET_OFFSET(u16, temp_v1, temp_v1->unk8)),GET_OFFSET(s32, temp_v1, temp_v1->unkC))), arg2, 3, 240, 0);
}


/**
* @brief Call func_800AB3FC with a fixed duration of 0x1E.
*
* @param a0 First parameter passed through.
* @param a1 Second parameter passed through.
*/

void func_800AB488(s32 arg0, s32 arg1) {
    func_800AB3FC(arg0, arg1, 30);
}