#ifndef LIBC_H
#define LIBC_H

#include "common.h"

/* --- String functions --- */
extern s32 strlen(const char *s);
extern char *strcpy(char *dst, const char *src);
extern char *strcat(char *dst, const char *src);
extern s32 strcmp(const char *s1, const char *s2);
extern s32 strncmp(const char *s1, const char *s2, u32 n);
extern s32 strtol(const char *s, char **endptr, s32 base);

/* --- Memory functions --- */
extern void *memcpy(void *dst, const void *src, u32 n);
extern void *memset(void *dst, s32 c, u32 n);
extern void *memchr(const void *s, s32 c, u32 n);
extern void *memmove(void *dst, const void *src, u32 n);
extern void bzero(void *s, u32 n);

/* --- I/O functions --- */
extern s32 printf(const char *fmt, ...);
extern s32 sprintf(char *buf, const char *fmt, ...);

/* --- Random number functions --- */
extern s32 rand(void);
extern void srand(u32 seed);

#endif /* LIBC_H */
