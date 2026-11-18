#ifndef MCH_H
#define MCH_H

#include "common.h"
#include "psxsdk/libgte.h"

typedef struct {
    /** 0x00 */ u16 parent;
    /** 0x02 */ u16 parentOffset; // parent * sizeof(MchBone)
    /** 0x04 */ SVECTOR boneVector;
    /** 0x0C */ SVECTOR rotation;
    /** 0x14 */ MATRIX transform;
    /** 0x34 */ s32 lockRotation;
    /** 0x38 */ u8 pad38[0x08];
} MchBone;

typedef struct {
    /** 0x00 */ u8 pad0[2];
    /** 0x02 */ u8 unk2;
    /** 0x03 */ u8 width;
    /** 0x04 */ u8 height;
    /** 0x05 */ u8 frameCount;
    /** 0x06 */ u16 destX;
    /** 0x08 */ u16 unk8;
    /** 0x0A */ u16 unkA;
    /** 0x0C */ u16 frameCoords[2]; // x and y of each frame
} MchTextureAnimation;

typedef struct {
    /** 0x00 */ s16 firstVertexIndex; // index to the first vertex of this skin in the shared vertex list
    /** 0x02 */ s16 vertexCount;
    /** 0x04 */ s16 boneId; // the bone that "owns" this skin
    /** 0x06 */ s16 unk6;
} MchSkinObject;

typedef struct {
    /** 0x00 */ s16 firstSkinObjectIndex;
    /** 0x02 */ s16 skinObjectCount;
    /** 0x04 */ s16 unk4;
    /** 0x06 */ u8 pad6[0x02];
    /** 0x08 */ SVECTOR unk8;
    /** 0x10 */ s16 firstTriangleIndex;
    /** 0x12 */ s16 triangleCount;
    /** 0x14 */ s16 firstQuadIndex;
    /** 0x16 */ s16 quadCount;
    /** 0x18 */ SVECTOR unk18;
} MchDrawGroup;

#define MCH_DRAW_GROUP_FLAG_1 0x1
#define MCH_DRAW_GROUP_FLAG_2 0x2
#define MCH_DRAW_GROUP_FLAG_4 0x4

typedef struct {
    /** 0x00 */ s32 boneCount;
    /** 0x04 */ u32 vertexCount;
    /** 0x08 */ s32 textureAnimationSectionSize;
    /** 0x0C */ s32 faceCount;
    /** 0x10 */ s32 drawGroupCount;
    /** 0x14 */ s32 skinObjectCount;
    /** 0x18 */ u16 untexturedTriangleCount;
    /** 0x1A */ u16 untexturedQuadCount;
    /** 0x1C */ u16 texturedTriangleCount;
    /** 0x1E */ u16 texturedQuadCount;
    /** 0x20 */ s32 boneSectionOffset;
    /** 0x24 */ s32 vertexSectionOffset;
    /** 0x28 */ s32 textureAnimationSectionOffset;
    /** 0x2C */ s32 faceSectionOffset;
    /** 0x30 */ s32 drawGroupSectionOffset;
    /** 0x34 */ s32 skinObjectSectionOffset;
    /** 0x38 */ u16 *animationSection;
    /** 0x3C */ u32 specialBones;
} MchModelHeader;

#endif
