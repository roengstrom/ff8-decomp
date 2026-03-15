#ifndef FIELD_H
#define FIELD_H

#include "common.h"

/**
 * @brief Field script entity / VM execution context.
 *
 * Used by the field engine's stack-based virtual machine. Each entity
 * has a stack of s32 values (slots 0-95), a stack pointer, result
 * registers, and various state fields for animation, movement, and
 * script execution.
 */
typedef struct {
    s32 stack[80];          /**< 0x000: Stack slots (s32 each, indexed by stackIdx). */
    s32 result;             /**< 0x140: Result/output register. */
    s32 result2;            /**< 0x144: Secondary result register. */
    u8 pad148[0x18];        /**< 0x148 */
    s32 unk160;             /**< 0x160 */
    u8 pad164[0x12];        /**< 0x164 */
    u16 unk176;             /**< 0x176 */
    u8 pad178[0x0C];        /**< 0x178 */
    u8 stackIdx;            /**< 0x184: Stack pointer index. */
    u8 pad185[0x03];        /**< 0x185 */
    u8 unk188;              /**< 0x188: Script parameter byte. */
    u8 unk189;              /**< 0x189: Script parameter byte. */
    u8 pad18A[0x06];        /**< 0x18A */
    u16 walkSpeed;          /**< 0x190: Walk speed (primary). */
    u16 walkSpeed2;         /**< 0x192: Walk speed (copy). */
    u16 runSpeed;           /**< 0x194: Run speed. */
    u8 pad196[0x06];        /**< 0x196 */
    u8 unk19C;              /**< 0x19C */
    u8 unk19D;              /**< 0x19D */
    u8 pad19E[0x58];        /**< 0x19E */
    u16 unk1F6;             /**< 0x1F6 */
    u16 unk1F8;             /**< 0x1F8 */
    u8 pad1FA[0x1E];        /**< 0x1FA */
    u8 pad218[0x06];        /**< 0x218 */
    u16 unk21E;             /**< 0x21E */
    u8 pad220[0x20];        /**< 0x220 */
    u8 unk240;              /**< 0x240: Animation/display byte. */
    u8 pad241[0x04];        /**< 0x241 */
    u8 unk245;              /**< 0x245 */
    u8 pad246[0x03];        /**< 0x246 */
    u8 unk249;              /**< 0x249 */
    u8 pad24A;              /**< 0x24A */
    u8 unk24B;              /**< 0x24B */
    u8 unk24C;              /**< 0x24C */
} FieldEntity;              /* size >= 0x24D */

#endif /* FIELD_H */
