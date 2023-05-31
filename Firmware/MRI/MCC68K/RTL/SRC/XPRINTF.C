#include "lib_top.h"


#if EXCLUDE_xprintf
    #pragma option -Qws			/* Prevent compiler warning */
#else

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1991, 1992 Microtec Research, Inc.       */
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

/************************************************************************
 *  xprintf.c
 *		MRI_PRT
 *
 *	Copyright 1986 by Microtec Research Inc.  All rights reserved.
 *
 *	int _mri_prt (stream, format [, arg]...)
 *
 *	mri_prt is the low level printf function.  It is called by printf
 *	eprintf, fprintf, and sprintf.
 *	It returns the number of characters output (excluding \0 for
 *	sprintf), or a negative value if an output error was encountered.
 *
 *  written by H. Yuen  12/12/86
 *
 *  07/01/88 HY - added %i, %p, %n formats; h, L modifiers; +,space flags
 *  08/16/89 jpc - added mrilib.h to aid in building 68k '881 versions
 *  09/22/89 jpc - split from G32 printf function.
 *		   12 -> sizeof (long double) when incrementing arg
 *  12/18/89 jpc - Removed _cgd routine.  Complete long double conversion
 *		   has to be added when compiler supports this.
 *		   Removed the use of "arg" as "char *".  Now uses ANSI
 *		   "va_arg" method of handling variable arguments.
 *  02/13/90 jpc - modified to print alternate forms.  Calling convention
 *		   of ftoa modified.
 *  05/09/90 jpc - calls _ftoa and _ltoa instead of ftoa and ltoa, respectively
 *  06/07/90 jpc - Leading zeros were dropped if # alternate form was used.
 *  06/14/90 jpc - modified to not bring in output routines when only sprintf
 *		   is called.  Address of putc or equiv. passed in as arg.
 *		   Also, for efficiency, as many post-inc and post-dec 
 *		   expressions as reasonably possible were removed.
 *		   #include's for stdio and mriext were removed to eliminate
 *		   reference to _iob.
 *		   reference to BUFSIZ now hard coded as 512
 *  04/04/91 jpc - ltostr -> _ltostr for ANSI compliance
 *  07/19/91 HY  - added #if for creating a subset version
 *  07/25/91 jpc - s -> str
 *		   Modify size of "buf" to reduce stack demands.
 *  11/10/93 jpc - fixed problem w. prec_flag; it was not getting reset
 *		   correctly.
 *  04/08/94 jpc - Fixed spr 34222: did not handle ("%-15.10s", "fred")
 *		   correctly.
 *		   Fixed spr 32202; did not handle ("%.0d", 0) correctly.
 *
 *************************************************************************/
/* (last mod 07/01/88  HY) */

#if _MCCH85
    #pragma option -nKl		/* Turn off library checking */
#endif

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <mriext.h>
#include "mrilib.h"

#define	NDIG	310	/* max # digits for 'f' format */
			/* (stolen from xftoa.c 07/25/91) */

#ifdef EXCLUDE_FORMAT_IO_MINUS_FLAG
    #informing -- The "-" flag has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_PLUS_FLAG
    #informing -- The "+" flag has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_SPACE_FLAG
    #informing -- The " " flag has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_SHARP_FLAG
    #informing -- The "#" flag has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_ZERO_FLAG
    #informing -- The "0" flag has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_h_OPT
    #informing -- The "h" option has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_l_OPT
    #informing -- The "l" option has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_L_OPT
    #informing -- The "L" option has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_STAR_OPT
    #informing -- The "*" option has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_INT_FMT
    #informing -- The "i" format has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_DEC_FMT
    #informing -- The "d" format has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_OCT_FMT
    #informing -- The "o" format has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_UNS_FMT
    #informing -- The "u" format has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_HEX_FMT
    #informing -- The "x" format has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_FLOAT_FMT
    #informing -- The "f", "e", and "g" formats has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_CHAR_FMT
    #informing -- The "c" format has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_STR_FMT
    #informing -- The "s" format has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_PNT_FMT
    #informing -- The "p" format has been disabled.
#endif

#ifdef EXCLUDE_FORMAT_IO_NUMB_FMT
    #informing -- The "n" format has been disabled.
#endif

#ifndef EXCLUDE_FORMAT_IO_MINUS_FLAG
    #define LEFTJ_EXP(x)	(x)
    #define LEFTJ_DCL		char leftj;
#else
    #define LEFTJ_EXP(x)	(0)
    #define LEFTJ_DCL
