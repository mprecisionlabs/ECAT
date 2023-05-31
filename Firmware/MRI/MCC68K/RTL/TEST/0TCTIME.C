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


/*	%W% %G%	*/

/*
	This routine was written by feeding data to the sun "ctime"
	routine and using the results to write the "data" array
	below.  The test points are 100 points distributed fairly
	evenly over the range of arguments which the sun accepts.
	(sun treats the input as a signed long.)

	written by jack christensen 2/1/90
*/


#include <stdio.h>
#include <time.h>

#include "message.h"

#if sun
#define const
#endif

const struct {
	time_t seconds;
	char result[30];
	} data [] = {
	{0x00000000, "Thu Jan  1 00:00:00 1970\n"},
	{0x0147ae14, "Sun Sep  6 13:13:56 1970\n"},
	{0x028f5c28, "Thu May 13 02:27:52 1971\n"},
	{0x03d70a3c, "Sun Jan 16 15:41:48 1972\n"},
	{0x051eb850, "Thu Sep 21 04:55:44 1972\n"},
	{0x06666664, "Sun May 27 18:09:40 1973\n"},
	{0x07ae1478, "Thu Jan 31 07:23:36 1974\n"},
	{0x08f5c28c, "Sun Oct  6 20:37:32 1974\n"},
	{0x0a3d70a0, "Thu Jun 12 09:51:28 1975\n"},
	{0x0b851eb4, "Sun Feb 15 23:05:24 1976\n"},
	{0x0cccccc8, "Thu Oct 21 12:19:20 1976\n"},
	{0x0e147adc, "Mon Jun 27 01:33:16 1977\n"},
	{0x0f5c28f0, "Thu Mar  2 14:47:12 1978\n"},
	{0x10a3d704, "Mon Nov  6 04:01:08 1978\n"},
	{0x11eb8518, "Thu Jul 12 17:15:04 1979\n"},
	{0x1333332c, "Mon Mar 17 06:29:00 1980\n"},
	{0x147ae140, "Thu Nov 20 19:42:56 1980\n"},
	{0x15c28f54, "Mon Jul 27 08:56:52 1981\n"},
	{0x170a3d68, "Thu Apr  1 22:10:48 1982\n"},
	{0x1851eb7c, "Mon Dec  6 11:24:44 1982\n"},
	{0x19999990, "Fri Aug 12 00:38:40 1983\n"},
	{0x1ae147a4, "Mon Apr 16 13:52:36 1984\n"},
	{0x1c28f5b8, "Fri Dec 21 03:06:32 1984\n"},
	{0x1d70a3cc, "Mon Aug 26 16:20:28 1985\n"},
	{0x1eb851e0, "Fri May  2 05:34:24 1986\n"},
	{0x1ffffff4, "Mon Jan  5 18:48:20 1987\n"},
	{0x2147ae08, "Fri Sep 11 08:02:16 1987\n"},
	{0x228f5c1c, "Mon May 16 21:16:12 1988\n"},
	{0x23d70a30, "Fri Jan 20 10:30:08 1989\n"},
	{0x251eb844, "Mon Sep 25 23:44:04 1989\n"},
	{0x26666658, "Fri Jun  1 12:58:00 1990\n"},
	{0x27ae146c, "Tue Feb  5 02:11:56 1991\n"},
	{0x28f5c280, "Fri Oct 11 15:25:52 1991\n"},
	{0x2a3d7094, "Tue Jun 16 04:39:48 1992\n"},
	{0x2b851ea8, "Fri Feb 19 17:53:44 1993\n"},
	{0x2cccccbc, "Tue Oct 26 07:07:40 1993\n"},
	{0x2e147ad0, "Fri Jul  1 20:21:36 1994\n"},
	{0x2f5c28e4, "Tue Mar  7 09:35:32 1995\n"},
	{0x30a3d6f8, "Fri Nov 10 22:49:28 1995\n"},
	{0x31eb850c, "Tue Jul 16 12:03:24 1996\n"},
	{0x33333320, "Sat Mar 22 01:17:20 1997\n"},
	{0x347ae134, "Tue Nov 25 14:31:16 1997\n"},
	{0x35c28f48, "Sat Aug  1 03:45:12 1998\n"},
	{0x370a3d5c, "Tue Apr  6 16:59:08 1999\n"},
	{0x3851eb70, "Sat Dec 11 06:13:04 1999\n"},
	{0x39999984, "Tue Aug 15 19:27:00 2000\n"},
	{0x3ae14798, "Sat Apr 21 08:40:56 2001\n"},
	{0x3c28f5ac, "Tue Dec 25 21:54:52 2001\n"},
	{0x3d70a3c0, "Sat Aug 31 11:08:48 2002\n"},
	{0x3eb851d4, "Wed May  7 00:22:44 2003\n"},
	{0x3fffffe8, "Sat Jan 10 13:36:40 2004\n"},
	{0x4147adfc, "Wed Sep 15 02:50:36 2004\n"},
	{0x428f5c10, "Sat May 21 16:04:32 2005\n"},
	{0x43d70a24, "Wed Jan 25 05:18:28 2006\n"},
	{0x451eb838, "Sat Sep 30 18:32:24 2006\n"},
	{0x4666664c, "Wed Jun  6 07:46:20 2007\n"},
	{0x47ae1460, "Sat Feb  9 21:00:16 2008\n"},
	{0x48f5c274, "Wed Oct 15 10:14:12 2008\n"},
	{0x4a3d7088, "Sat Jun 20 23:28:08 2009\n"},
	{0x4b851e9c, "Wed Feb 24 12:42:04 2010\n"},
	{0x4cccccb0, "Sun Oct 31 01:56:00 2010\n"},
	{0x4e147ac4, "Wed Jul  6 15:09:56 2011\n"},
	{0x4f5c28d8, "Sun Mar 11 04:23:52 2012\n"},
	{0x50a3d6ec, "Wed Nov 14 17:37:48 2012\n"},
	{0x51eb8500, "Sun Jul 21 06:51:44 2013\n"},
	{0x53333314, "Wed Mar 26 20:05:40 2014\n"},
	{0x547ae128, "Sun Nov 30 09:19:36 2014\n"},
	{0x55c28f3c, "Wed Aug  5 22:33:32 2015\n"},
	{0x570a3d50, "Sun Apr 10 11:47:28 2016\n"},
	{0x5851eb64, "Thu Dec 15 01:01:24 2016\n"},
	{0x59999978, "Sun Aug 20 14:15:20 2017\n"},
	{0x5ae1478c, "Thu Apr 26 03:29:16 2018\n"},
	{0x5c28f5a0, "Sun Dec 30 16:43:12 2018\n"},
	{0x5d70a3b4, "Thu Sep  5 05:57:08 2019\n"},
	{0x5eb851c8, "Sun May 10 19:11:04 2020\n"},
	{0x5fffffdc, "Thu Jan 14 08:25:00 2021\n"},
	{0x6147adf0, "Sun Sep 19 21:38:56 2021\n"},
	{0x628f5c04, "Thu May 26 10:52:52 2022\n"},
	{0x63d70a18, "Mon Jan 30 00:06:48 2023\n"},
	{0x651eb82c, "Thu Oct  5 13:20:44 2023\n"},
	{0x66666640, "Mon Jun 10 02:34:40 2024\n"},
	{0x67ae1454, "Thu Feb 13 15:48:36 2025\n"},
	{0x68f5c268, "Mon Oct 20 05:02:32 2025\n"},
	{0x6a3d707c, "Thu Jun 25 18:16:28 2026\n"},
	{0x6b851e90, "Mon Mar  1 07:30:24 2027\n"},
	{0x6ccccca4, "Thu Nov  4 20:44:20 2027\n"},
	{0x6e147ab8, "Mon Jul 10 09:58:16 2028\n"},
	{0x6f5c28cc, "Thu Mar 15 23:12:12 2029\n"},
	{0x70a3d6e0, "Mon Nov 19 12:26:08 2029\n"},
	{0x71eb84f4, "Fri Jul 26 01:40:04 2030\n"},
	{0x73333308, "Mon Mar 31 14:54:00 2031\n"},
	{0x747ae11c, "Fri Dec  5 04:07:56 2031\n"},
	{0x75c28f30, "Mon Aug  9 17:21:52 2032\n"},
	{0x770a3d44, "Fri Apr 15 06:35:48 2033\n"},
	{0x7851eb58, "Mon Dec 19 19:49:44 2033\n"},
	{0x7999996c, "Fri Aug 25 09:03:40 2034\n"},
	{0x7ae14780, "Mon Apr 30 22:17:36 2035\n"},
	{0x7c28f594, "Fri Jan  4 11:31:32 2036\n"},
	{0x7d70a3a8, "Tue Sep  9 00:45:28 2036\n"},
	{0x7eb851bc, "Fri May 15 13:59:24 2037\n"}};


main ()
{
int i, errorCount;

errorCount = 0;

for (i=0; i<(sizeof(data)/sizeof(data[0])); i++)
	{
	if (strcmp (data[i].result, ctime (&data[i].seconds)))
		{
		printf ("error: utec = %08.8x %s SUN = %s\n",
			data[i].seconds, ctime (&data[i].seconds),
			data[i].result);
		errorCount = 1;
		}
	}
if (errorCount)
	printf (ERRORS);
else
	printf (NO_ERRORS);
}
 
