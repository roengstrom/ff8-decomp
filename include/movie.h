#ifndef MOVIE_H
#define MOVIE_H

#include "common.h"

#define MOVIE_HAS_PICTURE 0x1
#define MOVIE_AUDIO_ONLY 0x2

extern void MovieInit(s32 bufferSize);
extern u8 *MovieOpen(s32 lba, s32 size, s32 flags, u8 *buf);
extern s32 MovieIsBuffered(void);
extern void MoviePlay(void);
extern void MovieStop(void);
extern void MovieUpdate(void);
extern s32 MovieGetFrame(void);
extern s32 MovieIsPlaying(void);
extern s32 MovieIsAudioStreaming(void);

#endif /* MOVIE_H */
