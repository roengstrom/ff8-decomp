nonmatching func_80013168, 0x20

glabel func_80013168
    /* 3968 80013168 E8FFBD27 */  addiu      $sp, $sp, -0x18
    /* 396C 8001316C 1000BFAF */  sw         $ra, 0x10($sp)
    /* 3970 80013170 7A68000C */  jal        func_8001A1E8
    /* 3974 80013174 44000424 */   addiu     $a0, $zero, 0x44
    /* 3978 80013178 1000BF8F */  lw         $ra, 0x10($sp)
    /* 397C 8001317C 00000000 */  nop
    /* 3980 80013180 0800E003 */  jr         $ra
    /* 3984 80013184 1800BD27 */   addiu     $sp, $sp, 0x18
endlabel func_80013168
