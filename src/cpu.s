.set noat
.set noreorder

.section .text
.align 2

/* void FlushInstructionCache(void); */
.globl FlushInstructionCache
.type FlushInstructionCache, @function
.ent FlushInstructionCache
FlushInstructionCache:
    .rept 1024
    nop
    .endr
    jr      $ra
    nop
.end FlushInstructionCache
.size FlushInstructionCache, . - FlushInstructionCache
