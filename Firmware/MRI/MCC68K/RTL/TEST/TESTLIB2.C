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

extern char _MRI_lib_version[];

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

ftoa(d,s,c,a);

eprintf(s, ch);
fprintf(f, s);
printf(s);
fscanf(f,s);
scanf(s);
sprintf(s, s);
sscanf(s, s);

strtod ((const char *)s, (char **)s);
atof(s);
atoi(s);
atol(s);
itoa(a, s);
itostr(u, s, a);
ltoa(ch, s);
ltostr(ch, s, ch);


acos(d);
asin(d);
atan(d);
atan2(d, d);
ceil(d);
cos(d);
cosh(d);
exp(d);
fabs(d);
floor(d);
fmod(d, d);
frexp(d, &ch);
ldexp(d, ch);
log(d);
log10(d);
modf(d, &d);
pow(d, d);
sin(d);
sinh(d);
sqrt(d);
tan(d);
tanh(d);
}
