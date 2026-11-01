.set noat
.set noreorder

.section .text
.align 2

.globl D_80039418
D_80039418:
    .space 0x2C

/* s32 func_80039444(u8 *src, u8 *dest); */
.globl func_80039444
.type func_80039444, @function
.ent func_80039444
func_80039444:
    lui     $at, %hi(D_80039418 + 0x28)
    sw      $a1, %lo(D_80039418 + 0x28)($at)
    lui     $v0, %hi(func_80039520)
    addiu   $v0, $v0, %lo(func_80039520)
    lui     $at, %hi(D_80039418 + 0x00)
    sw      $a0, %lo(D_80039418 + 0x00)($at)
    lui     $at, %hi(D_80039418 + 0x04)
    sw      $a1, %lo(D_80039418 + 0x04)($at)
    lui     $at, %hi(D_80039418 + 0x0C)
    sw      $a1, %lo(D_80039418 + 0x0C)($at)
    lui     $at, %hi(D_80039418 + 0x24)
    sw      $v0, %lo(D_80039418 + 0x24)($at)
    jr      $ra
    nop
.end func_80039444
.size func_80039444, . - func_80039444

/* s32 func_8003947C(void); */
.globl func_8003947C
.type func_8003947C, @function
.ent func_8003947C
func_8003947C:
    lui     $v0, %hi(D_80039418 + 0x24)
    lw      $v0, %lo(D_80039418 + 0x24)($v0)
    lui     $t0, %hi(D_80039418 + 0x00)
    lw      $t0, %lo(D_80039418 + 0x00)($t0)
    lui     $t1, %hi(D_80039418 + 0x04)
    lw      $t1, %lo(D_80039418 + 0x04)($t1)
    lui     $t2, %hi(D_80039418 + 0x08)
    lw      $t2, %lo(D_80039418 + 0x08)($t2)
    lui     $t4, %hi(D_80039418 + 0x0C)
    lw      $t4, %lo(D_80039418 + 0x0C)($t4)
    lui     $t5, %hi(D_80039418 + 0x10)
    lw      $t5, %lo(D_80039418 + 0x10)($t5)
    lui     $t6, %hi(D_80039418 + 0x14)
    lw      $t6, %lo(D_80039418 + 0x14)($t6)
    addiu   $t7, $zero, 0x5000
    lui     $a0, %hi(D_80039418 + 0x18)
    lw      $a0, %lo(D_80039418 + 0x18)($a0)
    lui     $at, %hi(D_80039418 + 0x20)
    sw      $ra, %lo(D_80039418 + 0x20)($at)
    jr      $v0
    nop
.end func_8003947C
.size func_8003947C, . - func_8003947C

.globl func_800394D0
.type func_800394D0, @function
.ent func_800394D0
func_800394D0:
    lui     $at, %hi(D_80039418 + 0x24)
    sw      $ra, %lo(D_80039418 + 0x24)($at)
    lui     $ra, %hi(D_80039418 + 0x20)
    lw      $ra, %lo(D_80039418 + 0x20)($ra)
    lui     $at, %hi(D_80039418 + 0x00)
    sw      $t0, %lo(D_80039418 + 0x00)($at)
    lui     $at, %hi(D_80039418 + 0x04)
    sw      $t1, %lo(D_80039418 + 0x04)($at)
    lui     $at, %hi(D_80039418 + 0x08)
    sw      $t2, %lo(D_80039418 + 0x08)($at)
    lui     $at, %hi(D_80039418 + 0x0C)
    sw      $t4, %lo(D_80039418 + 0x0C)($at)
    lui     $at, %hi(D_80039418 + 0x10)
    sw      $t5, %lo(D_80039418 + 0x10)($at)
    lui     $at, %hi(D_80039418 + 0x14)
    sw      $t6, %lo(D_80039418 + 0x14)($at)
    lui     $at, %hi(D_80039418 + 0x18)
    sw      $a0, %lo(D_80039418 + 0x18)($at)
    jr      $ra
    addiu   $v0, $zero, 0x1
.end func_800394D0
.size func_800394D0, . - func_800394D0

.globl func_80039520
.type func_80039520, @function
.ent func_80039520
func_80039520:
    lw      $t2, 0x0($t0)
    addiu   $t0, $t0, 0x4
    addiu   $t7, $t7, -0x4
    addu    $t6, $zero, $zero
.Lnext_flag:
    bnez    $t6, .Ltest_flag
    nop
    addiu   $t6, $zero, 0x8
    lbu     $t5, 0x0($t0)
    addiu   $t0, $t0, 0x1
    addiu   $t2, $t2, -0x1
    beqz    $t2, .Ldone
    addiu   $t7, $t7, -0x1
    bnez    $t7, .Ltest_flag
    nop
    jal     func_800394D0
    nop
.Ltest_flag:
    andi    $v0, $t5, 0x1
    beqz    $v0, .Lread_reference
    nop
    lbu     $v0, 0x0($t0)
    addiu   $t0, $t0, 0x1
    sb      $v0, 0x0($t1)
    addiu   $t1, $t1, 0x1
    addiu   $t2, $t2, -0x1
    beqz    $t2, .Ldone
    addiu   $t7, $t7, -0x1
    bnez    $t7, .Lshift_flags
    nop
    jal     func_800394D0
    nop
    j       .Lshift_flags
.Lread_reference:
    lbu     $a0, 0x0($t0)
    addiu   $t0, $t0, 0x1
    addiu   $t7, $t7, -0x1
    bnez    $t7, .Lread_reference_second
    nop
    jal     func_800394D0
    nop
