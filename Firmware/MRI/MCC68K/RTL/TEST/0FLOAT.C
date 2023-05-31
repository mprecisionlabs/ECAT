#include "lib_top.h"


#pragma	option	-nx

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1988, 1992 Microtec Research, Inc.			    */
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
**
**	Test double type floating point operations.
**
**	HISTORY:
**		12-29-88(mrm) Created
*/

#include <stdio.h>

#include "message.h"

typedef	float	X;

#define	PASS	0
#define FAIL	1
int	bug_found = 0;

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
	{	2.0,	1.0,	3.0,	1.0,	2.0,	2.0	},
	{	8.0,	2.0,	10.0,	6.0,	16.0,	4.0	},
	{	12.0,	3.0,	15.0,	9.0,	36.0,	4.0	},
	{	16.0,	2.0,	18.0,	14.0,	32.0,	8.0	},
	{	20.0,	2.0,	22.0,	18.0,	40.0,	10.0	},
	{	22.0,	2.0,	24.0,	20.0,	44.0,	11.0	},
	{	30.0,	2.0,	32.0,	28.0,	60.0,	15.0	},
	{	40.0,	5.0,	45.0,	35.0,	200.0,	8.0	},
	{	45.0,	5.0,	50.0,	40.0,	225.0,	9.0	},
	{	-2.0,	1.0,	-1.0,	-3.0,	-2.0,	-2.0	},
	{	-8.0,	2.0,	-6.0,	-10.0,	-16.0,	-4.0	},
	{	-12.0,	3.0,	-9.0,	-15.0,	-36.0,	-4.0	},
	{	-16.0,	2.0,	-14.0,	-18.0,	-32.0,	-8.0	},
	{	-20.0,	2.0,	-18.0,	-22.0,	-40.0,	-10.0	},
	{	-22.0,	2.0,	-20.0,	-24.0,	-44.0,	-11.0	},
	{	-30.0,	2.0,	-28.0,	-32.0,	-60.0,	-15.0	},
	{	-40.0,	5.0,	-35.0,	-45.0,	-200.0,	-8.0	},
	{	-45.0,	5.0,	-40.0,	-50.0,	-225.0,	-9.0	},
	{	-2.0,	-1.0,	-3.0,	-1.0,	2.0,	2.0	},
	{	-8.0,	-2.0,	-10.0,	-6.0,	16.0,	4.0	},
	{	-12.0,	-3.0,	-15.0,	-9.0,	36.0,	4.0	},
	{	-16.0,	-2.0,	-18.0,	-14.0,	32.0,	8.0	},
	{	-20.0,	-2.0,	-22.0,	-18.0,	40.0,	10.0	},
	{	-22.0,	-2.0,	-24.0,	-20.0,	44.0,	11.0	},
	{	-30.0,	-2.0,	-32.0,	-28.0,	60.0,	15.0	},
	{	-40.0,	-5.0,	-45.0,	-35.0,	200.0,	8.0	},
	{	-45.0,	-5.0,	-50.0,	-40.0,	225.0,	9.0	},
	{	2.0,	-1.0,	1.0,	3.0,	-2.0,	-2.0	},
	{	8.0,	-2.0,	6.0,	10.0,	-16.0,	-4.0	},
	{	12.0,	-3.0,	9.0,	15.0,	-36.0,	-4.0	},
	{	16.0,	-2.0,	14.0,	18.0,	-32.0,	-8.0	},
	{	20.0,	-2.0,	18.0,	22.0,	-40.0,	-10.0	},
	{	22.0,	-2.0,	20.0,	24.0,	-44.0,	-11.0	},
	{	30.0,	-2.0,	28.0,	32.0,	-60.0,	-15.0	},
	{	40.0,	-5.0,	35.0,	45.0,	-200.0,	-8.0	},
	{	45.0,	-5.0,	40.0,	50.0,	-225.0,	-9.0	},
	};

main()
{
	for (i=0; i<((sizeof(num))/(sizeof(num[0]))); i++)
	{
		c = num[i].a + num[i].b;
		if (c != num[i].add_res)
			print_message(FAIL, "float", (double)num[i].a, "+",
					(double)num[i].b, (double)c);
		else
			print_message(PASS, "float", (double)num[i].a, "+",
					(double)num[i].b, (double)c);

		c = num[i].a - num[i].b;
		if (c != num[i].sub_res)
			print_message(FAIL, "float", (double)num[i].a, "-",
					(double)num[i].b, (double)c);
		else
			print_message(PASS, "float", (double)num[i].a, "-",
					(double)num[i].b, (double)c);

		c = num[i].a * num[i].b;
		if (c != num[i].mul_res)
			print_message(FAIL, "float", (double)num[i].a, "*",
					(double)num[i].b, (double)c);
		else
			print_message(PASS, "float", (double)num[i].a, "*",
					(double)num[i].b, (double)c);

		c = num[i].a / num[i].b;
		if (c != num[i].div_res)
			print_message(FAIL, "float", (double)num[i].a, "/",
					(double)num[i].b, (double)c);
		else
			print_message(PASS, "float", (double)num[i].a, "/",
					(double)num[i].b, (double)c);
	}
	if (bug_found)
		printf(ERRORS);
	else
		printf (NO_ERRORS);
}

print_message(flag, type_str, oprnd1, opr, oprnd2, result)
int		flag;
char		*type_str;
double		oprnd1;
char		*opr;
double		oprnd2;
double		result;
{
	switch (flag)
	{
	case FAIL:
		printf("ERROR:  FLOAT  (%s) %f %s %f = %f\n",
			type_str, oprnd1, opr, oprnd2, result);
		++bug_found;
		break;
	case PASS:
	default:
		break;
	}
}
 
