nonmatching func_8003882C, 0x3C

glabel func_8003882C
    /* 2902C 8003882C E0FFBD27 */  addiu      $sp, $sp, -0x20
    /* 29030 80038830 21108000 */  addu       $v0, $a0, $zero
    /* 29034 80038834 2118A000 */  addu       $v1, $a1, $zero
    /* 29038 80038838 2140C000 */  addu       $t0, $a2, $zero
    /* 2903C 8003883C 03000424 */  addiu      $a0, $zero, 0x3
    /* 29040 80038840 21284000 */  addu       $a1, $v0, $zero
    /* 29044 80038844 21306000 */  addu       $a2, $v1, $zero
    /* 29048 80038848 1000A7AF */  sw         $a3, 0x10($sp)
    /* 2904C 8003884C 1800BFAF */  sw         $ra, 0x18($sp)
    /* 29050 80038850 D8E1000C */  jal        func_80038760
    /* 29054 80038854 21380001 */   addu      $a3, $t0, $zero
    /* 29058 80038858 1800BF8F */  lw         $ra, 0x18($sp)
    /* 2905C 8003885C 21100000 */  addu       $v0, $zero, $zero
    /* 29060 80038860 0800E003 */  jr         $ra
    /* 29064 80038864 2000BD27 */   addiu     $sp, $sp, 0x20
endlabel func_8003882C
