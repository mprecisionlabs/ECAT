#include "lib_top.h"


#if EXCLUDE_cxxconde
    #pragma option -Qws			/* Prevent compiler warning */
#else
 

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.			    */
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

/****************************************************************************/
/*  _cxxfini:	Stub for use in C libraries				    */
/*									    */
/*  NOTE:	These routines were written to function in linear address   */
/*		spaces.  If they are to be used on a paged or segmented	    */
/*		target, __cxxfini may have to be modified to prevent	    */
/*		caused by the possiblity of address wrapping		    */
/*									    */
/*  HISTORY:	02/19/92 jpc:	converted to stub for C libraries	    */
/****************************************************************************/

int _cxxfini () 
{
    return;
}
 
#endif /* EXCLUDE_cxxconde */
 
