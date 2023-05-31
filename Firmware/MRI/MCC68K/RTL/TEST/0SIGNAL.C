#include "lib_top.h"


#pragma	option	-nx

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) Microtec Research, Inc. 1992.				*/
/* All rights reserved. 						*/
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

/************************************************************************/
/*	Test routine to exercise "signal" and "raise" functions.	*/
/************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include "message.h"

int	errorCount;
int	testCount;

void _sig_abort (int input)	/* substitute for SIBABRT default routine */
{
    testCount++;
}

void exit (int input)	
{
    testCount += input;
}

static void sigTest (int input)
{
    testCount++;
}

main ()
{
/************************************************************************/
/*	First test is to see if "SIG_IGN" works				*/
/************************************************************************/

    if (signal (SIGABRT, SIG_IGN) != SIG_IGN) {
	puts ("Error: signal failed to return SIG_IGN for SIGABRT.");
	errorCount++;
	}

    if (signal (SIGFPE,  SIG_IGN) != SIG_IGN) {
	puts ("Error: signal failed to return SIG_IGN for SIGFPE.");
	errorCount++;
	}

    if (signal (SIGILL,  SIG_IGN) != SIG_IGN) {
	puts ("Error: signal failed to return SIG_IGN for SIGILL.");
	errorCount++;
	}

    if (signal (SIGINT,  SIG_IGN) != SIG_IGN) {
	puts ("Error: signal failed to return SIG_IGN for SIGINT.");
	errorCount++;
	}

    if (signal (SIGSEGV, SIG_IGN) != SIG_IGN) {
	puts ("Error: signal failed to return SIG_IGN for SIGSEGV.");
	errorCount++;
	}

    if (signal (SIGTERM, SIG_IGN) != SIG_IGN) {
	puts ("Error: signal failed to return SIG_IGN for SIGTERM.");
	errorCount++;
	}

    if (signal (20, SIG_IGN) != SIG_ERR) {
	puts ("Error: signal failed to return SIG_ERR for error condition 1.");
	errorCount++;
	}

    raise (SIGABRT);	/* All test must be ignored.  They will not return */
    raise (SIGFPE);	/* if they are not ignored. */
    raise (SIGILL);
    raise (SIGINT);
    raise (SIGSEGV);
    raise (SIGTERM);

    raise (SIGABRT);	/* Do again to make sure that SIG_IGN did not go away */
    raise (SIGFPE);
    raise (SIGILL);
    raise (SIGINT);
    raise (SIGSEGV);
    raise (SIGTERM);

/************************************************************************/
/*	Now specify another routine 					*/
/************************************************************************/

    if (signal (SIGABRT, sigTest) != sigTest) {
	puts ("Error: signal failed to return sigTest for SIGABRT.");
	errorCount++;
	}

    if (signal (SIGFPE, sigTest) != sigTest) {
	puts ("Error: signal failed to return sigTest for SIGFPE.");
	errorCount++;
	}

    if (signal (SIGILL, sigTest) != sigTest) {
	puts ("Error: signal failed to return sigTest for SIGILL.");
	errorCount++;
	}

    if (signal (SIGINT, sigTest) != sigTest) {
	puts ("Error: signal failed to return sigTest for SIGINT.");
	errorCount++;
	}

    if (signal (SIGSEGV, sigTest) != sigTest) {
	puts ("Error: signal failed to return sigTest for SIGSEGV.");
	errorCount++;
	}

    if (signal (SIGTERM, sigTest) != sigTest) {
	puts ("Error: signal failed to return sigTest for SIGTERM.");
	errorCount++;
	}

    raise (SIGABRT);	/* Each test will cause testCount to be incremented */
    raise (SIGFPE);
    raise (SIGILL);
    raise (SIGINT);
    raise (SIGSEGV);
    raise (SIGTERM);

    if (testCount != 6) {
	puts ("Error:  Signal handlers failed to be executed.");
	errorCount++;
	}

/************************************************************************/
/*	Now set to default and test the result				*/
/* 	Note that the exit () routine is intercepted for this test	*/
/************************************************************************/

    if (signal (SIGABRT, SIG_DFL) != SIG_DFL) {
	puts ("Error: signal failed to return SIG_DFL for SIGABRT.");
	errorCount++;
	}

    if (signal (SIGFPE, SIG_DFL) != SIG_DFL) {
	puts ("Error: signal failed to return SIG_DFL for SIGFPE.");
	errorCount++;
	}

    if (signal (SIGILL, SIG_DFL) != SIG_DFL) {
	puts ("Error: signal failed to return SIG_DFL for SIGILL.");
	errorCount++;
	}

    if (signal (SIGINT, SIG_DFL) != SIG_DFL) {
	puts ("Error: signal failed to return SIG_DFL for SIGINT.");
	errorCount++;
	}

    if (signal (SIGSEGV, SIG_DFL) != SIG_DFL) {
	puts ("Error: signal failed to return SIG_DFL for SIGSEGV.");
	errorCount++;
	}

    if (signal (SIGTERM, SIG_DFL) != SIG_DFL) {
	puts ("Error: signal failed to return SIG_DFL for SIGTERM.");
	errorCount++;
	}

    raise (SIGABRT);	/* Execute default signal handlers */
    raise (SIGFPE);
    raise (SIGILL);
    raise (SIGINT);
    raise (SIGSEGV);
    raise (SIGTERM);

    if (testCount != 10) {
	puts ("Error:  Default signal handlers failed to be executed"
		" correctly.");
	errorCount++;
	}

    if (errorCount) {
	printf (ERRORS);
	_exit(1);
	}

    printf (NO_ERRORS);
    _exit (0);
}
 
