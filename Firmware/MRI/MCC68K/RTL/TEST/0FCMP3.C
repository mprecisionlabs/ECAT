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
**	f_cmp.c
**	Test compare a_doublend b_doubleranch
*/

#include <stdio.h>

#include "message.h"

typedef	double	X;

#define	PASS	0
#define FAIL	1

int	bug_found;

	double	a_double, b_double;
	float	a_float, b_float;


main()
{
	a_double = 2345.0;
	b_double = 8976.0;
	if ( a_double == b_double)
		print_message(FAIL, "double", a_double, "==", b_double);
	else
		print_message(PASS, "double", a_double, "==", b_double);

	if ( a_double != b_double)
		print_message(PASS, "double", a_double, "!=", b_double);
	else
		print_message(FAIL, "double", a_double, "!=", b_double);

	if ( a_double < b_double)
		print_message(PASS, "double", a_double, "<", b_double);
	else
		print_message(FAIL, "double", a_double, "<", b_double);

	if ( a_double <= b_double)
		print_message(PASS, "double", a_double, "<=", b_double);
	else
		print_message(FAIL, "double", a_double, "<=", b_double);

	if ( a_double > b_double)
		print_message(FAIL, "double", a_double, ">", b_double);
	else
		print_message(PASS, "double", a_double, ">", b_double);

	if ( a_double >= b_double)
		print_message(FAIL, "double", a_double, ">=", b_double);
	else
		print_message(PASS, "double", a_double, ">=", b_double);

	a_double = -2345.0;
	b_double = -8976.0;
	if ( a_double == b_double)
		print_message(FAIL, "double", a_double, "==", b_double);
	else
		print_message(PASS, "double", a_double, "==", b_double);

	if ( a_double != b_double)
		print_message(PASS, "double", a_double, "!=", b_double);
	else
		print_message(FAIL, "double", a_double, "!=", b_double);

	if ( a_double < b_double)
		print_message(FAIL, "double", a_double, "<", b_double);
	else
		print_message(PASS, "double", a_double, "<", b_double);

	if ( a_double <= b_double)
		print_message(FAIL, "double", a_double, "<=", b_double);
	else
		print_message(PASS, "double", a_double, "<=", b_double);

	if ( a_double > b_double)
		print_message(PASS, "double", a_double, ">", b_double);
	else
		print_message(FAIL, "double", a_double, ">", b_double);

	if ( a_double >= b_double)
		print_message(PASS, "double", a_double, ">=", b_double);
	else
		print_message(FAIL, "double", a_double, ">=", b_double);

	a_double = 222.0;
	b_double = 222.0;
	if ( a_double == b_double)
		print_message(PASS, "double", a_double, "==", b_double);
	else
		print_message(FAIL, "double", a_double, "==", b_double);

	if ( a_double != b_double)
		print_message(FAIL, "double", a_double, "!=", b_double);
	else
		print_message(PASS, "double", a_double, "!=", b_double);

	if ( a_double < b_double)
		print_message(FAIL, "double", a_double, "<", b_double);
	else
		print_message(PASS, "double", a_double, "<", b_double);

	if ( a_double <= b_double)
		print_message(PASS, "double", a_double, "<=", b_double);
	else
		print_message(FAIL, "double", a_double, "<=", b_double);

	if ( a_double > b_double)
		print_message(FAIL, "double", a_double, ">", b_double);
	else
		print_message(PASS, "double", a_double, ">", b_double);

	if ( a_double >= b_double)
		print_message(PASS, "double", a_double, ">=", b_double);
	else
		print_message(FAIL, "double", a_double, ">=", b_double);

	a_double = 222.0;
	b_double = -222.0;
	if ( a_double == b_double)
		print_message(FAIL, "double", a_double, "==", b_double);
	else
		print_message(PASS, "double", a_double, "==", b_double);

	if ( a_double != b_double)
		print_message(PASS, "double", a_double, "!=", b_double);
	else
		print_message(FAIL, "double", a_double, "!=", b_double);

	if ( a_double < b_double)
		print_message(FAIL, "double", a_double, "<", b_double);
	else
		print_message(PASS, "double", a_double, "<", b_double);

	if ( a_double <= b_double)
		print_message(FAIL, "double", a_double, "<=", b_double);
	else
		print_message(PASS, "double", a_double, "<=", b_double);

	if ( a_double > b_double)
		print_message(PASS, "double", a_double, ">", b_double);
	else
		print_message(FAIL, "double", a_double, ">", b_double);

	if ( a_double >= b_double)
		print_message(PASS, "double", a_double, ">=", b_double);
	else
		print_message(FAIL, "double", a_double, ">=", b_double);


	a_float = 2345.0;
	b_float = 8976.0;
	if ( a_float == b_float)
		print_message(FAIL, "float", a_float, "==", b_float);
	else
		print_message(PASS, "float", a_float, "==", b_float);

	if ( a_float != b_float)
		print_message(PASS, "float", a_float, "!=", b_float);
	else
		print_message(FAIL, "float", a_float, "!=", b_float);

	if ( a_float < b_float)
		print_message(PASS, "float", a_float, "<", b_float);
	else
		print_message(FAIL, "float", a_float, "<", b_float);

	if ( a_float <= b_float)
		print_message(PASS, "float", a_float, "<=", b_float);
	else
		print_message(FAIL, "float", a_float, "<=", b_float);

	if ( a_float > b_float)
		print_message(FAIL, "float", a_float, ">", b_float);
	else
		print_message(PASS, "float", a_float, ">", b_float);

	if ( a_float >= b_float)
		print_message(FAIL, "float", a_float, ">=", b_float);
	else
		print_message(PASS, "float", a_float, ">=", b_float);

	a_float = -2345.0;
	b_float = -8976.0;
	if ( a_float == b_float)
		print_message(FAIL, "float", a_float, "==", b_float);
	else
		print_message(PASS, "float", a_float, "==", b_float);

	if ( a_float != b_float)
		print_message(PASS, "float", a_float, "!=", b_float);
	else
		print_message(FAIL, "float", a_float, "!=", b_float);

	if ( a_float < b_float)
		print_message(FAIL, "float", a_float, "<", b_float);
	else
		print_message(PASS, "float", a_float, "<", b_float);

	if ( a_float <= b_float)
		print_message(FAIL, "float", a_float, "<=", b_float);
	else
		print_message(PASS, "float", a_float, "<=", b_float);

	if ( a_float > b_float)
		print_message(PASS, "float", a_float, ">", b_float);
	else
		print_message(FAIL, "float", a_float, ">", b_float);

	if ( a_float >= b_float)
		print_message(PASS, "float", a_float, ">=", b_float);
	else
		print_message(FAIL, "float", a_float, ">=", b_float);

	a_float = 222.0;
	b_float = 222.0;
	if ( a_float == b_float)
		print_message(PASS, "float", a_float, "==", b_float);
	else
		print_message(FAIL, "float", a_float, "==", b_float);

	if ( a_float != b_float)
		print_message(FAIL, "float", a_float, "!=", b_float);
	else
		print_message(PASS, "float", a_float, "!=", b_float);

	if ( a_float < b_float)
		print_message(FAIL, "float", a_float, "<", b_float);
	else
		print_message(PASS, "float", a_float, "<", b_float);

	if ( a_float <= b_float)
		print_message(PASS, "float", a_float, "<=", b_float);
	else
		print_message(FAIL, "float", a_float, "<=", b_float);

	if ( a_float > b_float)
		print_message(FAIL, "float", a_float, ">", b_float);
	else
		print_message(PASS, "float", a_float, ">", b_float);

	if ( a_float >= b_float)
		print_message(PASS, "float", a_float, ">=", b_float);
	else
		print_message(FAIL, "float", a_float, ">=", b_float);

	a_float = 222.0;
	b_float = -222.0;
	if ( a_float == b_float)
		print_message(FAIL, "float", a_float, "==", b_float);
	else
		print_message(PASS, "float", a_float, "==", b_float);

	if ( a_float != b_float)
		print_message(PASS, "float", a_float, "!=", b_float);
	else
		print_message(FAIL, "float", a_float, "!=", b_float);

	if ( a_float < b_float)
		print_message(FAIL, "float", a_float, "<", b_float);
	else
		print_message(PASS, "float", a_float, "<", b_float);

	if ( a_float <= b_float)
		print_message(FAIL, "float", a_float, "<=", b_float);
	else
		print_message(PASS, "float", a_float, "<=", b_float);

	if ( a_float > b_float)
		print_message(PASS, "float", a_float, ">", b_float);
	else
		print_message(FAIL, "float", a_float, ">", b_float);

	if ( a_float >= b_float)
		print_message(PASS, "float", a_float, ">=", b_float);
	else
		print_message(FAIL, "float", a_float, ">=", b_float);


	if (bug_found)
		printf (ERRORS);
	else
		printf (NO_ERRORS);
}


print_message(flag, type_str, oprnd1, opr, oprnd2)
int		flag;
char		*type_str;
double		oprnd1;
char		*opr;
double		oprnd2;
{
	switch (flag)
	{
	case FAIL:
		printf("ERROR:  compare (%s) %f %s %f\n",
			type_str, oprnd1, opr, oprnd2);
		++bug_found;
		break;
	case PASS:
	default:
		break;
	}
}
 
