#include "lib_top.h"


#if EXCLUDE_s_close
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

/************************************************************************
*                                                                       *
*  int close (fno) int fno;						*
*                                                                       *
*  close file								*
*                                                                       *
*  close:								*
*	Closes the file associated with the file descriptors fno	*
*	(returned by open() or creat()).  Should return 0 if all goes	*
*	well or -1 on error.						*
*									* 
************************************************************************/

int close (fno)
int  fno;
{
    return 0;
}
 
#endif /* EXCLUDE_s_close */
 
