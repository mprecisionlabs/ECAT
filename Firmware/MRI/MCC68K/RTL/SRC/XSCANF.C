#include "lib_top.h"


#if EXCLUDE_xscanf
    #pragma option -Qws			/* Prevent compiler warning */
#else

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1990, 1991, 1992 Microtec Research, Inc. */
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


/*	%W% %G%	*/

/************************************************************************/
/* NAME                         _ S C A N F                             */
/*                                                                      */
/* FUNCTION     _scanf(g,u,p,f,l) - low level scanf function            */
/*                                                                      */
/* SYNOPSIS     _scanf(getcfn, ungetcfn, param, fmt, parglist)          */
/*	        int (*getcfn) - pointer to char get function            */
/*	        int (*ungetcfn) - pointer to char unget function        */
/*	        char *param - pointer passed to char functions          */
/*	        char *fmt - format string.                              */
/*	        va_list parglist - pointer to the argument list         */
/*                                                                      */
/* RETURN       number of matched and assigned input fields             */
/*                                                                      */
/* DESCRIPTION  low level scanf function                                */
/* "getcfn" is a pointer to a function that gets the next character.    */
/*      If NULL, input is from the NUL-terminated string "param".       */
/* "ungetcfn" is a pointer to a function that ungets a character.  If   */
/*      NULL, this function does not unget a character.                 */
/* "param" is an arbitrary pointer parameter that is passed to the      */
/*      above. If "getcfn" is NULL, this is a NUL-terminated string     */
/*      with input.                                                     */
/* "fmt" is the format string.                                          */
/* "parglist" is a pointer to the argument list.                        */
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*	03/16/94  jpc:	 %i directive could not handle converting "0".	*/
/*			 (spr 34705.)  Also there was some problems w.	*/
/*			 detection of beginning of conversion, which	*/
/*			 was fixed.					*/
/*	11/15/93  jpc:	 mods in "EXCLUDE" handling to avoid double	*/
/*			 negatives.					*/
/*	03/23/92  jpc:	 fix scanning of "0" with "%x"			*/
/*	07/02/91  jpc:   mods to get ANSI spec examples working		*/
/*			 correctly.					*/
/*	05/08/91  jpc:   Bring up to ANSI snuff				*/
/*	06/19/90  jpc:	 Removed stdio.h to get rid of _iob struct	*/
/*	05/16/90  jpc:   added "g" format				*/
/*	04/23/90  jpc:   modified to use <stdarg.h> rather than		*/
/*			 <varargs.h>.					*/
/*	09/16/89  jpc:	added mrilib.h for easy '881 build		*/
/*	 9/14/89  jpc  : cast ss_getc to get rid of warning		*/
/*	03/35/87  M.H. : add "stdlib.h", because min(a,b) was moved 	*/
/*			 from "stdio.h" to "stdlib.h"			*/
/*      12/22/86  M.H. : add chtol()                                    */
/*      12/17/86  M.H. : changed _flags to _flag                        */
/*      10-Oct-86 wbd: interface to MCC86 rtl                           */
/*                                                                      */
/* Copyright 1984, Hunter & Ready Inc.                                  */
/************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include "mrilib.h"


#if EXCLUDE_FORMAT_IO_h_OPT
    #informing -- The "h" option has been disabled.
#endif

#if EXCLUDE_FORMAT_IO_l_OPT
    #informing -- The "l" option has been disabled.
#endif

#if EXCLUDE_FORMAT_IO_L_OPT
    #informing -- The "L" option has been disabled.
#endif

#if EXCLUDE_FORMAT_IO_ASSGN_SUPP
    #informing -- The "*" option has been disabled.
#endif

#if EXCLUDE_FORMAT_IO_INT_FMT
    #informing -- The "i" format has been disabled.
#endif

#if EXCLUDE_FORMAT_IO_DEC_FMT
    #informing -- The "d" format has been disabled.
#endif

#if EXCLUDE_FORMAT_IO_OCT_FMT
    #informing -- The "o" format has been disabled.
#endif

#if EXCLUDE_FORMAT_IO_UNS_FMT
    #informing -- The "u" format has been disabled.
#endif

#if EXCLUDE_FORMAT_IO_HEX_FMT
    #informing -- The "x" format has been disabled.
#endif

#if EXCLUDE_FORMAT_IO_FLOAT_FMT
    #informing -- The "f", "e", and "g" formats has been disabled.
#endif

#if EXCLUDE_FORMAT_IO_CHAR_FMT
    #informing -- The "c" format has been disabled.
#endif

#if EXCLUDE_FORMAT_IO_STR_FMT
    #informing -- The "s" format has been disabled.
#endif

#if EXCLUDE_FORMAT_IO_PNT_FMT
    #informing -- The "p" format has been disabled.
#endif

#if EXCLUDE_FORMAT_IO_NUMB_FMT
    #informing -- The "n" format has been disabled.
#endif

#if EXCLUDE_FORMAT_IO_BRAKT_FMT
    #informing -- The "[" format has been disabled.
#endif


#if EXCLUDE_FORMAT_IO_NUMB_FMT
	#define	NUMB_EXP(x)	(1)
#else
	#define	NUMB_EXP(x)	(x)
#endif	/* EXCLUDE_FORMAT_IO_NUMB_FMT */


