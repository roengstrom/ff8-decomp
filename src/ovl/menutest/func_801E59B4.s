/* Handwritten assembly — uses addi $gp for PS1 scratchpad redirection.
 *
 * s32 func_801E59B4(u8 *input, u8 *textOut, GlyphEntry *glyphBuf)
 *
 * Text/string parser that processes encoded character data into display
 * buffers.  Redirects GP to scratchpad RAM (0x1F800300) for fast temp
 * storage, then iterates over the encoded input handling control codes,
 * multi-byte character decoding, and glyph width accumulation.
 *
 * Register allocation (handwritten, non-compiler):
 *   s7 = input      (a0)  Encoded string pointer, advanced by func_8002F548
 *   s3 = textOut     (a1)  Decoded text output buffer
 *   s5 = glyphBuf    (a2)  Glyph position array (8B per entry: x, y, h, pad)
 *   fp = lineCount         Return value — 0x0B newline count
 *   s4 = xPos              Horizontal glyph accumulator (reset by 0x02)
 *   s6 = yPos              Vertical position (advanced +16 by 0x02)
 *   s2 = scratch           Scratchpad decode buffer pointer (0x1F800300)
 *   s1 = buf               Local decode buffer (sp+0x10) or &D_801E7ADC
 *   s0 = iter              Character decode loop iterator
 *
 * Stack (0x80 bytes):
 *   sp+0x10  u8 localBuf[64]   Temp decode buffer
 *   sp+0x50  scratchpad base   0x1F800300
 *   sp+0x54  saved GP          Original GP before redirection
 *   sp+0x58  s0  sp+0x5C  s1  sp+0x60  s2  sp+0x64  s3
 *   sp+0x68  s4  sp+0x6C  s5  sp+0x70  s6  sp+0x74  s7
 *   sp+0x78  fp  sp+0x7C  ra
 *
 * Control codes:
 *   0x00, 0x01, 0x07 — end of string
 *   0x02             — carriage return: xPos=0, yPos+=16, re-decode
 *   0x03..0x0F       — parameter byte: copy *scratch to textOut
 *     0x05           — variable-width char: func_8002C734 → func_8002E3A4
 *   0x0A             — special decode sub-command (see 0x20/0x21/0x22/0x26)
 *   0x0B             — newline: record glyph entry, lineCount++
 *   0x10..0x18       — ignored (no output)
 *   0x19..0x1B       — multi-byte lower: idx = ch*224 + byte - 0x1520
 *   0x1C..0x1F       — multi-byte upper: idx = (ch*224 + byte - 0x18A0)|0x400
 *   0x20+            — single glyph: idx = ch - 0x20
 */

glabel func_801E59B4

    /* ---- Prologue: save callee-saved regs (scrambled order) ---- */
    addiu      $sp, $sp, -0x80
    sw         $s7, 0x74($sp)
    addu       $s7, $a0, $zero         /* input = a0 */
    sw         $s3, 0x64($sp)
    addu       $s3, $a1, $zero         /* textOut = a1 */
    sw         $s5, 0x6C($sp)
    addu       $s5, $a2, $zero         /* glyphBuf = a2 */
    sw         $fp, 0x78($sp)
    addu       $fp, $zero, $zero       /* lineCount = 0 */
    sw         $s4, 0x68($sp)
    addu       $s4, $fp, $zero         /* xPos = 0 (chained from fp) */
    sw         $s6, 0x70($sp)
    addu       $s6, $s4, $zero         /* yPos = 0 (chained from s4) */
    sw         $ra, 0x7C($sp)
    sw         $s2, 0x60($sp)
    sw         $s1, 0x5C($sp)
    sw         $s0, 0x58($sp)

    /* ---- GP redirect: save GP, point to PS1 scratchpad ---- */
    addu       $v0, $gp, $zero         /* v0 = original GP */
    lui        $t0, (0x1F800300 >> 16)
    ori        $t0, $t0, (0x1F800300 & 0xFFFF)
    addu       $gp, $t0, $zero         /* GP = 0x1F800300 (scratchpad) */
    sw         $v0, 0x54($sp)          /* save original GP */
    addu       $t0, $gp, $zero         /* t0 = scratchpad base */
    addi       $gp, $gp, 0x80          /* GP += 0x80 (handwritten: addi, not addiu) */

    /* if (input == NULL) goto end */
    beqz       $s7, .L801E5CC8
     sw        $t0, 0x50($sp)          /* save scratchpad base */

    /* ==== Outer loop: decode next input segment ==== */
