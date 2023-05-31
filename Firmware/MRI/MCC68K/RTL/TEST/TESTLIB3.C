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

/*    %W% %G%	*/

/*									*/
/*	This routine simply tests to be sure that all routines are	*/
/*	actually there and we can get clean links to them		*/
/*	If run, this routine will simply halt immediately.		*/
/*									*/

/*
Change history:
	11/22/93 jpc -	removed calls to single precision math routines.
*/


#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mriext.h>
#include <stdarg.h>
#include <time.h>
#include <assert.h>

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mriext.h>
#include <stdarg.h>
#include <time.h>
#include <assert.h>

#include "message.h"

/************************************************************************/
/*	The following are quick tests to insure that messenger symbols	*/
/*	can be resolved.						*/
/************************************************************************/

#if	_MCC86
extern	int	___CPU086;
extern	int	___CPU088;
extern	int	___CPU186;
extern	int	___CPU188;
extern	int	___CPU286;
extern	int	___CPUV25;
extern	int	___CPUV20;
extern	int	___CPUV30;
extern	int	___FPU;
#endif

#if 	_MCC68K
extern	int	___FPU;
#endif

_WARNING_open_stub_used () {
    return 0;
}

_WARNING_ftell_stub_used () {
    return 0;
}

_WARNING_lseek_stub_used () {
    return 0;
}

_WARNING_unlink_stub_used () {
    return 0;
}

void sig_nal (int test)
{
return;
}

int zapper (const void *arg1, const void * arg2)
{
return 0;
}

void kill (void)		/* Abort execution */
{
printf (NO_ERRORS);
exit (0);
}

struct tm timer;
time_t	clocker;
double dummy;

extern char const _MRI_lib_version[];

main()
{
  double d;
  float flt;
  char *s;
  char c;
  int ch,a,b;
  FILE *f;
  unsigned u;
  long l;
  va_list vaList;
  size_t sizeT;
  jmp_buf env;

kill ();

dummy = dummy * 100.;	/* Force linking floating pnt ftoa routine */

b = abs(a);
l = labs(l);
qsort(s, ch, ch, zapper);
rand();
srand(b);
atexit (kill);
div (a, a);
ldiv (l, l);
memmove (s, s, a);
setlocale (a, s);
strcoll (s, s);

asctime (&timer);
gmtime  (&clocker);
localtime (&clocker);
ctime (&clocker);

fgetpos (f, (fpos_t *) &l);
fsetpos (f, (fpos_t *) &l);
fseek   (f, l, a);
ftell	(f);
lseek	();
vsprintf (s, s, vaList);
vprintf	 (s, vaList);
vfprintf (f, s, vaList);
strstr (s, s);
bsearch (s, s, sizeT, sizeT, zapper);
rewind (f);
perror (s);
strerror (a);
abort ();
setjmp (env);
longjmp (env, 1);

#if !(_MCC68K || _MCCSP || VANILLA_LIB || _MCC88K || _MCC960 || _MCC29K || \
	_MCCG32 || _MCCH85)
_chkstk ();
#endif 

sbrk ();
/*	_main ();	*/	/* C++ routines */
#if !(_MCCT9K)			/* Do not test for non C++ targets */
    _cxxfini();
#endif

unlink(s);
remove(s);
clock ();
difftime (clocker, clocker);
time (&clocker);
freopen	(s, s, f);
setvbuf	(f, s, 1, 1);
mblen (s, 1);
mbtowc ((wchar_t *) s, s, 1);
wctomb (s, 2);
wcstombs (s, (wchar_t *) s, 10);
mbstowcs ((wchar_t *)s, s, 10);
strxfrm (s, s, 10);
raise (1);
signal (1, sig_nal);
localeconv ();
strftime (s, (size_t) 10, s, &timer);
_no_nl_puts (s);
_ASSERT (s, s, 1);
printf (_MRI_lib_version);
#ifndef _MCC86
_initcopy ();
#endif
HUGE_VAL;
_mri_start ();			/* C level entry point */
_simulated_halt ();		/* Label for halt; used by debugger */
}
