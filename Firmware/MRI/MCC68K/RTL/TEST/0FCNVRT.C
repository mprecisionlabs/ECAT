#include "lib_top.h"


#pragma	option	-nx

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.			    */
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
**	Test floating point conversions.
**
**	HISTORY:
**		12-29-88(mrm) Created
*/

#include <stdio.h>

#include "message.h"

typedef	double	X;
typedef	float	Y;
typedef	long double	Z;

#define	PASS	0
#define FAIL	1

int	bug_found;

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

	long		aint, bint, cint;
	unsigned long	auint, buint, cuint;
	float		afloat, bfloat, cfloat;
	double		adouble, bdouble, cdouble;
main()
{

	for (i=0; i<((sizeof(num))/(sizeof(num[0]))); i++)
	{
		aint = (long)num[i].a;
		bint = (long)num[i].b;
		cint = aint + bint;
		c = num[i].a + num[i].b;
		if (cint != (long)c)
			print_message(FAIL, "float", (double)c,
				"long", (double)c);
		else
			print_message(PASS, "float", (double)c,
				"long", (double)c);

		afloat = (float)aint;
		bfloat = (float)bint;
		cfloat = afloat + bfloat;
		cint = aint + bint;
		if (cfloat != (float)cint)
			print_message(FAIL, "long", (double)cfloat, "float",
				(double)cint);
		else
			print_message(PASS, "long", (double)cfloat, "float",
				(double)cint);

		adouble = (double)aint;
		bdouble = (double)bint;
		cdouble = adouble + bdouble;
		cint = aint + bint;
		if (cdouble != (double)cint)
			print_message(FAIL, "long", cdouble, "double",
				(double)cint);
		else
			print_message(PASS, "long", cdouble, "double",
				(double)cint);

		aint = (long)adouble;
		bint = (long)bdouble;
		cint = aint + bint;
		cdouble = adouble + bdouble;
		if (cint != (long)cdouble)
			print_message(FAIL, "double", (double)cint, "long",
				cdouble);
		else
			print_message(PASS, "double", (double)cint, "long",
				cdouble);

		adouble = (double)afloat;
		bdouble = (double)bfloat;
		cdouble = adouble + bdouble;
		cfloat = afloat + bfloat;
		if (cdouble != (double)cfloat)
			print_message(FAIL, "float", cdouble, "double",
				(double)cfloat);
		else
			print_message(PASS, "float", cdouble, "double",
				(double)cfloat);

		afloat = (float)adouble;
		bfloat = (float)bdouble;
		cfloat = afloat + bfloat;
		cdouble = adouble + bdouble;
		if (cfloat != (float)cdouble)
			print_message(FAIL, "double", (double)cfloat, "float",
				cdouble);
		else
			print_message(PASS, "double", (double)cfloat, "float",
				cdouble);



	}
	if (bug_found)
		printf(ERRORS);
	else
		printf (NO_ERRORS);
}



print_message(flag, type1, opr1, type2, opr2)
int	flag;
char	*type1;
double	opr1;
char	*type2;
double	opr2;
{
	switch (flag)
	{
	case FAIL:
		printf("ERROR:  CONVERT (%s) %f ---> (%s) %f\n",
			type1, opr1, type2, opr2);
		++bug_found;
		break;
	case PASS:
	default:
		break;
	}
}
 