.L801E5A1C:
    beqz       $s7, .L801E5CC8         /* if (input == NULL) goto end */
     addu      $a0, $s7, $zero
    lw         $a1, 0x50($sp)          /* a1 = scratchpad base */
    jal        func_8002F688            /* decode input to scratchpad */
     addiu     $a2, $zero, -0x1
    jal        func_8002F548            /* advance to next input segment */
     addu      $a0, $s7, $zero
    addu       $s7, $v0, $zero         /* input = next segment */
    lw         $s2, 0x50($sp)          /* scratch = scratchpad base */

    /* ==== Inner loop: process each decoded byte ==== */
.L801E5A40:
    nop
    lbu        $a0, 0x0($s2)           /* ch = *scratch */
    addiu      $v0, $zero, 0x1
    beq        $a0, $v0, .L801E5CC8    /* ch == 0x01 → end */
     addiu     $s2, $s2, 0x1           /* scratch++ */
    addiu      $v0, $zero, 0x7
    beq        $a0, $v0, .L801E5CC8    /* ch == 0x07 → end */
     nop
    beqz       $a0, .L801E5CC8         /* ch == 0x00 → end */
     addiu     $v0, $zero, 0xA
    bne        $a0, $v0, .L801E5BB4    /* ch != 0x0A → not special */
     addiu     $v0, $zero, 0xB

    /* ---- Special decode: ch == 0x0A, sub-command dispatch ---- */
    lbu        $a0, 0x0($s2)           /* subch = *scratch */
    addiu      $s2, $s2, 0x1
    addiu      $v0, $zero, 0x21
    beq        $a0, $v0, .L801E5ABC    /* subch == 0x21 */
     addiu     $s1, $sp, 0x10          /* buf = &localBuf (always set) */
    slti       $v0, $a0, 0x22
    beqz       $v0, .L801E5AA0         /* subch >= 0x22 */
     addiu     $v0, $zero, 0x20
    beq        $a0, $v0, .L801E5AF0    /* subch == 0x20 */
     addu      $a0, $s1, $zero
    j          .L801E5B0C              /* default → decode buf */
     addiu     $a1, $zero, 0x4

    /* subch >= 0x22 */
.L801E5AA0:
    addiu      $v0, $zero, 0x22
    beq        $a0, $v0, .L801E5AD0    /* subch == 0x22 */
     addiu     $v0, $zero, 0x26
    beq        $a0, $v0, .L801E5AE4    /* subch == 0x26 */
     addu      $a0, $s1, $zero
    j          .L801E5B0C              /* default → decode buf */
     addiu     $a1, $zero, 0x4

    /* case 0x21: D_801E7ACC[2] → func_8002F294 */
.L801E5ABC:
    lui        $t0, %hi(D_801E7ACC)
    addiu      $t0, $t0, %lo(D_801E7ACC)
    lw         $a0, 0x8($t0)
    j          .L801E5AFC
     addu      $a1, $s1, $zero

    /* case 0x22: D_801E7ACC[1] → func_8002F294 */
.L801E5AD0:
    lui        $t0, %hi(D_801E7ACC)
    addiu      $t0, $t0, %lo(D_801E7ACC)
    lw         $a0, 0x4($t0)
    j          .L801E5AFC
     addu      $a1, $s1, $zero

    /* case 0x26: buf = &D_801E7ADC, skip to copy check */
.L801E5AE4:
    lui        $v0, %hi(D_801E7ADC)
    j          .L801E5B84
     addiu     $s1, $v0, %lo(D_801E7ADC)

    /* case 0x20: D_801E7ACC[0] → func_8002F294 */
.L801E5AF0:
    lui        $t0, %hi(D_801E7ACC)
    lw         $a0, %lo(D_801E7ACC)($t0)
    addu       $a1, $s1, $zero

    /* func_8002F294(tableEntry, buf, 1) */
.L801E5AFC:
    jal        func_8002F294
     addiu     $a2, $zero, 0x1
    addu       $a0, $s1, $zero         /* a0 = buf */
    addiu      $a1, $zero, 0x4

    /* Decode buffer: func_8002F2EC(buf, 4, 1, 0x10) */
