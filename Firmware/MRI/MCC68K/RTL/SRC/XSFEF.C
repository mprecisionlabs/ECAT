#include "lib_top.h"


#if EXCLUDE_xsfef
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

/* Copyright 1986, Hunter & Ready Inc. */

/************************************************************************/
/*	History:							*/
/*	09/16/89 jpc	Added mrilib.h for easy '881 build		*/
/*									*/
/************************************************************************/

#include <math.h>
#include <stdlib.h>
#include "mrilib.h"

/* Internal routine called by xscanf to handle %e and %f formats.
    pvar -- pointer to the variable to assign to.
    str -- pointer to the character string to convert.
    isdouble -- assign to a double instead of a float.
*/
void _sfef(pvar, str, isdouble)
char *pvar;
char *str;
int isdouble;
{
    double dval = atof(str);
    if (isdouble)
	* (double *) pvar = dval;
    else
	* (float *) pvar = (float) dval;
}

#endif /* EXCLUDE_xsfef */

