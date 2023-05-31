#include "lib_top.h"


#pragma	option	-nx

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) 1991,1992,1993  Microtec Research, Inc.		*/
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

/*    %W% %G% */

/* 0printf.c - program to test printf()  (non-floating pt.) */

/*	
*	04/08/94 jpc -	Added tests for spr 32202 and spr 34222
*/

#include <stdio.h>
#include "message.h"

#define compare(R,X)	{ \
			if (strcmp (output, (char *) R )) \
				{ \
				printf (errorFormat, \
					testNo, (char *) R,\
					output, X); \
				errorCount++; \
				} \
			testNo++; \
			}

#define	test(R,X,Y)	sprintf (output, X, Y);\
			compare (R,X)

#define	test2(R,X,Y,Z)	sprintf (output, X, Y, Z);\
			compare (R,X)

#define	test3(R,X,Y,Z,A)	sprintf (output, X, Y, Z, A);\
			compare (R,X)

static const char errorFormat[] = 
	"ERROR %3d expected = '%s' output = '%s' control = '%s'\n";

main()
{
  int 	i;
  char	output[256];
  int errorCount = 0;
  int testNo = 0;
  int dummy;
  

test("Hello, world", 		"Hello, world",	dummy);
test("Hello, world", 		"%s",		"Hello, world");
test("fred           ",		"%-15.10s",	"fred");	/* spr 34222 */
test("<SEQ   ",			"<%-6.6s",	"SEQ");		/* spr 38782 */

sprintf (output, "%c%c%c%c%c%c%c%c%c%c%c%c",
	'H','e','l','l','o',',',' ','w','o','r','l','d');
compare ("Hello, world", "%c%c%c%c%c%c%c%c%c%c%c%c");

test("Hello, wor",		"%10.10s",	"Hello, world");
test("          ",		"%10.s",	"Hello, world");
test("Hello, wor",		"%.10s",	"Hello, world");
test("Hello, world",		"%-10s",	"Hello, world");
test("Hello, world        ",	"%-20s",	"Hello, world");
test("        Hello, world",	"%20s",		"Hello, world");

test("",		"%.0d",		0);
test("",		"%.0i",		0);
test("1",		"%d",		1);
test("",		"%.0u",		0);
test("2",		"%u",		2);
test("3",		"%ld",		3L);
test("4",		"%lu",		4L);
test("5",		"%1d",		5);
test("6",		"%1u",		6);
test("7",		"%1ld",		7L);
test("8",		"%1lu",		8L);
test("9",		"%x",		9);
test("a",		"%x",		10);
test("9",		"%X",		9);
test("A",		"%X",		10);
test("9",		"%0x",		9);
test("a",		"%0x",		10);
test("9",		"%0X",		9);
test("A",		"%0X",		10);

test("         1",	"%10d",		1);
test("         2",	"%10u",		2);
test("         3",	"%10ld",	3L);
test("         4",	"%10lu",	4L);
test("         5",	"%10d",		5);
test("         6",	"%10u",		6);
test("         7",	"%10ld",	7L);
test("         8",	"%10lu",	8L);
test("         9",	"%10x",		9);
test("         a",	"%10x",		10);
test("         9",	"%10X",		9);
test("         A",	"%10X",		10);

test("0000000001",	"%010d",	1);
test("0000000002",	"%010u",	2);
test("0000000003",	"%010ld",	3L);
test("0000000004",	"%010lu",	4L);
test("0000000005",	"%010d",	5);
test("0000000006",	"%010u",	6);
test("0000000007",	"%010ld",	7L);
test("0000000008",	"%010lu",	8L);
test("0000000009",	"%010x",	9);
test("000000000a",	"%010x",	10);
test("0000000009",	"%010X",	9);
test("000000000A",	"%010X",	10);

test("1         ",	"%-10d",	1);
test("2         ",	"%-10u",	2);
test("3         ",	"%-10ld",	3L);
test("4         ",	"%-10lu",	4L);
test("5         ",	"%-10d",	5);
test("6         ",	"%-10u",	6);
test("7         ",	"%-10ld",	7L);
test("8         ",	"%-10lu",	8L);
test("9         ",	"%-10x",	9);
test("a         ",	"%-10x",	10);
test("9         ",	"%-10X",	9);
test("A         ",	"%-10X",	10);

test("1         ",	"%-010d",	1);
test("2         ",	"%-010u",	2);
test("3         ",	"%-010ld",	3L);
test("4         ",	"%-010lu",	4L);
test("5         ",	"%-010d",	5);
test("6         ",	"%-010u",	6);
test("7         ",	"%-010ld",	7L);
test("8         ",	"%-010lu",	8L);
test("9         ",	"%-010x",	9);
test("a         ",	"%-010x",	10);
test("9         ",	"%-010X",	9);
test("A         ",	"%-010X",	10);

	test("",		"%.0x",		0);
	test("",		"%.0X",		0);
#if _SIZEOF_INT == _SIZEOF_SHORT
	test("-1",		"%d",		-1);
	test("65534",		"%u",		-2);
	test("-3",		"%ld",		-3L);
	test("4294967292",	"%lu",		-4L);
	test("-5",		"%1d",		-5);
	test("65530",		"%1u",		-6);
	test("-7",		"%1ld",		-7L);
	test("4294967288",	"%1lu",		-8L);
	test("fff7",		"%x",		-9);
	test("fff6",		"%x",		-10);
	test("FFF7",		"%X",		-9);
	test("FFF6",		"%X",		-10);
	test("fff7",		"%0x",		-9);
	test("fff6",		"%0x",		-10);
	test("FFF7",		"%0X",		-9);
	test("FFF6",		"%0X",		-10);
#else
	test("-1",		"%d",		-1);
	test("4294967294",	"%u",		-2);
	test("-3",		"%ld",		-3L);
	test("4294967292",	"%lu",		-4L);
	test("-5",		"%1d",		-5);
	test("4294967290",	"%1u",		-6);
	test("-7",		"%1ld",		-7L);
	test("4294967288",	"%1lu",		-8L);
	test("fffffff7",	"%x",		-9);
	test("fffffff6",	"%x",		-10);
	test("FFFFFFF7",	"%X",		-9);
	test("FFFFFFF6",	"%X",		-10);
	test("fffffff7",	"%0x",		-9);
	test("fffffff6",	"%0x",		-10);
	test("FFFFFFF7",	"%0X",		-9);
	test("FFFFFFF6",	"%0X",		-10);
#endif

#if _SIZEOF_INT == _SIZEOF_SHORT
	test("        -1",	"%10d",		-1);
	test("     65534",	"%10u",		-2);
	test("        -3",	"%10ld",	-3L);
	test("4294967292",	"%10lu",	-4L);
	test("        -5",	"%10d",		-5);
	test("     65530",	"%10u",		-6);
	test("        -7",	"%10ld",	-7L);
	test("4294967288",	"%10lu",	-8L);
#else
	test("        -1",	"%10d",		-1);
	test("4294967294",	"%10u",		-2);
	test("        -3",	"%10ld",	-3L);
	test("4294967292",	"%10lu",	-4L);
	test("        -5",	"%10d",		-5);
	test("4294967290",	"%10u",		-6);
	test("        -7",	"%10ld",	-7L);
	test("4294967288",	"%10lu",	-8L);
#endif

#if _SIZEOF_INT == _SIZEOF_SHORT
	test("-000000001",	"%010d",	-1);
	test("0000065534",	"%010u",	-2);
	test("-000000003",	"%010ld",	-3L);
	test("4294967292",	"%010lu",	-4L);
	test("-000000005",	"%010d",	-5);
	test("0000065530",	"%010u",	-6);
	test("-000000007",	"%010ld",	-7L);
	test("4294967288",	"%010lu",	-8L);
#else
	test("-000000001",	"%010d",	-1);
	test("4294967294",	"%010u",	-2);
	test("-000000003",	"%010ld",	-3L);
	test("4294967292",	"%010lu",	-4L);
	test("-000000005",	"%010d",	-5);
	test("4294967290",	"%010u",	-6);
	test("-000000007",	"%010ld",	-7L);
	test("4294967288",	"%010lu",	-8L);
#endif

#if _SIZEOF_INT == _SIZEOF_SHORT
	test("-1        ",	"%-10d",	-1);
	test("65534     ",	"%-10u",	-2);
	test("-3        ",	"%-10ld",	-3L);
	test("4294967292",	"%-10lu",	-4L);
	test("-5        ",	"%-10d",	-5);
	test("65530     ",	"%-10u",	-6);
	test("-7        ",	"%-10ld",	-7L);
	test("4294967288",	"%-10lu",	-8L);
#else
	test("-1        ",	"%-10d",	-1);
	test("4294967294",	"%-10u",	-2);
	test("-3        ",	"%-10ld",	-3L);
	test("4294967292",	"%-10lu",	-4L);
	test("-5        ",	"%-10d",	-5);
	test("4294967290",	"%-10u",	-6);
	test("-7        ",	"%-10ld",	-7L);
	test("4294967288",	"%-10lu",	-8L);
#endif

#if _SIZEOF_INT == _SIZEOF_SHORT
	test("-1        ",	"%-010d",	-1);
	test("65534     ",	"%-010u",	-2);
	test("-3        ",	"%-010ld",	-3L);
	test("4294967292",	"%-010lu",	-4L);
	test("-5        ",	"%-010d",	-5);
	test("65530     ",	"%-010u",	-6);
	test("-7        ",	"%-010ld",	-7L);
	test("4294967288",	"%-010lu",	-8L);
#else
	test("-1        ",	"%-010d",	-1);
	test("4294967294",	"%-010u",	-2);
	test("-3        ",	"%-010ld",	-3L);
	test("4294967292",	"%-010lu",	-4L);
	test("-5        ",	"%-010d",	-5);
	test("4294967290",	"%-010u",	-6);
	test("-7        ",	"%-010ld",	-7L);
	test("4294967288",	"%-010lu",	-8L);
#endif

test("        +1",	"%+10d",	1);
test("         2",	"%+10u",	2);
test("        +3",	"%+10ld",	3L);
test("         4",	"%+10lu",	4L);
test("        +5",	"%+10d",	5);
test("         6",	"%+10u",	6);
test("        +7",	"%+10ld",	7L);
test("         8",	"%+10lu",	8L);

test("+000000001",	"%+010d",	1);
test("0000000002",	"%+010u",	2);
test("+000000003",	"%+010ld",	3L);
test("0000000004",	"%+010lu",	4L);
test("+000000005",	"%+010d",	5);
test("0000000006",	"%+010u",	6);
test("+000000007",	"%+010ld",	7L);
test("0000000008",	"%+010lu",	8L);

test("         1",	"% 10d", 	1);
test("         2",	"% 10u", 	2);
test("         3",	"% 10ld", 	3L);
test("         4",	"% 10lu", 	4L);
test("         5",	"% 10d", 	5);
test("         6",	"% 10u", 	6);
test("         7",	"% 10ld", 	7L);
test("         8",	"% 10lu", 	8L);

test(" 000000001",	"% 010d", 	1);
test("0000000002",	"% 010u", 	2);
test(" 000000003",	"% 010ld", 	3L);
test("0000000004",	"% 010lu", 	4L);
test(" 000000005",	"% 010d", 	5);
test("0000000006",	"% 010u", 	6);
test(" 000000007",	"% 010ld", 	7L);
test("0000000008",	"% 010lu", 	8L);

#if _SIZEOF_INT == _SIZEOF_SHORT
	test("        -1",	"%+10d",	-1);
	test("     65534",	"%+10u",	-2);
	test("        -3",	"%+10ld",	-3L);
	test("4294967292",	"%+10lu",	-4L);
	test("        -5",	"%+10d",	-5);
	test("     65530",	"%+10u",	-6);
	test("        -7",	"%+10ld",	-7L);
	test("4294967288",	"%+10lu",	-8L);
#else
	test("        -1",	"%+10d",	-1);
	test("4294967294",	"%+10u",	-2);
	test("        -3",	"%+10ld",	-3L);
	test("4294967292",	"%+10lu",	-4L);
	test("        -5",	"%+10d",	-5);
	test("4294967290",	"%+10u",	-6);
	test("        -7",	"%+10ld",	-7L);
	test("4294967288",	"%+10lu",	-8L);
#endif

#if _SIZEOF_INT == _SIZEOF_SHORT
	test("-000000001",	"%+010d",	-1);
	test("0000065534",	"%+010u",	-2);
	test("-000000003",	"%+010ld",	-3L);
	test("4294967292",	"%+010lu",	-4L);
	test("-000000005",	"%+010d",	-5);
	test("0000065530",	"%+010u",	-6);
	test("-000000007",	"%+010ld",	-7L);
	test("4294967288",	"%+010lu",	-8L);
#else
	test("-000000001",	"%+010d",	-1);
	test("4294967294",	"%+010u",	-2);
	test("-000000003",	"%+010ld",	-3L);
	test("4294967292",	"%+010lu",	-4L);
	test("-000000005",	"%+010d",	-5);
	test("4294967290",	"%+010u",	-6);
	test("-000000007",	"%+010ld",	-7L);
	test("4294967288",	"%+010lu",	-8L);
#endif

#if _SIZEOF_INT == _SIZEOF_SHORT
	test("        -1",	"% 10d",	-1);
	test("     65534",	"% 10u",	-2);
	test("        -3",	"% 10ld",	-3L);
	test("4294967292",	"% 10lu",	-4L);
	test("        -5",	"% 10d",	-5);
	test("     65530",	"% 10u",	-6);
	test("        -7",	"% 10ld",	-7L);
	test("4294967288",	"% 10lu",	-8L);
#else
	test("        -1",	"% 10d",	-1);
	test("4294967294",	"% 10u",	-2);
	test("        -3",	"% 10ld",	-3L);
	test("4294967292",	"% 10lu",	-4L);
	test("        -5",	"% 10d",	-5);
	test("4294967290",	"% 10u",	-6);
	test("        -7",	"% 10ld",	-7L);
	test("4294967288",	"% 10lu",	-8L);
#endif

#if _SIZEOF_INT == _SIZEOF_SHORT
	test("-000000001",	"% 010d",	-1);
	test("0000065534",	"% 010u",	-2);
	test("-000000003",	"% 010ld",	-3L);
	test("4294967292",	"% 010lu",	-4L);
	test("-000000005",	"% 010d",	-5);
	test("0000065530",	"% 010u",	-6);
	test("-000000007",	"% 010ld",	-7L);
	test("4294967288",	"% 010lu",	-8L);
#else
	test("-000000001",	"% 010d",	-1);
	test("4294967294",	"% 010u",	-2);
	test("-000000003",	"% 010ld",	-3L);
	test("4294967292",	"% 010lu",	-4L);
	test("-000000005",	"% 010d",	-5);
	test("4294967290",	"% 010u",	-6);
	test("-000000007",	"% 010ld",	-7L);
	test("4294967288",	"% 010lu",	-8L);
#endif

#if _SIZEOF_DATA_POINTER == 2
	test("DEAD",		"%p",		(char *) 0xaddead);
	test("DEAD",		"%p",		(char *) 0xaddead);
	test("DEAD",		"%0p",		(char *) 0xaddead);
	test("DEAD",		"%0p",		(char *) 0xaddead);
	test("      DEAD",	"%10p",		(char *) 0xaddead);
	test("      DEAD",	"%10p",		(char *) 0xaddead);
	test("000000DEAD",	"%010p",	(char *) 0xaddead);
	test("000000DEAD",	"%010p",	(char *) 0xaddead);
	test("DEAD      ",	"%-10p",	(char *) 0xaddead);
	test("DEAD      ",	"%-10p",	(char *) 0xaddead);
	test("DEAD      ",	"%-010p",	(char *) 0xaddead);
	test("DEAD      ",	"%-010p",	(char *) 0xaddead);
#else
	test("ADDEAD",		"%p",		(char *) 0xaddead);
	test("ADDEAD",		"%p",		(char *) 0xaddead);
	test("ADDEAD",		"%0p",		(char *) 0xaddead);
	test("ADDEAD",		"%0p",		(char *) 0xaddead);
	test("    ADDEAD",	"%10p",		(char *) 0xaddead);
	test("    ADDEAD",	"%10p",		(char *) 0xaddead);
	test("0000ADDEAD",	"%010p",	(char *) 0xaddead);
	test("0000ADDEAD",	"%010p",	(char *) 0xaddead);
	test("ADDEAD    ",	"%-10p",	(char *) 0xaddead);
	test("ADDEAD    ",	"%-10p",	(char *) 0xaddead);
	test("ADDEAD    ",	"%-010p",	(char *) 0xaddead);
	test("ADDEAD    ",	"%-010p",	(char *) 0xaddead);
#endif

	test ("",		"%.0o",		0);
#if _SIZEOF_INT == _SIZEOF_SHORT
	test ("0",		"%#o",		0);
	test ("01",		"%#o",		1);
	test ("0177777",	"%#o",		-1);
	test ("0531",		"%#.4o",	345);	/* question to ANSI C */
	test ("0",		"%#x",		0);
	test ("0x1",		"%#x",		1);
	test ("0xffff",		"%#x",		-1);
	test ("0",		"%#X",		0);
	test ("0X1",		"%#X",		1);
	test ("0XFFFF",		"%#X",		-1);
#else
	test ("0",		"%#o", 		0);
	test ("01",		"%#o", 		1);
	test ("037777777777",	"%#o", 		-1);
	test ("0531",		"%#.4o", 	345);	/* question to ANSI C */
	test ("0",		"%#x", 		0);
	test ("0x1",		"%#x", 		1);
	test ("0xffffffff",	"%#x", 		-1);
	test ("0",		"%#X", 		0);
	test ("0X1",		"%#X", 		1);
	test ("0XFFFFFFFF",	"%#X", 		-1);
#endif

test ("0.",		"%#.0f",	0.0);
test ("1.",		"%#.0f",	1.0);
test ("-1.",		"%#.0f",	-1.0);
test ("0.00000",	"%#g",		0.0);
test ("1.00000",	"%#g",		1.0);
test ("-1.00000",	"%#g",		-1.0);
test ("0.00000",	"%#G",		0.0);
test ("1.00000",	"%#G",		1.0);
test ("-1.00000",	"%#G",		-1.0);
test ("0.e+00",		"%#.0e",	0.0);
test ("1.e+00",		"%#.0e",	1.0);
test ("-1.e+00",	"%#.0e",	-1.0);
test ("0.E+00",		"%#.0E",	0.0);
test ("1.E+00",		"%#.0E",	1.0);
test ("-1.E+00",	"%#.0E",	-1.0);

test2("        Hello, world",	"%*s",	20,	"Hello, world");
test2("          ",		"%*.s", 10,	"Hello, world");
test2("Hello, world        ",	"%-*s", 20,	"Hello, world");
test2("Hello, wor",		"%.*s", 10,	"Hello, world");
test2("Hello, world",		"%-*s", 10,	"Hello, world");
test2("          ",		"%*.s", -10,	"Hello, world");
test2("Hello, world",		"%.*s", -10,	"Hello, world");
test2("Hello, world",		"%-*s", -10,	"Hello, world");
test2("Hello, world        ",	"%-*s", -20,	"Hello, world");
test2("Hello, world        ",	"%*s",  -20,	"Hello, world");

test3("Hello, wor",		"%*.*s",10,	10,	"Hello, world");
test3("Hello, world",		"%*.*s",-10,	-10,	"Hello, world");

sprintf(output, "%n", &i);
test ("i = 0",	"i = %d",	i);
sprintf(output, "1234567890%n", &i);
test ("i = 10",	"i = %d",	i);
sprintf (output, "%n%n", &i, &i);
test ("i = 0",	"i = %d",	i);
sprintf(output, "%d%n", 145, &i);
test ("i = 3",	"i = %d",	i);

test ("     00025",	"%010.5d",	25);
test ("00025     ",	"%-010.5d",	25);
test ("     00025",	"%10.5d",	25);
test ("00025     ",	"%-10.5d",	25);
test ("     00025",	"% 010.5d",	25);
test ("    +00025",	"%+010.5d",	25);
test ("+25",		"%+02d",	25);

test ("-0025",		"%.4d",		-25);
test ("  073",		"%05.3o",	59);
test ("  0000000059",	"%012.10u",	59);
test (" 03b",		"%04.3x",	59);
test (" 03B",		"%04.3X",	59);

if (errorCount)
    printf (ERRORS);
else
    printf (NO_ERRORS);
}
 
