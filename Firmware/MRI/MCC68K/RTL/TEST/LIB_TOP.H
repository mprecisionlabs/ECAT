/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1994 Microtec Research, Inc.       			    */
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

/*
	This header file pulls in the configuration file used in building
	libraries.  It also provides a mechanism for forcing debug to
	be turned on regardless of the setting the the configuration file.
*/


#ifdef	CONFIGURATION_FILE		/* Include discretionary options file */
    #define quote(a) #a
    #define apply(f,s) f(s)
    #include apply(quote, CONFIGURATION_FILE)
#endif

#if DEBUG_ON
    #if __OPTION_AVAIL ("-Gr")	/* Is -Gr option available on this compiler */
        #pragma option -nGr
    #endif

    #pragma option -g
#endif

