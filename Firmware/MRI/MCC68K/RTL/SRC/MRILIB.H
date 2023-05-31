/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1991, 1993 Microtec Research, Inc.       */
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


/* %W% %G% */

/************************************************************************/
/*									*/
/*	mrilib.h							*/
/*		Declare _ftoa() and _ltoa().				*/
/*		Also declare ARGS_BY_REGISTER to indicated those	*/
/*			targets which pass arguments by register.	*/
/*									*/
/************************************************************************/

extern int    _ftoa(double value, char *string, int format, int prec);
extern int    _ltoa(long value, char *string);


#if	_MCC88K || _MCCSP || _MCC29K || _MCC80 || _MCC960 

    #define ARGS_BY_REGISTER	1

#endif
