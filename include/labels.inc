# This file is used by the original compiler/assembler.
# Defines the expected assembly macros.

.include "gte_macros.inc"

# A function symbol.
.macro glabel label, visibility=global
    .\visibility \label
    .type \label, @function
    \label:
        .ent \label
.endm

# The end of a function symbol.
.macro endlabel label
    .set mips1
    .size \label, . - \label
    .end \label
.endm

# An alternative entry to a function.
.macro alabel label, visibility=global
    .\visibility \label
    .type \label, @function
    \label:
        .aent \label
.endm

# A label referenced by an error handler table.
.macro ehlabel label, visibility=global
    .\visibility \label
    \label:
.endm


# A label referenced by a jumptable.
.macro jlabel label, visibility=global
    .\visibility \label
    \label:
.endm


# A data symbol.
.macro dlabel label, visibility=global
    .\visibility \label
    .type \label, @object
    \label:
.endm

# End of a data symbol.
.macro enddlabel label
    .set mips1
    .size \label, . - \label
.endm


# rfe is MIPS1-only (removed in MIPS3). Define as macro so it works at any
# MIPS level, since nonmatching sets .set mips3.
.macro rfe
    .word 0x42000010
.endm

# Label to signal the symbol haven't been matched yet.
# .set mips3 allows data bytes that decode as MIPS2+ instructions to assemble.
# endlabel/enddlabel reset back to .set mips1.
.macro nonmatching label, size=1
    .set mips3
    .global \label\().NON_MATCHING
    .type \label\().NON_MATCHING, @object
    .size \label\().NON_MATCHING, \size
    \label\().NON_MATCHING:
.endm
