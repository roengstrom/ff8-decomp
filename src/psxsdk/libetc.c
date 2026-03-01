#include "common.h"

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", VSync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", VSYNC_OBJ_A4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", VSYNC_OBJ_164);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", VSYNC_OBJ_178);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", VSYNC_OBJ_200);

typedef struct {
    void (*f0)();
    void (*f1)();
    void (*f2)();
    void (*f3)();
    void (*f4)();
    void (*f5)();
    void (*f6)();
} CallbackTable;
extern CallbackTable *D_8005D54C;

void ResetCallback() { D_8005D54C->f3(); }

void InterruptCallback() { D_8005D54C->f2(); }

void DMACallback() { D_8005D54C->f1(); }

void VSyncCallback(a0) s32 a0; { D_8005D54C->f5(4, a0); }

void VSyncCallbacks() { D_8005D54C->f5(); }

void StopCallback() { D_8005D54C->f4(); }

void RestartCallback() { D_8005D54C->f6(); }

extern u16 D_8005C4C6;
u16 CheckCallback() { return D_8005C4C6; }

u16 GetIntrMask(void) {
    extern u16 *D_8005D554;
    return *D_8005D554;
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", SetIntrMask);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_194);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_26C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_410);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_43C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_4EC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_584);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_624);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_68C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_69C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", startIntrVSync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_VB_OBJ_58);

void INTR_VB_OBJ_C4(s32 a0, s32 a1) {
    extern s32 D_8005D56C[];
    if (a1 != D_8005D56C[a0]) {
        D_8005D56C[a0] = a1;
    }
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_VB_OBJ_F0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", startIntrDMA);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_DMA_OBJ_4C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_DMA_OBJ_1CC);

void INTR_DMA_OBJ_270(void) {
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_DMA_OBJ_278);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", func_800432C4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", func_800432D8);