#if EXCLUDE_FORMAT_IO_PNT_FMT
	#define	PNT_EXP(x)	(0)
	#define	PNT_DEF
#else
	#define	PNT_EXP(x)	(x)
	#define	PNT_DEF		int	ispointer;	/* %p format */
#endif  /* EXCLUDE_FORMAT_IO_PNT_FMT */

#if EXCLUDE_FORMAT_IO_ASSGN_SUPP
	#define	ASSGN_EXP(x)	(1)
	#define	ASSGN_DEF
#else
	#define	ASSGN_EXP(x)	(x)
	#define	ASSGN_DEF	int	isassigned;  /* assign value? */
#endif	/* EXCLUDE_FORMAT_IO_ASSGN_SUPP */

#if	(defined (EXCLUDE_FORMAT_IO_h_OPT)) && \
	(defined (EXCLUDE_FORMAT_IO_l_OPT)) && \
	(defined (EXCLUDE_FORMAT_IO_L_OPT))    /* h=short, l=long, other=int */
	#define	SIZE_DEF
	#define	SIZE_EXP(x)	(0)
#else
	#define	SIZE_DEF	int size;	
	#define	SIZE_EXP(x)	(x)
#endif

#if EXCLUDE_FORMAT_IO_h_OPT
	#define	OPT_h_EXP(x)	(0)
#else
	#define	OPT_h_EXP(x)	(x)
#endif

#if EXCLUDE_FORMAT_IO_l_OPT
	#define	OPT_l_EXP(x)	(0)
#else
	#define	OPT_l_EXP(x)	(x)
#endif

#if EXCLUDE_FORMAT_IO_L_OPT
	#define	OPT_L_EXP(x)	(0)
#else
	#define	OPT_L_EXP(x)	(x)
#endif

#if	(defined (EXCLUDE_FORMAT_IO_INT_FMT)) && \
	(defined (EXCLUDE_FORMAT_IO_DEC_FMT)) && \
	(defined (EXCLUDE_FORMAT_IO_UNS_FMT))
	#define	BASE_10_EXP(x)	(0)
#else
	#define	BASE_10_EXP(x)	(x)
#endif

#if	(defined (EXCLUDE_FORMAT_IO_INT_FMT)) && \
	(defined (EXCLUDE_FORMAT_IO_HEX_FMT)) && \
	(defined (EXCLUDE_FORMAT_IO_PNT_FMT))
	#define	BASE_16_EXP(x)	(0)
#else
	#define	BASE_16_EXP(x)	(x)
#endif

#if	(defined (EXCLUDE_FORMAT_IO_INT_FMT)) && \
	(defined (EXCLUDE_FORMAT_IO_OCT_FMT))
	#define	BASE_8_EXP(x)	(0)
#else
	#define	BASE_8_EXP(x)	(x)
#endif

/*	Stolen from stdio.h	*/
#define EOF             (-1)

