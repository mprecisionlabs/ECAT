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
/*	The following tests the strftime function.			*/
/************************************************************************/

/*	%W% %G%	*/

#include <stdio.h>
#include <time.h>
#include "message.h"

const struct {
	time_t input;
	char output[125];
	} data[] = {
{0x0,
"Thu Thursday Jan January 01/01/70 00:00:00 01 00 12 001 01 00 AM 00 00 4 00 01/01/70 00:00:00 70 1970  %"},
{0xffffff,
"Tue Tuesday Jul July 07/14/70 04:20:15 14 04 04 195 07 20 AM 15 28 2 28 07/14/70 04:20:15 70 1970  %"},
{0x1fffffe,
"Sun Sunday Jan January 01/24/71 08:40:30 24 08 08 024 01 40 AM 30 04 0 03 01/24/71 08:40:30 71 1971  %"},
{0x2fffffd,
"Fri Friday Aug August 08/06/71 13:00:45 06 13 01 218 08 00 PM 45 31 5 31 08/06/71 13:00:45 71 1971  %"},
{0x3fffffc,
"Wed Wednesday Feb February 02/16/72 17:21:00 16 17 05 047 02 21 PM 00 07 3 07 02/16/72 17:21:00 72 1972  %"},
{0x4fffffb,
"Mon Monday Aug August 08/28/72 21:41:15 28 21 09 241 08 41 PM 15 35 1 35 08/28/72 21:41:15 72 1972  %"},
{0x5fffffa,
"Sun Sunday Mar March 03/11/73 02:01:30 11 02 02 070 03 01 AM 30 10 0 10 03/11/73 02:01:30 73 1973  %"},
{0x6fffff9,
"Fri Friday Sep September 09/21/73 06:21:45 21 06 06 264 09 21 AM 45 37 5 38 09/21/73 06:21:45 73 1973  %"},
{0x7fffff8,
"Wed Wednesday Apr April 04/03/74 10:42:00 03 10 10 093 04 42 AM 00 13 3 13 04/03/74 10:42:00 74 1974  %"},
{0x8fffff7,
"Mon Monday Oct October 10/14/74 15:02:15 14 15 03 287 10 02 PM 15 41 1 41 10/14/74 15:02:15 74 1974  %"},
{0x9fffff6,
"Sat Saturday Apr April 04/26/75 19:22:30 26 19 07 116 04 22 PM 30 16 6 16 04/26/75 19:22:30 75 1975  %"},
{0xafffff5,
"Thu Thursday Nov November 11/06/75 23:42:45 06 23 11 310 11 42 PM 45 44 4 44 11/06/75 23:42:45 75 1975  %"},
{0xbfffff4,
"Wed Wednesday May May 05/19/76 04:03:00 19 04 04 140 05 03 AM 00 20 3 20 05/19/76 04:03:00 76 1976  %"},
{0xcfffff3,
"Mon Monday Nov November 11/29/76 08:23:15 29 08 08 334 11 23 AM 15 48 1 48 11/29/76 08:23:15 76 1976  %"},
{0xdfffff2,
"Sat Saturday Jun June 06/11/77 12:43:30 11 12 12 162 06 43 PM 30 23 6 23 06/11/77 12:43:30 77 1977  %"},
{0xefffff1,
"Thu Thursday Dec December 12/22/77 17:03:45 22 17 05 356 12 03 PM 45 51 4 51 12/22/77 17:03:45 77 1977  %"},
{0xffffff0,
"Tue Tuesday Jul July 07/04/78 21:24:00 04 21 09 185 07 24 PM 00 27 2 27 07/04/78 21:24:00 78 1978  %"},
{0x10ffffef,
"Mon Monday Jan January 01/15/79 01:44:15 15 01 01 015 01 44 AM 15 02 1 03 01/15/79 01:44:15 79 1979  %"},
{0x11ffffee,
"Sat Saturday Jul July 07/28/79 06:04:30 28 06 06 209 07 04 AM 30 29 6 30 07/28/79 06:04:30 79 1979  %"},
{0x12ffffed,
"Thu Thursday Feb February 02/07/80 10:24:45 07 10 10 038 02 24 AM 45 05 4 05 02/07/80 10:24:45 80 1980  %"},
{0x13ffffec,
"Tue Tuesday Aug August 08/19/80 14:45:00 19 14 02 232 08 45 PM 00 33 2 33 08/19/80 14:45:00 80 1980  %"},
{0x14ffffeb,
"Sun Sunday Mar March 03/01/81 19:05:15 01 19 07 060 03 05 PM 15 09 0 08 03/01/81 19:05:15 81 1981  %"},
{0x15ffffea,
"Fri Friday Sep September 09/11/81 23:25:30 11 23 11 254 09 25 PM 30 36 5 36 09/11/81 23:25:30 81 1981  %"},
{0x16ffffe9,
"Thu Thursday Mar March 03/25/82 03:45:45 25 03 03 084 03 45 AM 45 12 4 12 03/25/82 03:45:45 82 1982  %"},
{0x17ffffe8,
"Tue Tuesday Oct October 10/05/82 08:06:00 05 08 08 278 10 06 AM 00 40 2 40 10/05/82 08:06:00 82 1982  %"},
{0x18ffffe7,
"Sun Sunday Apr April 04/17/83 12:26:15 17 12 12 107 04 26 PM 15 16 0 15 04/17/83 12:26:15 83 1983  %"},
{0x19ffffe6,
"Fri Friday Oct October 10/28/83 16:46:30 28 16 04 301 10 46 PM 30 43 5 43 10/28/83 16:46:30 83 1983  %"},
{0x1affffe5,
"Wed Wednesday May May 05/09/84 21:06:45 09 21 09 130 05 06 PM 45 19 3 19 05/09/84 21:06:45 84 1984  %"},
{0x1bffffe4,
"Tue Tuesday Nov November 11/20/84 01:27:00 20 01 01 325 11 27 AM 00 47 2 47 11/20/84 01:27:00 84 1984  %"},
{0x1cffffe3,
"Sun Sunday Jun June 06/02/85 05:47:15 02 05 05 153 06 47 AM 15 22 0 21 06/02/85 05:47:15 85 1985  %"},
{0x1dffffe2,
"Fri Friday Dec December 12/13/85 10:07:30 13 10 10 347 12 07 AM 30 49 5 49 12/13/85 10:07:30 85 1985  %"},
{0x1effffe1,
"Wed Wednesday Jun June 06/25/86 14:27:45 25 14 02 176 06 27 PM 45 25 3 25 06/25/86 14:27:45 86 1986  %"},
{0x1fffffe0,
"Mon Monday Jan January 01/05/87 18:48:00 05 18 06 005 01 48 PM 00 01 1 01 01/05/87 18:48:00 87 1987  %"},
{0x20ffffdf,
"Sat Saturday Jul July 07/18/87 23:08:15 18 23 11 199 07 08 PM 15 28 6 28 07/18/87 23:08:15 87 1987  %"},
{0x21ffffde,
"Fri Friday Jan January 01/29/88 03:28:30 29 03 03 029 01 28 AM 30 04 5 04 01/29/88 03:28:30 88 1988  %"},
{0x22ffffdd,
"Wed Wednesday Aug August 08/10/88 07:48:45 10 07 07 223 08 48 AM 45 32 3 32 08/10/88 07:48:45 88 1988  %"},
{0x23ffffdc,
"Mon Monday Feb February 02/20/89 12:09:00 20 12 12 051 02 09 PM 00 08 1 08 02/20/89 12:09:00 89 1989  %"},
{0x24ffffdb,
"Sat Saturday Sep September 09/02/89 16:29:15 02 16 04 245 09 29 PM 15 35 6 35 09/02/89 16:29:15 89 1989  %"},
{0x25ffffda,
"Thu Thursday Mar March 03/15/90 20:49:30 15 20 08 074 03 49 PM 30 10 4 11 03/15/90 20:49:30 90 1990  %"},
{0x26ffffd9,
"Wed Wednesday Sep September 09/26/90 01:09:45 26 01 01 269 09 09 AM 45 38 3 39 09/26/90 01:09:45 90 1990  %"},
{0x27ffffd8,
"Mon Monday Apr April 04/08/91 05:30:00 08 05 05 098 04 30 AM 00 14 1 14 04/08/91 05:30:00 91 1991  %"},
{0x28ffffd7,
"Sat Saturday Oct October 10/19/91 09:50:15 19 09 09 292 10 50 AM 15 41 6 41 10/19/91 09:50:15 91 1991  %"},
{0x29ffffd6,
"Thu Thursday Apr April 04/30/92 14:10:30 30 14 02 121 04 10 PM 30 17 4 17 04/30/92 14:10:30 92 1992  %"},
{0x2affffd5,
"Tue Tuesday Nov November 11/10/92 18:30:45 10 18 06 315 11 30 PM 45 45 2 45 11/10/92 18:30:45 92 1992  %"},
{0x2bffffd4,
"Sun Sunday May May 05/23/93 22:51:00 23 22 10 143 05 51 PM 00 21 0 20 05/23/93 22:51:00 93 1993  %"},
{0x2cffffd3,
"Sat Saturday Dec December 12/04/93 03:11:15 04 03 03 338 12 11 AM 15 48 6 48 12/04/93 03:11:15 93 1993  %"},
{0x2dffffd2,
"Thu Thursday Jun June 06/16/94 07:31:30 16 07 07 167 06 31 AM 30 24 4 24 06/16/94 07:31:30 94 1994  %"},
{0x2effffd1,
"Tue Tuesday Dec December 12/27/94 11:51:45 27 11 11 361 12 51 AM 45 52 2 52 12/27/94 11:51:45 94 1994  %"},
{0x2fffffd0,
"Sun Sunday Jul July 07/09/95 16:12:00 09 16 04 190 07 12 PM 00 28 0 27 07/09/95 16:12:00 95 1995  %"},
{0x30ffffcf,
"Fri Friday Jan January 01/19/96 20:32:15 19 20 08 019 01 32 PM 15 02 5 03 01/19/96 20:32:15 96 1996  %"},
{0x31ffffce,
"Thu Thursday Aug August 08/01/96 00:52:30 01 00 12 214 08 52 AM 30 30 4 31 08/01/96 00:52:30 96 1996  %"},
{0x32ffffcd,
"Tue Tuesday Feb February 02/11/97 05:12:45 11 05 05 042 02 12 AM 45 06 2 06 02/11/97 05:12:45 97 1997  %"},
{0x33ffffcc,
"Sun Sunday Aug August 08/24/97 09:33:00 24 09 09 236 08 33 AM 00 34 0 33 08/24/97 09:33:00 97 1997  %"},
{0x34ffffcb,
"Fri Friday Mar March 03/06/98 13:53:15 06 13 01 065 03 53 PM 15 09 5 09 03/06/98 13:53:15 98 1998  %"},
{0x35ffffca,
"Wed Wednesday Sep September 09/16/98 18:13:30 16 18 06 259 09 13 PM 30 37 3 37 09/16/98 18:13:30 98 1998  %"},
{0x36ffffc9,
"Mon Monday Mar March 03/29/99 22:33:45 29 22 10 088 03 33 PM 45 13 1 13 03/29/99 22:33:45 99 1999  %"},
{0x37ffffc8,
"Sun Sunday Oct October 10/10/99 02:54:00 10 02 02 283 10 54 AM 00 41 0 40 10/10/99 02:54:00 99 1999  %"},
{0x38ffffc7,
"Fri Friday Apr April 04/21/00 07:14:15 21 07 07 112 04 14 AM 15 16 5 16 04/21/00 07:14:15 00 2000  %"},
{0x39ffffc6,
"Wed Wednesday Nov November 11/01/00 11:34:30 01 11 11 306 11 34 AM 30 44 3 44 11/01/00 11:34:30 00 2000  %"},
{0x3affffc5,
"Mon Monday May May 05/14/01 15:54:45 14 15 03 134 05 54 PM 45 20 1 19 05/14/01 15:54:45 01 2001  %"},
{0x3bffffc4,
"Sat Saturday Nov November 11/24/01 20:15:00 24 20 08 328 11 15 PM 00 47 6 47 11/24/01 20:15:00 01 2001  %"},
{0x3cffffc3,
"Fri Friday Jun June 06/07/02 00:35:15 07 00 12 158 06 35 AM 15 22 5 23 06/07/02 00:35:15 02 2002  %"},
{0x3dffffc2,
"Wed Wednesday Dec December 12/18/02 04:55:30 18 04 04 352 12 55 AM 30 50 3 51 12/18/02 04:55:30 02 2002  %"},
{0x3effffc1,
"Mon Monday Jun June 06/30/03 09:15:45 30 09 09 181 06 15 AM 45 26 1 25 06/30/03 09:15:45 03 2003  %"},
{0x3fffffc0,
"Sat Saturday Jan January 01/10/04 13:36:00 10 13 01 010 01 36 PM 00 01 6 01 01/10/04 13:36:00 04 2004  %"},
{0x40ffffbf,
"Thu Thursday Jul July 07/22/04 17:56:15 22 17 05 204 07 56 PM 15 29 4 29 07/22/04 17:56:15 04 2004  %"},
{0x41ffffbe,
"Tue Tuesday Feb February 02/01/05 22:16:30 01 22 10 032 02 16 PM 30 05 2 05 02/01/05 22:16:30 05 2005  %"},
{0x42ffffbd,
"Mon Monday Aug August 08/15/05 02:36:45 15 02 02 227 08 36 AM 45 33 1 32 08/15/05 02:36:45 05 2005  %"},
{0x43ffffbc,
"Sat Saturday Feb February 02/25/06 06:57:00 25 06 06 056 02 57 AM 00 08 6 08 02/25/06 06:57:00 06 2006  %"},
{0x44ffffbb,
"Thu Thursday Sep September 09/07/06 11:17:15 07 11 11 250 09 17 AM 15 36 4 36 09/07/06 11:17:15 06 2006  %"},
{0x45ffffba,
"Tue Tuesday Mar March 03/20/07 15:37:30 20 15 03 079 03 37 PM 30 12 2 12 03/20/07 15:37:30 07 2007  %"},
{0x46ffffb9,
"Sun Sunday Sep September 09/30/07 19:57:45 30 19 07 273 09 57 PM 45 39 0 39 09/30/07 19:57:45 07 2007  %"},
{0x47ffffb8,
"Sat Saturday Apr April 04/12/08 00:18:00 12 00 12 103 04 18 AM 00 14 6 15 04/12/08 00:18:00 08 2008  %"},
{0x48ffffb7,
"Thu Thursday Oct October 10/23/08 04:38:15 23 04 04 297 10 38 AM 15 42 4 43 10/23/08 04:38:15 08 2008  %"},
{0x49ffffb6,
"Tue Tuesday May May 05/05/09 08:58:30 05 08 08 125 05 58 AM 30 18 2 18 05/05/09 08:58:30 09 2009  %"},
{0x4affffb5,
"Sun Sunday Nov November 11/15/09 13:18:45 15 13 01 319 11 18 PM 45 45 0 45 11/15/09 13:18:45 09 2009  %"},
{0x4bffffb4,
"Fri Friday May May 05/28/10 17:39:00 28 17 05 148 05 39 PM 00 21 5 21 05/28/10 17:39:00 10 2010  %"},
{0x4cffffb3,
"Wed Wednesday Dec December 12/08/10 21:59:15 08 21 09 342 12 59 PM 15 49 3 49 12/08/10 21:59:15 10 2010  %"},
{0x4dffffb2,
"Tue Tuesday Jun June 06/21/11 02:19:30 21 02 02 172 06 19 AM 30 25 2 25 06/21/11 02:19:30 11 2011  %"},
{0x4effffb1,
"Sun Sunday Jan January 01/01/12 06:39:45 01 06 06 001 01 39 AM 45 00 0 00 01/01/12 06:39:45 12 2012  %"},
{0x4fffffb0,
"Fri Friday Jul July 07/13/12 11:00:00 13 11 11 195 07 00 AM 00 28 5 28 07/13/12 11:00:00 12 2012  %"},
{0x50ffffaf,
"Wed Wednesday Jan January 01/23/13 15:20:15 23 15 03 023 01 20 PM 15 03 3 04 01/23/13 15:20:15 13 2013  %"},
{0x51ffffae,
"Mon Monday Aug August 08/05/13 19:40:30 05 19 07 217 08 40 PM 30 31 1 31 08/05/13 19:40:30 13 2013  %"},
{0x52ffffad,
"Sun Sunday Feb February 02/16/14 00:00:45 16 00 12 047 02 00 AM 45 06 0 06 02/16/14 00:00:45 14 2014  %"},
{0x53ffffac,
"Fri Friday Aug August 08/29/14 04:21:00 29 04 04 241 08 21 AM 00 34 5 34 08/29/14 04:21:00 14 2014  %"},
{0x54ffffab,
"Wed Wednesday Mar March 03/11/15 08:41:15 11 08 08 070 03 41 AM 15 10 3 10 03/11/15 08:41:15 15 2015  %"},
{0x55ffffaa,
"Mon Monday Sep September 09/21/15 13:01:30 21 13 01 264 09 01 PM 30 38 1 37 09/21/15 13:01:30 15 2015  %"},
{0x56ffffa9,
"Sat Saturday Apr April 04/02/16 17:21:45 02 17 05 093 04 21 PM 45 13 6 13 04/02/16 17:21:45 16 2016  %"},
{0x57ffffa8,
"Thu Thursday Oct October 10/13/16 21:42:00 13 21 09 287 10 42 PM 00 41 4 41 10/13/16 21:42:00 16 2016  %"},
{0x58ffffa7,
"Wed Wednesday Apr April 04/26/17 02:02:15 26 02 02 116 04 02 AM 15 17 3 17 04/26/17 02:02:15 17 2017  %"},
{0x59ffffa6,
"Mon Monday Nov November 11/06/17 06:22:30 06 06 06 310 11 22 AM 30 45 1 44 11/06/17 06:22:30 17 2017  %"},
{0x5affffa5,
"Sat Saturday May May 05/19/18 10:42:45 19 10 10 139 05 42 AM 45 20 6 20 05/19/18 10:42:45 18 2018  %"},
{0x5bffffa4,
"Thu Thursday Nov November 11/29/18 15:03:00 29 15 03 333 11 03 PM 00 48 4 48 11/29/18 15:03:00 18 2018  %"},
{0x5cffffa3,
"Tue Tuesday Jun June 06/11/19 19:23:15 11 19 07 162 06 23 PM 15 23 2 24 06/11/19 19:23:15 19 2019  %"},
{0x5dffffa2,
"Sun Sunday Dec December 12/22/19 23:43:30 22 23 11 356 12 43 PM 30 50 0 51 12/22/19 23:43:30 19 2019  %"},
{0x5effffa1,
"Sat Saturday Jul July 07/04/20 04:03:45 04 04 04 186 07 03 AM 45 26 6 26 07/04/20 04:03:45 20 2020  %"},
{0x5fffffa0,
"Thu Thursday Jan January 01/14/21 08:24:00 14 08 08 014 01 24 AM 00 02 4 02 01/14/21 08:24:00 21 2021  %"},
{0x60ffff9f,
"Tue Tuesday Jul July 07/27/21 12:44:15 27 12 12 208 07 44 PM 15 30 2 30 07/27/21 12:44:15 21 2021  %"},
{0x61ffff9e,
"Sun Sunday Feb February 02/06/22 17:04:30 06 17 05 037 02 04 PM 30 05 0 05 02/06/22 17:04:30 22 2022  %"},
{0x62ffff9d,
"Fri Friday Aug August 08/19/22 21:24:45 19 21 09 231 08 24 PM 45 33 5 33 08/19/22 21:24:45 22 2022  %"},
{0x63ffff9c,
"Thu Thursday Mar March 03/02/23 01:45:00 02 01 01 061 03 45 AM 00 09 4 09 03/02/23 01:45:00 23 2023  %"},
{0x64ffff9b,
"Tue Tuesday Sep September 09/12/23 06:05:15 12 06 06 255 09 05 AM 15 37 2 37 09/12/23 06:05:15 23 2023  %"},
{0x65ffff9a,
"Sun Sunday Mar March 03/24/24 10:25:30 24 10 10 084 03 25 AM 30 12 0 12 03/24/24 10:25:30 24 2024  %"},
{0x66ffff99,
"Fri Friday Oct October 10/04/24 14:45:45 04 14 02 278 10 45 PM 45 40 5 40 10/04/24 14:45:45 24 2024  %"},
{0x67ffff98,
"Wed Wednesday Apr April 04/16/25 19:06:00 16 19 07 106 04 06 PM 00 15 3 15 04/16/25 19:06:00 25 2025  %"},
{0x68ffff97,
"Mon Monday Oct October 10/27/25 23:26:15 27 23 11 300 10 26 PM 15 43 1 42 10/27/25 23:26:15 25 2025  %"},
{0x69ffff96,
"Sun Sunday May May 05/10/26 03:46:30 10 03 03 130 05 46 AM 30 18 0 18 05/10/26 03:46:30 26 2026  %"},
{0x6affff95,
"Fri Friday Nov November 11/20/26 08:06:45 20 08 08 324 11 06 AM 45 46 5 46 11/20/26 08:06:45 26 2026  %"},
{0x6bffff94,
"Wed Wednesday Jun June 06/02/27 12:27:00 02 12 12 153 06 27 PM 00 22 3 22 06/02/27 12:27:00 27 2027  %"},
{0x6cffff93,
"Mon Monday Dec December 12/13/27 16:47:15 13 16 04 347 12 47 PM 15 50 1 49 12/13/27 16:47:15 27 2027  %"},
{0x6dffff92,
"Sat Saturday Jun June 06/24/28 21:07:30 24 21 09 176 06 07 PM 30 25 6 25 06/24/28 21:07:30 28 2028  %"},
{0x6effff91,
"Fri Friday Jan January 01/05/29 01:27:45 05 01 01 005 01 27 AM 45 01 5 01 01/05/29 01:27:45 29 2029  %"},
{0x6fffff90,
"Wed Wednesday Jul July 07/18/29 05:48:00 18 05 05 199 07 48 AM 00 29 3 29 07/18/29 05:48:00 29 2029  %"},
{0x70ffff8f,
"Mon Monday Jan January 01/28/30 10:08:15 28 10 10 028 01 08 AM 15 04 1 04 01/28/30 10:08:15 30 2030  %"},
{0x71ffff8e,
"Sat Saturday Aug August 08/10/30 14:28:30 10 14 02 222 08 28 PM 30 31 6 32 08/10/30 14:28:30 30 2030  %"},
{0x72ffff8d,
"Thu Thursday Feb February 02/20/31 18:48:45 20 18 06 051 02 48 PM 45 07 4 07 02/20/31 18:48:45 31 2031  %"},
{0x73ffff8c,
"Tue Tuesday Sep September 09/02/31 23:09:00 02 23 11 245 09 09 PM 00 35 2 35 09/02/31 23:09:00 31 2031  %"},
{0x74ffff8b,
"Mon Monday Mar March 03/15/32 03:29:15 15 03 03 075 03 29 AM 15 11 1 10 03/15/32 03:29:15 32 2032  %"},
{0x75ffff8a,
"Sat Saturday Sep September 09/25/32 07:49:30 25 07 07 269 09 49 AM 30 38 6 38 09/25/32 07:49:30 32 2032  %"},
{0x76ffff89,
"Thu Thursday Apr April 04/07/33 12:09:45 07 12 12 097 04 09 PM 45 14 4 14 04/07/33 12:09:45 33 2033  %"},
{0x77ffff88,
"Tue Tuesday Oct October 10/18/33 16:30:00 18 16 04 291 10 30 PM 00 42 2 42 10/18/33 16:30:00 33 2033  %"},
{0x78ffff87,
"Sun Sunday Apr April 04/30/34 20:50:15 30 20 08 120 04 50 PM 15 17 0 17 04/30/34 20:50:15 34 2034  %"},
{0x79ffff86,
"Sat Saturday Nov November 11/11/34 01:10:30 11 01 01 315 11 10 AM 30 45 6 45 11/11/34 01:10:30 34 2034  %"},
{0x7affff85,
"Thu Thursday May May 05/24/35 05:30:45 24 05 05 144 05 30 AM 45 21 4 21 05/24/35 05:30:45 35 2035  %"},
{0x7bffff84,
"Tue Tuesday Dec December 12/04/35 09:51:00 04 09 09 338 12 51 AM 00 49 2 49 12/04/35 09:51:00 35 2035  %"},
{0x7cffff83,
"Sun Sunday Jun June 06/15/36 14:11:15 15 14 02 167 06 11 PM 15 23 0 24 06/15/36 14:11:15 36 2036  %"},
{0x7dffff82,
"Fri Friday Dec December 12/26/36 18:31:30 26 18 06 361 12 31 PM 30 51 5 52 12/26/36 18:31:30 36 2036  %"},
{0x7effff81,
"Wed Wednesday Jul July 07/08/37 22:51:45 08 22 10 189 07 51 PM 45 27 3 27 07/08/37 22:51:45 37 2037  %"},
{0x7fffff80,
"Tue Tuesday Jan January 01/19/38 03:12:00 19 03 03 019 01 12 AM 00 03 2 03 01/19/38 03:12:00 38 2038  %"} };

