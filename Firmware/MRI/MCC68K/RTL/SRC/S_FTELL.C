#include "lib_top.h"


#if EXCLUDE_s_ftell
    #pragma option -Qws			/* Prevent compiler warning */
#else
 

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) Microtec Research, Inc. 1986-91, 1992.			*/
/* All rights reserved.							*/
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

/****************************************************************************/
/*									    */
/*  long int ftell (FILE *stream);					    */
/*									    */
/*  Return current file position.					    */
/*									    */
/*  ftell:								    */
/*	Return the current byte offset to the file "stream".  If an error   */
/*	occurs, -1 is returned.						    */
/*									    */
/****************************************************************************/

#include	<stdio.h>
#include	"cxx_targ.h"

/************************************************************************/
/* Do not generated warning for targets supporting C++ or for mcc386.	*/
/* mcc386's builder/binders treat unresolved references as errors and	*/
/* will not generate executables.					*/
/************************************************************************/

#if ! (CXX_AVAIL || _MCC386)

    #define	GEN_WARNING	1		

#endif

long int ftell (FILE *stream)
{
#if GEN_WARNING

	extern	_WARNING_ftell_stub_used();
	int (* volatile stub)() = _WARNING_ftell_stub_used;

	_rtl_stub_msg("ftell");

#endif

	return 0;
}
 
#endif /* EXCLUDE_s_ftell */
 
