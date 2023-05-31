#include "lib_top.h"


#pragma	option	-nx

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.       		    */
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

/* 
**	int_math.c
**	Testing integer +, -, *, /
*/

#include <stdio.h>

#include "message.h"

#define numEl(x)	(sizeof(x) / sizeof(x[0]))

int	errorCount;

typedef	long	X;
typedef	long	Y;
typedef	long	Z;

struct	test_num {
		X	a;
		X	b;
		X	add_res;
		X	sub_res;
		X	mul_res;
		X	div_res;
};

int		i;
X		c;
static struct test_num	num[] = {
	{	2,	1,	3,	1,	2,	2	},
	{	8,	2,	10,	6,	16,	4	},
	{	12,	3,	15,	9,	36,	4	},
	{	16,	2,	18,	14,	32,	8	},
	{	20,	2,	22,	18,	40,	10	},
	{	22,	2,	24,	20,	44,	11	},
	{	30,	2,	32,	28,	60,	15	},
	{	40,	5,	45,	35,	200,	8	},
	{	45,	5,	50,	40,	225,	9	},
	{	-2,	1,	-1,	-3,	-2,	-2	},
	{	-8,	2,	-6,	-10,	-16,	-4	},
	{	-12,	3,	-9,	-15,	-36,	-4	},
	{	-16,	2,	-14,	-18,	-32,	-8	},
	{	20,	-2,	18,	22,	-40,	-10	},
	{	22,	-2,	20,	24,	-44,	-11	},
	{	30,	-2,	28,	32,	-60,	-15	},
	{	40,	-5,	35,	45,	-200,	-8	},
	{	45,	-5,	40,	50,	-225,	-9	},
	{	-2,	-1,	-3,	-1,	2,	2	},
	{	-8,	-2,	-10,	-6,	16,	4	},
	{	-12,	-3,	-15,	-9,	36,	4	},
	{	-16,	-2,	-18,	-14,	32,	8	},
	{	-20,	-2,	-22,	-18,	40,	10	},
	{	-22,	-2,	-24,	-20,	44,	11	},
	{	-30,	-2,	-32,	-28,	60,	15	},
	{	-40,	-5,	-45,	-35,	200,	8	},
	{	-45,	-5,	-50,	-40,	225,	9	},
	};

main()
{
	for (i=0; i<numEl(num); i++)
	{
		c = num[i].a + num[i].b;
		if (c != num[i].add_res)
			{
			printf("ERROR:  ADDITION DOES NOT WORK: "
				"%d + %d = %d MUST BE %d\n", 
				num[i].a, num[i].b, c, num[i].add_res);
			errorCount++;
			}

		c = num[i].a - num[i].b;
		if (c != num[i].sub_res)
			{
			printf("ERROR:  SUBTRACTION DOES NOT WORK: "
				"%d - %d = %d MUST BE %d\n",
				num[i].a, num[i].b, c, num[i].sub_res);
			errorCount++;
			}

		c = num[i].a * num[i].b;
		if (c != num[i].mul_res)
			{
			printf("ERROR:  MULTIPLICATION DOES NOT WORK: "
				"%d * %d = %d MUST BE %d\n",
				num[i].a, num[i].b, c, num[i].mul_res);
			errorCount++;
			}

		c = num[i].a / num[i].b;
		if (c != num[i].div_res)
			{
			printf("ERROR:  DIVISION DOES NOT WORK: "
				"%d / %d = %d MUST BE %d\n",
				num[i].a, num[i].b, c, num[i].div_res);
			errorCount++;
			}
	}

if (errorCount)
	printf (ERRORS);
else
	printf (NO_ERRORS);

}
 
