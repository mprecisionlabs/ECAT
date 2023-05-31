#include "lib_top.h"


#if EXCLUDE_s_unlink
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


/*	%W% %G%	*/

/****************************************************************************/
/*									    */
/*	This routine is simply a stub to allow correct linking		    */
/*									    */
/*	The final version must set "errno" when an error occurs		    */
/*									    */
/****************************************************************************/

#include "cxx_targ.h"

/************************************************************************/
/* Do not generated warning for targets supporting C++ or for mcc386.   */
/* mcc386's builder/binders treat unresolved references as errors and   */
/* will not generate executables.                                       */
/************************************************************************/

#if ! (CXX_AVAIL || _MCC386)

    #define     GEN_WARNING     1

#endif


int  unlink (const char *filename)
{
#if GEN_WARNING

	extern	_WARNING_unlink_stub_used();
	int (* volatile stub)() = _WARNING_unlink_stub_used;

	_rtl_stub_msg("unlink");

#endif
	return 0;
}
 
#endif /* EXCLUDE_s_unlink */
 
