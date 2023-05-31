#include "lib_top.h"


#if EXCLUDE_s_creat
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
*  int creat (filename, prot) char *filename, int prot;			*
*                                                                       *
*  open file, create file if necessary					*
*                                                                       *
*  creat:								*
*	Creates and opens (for writing) a file named filename with	*
*	protection as specified by prot.  For things like terminals	*
*	that cannot be created it should just open the device.  Creat	*
*	should return a file descriptor (or "file handle") for the	*
*	file it opens.  On failure it should return -1.			*
*									* 
************************************************************************/

#define		O_WRONLY	0x0001		/* open for write only */
#define		O_CREAT		0x0200		/* create file */
#define		O_TRUNC		0x0400		/* truncate file */
#define		O_FORM		0x4000		/* text file */

creat (filename, mode)
char *filename;
int mode;
{
    return (open (filename, O_WRONLY | O_CREAT | O_TRUNC | O_FORM, mode));
}
 
#endif /* EXCLUDE_s_creat */
 