.Lread_reference_second:
    lbu     $a1, 0x0($t0)
    addiu   $t0, $t0, 0x1
    andi    $v0, $a1, 0xF0
    sll     $v0, $v0, 4
    or      $a0, $a0, $v0
    andi    $v0, $a1, 0xF
    addu    $t3, $t1, $v0
    addiu   $t3, $t3, 0x3
    addiu   $v0, $a0, -0xFEE
    subu    $v1, $t1, $t4
    subu    $v0, $v1, $v0
    andi    $v0, $v0, 0xFFF
    subu    $a3, $t1, $v0
.Lfill_zero:
    sltu    $v0, $a3, $t4
    beqz    $v0, .Lcopy_reference
    nop
    sb      $zero, 0x0($t1)
    addiu   $t1, $t1, 0x1
    j       .Lfill_zero
    addiu   $a3, $a3, 0x1
.Lcopy_reference:
    sltu    $v0, $t1, $t3
    beqz    $v0, .Lreference_done
    nop
    lbu     $v0, 0x0($a3)
    addiu   $a3, $a3, 0x1
    sb      $v0, 0x0($t1)
    j       .Lcopy_reference
    addiu   $t1, $t1, 0x1
.Lreference_done:
    addiu   $t2, $t2, -0x2
    beqz    $t2, .Ldone
    addiu   $t7, $t7, -0x1
    bgtz    $t7, .Lshift_flags
    nop
    jal     func_800394D0
    nop
.Lshift_flags:
    sra     $t5, $t5, 1
    addiu   $t6, $t6, -0x1
    j       .Lnext_flag
    nop
.Ldone:
    lui     $a0, %hi(D_80039418 + 0x28)
    lw      $a0, %lo(D_80039418 + 0x28)($a0)
    nop
    subu    $v0, $t1, $a0
    lui     $at, %hi(D_80039418 + 0x28)
    sw      $v0, %lo(D_80039418 + 0x28)($at)
    addu    $v0, $zero, $zero
    jr      $ra
    nop
.end func_80039520
.size func_80039520, . - func_80039520

/* void FastMemCpy(u8 *dest, u8 *src, u32 size); */
.globl FastMemCpy
.type FastMemCpy, @function
.ent FastMemCpy
FastMemCpy:
    sltiu   $v0, $a2, 0x10
    bnez    $v0, .Lcopy_bytes
    nop
.Lcopy_words:
    lw      $t0, 0x0($a1)
    lw      $t1, 0x4($a1)
    lw      $t2, 0x8($a1)
    lw      $t3, 0xC($a1)
    addiu   $a1, $a1, 0x10
    sw      $t0, 0x0($a0)
    sw      $t1, 0x4($a0)
    sw      $t2, 0x8($a0)
    sw      $t3, 0xC($a0)
    addiu   $a2, $a2, -0x10
    sltiu   $v0, $a2, 0x10
    beqz    $v0, .Lcopy_words
    addiu   $a0, $a0, 0x10
.Lcopy_bytes:
    beqz    $a2, .Lcopy_done
.Lcopy_byte:
    lb      $t0, 0x0($a1)
    addiu   $a1, $a1, 0x1
    sb      $t0, 0x0($a0)
    addiu   $a0, $a0, 0x1
    addiu   $a2, $a2, -0x1
    bnez    $a2, .Lcopy_byte
    nop
.Lcopy_done:
    jr      $ra
    nop
.end FastMemCpy
.size FastMemCpy, . - FastMemCpy

/* void func_800396E0(void *dst, s32 size); */
.globl func_800396E0
.type func_800396E0, @function
.ent func_800396E0
func_800396E0:
    sltiu   $v0, $a1, 0x10
    bnez    $v0, .Lclear_bytes
    nop
.Lclear_words:
    sw      $zero, 0x0($a0)
    sw      $zero, 0x4($a0)
    sw      $zero, 0x8($a0)
    sw      $zero, 0xC($a0)
    addiu   $a1, $a1, -0x10
    sltiu   $v0, $a1, 0x10
    beqz    $v0, .Lclear_words
    addiu   $a0, $a0, 0x10
.Lclear_bytes:
    beqz    $a1, .Lclear_done
.Lclear_byte:
    sb      $zero, 0x0($a0)
    addiu   $a1, $a1, -0x1
    bnez    $a1, .Lclear_byte
    addiu   $a0, $a0, 0x1
.Lclear_done:
    jr      $ra
    nop
.end func_800396E0
.size func_800396E0, . - func_800396E0

/* u8 *func_80039728(u8 *table, s32 idx, s32 *size); */
.globl func_80039728
.type func_80039728, @function
.ent func_80039728
func_80039728:
    sll     $a1, $a1, 3
    addu    $a1, $a0, $a1
    addiu   $a1, $a1, 0x4
    lw      $v1, 0x4($a1)
    lw      $v0, 0x0($a1)
    sw      $v1, 0x0($a2)
    nop
    jr      $ra
    addu    $v0, $a0, $v0
.end func_80039728
.size func_80039728, . - func_80039728

/* u8 *getOffsetTableEntry(OffsetTable *table, s32 idx); */
.globl getOffsetTableEntry
.type getOffsetTableEntry, @function
.ent getOffsetTableEntry
getOffsetTableEntry:
    sll     $a1, $a1, 2
    addu    $a1, $a0, $a1
    lw      $v0, 0x0($a1)
    nop
    jr      $ra
    addu    $v0, $a0, $v0
.end getOffsetTableEntry
.size getOffsetTableEntry, . - getOffsetTableEntry
