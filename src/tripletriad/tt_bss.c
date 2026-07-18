typedef unsigned char u8;

/**
 * @brief Memory map of the Triple Triad overlay's runtime state (BSS),
 *        0x801A2C40 (end of the file image) .. 0x801D4B38.
 *
 * LAYOUT IS LOAD-BEARING and this file is deliberately header-free: cc1
 * emits uninitialized globals as .comm in FIRST-SEEN order (header externs
 * would hijack the order), and maspsx packs them into .bss sequentially with
 * no implicit alignment — so these definitions, top to bottom, ARE the
 * memory map. Every definition is a plain byte array of the object's exact
 * size; the real types live in the headers' extern declarations. pad_*
 * arrays cover gaps with no referenced symbol. Names that alias into
 * larger objects are linker assignments in undefined_syms.tripletriad.txt.
 * The layout is guarded by make verify: referenced addresses are baked
 * into code relocations, so any drift breaks the overlay's SHA1.
 */

u8 g_tripleTriadActiveList[4];                    /* 0x801A2C40 */
u8 D_801A2C44[1];                                 /* 0x801A2C44 */
u8 pad_801A2C45[3];                               /* 0x801A2C45 */
u8 D_801A2C48[0xA];                                 /* 0x801A2C48 */
u8 pad_801A2C52[2];                               /* 0x801A2C52 */
u8 g_tripleTriadRules[4];                         /* 0x801A2C54 */
u8 pad_801A2C58[0x14];                              /* 0x801A2C58 */
u8 g_tripleTriadFrameCount[4];                    /* 0x801A2C6C */
u8 D_801A2C70[2];                                 /* 0x801A2C70 */
u8 pad_801A2C72[2];                               /* 0x801A2C72 */
u8 g_tripleTriadInputFlags[4];                    /* 0x801A2C74 */
u8 g_tripleTriadCardCounts[0x6E];                   /* 0x801A2C78 */
u8 g_tripleTriadState[1];                         /* 0x801A2CE6 */
u8 pad_801A2CE7[1];                               /* 0x801A2CE7 */
u8 g_orderingTables[0xE0];                          /* 0x801A2CE8 */
u8 g_primPools[0x20000];                            /* 0x801A2DC8 */
u8 g_vsyncMode[1];                                /* 0x801C2DC8 */
u8 g_fadeCounter[1];                              /* 0x801C2DC9 */
u8 g_drawBufferIndex[1];                          /* 0x801C2DCA */
u8 pad_801C2DCB[5];                               /* 0x801C2DCB */
u8 g_drawEnvs[0xB8];                                /* 0x801C2DD0 */
u8 g_dispEnvs[0x28];                                /* 0x801C2E88 */
u8 g_otBase[4];                                   /* 0x801C2EB0 */
u8 g_primCursor[4];                               /* 0x801C2EB4 */
u8 g_padRepeat[8];                                /* 0x801C2EB8 */
u8 g_padPressed[8];                               /* 0x801C2EC0 */
u8 g_padHeld[8];                                  /* 0x801C2EC8 */
u8 g_vramQueue[0x100];                              /* 0x801C2ED0 */
u8 g_vramQueueCount[4];                           /* 0x801C2FD0 */
u8 pad_801C2FD4[4];                               /* 0x801C2FD4 */
u8 g_scratchPtr[4];                               /* 0x801C2FD8 */
u8 pad_801C2FDC[4];                               /* 0x801C2FDC */
u8 g_textFrameBufs[0x10000];                        /* 0x801C2FE0 */
u8 g_textFbPtr[4];                                /* 0x801D2FE0 */
u8 pad_801D2FE4[0xC];                               /* 0x801D2FE4 */
u8 g_textOTs[0x10];                                 /* 0x801D2FF0 */
u8 g_textOtPtr[4];                                /* 0x801D3000 */
u8 pad_801D3004[4];                               /* 0x801D3004 */
u8 g_triadIconScratch[4];                         /* 0x801D3008 */
u8 g_cardFlipSpin[4];                             /* 0x801D300C */
u8 g_cardFlipXform[4];                            /* 0x801D3010 */
u8 pad_801D3014[4];                               /* 0x801D3014 */
u8 g_resultSfxHandle[4];                          /* 0x801D3018 */
u8 pad_801D301C[0xC];                               /* 0x801D301C */
u8 D_801D3028[0x10];                                /* 0x801D3028 */
u8 D_801D3038[0xC0];                                /* 0x801D3038 */
u8 g_cardFlipPhase[4];                            /* 0x801D30F8 */
u8 D_801D30FC[1];                                 /* 0x801D30FC */
u8 pad_801D30FD[0x13];                              /* 0x801D30FD */
u8 g_cardObjList[0x10];                             /* 0x801D3110 */
u8 g_cardObjPool[0xA0];                             /* 0x801D3120 */
u8 g_tripleTriadCardHands[0x168];                   /* 0x801D31C0 */
u8 g_substateMask[4];                             /* 0x801D3328 */
u8 g_padHeldLatch[2];                             /* 0x801D332C */
u8 g_padRepeatLatch[2];                           /* 0x801D332E */
u8 g_padPressedLatch[2];                          /* 0x801D3330 */
u8 pad_801D3332[2];                               /* 0x801D3332 */
u8 g_substateSuppress[4];                         /* 0x801D3334 */
u8 g_menuPadSource[1];                            /* 0x801D3338 */
u8 pad_801D3339[7];                               /* 0x801D3339 */
u8 D_801D3340[0x18];                                /* 0x801D3340 */
u8 g_activeSubstate[1];                           /* 0x801D3358 */
u8 g_substatePhase[1];                            /* 0x801D3359 */
u8 pad_801D335A[2];                               /* 0x801D335A */
u8 D_801D335C[4];                                 /* 0x801D335C */
u8 g_cursorPool[0x20];                              /* 0x801D3360 */
u8 g_cursorList[0x10];                              /* 0x801D3380 */
u8 D_801D3390[4];                                 /* 0x801D3390 */
u8 pad_801D3394[4];                               /* 0x801D3394 */
u8 g_tripleTriadBoard[0xC8];                        /* 0x801D3398 */
u8 g_aiSearchStack[0xD8];                           /* 0x801D3460 */
u8 g_aiPlacementBudget[4];                        /* 0x801D3538 */
u8 g_aiSearchTimer[4];                            /* 0x801D353C */
u8 g_aiTurnPool[0x20];                              /* 0x801D3540 */
u8 g_aiTurnList[0x10];                              /* 0x801D3560 */
u8 g_tripleTriadPlayerHands[0x50];                  /* 0x801D3570 */
u8 g_tripleTriadCurrentSeat[4];                   /* 0x801D35C0 */
u8 pad_801D35C4[4];                               /* 0x801D35C4 */
u8 g_evalCardBaseWeight[4];                       /* 0x801D35C8 */
u8 g_evalCardValueScale[4];                       /* 0x801D35CC */
u8 g_evalRandomRange[4];                          /* 0x801D35D0 */
u8 g_evalDifficultyWeight[4];                     /* 0x801D35D4 */
u8 g_evalHandPotentialWeight[4];                  /* 0x801D35D8 */
u8 pad_801D35DC[4];                               /* 0x801D35DC */
u8 g_tripleTriadCardValues[0x1B8];                  /* 0x801D35E0 */
u8 g_taskPool[0x4C0];                               /* 0x801D3798 */
u8 g_taskList[0x10];                                /* 0x801D3C58 */
u8 g_gradFadeList[0x10];                            /* 0x801D3C68 */
u8 g_gradFadePool[0x90];                            /* 0x801D3C78 */
u8 g_gradFadeCount[4];                            /* 0x801D3D08 */
u8 pad_801D3D0C[0x168];                             /* 0x801D3D0C */
u8 g_removeCardEdge[2];                           /* 0x801D3E74 */
u8 g_addCardEdge[2];                              /* 0x801D3E76 */
u8 g_handBuildCount[4];                           /* 0x801D3E78 */
u8 pad_801D3E7C[4];                               /* 0x801D3E7C */
u8 g_setupHandlerPool[0x20];                        /* 0x801D3E80 */
u8 g_setupHandlerList[0x10];                        /* 0x801D3EA0 */
u8 g_fadePhase[4];                                /* 0x801D3EB0 */
u8 g_fadeLastAngle[4];                            /* 0x801D3EB4 */
u8 g_fadePhaseMirror[4];                          /* 0x801D3EB8 */
u8 pad_801D3EBC[4];                               /* 0x801D3EBC */
u8 g_scriptActions[0xDC];                           /* 0x801D3EC0 */
u8 pad_801D3F9C[4];                               /* 0x801D3F9C */
u8 g_scriptHandlerList[0x10];                       /* 0x801D3FA0 */
u8 g_scriptHandlerPool[0xC8];                       /* 0x801D3FB0 */
u8 g_nameBannerBuf[0x100];                          /* 0x801D4078 */
u8 g_capturedCount[4];                            /* 0x801D4178 */
u8 pad_801D417C[0xC];                               /* 0x801D417C */
u8 g_bannerBuf[0x100];                              /* 0x801D4188 */
u8 g_sweepProcessed[4];                           /* 0x801D4288 */
u8 pad_801D428C[0xC];                               /* 0x801D428C */
u8 g_handBuildHands[0xA];                           /* 0x801D4298 */
u8 pad_801D42A2[6];                               /* 0x801D42A2 */
u8 g_claimSetupPool[0x50];                          /* 0x801D42A8 */
u8 D_801D42F8[0x10];                                /* 0x801D42F8 */
u8 g_activeCardObjs[0x140];                         /* 0x801D4308 */
u8 g_sweepTarget[4];                              /* 0x801D4448 */
u8 D_801D444C[1];                                 /* 0x801D444C */
u8 g_sweepDone[1];                                /* 0x801D444D */
u8 pad_801D444E[2];                               /* 0x801D444E */
u8 g_claimSeat[4];                                /* 0x801D4450 */
u8 D_801D4454[4];                                 /* 0x801D4454 */
u8 pad_801D4458[0x90];                              /* 0x801D4458 */
u8 g_cardDetailMsg[0x14];                           /* 0x801D44E8 */
u8 g_cardDisplaySlot[4];                          /* 0x801D44FC */
u8 D_801D4500[0x60];                                /* 0x801D4500 */
u8 D_801D4560[4];                                 /* 0x801D4560 */
u8 pad_801D4564[4];                               /* 0x801D4564 */
u8 D_801D4568[0x400];                               /* 0x801D4568 */
u8 D_801D4968[0x10];                                /* 0x801D4968 */
u8 D_801D4978[0x40];                                /* 0x801D4978 */
u8 D_801D49B8[0x10];                                /* 0x801D49B8 */
u8 D_801D49C8[0x28];                                /* 0x801D49C8 */
u8 pad_801D49F0[8];                               /* 0x801D49F0 */
u8 D_801D49F8[0x90];                                /* 0x801D49F8 */
u8 D_801D4A88[0x6E];                                /* 0x801D4A88 */
u8 D_801D4AF6[1];                                 /* 0x801D4AF6 */
u8 pad_801D4AF7[1];                               /* 0x801D4AF7 */
u8 D_801D4AF8[0x10];                                /* 0x801D4AF8 */
u8 D_801D4B08[0x10];                                /* 0x801D4B08 */
u8 D_801D4B18[2];                                 /* 0x801D4B18 */
u8 D_801D4B1A[2];                                 /* 0x801D4B1A */
u8 pad_801D4B1C[4];                               /* 0x801D4B1C */
u8 D_801D4B20[4];                                 /* 0x801D4B20 */
u8 D_801D4B24[4];                                 /* 0x801D4B24 */
u8 D_801D4B28[4];                                 /* 0x801D4B28 */
u8 D_801D4B2C[4];                                 /* 0x801D4B2C */
u8 D_801D4B30[4];                                 /* 0x801D4B30 */
u8 D_801D4B34[4];                                 /* 0x801D4B34 */
