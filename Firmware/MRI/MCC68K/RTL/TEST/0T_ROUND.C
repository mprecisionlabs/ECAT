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


/************************************************************************/
/*	This routine tests for the rounding problem found by Acuson	*/
/*	Rather than printing the result out, it is simply written to	*/
/*	a buffer and that is compared to what was supposed to be	*/
/*	written.							*/
/************************************************************************/

/*	%W% %G%	*/

/*
Change history:
	11/20/93 jpc -	Mods to allow -Kq to function with this test.
*/

#include <stdio.h>

#include "message.h"

const struct {
	double values;
	char results[20];
	char format[20];
	} data [] = {
#if __OPTION_VALUE ("-Kq")
	{.653889, "0.654" ,		"%5.3f" },
	{-.52299, "-0.523",		"%5.3f" },
#else
	{.5697653889, "0.5697654" ,	"%9.7f" },
	{-.945252299, "-0.9452523",	"%9.7f" },
#endif
	{.228,        "0.23"	  ,	"%3.2f" },
	{.0228,	      "0.023"	  ,	"%3.3f" },
	{.0224,	      "0.022"	  ,	"%3.3f" },
	{55.55555,    "55.556"	  ,	"%3.3f" },
	{.055555,     "0.056"  	  ,	"%3.3f" },
	{123456.6,    "123457"	  ,	"%.0f"  },
	{.123455,     "1.234550e-01",	"%.6e"  },
	{.123454,     "1.2345e-01",	"%.4e"  },
	{33.137,      "33.14"	  ,	"%.4g"  },
	{133.137,     "133.14"	  ,	"%.5g"  },
	{33.134,      "33.13"	  ,	"%.4g"  },
	{133.134,     "133.13"	  ,	"%.5g"  },
	{17.,	      "2e+01"	  ,	"%.1g"  },
	{13.,	      "1e+01"	  ,	"%.1g"  },
	{1234567.,    "1.2346e+06",	"%.5g"  },
	{1234547.,    "1.2345e+06",	"%.5g"  },
	{55.55555,    "55.6"	  ,	"%.3g"  },
	{0.055555,    "0.0556"	  ,	"%.3g"  },
	{25.,	      "25"	  ,	"%.2g"  },
	{1.2347,      "1.235"	  ,	"%.4g"  },
	{1.2344,      "1.234"	  ,	"%.4g"  },
	{.12347,      "0.1235"    ,	"%.4g"  },
	{.12344,      "0.1234"	  ,	"%.4g"  },
	{.99999,      "1.0000"	  ,	"%.4f"	},
        {.99999,      "1"         ,     "%.4g"  },
        {.99999999,   "1.0000e+00",     "%.4e"  }
	};


main ()
{
char buffer [50];
int i;
int error = 0;

for (i=0; i<(sizeof (data) / sizeof (data[0])); i++)
	{
	sprintf (buffer, data[i].format, data[i].values);
	if (strcmp (buffer, data[i].results))
		{
		printf ("Error: expected = %s actual = %s test #%d\n",
			data[i].results, buffer, i);
		error = 1;
		}
	}

if (error)
	printf (ERRORS);
else
	printf (NO_ERRORS);
}
 
