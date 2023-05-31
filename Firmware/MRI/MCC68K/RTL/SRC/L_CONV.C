#include "lib_top.h"


#if EXCLUDE_l_conv
    #pragma option -Qws			/* Prevent compiler warning */
#else
 

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.                         */
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


/************************************************************************/
/*									*/
/*	struct lconv *localeconv (void);				*/
/*									*/
/*	returns:  address of structure containing numeric formatting	*/
/*		  information.  This implementation only supports 'C'	*/
/*		  locale, so all elements except decimal point either	*/
/*		  point to a null string or to CHAR_MAX.		*/
/*									*/
/*	History --							*/
/*		07/18/91 jpc -	Written					*/
/************************************************************************/
/*	%W% %G%	*/

#include <locale.h>
#include <limits.h>

static struct lconv lconv_data; 

struct lconv *localeconv (void)
{
  lconv_data.decimal_point = "."; 
  lconv_data.thousands_sep = "";
  lconv_data.grouping = "";
  lconv_data.int_curr_symbol = "";
  lconv_data.currency_symbol = "";
  lconv_data.mon_decimal_point = "";
  lconv_data.mon_thousands_sep = "";
  lconv_data.mon_grouping = "";
  lconv_data.positive_sign = "";
  lconv_data.negative_sign = "";
  lconv_data.int_frac_digits = CHAR_MAX;
  lconv_data.frac_digits = CHAR_MAX;
  lconv_data.p_cs_precedes = CHAR_MAX;
  lconv_data.p_sep_by_space = CHAR_MAX;
  lconv_data.n_cs_precedes = CHAR_MAX;
  lconv_data.n_sep_by_space = CHAR_MAX;
  lconv_data.p_sign_posn = CHAR_MAX;
  lconv_data.n_sign_posn = CHAR_MAX;

    return (struct lconv *) & lconv_data;
}
 
#endif /* EXCLUDE_l_conv */
 
