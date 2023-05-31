#include "lib_top.h"


#if EXCLUDE_strerror
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

/************************************************************************
 *  strerror.c
 *
 *	Maps an error number to an error message string.
 *
 *  The error numbers are defined in <errno.h> and also compatiable
 *  with UNIX System V.
 *
 *  Written by H. Yuen  03/13/91
 *
 ************************************************************************/
/* (last mod 03/14/91  HY) */


#define	WIDTH	30


static char const sys_errlist[][WIDTH] = {
	/* EZERO	0  */	"No error",
	/* EPERM	1  */	"Not owner",
	/* ENOENT	2  */	"No such file or directory",
	/* ESRCH	3  */	"No such process",
	/* EINTR	4  */	"Interrupted system call",
	/* EIO		5  */	"I/O error",
	/* ENXIO	6  */	"No such device or address",
	/* E2BIG	7  */	"Arg list too long",
	/* ENOEXEC	8  */	"Exec format error",
	/* EBADF	9  */	"Bad file number",
	/* ECHILD	10 */	"No child processes",
	/* EAGAIN	11 */	"No more processes",
	/* ENOMEM	12 */	"Not enough space",
	/* EACCES	13 */	"Permission denied",
	/* EFAULT	14 */	"Bad address",
	/* ENOTBLK	15 */	"Block device required",
	/* EBUSY	16 */	"Mount device busy",
	/* EEXIST	17 */	"File exists",
	/* EXDEV	18 */	"Cross-device link",
	/* ENODEV	19 */	"No such device",
	/* ENOTDIR	20 */	"Not a directory",
	/* EISDIR	21 */	"Is a directory",
	/* EINVAL	22 */	"Invalid argument",
	/* ENFILE	23 */	"File table overflow",
	/* EMFILE	24 */	"Too many open files",
	/* ENOTTY	25 */	"Not a typewriter",
	/* ETXTBSY	26 */	"Text file busy",
	/* EFBIG	27 */	"File too large",
	/* ENOSPC	28 */	"No space left on device",
	/* ESPIPE	29 */	"Illegal seek",
	/* EROFS	30 */	"Read-only file system",
	/* EMLINK	31 */	"Too many links",
	/* EPIPE	32 */	"Broken pipe",
	/* EDOM		33 */	"Argument too large",
	/* ERANGE	34 */	"Result too large",
	/* EBSIG	35 */	"Bad signal number",	/* non UNIX */
				"Error undefined"
};

#define SYS_NERR	((sizeof(sys_errlist)/sizeof(char[WIDTH]))- 1)

char *strerror (int errnum)
{
	if (errnum < 0 || errnum > SYS_NERR)
	    errnum = SYS_NERR;
	return (sys_errlist[errnum]);
}
 
#endif /* EXCLUDE_strerror */
 
