#include "lib_top.h"


#if EXCLUDE_s_open
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


/*	%W% %G% */

/************************************************************************
*                                                                       *
*  int open (char *filename, int flags, int mode);			*
*                                                                       *
*  open file								*
*                                                                       *
*  int open(filename, flags, mode) char *filename; int flags, mode;	*
*	Opens the specified file, depending on the indicated type,	*
*	the types are as follows:					*
*									* 
*	O_RDONLY        0x0000          open for read only		*
*	O_WRONLY        0x0001          open for write only		*
*	O_RDWR          0x0002          open for read and write		*
*	O_APPEND        0x0008          writes performed at EOF		*
*	O_CREAT         0x0200          create file			*
*	O_TRUNC         0x0400          truncate file			*
*	O_EXCL		0x0800		file must not pre-exist		*
*	O_FORM          0x4000          text file			*
*	O_BINARY        0x8000          binary file			*
*									* 
*	The value of mode is always set to 0744 for UNIX compatibility.	*
*	Open should return a file descriptor (or "file handle") for	*
*	the file it opens.  On failure it should return -1.		*
*									* 
*	This stub returns the smallest available file descriptor.	*
*									*
************************************************************************/

#include <stdio.h>			/* Get FOPEN_MAX */
#include "cxx_targ.h"

#define O_RDONLY        0x0000          /* open for reading only */
#define O_WRONLY        0x0001          /* open for writing only */
#define O_RDWR          0x0002          /* open for reading and writing */
#define O_APPEND        0x0008          /* writes performed at end of file */
#define O_CREAT         0x0200          /* create file if necessary */
#define O_TRUNC         0x0400          /* truncate file */
#define	O_EXCL		0x0800		/* file must not pre-exist */
					/* for compatibility w. C++ libraries */
#define O_FORM          0x4000          /* text file */
#define O_BINARY        0x8000          /* binary file */

/************************************************************************/
/* Do not generate warning for targets supporting C++ or for mcc386.    */
/* mcc386's builder/binders treat unresolved references as errors and   */
/* will not generate executables.                                       */
/************************************************************************/

#if ! (CXX_AVAIL || _MCC386)

    #define     GEN_WARNING     1

#endif


int open (filename, flags, mode)
char *filename;
int  flags;
int  mode;
{
int i;
int track_array[FOPEN_MAX];

#if GEN_WARNING

    extern  _WARNING_open_stub_used();	/* generate error at link time */
    int (* volatile stub)() = _WARNING_open_stub_used;

    _rtl_stub_msg("open");	/* generate run-time error if stub used */

#endif

    for (i=0; i<FOPEN_MAX; i++)	/* preset to zero */
	track_array[i] = 0;

    for (i=0; i<FOPEN_MAX; i++)	/* set used file descriptors to 1 */
	if (_iob[i]._flag)
	    track_array[_iob[i]._file] = 1;

    for (i=0; i<FOPEN_MAX; i++)	/* return lowest available file descriptor */
	if (track_array[i] == 0)
	    return i;

    return -1;			/* could not find a file descriptor */
}
 
#endif /* EXCLUDE_s_open */
 
