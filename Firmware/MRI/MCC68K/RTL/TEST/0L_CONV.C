#include "lib_top.h"


#pragma	option	-nx

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) 1991-1992  Microtec Research, Inc.			*/
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


/************************************************************************/
/*	Routine to test localeconv()					*/
/************************************************************************/
/*	%W% %G%	*/

#include <locale.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include "message.h"

main ()
{
struct lconv *local_lconv;

local_lconv = localeconv();		/* call routine */

if (	strcmp (local_lconv -> decimal_point, ".")	||
	strcmp (local_lconv -> thousands_sep, "")	||
	strcmp (local_lconv -> grouping	    , "")	||
	strcmp (local_lconv -> int_curr_symbol, "")	||
	strcmp (local_lconv -> currency_symbol, "")	||
	strcmp (local_lconv -> mon_decimal_point, "")	||
	strcmp (local_lconv -> mon_grouping, "")	||
	strcmp (local_lconv -> positive_sign, "")	||
	strcmp (local_lconv -> negative_sign, "")	||
	(local_lconv -> int_frac_digits != CHAR_MAX)	||
	(local_lconv -> frac_digits != CHAR_MAX)	||
	(local_lconv -> p_cs_precedes != CHAR_MAX)	||
	(local_lconv -> p_sep_by_space != CHAR_MAX)	||
	(local_lconv -> n_cs_precedes != CHAR_MAX)	||
	(local_lconv -> n_sep_by_space != CHAR_MAX)	||
	(local_lconv -> p_sign_posn != CHAR_MAX)	||
	(local_lconv -> n_sign_posn != CHAR_MAX)
	) {

	puts ("Error:  invalid data in lconv structure.");
	printf (ERRORS);
	exit (1);
	}

printf (NO_ERRORS);
exit (0);
}

 
