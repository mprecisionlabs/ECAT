
/************************************************************************/
/* Copyright (c) 1988-1989  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

/*  <locale.h>   Microtec Research, Inc. ANSI C Compiler  */
/* @(#)locale.h	1.3 4/18/90 */


#ifndef __LOCALE_H
#define __LOCALE_H

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

#define NULL		((void*)0)

#define LC_ALL		0
#define LC_COLLATE	1
#define LC_CTYPE	2
#define LC_MONETARY	3
#define LC_NUMERIC	4
#define LC_TIME		5

#if __cplusplus 
extern "C" {
#endif

extern char *setlocale(int category, const char *locale);
extern struct lconv *localeconv(void);

#if __cplusplus
}
#endif

#endif /* end __LOCALE_H */
