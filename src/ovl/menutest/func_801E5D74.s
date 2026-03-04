/* Menu test state machine — 28-case switch driving all menu navigation/display.
 *
 * void func_801E5D74(MenuState *state)
 *
 * State variable at state+0x10 selects the current phase:
 *   0: set state=1                    14: setup D_801E7ACC, decode string
 *   1: VSync wait (func_80035E00)     15: fade-in animation (+0x100/frame)
 *   2: page transition checks         16: input poll + page decode
 *   3: alloc resource (func_801E5800) 17: D_801E7ACC setup + fade
 *   4: fade-in animation (+0x100)     18: alloc + string decode
 *   5: input poll (shared w/ 8,23)    19: scroll animation (-0x200)
 *   6: alloc with D_801E7ACC setup    20: set state=0x15, fall to 21
 *   7: fade-in animation              21: input poll + page confirm
 *   8: input poll (shared w/ 5,23)    22: results/scoring
 *   9: string decode (func_801E58B8)  23: input poll (shared w/ 5,8)
 *  10: fade-in animation              24: set state=0x19, fall to 25
 *  11: input poll + scroll dispatch   25: fade-out animation (-0x100)
 *  12: string decode dispatch         26: VSync wait
 *  13: input poll + scroll dispatch   27: cleanup + exit
 *
 * Register allocation:
 *   s1 = state     ($17, a0 — menu state structure pointer)
 *   s2 = stateP10  ($18, s1+0x10 — pointer to state variable u16)
 *   s3 = dispBufA  ($19, D_801FAB00+0x1A — display buffer A index)
 *   s0 = inputMask ($16, D_801FAB00+0x1C — controller input bitmask)
 *
 * Stack layout (0x28 bytes):
 *   sp+0x10  saved s0
 *   sp+0x14  saved s1
 *   sp+0x18  saved s2
 *   sp+0x1C  saved s3
 *   sp+0x20  saved ra
 *
 * @note Scrambled prologue save order: s1, s2, ra, s3, s0.
 *       C compiler cannot reproduce the exact s-reg assignment (s1 for first
 *       param instead of s0) due to graph coloring constraints.
 */


