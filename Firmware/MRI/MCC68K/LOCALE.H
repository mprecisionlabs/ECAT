
/************************************************************************/
/* Copyright (c) 1988-1989,1991  Microtec Research, Inc.		*/
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

/*  <locale.h>   Microtec Research, Inc. ANSI C Compiler  */
/* %W% %G% */


#ifndef __LOCALE_H
#define __LOCALE_H	1

struct lconv
	{
	char *decimal_point;
	char *thousands_sep;
	char *grouping;
	char *int_curr_symbol;
	char *currency_symbol;
	char *mon_decimal_point;
	char *mon_thousands_sep;
	char *mon_grouping;
	char *positive_sign;
	char *negative_sign;
	char int_frac_digits;
	char frac_digits;
	char p_cs_precedes;
	char p_sep_by_space;
	char n_cs_precedes;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
	};

#if __cplusplus
    #define	NULL		(0)
#else
    #define	NULL		((void *)0)
#endif

#define LC_ALL		0
#define LC_COLLATE	1
#define LC_CTYPE	2
#define LC_MONETARY	3
#define LC_NUMERIC	4
#define LC_TIME		5

#if __cplusplus 
    extern "C" {
#endif

extern char *setlocale(int, const char *);
extern struct lconv *localeconv(void);

#if __cplusplus
    }
#endif

#endif /* end __LOCALE_H */
