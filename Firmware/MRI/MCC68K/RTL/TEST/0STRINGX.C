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

/* pgm to test stringx functions - written by H. Yuen  05/30/85 */
/*	*/
/*	Changes:	*/
/*	7/11/89	jpc: Added test complete	*/
/*	3/15/94 jpc: Added test for spr 34474	*/

/*	%W% %G%	*/

#include <string.h>
#include <mriext.h>
#include <stdio.h>

#if sun
    #include	<memory.h>
    extern void	*memmove();
#endif

#include "message.h"
#pragma	options -nx

extern char *strpbrk(), *strtok();
/* char test[] = "This is a test of the stringx functions\n"; */
	/***   123456789 123456789 123456789 123456789  ***/
/* char xtok[] = "\tfor (i=1; i<100; i++) if (abcd) break;\n"; */

static	int globError;
static  long zap;

main()
{
	register char *p;
	register int  i;
	char tok[128];
	char test[200];
	char xtok[100];
	long	tmemcpy[10], tmemcpy2[10];

	strcpy(test,"This is a test of the stringx functions\n");
	strcpy(xtok,"\tfor (i=1; i<100; i++) if (abcd) break;\n");
	if (strpbrk(test,"bad") != &test[8])	error(1);
	if (strpbrk(test,"\t\f\r\b\\y"))	error(2);
	if (strpbrk(test,"xyz ab") != &test[4]) error(3);
	if (strpbrk(test,"xyz") != &test[28])	error(4);
	if (strpbrk(test,""))			error(5);

	if (strspn(test,"TBhijkabcd") != 3)		error(11);
	if (strspn(test,"test is a hT") != 15)		error(12);
	if (strspn(test,"testisahTofstring") != 4)	error(13);
	if (strspn(test,"testisahTofstring ") != 28)	error(14);
	if (strspn(test,"SORRY!") != 0)			error(15);
	if (strspn(test,"") != 0)			error(16);

	if (strcspn(test,"testisaofstring") != 2)	error(21);
	if (strcspn(test,"OK I AM DONE !") != 4)	error(22);
	if (strcspn(test,"0123456789\n") != 39)		error(23);
	if (strcspn(test,"OK THIS IS IT !") != 0)	error(24);
	if (strcspn(test,"") != 40)			error(25);

	strcpy(tok,xtok);
	if (strcmp(strtok("ABC",""),"ABC"))		error(30);
	if (strtok(NULLPTR,"abc test"))			error(31);
	if (strcmp(strtok(tok," \t\n"),"for"))		error(32);
	if (strcmp(strtok(NULLPTR," \t\n;"),"(i=1"))	error(33);
	if (strcmp(strtok(NULLPTR," \t\n"),"i<100;"))	error(34);
	if (strcmp(strtok(NULLPTR,"[]"),
			"i++) if (abcd) break;\n"))	error(35);
	if (strtok(NULLPTR," \t\n;"))			error(36);

        if (strcmp ("Test String", "Test Strinh") >= 0) error(37);
        if (strcmp ("Test String", "Test Strinf") <= 0) error(38);
        if (strcmp ("Test String", "Test String1") >= 0)        error(39);
        if (strcmp ("Test String1", "Test String") <= 0)        error(40);

        if (strcmp ("Test String"+1, "Test Strinh"+1) >= 0)     error(41);
        if (strcmp ("Test String"+1, "Test Strinf"+1) <= 0)     error(42);
        if (strcmp ("Test String"+1, "Test String1"+1) >= 0)    error(43);
        if (strcmp ("Test String1"+1, "Test String"+1) <= 0)    error(44);

						/* Sorry dumplicate error #'s */
        if (strcmp ("Test String", "1Test Strinh"+1) >= 0)     error(41);
        if (strcmp ("Test String", "1Test Strinf"+1) <= 0)     error(42);
        if (strcmp ("Test String", "1Test String1"+1) >= 0)    error(43);
        if (strcmp ("Test String1", "1Test String"+1) <= 0)    error(44);


	memclr ((char *) tmemcpy, sizeof(tmemcpy));
	memset (tmemcpy2, 5, sizeof(tmemcpy2));
	if (memcpy (tmemcpy, tmemcpy2, sizeof(tmemcpy)-4) != (char *) tmemcpy)
								error(45);
	if (memcmp (&tmemcpy[8], "\5\5\5\5\0\0\0\0", 8))	error(46);

	memclr ((char *) tmemcpy, sizeof(tmemcpy));
	if (memcpy (tmemcpy, tmemcpy2, sizeof(tmemcpy) - 3) != (char *) tmemcpy)
								error(47);
	if (memcmp (&tmemcpy[8], "\5\5\5\5\5\0\0\0", 8))	error(48);

	memclr ((char *) tmemcpy, sizeof(tmemcpy));
	if (memcpy ((short *)tmemcpy+1,
		    (short *)tmemcpy2+1,
		    sizeof(tmemcpy) - 4) != (char *) ((short *)tmemcpy+1))
			error(49);
	if (memcmp (&tmemcpy[8], "\5\5\5\5\5\5\0\0", 8))	error(50);

	memclr ((char *) tmemcpy, sizeof(tmemcpy));
	if (memcpy ((short *)tmemcpy+1,
		    (char *)tmemcpy2+1,
		    sizeof(tmemcpy) - 4) != (char *) ((short *)tmemcpy+1))
			error(51);
	if (memcmp (&tmemcpy[8], "\5\5\5\5\5\5\0\0", 8))	error(52);

	memclr ((char *) tmemcpy, sizeof(tmemcpy));
	if (memcpy ((short *)tmemcpy+1,
		    (short *)tmemcpy2+1,
		    sizeof(tmemcpy) - 3) != (char *) ((short *)tmemcpy+1))
			error(53);
	if (memcmp (&tmemcpy[8], "\5\5\5\5\5\5\5\0", 8))	error(54);

	memclr ((char *) tmemcpy, sizeof(tmemcpy));
	if (memcpy ((char *)tmemcpy+1,
		    (char *)tmemcpy2+1,
		    sizeof(tmemcpy) - 3) != (char *)tmemcpy+1)	error(55);
	if (memcmp (&tmemcpy[8], "\5\5\5\5\5\5\0\0", 8))	error(56);

/* Do some tests of memcmp */

	if (memcmp ("Long String", "Long String", 11))
								error(57);
	if (memcmp ("Long String"+1, "Long String"+1, 11-1))
								error(58);
	if (memcmp ("1Long String"+1, "Long String", 11))
								error(59);
	if (memcmp ("Long String", "1Long String"+1, 11))
								error(60);
	if (memcmp ("11Long String"+2, "Long String", 11))
								error(61);
	if (memcmp ("Long String", "11Long String"+2, 11))
								error(62);
	if (memcmp ("111Long String"+3, "Long String", 11))
								error(63);
	if (memcmp ("Long String", "111Long String"+3, 11))
								error(64);
	if (memcmp ("111Long String"+3, "111Long String"+3, 11))
								error(65);
	if (memcmp ("11Long String"+2, "11Long String"+2, 11))
								error(66);

	if (memcmp ("Short", "Siort", 6) >= 0)
								error(67);
	if (memcmp ("Siort", "SHort", 6) <= 0)
								error(68);
	if (memcmp ("Short", "SGort", 6) <= 0)
								error(69);
	if (memcmp ("SGort", "Short", 6) >= 0)
								error(70);

	if (memcmp ("Long String", "Long Strinh", 11) >= 0)
								error(71);
	if (memcmp ("Long Strinh", "Long String", 11) <= 0)
								error(72);
	if (memcmp ("Long String", "Long Strinf", 11) <= 0)
								error(73);
	if (memcmp ("Long Strinf", "Long String", 11) >= 0)
								error(74);

	if (memcmp ("Long String", "Mong String", 11) >= 0)
								error(75);
	if (memcmp ("Mong String", "Long String", 11) <= 0)
								error(76);
	if (memcmp ("Long String", "Kong String", 11) <= 0)
								error(77);
	if (memcmp ("Kong String", "Long String", 11) >= 0)
								error(78);

/* Run some tests on memset */
	if (memclr (test, sizeof (test)) != test)	error(79);/* zero */
	if (memset (test+1, 0xa5, sizeof (test)-2) != test+1) error (80);
	if ((test[0] != 0) ||			/* start and finish odd addr */
	    (test[sizeof(test)-1] != 0))	error(81);
	for (i=1; i < sizeof(test)-1; i++)
	    if ((unsigned char) test[i] != 0xa5)
		{
		error (82);
		break;
		}

/* Run some tests on memccpy */
	strcpy (test, "Long String");		/* Put test string into test */
	strcpy (xtok, "12345123456");

	if (memccpy (xtok, test, 'S', sizeof ("Long String"))
		!= xtok + 6)			error (83);
	if (strcmp (xtok, "Long S23456"))	error (84);

	strcpy (xtok, "12345123456");
	if (memccpy (xtok, test, 'z', sizeof ("Long String"))
		!= NULL)			error (85);
	if (strcmp (xtok, "Long String"))	error (86);

	strcpy (xtok, "12345123456");
	if (memccpy (xtok, test+1, 'S', sizeof ("Long String") - 1)
		!= xtok + 5)			error (87);
	if (strcmp (xtok, "ong S123456"))	error (88);
	
/* Run some tests on memchr */
	strcpy (test, "Long String");		/* Put test string into test */
	if (memchr (test, 'S', sizeof ("Long String")) != test + 5)
						error (89);
	if (memchr (test + 1, 'S', sizeof ("Long String")-1) != test + 5)
						error (90);
	if (memchr (test, 'z', sizeof ("Long String")) != NULL)
						error (91);

/* Run some tests on strcat */
	strcpy (test, "First String ");
	strcpy (xtok, "Second String");
	if (strcmp (strcat (test, xtok), "First String Second String"))
						error (92);

/* Test strchr */
	if (strchr (test, 'S') != test + 6)	error (93);
	if (strchr (test, 'z') != NULL)		error (94);

/* Test strlen */
	if (strlen (test) != sizeof ("First String Second String") - 1)
						error (95);

/* Test strrchr */
	if (strrchr (test, 'S') != test + 20)	error (96);
	if (strrchr (test, 'z') != NULL)	error (97);

/* test strncpy */
	strcpy (test, "First One");
	if (strncpy (test, "Test String", 5) != test)	error (98);
	if (strcmp (test, "Test  One"))		error (99);
	memset (test, 5, sizeof (test));
	if (strncpy (test, "Test String", 25) != test) error (100);
	if (strcmp (test, "Test String"))	error (101);
	for (i = sizeof ("Test String") ; i < 25; ++i)
	    {
	    if (test[i] != '\0')
		{
		error (102);
		break;
		}
	    }
	for (i = 25; i < sizeof (test); ++i)
	    {
	    if (test[i] != 5)
		{
		error (103);
		break;
		}
	    }

/* test strncmp */
	if (strncmp ("Test One", "Test Two", 25) >= 0)	error (104);
	if (strncmp ("Test Two", "Test One", 25) <= 0)	error (105);
	if (strncmp ("Test One", "Test One", 25) != 0)	error (106);
	if (strncmp ("Test One", "Test Two", 5)  != 0)	error (107);
	if (strncmp ("Test One", "Test Two", 6)  >= 0)	error (108);

/* test strncat */
	strcpy (test, "First One ");
	if (strncat (test, "Second One ", 100) != test)	error (109);
	if (strcmp (test, "First One Second One "))	error (110);
	strncat (test, "Third One", 5);
	if (strcmp (test, "First One Second One Third"))
							error (111);
	
/* test memmove */
	strcpy (test, "First One ");
	if (memmove (test + 5, test, 25) != test + 5)	error (112);
	if (strcmp (test, "FirstFirst One "))		error (113);
	strcpy (test, "First One ");
	if (memmove (test, test + 5, 25) != test)	error (114);
	if (strcmp (test, " One "))			error (115);

/* test strcoll */
        if (strcoll ("Test String", "Test Strinh") >= 0) error(116);
        if (strcoll ("Test String", "Test Strinf") <= 0) error(117);
        if (strcoll ("Test String", "Test String1") >= 0)        error(118);
        if (strcoll ("Test String1", "Test String") <= 0)        error(119);

        if (strcoll ("Test String"+1, "Test Strinh"+1) >= 0)     error(120);
        if (strcoll ("Test String"+1, "Test Strinf"+1) <= 0)     error(121);
        if (strcoll ("Test String"+1, "Test String1"+1) >= 0)    error(122);
        if (strcoll ("Test String1"+1, "Test String"+1) <= 0)    error(123);

        if (strcoll ("Test String", "1Test Strinh"+1) >= 0)     error(124);
        if (strcoll ("Test String", "1Test Strinf"+1) <= 0)     error(125);
        if (strcoll ("Test String", "1Test String1"+1) >= 0)    error(126);
        if (strcoll ("Test String1", "1Test String"+1) <= 0)    error(127);

/* One more quick test for memset */

	memclr (test, sizeof(test));	/* pre-clear */
	if (memset (test+1, 0xa5, 3) != test+1) error(128);/*set small amt mem*/
	if ((test[0] != 0) || (test[4] != 0))	error(129);
	for (i=1; i<4; i++)
	    if ((unsigned char)test[i] != 0xa5) error (130);

/* One more quick test for memcpy -- using short amt */

	memclr (test, sizeof(test));	/* pre-clear */
	if(memcpy (test+3, "Test string", 1) != test+3)	error (131);
	if ((test[2] != 0) || (test[4] != 0))	error(132);
	if (strcmp (test+3, "T"))	error (133);
	
/* Test cases for spr 24796 */

	
#if ! sun				/* sun libraries exhibit this bug */
        {
	    static char s1[] = { 0x04, 0x01, 0xff, 0x00 };
	    static char s2[] = { 0x04, 0x01, 0x0f, 0x00 };

	    if (memcmp	(s1, s2, 4) <= 0)	error (134);
	    if (strcmp	(s1, s2)    <= 0)	error (135);
	    if (strncmp	(s1, s2, 11) <= 0)	error (136);
        }
#endif

/* Test case for spr 34474 */

        {
	if (memcmp("....abcd1818abc","....abcd4444abc",12) >= 0) error (137);
	}

        if (globError)
                printf (ERRORS);
        else
                printf (NO_ERRORS);

}

error(n)
{	printf("Error %2d\n",n); globError++; }
 
