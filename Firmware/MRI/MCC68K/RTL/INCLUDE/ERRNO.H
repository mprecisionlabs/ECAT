
/************************************************************************/
/* Copyright (c) 1985-91 Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
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

/*  <errno.h>	Microtec Research, Inc. ANSI C Compiler  */
/* %W% %G% */

#ifndef __ERRNO_H
#define __ERRNO_H	1

extern int errno;

#define	EDOM	33		/* Argument too large */
#define	ERANGE	34		/* Result too large */

/* Microtec extensions (UNIX compatible): */

#define	EPERM	1		/* Not owner */
#define	ENOENT	2		/* No such file or directory */
#define	ESRCH	3		/* No such process */
#define	EINTR	4		/* Interrupted system call */
#define	EIO	5		/* I/O error */
#define	ENXIO	6		/* No such device or address */
#define	E2BIG	7		/* Arg list too long */
#define	ENOEXEC	8		/* Exec format error */
#define	EBADF	9		/* Bad file number */
#define	ECHILD	10		/* No children */
#define	EAGAIN	11		/* No more processes */
#define	ENOMEM	12		/* Not enough core */
#define	EACCES	13		/* Permission denied */
#define	EFAULT	14		/* Bad address */
#define	ENOTBLK	15		/* Block device required */
#define	EBUSY	16		/* Mount device busy */
#define	EEXIST	17		/* File exists */
#define	EXDEV	18		/* Cross-device link */
#define	ENODEV	19		/* No such device */
#define	ENOTDIR	20		/* Not a directory*/
#define	EISDIR	21		/* Is a directory */
#define	EINVAL	22		/* Invalid argument */
#define	ENFILE	23		/* File table overflow */
#define	EMFILE	24		/* Too many open files */
#define	ENOTTY	25		/* Not a typewriter */
#define	ETXTBSY	26		/* Text file busy */
#define	EFBIG	27		/* File too large */
#define	ENOSPC	28		/* No space left on device */
#define	ESPIPE	29		/* Illegal seek */
#define	EROFS	30		/* Read-only file system */
#define	EMLINK	31		/* Too many links */
#define	EPIPE	32		/* Broken pipe */
#define EBSIG	35		/* Bad signal number (non UNIX) */

#endif /* end __ERRNO_H */

