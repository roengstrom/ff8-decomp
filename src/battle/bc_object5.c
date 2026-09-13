#include "common.h"
#include "battle.h"
#include "gamestate.h"
#include "battle/bc_object5.h"

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

s32 func_800AA71C(s32 arg0, s32 arg1) {
    if (arg0 == 0) {
        return func_800AA4F8(arg1) != 255;
    }
    
    if (arg0 == 3) {
        return func_800AA4F8(arg1) == 255;
    }
}

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
                if (!(D_800ED148.entities[i].controlFlags & 0x100)) {
                    return i;
                }
            }
                
            else {
                if ((D_800ED148.entities[i].controlFlags & 0x100)) {
                     return i;
                }
            }
        }
    }
    
    return 255;
}

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

s32 func_800AA980(s32 arg0, s32 arg1) {
    if (arg0 == 0) {
        return func_800AA930(arg1);
    }

    if (arg0 == 3) {
        return (~func_800AA930(arg1)) & 1;
    }
}

s32 func_800AA9C8(s32 arg0, s32 arg1) {
    if (arg0 == 0) {
        return (D_8007809A >> arg1) & 1;
    }

    if (arg0 == 3) {
        return ((D_8007809A >> arg1) & 1) ^ 1;
    }
}

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

s32 func_800AAA9C(s32 arg0, u32 arg1, BattleEntityData* arg2) {
    if (arg1 < 16) {
        if (arg0 == 0 && ((arg2->unk80 >> arg1) & 1)) {
            return 1;
        }
        
        if (arg0 == 3 && !((arg2->unk80 >> arg1) & 1)) {
            return 1;
        }
    }
    
    else if (arg0 == 0 && (arg2->unk8 & (1 << (arg1 - 16)))) {
        return 1;
    } 
    
    else if (arg0 == 3 && !(arg2->unk8 & (1 << (arg1 - 16)))) {
        return 1;  
    }
        
    
    return 0;
}

s32 func_800AAB50(s32 arg0, u32 arg1, BattleEntityData* arg2) {
    switch (arg1) {
        case 1 ... 9:
            return func_800AA68C(arg2->unk18, arg0, (arg2->unk1C / 10) * arg1);
        
        case 10:
            return func_800AA68C(arg2->unk18, arg0, arg2->unk1C >> 2);        

        default:
            return func_800AA68C(arg2->unk18, arg0, arg1);
    }
}

s32 func_800AABEC(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    if (arg4 != 0 && arg1 == 0) {
        if (D_800ED148.entities[arg3].controlFlags & 1) {
            BattleEntityData* var_a2 = (BattleEntityData*)&D_800ED148.entities[arg3].entityData;
            arg2 += func_800AAA9C(arg0, arg1, var_a2);
        }
        
        return arg2;
    }

    else if (D_800ED148.entities[arg3].controlFlags & 1) {
        if (!(D_800ED148.entities[arg3].status & 1)) {
            BattleEntityData* var_a2 = (BattleEntityData*)&D_800ED148.entities[arg3].entityData;
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
                if (D_800ED148.entities[i].linkedIdx == arg0) {
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
        
         arg1 += func_800AAA9C(0, arg0, &D_800ED148.entities[arg2].entityData);
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

void func_800AAF48(s32 arg0) {
    BattleEntityData* temp_v0;

    temp_v0 = func_800AA57C(200, arg0);
    temp_v0->unk18 = temp_v0->unk1C;
}

void func_800AAF70(s32 arg0, s16 arg1) {
    BattleEntityData* temp_v0;

    temp_v0 = func_800AA57C(200, arg0);
    temp_v0->unk18 += arg1;
}

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
        func_8009AF3C(td->unk4, 0x1E, 3, 0xF0, 0);
        td->done = 1;
    }
    
    td->timer--;
}

void func_800AB0C0(s32 arg0, u16 arg1) {
    TaskEntry* currentEntry;

    currentEntry = &D_800ED148.taskData[func_8009B3D0(func_800AB054)];
    currentEntry->unk4 = func_800B0398(arg0);
    currentEntry->timer = arg1;
}

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

void func_800AB208(u16 arg0) {
    TaskEntry* currentEntry = &D_800ED148.taskData[func_8009B3D0(func_800AB1AC)];
    
    currentEntry->timer = arg0;
}

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

void func_800AB3C4(void) {
    D_800ED148.unk5C2 = 1;
    D_800ED148.unk12F9 = 0;
    D_800ED148.unk12FD = 0;
}

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
void func_800AB3FC(s32 a0, s32 a1, s32 a2) {
    volatile u8 *base = (u8 *)&D_800ED148;
    u8 *entity = (u8 *)base + a0 * 0xD0;
    s32 sub = *(s32 *)(entity + 0x14);
    s32 tbl = *(s32 *)sub;
    s32 offTab = *(s32 *)(tbl + 8) + tbl;
    s32 dataOff = *(s32 *)(tbl + 0xC);
    s32 result;
    a1 = a1 * 2 + offTab;
    result = func_800A9784(*(u16 *)a1, dataOff + tbl);
    result = func_800B0398(result);
    func_8009AF3C(result, a2, 3, 0xF0, 0);
}

void func_800AB488(s32 arg0, s32 arg1) {
    func_800AB3FC(arg0, arg1, 30);
}