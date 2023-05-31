#include "lib_top.h"


#pragma	option	-nx

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) 1991-1992  Microtec Research, Inc.			*/
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


/*	%W% %G%	*/

#include <stddef.h>
#include <stdio.h>

#include "message.h"

/*	08/02/91:	use write instead of puts to test z80 triple libr. */
/*			scheme						*/

static char mess1[] = "argc != 0\n";
static char mess2[] = "argv[argc] != NULL\n";
static char mess3[] = ERRORS;
static char mess4[] = NO_ERRORS;

main (argc, argv)
	int argc;
	char *argv[];
	{
	int	error = 0;

	if (argc != 0)
		{
		write (1, mess1, sizeof(mess1)-1);
		error = 1;
		}
	if (argv[argc] != NULL)
		{
		write (1, mess2, sizeof(mess2)-1);
		error = 1;
		}
	if (error)
		write (1, mess3, sizeof(mess3)-1);
	else
		write (1, mess4, sizeof(mess4)-1);
	}

 
