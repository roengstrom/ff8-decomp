#ifndef BTL_ANIM_PACKET_H
#define BTL_ANIM_PACKET_H

#include "common.h"

/**
 * @brief Commit the packet cursor back to the active display-list buffer (btl_anim.c).
 *
 * Declared apart from btl_anim.h because tripletriad's func_800A443C returns
 * this function's value through an implicit declaration and be_object4.c
 * includes btl_anim.h: with the prototype visible there it no longer compiles,
 * and giving the definition a return type grows it by one instruction.
 *
 * @param pkt First free packet after the ones just written.
 */
extern void storeGpuPacket(u32 pkt);

#endif /* BTL_ANIM_PACKET_H */
