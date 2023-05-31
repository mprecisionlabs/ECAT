#include "lib_top.h"


#pragma	option	-nx

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1992 Microtec Research, Inc.       			    */
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

#include <stdio.h>
#include "message.h"

int i;
int errorCount;
char outString [256];

struct {
	char format[10];
	char result[10];
	} control [] = {
	{"%x",	"0"},
	{"%d",	"0"},
	{"%x",	"7fff"},
	{"%d",	"32767"},
#if _SIZEOF_INT == _SIZEOF_LONG
	{"%x",	"ffffffff"},
#else
	{"%x",	"ffff"},
#endif
	{"%d",	"-1"},
#if _SIZEOF_INT == _SIZEOF_LONG
	{"%x",	"ffffffff"},
#else
	{"%x",	"ffff"},
#endif
	{"%d",	"-1"},
	{"%d",	"0"},
	{"%d",	"10922"},
	{"%d",	"-10922"},
	{"%d",	"-10922"},
	{"%d",	"10922"},
	{"%d",	"2"},
	{"%d",	"2"},
	{"%d",	"-2"},
	{"%d",	"0"},
	{"%d",	"-2"},
	{"%d",	"2"} };
		

doError (test)
int test;
{
    printf ("Error: test %d; expected: %s actual: %s\n", test,
	control[test].result, outString);
    errorCount++;
}

test (inValue)
int inValue;
{
    sprintf (outString, control[i].format, inValue);
    if (strcmp (outString, control[i].result))
	doError (i);
    ++i;
}

main()				
{
	short s1=0, s2=32767, s3=0xffff, s4=0xffff;
	short f1,f2;

test (s1);
test (s1);
test (s2);
test (s2);
test (s3);
test (s3);
test (s4);
test (s4);

f1= s3-1; f1++;

test (s1/s3);
test (s2/3);
test (-s2/3);
test (s2/-3);
test (-s2/-3);
test ((f1+s4)/f1);
test ((s3+s4)/s3);
test ((s3+s4)/-s3);
test ((s3-s4)/s3);
test ((-s3-s4)/s3);
test ((-s3-s4)/-s3);

if (errorCount)
    printf (ERRORS);
else
    printf (NO_ERRORS);
}
 
