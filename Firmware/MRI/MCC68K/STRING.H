
/************************************************************************/
/* Copyright (c) 1988-1991 Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/
/*
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
*/

/*  <string.h>  Microtec Research, Inc. ANSI C Compiler  */
/* %W% %G% */

#ifndef __STRING_H
#define __STRING_H	1

#if __cplusplus
    #define	NULL	(0)
#else
    #define	NULL	((void *)0)
#endif

#if __STDC__
    #ifndef	_SIZE_T_DEFINED
	#ifdef _sun
	    typedef int size_t;
	#else
	    typedef _SIZE_T size_t;
	#endif
	#define	_SIZE_T_DEFINED		1
    #endif

    #if __cplusplus
	extern "C" {
    #endif

    extern void   *memcpy(void *, const void *, size_t);
    extern void   *memmove(void *, const void *, size_t);
    extern char   *strcpy(char *, const char *);
    extern char   *strncpy(char *, const char *, size_t);
    extern char   *strcat(char *, const char *);
    extern char   *strncat(char *, const char *, size_t);
    extern int     memcmp(const void *, const void *, size_t);
    extern int     strcmp(const char *, const char *);
    extern int     strcoll(const char *, const char *);
    extern int     strncmp(const char *, const char *, size_t);
    extern size_t  strxfrm(char *, const char *, size_t);
    extern void   *memchr(const void *, int, size_t);
    extern char   *strchr(const char *, int);
    extern size_t  strcspn(const char *, const char *);
    extern char   *strpbrk(const char *, const char *);
    extern char   *strrchr(const char *, int);
    extern size_t  strspn(const char *, const char *);
    extern char   *strstr(const char *, const char *);
    extern char   *strtok(char *, const char *);
    extern void   *memset(void *, int, size_t);
    extern char   *strerror(int);
    extern size_t  strlen(const char *);

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
    extern int	   strxfrm();
    extern void   *memchr();
    extern char   *strchr();
    extern int	   strcspn();
    extern char   *strpbrk();
    extern char   *strrchr();
    extern int	   strspn();
    extern char   *strstr();
    extern char   *strtok();
    extern void   *memset();
    extern char   *strerror();
    extern int	   strlen();

#endif /* __STDC__ */

#endif /* end __STRING_H */
