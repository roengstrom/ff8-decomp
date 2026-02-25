nonmatching func_80013168, 0x20

glabel func_80013168
    /* 03968 80013168 E8FFBD27 */  addiu      $sp, $sp, -0x18
    /* 0396C 8001316C 1000BFAF */  sw         $ra, 0x10($sp)
    /* 03970 80013170 7A680C0C */  jal        func_8001A1E8
    /* 03974 80013174 44000424 */   addiu     $a0, $zero, 0x44
    /* 03978 80013178 1000BF8F */  lw         $ra, 0x10($sp)
    /* 0397C 8001317C 00000000 */  nop
    /* 03980 80013180 0800E003 */  jr         $ra
    /* 03984 80013184 1800BD27 */   addiu     $sp, $sp, 0x18
endlabel func_80013168
