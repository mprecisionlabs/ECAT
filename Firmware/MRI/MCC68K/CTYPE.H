
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

/*  <ctype.h>	Microtec Research, Inc. ANSI C Compiler  */
/* %W% %G% */

#ifndef __CTYPE_H
#define __CTYPE_H	1

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

    extern int isalnum(int);
    extern int isalpha(int);
    extern int iscntrl(int);
    extern int isdigit(int);
    extern int isgraph(int);
    extern int islower(int);
    extern int isprint(int);
    extern int ispunct(int);
    extern int isspace(int);
    extern int isupper(int);
    extern int isxdigit(int);
    extern int tolower(int);
    extern int toupper(int);

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