#endif

#ifndef EXCLUDE_FORMAT_IO_PLUS_FLAG
    #define PLUS_EXP(x)	(x)
    #define PLUS_DCL		char plus;
#else
    #define PLUS_EXP(x)	(0)
    #define PLUS_DCL
#endif		/* ! EXCLUDE_FORMAT_IO_PLUS_FLAG */

#ifndef EXCLUDE_FORMAT_IO_SPACE_FLAG
    #define SPACE_EXP(x)	(x)
    #define SPACE_DCL		char space;
#else
    #define SPACE_EXP(x)	(0)
    #define SPACE_DCL
#endif		/* ! EXCLUDE_FORMAT_IO_SPACE_FLAG */

#ifndef EXCLUDE_FORMAT_IO_SHARP_FLAG
    #define ALT_EXP(x)		(x)
    #define ALT_DCL		char alt;
#else
    #define ALT_EXP(x)		(0)
    #define ALT_DCL
#endif		/* ! EXCLUDE_FORMAT_IO_SHARP_FLAG */


#ifndef EXCLUDE_FORMAT_IO_ZERO_FLAG
    #define PADCHAR_EXP(x)	(x)
    #define PADCHAR_CHAR(x)	(x)
    #define PADCHAR_DCL	char padchar;
#else
    #define PADCHAR_EXP(x)	(0)
    #define PADCHAR_CHAR(x)	(' ')
    #define PADCHAR_DCL
#endif		/* ! EXCLUDE_FORMAT_IO_ZERO_FLAG */

#ifndef EXCLUDE_FORMAT_IO_h_OPT
    #define SHORTARG_EXP(x)	(x)
    #define SHORTARG_DCL	char shortarg;
#else
    #define SHORTARG_EXP(x)	(0)
    #define SHORTARG_DCL
#endif		/* ! EXCLUDE_FORMAT_IO_h_OPT */

#ifndef	EXCLUDE_FORMAT_IO_l_OPT
    #define LONGARG_EXP(x)	(x)
    #define LONGARG_DCL	char longarg;
#else
    #define LONGARG_EXP(x)	(0)
    #define LONGARG_DCL
#endif		/* ! EXCLUDE_FORMAT_IO_l_OPT */


#ifndef EXCLUDE_FORMAT_IO_L_OPT
    #define LONGDBL_EXP(x)	(x)
    #define LONGDBL_DCL	char longdbl;
#else
    #define LONGDBL_EXP(x)	(0)
    #define LONGDBL_DCL	
#endif		/* ! EXCLUDE_FORMAT_IO_L_OPT */

#if _SIZEOF_DATA_POINTER == 2
	#define	GET_POINTER	\
		((unsigned long)(unsigned short)(va_arg (arg, void *)))
#else
	#define	GET_POINTER	((unsigned long)(va_arg (arg, void *)))
#endif

#define TRANS(x,y)	{if (((*trans)((x),(y))) == (EOF)) return EOF;}
			/* Return EOF on error from put function */



