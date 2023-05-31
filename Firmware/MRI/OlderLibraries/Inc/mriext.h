
/************************************************************************/
/* Copyright (c) 1988-1989  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

/*  <mriext.h>  Microtec Research, Inc. ANSI C Compiler  */
/* @(#)mriext.h	1.23 9/13/90 */

#ifndef __MRIEXT_H
#define __MRIEXT_H

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

extern int isascii(int c);
extern int toascii(int c);
extern int _tolower(int c);
extern int _toupper(int c);

#if __cplusplus
}
#endif

#else /* !__STDC__ */

extern int isascii();
extern int toascii();
extern int _tolower();
extern int _toupper();

#endif /* !__STDC__ */

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

#undef	stdaux
#undef	stdprn
#undef	fileno

#define	stdaux		(&_iob[3])
#define	stdprn		(&_iob[4])
#define	fileno(f)	((f)->_file)

#if _FPU
#undef	eprintf
#define	eprintf	_eprintf881
#endif

#if __STDC__

#if __cplusplus
extern "C" {
#endif

extern int  eprintf (const char *format, ...);
extern long getl (FILE *stream);
extern int  getw (FILE *stream);
extern long putl (long l, FILE *stream);
extern int  putw (int  w, FILE *stream);

#if __cplusplus
}
#endif

#else
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

#if _FPU		/* Convert function to '881 versions */

#undef	ftoa
#define	ftoa	_ftoa881

#endif /* _FPU */

#if __cplusplus
extern "C" {
#endif

extern int       ftoa(double value, char *string, char format, int prec);
extern int	 itoa(int i, char *cp);
extern int	 itostr(unsigned u, char *cp, int base);
extern int       ltoa(long value, char *string);
extern int       ltostr(unsigned long value, char *string, int base);
extern int	 swab(char *to, char *from, int bytes);
extern void     *zalloc(size_t size);

#if __cplusplus
}
#endif

#else /* __STDC__ */

extern int       ftoa();
extern int	 itoa();
extern int	 itostr();
extern int       ltoa();
extern int       ltostr();
extern int	 swab();
extern void     *zalloc();

#endif /*! __STDC__ */

/************************************************************************/
/*		Microtec extensions to <string.h>			*/
/************************************************************************/

#if __STDC__

#if __cplusplus
extern "C" {
#endif

extern char   *memccpy (char *s1, const char *s2, int c, size_t n);
extern char   *memclr (char *s, size_t n);

#if __cplusplus
}
#endif

#else /* __STDC__ */

extern char   *memccpy ();
extern char   *memclr ();

#endif /* ! __STDC__ */

#endif /* end __MRIEXT_H */
