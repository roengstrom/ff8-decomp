#ifndef LIBSPU_H
#define LIBSPU_H

typedef void (*SpuIRQCallbackProc)(void);
typedef void (*SpuTransferCallbackProc)(void);

extern void SpuInit (void);
extern void SpuStart (void);
extern void SpuQuit (void);
extern long SpuSetReverb (long on_off);
extern unsigned long SpuWrite (unsigned char *addr, unsigned long size);
extern unsigned long SpuRead (unsigned char *addr, unsigned long size);
extern long SpuSetTransferMode (long mode);
extern unsigned long SpuSetTransferStartAddr (unsigned long addr);
extern SpuTransferCallbackProc SpuSetTransferCallback (SpuTransferCallbackProc func);
extern long SpuSetIRQ (long on_off);
extern unsigned long SpuSetIRQAddr (unsigned long);
extern SpuIRQCallbackProc SpuSetIRQCallback (SpuIRQCallbackProc);
extern long SpuInitMalloc (long num, char *top);
extern void SpuGetVoiceEnvelope (int vNum, short *envx);
extern long SpuSetReverbModeType (long mode);
extern void SpuSetReverbModeDepth (short depth_left, short depth_right);
extern void SpuGetReverbModeType (long *mode);

#endif /* LIBSPU_H */
