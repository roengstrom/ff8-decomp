#include "common.h"

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdInit);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", EVENT_OBJ_74);

void EVENT_OBJ_84(void) {
    DeliverEvent(0xF0000003, 0x20);
}

void EVENT_OBJ_AC(void) {
    DeliverEvent(0xF0000003, 0x40);
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", EVENT_OBJ_D4);

u8 CdStatus(void) {
    extern u8 D_8005D674;
    return D_8005D674;
}

u8 CdMode(void) {
    extern u8 D_8005D684;
    return D_8005D684;
}

u8 CdLastCom(void) {
    extern u8 D_8005D685;
    return D_8005D685;
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdLastPos);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdReset);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", SYS_OBJ_98);

void CdFlush(void) { CD_flush(); }

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdSetDebug);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdComstr);

void SYS_OBJ_108(void) {
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdIntstr);

void SYS_OBJ_13C(void) {
}

void CdSync(void) { CD_sync(); }

void CdReady(void) { CD_ready(); }

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdSyncCallback);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdReadyCallback);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdControl);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdControlF);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdControlB);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", SYS_OBJ_538);

s32 CdMix(s32 *a0) {
    CD_vol(a0);
    return 1;
}

s32 CdGetSector(void *a0, s32 a1) {
    return CD_getsector(a0, a1) == 0;
}

s32 CdGetSector2(void *a0, s32 a1) {
    return CD_getsector2(a0, a1) == 0;
}

void CdDataCallback(a0) s32 a0; { DMACallback(3, a0); }

void CdDataSync(void) { CD_datasync(); }

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdIntToPos);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdPosToInt);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_64);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_25C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_35C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_3A8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_428);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_4A8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_548);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_sync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_694);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_6C4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_7AC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_ready);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_914);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_944);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_A74);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_cw);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_D70);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_DA0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_E88);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_vol);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_flush);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_initvol);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_initintr);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_init);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_1318);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_datasync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_1440);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_getsector);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_getsector2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_set_test_parmnum);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_1688);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_16BC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdSearchFile);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_108);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_2A4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_2B0);

s32 ISO9660_OBJ_2D8(char *a0, char *a1) {
    return strncmp(a0, a1, 12) == 0;
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_2F8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_594);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_5BC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_640);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_660);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_7D4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_7F0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_8D4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_8FC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", func_80045C54);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_9C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_158);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_260);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_32C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_504);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdReadBreak);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdRead);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_608);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_614);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdReadSync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_740);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdReadCallback);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdReadMode);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdDiskReady);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", TYPE_OBJ_108);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdGetDiskType);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", TYPE_OBJ_178);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", TYPE_OBJ_250);