#if _sun
    #define	NULL	((void *)0)
#endif

extern void _sfef();

#define EMPTY		-2
#define SETSIZE		32
#define MAXWIDTH	32767
#define NUL		'\0'

#define	isset(a, i)	(a[(i) >> 3] & (1 << ((i) & 7)))
#define	setbit(a, i)	a[(i) >> 3] |= 1 << ((i) & 7)
#define	clrbit(a, i)	a[(i) >> 3] &= ~(1 << ((i) & 7))

#define min(x,y)	(((x)<(y))?(x):(y))

#define getNext 					\
    {							\
	if (--width > 0) {              		\
            if ((in = (*getcfn)(param)) != EOF)		\
		++inputCnt;				\
	}						\
	else						\
	    in = EMPTY;					\
    }

#define computeCount	(inputCnt - (((in == EOF) || (in == EMPTY)) ? 0 : 1))

#define FMT_NONE	0
#define FMT_EOF		1
#define FMT_LITNOMATCH	2
#define FMT_WS		3
#define FMT_LITERAL	4
#define FMT_CHAR	5
#define FMT_STRING	6
#define FMT_INT		7
#define FMT_FLOAT	8
#define FMT_CHARSET	9
#define	FMT_INPUTCNT	10

#define FIF_CONTIN	010
#define FIF_SKIP_WS	004
#define FIF_NEED_CHAR	002
#define FIF_ASSIGNS	001

#define FORMAT_FLAGS	"\000\000\000\014\012\013\017\017\017\013\011"

#if EXCLUDE_FORMAT_IO_INT_FMT && EXCLUDE_FORMAT_IO_DEC_FMT && \
    EXCLUDE_FORMAT_IO_UNS_FMT && EXCLUDE_FORMAT_IO_HEX_FMT && \
    EXCLUDE_FORMAT_IO_OCT_FMT && EXCLUDE_FORMAT_IO_PNT_FMT
	/* No code inserted here */
#else

static int chtol(int ch)	/* conver a character to its value int */
{
	if ((ch <='9') && (ch>='0'))
		return (ch - '0');
	if ((ch <='f') && (ch>='a'))
		return (ch - 'a' + 10);
	if ((ch <='F') && (ch>='A'))
		return (ch - 'A' + 10);
	return (0);
}

#endif	/* EXCLUDE_FORMAT_IO_INT_FMT && .... */

#if EXCLUDE_FORMAT_IO_INT_FMT && EXCLUDE_FORMAT_IO_OCT_FMT
	/* No code inserted here */
#else

static int isodigit(int c)
{
    return (c >= '0') && (c <= '7');
}

#endif	/* (EXCLUDE_FORMAT_IO_INT_FMT) || .... */

#if EXCLUDE_FORMAT_IO_INT_FMT && EXCLUDE_FORMAT_IO_DEC_FMT && \
    EXCLUDE_FORMAT_IO_UNS_FMT
	/* No code inserted here */
#else

static int isdigit(int c)
{
    return (c >= '0') && (c <= '9');
}

#endif	/* EXCLUDE_FORMAT_IO_INT_FMT && .... */

#if EXCLUDE_FORMAT_IO_INT_FMT && EXCLUDE_FORMAT_IO_HEX_FMT && \
    EXCLUDE_FORMAT_IO_PNT_FMT
	/* No code insterted here */
#else

static int isxdigit(int c)
{
    return ((c >= '0') && (c <= '9')) || 
	   ((c >= 'a') && (c <= 'f')) || 
	   ((c >= 'A') && (c <= 'F'));
}

#endif	/* EXCLUDE_FORMAT_IO_INT_FMT && .... */

static int isspace (int c)
{
    return (c == ' ' || (c >= '\t' && c <= '\r'));
}

/****************************************************************/
/* little helper function for getting next character from string */

static int ss_getc(ptr)
char **ptr;
{
    int ch = **ptr;
    if (ch == NUL)
	return EOF;
    ++*ptr;
    return ch;
}

/****************************************************************/


