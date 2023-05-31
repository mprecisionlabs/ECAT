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

assert (1);		/* Make sure assert macro is there */

isalnum(ch); 
isalpha(ch); 
iscntrl(ch);
isdigit(ch); 
isgraph(ch); 
islower(ch); 
isprint(ch); 
ispunct(ch); 
isspace(ch); 
isupper(ch); 
isxdigit(ch); 
toupper(ch);
tolower(ch);
_toupper(ch);
_tolower(ch);

#undef	isalnum			/* Make sure that functions exist for these */
#undef	isalpha 
#undef	iscntrl
#undef	isdigit 
#undef	isgraph 
#undef	islower 
#undef	isprint 
#undef	ispunct 
#undef	isspace 
#undef	isupper 
#undef	isxdigit 
#undef	toupper
#undef	tolower
#undef	_toupper
#undef	_tolower

isalnum(ch); 
isalpha(ch); 
iscntrl(ch);
isdigit(ch); 
isgraph(ch); 
islower(ch); 
isprint(ch); 
ispunct(ch); 
isspace(ch); 
isupper(ch); 
isxdigit(ch); 
toupper(ch);
tolower(ch);
_tolower(ch);
_toupper(ch);

isascii(ch);		/* Microtec extensions */
toascii(ch);
b = max(a,b);
a = min(a,b);



getc(f);	                
getchar();
putc(ch,f);
putchar(ch);
fileno(f);	
feof(f);	
ferror(f);	
clearerr(f);	
close();
creat();
exit(ch);
fclose(f);
fflush(f);
fgetc(f);
fgets(s, ch, f);
fopen(s, s);
fputc(ch, f);
fputs(s, f);
fread(s, ch, ch, f);
fwrite(s, ch, ch, f);
getl(f);
gets(s);
getw(f);
open();
putl(l, f);
puts(s);
putw(a, f);
read();
ungetc(ch, f);
write();
setbuf(f, s);


#undef	getc
#undef	putc
#undef	getchar
#undef	putchar
#undef	clearerr
#undef	feof
#undef	ferror
#undef	eprintf

getc(f);	                
putc(ch,f);
getchar();
putchar(ch);
clearerr(f);	
feof(f);	
ferror(f);	
eprintf(s, ch);

memccpy(s, s, ch, ch);
memchr(s, ch, ch);
memclr(s, ch);
memcmp(s, s, ch);
memcpy(s, s, ch);
memset(s, ch, ch);
strcat(s, s);
strchr(s, ch);
strcmp(s, s);
strcpy(s, s);
strcspn(s, s);
strlen(s);
strncat(s, s, ch);
strncmp(s, s, ch);
strncpy(s, s, ch);
strpbrk(s, s);
strrchr(s, ch);
strspn(s, s);
strtok(s, s);
strtol(s, &s, ch);
strtoul(s, &s, ch);
swab(s, s, a);
 
calloc(ch, ch);
free(s);
malloc(ch);
realloc(s, ch);
zalloc(ch);
}