int	_mri_prt(void *fp, register char *fmt, va_list arg, 
		int (*trans) (int ch, void *stream))
{
    register char *str;	/* string ptr for output */
    long tempLong;	/* put arg for zero test for alt. format */
    int  width;		/* field width */
    int  prec;		/* precision */
    int  n, len;
    int  nc;		/* # chars output */
    char c;		/* format char */
    char buf[NDIG+5];	/* temp buffer for output */
			/* used to be coded as "BUFSIZ" */
    int	 pad_field;	/* count used to shift "prec" inside "field" */
    int  prefix_cnt;	/* used to indicate -, +, ' ', or '0x' prefix */
    int  prec_flag;	/* used to indicated that precision was specified */
    int	 char_flag;	/* use to indicate %s or %c conversion */
    SHORTARG_DCL	/* true if arg is short ('h') */
    LONGARG_DCL		/* true if arg is long  ('l') */
    LONGDBL_DCL		/* true if 'L' flag */
    LEFTJ_DCL		/* true if left-justified */
    PLUS_DCL		/* true if to print +/- for signed value */
    SPACE_DCL		/* prepend ' ' if needed for signed value */
    ALT_DCL		/* true if to use alternate form ('#') */
    PADCHAR_DCL		/* for padding */

    nc = 0;

    while (c = *fmt) {
	fmt++;
	if (c != '%') {
		TRANS (c,fp);
		nc++;
	}
	else {
		prefix_cnt = 0;			/* reset */
		char_flag = 0;			/* reset */
		LEFTJ_EXP   (leftj = 0);	/* default is right-justified */
		PLUS_EXP    (plus = 0);
		SPACE_EXP   (space = 0);
		ALT_EXP     (alt = 0);
		PADCHAR_EXP (padchar = ' ');	/* default is blank padding */

		for (;;) {		/* ANSI says flags in any order */
		    switch (*fmt) {
#ifndef EXCLUDE_FORMAT_IO_MINUS_FLAG
			case '-' :
			    leftj++;	/* to be left-justified */
			    PADCHAR_EXP(padchar = ' ');	/* suppress 0 pad */
		            fmt++;
			    continue;
#endif	/* EXCLUDE_FORMAT_IO_MINUS_FLAG */

#ifndef EXCLUDE_FORMAT_IO_ZERO_FLAG
		    	case '0' :		/* cannot be both - and 0 */
		            if (LEFTJ_EXP (leftj) == 0)
			        PADCHAR_EXP(padchar = '0');/*to be zero filled*/
		            fmt++;
			    continue;
#endif	/* EXCLUDE_FORMAT_IO_ZERO_FLAG */

#ifndef EXCLUDE_FORMAT_IO_PLUS_FLAG
		        case '+' :		/* print +/- for signed value */
		            plus++;
			    SPACE_EXP (space = 0);	/* suppress space */
		            fmt++;
			    continue;
#endif	/* EXCLUDE_FORMAT_IO_PLUS_FLAG */

#ifndef EXCLUDE_FORMAT_IO_SPACE_FLAG
			case ' ' :		/* cannot be both + and ' ' */
			    if (PLUS_EXP (plus) == 0)
				space++;
			    fmt++;
			    continue;
#endif	/* EXCLUDE_FORMAT_IO_SPACE_FLAG */

#ifndef EXCLUDE_FORMAT_IO_SHARP_FLAG
			case '#' :
			    alt++;		/* use alternate form */
			    fmt++;
			    continue;
#endif	/* EXCLUDE_FORMAT_IO_SHARP_FLAG */
		    }
		    break;		/* break loop if no flag */
		}			/* for (;;) */

#ifndef	EXCLUDE_FORMAT_IO_STAR_OPT
		if (*fmt == '*') {
		    width = va_arg (arg, int);	/* get width from arg */
		    if (width < 0) {
			width = -width;		/* -ve width means left-j */
			LEFTJ_EXP (leftj++);
		    }
		    fmt++;
		}
		else
#endif	/* EXCLUDE_FORMAT_IO_STAR_OPT */
		{
		    width = 0;
		    while (*fmt >= '0' && *fmt <= '9') {
			width = width * 10 + (*fmt - '0');
			fmt++;
			}
		}
		prec = -1;		/* no precision yet */
		prec_flag = 0;		/* reset */
		if (*fmt == '.') {
		    prec_flag = 1;	/* indicate a precision was specified */
		    fmt++;
#ifndef EXCLUDE_FORMAT_IO_STAR_OPT
		    if (*fmt == '*') {
			prec = va_arg (arg, int);	/* get prec from arg */
			fmt++;
		    }
		    else
#endif	/* EXCLUDE_FORMAT_IO_STAR_OPT */
		    {
			prec = 0;
			while (*fmt >= '0' && *fmt <= '9') {
			    prec = prec * 10 + (*fmt - '0');
			    fmt++;
			    }
		    }
		}
		SHORTARG_EXP (shortarg = 0);	/* set defaults */
		LONGARG_EXP  (longarg = 0);
		LONGDBL_EXP  (longdbl = 0);

#ifndef EXCLUDE_FORMAT_IO_l_OPT			/* set option flags */
		if (*fmt == 'l') {
		    longarg++;
		    fmt++;
		    goto opt_fin;
		}
#endif	/* EXCLUDE_FORMAT_IO_l_OPT */

#ifndef EXCLUDE_FORMAT_IO_h_OPT
		if (*fmt == 'h') {
		    shortarg++;
		    fmt++;
		    goto opt_fin;
		}
#endif	/* EXCLUDE_FORMAT_IO_h_OPT */

#ifndef	EXCLUDE_FORMAT_IO_L_OPT
		if (*fmt == 'L') {
		    longdbl++;
		    fmt++;
		    goto opt_fin;
		}
#endif	/* EXCLUDE_FORMAT_IO_L_OPT */

	    opt_fin:
		str = buf;
		switch (c = *fmt++) {

#ifndef EXCLUDE_FORMAT_IO_STR_FMT
		case 's':			/* string */
			char_flag = 1;		/* character-type conversion */
			str = va_arg (arg, char *);
			if (str == 0)
			    str = "(null)";
			len = strlen(str);
			if (prec < len && prec >= 0)	/* limit string */
			    len = prec;
			break;
#endif	/* EXCLUDE_FORMAT_IO_STR_FMT */

#ifndef EXCLUDE_FORMAT_IO_CHAR_FMT
		case 'c':			/* character */
			char_flag = 1;		/* character-type conversion */
			buf[0] = va_arg (arg, int);  /* char is passed as int */
			buf[1] = '\0';
			len = 1;
			break;
#endif	/* EXCLUDE_FORMAT_IO_CHAR_FMT */

#if (! EXCLUDE_FORMAT_IO_INT_FMT) || (! EXCLUDE_FORMAT_IO_DEC_FMT)
		case 'd':			/* signed int */
		case 'i':
		    {
		    unsigned long long_value;

			if (prec_flag)		/* no pad char if prec speced*/
			    PADCHAR_EXP (padchar = ' ');

			if (LONGARG_EXP (longarg)) {
			    long_value = va_arg (arg, long);
			} else if (SHORTARG_EXP (shortarg)) {
			    long_value = (long) (short) va_arg (arg, int);
			} else {
			    long_value = (long) va_arg (arg, int);
			}
			
			if ((long_value == 0) && (prec == 0)) {
			    len = 0;		/* print nothing */
			    *str = '\0';	/* per ANSI */
			}
			else {
			    len = _ltoa(long_value, ++str);

			    if (*str != '-' && 
			    		(PLUS_EXP(plus)||SPACE_EXP(space))) {
				*--str = PLUS_EXP (plus) ? '+' : ' ';
				len++;
			    }
			}
		    }
#endif	/* (! EXCLUDE_FORMAT_IO_INT_FMT) || (! EXCLUDE_FORMAT_IO_DEC_FMT) */

#if 	(!EXCLUDE_FORMAT_IO_INT_FMT)||(! EXCLUDE_FORMAT_IO_DEC_FMT)|| \
	(!EXCLUDE_FORMAT_IO_OCT_FMT)||(! EXCLUDE_FORMAT_IO_UNS_FMT)||\
	(!EXCLUDE_FORMAT_IO_HEX_FMT)||(! EXCLUDE_FORMAT_IO_PNT_FMT)
		  yy:
			if ((*str=='-') || (*str=='+') || (*str==' ')) {
			    prefix_cnt = 1;
			}
			if (prec >= 0) {
			    if (prec < (len - prefix_cnt))
				prec = len - prefix_cnt;
			    if (width < (prec + prefix_cnt))
				width = prec + prefix_cnt;
			}
			break;
#endif /* (! EXCLUDE_FORMAT_IO_INT_FMT) || ....  */

#ifndef EXCLUDE_FORMAT_IO_UNS_FMT
		case 'u':			/* unsigned int */
			n = 10;
			goto xx;
#endif	/* EXCLUDE_FORMAT_IO_UNS_FMT */

#ifndef EXCLUDE_FORMAT_IO_OCT_FMT
		case 'o':			/* octal format */
			n = 8;
			goto xx;
#endif	/* EXCLUDE_FORMAT_IO_OCT_FMT */

#ifndef	EXCLUDE_FORMAT_IO_HEX_FMT
		case 'X':			/* hex (upper case ABCDEF) */
		case 'x':			/* hex (lower case abcdef) */
			n = 16;
#endif	/* EXCLUDE_FORMAT_IO_HEX_FMT */

#if	(! EXCLUDE_FORMAT_IO_OCT_FMT) || (! EXCLUDE_FORMAT_IO_HEX_FMT) || \
	(! EXCLUDE_FORMAT_IO_UNS_FMT)
		    xx:
			if (prec_flag)		/* no pad char if prec spec'ed*/
			    PADCHAR_EXP (padchar = ' ');

			tempLong = LONGARG_EXP (longarg) ?
			    va_arg (arg, long) :
			    (SHORTARG_EXP (shortarg) ? 
				((long)(unsigned short) va_arg(arg, unsigned)) :
				((long) va_arg (arg, unsigned)));

			if ((tempLong == 0) && (prec == 0)) {
			    len = 0;			/* print nothing */
			    *str = '\0';
			} 
			else {
			    len = _ltostr (tempLong, str += 2, n);

			    if (ALT_EXP (alt) && (n != 10)) {
			        if ((n == 8) && (tempLong != 0)) {
				    if (prec <= len) {
				        prec = len+1;	/* alt format in octal*/
				    }
			        }
			        else {
				    if ((n == 16) && (tempLong)) {
				        prefix_cnt = 2;	    /* hex alt format */
				        len += 2;
				        str -= 2;
				        *str = '0';
				        *(str + 1) = 'X';
				    }
			        }
			    }

			    if (c == 'x') {
			        char *tempStr = str;

			        for ( ; *tempStr; tempStr++)
				    if (*tempStr >= 'A')
				        *tempStr += 'a' - 'A';  /* lower case */
			    }
			}
			goto yy;
#endif        /* (! EXCLUDE_FORMAT_IO_OCT_FMT) || .... */

#ifndef       EXCLUDE_FORMAT_IO_FLOAT_FMT
		case 'E':			/* E format */
		case 'e':			/* e format */
		case 'f':			/* f format */
		case 'G':			/* G format */
		case 'g':			/* g format */
						/* cgd no longer called here */
			len = _ftoa (va_arg (arg, double), ++str,
			    c | (ALT_EXP(alt) ? (char) 0x80 : (char) 0) , prec);
							/* hide alt in "fmt" */

			if (*str == '-') {
			    prefix_cnt = 1;
			} else {
			    if (PLUS_EXP(plus)||SPACE_EXP(space)) {
				*--str = PLUS_EXP (plus) ? '+' : ' ';
				len++;
				prefix_cnt = 1;
			    }
			}

			prec = -1;
			break;
#endif	/* ! EXCLUDE_FORMAT_IO_FLOAT_FMT */

#ifndef EXCLUDE_FORMAT_IO_PNT_FMT
		case 'p':			/* pointer */
			len = _ltostr (GET_POINTER, str, 16);
			goto yy;
#endif	/* ! EXCLUDE_FORMAT_IO_PNT_FMT */

#ifndef	EXCLUDE_FORMAT_IO_NUMB_FMT
		case 'n':			/* write current nc */
			if (LONGARG_EXP (longarg))
			    *(va_arg (arg, long *))  = nc;
			else if (SHORTARG_EXP (shortarg))
			    *(va_arg (arg, short *)) = nc;
			else
			    *(va_arg (arg, int *))   = nc;
			continue;		/* no output */
#endif	/* ! EXCLUDE_FORMAT_IO_NUMB_FMT */

		default:
			TRANS (c,fp);
			nc++;
			continue;
		}

		if ((n = width - len) > 0)	/* extra space */
		    nc += n;

		if (!LEFTJ_EXP(leftj)) {	/* if not left-justified */

		    pad_field = width - max (prec + prefix_cnt, len);

		    if (PADCHAR_EXP (padchar == '0') && (len > 0)) {
			while (prefix_cnt-- > 0) {
			    TRANS (*str,fp);	/* output char if needed */
			    str++;
			    nc++;
			    len--;
			}
		    }

		    for (++pad_field; --pad_field > 0;) {	/* Pad if */
			TRANS (PADCHAR_CHAR(padchar), fp);    /* necessary */
			--n;
		    }

		    if (PADCHAR_EXP (padchar == ' ') && (len > 0)) {
			while (prefix_cnt-- > 0) {
			    TRANS (*str,fp);	/* output char if needed */
			    str++;		/* for situation driven by */
			    nc++;		/* precision */
			    len--;
			}
		    }

		    for (++n; --n > 0;)		/* pad */
			TRANS (char_flag ? ' ': '0', fp);
		}

		nc += len;

		if (len > 0){
		    while (prefix_cnt-- > 0) {
			TRANS (*str,fp);	/* output char if needed */
			str++;			/* for situation driven by */
			len--;			/* precision */
		    }
		}

		if ((n > 0) && (char_flag == 0)) {/* only true for left just. */
		    pad_field = prec - len;	/* leading zeros ? */
		    for (++pad_field; --pad_field > 0;) {
			TRANS ('0', fp);
			--n;			/* adjust counter */
		    }
		}

		for (++len; --len > 0;) {
		    TRANS (*str,fp);
		    str++;
		    }

		for (++n; --n > 0;)		/* Pad on right w. blanks */
		    TRANS (' ', fp);
	}
    }

    return (nc);
}

#endif /* EXCLUDE_xprintf */

