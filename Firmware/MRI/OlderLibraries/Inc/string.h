
/************************************************************************/
/* Copyright (c) 1988-1989  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

/*  <string.h>  Microtec Research, Inc. ANSI C Compiler  */
/* @(#)string.h	1.2 4/13/90 */

#ifndef __STRING_H
#define __STRING_H

#define NULL ((void*)0)
typedef unsigned size_t;

#if __STDC__

#if __cplusplus
extern "C" {
#endif

extern void   *memcpy(void *s1, const void *s2, size_t n);
extern void   *memmove(void *s1, const void *s2, size_t n);
extern char   *strcpy(char *s1, const char *s2);
extern char   *strncpy(char *s1, const char *s2, size_t n);
extern char   *strcat(char *s1, const char *s2);
extern char   *strncat(char *s1, const char *s2, size_t n);
extern int     memcmp(const void *s1, const void *s2, size_t n);
extern int     strcmp(const char *s1, const char *s2);
extern int     strcoll(const char *s1, const char *s2);
extern int     strncmp(const char *s1, const char *s2, size_t n);
extern size_t  strxfrm(char *s1, const char *s2, size_t n);
extern void   *memchr(const void *s, int c, size_t n);
extern char   *strchr(const char *s, int c);
extern size_t  strcspn(const char *s1, const char *s2);
extern char   *strpbrk(const char *s1, const char *s2);
extern char   *strrchr(const char *s, int c);
extern size_t  strspn(const char *s1, const char *s2);
extern char   *strstr(const char *s1, const char *s2);
extern char   *strtok(char *s1, const char *s2);
extern void   *memset(void *s, int c, size_t n);
extern char   *strerror(int errnum);
extern size_t  strlen(const char *s);

#if __cplusplus
}
#endif

#else /* !__STDC__ */

extern void   *memcpy();
extern void   *memmove();
extern char   *strcpy();
extern char   *strncpy();
extern char   *strcat();
extern char   *strncat();
extern int     memcmp();
extern int     strcmp();
extern int     strcoll();
extern int     strncmp();
extern size_t  strxfrm();
extern void   *memchr();
extern char   *strchr();
extern size_t  strcspn();
extern char   *strpbrk();
extern char   *strrchr();
extern size_t  strspn();
extern char   *strstr();
extern char   *strtok();
extern void   *memset();
extern char   *strerror();
extern size_t  strlen();

#endif /* __STDC__ */

#endif /* end __STRING_H */
