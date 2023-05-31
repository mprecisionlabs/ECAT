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

/* pgm to test atof() - written by H. Yuen  12/23/86 */

/*	%W% %G%	*/

#include <math.h>
#include <mriext.h>
#include <stdlib.h>
#include <stdio.h>
#include "message.h"

#if __OPTION_VALUE("-Kq")
    #define	DELTA	(1e-5)
#else
    #define	DELTA	(1e-15)
#endif

#define LO(x)	((x) * (1.-DELTA))
#define HI(x)	((x) * (1.+DELTA))

#define	LT_LOW(x)	(x < cases[i].value_lo)
#define GT_HI(x)	(x > cases[i].value_hi)

struct testcase {
	char	str[20];
	double	value_lo;
	double	value_hi;
	int	offset;
} cases[] = {
		"  000",	0.0,		0.0,		5,
		"10.",		10.0,		10.0,		3,
		"+1234",	1234,		1234,		5,
		"abcd",		0.0,		0.0,		0,
		" -sorry",	-0.0,		-0.0,		0,
		"-32700",	-32700.0,	-32700.0,	6,
		"0x1234",	0.0,		0.0,		1,
		" 65540",	65540.0,	65540.0,	6,
#if __OPTION_VALUE("-Kq")
		"+214748.",	214748.0,	214748.0,	8,
#else
		"+2147483640",	2147483640.0,	2147483640.0,	11,
#endif
		"  -65550",	-65550.0,	-65550.0,	8,
		" +02468 0",	2468.0,		2468.0,		7,
		"\t\t 9876",	9876.0,		9876.0,		7,
		"000111",	111.0,		111.0,		6,
#if __OPTION_VALUE("-Kq")
		"1E7 0",	1E7,		1E7,		3,
#else
		"1E20 0",	1E20,		1E20,		4,
#endif
		" 0.00000321",	LO(0.00000321),	HI(0.00000321),	11,
#if !__OPTION_VALUE("-Kq")
		" -1e-100",	HI(-1e-100),	LO(-1e-100),	8,
#endif
		"3.1415926 53",	LO(3.1415926),	HI(3.1415926),	9,
		"8989.2323",	LO(8989.2323),	HI(8989.2323),	9,
#if !__OPTION_VALUE("-Kq")
		"1.77e+308",	LO(1.77e+308),	HI(1.77e+308),	9,
		"-9.4E-307",	HI(-9.4E-307),	LO(-9.4E-307),	9,
#endif
		"",	0.0,		0 };

main()
{
	register struct testcase *p;
	register int n, i, j;
	double	jd;
	int errorFlag = 0;
	char *	endptr;

	p = &cases[0];
	for (i=0; ; i++, p++) {
		if (p->str[0] == 0) break;

		jd = atof(p->str);
		if (LT_LOW (jd) || GT_HI (jd)) {
#if __OPTION_VALUE("-Kq")
		    printf("atof error %2d  %s => %08lx (%08lx)"
			" (%08lx)\n",
#else
		    printf("atof error %2d  %s => %08lx %08lx (%08lx %08lx)"
			" (%08lx %08lx)\n",
#endif
			i, p->str, jd, p->value_lo, p->value_hi);
		    errorFlag++;
		    }
		
		jd = strtod (p->str, &endptr);
		if (LT_LOW (jd) || GT_HI (jd)) {
#if __OPTION_VALUE("-Kq")
		    printf("strtod error %2d %s => %08lx (%08lx)"
			" (%08lx)\n",
#else
		    printf("strtod error %2d %s => %08lx %08lx (%08lx %08lx)"
			" (%08lx %08lx)\n",
#endif
			i, p->str, jd, p->value_lo, p->value_hi);
		    errorFlag++;
		    }
		if (endptr != (p->str + p->offset)) {
		    printf("strtod error %2d %s addr:  expected %p actual %p\n",
			i, p->str, p->str + p->offset, endptr);
		    errorFlag++;
		    }
	}
	if (errorFlag)
	    printf (ERRORS);
	else
	    printf (NO_ERRORS);
}
 
