nonmatching func_8002E7A4, 0x20

glabel func_8002E7A4
    /* 1EFA4 8002E7A4 E8FFBD27 */  addiu      $sp, $sp, -0x18
    /* 1EFA8 8002E7A8 1000BFAF */  sw         $ra, 0x10($sp)
    /* 1EFAC 8002E7AC 2BB9000C */  jal        func_8002E4AC
    /* 1EFB0 8002E7B0 21280000 */   addu      $a1, $zero, $zero
    /* 1EFB4 8002E7B4 1000BF8F */  lw         $ra, 0x10($sp)
    /* 1EFB8 8002E7B8 FFFF4230 */  andi       $v0, $v0, 0xFFFF
    /* 1EFBC 8002E7BC 0800E003 */  jr         $ra
    /* 1EFC0 8002E7C0 1800BD27 */   addiu     $sp, $sp, 0x18
endlabel func_8002E7A4
