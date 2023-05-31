#include "lib_top.h"


#if EXCLUDE_ctype
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1992 Microtec Research, Inc.             */
/* All rights reserved                                                      */
/****************************************************************************/
/*
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
*/


/* %Z%  %M%  %I%  %G% */

/************************************************************************
 *  ctype.c
 *			C T Y P E    functions
 *
 *	Static table definition for ctype macros.
 *
 *	History:
 *	06/26/90  ajt & jpc : expanded _ctype_ table to 255
 *	09/12/89  jpc: added const qualifier
 ************************************************************************/

#include <ctype.h>

const char _ctype_[] = {		/* table of ASCII char types */

	0,		/* EOF */

/*	^@	^A	^B	^C	^D	^E	^F	^G  */
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,

/*	^H	^I	^J	^K	^L	^M	^N	^O  */
	_C,	_C|_S,	_C|_S,	_C|_S,	_C|_S,	_C|_S,	_C,	_C,

/*	^P	^Q	^R	^S	^T	^U	^V	^W  */
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,

/*	^X	^Y	^Z	^[	^\	^]	^^	^_  */
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,

/*	SP	!	"	#	$	%	&	'   */
	_S|_B,	_P,	_P,	_P,	_P,	_P,	_P,	_P,

/*	(	)	*	+	,	-	.	/   */
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P,

/*	0	1	1	3	4	5	6	7   */
	_N,	_N,	_N,	_N,	_N,	_N,	_N,	_N,

/*	8	9	:	;	<	=	>	?   */
	_N,	_N,	_P,	_P,	_P,	_P,	_P,	_P,
	
/*	@	A	B	C	D	E	F	G   */
	_P,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U,

/*	H	I	J	K	L	M	N	O   */
	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,

/*	P	Q	R	S	T	U	V	W   */
	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,

/*	X	Y	Z	[	\	]	^	_   */
	_U,	_U,	_U,	_P,	_P,	_P,	_P,	_P,

/*	`	a	b	c	d	e	f	g   */
	_P,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L,

/*	h	i	j	k	l	m	n	o   */
	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,

/*	p	q	r	s	t	u	v	w   */
	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,

/*	x	y	z	{	|	}	~	DEL */
	_L,	_L,	_L,	_P,	_P,	_P,	_P,	_C,

        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 128 .. 143 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 144 .. 159 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 160 .. 175 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 176 .. 191 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 192 .. 207 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 208 .. 223 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 224 .. 239 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /* 240 .. 255 */
 

 };
 
#endif /* EXCLUDE_ctype */
 
