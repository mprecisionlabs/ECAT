#include "lib_top.h"


#pragma	option	-nx

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1993 Microtec Research, Inc.				    */
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
/* NAME                      0 h u g e v a l				*/ 
/*                                                                      */
/* FUNCTION     0hugeval - Test HUGE_VAL macro.				*/
/*                                                                      */
/* DESCRIPTION                                                          */
/*		Compare value returned by HUGE_VAL against <inf>	*/
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*	01/18/93 jpc:	written.					*/
/*                                                                      */
/************************************************************************/

#pragma	option	-Qws		/* Kill warning for 1.e310 */

#include <stdio.h>
#include <math.h>
#include "message.h"

main () {

    if (HUGE_VAL != 1.e310) {
	puts ("HUGE_VAL != <inf>");
	printf (ERRORS);
    } else {
	printf (NO_ERRORS);
    }

}
