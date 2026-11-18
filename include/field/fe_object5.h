#ifndef FE_OBJECT5_H
#define FE_OBJECT5_H

#include "common.h"
#include "field.h"

extern s32  opHandler_SEALEDOFF(ScriptContext *context);          /* 0x800B085C  op159 */
extern s32  opHandler_RESETGF(ScriptContext *context);
extern s32  opHandler_HOLD(ScriptContext *context);                /* op131 HOLD */
extern s32  opHandler_SHOW(Actor *actor);
extern s32  opHandler_HIDE(Actor *actor);
extern s32  opHandler_TALKON(Actor *actor);
extern s32  opHandler_TALKOFF(Actor *actor);
extern s32  opHandler_PUSHON(Actor *actor);
extern s32  opHandler_PUSHOFF(Actor *actor);
extern s32  opHandler_FOLLOWOFF(Actor *actor);                /* op0AB */
extern s32  opHandler_FOLLOWON(Actor *actor);                /* op0AC */
extern s32  opHandler_THROUGHON(Actor *actor);
extern s32  opHandler_THROUGHOFF(Actor *actor);
extern s32  opHandler_ISTOUCH(Actor *actor);
extern s32  opHandler_TALKRADIUS(Actor *actor);
extern s32  opHandler_PUSHRADIUS(Actor *actor);
extern s32  opHandler_GETINFO(Actor *actor);
extern s32  opHandler_PGETINFO(ScriptContext *context);                /* op070 PGETINFO */
extern s32  opHandler_WHOAMI(ScriptContext *context);
extern s32  opHandler_JUNCTION(ScriptContext *context);                /* op??? JUNCTION */
extern s32  opHandler_COPYINFO(Actor *actor);
extern s32  opHandler_PCOPYINFO(Actor *actor);
extern s32  opHandler_ACTORMODE(Actor *actor);                /* op12D ACTORMODE */
extern s32  opHandler_MOVIEREADY(ScriptContext *context);                /* op0A3 MOVIEREADY */
extern s32  opHandler_MOVIE(ScriptContext *context);                /* op04F MOVIE */
extern void ExitMovieMode(void);
extern s32  opHandler_MOVIESYNC(ScriptContext *context);                  /* op050 MOVIESYNC */
extern s32  opHandler_SPUREADY(ScriptContext *context);                /* op056 SPUREADY */
extern s32  opHandler_SPUSYNC(ScriptContext *context);            /* 0x800B16B0 op164 */
extern s32  opHandler_MOVIECUT(ScriptContext *context);
extern s32  opHandler_SETVIBRATE(ScriptContext *context);                /* op0A1 SETVIBRATE */
extern s32  opHandler_STOPVIBRATE(ScriptContext *context);                  /* op0A2 */
extern s32  opHandler_LOADSYNC(ScriptContext *context);
extern s32  opHandler_INITSOUND(void);                    /* op0CF reset SPU vol */
extern s32  opHandler_SETBATTLEMUSIC(ScriptContext *context);     /* 0x800B1870 op0CB */
extern s32  opHandler_MUSICLOAD(ScriptContext *context);                /* op0B5 MUSICLOAD */
extern void func_800B19D4(void);                    /* MUSICCHANGE helper */
extern s32  opHandler_MUSICCHANGE(void);            /* 0x800B1A20 op0B4 */
extern s32  opHandler_MUSICREPLAY(void);                    /* op141 */
extern s32  opHandler_MUSICSKIP(ScriptContext *context);                /* op144 */
extern s32  opHandler_CHOICEMUSIC(ScriptContext *context);                /* op135 */
extern s32  opHandler_CROSSMUSIC(ScriptContext *context);                /* op0BA */
extern s32  opHandler_DUALMUSIC(ScriptContext *context);                /* op0BB */
extern s32  opHandler_KEYSIGHNCHANGE(ScriptContext *context);
extern s32  opHandler_MUSICSTOP(ScriptContext *context);          /* 0x800B1E34 op0BF */
extern s32  opHandler_MUSICSTATUS(Actor *actor);
extern s32  opHandler_OP16F(Actor *actor);
extern s32  opHandler_MUSICVOL(ScriptContext *context);           /* 0x800B1F48 op0C0 */
extern s32  opHandler_MUSICVOLTRANS(ScriptContext *context);      /* 0x800B1FE0 op0C1 */
extern s32  opHandler_MUSICVOLFADE(ScriptContext *context);                /* op0C2 */
extern s32  opHandler_MUSICVOLSYNC(ScriptContext *context);
extern void func_800B2188(void);                    /* SPU upload helper */
extern void func_800B21E0(void);                    /* EFFECTLOAD CD-read helper */
extern s32  opHandler_EFFECTLOAD(ScriptContext *context);                /* op0BD EFFECTLOAD */
extern s32  opHandler_EFFECTPLAY(ScriptContext *context);         /* 0x800B22C0 op0BC */

#endif