.L801E5B0C:
    addiu      $a2, $zero, 0x1
    jal        func_8002F2EC
     addiu     $a3, $zero, 0x10

    /* Skip 0x10 marker bytes at start of decoded buffer */
    lbu        $v1, 0x0($s1)
    addiu      $v0, $zero, 0x10
    bne        $v1, $v0, .L801E5B44
     nop
    addu       $v1, $v0, $zero         /* marker = 0x10 */
    addiu      $s1, $s1, 0x1
.L801E5B30:                             /* while (*buf == 0x10) buf++ */
    lbu        $v0, 0x0($s1)
    nop
    beq        $v0, $v1, .L801E5B30
     addiu     $s1, $s1, 0x1
    addiu      $s1, $s1, -0x1          /* undo last increment */

    /* Apply character decode: *iter += func_80020F84(0xB)[1] - 1 */
.L801E5B44:
    lbu        $v0, 0x0($s1)
    nop
    beqz       $v0, .L801E5B84         /* if empty → skip decode */
     addu      $s0, $s1, $zero         /* iter = buf */
.L801E5B54:                             /* do { */
    jal        func_80020F84
     addiu     $a0, $zero, 0xB
    lbu        $v0, 0x1($v0)           /*   offset = table[1] */
    lbu        $v1, 0x0($s0)           /*   ch = *iter */
    addiu      $v0, $v0, 0xFF          /*   offset -= 1 */
    addu       $v1, $v1, $v0           /*   ch += offset */
    sb         $v1, 0x0($s0)           /*   *iter = ch */
    addiu      $s0, $s0, 0x1           /*   iter++ */
    lbu        $v0, 0x0($s0)
    nop
    bnez       $v0, .L801E5B54         /* } while (*iter != 0) */
     nop

    /* Copy decoded string to textOut */
.L801E5B84:
    lbu        $v0, 0x0($s1)
    nop
    beqz       $v0, .L801E5A40         /* if empty → back to inner loop */
     nop
.L801E5B94:                             /* while (*buf) *textOut++ = *buf++ */
    addiu      $s1, $s1, 0x1
    sb         $v0, 0x0($s3)
    lbu        $v0, 0x0($s1)
    nop
    bnez       $v0, .L801E5B94
     addiu     $s3, $s3, 0x1
    j          .L801E5A40
     nop

    /* ==== Not special: ch != 0x0A ==== */
.L801E5BB4:
    bne        $a0, $v0, .L801E5BE0    /* ch != 0x0B → char handling */
     slti      $v0, $a0, 0x19

    /* ---- Newline: ch == 0x0B ---- */
    lbu        $v0, 0x0($s2)           /* lineHeight = *scratch */
    addiu      $s2, $s2, 0x1
    addiu      $fp, $fp, 0x1           /* lineCount++ */
    sh         $s4, 0x0($s5)           /* glyphBuf->x = xPos */
    sh         $s6, 0x2($s5)           /* glyphBuf->y = yPos */
    addiu      $v0, $v0, -0x20         /* lineHeight -= 0x20 */
    sh         $v0, 0x4($s5)           /* glyphBuf->h = lineHeight */
    j          .L801E5A40
     addiu     $s5, $s5, 0x8           /* glyphBuf += 8 */

    /* ==== Character output: store ch, then dispatch ==== */
.L801E5BE0:
    sb         $a0, 0x0($s3)           /* *textOut = ch */
    bnez       $v0, .L801E5C68         /* ch < 0x19 → control code */
     addiu     $s3, $s3, 0x1           /* textOut++ */

    /* ch >= 0x19: glyph character */
    slti       $v0, $a0, 0x20
    bnez       $v0, .L801E5C00         /* ch < 0x20 → multi-byte */
     slti      $v0, $a0, 0x1C
    j          .L801E5C58              /* ch >= 0x20: simple glyph */
     addiu     $a0, $a0, -0x20         /* glyphIdx = ch - 0x20 */

    /* Multi-byte glyph: 0x19 <= ch < 0x20 */
