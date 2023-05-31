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


/************************************************************************/
/*									*/
/*	This routine tests the ability of _scanf to handle ANSI formats */
/*	correctly.							*/
/*									*/
/************************************************************************/

/*	%W% %G%	*/
/*
History:
	03/16/94 jpc - Added check for spr 34705.
	11/15/93 jpc - Removed all double negatives in "EXCLUDE" expressions
	11/12/93 jpc - Forgot "IO" on "HEX" symbols.  Replaced it.
	11/12/93 jpc - Added EXCLUDE..HEX and EXCLUDE..DEC to "%n" tests
*/

#include <stdio.h>
#include "message.h"

#define	num_el(X)	(sizeof(X)/sizeof(X[0]))

const struct longStruct {
	char reader[20];
	char format[20];
	long   result;
	} longTests[] = {
#if EXCLUDE_FORMAT_IO_l_OPT
	/* No code inserted here */
#else
#if EXCLUDE_FORMAT_IO_DEC_FMT
	/* No code inserted here */
#else
	{"200000a",	"%ld",	200000},		/* %d format */
	{"-200000a",	"%ld",	-200000},
	{"200000a",	"%3ld",	200},
	{"-200000a",	"%3ld",	-20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500a",	"%*3ld %ld",	500},
	{"-200500a",	"%*3ld %ld",	500},
#endif	/* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_DEC_FMT */

#if EXCLUDE_FORMAT_IO_INT_FMT
	/* No code inserted here */
#else
	{"200000a",	"%li",	200000},		/* %i - decimal object*/
	{"-200000a",	"%li",	-200000},
	{"200000a",	"%3li",	200},
	{"-200000a",	"%3li",	-20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500a",	"%*3li %li",	500},
	{"-200500a",	"%*4li %li",	500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"0200000a",	"%li",	0200000},		/* %i - octal object */
	{"-0200000a",	"%li",	-0200000},
	{"0200000a",	"%4li",	0200},
	{"-0200000a",	"%4li",	-020},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"02000500a",	"%*4li %li",	0500},
	{"-02000500a",	"%*5li %li",	0500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"0x200000g",	"%li",	0x200000},		/* %i - hex formgt */
	{"-0x200000g",	"%li",	-0x200000},
	{"0x200000g",	"%5li",	0x200},
	{"-0x200000g",	"%5li",	-0x20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"0x2000x500g",	"%*5li %li",	0x500},
	{"-0x2000x500g","%*6li %li",	0x500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_INT_FMT */

#if EXCLUDE_FORMAT_IO_OCT_FMT
	/* No code inserted here */
#else
	{"0200000a",	"%lo",	0200000},		/* %o - 0 prefox */
	{"-0200000a",	"%lo",	-0200000},
	{"0200000a",	"%4lo",	0200},
	{"-0200000a",	"%4lo",	-020},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"02000500a",	"%*4lo %lo",	0500},
	{"-02000500a",	"%*4lo %lo",	0500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"200000a",	"%lo",	0200000},		/* %o - no 0 prefox */
	{"-200000a",	"%lo",	-0200000},
	{"200000a",	"%3lo",	0200},
	{"-200000a",	"%3lo",	-020},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500a",	"%*3lo %lo",	0500},
	{"-200500a",	"%*4lo %lo",	0500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_OCT_FMT */

#if EXCLUDE_FORMAT_IO_HEX_FMT
	/* No code inserted here */
#else
	{"0x200000g",	"%lx",	0x200000},		/* %x - 0x prefix */
	{"-0x200000g",	"%lx",	-0x200000},
	{"0x200000g",	"%5lx",	0x200},
	{"-0x200000g",	"%5lx",	-0x20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"0x2000x500g",	"%*5lx %lx",	0x500},
	{"-0x2000x500g","%*6lx %lx",	0x500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"200000g",	"%lx",	0x200000},		/* %x - no 0x prefix */
	{"-200000g",	"%lx",	-0x200000},
	{"200000g",	"%3lx",	0x200},
	{"-200000g",	"%3lx",	-0x20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500g",	"%*3lx %lx",	0x500},
	{"-2000500g",	"%*3lx %lx",	0x500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
	{"0",		"%x",	0},
	{"0 123",	"%x",	0},
#endif	/* ! EXCLUDE_FORMAT_IO_HEX_FMT */
#endif	/* ! EXCLUDE_FORMAT_IO_l_OPT */
	{"",		"",	0}
	};

const struct int32Struct {			/* Tests for 32 bit integer */
	char reader[20];
	char format[20];
	long   result;
	} int32Tests[] = {
#if EXCLUDE_FORMAT_IO_DEC_FMT
	/* No code inserted here */
#else
	{"200000a",	"%d",	200000},		/* %d format */
	{"-200000a",	"%d",	-200000},
	{"200000a",	"%3d",	200},
	{"-200000a",	"%3d",	-20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500a",	"%*3d %d",	500},
	{"-200500a",	"%*3d %d",	500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_DEC_FMT */

#if EXCLUDE_FORMAT_IO_INT_FMT
	/* No code inserted here */
#else
	{"0a",		"%i",	0},			/* %i - octal 0 */
	{"0x0",		"%i",	0},			/* %i - hex zero */
	{"200000a",	"%i",	200000},		/* %i - decimal object*/
	{"-200000a",	"%i",	-200000},
	{"200000a",	"%3i",	200},
	{"-200000a",	"%3i",	-20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500a",	"%*3i %i",	500},
	{"-200500a",	"%*4i %i",	500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"0200000a",	"%i",	0200000},		/* %i - octal object */
	{"-0200000a",	"%i",	-0200000},
	{"0200000a",	"%4i",	0200},
	{"-0200000a",	"%4i",	-020},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"02000500a",	"%*4i %i",	0500},
	{"-02000500a",	"%*5i %i",	0500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"0x200000g",	"%i",	0x200000},		/* %i - hex formgt */
	{"-0x200000g",	"%i",	-0x200000},
	{"0x200000g",	"%5i",	0x200},
	{"-0x200000g",	"%5i",	-0x20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"0x2000x500g",	"%*5i %i",	0x500},
	{"-0x2000x500g","%*6i %i",	0x500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_INT_FMT */

#if EXCLUDE_FORMAT_IO_OCT_FMT
	/* No code inserted here */
#else
	{"0200000a",	"%o",	0200000},		/* %o - 0 prefox */
	{"-0200000a",	"%o",	-0200000},
	{"0200000a",	"%4o",	0200},
	{"-0200000a",	"%4o",	-020},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"02000500a",	"%*4o %o",	0500},
	{"-02000500a",	"%*4o %o",	0500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"200000a",	"%o",	0200000},		/* %o - no 0 prefox */
	{"-200000a",	"%o",	-0200000},
	{"200000a",	"%3o",	0200},
	{"-200000a",	"%3o",	-020},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500a",	"%*3o %o",	0500},
	{"-200500a",	"%*4o %o",	0500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_OCT_FMT */

#if EXCLUDE_FORMAT_IO_HEX_FMT
	/* No code inserted here */
#else
	{"0x200000g",	"%x",	0x200000},		/* %x - 0x prefix */
	{"-0x200000g",	"%x",	-0x200000},
	{"0x200000g",	"%5x",	0x200},
	{"-0x200000g",	"%5x",	-0x20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"0x2000x500g",	"%*5x %x",	0x500},
	{"-0x2000x500g","%*6x %x",	0x500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"200000g",	"%x",	0x200000},		/* %x - no 0x prefix */
	{"-200000g",	"%x",	-0x200000},
	{"200000g",	"%3x",	0x200},
	{"-200000g",	"%3x",	-0x20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500g",	"%*3x %x",	0x500},
	{"-2000500g",	"%*3x %x",	0x500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_HEX_FMT */
	{"",		"",		0}
	};

const struct int16Struct {			/* Tests for 16 bit int*/
	char reader[20];
	char format[20];
	short   result;
	} int16Tests[] = {
#if EXCLUDE_FORMAT_IO_DEC_FMT
	/* No code inserted here */
#else
	{"2000a",	"%d",	2000},			/* %d format */
	{"-2000a",	"%d",	-2000},
	{"200000a",	"%3d",	200},
	{"-200000a",	"%3d",	-20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500a",	"%*3d %d",	500},
	{"-200500a",	"%*3d %d",	500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_DEC_FMT */

#if EXCLUDE_FORMAT_IO_INT_FMT
	/* No code inserted here */
#else
	{"2000",	"%i",	2000},			/* %i - decimal object*/
	{"-2000a",	"%i",	-2000},
	{"2000000a",	"%3i",	200},
	{"-200000a",	"%3i",	-20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500a",	"%*3i %i",	500},
	{"-200500a",	"%*4i %i",	500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"02000a",	"%i",	02000},			/* %i - octal object */
	{"-02000a",	"%i",	-02000},
	{"0200000a",	"%4i",	0200},
	{"-0200000a",	"%4i",	-020},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"02000500a",	"%*4i %i",	0500},
	{"-02000500a",	"%*5i %i",	0500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"0x2000g",	"%i",	0x2000},		/* %i - hex formgt */
	{"-0x2000g",	"%i",	-0x2000},
	{"0x200000g",	"%5i",	0x200},
	{"-0x200000g",	"%5i",	-0x20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"0x2000x500g",	"%*5i %i",	0x500},
	{"-0x2000x500g","%*6i %i",	0x500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_INT_FMT */

#if EXCLUDE_FORMAT_IO_OCT_FMT
	/* No code inserted here */
#else
	{"02000a",	"%o",	02000},			/* %o - 0 prefix */
	{"-02000a",	"%o",	-02000},
	{"0200000a",	"%4o",	0200},
	{"-0200000a",	"%4o",	-020},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"02000500a",	"%*4o %o",	0500},
	{"-02000500a",	"%*5o %o",	0500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"2000a",	"%o",	02000},			/* %o - no 0 prefix */
	{"-2000a",	"%o",	-02000},
	{"200000a",	"%3o",	0200},
	{"-200000a",	"%3o",	-020},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500a",	"%*3o %o",	0500},
	{"-200500a",	"%*4o %o",	0500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_OCT_FMT */

#if EXCLUDE_FORMAT_IO_HEX_FMT
	/* No code inserted here */
#else
	{"0x2000g",	"%x",	0x2000},		/* %x - 0x prefix */
	{"-0x2000g",	"%x",	-0x2000},
	{"0x200000g",	"%5x",	0x200},
	{"-0x200000g",	"%5x",	-0x20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"0x2000x500g",	"%*5x %x",	0x500},
	{"-0x2000x500g","%*6x %x",	0x500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"2000g",	"%x",	0x2000},		/* %x - no 0x prefix */
	{"-2000g",	"%x",	-0x2000},
	{"200000g",	"%3x",	0x200},
	{"-200000g",	"%3x",	-0x20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500g",	"%*3x %x",	0x500},
	{"-2000500g",	"%*4x %x",	0x500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_HEX_FMT */
	{"",		"",		0}
	};

const struct shortStruct {
	char reader[20];
	char format[20];
	short   result;
	} shortTests[] = {
#if EXCLUDE_FORMAT_IO_h_OPT
	/* No code inserted here */
#else
#if EXCLUDE_FORMAT_IO_DEC_FMT
	/* No code inserted here */
#else
	{"2000a",	"%hd",	2000},			/* %d format */
	{"-2000a",	"%hd",	-2000},
	{"200000a",	"%3hd",	200},
	{"-200000a",	"%3hd",	-20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500a",	"%*3hd %hd",	500},
	{"-200500a",	"%*3hd %hd",	500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_DEC_FMT */

#if EXCLUDE_FORMAT_IO_INT_FMT
	/* No code inserted here */
#else
	{"2000",	"%hi",	2000},			/* %i - decimal object*/
	{"-2000a",	"%hi",	-2000},
	{"2000000a",	"%3hi",	200},
	{"-200000a",	"%3hi",	-20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500a",	"%*3hi %hi",	500},
	{"-200500a",	"%*4hi %hi",	500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"02000a",	"%hi",	02000},			/* %i - octal object */
	{"-02000a",	"%hi",	-02000},
	{"0200000a",	"%4hi",	0200},
	{"-0200000a",	"%4hi",	-020},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"02000500a",	"%*4hi %hi",	0500},
	{"-02000500a",	"%*5hi %hi",	0500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"0x2000g",	"%hi",	0x2000},		/* %i - hex formgt */
	{"-0x2000g",	"%hi",	-0x2000},
	{"0x200000g",	"%5hi",	0x200},
	{"-0x200000g",	"%5hi",	-0x20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"0x2000x500g",	"%*5hi %hi",	0x500},
	{"-0x2000x500g","%*6hi %hi",	0x500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_INT_FMT */

#if EXCLUDE_FORMAT_IO_OCT_FMT
	/* No code inserted here */
#else
	{"02000a",	"%ho",	02000},			/* %o - 0 prefox */
	{"-02000a",	"%ho",	-02000},
	{"0200000a",	"%4ho",	0200},
	{"-0200000a",	"%4ho",	-020},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"02000500a",	"%*4ho %ho",	0500},
	{"-02000500a",	"%*5ho %ho",	0500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"2000a",	"%ho",	02000},			/* %o - no 0 prefox */
	{"-2000a",	"%ho",	-02000},
	{"200000a",	"%3ho",	0200},
	{"-200000a",	"%3ho",	-020},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500a",	"%*3ho %ho",	0500},
	{"-200500a",	"%*4ho %ho",	0500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_OCT_FMT */

#if EXCLUDE_FORMAT_IO_HEX_FMT
	/* No code inserted here */
#else
	{"0x2000g",	"%hx",	0x2000},		/* %x - 0x prefix */
	{"-0x2000g",	"%hx",	-0x2000},
	{"0x200000g",	"%5hx",	0x200},
	{"-0x200000g",	"%5hx",	-0x20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"0x2000x500g",	"%*5hx %hx",	0x500},
	{"-0x2000x500g","%*6hx %hx",	0x500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"2000g",	"%hx",	0x2000},		/* %x - no 0x prefix */
	{"-2000g",	"%hx",	-0x2000},
	{"200000g",	"%3hx",	0x200},
	{"-200000g",	"%3hx",	-0x20},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"200500g",	"%*3hx %hx",	0x500},
	{"-2000500g",	"%*3hx %hx",	0x500},
#endif  /* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_HEX_FMT */
#endif	/* ! EXCLUDE_FORMAT_IO_h_OPT */
	{"",		"",		0}
	};

const struct doubleStruct {
	char reader[20];
	char format[20];
	double resultLo;
	double resultHi;
	} doubleTests [] = {

#if EXCLUDE_FORMAT_IO_FLOAT_FMT
	/* No code inserted here */
#else
#if EXCLUDE_FORMAT_IO_l_OPT
	/* No code inserted here */
#else
	{"3.1415a",	"%lf",	3.14145,	3.14155},	/* positive */
	{"31415a",	"%lf",	31415,		31415},
	{".0031415a",	"%lf",	.00314145,	.00314155},

	{"-3.1415a",	"%lf",	-3.14155,	-3.14145},	/* negative */
	{"-31415a",	"%lf",	-31415,		-31415},
	{"-.0031415a",	"%lf",	-.00314155,	-.00314145},

	{"3.1415e10a",	"%lf",	3.14145e10,	3.14155e10},	/* positive */
#if __OPTION_VALUE("-Kq")
	{"31415a",	"%lf",	31415,		31415},
#else
	{"31415e10a",	"%lf",	31415e10,	31415e10},
#endif
	{".0031415e-10a","%lf",	.00314145e-10,	.00314155e-10},

	{"-3.1415e10a",	"%lf",	-3.14155e10,	-3.14145e10},	/* negative */
#if __OPTION_VALUE("-Kq")
	{"-31415a",	"%lf",	-31415,		-31415},
#else
	{"-31415e10a",	"%lf",	-31415e10,	-31415e10},
#endif
	{"-.0031415e-10a","%lf",	-.00314155e-10,	-.00314145e-10},

#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"-3.1415e10a 2.5",
		"%lf %l*f",	-3.14155e10,	-3.14145e10},	/* * */
#if __OPTION_VALUE("-Kq")
	{"-31415a 2.5",
		"%lf %l*f",	-31415,		-31415},
#else
	{"-31415e10a 2.5",
		"%lf %l*f",	-31415e10,	-31415e10},
#endif
	{"-.0031415e-10a 2.5",
		"%lf %l*f",	-.00314155e-10,	-.00314145e-10},

	{"3.1415a",	"%*3lf %lf",414.5,	415.5},	/* Field width */
	{"31415a",	"%*3lf %lf",15,	15},
	{".00314.15a",	"%*4lf %lf",14.145,14.155},
#endif	/* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"3.1415a",	"%lf",	3.14145,	3.14155},	/* positive */
	{"3.1415a",	"%le",	3.14145,	3.14155},
	{"3.1415a",	"%lE",	3.14145,	3.14155},
	{"3.1415a",	"%lg",	3.14145,	3.14155},
	{"3.1415a",	"%lG",	3.14145,	3.14155},
#endif	/* ! EXCLUDE_FORMAT_IO_l_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_FLOAT_FMT */
	{"",		"",	0.,		0.}
	};
	

const struct floatStruct {
	char reader[20];
	char format[20];
	float resultLo;
	float resultHi;
	} floatTests [] = {

#if EXCLUDE_FORMAT_IO_FLOAT_FMT
	/* No code inserted here */
#else
	{"3.1415a",	"%f",	3.14145,	3.14155},	/* positive */
	{"31415a",	"%f",	31415,		31415},
	{".0031415a",	"%f",	.00314145,	.00314155},

	{"-3.1415a",	"%f",	-3.14155,	-3.14145},	/* negative */
	{"-31415a",	"%f",	-31415,		-31415},
	{"-.0031415a",	"%f",	-.00314155,	-.00314145},

	{"3.1415e10a",	"%f",	3.14145e10,	3.14155e10},	/* positive */
	{"31415e10a",	"%f",	31414.5e10,	31415.5e10},
	{".0031415e-10a","%f",	.00314145e-10,	.00314155e-10},

	{"-3.1415e10a",	"%f",	-3.14155e10,	-3.14145e10},	/* negative */
	{"-31415e10a",	"%f",	-31415.5e10,	-31414.5e10},
	{"-.0031415e-10a","%f",	-.00314155e-10,	-.00314145e-10},

#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"-3.1415e10a 2.5",
		"%f %*f",	-3.14155e10,	-3.14145e10},	/* * */
	{"-31415e10a 2.5",
		"%f %*f",	-31415.5e10,	-31414.5e10},
	{"-.0031415e-10a 2.5",
		"%f %*f",	-.00314155e-10,	-.00314145e-10},

	{"3.1415a",	"%*3f %f",414.5,	415.5},	/* Field width */
	{"31415a",	"%*3f %f",14.5,	15.5},
	{".00314.15a",	"%*4f %f",14.145,14.155},
#endif	/* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"3.1415a",	"%f",	3.14145,	3.14155},	/* positive */
	{"3.1415a",	"%e",	3.14145,	3.14155},
	{"3.1415a",	"%E",	3.14145,	3.14155},
	{"3.1415a",	"%g",	3.14145,	3.14155},
	{"3.1415a",	"%G",	3.14145,	3.14155},
#endif	/* ! EXCLUDE_FORMAT_IO_FLOAT_FMT */
	{"",		"",	0.,		0.}
	};
	

const struct longDoubleStruct {
	char reader[20];
	char format[20];
	long double resultLo;
	long double resultHi;
	} longDoubleTests [] = {

#if EXCLUDE_FORMAT_IO_FLOAT_FMT
	/* No code inserted here */
#else
#if EXCLUDE_FORMAT_IO_L_OPT
	/* No code inserted here */
#else
	{"3.1415a",	"%Lf",	3.14145,	3.14155},	/* positive */
	{"31415a",	"%Lf",	31415,		31415},
	{".0031415a",	"%Lf",	.00314145,	.00314155},

	{"-3.1415a",	"%Lf",	-3.14155,	-3.14145},	/* negative */
	{"-31415a",	"%Lf",	-31415,		-31415},
	{"-.0031415a",	"%Lf",	-.00314155,	-.00314145},

	{"3.1415e10a",	"%Lf",	3.14145e10,	3.14155e10},	/* positive */
#if __OPTION_VALUE ("-Kq")
	{"31415a",	"%Lf",	31415,		31415},
#else
	{"31415e10a",	"%Lf",	31415e10,	31415e10},
#endif
	{".0031415e-10a","%Lf",	.00314145e-10,	.00314155e-10},

	{"-3.1415e10a",	"%Lf",	-3.14155e10,	-3.14145e10},	/* negative */
#if __OPTION_VALUE ("-Kq")
	{"-31415a",	"%Lf",	-31415,		-31415},
#else
	{"-31415e10a",	"%Lf",	-31415e10,	-31415e10},
#endif
	{"-.0031415e-10a","%Lf",	-.00314155e-10,	-.00314145e-10},

#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"-3.1415e10a 2.5",
		"%Lf %*Lf",	-3.14155e10,	-3.14145e10},	/* * */
#if __OPTION_VALUE ("-Kq")
	{"-31415a 2.5",
		"%Lf %*Lf",	-31415,		-31415},
#else
	{"-31415e10a 2.5",
		"%Lf %*Lf",	-31415e10,	-31415e10},
#endif

	{"-.0031415e-10a 2.5",
		"%Lf %*Lf",	-.00314155e-10,	-.00314145e-10},

	{"3.1415a",	"%*3Lf %Lf",414.5,	415.5},	/* Field width */
	{"31415a",	"%*3Lf %Lf",15,	15},
	{".00314.15a",	"%*4Lf %Lf",14.145,14.155},
#endif	/* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */

	{"3.1415a",	"%Lf",	3.14145,	3.14155},	/* positive */
	{"3.1415a",	"%Le",	3.14145,	3.14155},
	{"3.1415a",	"%LE",	3.14145,	3.14155},
	{"3.1415a",	"%Lg",	3.14145,	3.14155},
	{"3.1415a",	"%LG",	3.14145,	3.14155},
#endif	/* ! EXCLUDE_FORMAT_IO_L_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_FLOAT_FMT */
	{"",		"",	0.,		0.}
	};
	
const struct stringStruct {
	char reader[20];
	char format[20];
	char result[20];
	} stringTests [] = {
#if EXCLUDE_FORMAT_IO_STR_FMT
	/* No code inserted here */
#else
	{"stringTest",		"%s",		"stringTest"},
	{"stringTest",		"%5s",		"strin"},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"string barf",		"%s %*s",	"string"},
	{"string barf", 	"%*3s %s",	"ing"},
#if EXCLUDE_FORMAT_IO_BRAKT_FMT
	/* No code inserted here */
#else
	{"aabbccstring",	"%*[abc] %s",	"string"},
#endif	/* ! EXCLUDE_FORMAT_IO_BRAKT_FMT */
#endif	/* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#if EXCLUDE_FORMAT_IO_BRAKT_FMT
	/* No code inserted here */
#else
	{"aabbccstring",	"%[abc]",	"aabbcc"},
	{"aabbccstring",	"%[abc]",	"aabbcc"},
	{"aabbccstring",	"%[^string]",	"aabbcc"},
	{"]abbccstring",	"%[]abc]",	"]abbcc"},
	{"-abbccstring",	"%[-abc]",	"-abbcc"},
	{"aabbccstring",	"%3[abc]",	"aab"},
#endif	/* ! EXCLUDE_FORMAT_IO_BRAKT_FMT */
#endif	/* ! EXCLUDE_FORMAT_IO_STR_FMT */
	{"",			"",		""}
	};

const struct charStruct {
	char reader[20];
	char format[20];
	char result[20];
	} charTests [] = {
#if EXCLUDE_FORMAT_IO_CHAR_FMT
	/* No code inserted here */
#else
	{"string",	"%c",		"sZZZZZZ"},
	{"string",	"%3c",		"strZZZZ"},
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	{"string",	"%3c %*c",	"strZZZZ"},
#endif	/* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_STR_FMT */
	{"",		"",		""}
	};

struct Point32Struct {		/* 32 bit void * */
	char* reader;
	char* format;
	void* result;
	} point32Tests [4];

struct Point16Struct {		/* 32 bit void * */
	char* reader;
	char* format;
	void* result;
	} point16Tests [4];

int	errorCount;
int	i;
int	returnVal;
long	longResult;
int	intResult;
short	shortResult;
long double longDoubleResult;
double	doubleResult;
float	floatResult;
char	stringResult[50];
char	string2Result[50];
char	charResult[50];
void	*point32Result;
void	*point16Result;

struct longStruct	*longStructP;
struct shortStruct	*shortStructP;
struct int16Struct	*int16StructP;
struct int32Struct	*int32StructP;
struct doubleStruct	*doubleStructP;
struct floatStruct	*floatStructP;
struct longDoubleStruct	*longDoubleStructP;
struct stringStruct	*stringStructP;
struct charStruct	*charStructP;
struct Point32Struct	*point32StructP;
struct Point16Struct	*point16StructP;

main ()
{

   int index = 0;
	
#if EXCLUDE_FORMAT_IO_PNT_FMT
	/* No code inserted here */
#else
	point32Tests[index].reader = "deaddead";	
        point32Tests[index].format= "%p";		
        point32Tests[index].result = (void *) 0xdeaddead;
        index++;
	point32Tests[index].reader = "deaddead";	
        point32Tests[index].format= "%3p";		
        point32Tests[index].result = (void *) 0xdea;
        index++;
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	point32Tests[index].reader = "deaddead";	
        point32Tests[index].format=  "%*3p %p";		
        point32Tests[index].result = (void *)  0xddead;
        index++;
#endif	/* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_PNT_FMT */
	point32Tests[index].reader = "";	
        point32Tests[index].format= "";		
        point32Tests[index].result = NULL; 

        index = 0;

#if EXCLUDE_FORMAT_IO_PNT_FMT
	/* No code inserted here */
#else
	point16Tests[index].reader = "dead";	
        point16Tests[index].format= "%p";		
        point16Tests[index].result = (void *) 0xdead;
        index++;
	point16Tests[index].reader = "deaddead";	
        point16Tests[index].format= "%3p";		
        point16Tests[index].result = (void *) 0xdea;
        index++;
#if EXCLUDE_FORMAT_IO_ASSGN_OPT
	/* No code inserted here */
#else
	point16Tests[index].reader = "deaddea";	
        point16Tests[index].format=  "%*3p %p";		
        point16Tests[index].result = (void *)  0xddea;
        index++;
#endif	/* ! EXCLUDE_FORMAT_IO_ASSGN_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_PNT_FMT */
	point16Tests[index].reader = "";	
        point16Tests[index].format= "";		
        point16Tests[index].result = NULL; 

    for (i=0; i<(num_el(longTests)-1); i++) {
	longStructP = &longTests[i];		/* for speed */
	longResult = 0;
	if ((returnVal = sscanf (longStructP->reader, longStructP->format,
		&longResult, &longResult)) != 1) {
	    printf ("Error: longTests test # %i, sscanf returned %d, "
		    "expected 1\n", i, returnVal);
	    errorCount++;
	    }
	if (longResult != longStructP->result) {
	    printf ("Error: longTests: conversion: test # %d, got %ld, "
		    "expected %ld\n", i, longResult, longStructP->result);
	    errorCount++;
	    }
	}

if (sizeof(int) == 4)				/* 32 bit machines */
    for (i=0; i<(num_el(int32Tests)-1); i++) {

	int32StructP = &int32Tests[i];		/* for speed */
	intResult = 0;

	if ((returnVal = sscanf (int32StructP->reader, int32StructP->format,
		&intResult, &intResult)) != 1) {
	    printf ("Error: intTests test # = %i, sscanf returned %d, "
		"expected 1\n", i, returnVal);
	    errorCount++;
	    }

	if (intResult != int32StructP->result) {
	    printf ("Error: intTests: conversion: test # = %d, got %lx, "
		    "expected %lx\n", i, intResult, int32StructP->result);
	    errorCount++;
	    }
	}
else						/* 16 bit machines */
    for (i=0; i<(num_el(int16Tests)-1); i++) {

	int16StructP = &int16Tests[i];		/* for speed */
	intResult = 0;

	if ((returnVal = sscanf (int16StructP->reader, int16StructP->format,
		&intResult, &intResult)) != 1) {
	    printf ("Error: intTests test # = %i, sscanf returned %d, "
		    "expected 1\n", i, returnVal);
	    errorCount++;
	    }

	if (intResult != int16StructP->result) {
	    printf ("Error: intTests: conversion: test # = %d, got %x, "
		    "expected %x\n", i, intResult, int16StructP->result);
	    errorCount++;
	    }
	}

    for (i=0; i<(num_el(shortTests)-1); i++) {

	shortStructP = &shortTests[i];		/* for speed */
	shortResult = 0;

	if ((returnVal = sscanf (shortStructP->reader, shortStructP->format,
		&shortResult, &shortResult)) != 1) {
	    printf ("Error: shortTests test # = %i, sscanf returned %d, "
		    "expected 1\n", i, returnVal);
	    errorCount++;
	    }

	if (shortResult != shortStructP->result) {
	    printf ("Error: shortTests: conversion: test # = %d, got %x, "
		    "expected %x\n", i, intResult, shortStructP->result);
	    errorCount++;
	    }
	}

    for (i=0; i<(num_el(doubleTests)-1); i++) {

	doubleStructP = &doubleTests[i];		/* for speed */
	doubleResult = 0.;

	if ((returnVal = sscanf (doubleStructP->reader, doubleStructP->format,
		&doubleResult, &doubleResult)) != 1) {
	    printf ("Error: doubleTests test # = %i, sscanf returned %d, "
		    "expected 1\n", i, returnVal);
	    errorCount++;
	    }

	if ((doubleResult < doubleStructP->resultLo)
		|| (doubleResult > doubleStructP->resultHi)) {
	    printf ("Error: doubleTests: conversion: test # = %d, got %f, "
		    "expected %f\n", i, doubleResult, doubleStructP->resultLo);
	    errorCount++;
	    }
	}

    for (i=0; i<(num_el(floatTests)-1); i++) {

	floatStructP = &floatTests[i];		/* for speed */
	floatResult = 0.;

	if ((returnVal = sscanf (floatStructP->reader, floatStructP->format,
		&floatResult, &floatResult)) != 1) {
	    printf ("Error: floatTests test # = %i, sscanf returned %d, "
		    "expected 1\n", i, returnVal);
	    errorCount++;
	    }

	if ((floatResult < floatStructP->resultLo)
		|| (floatResult > floatStructP->resultHi)) {
	    printf ("Error: floatTests: conversion: test # = %d, got %f, "
		    "expected %f\n", i, floatResult, floatStructP->resultLo);
	    errorCount++;
	    }
	}

    for (i=0; i<(num_el(longDoubleTests)-1); i++) {

	longDoubleStructP = &longDoubleTests[i];	/* for speed */
	longDoubleResult = 0.;

	if ((returnVal = sscanf (longDoubleStructP->reader,
		longDoubleStructP->format,
		&longDoubleResult, &longDoubleResult)) != 1) {
	    printf ("Error: longDoubleTests test # = %i, sscanf returned %d, "
		    "expected 1\n", i, returnVal);
	    errorCount++;
	    }

	if ((longDoubleResult < longDoubleStructP->resultLo)
		|| (longDoubleResult > longDoubleStructP->resultHi)) {
	    printf ("Error: longDoubleTests: conversion: test # = %d, got %lf, "
		    "expected %lf\n", i, longDoubleResult,
		    longDoubleStructP->resultLo);
	    errorCount++;
	    }
	}

    for (i=0; i<(num_el(stringTests)-1); i++) {

	stringStructP = &stringTests[i];	/* for speed */
	strcpy (stringResult, "");

	if ((returnVal = sscanf (stringStructP->reader,
		stringStructP->format,
		stringResult, stringResult)) != 1) {
	    printf ("Error: stringTests test # = %i, sscanf returned %d, "
		    "expected 1\n", i, returnVal);
	    errorCount++;
	    }

	if (strcmp (stringResult, stringStructP->result)) {
	    printf ("Error: stringTests: conversion: test # = %d, got -%s-, "
		    "expected -%s-\n", i, stringResult,
		    stringStructP->result);
	    errorCount++;
	    }
	}

    for (i=0; i<(num_el(charTests)-1); i++) {

	strcpy (stringResult, "ZZZZZZZ");	/* set result string */
	charStructP = &charTests[i];	/* for speed */
	strcpy (stringResult, "");

	if ((returnVal = sscanf (charStructP->reader,
		charStructP->format,
		stringResult, stringResult)) != 1) {
	    printf ("Error: charTests test # = %i, sscanf returned %d, "
		    "expected 1\n", i, returnVal);
	    errorCount++;
	    }

	if (strcmp (stringResult, charStructP->result)) {
	    printf ("Error: stringTests: conversion: test # = %d, got -%s-, "
		    "expected -%s-\n", i, stringResult,
		    charStructP->result);
	    errorCount++;
	    }
	}

if (sizeof (void *) == 4)			/* 32 bit pointers */
    for (i=0; i<(num_el(point32Tests)-1); i++) {

	if(point32Tests[i].reader[0] == 0) break;
        point32StructP = &point32Tests[i];	/* for speed */
	point32Result = NULL;

	if ((returnVal = sscanf (point32StructP->reader,
		point32StructP->format,
		&point32Result, &point32Result)) != 1) {
	    printf ("Error: point32Tests test # = %i, sscanf returned %d, "
		    "expected 1\n", i, returnVal);
	    errorCount++;
	    }

	if (point32Result != point32StructP->result) {
	    printf ("Error: point32Tests: conversion: test # = %d, got %p, "
		    "expected %p\n", i, point32Result,
		    point32StructP->result);
	    errorCount++;
	    }
	}

if (sizeof (void *) == 2)			/* 16 bit pointers */
    for (i=0; i<(num_el(point16Tests)-1); i++) {
        if(point16Tests[i].reader[0] == 0) break;
	point16StructP = &point16Tests[i];	/* for speed */
	point16Result = NULL;

	if ((returnVal = sscanf (point16StructP->reader,
		point16StructP->format,
		&point16Result, &point16Result)) != 1) {
	    printf ("Error: point16Tests test # = %i, sscanf returned %d, "
		    "expected 1\n", i, returnVal);
	    errorCount++;
	    }

	if (point16Result != point16StructP->result) {
	    printf ("Error: point16Tests: conversion: test # = %d, got %p, "
		    "expected %p\n", i, point16Result,
		    point16StructP->result);
	    errorCount++;
	    }
	}

#if EXCLUDE_FORMAT_IO_DEC_FMT
	/* No code inserted here */
#else
#if EXCLUDE_FORMAT_IO_NUMB_FMT
	/* No code inserted here */
#else
    if ((returnVal = sscanf ("123 456 789", "%d %d %d%n",
	    &intResult, &intResult, &intResult, &intResult)) != 3) {
	printf ("Error: n format: test 1: sscanf returned %d, expected 3\n",
		i, returnVal);
	errorCount++;
	}
    if (intResult != 11) {
	printf ("Error: n format: test 1: %%n format failed: recieved %d, "
		"expected 11\n", intResult);
	errorCount++;
	}

#if EXCLUDE_FORMAT_IO_l_OPT
	/* No code inserted here */
#else
    if ((returnVal = sscanf ("123 456 789", "%d %d %d%ln",
	    &intResult, &intResult, &intResult, &longResult)) != 3) {
	printf ("Error: ln format: test 1: sscanf returned %d, expected 3\n",
		i, returnVal);
	errorCount++;
	}
    if (longResult != 11) {
	printf ("Error: ln format: test 1: %%n format failed: recieved %d, "
		"expected 11\n", longResult);
	errorCount++;
	}
#endif	/* ! EXCLUDE_FORMAT_IO_l_OPT */

#if EXCLUDE_FORMAT_IO_h_OPT
	/* No code inserted here */
#else
    if ((returnVal = sscanf ("123 456 789", "%d %d %d%hn",
	    &intResult, &intResult, &intResult, &shortResult)) != 3) {
	printf ("Error: ln format: test 1: sscanf returned %d, expected 3\n",
		i, returnVal);
	errorCount++;
	}
    if (shortResult != 11) {
	printf ("Error: ln format: test 1: %%n format failed: recieved %d, "
		"expected %d\n", shortResult);
	errorCount++;
	}
#endif	/* ! EXCLUDE_FORMAT_IO_h_OPT */
#endif	/* ! EXCLUDE_FORMAT_IO_NUMB_FMT */
#endif	/* ! EXCLUDE_FORMAT_IO_DEC_FMT */

#if	(! EXCLUDE_FORMAT_IO_NUMB_FMT) && (! EXCLUDE_FORMAT_IO_STR_FMT) && \
	(! EXCLUDE_FORMAT_IO_CHAR_FMT) && (! EXCLUDE_FORMAT_IO_FLOAT_FMT) && \
	(! EXCLUDE_FORMAT_IO_BRAKT_FMT) && (! EXCLUDE_FORMAT_IO_h_OPT) && \
	(! EXCLUDE_FORMAT_IO_l_OPT) && (! EXCLUDE_FORMAT_IO_L_OPT) && \
	(! EXCLUDE_FORMAT_IO_DEC_FMT)
    {
    int count1, count2, count3, count4, count5, count6, count7, count8, count9,
	count10;
    int intResult2;

/*			  000000000011111111112222222222333333333344444444445 */
/*	                  123456789012345678901234567890123456789012345678901 */
    returnVal = sscanf (" 123 456 789 3.1415 3.1415 3.1415 string1 string2 c ",
	"%d %n %ld %n %hd %n %lf %n %Lf %n %f %n %s %n %[string2] %n %c %n",
	&intResult, &count1, &longResult, &count2, &shortResult, &count3,
	&doubleResult, &count4, &longDoubleResult, &count5,&floatResult,&count6,
	stringResult, &count7, string2Result, &count8, charResult, &count9);

    if (returnVal != 9) {
	printf ("Error: n format; final test: sscanf returned %d, "
	    "expected 9\n", returnVal);
	errorCount++;
	}

    if ((count1 != 5) || (count2 != 9) || (count3 != 13) || (count4 != 20) ||
	(count5 != 27) || (count6 != 34) || (count7 != 42) || (count8 != 50) ||
	(count9 != 52)) {
	printf ("Error: n format; final test a; value set to %%n incorrect\n");
	errorCount++;
	}

    if ((intResult != 123) || (longResult != 456) || (shortResult != 789) ||
	(floatResult <= 3.14145) || (floatResult >= 3.14155) ||
	(doubleResult <= 3.14145) || (doubleResult >= 3.14155) ||
	(longDoubleResult <= 3.14145) || (longDoubleResult >= 3.14155) ||
	(strcmp (stringResult, "string1")) || (strcmp (string2Result,"string2"))
	|| (charResult[0] != 'c')) {
	printf ("Error: n format; final test a; value set was incorrect\n");
	errorCount++;
	}

#if 	(! EXCLUDE_FORMAT_IO_HEX_FMT) && (! EXCLUDE_FORMAT_IO_DEC_FMT)

    returnVal = sscanf ("123abc", "%d%n%x%n", &intResult, &count1,
	&intResult2, &count2);

    if (returnVal != 2) {
	printf ("Error: n format; final test b; sscanf returned %d, "
	    "expected 2\n", returnVal);
	}

    if ((intResult != 123) || (count1 != 3) || (intResult2 != 0xabc) ||
	(count2 != 6)) {
	printf ("Error: n format; final test b; value set was incorrect\n");
	}

#endif	/* (! EXCLUDE_FORMAT_IO_HEX_FMT) && (! EXCLUDE_FORMAT_IO_DEC_FMT) */

#if EXCLUDE_FORMAT_IO_DEC_FMT
	/* No code inserted here */
#else

    returnVal = sscanf ("123456", "%3d%n%d%n",  &intResult, &count1,
	&intResult2, &count2);

    if (returnVal != 2) {
        printf ("Error: n format; final test c; sscanf returned %d, "
            "expected 2\n", returnVal);
        }
 
    if ((intResult != 123) || (count1 != 3) || (intResult2 != 456) ||
        (count2 != 6)) {
        printf ("Error: n format; final test c; value set was incorrect\n");
       }

#endif	/* ! EXCLUDE_FORMAT_IO_DEC_FMT */

    }
#endif	/* (! EXCLUDE_FORMAT_IO_NUMB_FMT) || .... */

    if (errorCount) {
	printf (ERRORS);
	exit (1);
	}
    else {
	printf (NO_ERRORS);
	exit ();
	}
}
 