#define num_el ((sizeof (data))/(sizeof (data[0])))

char output[512];
size_t returnVal;

char format [] =	"%a %A %b %B %c %d %H %I %j %m %M %p %S %U %w %W %x "
			"%X %y %Y %Z %%";

static int errorCount;
static char temp[25];

main () 
{
int i;

for (i=0; i < num_el; i++) {
    returnVal = strftime
	(output, sizeof(output), format, localtime (&data[i].input));

    if (strcmp (output, data[i].output)) {
	puts ("");
	printf ("Error: test #%d; actual then expected:\n", i);
	puts (output);
	puts (data[i].output);
	errorCount++;
	}

    if (returnVal != strlen (output)) {
	puts ("");
	printf ("Error: test #%d; actual return = %d, "
	    "expected return %d\n", i, returnVal, strlen (output));
	errorCount++;
	}
    }

strcpy (temp, "xxxxxxxxxx");	/* return 0 check */
if (strftime (output, (size_t) 5, temp, localtime (&data[num_el - 1].input)) != 0) {
    puts ("Error:  failed overflow test 1");
    errorCount++;
    }
if (strcmp (temp+5, "xxxxx")) {	/* blown array check */
    puts ("Error:  failed overflow test 2");
    errorCount++;
    }

strcpy (output, "xxxxxxxxxx");	/* return 0 check */
if (strftime (output, (size_t) 5, "%c", localtime (&data[num_el -1].input))) {
    puts ("Error:  failed overflow test 3");
    errorCount++;
    }
if (strcmp (temp+5, "xxxxx")) {	/* blown array check */
    puts ("Error:  failed overflow test 4");
    errorCount++;
    }

if (errorCount) {
    printf (ERRORS);
    exit (1);
    }

printf (NO_ERRORS);
exit (0);
}
 