.L801E5C00:
    beqz       $v0, .L801E5C30         /* ch >= 0x1C → upper range */
     sll       $v0, $a0, 3             /* v0 = ch * 8 */

    /* Lower range [0x19..0x1B]: idx = ch*224 + byte - 0x1520 */
    subu       $v0, $v0, $a0           /* v0 = ch * 7 */
    lbu        $v1, 0x0($s2)           /* nextByte = *scratch */
    sll        $a0, $v0, 5             /* a0 = ch * 224 */
    sb         $v1, 0x0($s3)           /* *textOut = nextByte */
    addiu      $s3, $s3, 0x1
    lbu        $v0, 0x0($s2)
    addiu      $s2, $s2, 0x1
    addu       $a0, $a0, $v0           /* a0 += nextByte */
    j          .L801E5C58
     addiu     $a0, $a0, -0x1520

    /* Upper range [0x1C..0x1F]: idx = (ch*224 + byte - 0x18A0) | 0x400 */
.L801E5C30:
    subu       $v0, $v0, $a0           /* v0 = ch * 7 */
    lbu        $v1, 0x0($s2)
    sll        $a0, $v0, 5             /* a0 = ch * 224 */
    sb         $v1, 0x0($s3)
    addiu      $s3, $s3, 0x1
    lbu        $v0, 0x0($s2)
    addiu      $s2, $s2, 0x1
    addu       $a0, $a0, $v0
    addiu      $a0, $a0, -0x18A0
    ori        $a0, $a0, 0x400         /* upper page flag */

    /* Get glyph width and accumulate xPos */
.L801E5C58:
    jal        func_8002E428
     nop
    j          .L801E5A40
     addu      $s4, $s4, $v0           /* xPos += glyphWidth */

    /* ==== Control codes: ch < 0x19 ==== */
.L801E5C68:
    addiu      $v0, $zero, 0x2
    bne        $a0, $v0, .L801E5C80
     slti      $v0, $a0, 0x10

    /* ch == 0x02: carriage return — reset xPos, advance yPos */
    addu       $s4, $zero, $zero       /* xPos = 0 */
    j          .L801E5A1C              /* back to outer loop (re-decode) */
     addiu     $s6, $s6, 0x10          /* yPos += 16 */

    /* Other control codes */
.L801E5C80:
    beqz       $v0, .L801E5A40         /* ch >= 0x10 → ignore */
     nop

    /* ch < 0x10 (and != 2): copy parameter byte to textOut */
    lbu        $v0, 0x0($s2)
    nop
    sb         $v0, 0x0($s3)           /* *textOut = paramByte */
    addiu      $v0, $zero, 0x5
    bne        $a0, $v0, .L801E5CC0    /* ch != 5 → simple skip */
     addiu     $s3, $s3, 0x1           /* textOut++ */

    /* ch == 0x05: variable-width character */
    lbu        $a0, 0x0($s2)
    jal        func_8002C734            /* lookup character */
     addiu     $s2, $s2, 0x1
    jal        func_8002E3A4            /* get width */
     addu      $a0, $v0, $zero
    addu       $s4, $s4, $v0           /* xPos += width */
    j          .L801E5A40
     addiu     $s4, $s4, 0x1           /* xPos++ (inter-glyph spacing) */

    /* ch < 0x10, ch != 5: advance scratch past parameter byte */
.L801E5CC0:
    j          .L801E5A40
     addiu     $s2, $s2, 0x1

    /* ==== End: null-terminate, restore GP, return lineCount ==== */
.L801E5CC8:
    sb         $zero, 0x0($s3)         /* *textOut = '\0' */
    addi       $gp, $gp, -0x80         /* GP -= 0x80 (handwritten: addi) */
    addu       $v0, $gp, $zero
    lw         $t0, 0x54($sp)          /* t0 = saved original GP */
    nop
    addu       $gp, $t0, $zero         /* GP restored */
    addu       $v0, $fp, $zero         /* return lineCount */

    /* ---- Epilogue: restore callee-saved registers ---- */
    lw         $ra, 0x7C($sp)
    lw         $fp, 0x78($sp)
    lw         $s7, 0x74($sp)
    lw         $s6, 0x70($sp)
    lw         $s5, 0x6C($sp)
    lw         $s4, 0x68($sp)
    lw         $s3, 0x64($sp)
    lw         $s2, 0x60($sp)
    lw         $s1, 0x5C($sp)
    lw         $s0, 0x58($sp)
    addiu      $sp, $sp, 0x80
    jr         $ra
     nop

endlabel func_801E59B4
