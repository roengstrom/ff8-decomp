nonmatching func_8002AEF8, 0x5C

glabel func_8002AEF8
    /* 1B6F8 8002AEF8 E8FFBD27 */  addiu      $sp, $sp, -0x18
    /* 1B6FC 8002AEFC 1000B0AF */  sw         $s0, 0x10($sp)
    /* 1B700 8002AF00 21800400 */  addu       $s0, $a0, $zero
    /* 1B704 8002AF04 80190400 */  sll        $v1, $s0, 6
    /* 1B708 8002AF08 0880023C */  lui        $v0, %hi(D_80083210)
    /* 1B70C 8002AF0C 10324224 */  addiu      $v0, $v0, %lo(D_80083210)
    /* 1B710 8002AF10 21186200 */  addu       $v1, $v1, $v0
    /* 1B714 8002AF14 1400BFAF */  sw         $ra, 0x14($sp)
    /* 1B718 8002AF18 0900A014 */  bnez       $a1, .Lfunc_8002AEF8_end
    /* 1B71C 8002AF1C 340065A0 */   sb        $a1, 0x34($v1)
    /* 1B720 8002AF20 A4AB000C */  jal        func_8002AE90
    /* 1B724 8002AF24 21280000 */   addu      $a1, $zero, $zero
    /* 1B728 8002AF28 21200002 */  addu       $a0, $s0, $zero
    /* 1B72C 8002AF2C 9EAB000C */  jal        func_8002AE78
    /* 1B730 8002AF30 21280000 */   addu      $a1, $zero, $zero
    /* 1B734 8002AF34 21200002 */  addu       $a0, $s0, $zero
    /* 1B738 8002AF38 98AB000C */  jal        func_8002AE60
    /* 1B73C 8002AF3C 21280000 */   addu      $a1, $zero, $zero
.Lfunc_8002AEF8_end:
    /* 1B740 8002AF40 1400BF8F */  lw         $ra, 0x14($sp)
    /* 1B744 8002AF44 1000B08F */  lw         $s0, 0x10($sp)
    /* 1B748 8002AF48 1800BD27 */  addiu      $sp, $sp, 0x18
    /* 1B74C 8002AF4C 0800E003 */  jr         $ra
    /* 1B750 8002AF50 00000000 */   nop
endlabel func_8002AEF8
