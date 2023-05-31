
/************************************************************************/
/* Copyright (c) 1988-1989  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

/*  <ctype.h>	Microtec Research, Inc. ANSI C Compiler  */
/* @(#)ctype.h	1.8 5/14/90 */


#ifndef __CTYPE_H
#define __CTYPE_H

#define _U	0x01
#define _L	0x02
#define _N	0x04
#define _S	0x08
#define _P	0x10
#define _C	0x20
#define _B	0x40
#define _X	0x80

#undef isalnum
#undef isalpha
#undef iscntrl
#undef isdigit
#undef isgraph
#undef islower
#undef isprint
#undef ispunct
#undef isspace
#undef isupper
#undef isxdigit
#undef tolower
#undef toupper

#if __STDC__

extern const char _ctype_[];

#if __cplusplus
extern "C" {
#endif

extern int isalnum(int c);
extern int isalpha(int c);
extern int iscntrl(int c);
extern int isdigit(int c);
extern int isgraph(int c);
extern int islower(int c);
extern int isprint(int c);
extern int ispunct(int c);
extern int isspace(int c);
extern int isupper(int c);
extern int isxdigit(int c);
extern int tolower(int c);
extern int toupper(int c);

#if __cplusplus
}
#endif

#else /* !__STDC__ */

extern char _ctype_[];

extern int isalnum();
extern int isalpha();
extern int iscntrl();
extern int isdigit();
extern int isgraph();
extern int islower();
extern int isprint();
extern int ispunct();
extern int isspace();
extern int isupper();
extern int isxdigit();
extern int tolower();
extern int toupper();

#endif /* __STDC__ */

#define isalnum(c) ((_ctype_+1)[c]&(_U|_L|_N))
#define isalpha(c) ((_ctype_+1)[c]&(_U|_L))
#define iscntrl(c) (((_ctype_+1)[c])&(_C))
#define isdigit(c) ((_ctype_+1)[c]&(_N))
#define isgraph(c) ((_ctype_+1)[c]&(_U|_L|_N|_P))
#define islower(c) ((_ctype_+1)[c]&(_L))
#define isprint(c) ((_ctype_+1)[c]&(_U|_L|_N|_P|_B))
#define ispunct(c) ((_ctype_+1)[c]&(_P))
#define isspace(c) ((_ctype_+1)[c]&(_S))
#define isupper(c) ((_ctype_+1)[c]&(_U))
#define isxdigit(c) ((_ctype_+1)[c]&(_N|_X))

#endif /* end __CTYPE_H */