int _scanf(getcfn, ungetcfn, param, fmt, parglist)
register int (*getcfn)(char *);
         int (*ungetcfn)(int, char *);
register char *param;
char	*fmt;
va_list parglist;

{
    char *format_flag = FORMAT_FLAGS;	/* replaces table */
register int in = EMPTY;	/* current character from input stream */
register int inputCnt = 0;	/* # of input chars read so far */
    int fch;		/* current character from format string */
    int format = FMT_WS; /* field attribute: format code */
    int width;		/* field attribute: total width */
    int (*isdigitfn)();	/* field attribute: is-digit? function */
    int base;		/* field attribute: numeric base */
    char set[SETSIZE];	/* set of characters matched by %[, also %f buffer */
    int nmatch = 0;	/* number of matched and assigned fields */
    int ismatch;	/* did this field match? */
    char *pvar;		/* pointer to variable we are assigning to */
    char *inputstr;	/* input string */
    int	start_convrt=0;	/* flag indicated conversion has started */

    PNT_DEF		/* flag to indicated %p format */
    ASSGN_DEF		/* field attribute: assign value? */
    SIZE_DEF		/* field attribute: h=short, l=long, other=int */

    if (getcfn == (int (*)()) NULL) {
	getcfn = (int (*)()) ss_getc;
	inputstr = param;
	param = (char *) &inputstr;
    }
    while (format_flag[format] & FIF_CONTIN) {
	format = FMT_NONE;
	width = 0;
	ismatch = 0;
	ASSGN_EXP (isassigned = 1);
	PNT_EXP (ispointer = 0);
	switch (fch = *fmt++) {
	    case NUL:
	        break;
	    case ' ':
	    case '\t':
	    case '\n':
	        format = FMT_WS;
	        break;
	    case '%':
	        fch = *fmt++;
	        if (fch == '*') {
		    ASSGN_EXP (isassigned = 0);
		    fch = *fmt++;
	        }

	        for (; isdigit(fch); fch = *fmt++)	/* Get field width */
		    width = 10 * width + fch - '0';

	        SIZE_EXP (size = fch);
	        if (OPT_h_EXP (size == 'h') 
			|| OPT_l_EXP (size == 'l') 
			|| OPT_L_EXP (size == 'L'))
		    fch = *fmt++;

	        switch (fch) {
		    case NUL:
			format = FMT_EOF;
			break;
		    case '%':
			format = FMT_LITERAL;
			break;

#if EXCLUDE_FORMAT_IO_CHAR_FMT
	/* No code inserted here */
#else
		    case 'c':
			format = FMT_CHAR;
			break;
#endif	/* EXCLUDE_FORMAT_IO_CHAR_FMT */

#if EXCLUDE_FORMAT_IO_STR_FMT
	/* No code inserted here */
#else
		    case 's':
			format = FMT_STRING;
			break;
#endif	/* EXCLUDE_FORMAT_IO_STR_FMT */

#if EXCLUDE_FORMAT_IO_OCT_FMT
	/* No code inserted here */
#else
		    case 'o':
			format = FMT_INT;
			isdigitfn = isodigit;
			base = 8;
			break;
#endif	/* EXCLUDE_FORMAT_IO_OCT_FMT */

#if EXCLUDE_FORMAT_IO_UNS_FMT && EXCLUDE_FORMAT_IO_DEC_FMT
	/* No code inserted here */
#else

		    case 'u':
		    case 'd':
			format = FMT_INT;
			isdigitfn = isdigit;
			base = 10;
			break;
#endif	/* (EXCLUDE_FORMAT_IO_UNS_FMT) || .... */

#if EXCLUDE_FORMAT_IO_PNT_FMT
	/* No code inserted here */
#else
		    case 'p':
			ispointer++;		/* Set flag then treat as %x */
#endif  /* EXCLUDE_FORMAT_IO_PNT_FMT */

#if EXCLUDE_FORMAT_IO_PNT_FMT && EXCLUDE_FORMAT_IO_HEX_FMT
	/* No code inserted here */
#else
		    case 'X':
		    case 'x':
			format = FMT_INT;
			isdigitfn = isxdigit;
			base = 16;
			break;
#endif	/* EXCLUDE_FORMAT_IO_PNT_FMT && .... */

#if EXCLUDE_FORMAT_IO_INT_FMT
	/* No code inserted here */
#else
		    case 'i':
			format = FMT_INT;
			base = 0;		/* Pick up base from input */
			break;
#endif	/* EXCLUDE_FORMAT_IO_INT_FMT */

#if EXCLUDE_FORMAT_IO_FLOAT_FMT
	/* No code inserted here */
#else
		    case 'e':
		    case 'f':
		    case 'g':
		    case 'E':
		    case 'G':
			format = FMT_FLOAT;
			width = min(width, SETSIZE - 1);
			break;
#endif	/* EXCLUDE_FORMAT_IO_FLOAT_FMT */

#if EXCLUDE_FORMAT_IO_NUMB_FMT
	/* No code inserted here */
#else
		    case 'n':
			format = FMT_INPUTCNT;
			break;
#endif  /* EXCLUDE_FORMAT_IO_NUMB_FMT */

#if EXCLUDE_FORMAT_IO_BRAKT_FMT
	/* No code inserted here */
#else
		    case '[':
			format = FMT_CHARSET;
			{
			    int iscomplement = 0;
			    int i;

			    fch = *fmt++;
			    if (iscomplement = fch == '^')
				fch = *fmt++;
			    for (i = 0; i < SETSIZE; i++)
				set[i] = 0;
			    if (fch == ']') {	/* close bracket first in list*/
				setbit (set, ']');
				fch = *fmt & 0xff;
				fmt++;
			    }
			    for (;fch != NUL && fch != ']'; fch = *fmt++ & 0xFF)
				setbit(set, fch);
			    if (fch == NUL)
				format = FMT_EOF;
			    if (iscomplement)
				for (i = 0; i < SETSIZE; i++)
				    set[i] ^= 0xFF;
			    clrbit(set, NUL);	/* NUL is always terminator */
			}
			break;
#endif	/* EXCLUDE_FORMAT_IO_BRAKT_FMT */

		    }				/* End of small switch */
		    break;
		default:
		    format = FMT_LITERAL;
		    break;
		}				/* End of large switch */

	if (width == 0)
	    switch (format) {

#if EXCLUDE_FORMAT_IO_CHAR_FMT
	/* No code inserted here */
#else
		case FMT_CHAR:
		    width = 1;
		    break;
#endif	/* EXCLUDE_FORMAT_IO_CHAR_FMT */

#if EXCLUDE_FORMAT_IO_STR_FMT && EXCLUDE_FORMAT_IO_HEX_FMT && \
    EXCLUDE_FORMAT_IO_UNS_FMT && EXCLUDE_FORMAT_IO_OCT_FMT && \
    EXCLUDE_FORMAT_IO_DEC_FMT && EXCLUDE_FORMAT_IO_INT_FMT && \
    EXCLUDE_FORMAT_IO_BRAKT_FMT && EXCLUDE_FORMAT_IO_PNT_FMT
	/* No code inserted here */
#else

		case FMT_STRING:
		case FMT_INT:
		case FMT_CHARSET:
		    width = MAXWIDTH;
		    break;
#endif	/* EXCLUDE_FORMAT_IO_STR_FMT && .... */

#if EXCLUDE_FORMAT_IO_FLOAT_FMT
	/* No code inserted here */
#else
		case FMT_FLOAT:
		    width = SETSIZE - 1;
		    break;
#endif	/* EXCLUDE_FORMAT_IO_FLOAT_FMT */
	    }

	if (format_flag[format] & FIF_SKIP_WS) {
	    if (in == EMPTY)
		if (EOF != (in = (*getcfn)(param)))
		    ++inputCnt;
	    while (isspace(in))
		if (EOF != (in = (*getcfn)(param)))
		    ++inputCnt;
		else
		    break;
	}

	if (format_flag[format] & FIF_NEED_CHAR && in == EMPTY)
	    if (EOF != (in = (*getcfn)(param)))
		++inputCnt;

	pvar = format_flag[format] & FIF_ASSIGNS && ASSGN_EXP (isassigned)
		    ? va_arg(parglist, char *)
		    : NULL;

	switch (format) {
	    case FMT_LITERAL:
		if (in == fch)
		    in = EMPTY;
		else
		    format = FMT_LITNOMATCH;
		break;

#if EXCLUDE_FORMAT_IO_CHAR_FMT
	/* No code inserted here */
#else
	    case FMT_CHAR:
		while (in != EOF) {
		    start_convrt = 1;
		    ismatch = 1;
		    if (ASSGN_EXP (isassigned))
			*pvar++ = in;
			getNext;
			if (in == EMPTY)
			    break;
		    }
		break;
#endif	/* EXCLUDE_FORMAT_IO_CHAR_FMT */

#if EXCLUDE_FORMAT_IO_STR_FMT
	/* No code inserted here */
#else
	    case FMT_STRING:
		while (in != EOF && !isspace(in)) {
		    start_convrt = 1;
		    ismatch = 1;
		    if (ASSGN_EXP (isassigned))
			*pvar++ = in;
		    getNext;
		    if (in == EMPTY)
			break;
		    }    
		if (ismatch && ASSGN_EXP (isassigned))	/* Terminate string */
		    *pvar = NUL;
		break;
#endif	/* EXCLUDE_FORMAT_IO_STR_FMT */

#if EXCLUDE_FORMAT_IO_BRAKT_FMT
	/* No code inserted here */
#else
	    case FMT_CHARSET:
		while (in != EOF && isset(set, in)) {
		    start_convrt = 1;
		    ismatch = 1;
		    if (ASSGN_EXP (isassigned))
			*pvar++ = in;
		    getNext;
		    if (in == EMPTY)
			break;
		}
		if (ismatch && ASSGN_EXP (isassigned))	/* Terminate string */
		    *pvar = NUL;
		break;
#endif	/* EXCLUDE_FORMAT_IO_BRAKT_FMT */

#if EXCLUDE_FORMAT_IO_HEX_FMT && EXCLUDE_FORMAT_IO_UNS_FMT && \
    EXCLUDE_FORMAT_IO_OCT_FMT && EXCLUDE_FORMAT_IO_DEC_FMT && \
    EXCLUDE_FORMAT_IO_INT_FMT && EXCLUDE_FORMAT_IO_PNT_FMT
	/* No code inserted here */
#else

	    case FMT_INT:
		{
		    int isminus = 0;		/* Check for negative number */
    		    long num = 0;

		    if (in == '+' || in == '-') {
			isminus = in == '-';
			start_convrt = 1;
			getNext;
		    }

		    if (0) ;	/* allow following conditional compilation */

#if EXCLUDE_FORMAT_IO_INT_FMT
	/* No code inserted here */
#else
		    else if (base == 0) {	/* Handle %i format */
			if (in != '0') {	/* Check for 0 prefix */
			    base = 10;
			    isdigitfn = isdigit;
			}
			else {				/* in == 0 */
			    start_convrt = 1;
			    getNext;
			    if ((in == 'x') || (in == 'X')) {	/* Hex */
				base = 16;
				isdigitfn = isxdigit;
				getNext;
			    }
			    else {
		                ismatch = 1;		/* take care of "0" */
				base = 8;		/* Octal */
				isdigitfn = isodigit;
			    }
			}
		    }
#endif 	/* EXCLUDE_FORMAT_IO_INT_FMT */

#if EXCLUDE_FORMAT_IO_HEX_FMT && EXCLUDE_FORMAT_IO_PNT_FMT
	/* No code inserted here */
#else

		    else if (base == 16) {
			if (in == '0') {	/* look for "0x" */
			    start_convrt = 1;
			    getNext;
		            ismatch = 1;	/* take care of "0" */

			    if ((in == 'x') || (in == 'X')) {/* peel off "0x" */
				getNext;
				if (!(*isdigitfn)(in)) {     /* invalid digit */
				    ismatch = 0;
				    }
				}
			}
		    }
#endif	/* EXCLUDE_FORMAT_IO_HEX_FMT && .... */

		    while ((*isdigitfn)(in)) { 		/* Convert str to # */
			if (BASE_10_EXP (base == 10))	/* for speed */
			    num = (num << 3) + num + num + chtol(in);
			else if (BASE_16_EXP (base == 16))
			    num = (num << 4) + chtol(in);
			else if (BASE_8_EXP (1))
			    num = (num << 3) + chtol(in);
			start_convrt = 1;
		        ismatch = 1;
			getNext;
			}; 

		    if (ASSGN_EXP (isassigned) && ismatch) {
			if (isminus)
			    num = -num;
			if (PNT_EXP(ispointer))	/* handle %p first */
			    * (void **) pvar = (void *) num;
			else if (OPT_h_EXP (size == 'h'))
			    * (short *) pvar = (short) num;
			else if (OPT_l_EXP (size == 'l'))
			    * (long *) pvar = num;
			else
			    * (int *) pvar = (int) num;
		    }
		}
		break;
#endif	/* EXCLUDE_FORMAT_IO_HEX_FMT && .... */

#if EXCLUDE_FORMAT_IO_NUMB_FMT
	/* No code inserted here */
#else
	    case FMT_INPUTCNT:
		start_convrt = 1;
		if (ASSGN_EXP (isassigned)) {
		    if (OPT_h_EXP (size == 'h'))
			* (short *) pvar = (short) computeCount;
		    else if (OPT_l_EXP (size == 'l'))
			* (long *) pvar = (long) computeCount;
		    else
			* (int *) pvar = (int) computeCount;
		}
		break;
#endif  /* EXCLUDE_FORMAT_IO_NUMB_FMT */

#if EXCLUDE_FORMAT_IO_FLOAT_FMT
	/* No code inserted here */
#else
	    case FMT_FLOAT:
		{
		    char *pc = set;
		    int numberp = 0;
		    if (in == '+' || in == '-') {
			*pc++ = in;
			start_convrt = 1;
			getNext;
		    }
		    if (width > 0)
			while (isdigit(in)) {
			    *pc++ = in;
			    numberp = 1;
			    start_convrt = 1;
			    getNext;
			}

		    if (width > 0 && in == '.') {
			*pc++ = in;
			numberp = 1;
			start_convrt = 1;
			getNext;
		    }
		    if (width > 0)
			while (isdigit(in)) {
			    *pc++ = in;
			    numberp = 1;
			    start_convrt = 1;
			    getNext;
			}
		    if (width > 0 && numberp && (in == 'e' || in == 'E')) {
			*pc++ = in;
			numberp = 0;	/* must have #'s after "e" */
			getNext;
			if (width > 0 && (in == '+' || in == '-')) {
			    *pc++ = in;
			    getNext;
			}
			if (width > 0)
			    while (isdigit(in)) {
				numberp = 1;	/* have valid float again */
				*pc++ = in;
				getNext;
			    }
		    }
		    *pc = NUL;
		    if (numberp) {
			ismatch = 1;
			if (ASSGN_EXP (isassigned))
			    _sfef(pvar, set, 
				OPT_l_EXP (size == 'l') || 
				OPT_L_EXP (size == 'L'));
		    }
		}
		break;			/* end of FMT_FLOAT */
#endif	/* EXCLUDE_FORMAT_IO_FLOAT_FMT */

	}				/* end of switch */
	if (ismatch && ASSGN_EXP (isassigned))
	    nmatch++;
	else if ((!ismatch) && (format != FMT_WS) && 
		(format != FMT_LITERAL) && NUMB_EXP(format != FMT_INPUTCNT))
	    break;			/* Stop scan of no match */
    }					/* end of while loop */

    if (in != EOF && in != EMPTY && ungetcfn != (int (*)()) NULL)
	(*ungetcfn)(in, param);

    if ((in == EOF) && (!nmatch) && ((format==FMT_LITNOMATCH) || (!start_convrt)))
	    return EOF;			/* Input error -- no conversion */

    return nmatch;
}

#endif /* EXCLUDE_xscanf */