glabel func_801E5D74
    /* 574 801E5D74 D8FFBD27 */  addiu      $sp, $sp, -0x28
    /* 578 801E5D78 1400B1AF */  sw         $s1, 0x14($sp)
    /* 57C 801E5D7C 21888000 */  addu       $s1, $a0, $zero
    /* 580 801E5D80 1800B2AF */  sw         $s2, 0x18($sp)
    /* 584 801E5D84 10003226 */  addiu      $s2, $s1, 0x10
    /* 588 801E5D88 2080023C */  lui        $v0, %hi(D_801FAB00)
    /* 58C 801E5D8C 00AB4224 */  addiu      $v0, $v0, %lo(D_801FAB00)
    /* 590 801E5D90 2000BFAF */  sw         $ra, 0x20($sp)
    /* 594 801E5D94 1C00B3AF */  sw         $s3, 0x1C($sp)
    /* 598 801E5D98 1000B0AF */  sw         $s0, 0x10($sp)
    /* 59C 801E5D9C 1A005394 */  lhu        $s3, 0x1A($v0)
    /* 5A0 801E5DA0 1C005094 */  lhu        $s0, 0x1C($v0)
    /* 5A4 801E5DA4 10002496 */  lhu        $a0, 0x10($s1)
  .L801E5DA8:
    /* 5A8 801E5DA8 00000000 */  nop
    /* 5AC 801E5DAC FFFF8330 */  andi       $v1, $a0, 0xFFFF
    /* 5B0 801E5DB0 1C00622C */  sltiu      $v0, $v1, 0x1C
    /* 5B4 801E5DB4 AE014010 */  beqz       $v0, .L801E6470
    /* 5B8 801E5DB8 1E80023C */   lui       $v0, %hi(jtbl_801E6948)
    /* 5BC 801E5DBC 48694224 */  addiu      $v0, $v0, %lo(jtbl_801E6948)
    /* 5C0 801E5DC0 80180300 */  sll        $v1, $v1, 2
    /* 5C4 801E5DC4 21186200 */  addu       $v1, $v1, $v0
    /* 5C8 801E5DC8 0000628C */  lw         $v0, 0x0($v1)
    /* 5CC 801E5DCC 00000000 */  nop
    /* 5D0 801E5DD0 08004000 */  jr         $v0
    /* 5D4 801E5DD4 00000000 */   nop
    /* ---- case 0 ---- */
  jlabel .L801E5DD8
    /* 5D8 801E5DD8 01000224 */  addiu      $v0, $zero, 0x1
    /* 5DC 801E5DDC 1C990708 */  j          .L801E6470
    /* 5E0 801E5DE0 000042A6 */   sh        $v0, 0x0($s2)
    /* ---- case 1 ---- */
  jlabel .L801E5DE4
    /* 5E4 801E5DE4 80D7000C */  jal        func_80035E00
    /* 5E8 801E5DE8 00000000 */   nop
    /* 5EC 801E5DEC A0014014 */  bnez       $v0, .L801E6470
    /* 5F0 801E5DF0 02000224 */   addiu     $v0, $zero, 0x2
    /* 5F4 801E5DF4 1C990708 */  j          .L801E6470
    /* 5F8 801E5DF8 000042A6 */   sh        $v0, 0x0($s2)
    /* ---- case 2 ---- */
  jlabel .L801E5DFC
    /* 5FC 801E5DFC 2C002392 */  lbu        $v1, 0x2C($s1)
    /* 600 801E5E00 01000224 */  addiu      $v0, $zero, 0x1
    /* 604 801E5E04 E8FF6210 */  beq        $v1, $v0, .L801E5DA8
    /* 608 801E5E08 11000424 */   addiu     $a0, $zero, 0x11
    /* 60C 801E5E0C 2D002292 */  lbu        $v0, 0x2D($s1)
    /* 610 801E5E10 00000000 */  nop
    /* 614 801E5E14 1E00422C */  sltiu      $v0, $v0, 0x1E
    /* 618 801E5E18 E3FF4010 */  beqz       $v0, .L801E5DA8
    /* 61C 801E5E1C 03000424 */   addiu     $a0, $zero, 0x3
    /* 620 801E5E20 95DE070C */  jal        func_801F7A54
    /* 624 801E5E24 00000000 */   nop
    /* 628 801E5E28 1F000324 */  addiu      $v1, $zero, 0x1F
    /* 62C 801E5E2C DEFF4310 */  beq        $v0, $v1, .L801E5DA8
    /* 630 801E5E30 09000424 */   addiu     $a0, $zero, 0x9
    /* 634 801E5E34 2080023C */  lui        $v0, %hi(D_801FABD4)
    /* 638 801E5E38 2D002392 */  lbu        $v1, 0x2D($s1)
    /* 63C 801E5E3C D4AB4290 */  lbu        $v0, %lo(D_801FABD4)($v0)
    /* 640 801E5E40 01006324 */  addiu      $v1, $v1, 0x1
    /* 644 801E5E44 2A104300 */  slt        $v0, $v0, $v1
    /* 648 801E5E48 D7FF4010 */  beqz       $v0, .L801E5DA8
    /* 64C 801E5E4C 0E000424 */   addiu     $a0, $zero, 0xE
    /* 650 801E5E50 6A970708 */  j          .L801E5DA8
    /* 654 801E5E54 06000424 */   addiu     $a0, $zero, 0x6
    /* ---- case 3 ---- */
  jlabel .L801E5E58
    /* 658 801E5E58 0096070C */  jal        func_801E5800
    /* 65C 801E5E5C 21000424 */   addiu     $a0, $zero, 0x21
    /* 660 801E5E60 1C80043C */  lui        $a0, (0x801CD000 >> 16)
    /* 664 801E5E64 00D08434 */  ori        $a0, $a0, (0x801CD000 & 0xFFFF)
    /* 668 801E5E68 05000524 */  addiu      $a1, $zero, 0x5
    /* 66C 801E5E6C 4697070C */  jal        func_801E5D18
    /* 670 801E5E70 200022AE */   sw        $v0, 0x20($s1)
    /* 674 801E5E74 FF970708 */  j          .L801E5FFC
    /* 678 801E5E78 04000224 */   addiu     $v0, $zero, 0x4
    /* ---- case 4 ---- */
  jlabel .L801E5E7C
    /* 67C 801E5E7C 24002296 */  lhu        $v0, 0x24($s1)
    /* 680 801E5E80 00000000 */  nop
    /* 684 801E5E84 00014224 */  addiu      $v0, $v0, 0x100
    /* 688 801E5E88 240022A6 */  sh         $v0, 0x24($s1)
    /* 68C 801E5E8C 00140200 */  sll        $v0, $v0, 16
    /* 690 801E5E90 03140200 */  sra        $v0, $v0, 16
    /* 694 801E5E94 00104228 */  slti       $v0, $v0, 0x1000
    /* 698 801E5E98 67014014 */  bnez       $v0, .L801E6438
    /* 69C 801E5E9C 01000424 */   addiu     $a0, $zero, 0x1
    /* 6A0 801E5EA0 00100224 */  addiu      $v0, $zero, 0x1000
    /* 6A4 801E5EA4 240022A6 */  sh         $v0, 0x24($s1)
    /* 6A8 801E5EA8 0C990708 */  j          .L801E6430
    /* 6AC 801E5EAC 05000224 */   addiu     $v0, $zero, 0x5
    /* ---- case 6 ---- */
  jlabel .L801E5EB0
    /* 6B0 801E5EB0 21000424 */  addiu      $a0, $zero, 0x21
    /* 6B4 801E5EB4 2080023C */  lui        $v0, %hi(D_801FABD4)
    /* 6B8 801E5EB8 D4AB4390 */  lbu        $v1, %lo(D_801FABD4)($v0)
    /* 6BC 801E5EBC 1E80023C */  lui        $v0, %hi(D_801E7ACC)
    /* 6C0 801E5EC0 CC7A43AC */  sw         $v1, %lo(D_801E7ACC)($v0)
    /* 6C4 801E5EC4 2D002392 */  lbu        $v1, 0x2D($s1)
    /* 6C8 801E5EC8 CC7A4224 */  addiu      $v0, $v0, %lo(D_801E7ACC)
    /* 6CC 801E5ECC 0096070C */  jal        func_801E5800
    /* 6D0 801E5ED0 040043AC */   sw        $v1, 0x4($v0)
    /* 6D4 801E5ED4 1C80043C */  lui        $a0, (0x801CD000 >> 16)
    /* 6D8 801E5ED8 00D08434 */  ori        $a0, $a0, (0x801CD000 & 0xFFFF)
    /* 6DC 801E5EDC 0A000524 */  addiu      $a1, $zero, 0xA
    /* 6E0 801E5EE0 4697070C */  jal        func_801E5D18
    /* 6E4 801E5EE4 200022AE */   sw        $v0, 0x20($s1)
    /* 6E8 801E5EE8 FF970708 */  j          .L801E5FFC
    /* 6EC 801E5EEC 07000224 */   addiu     $v0, $zero, 0x7
    /* ---- case 7 ---- */
  jlabel .L801E5EF0
    /* 6F0 801E5EF0 24002296 */  lhu        $v0, 0x24($s1)
    /* 6F4 801E5EF4 00000000 */  nop
    /* 6F8 801E5EF8 00014224 */  addiu      $v0, $v0, 0x100
    /* 6FC 801E5EFC 240022A6 */  sh         $v0, 0x24($s1)
    /* 700 801E5F00 00140200 */  sll        $v0, $v0, 16
    /* 704 801E5F04 03140200 */  sra        $v0, $v0, 16
    /* 708 801E5F08 00104228 */  slti       $v0, $v0, 0x1000
    /* 70C 801E5F0C 4A014014 */  bnez       $v0, .L801E6438
    /* 710 801E5F10 01000424 */   addiu     $a0, $zero, 0x1
    /* 714 801E5F14 00100224 */  addiu      $v0, $zero, 0x1000
    /* 718 801E5F18 240022A6 */  sh         $v0, 0x24($s1)
    /* 71C 801E5F1C 0C990708 */  j          .L801E6430
    /* 720 801E5F20 08000224 */   addiu     $v0, $zero, 0x8
    /* ---- case 9 ---- */
  jlabel .L801E5F24
    /* 724 801E5F24 2E96070C */  jal        func_801E58B8
    /* 728 801E5F28 00000000 */   nop
    /* 72C 801E5F2C 1C80043C */  lui        $a0, (0x801CD000 >> 16)
    /* 730 801E5F30 00D08434 */  ori        $a0, $a0, (0x801CD000 & 0xFFFF)
    /* 734 801E5F34 4697070C */  jal        func_801E5D18
    /* 738 801E5F38 06000524 */   addiu     $a1, $zero, 0x6
    /* 73C 801E5F3C 01000224 */  addiu      $v0, $zero, 0x1
    /* 740 801E5F40 2F0022A2 */  sb         $v0, 0x2F($s1)
    /* 744 801E5F44 0A000224 */  addiu      $v0, $zero, 0xA
    /* 748 801E5F48 1C990708 */  j          .L801E6470
    /* 74C 801E5F4C 000042A6 */   sh        $v0, 0x0($s2)
    /* ---- case 10 ---- */
  jlabel .L801E5F50
    /* 750 801E5F50 24002296 */  lhu        $v0, 0x24($s1)
    /* 754 801E5F54 00000000 */  nop
    /* 758 801E5F58 00014224 */  addiu      $v0, $v0, 0x100
    /* 75C 801E5F5C 240022A6 */  sh         $v0, 0x24($s1)
    /* 760 801E5F60 00140200 */  sll        $v0, $v0, 16
    /* 764 801E5F64 03140200 */  sra        $v0, $v0, 16
    /* 768 801E5F68 00104228 */  slti       $v0, $v0, 0x1000
    /* 76C 801E5F6C 32014014 */  bnez       $v0, .L801E6438
    /* 770 801E5F70 01000424 */   addiu     $a0, $zero, 0x1
    /* 774 801E5F74 00100224 */  addiu      $v0, $zero, 0x1000
    /* 778 801E5F78 240022A6 */  sh         $v0, 0x24($s1)
    /* 77C 801E5F7C 0C990708 */  j          .L801E6430
    /* 780 801E5F80 0B000224 */   addiu     $v0, $zero, 0xB
    /* ---- case 11 ---- */
  jlabel .L801E5F84
    /* 784 801E5F84 1E80023C */  lui        $v0, %hi(D_801E7ABC)
    /* 788 801E5F88 BC7A4584 */  lh         $a1, %lo(D_801E7ABC)($v0)
    /* 78C 801E5F8C 2F002692 */  lbu        $a2, 0x2F($s1)
    /* 790 801E5F90 00DA070C */  jal        func_801F6800
    /* 794 801E5F94 21206002 */   addu      $a0, $s3, $zero
    /* 798 801E5F98 01000424 */  addiu      $a0, $zero, 0x1
    /* 79C 801E5F9C 21282002 */  addu       $a1, $s1, $zero
    /* 7A0 801E5FA0 0B96070C */  jal        func_801E582C
    /* 7A4 801E5FA4 2F0022A2 */   sb        $v0, 0x2F($s1)
    /* 7A8 801E5FA8 40000232 */  andi       $v0, $s0, 0x40
    /* 7AC 801E5FAC 0A004010 */  beqz       $v0, .L801E5FD8
    /* 7B0 801E5FB0 10000232 */   andi      $v0, $s0, 0x10
    /* 7B4 801E5FB4 52C3000C */  jal        func_80030D48
    /* 7B8 801E5FB8 02000424 */   addiu     $a0, $zero, 0x2
    /* 7BC 801E5FBC 2F002292 */  lbu        $v0, 0x2F($s1)
    /* 7C0 801E5FC0 00000000 */  nop
    /* 7C4 801E5FC4 02004014 */  bnez       $v0, .L801E5FD0
    /* 7C8 801E5FC8 18000224 */   addiu     $v0, $zero, 0x18
    /* 7CC 801E5FCC 0C000224 */  addiu      $v0, $zero, 0xC
  .L801E5FD0:
    /* 7D0 801E5FD0 000042A6 */  sh         $v0, 0x0($s2)
    /* 7D4 801E5FD4 10000232 */  andi       $v0, $s0, 0x10
  .L801E5FD8:
    /* 7D8 801E5FD8 25014010 */  beqz       $v0, .L801E6470
    /* 7DC 801E5FDC 03000424 */   addiu     $a0, $zero, 0x3
    /* 7E0 801E5FE0 FC980708 */  j          .L801E63F0
    /* 7E4 801E5FE4 00000000 */   nop
    /* ---- case 12 ---- */
  jlabel .L801E5FE8
    /* 7E8 801E5FE8 1C80043C */  lui        $a0, (0x801CD000 >> 16)
    /* 7EC 801E5FEC 00D08434 */  ori        $a0, $a0, (0x801CD000 & 0xFFFF)
    /* 7F0 801E5FF0 4697070C */  jal        func_801E5D18
    /* 7F4 801E5FF4 07000524 */   addiu     $a1, $zero, 0x7
    /* 7F8 801E5FF8 0D000224 */  addiu      $v0, $zero, 0xD
  .L801E5FFC:
    /* 7FC 801E5FFC 2F0020A2 */  sb         $zero, 0x2F($s1)
    /* 800 801E6000 1C990708 */  j          .L801E6470
    /* 804 801E6004 000042A6 */   sh        $v0, 0x0($s2)
    /* ---- case 13 ---- */
  jlabel .L801E6008
    /* 808 801E6008 1E80023C */  lui        $v0, %hi(D_801E7ABC)
    /* 80C 801E600C BC7A4584 */  lh         $a1, %lo(D_801E7ABC)($v0)
    /* 810 801E6010 2F002692 */  lbu        $a2, 0x2F($s1)
    /* 814 801E6014 00DA070C */  jal        func_801F6800
    /* 818 801E6018 21206002 */   addu      $a0, $s3, $zero
    /* 81C 801E601C 01000424 */  addiu      $a0, $zero, 0x1
    /* 820 801E6020 21282002 */  addu       $a1, $s1, $zero
    /* 824 801E6024 0B96070C */  jal        func_801E582C
    /* 828 801E6028 2F0022A2 */   sb        $v0, 0x2F($s1)
    /* 82C 801E602C 40000232 */  andi       $v0, $s0, 0x40
    /* 830 801E6030 0A004010 */  beqz       $v0, .L801E605C
    /* 834 801E6034 10000232 */   andi      $v0, $s0, 0x10
    /* 838 801E6038 52C3000C */  jal        func_80030D48
    /* 83C 801E603C 02000424 */   addiu     $a0, $zero, 0x2
    /* 840 801E6040 2F002292 */  lbu        $v0, 0x2F($s1)
    /* 844 801E6044 00000000 */  nop
    /* 848 801E6048 02004014 */  bnez       $v0, .L801E6054
    /* 84C 801E604C 0E000224 */   addiu     $v0, $zero, 0xE
    /* 850 801E6050 18000224 */  addiu      $v0, $zero, 0x18
  .L801E6054:
    /* 854 801E6054 000042A6 */  sh         $v0, 0x0($s2)
    /* 858 801E6058 10000232 */  andi       $v0, $s0, 0x10
  .L801E605C:
    /* 85C 801E605C 04014010 */  beqz       $v0, .L801E6470
    /* 860 801E6060 03000424 */   addiu     $a0, $zero, 0x3
    /* 864 801E6064 FC980708 */  j          .L801E63F0
    /* 868 801E6068 00000000 */   nop
    /* ---- case 14 ---- */
  jlabel .L801E606C
    /* 86C 801E606C 1C80043C */  lui        $a0, (0x801CD000 >> 16)
    /* 870 801E6070 2D002292 */  lbu        $v0, 0x2D($s1)
    /* 874 801E6074 1E80033C */  lui        $v1, %hi(D_801E7ACC)
    /* 878 801E6078 2F0020A2 */  sb         $zero, 0x2F($s1)
    /* 87C 801E607C CC7A62AC */  sw         $v0, %lo(D_801E7ACC)($v1)
    /* 880 801E6080 2D002292 */  lbu        $v0, 0x2D($s1)
    /* 884 801E6084 CC7A6324 */  addiu      $v1, $v1, %lo(D_801E7ACC)
    /* 888 801E6088 01004224 */  addiu      $v0, $v0, 0x1
    /* 88C 801E608C 040062AC */  sw         $v0, 0x4($v1)
    /* 890 801E6090 2D002592 */  lbu        $a1, 0x2D($s1)
    /* 894 801E6094 00D08434 */  ori        $a0, $a0, (0x801CD000 & 0xFFFF)
    /* 898 801E6098 4697070C */  jal        func_801E5D18
    /* 89C 801E609C 2B280500 */   sltu      $a1, $zero, $a1
    /* 8A0 801E60A0 0F000224 */  addiu      $v0, $zero, 0xF
    /* 8A4 801E60A4 1C990708 */  j          .L801E6470
    /* 8A8 801E60A8 000042A6 */   sh        $v0, 0x0($s2)
    /* ---- case 15 ---- */
  jlabel .L801E60AC
    /* 8AC 801E60AC 24002296 */  lhu        $v0, 0x24($s1)
    /* 8B0 801E60B0 00000000 */  nop
    /* 8B4 801E60B4 00014224 */  addiu      $v0, $v0, 0x100
    /* 8B8 801E60B8 240022A6 */  sh         $v0, 0x24($s1)
    /* 8BC 801E60BC 00140200 */  sll        $v0, $v0, 16
    /* 8C0 801E60C0 03140200 */  sra        $v0, $v0, 16
    /* 8C4 801E60C4 00104228 */  slti       $v0, $v0, 0x1000
    /* 8C8 801E60C8 DB004014 */  bnez       $v0, .L801E6438
    /* 8CC 801E60CC 01000424 */   addiu     $a0, $zero, 0x1
    /* 8D0 801E60D0 00100224 */  addiu      $v0, $zero, 0x1000
    /* 8D4 801E60D4 240022A6 */  sh         $v0, 0x24($s1)
    /* 8D8 801E60D8 0C990708 */  j          .L801E6430
    /* 8DC 801E60DC 10000224 */   addiu     $v0, $zero, 0x10
    /* ---- case 16 ---- */
  jlabel .L801E60E0
    /* 8E0 801E60E0 1E80023C */  lui        $v0, %hi(D_801E7ABC)
    /* 8E4 801E60E4 BC7A4584 */  lh         $a1, %lo(D_801E7ABC)($v0)
    /* 8E8 801E60E8 2F002692 */  lbu        $a2, 0x2F($s1)
    /* 8EC 801E60EC 00DA070C */  jal        func_801F6800
    /* 8F0 801E60F0 21206002 */   addu      $a0, $s3, $zero
    /* 8F4 801E60F4 01000424 */  addiu      $a0, $zero, 0x1
    /* 8F8 801E60F8 21282002 */  addu       $a1, $s1, $zero
    /* 8FC 801E60FC 0B96070C */  jal        func_801E582C
    /* 900 801E6100 2F0022A2 */   sb        $v0, 0x2F($s1)
    /* 904 801E6104 40000232 */  andi       $v0, $s0, 0x40
    /* 908 801E6108 16004010 */  beqz       $v0, .L801E6164
    /* 90C 801E610C 10000232 */   andi      $v0, $s0, 0x10
    /* 910 801E6110 52C3000C */  jal        func_80030D48
    /* 914 801E6114 02000424 */   addiu     $a0, $zero, 0x2
    /* 918 801E6118 2F002292 */  lbu        $v0, 0x2F($s1)
    /* 91C 801E611C 00000000 */  nop
    /* 920 801E6120 0E004014 */  bnez       $v0, .L801E615C
    /* 924 801E6124 18000224 */   addiu     $v0, $zero, 0x18
    /* 928 801E6128 13000424 */  addiu      $a0, $zero, 0x13
    /* 92C 801E612C 2D002292 */  lbu        $v0, 0x2D($s1)
    /* 930 801E6130 1E80033C */  lui        $v1, %hi(D_801E7ACC)
    /* 934 801E6134 01004224 */  addiu      $v0, $v0, 0x1
    /* 938 801E6138 0096070C */  jal        func_801E5800
    /* 93C 801E613C CC7A62AC */   sw        $v0, %lo(D_801E7ACC)($v1)
    /* 940 801E6140 21204000 */  addu       $a0, $v0, $zero
    /* 944 801E6144 1E80053C */  lui        $a1, %hi(D_801E71BC)
    /* 948 801E6148 BC71A524 */  addiu      $a1, $a1, %lo(D_801E71BC)
    /* 94C 801E614C 1E80063C */  lui        $a2, %hi(D_801E79BC)
    /* 950 801E6150 6D96070C */  jal        func_801E59B4
    /* 954 801E6154 BC79C624 */   addiu     $a2, $a2, %lo(D_801E79BC)
    /* 958 801E6158 12000224 */  addiu      $v0, $zero, 0x12
  .L801E615C:
    /* 95C 801E615C 000042A6 */  sh         $v0, 0x0($s2)
    /* 960 801E6160 10000232 */  andi       $v0, $s0, 0x10
  .L801E6164:
    /* 964 801E6164 C2004010 */  beqz       $v0, .L801E6470
    /* 968 801E6168 03000424 */   addiu     $a0, $zero, 0x3
    /* 96C 801E616C FC980708 */  j          .L801E63F0
    /* 970 801E6170 00000000 */   nop
    /* ---- case 17 ---- */
  jlabel .L801E6174
    /* 974 801E6174 2D002292 */  lbu        $v0, 0x2D($s1)
    /* 978 801E6178 1E80033C */  lui        $v1, %hi(D_801E7ACC)
    /* 97C 801E617C CC7A62AC */  sw         $v0, %lo(D_801E7ACC)($v1)
    /* 980 801E6180 2D002292 */  lbu        $v0, 0x2D($s1)
    /* 984 801E6184 CC7A6324 */  addiu      $v1, $v1, %lo(D_801E7ACC)
    /* 988 801E6188 01004224 */  addiu      $v0, $v0, 0x1
    /* 98C 801E618C 040062AC */  sw         $v0, 0x4($v1)
    /* 990 801E6190 24002296 */  lhu        $v0, 0x24($s1)
    /* 994 801E6194 00000000 */  nop
    /* 998 801E6198 00014224 */  addiu      $v0, $v0, 0x100
    /* 99C 801E619C 240022A6 */  sh         $v0, 0x24($s1)
    /* 9A0 801E61A0 24002286 */  lh         $v0, 0x24($s1)
    /* 9A4 801E61A4 00000000 */  nop
    /* 9A8 801E61A8 00104228 */  slti       $v0, $v0, 0x1000
    /* 9AC 801E61AC B0004014 */  bnez       $v0, .L801E6470
    /* 9B0 801E61B0 2F0020A2 */   sb        $zero, 0x2F($s1)
    /* 9B4 801E61B4 00100224 */  addiu      $v0, $zero, 0x1000
    /* 9B8 801E61B8 B5980708 */  j          .L801E62D4
    /* 9BC 801E61BC 240022A6 */   sh        $v0, 0x24($s1)
    /* ---- case 18 ---- */
  jlabel .L801E61C0
    /* 9C0 801E61C0 1B000424 */  addiu      $a0, $zero, 0x1B
    /* 9C4 801E61C4 0096070C */  jal        func_801E5800
    /* 9C8 801E61C8 2F0020A2 */   sb        $zero, 0x2F($s1)
    /* 9CC 801E61CC 1D80043C */  lui        $a0, (0x801D1000 >> 16)
    /* 9D0 801E61D0 2E002592 */  lbu        $a1, 0x2E($s1)
    /* 9D4 801E61D4 00108434 */  ori        $a0, $a0, (0x801D1000 & 0xFFFF)
    /* 9D8 801E61D8 4697070C */  jal        func_801E5D18
    /* 9DC 801E61DC 200022AE */   sw        $v0, 0x20($s1)
    /* 9E0 801E61E0 01000424 */  addiu      $a0, $zero, 0x1
    /* 9E4 801E61E4 21282002 */  addu       $a1, $s1, $zero
    /* 9E8 801E61E8 13000224 */  addiu      $v0, $zero, 0x13
    /* 9EC 801E61EC 2A0020A6 */  sh         $zero, 0x2A($s1)
    /* 9F0 801E61F0 0B96070C */  jal        func_801E582C
    /* 9F4 801E61F4 000042A6 */   sh        $v0, 0x0($s2)
    /* 9F8 801E61F8 1C990708 */  j          .L801E6470
    /* 9FC 801E61FC 00000000 */   nop
    /* ---- case 19 ---- */
  jlabel .L801E6200
    /* A00 801E6200 2A002296 */  lhu        $v0, 0x2A($s1)
    /* A04 801E6204 00000000 */  nop
    /* A08 801E6208 00FE4224 */  addiu      $v0, $v0, -0x200
    /* A0C 801E620C 2A0022A6 */  sh         $v0, 0x2A($s1)
    /* A10 801E6210 00140200 */  sll        $v0, $v0, 16
    /* A14 801E6214 03004018 */  blez       $v0, .L801E6224
    /* A18 801E6218 01000424 */   addiu     $a0, $zero, 0x1
    /* A1C 801E621C 86006012 */  beqz       $s3, .L801E6438
    /* A20 801E6220 00000000 */   nop
  .L801E6224:
    /* A24 801E6224 21282002 */  addu       $a1, $s1, $zero
    /* A28 801E6228 0B96070C */  jal        func_801E582C
    /* A2C 801E622C 2A0020A6 */   sh        $zero, 0x2A($s1)
    /* A30 801E6230 14000424 */  addiu      $a0, $zero, 0x14
    /* A34 801E6234 21108000 */  addu       $v0, $a0, $zero
    /* A38 801E6238 6A970708 */  j          .L801E5DA8
    /* A3C 801E623C 000042A6 */   sh        $v0, 0x0($s2)
    /* ---- case 20 ---- */
  jlabel .L801E6240
    /* A40 801E6240 15000224 */  addiu      $v0, $zero, 0x15
    /* A44 801E6244 000042A6 */  sh         $v0, 0x0($s2)
    /* ---- case 21 ---- */
  jlabel .L801E6248
    /* A48 801E6248 1E80023C */  lui        $v0, %hi(D_801E7ABC)
    /* A4C 801E624C BC7A4584 */  lh         $a1, %lo(D_801E7ABC)($v0)
    /* A50 801E6250 2F002692 */  lbu        $a2, 0x2F($s1)
    /* A54 801E6254 00DA070C */  jal        func_801F6800
    /* A58 801E6258 21206002 */   addu      $a0, $s3, $zero
    /* A5C 801E625C 01000424 */  addiu      $a0, $zero, 0x1
    /* A60 801E6260 21282002 */  addu       $a1, $s1, $zero
    /* A64 801E6264 0B96070C */  jal        func_801E582C
    /* A68 801E6268 2F0022A2 */   sb        $v0, 0x2F($s1)
    /* A6C 801E626C 40000232 */  andi       $v0, $s0, 0x40
    /* A70 801E6270 7F004010 */  beqz       $v0, .L801E6470
    /* A74 801E6274 00000000 */   nop
    /* A78 801E6278 52C3000C */  jal        func_80030D48
    /* A7C 801E627C 02000424 */   addiu     $a0, $zero, 0x2
    /* A80 801E6280 1E80023C */  lui        $v0, %hi(D_801E7ABE)
    /* A84 801E6284 2F002392 */  lbu        $v1, 0x2F($s1)
    /* A88 801E6288 BE7A4290 */  lbu        $v0, %lo(D_801E7ABE)($v0)
    /* A8C 801E628C 00000000 */  nop
    /* A90 801E6290 05006214 */  bne        $v1, $v0, .L801E62A8
    /* A94 801E6294 00000000 */   nop
    /* A98 801E6298 26002296 */  lhu        $v0, 0x26($s1)
    /* A9C 801E629C 00000000 */  nop
    /* AA0 801E62A0 01004224 */  addiu      $v0, $v0, 0x1
    /* AA4 801E62A4 260022A6 */  sh         $v0, 0x26($s1)
  .L801E62A8:
    /* AA8 801E62A8 2E002292 */  lbu        $v0, 0x2E($s1)
    /* AAC 801E62AC 00000000 */  nop
    /* AB0 801E62B0 01004224 */  addiu      $v0, $v0, 0x1
    /* AB4 801E62B4 2E0022A2 */  sb         $v0, 0x2E($s1)
    /* AB8 801E62B8 FF004230 */  andi       $v0, $v0, 0xFF
    /* ABC 801E62BC 0A00422C */  sltiu      $v0, $v0, 0xA
    /* AC0 801E62C0 05004014 */  bnez       $v0, .L801E62D8
    /* AC4 801E62C4 12000224 */   addiu     $v0, $zero, 0x12
    /* AC8 801E62C8 16000224 */  addiu      $v0, $zero, 0x16
    /* ACC 801E62CC 1C990708 */  j          .L801E6470
    /* AD0 801E62D0 000042A6 */   sh        $v0, 0x0($s2)
  .L801E62D4:
    /* AD4 801E62D4 12000224 */  addiu      $v0, $zero, 0x12
  .L801E62D8:
    /* AD8 801E62D8 1C990708 */  j          .L801E6470
    /* ADC 801E62DC 000042A6 */   sh        $v0, 0x0($s2)
    /* ---- case 22 ---- */
  jlabel .L801E62E0
    /* AE0 801E62E0 21000424 */  addiu      $a0, $zero, 0x21
    /* AE4 801E62E4 0096070C */  jal        func_801E5800
    /* AE8 801E62E8 2F0020A2 */   sb        $zero, 0x2F($s1)
    /* AEC 801E62EC 26002386 */  lh         $v1, 0x26($s1)
    /* AF0 801E62F0 200022AE */  sw         $v0, 0x20($s1)
    /* AF4 801E62F4 0A000224 */  addiu      $v0, $zero, 0xA
    /* AF8 801E62F8 20006214 */  bne        $v1, $v0, .L801E637C
    /* AFC 801E62FC 16000424 */   addiu     $a0, $zero, 0x16
    /* B00 801E6300 2D002292 */  lbu        $v0, 0x2D($s1)
    /* B04 801E6304 1E80033C */  lui        $v1, %hi(D_801E7ACC)
    /* B08 801E6308 01004224 */  addiu      $v0, $v0, 0x1
    /* B0C 801E630C 5EC3000C */  jal        func_80030D78
    /* B10 801E6310 CC7A62AC */   sw        $v0, %lo(D_801E7ACC)($v1)
    /* B14 801E6314 5EC3000C */  jal        func_80030D78
    /* B18 801E6318 17000424 */   addiu     $a0, $zero, 0x17
    /* B1C 801E631C 5EC3000C */  jal        func_80030D78
    /* B20 801E6320 18000424 */   addiu     $a0, $zero, 0x18
    /* B24 801E6324 2C002292 */  lbu        $v0, 0x2C($s1)
    /* B28 801E6328 00000000 */  nop
    /* B2C 801E632C 20004014 */  bnez       $v0, .L801E63B0
    /* B30 801E6330 08001024 */   addiu     $s0, $zero, 0x8
    /* B34 801E6334 0780023C */  lui        $v0, %hi(g_gameState)
    /* B38 801E6338 78734224 */  addiu      $v0, $v0, %lo(g_gameState)
    /* B3C 801E633C 330D4390 */  lbu        $v1, 0xD33($v0)
    /* B40 801E6340 00000000 */  nop
    /* B44 801E6344 01006324 */  addiu      $v1, $v1, 0x1
    /* B48 801E6348 95DE070C */  jal        func_801F7A54
    /* B4C 801E634C 330D43A0 */   sb        $v1, 0xD33($v0)
    /* B50 801E6350 1F000324 */  addiu      $v1, $zero, 0x1F
    /* B54 801E6354 05004310 */  beq        $v0, $v1, .L801E636C
    /* B58 801E6358 03001024 */   addiu     $s0, $zero, 0x3
    /* B5C 801E635C 95DE070C */  jal        func_801F7A54
    /* B60 801E6360 02001024 */   addiu     $s0, $zero, 0x2
    /* B64 801E6364 B5DE070C */  jal        func_801F7AD4
    /* B68 801E6368 01004424 */   addiu     $a0, $v0, 0x1
  .L801E636C:
    /* B6C 801E636C 2E96070C */  jal        func_801E58B8
    /* B70 801E6370 00000000 */   nop
    /* B74 801E6374 ED980708 */  j          .L801E63B4
    /* B78 801E6378 1C80043C */   lui       $a0, (0x801CD000 >> 16)
  .L801E637C:
    /* B7C 801E637C 52C3000C */  jal        func_80030D48
    /* B80 801E6380 05000424 */   addiu     $a0, $zero, 0x5
    /* B84 801E6384 26002386 */  lh         $v1, 0x26($s1)
    /* B88 801E6388 1E80043C */  lui        $a0, %hi(D_801E7ACC)
    /* B8C 801E638C 80100300 */  sll        $v0, $v1, 2
    /* B90 801E6390 21104300 */  addu       $v0, $v0, $v1
    /* B94 801E6394 40100200 */  sll        $v0, $v0, 1
    /* B98 801E6398 CC7A82AC */  sw         $v0, %lo(D_801E7ACC)($a0)
    /* B9C 801E639C 2C002292 */  lbu        $v0, 0x2C($s1)
    /* BA0 801E63A0 00000000 */  nop
    /* BA4 801E63A4 02004010 */  beqz       $v0, .L801E63B0
    /* BA8 801E63A8 04001024 */   addiu     $s0, $zero, 0x4
    /* BAC 801E63AC 09001024 */  addiu      $s0, $zero, 0x9
  .L801E63B0:
    /* BB0 801E63B0 1C80043C */  lui        $a0, (0x801CD000 >> 16)
  .L801E63B4:
    /* BB4 801E63B4 00D08434 */  ori        $a0, $a0, (0x801CD000 & 0xFFFF)
    /* BB8 801E63B8 4697070C */  jal        func_801E5D18
    /* BBC 801E63BC 21280002 */   addu      $a1, $s0, $zero
    /* BC0 801E63C0 01000424 */  addiu      $a0, $zero, 0x1
    /* BC4 801E63C4 0B96070C */  jal        func_801E582C
    /* BC8 801E63C8 21282002 */   addu      $a1, $s1, $zero
    /* BCC 801E63CC 17000224 */  addiu      $v0, $zero, 0x17
    /* BD0 801E63D0 1C990708 */  j          .L801E6470
    /* BD4 801E63D4 000042A6 */   sh        $v0, 0x0($s2)
    /* ---- case 5, 8, 23 ---- */
  jlabel .L801E63D8
    /* BD8 801E63D8 01000424 */  addiu      $a0, $zero, 0x1
    /* BDC 801E63DC 0B96070C */  jal        func_801E582C
    /* BE0 801E63E0 21282002 */   addu      $a1, $s1, $zero
    /* BE4 801E63E4 40000232 */  andi       $v0, $s0, 0x40
    /* BE8 801E63E8 21004010 */  beqz       $v0, .L801E6470
    /* BEC 801E63EC 02000424 */   addiu     $a0, $zero, 0x2
  .L801E63F0:
    /* BF0 801E63F0 52C3000C */  jal        func_80030D48
    /* BF4 801E63F4 00000000 */   nop
    /* BF8 801E63F8 18000224 */  addiu      $v0, $zero, 0x18
    /* BFC 801E63FC 1C990708 */  j          .L801E6470
    /* C00 801E6400 000042A6 */   sh        $v0, 0x0($s2)
    /* ---- case 24 ---- */
  jlabel .L801E6404
    /* C04 801E6404 19000224 */  addiu      $v0, $zero, 0x19
    /* C08 801E6408 000042A6 */  sh         $v0, 0x0($s2)
    /* ---- case 25 ---- */
  jlabel .L801E640C
    /* C0C 801E640C 24002296 */  lhu        $v0, 0x24($s1)
    /* C10 801E6410 00000000 */  nop
    /* C14 801E6414 00FF4224 */  addiu      $v0, $v0, -0x100
    /* C18 801E6418 240022A6 */  sh         $v0, 0x24($s1)
    /* C1C 801E641C 00140200 */  sll        $v0, $v0, 16
    /* C20 801E6420 0500401C */  bgtz       $v0, .L801E6438
    /* C24 801E6424 01000424 */   addiu     $a0, $zero, 0x1
    /* C28 801E6428 1A000224 */  addiu      $v0, $zero, 0x1A
    /* C2C 801E642C 240020A6 */  sh         $zero, 0x24($s1)
  .L801E6430:
    /* C30 801E6430 000042A6 */  sh         $v0, 0x0($s2)
    /* C34 801E6434 01000424 */  addiu      $a0, $zero, 0x1
  .L801E6438:
    /* C38 801E6438 0B96070C */  jal        func_801E582C
    /* C3C 801E643C 21282002 */   addu      $a1, $s1, $zero
    /* C40 801E6440 1C990708 */  j          .L801E6470
    /* C44 801E6444 00000000 */   nop
    /* ---- case 26 ---- */
  jlabel .L801E6448
    /* C48 801E6448 80D7000C */  jal        func_80035E00
    /* C4C 801E644C 00000000 */   nop
    /* C50 801E6450 07004014 */  bnez       $v0, .L801E6470
    /* C54 801E6454 1B000224 */   addiu     $v0, $zero, 0x1B
    /* C58 801E6458 1C990708 */  j          .L801E6470
    /* C5C 801E645C 000042A6 */   sh        $v0, 0x0($s2)
    /* ---- case 27 ---- */
  jlabel .L801E6460
    /* C60 801E6460 3FC6070C */  jal        func_801F18FC
    /* C64 801E6464 21202002 */   addu      $a0, $s1, $zero
    /* C68 801E6468 ECC2070C */  jal        func_801F0BB0
    /* C6C 801E646C 00000000 */   nop
  .L801E6470:
    /* C70 801E6470 24002486 */  lh         $a0, 0x24($s1)
    /* C74 801E6474 6CC7070C */  jal        func_801F1DB0
    /* C78 801E6478 00000000 */   nop
    /* C7C 801E647C 24002486 */  lh         $a0, 0x24($s1)
    /* C80 801E6480 55B2000C */  jal        func_8002C954
    /* C84 801E6484 00000000 */   nop
    /* C88 801E6488 24002486 */  lh         $a0, 0x24($s1)
    /* C8C 801E648C 52C2070C */  jal        func_801F0948
    /* C90 801E6490 00000000 */   nop
    /* C94 801E6494 2000BF8F */  lw         $ra, 0x20($sp)
    /* C98 801E6498 1C00B38F */  lw         $s3, 0x1C($sp)
    /* C9C 801E649C 1800B28F */  lw         $s2, 0x18($sp)
    /* CA0 801E64A0 1400B18F */  lw         $s1, 0x14($sp)
    /* CA4 801E64A4 1000B08F */  lw         $s0, 0x10($sp)
    /* CA8 801E64A8 2800BD27 */  addiu      $sp, $sp, 0x28
    /* CAC 801E64AC 0800E003 */  jr         $ra
    /* CB0 801E64B0 00000000 */   nop
endlabel func_801E5D74
