#include "lib_top.h"


#if EXCLUDE_m_misc || _80960SB || _80960KB || _80960MC
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.mathmisc.c 1.11
 * COPYRIGHT  Hewlett-Packard Company  1987, 1988
 */

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/*----------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.                     */
/* All rights reserved							*/
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


/*	%W% %G%	*/

/************************************************************************/
/*	History:							*/
/*	09/10/89 jpc:	Added #if ! _MCC68K to remove definition of 	*/
/*			_fp_* for 68K.  				*/
/*	04/11/90 jpc:	added _MCC88K to #if's				*/
/*	05/21/90 jpc:   made _fp_status & _fp_control non-C-common	*/
/*			assign them to zero				*/
/*	01/29/91 jpc:	added _MCCSP to #if's				*/
/*	02/15/91 jpc:	added _MCC80 to #if's				*/
/*	04/01/91 jpc:	removed declarations for fp_cntrl fp_status	*/
/************************************************************************/

#include "m_incl.h"

const UInt32  _hex_NaN[2] = HD_INIT(0x7FFFFFFF, 0xFFFFFFFF);
const UInt32  _hex_NaNf   = 0x7FFFFFFF;
 
#endif /* EXCLUDE_m_misc */
 
