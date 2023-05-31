
/************************************************************************/
/* Copyright (c) 1988-1991, 1992 Microtec Research, Inc.                */
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
/*  <mriext.h>  Microtec Research, Inc. ANSI C Compiler  */
/* %W% %G% */

#ifndef __MRIEXT_H
#define __MRIEXT_H	1

#include <stddef.h>	/* define the type "size_t" used in this header */
#include <stdio.h>	/* define the type "FILE"   used in this header */

/************************************************************************/
/*		Microtec extensions to <ctype.h>			*/
/************************************************************************/

#undef isascii
#undef toascii
#undef _tolower
#undef _toupper

#if __STDC__

    #if __cplusplus
	extern "C" {
    #endif

    extern int isascii(int);
    extern int toascii(int);
    extern int _tolower(int);
    extern int _toupper(int);

    #if __cplusplus
	}
    #endif

#else /* ! __STDC__ */

    extern int isascii();
    extern int toascii();
    extern int _tolower();
    extern int _toupper();

#endif /* ! __STDC__ */

#define isascii(c) ((unsigned)(c)<=0x7F)
#define toascii(c) ((c)&0x7F)
#define _tolower(c) ((c)+('a'-'A'))
#define _toupper(c) ((c)+('A'-'a'))

#undef	 max
#define  max(a,b)	(((a) > (b)) ? (a) : (b))
#undef	 min
#define  min(a,b)	(((a) < (b)) ? (a) : (b))

/************************************************************************/
/*		Microtec extensions to <stdio.h>			*/
/************************************************************************/

#undef	BLKSIZE
#undef	NULLPTR
#undef	TRUE
#undef	FALSE

#define BLKSIZE	BUFSIZ
#define NULLPTR ((char *)0)
#define	TRUE	1
#define	FALSE	0

#ifdef _MCC960
    /* No code inserted here */
#else
    #undef	stdaux
    #undef	stdprn
    #define	stdaux		(&_iob[3])
    #define	stdprn		(&_iob[4])
#endif

#undef	fileno
#define	fileno(f)	((f)->_file)

#if __STDC__

    #if __cplusplus
	extern "C" {
    #endif

    extern int  eprintf (const char *, ...);
    extern long getl (FILE *);
    extern int  getw (FILE *);
    extern long putl (long, FILE *);
    extern int  putw (int, FILE *);

    #if __cplusplus
	}
    #endif

#else	/* ! __STDC__ */
    extern int  eprintf ();
    extern long getl ();
    extern int  getw ();
    extern long putl ();
    extern int  putw ();
#endif

/************************************************************************/
/*		Microtec extensions to <stdlib.h>			*/
/************************************************************************/

#if __STDC__

    #if __cplusplus
	extern "C" {
    #endif

    extern int	ftoa(double, char *, int, int);
    extern int	itoa(int, char *);
    extern int	itostr(unsigned, char *, int);
    extern int	ltoa(long, char *);
    extern int	ltostr(unsigned long, char *, int);
    extern void	swab(char *, char *, int);
    extern void	*zalloc(size_t);

    #if __cplusplus
	}
    #endif

#else /* ! __STDC__ */

    extern int	ftoa();
    extern int	itoa();
    extern int	itostr();
    extern int	ltoa();
    extern int	ltostr();
    extern void	swab();
    extern void	*zalloc();

#endif /*! __STDC__ */

/************************************************************************/
/*		Microtec extensions to <string.h>			*/
/************************************************************************/

#if __STDC__

    #if __cplusplus
	extern "C" {
    #endif

    extern char   *memccpy (char *, const char *, int, size_t);
    extern char   *memclr (char *, size_t);

    #if __cplusplus
	}
    #endif

#else /* ! __STDC__ */

    extern char   *memccpy ();
    extern char   *memclr ();

#endif /* ! __STDC__ */

#endif /* end __MRIEXT_H */
