#include "lib_top.h"


#if EXCLUDE_csys
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
#if _MCC960
    #pragma option -nGr			/* Make sure -Gr is turned off */
    #pragma option -g			/* Force -g option to be used always */

    #include "i960.h"			/* Define 960 data structures */
#endif

/************************************************************************/
/* C System Initialization Routine					*/
/*	- Copyright 1985 to 1991, 1992 Microtec Research, Inc.		*/
/*	- This program is the property of Microtec Research, Inc,	*/
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


/****************************************************************************

This library assumes that the following UNIX style system routines exist:

int close(fno) int fno;
    Closes the file associated with the file descriptors fno (returned by
    open() or creat()).  Should return 0 if all goes well or -1 on error.

int creat(filename,prot) char *filename; int prot;
    Creates and opens (for writing) a file named filename with protection
    as specified by prot.  For output devices like terminals that cannot
    be created, it should just open the device.  Creat should return a
    file descriptor (or "file handle") for the file it opens.  On failure
    it should return -1.

void _exit(code) int code;
    Exits from the program, with a status code specified by code.
    Should not return. Code for _exit is located in "entry.c" (crt0.mac
    for mcc86).

void exit (val) int val;
    Executes all functions specified in atexit() function calls.  Closes
    all open files and then calls _exit using "val" as the argument.  Does
    not return.

long lseek(fno, offset, whence);
    Positions a file associated with the file descriptor fno to an arbitrary
    byte position specified by offset and returns the new position as a long.
    If whence is 0, the new position is relative to the beginning of the
    file.  If whence is 1, the new position is relative to the current
    position.  If whence is 2, the new position is relative to the end
    of the file.

int read(fno,buf,nbyte) int fno, nbyte; char *buf;
    Reads at most nbyte bytes into buf from the file connected to fno (where
    fno is one of the file descriptors returned by open() or creat()). It
    should return the number of bytes read (a value of 0 is used to
    indicate EOF), or a -1 on error.  All library input occurs through
    the read function.
	
int open(filename, flags, mode) char *filename; int type, mode;
    Opens the specified file, depending on the indicated type,  the types are
    as follows:

		O_RDONLY        0x0000		open for read only
		O_WRONLY        0x0001 		open for write only
		O_RDWR          0x0002		open for read and write
		O_APPEND        0x0008		writes performed at EOF
		O_CREAT         0x0200		create file
		O_TRUNC         0x0400		truncate file
		O_FORM          0x4000		text file
		O_BINARY	0x8000		binary file

    The value of mode is always set to 0744 for UNIX compatibility.
    Open should return a file descriptor (or "file handle") for the file
    it opens.  On failure it should return -1.

char *sbrk (size) int size;
    This should increment the heap pointer by size bytes
    and return the old heap pointer.  If there is not enough space on the
    heap, it should return -1.

int write(fno,buf,nbyte) int fno, nbyte; char *buf;
    Writes at most nbyte bytes into the file connected to fno (where fno is
    one of the file descriptors returned by open() or creat()) into buf.  It
    should return the number of bytes written, or a -1 to indicate an error.
    All library output occurs through the write function.

    For mcc68k only:
	If read or write is called is called, the routines _INCHRW and
	_OUTCHR, respectively, must also be provided.  _INCHRW reads from an
	input device and returns an integer of value 0..255 or -1 for error.
	_OUTCHR writes one character to an output device; its return value
	is ignored.

The above routines should use the variable errno to explain what went wrong
when problems occur (the various error codes are defined in errno.h).

Stubs or minimal implementations for the above routines have been provided
in files residing in the directory containing this file.  The user must
provide working versions of these routines if they are used in used in his
application.

****************************************************************************/

/************************************************************************/
/*  This is a sample of the _START() routine which is called by the	*/
/*  user's initialization routine to start a C program.  		*/
/*									*/
/*									*/
/*  *	It defines and initializes variables for run-time functions	*/
/*	'strtok', 'malloc', 'rand', time, and i/o functions.  It also	*/
/*	defines 'errno'. All data is initialized at run-time in _START.	*/
/*	This data does not require _initcopy() for initialization.	*/
/*									*/
/*  *   For all except mcc86:						*/
/*	If the preprocessor symbol "EXCLUDE_INITDATA" is not defined,	*/
/*	it calls the _initcopy routine to initialize RAM. _initcopy	*/
/*	copies data from the section created by the linker's INITDATA	*/
/*	command.  See the assembler reference manual for more		*/
/*	information on the INITDATA command.				*/
/*									*/
/*  *	It opens stdin, stdout, stderr, stdaux, and stdprn to the	*/
/*	console. The streams stdin and stdout are opened using line	*/
/*	buffering.  The streams stderr, stdaux, and stdprn are opened	*/
/*	unbuffered.							*/
/*	Note: mcc960 does not open stderr, stdaux, or stdprn.		*/
/*									*/
/*  *	For all except mcc86:						*/
/*	It generates error messages if the _initcopy routine failed.	*/
/*									*/
/*  *	If the pre-processor symbol INCLUDE_BUILD_ARGV is set, parse	*/
/*	the input command line and establish argc and argv for main().	*/
/*									*/
/*  *	Finally, it calls the user's main() function.			*/
/*									*/
/*  Before entering _START(), the execution environment should already	*/
/*  be set up, including the heap, the stack, and, optionally, the	*/
/*  command line.							*/
/*									*/
/*  The routine _initcopy is used to initialize RAM when the linker	*/
/*  INITDATA command is used.  Compile csys.c with -DEXCLUDE_INITDATA	*/
/*  if the linker's INITDATA command is not used.			*/
/*									*/
/*  _initcopy is called in this routine to insure that stderr is opened	*/
/*  when any error message generated by _initcopy is written. The user	*/
/*  may adjust the manner in which _initcopy is called and how error	*/
/*  conditions are handled to fit the application.			*/
/*									*/
/*  NOTE:  The tests which generate error messages for the _initcopy	*/
/*	   routine may be removed once testing is complete.		*/
/*									*/
/*  NOTE:  The opening of any stream in this file may be removed if	*/
/*	   that stream is not used in the user program.  Also, no	*/
/*	   hardware initialization is performed when stdin, stdout, and	*/
/*	   stderr are opened.  If initialization needs to be performed,	*/
/*	   the user must provide it.					*/
/************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#if ! _MCC960
    #include <mriext.h>		/* define stdaux and stdprn */
#endif

#if ! (_MCC86 || _MCC386)

    /********************************************************************/
    /*	Note:	The following variables, _simulated_input and		*/
    /*		_simulated_output, are used by read() and write()	*/
    /*		to allow reading and writing of data through the	*/
    /*		debugger.  They should be removed when read() and	*/
    /*		write() are rewritten for a particular application.	*/	
    /*									*/
    /*		The variables are initialized to place them in the	*/
    /*		initialized data section.  The #pragma is then used to	*/
    /*		place the variables in a unique section called		*/
    /*		"ioports".						*/
    /*									*/
    /*		The padding specified is required by various library	*/
    /*		routines, some of which read extra words.  The padding	*/
    /*		is to prevent these routines from reading		*/
    /*		_simulated_input.					*/
    /********************************************************************/
    #pragma	options	-NIioports
    
    #if _MCCG32
        static	 int  _space1 = 0;	/* padding */
        static	 int  _space2 = 0;	/* padding */
    #endif
    
    #if _MCC960
        static	 int  _space1 = 0;	/* padding */
        static	 int  _space2 = 0;	/* padding */
        static	 int  _space3 = 0;	/* padding */
        static	 int  _space4 = 0;	/* padding */
    #endif
    
    volatile unsigned char _simulated_input = 0;
					/* used by simulator for input */
    volatile unsigned char _simulated_output = 0;
					/* used by simulator for output */	
    
#endif		/* ! (_MCC86 || _MCC386) */



/************************************************************************/
/*	The following defines information used by the build_argv()	*/
/*	function.  build_argv() will parse a command line placed in	*/
/*	_com_line and place the information in argc and argv to be	*/
/*	passed to main().						*/
/************************************************************************/

#ifdef	INCLUDE_BUILD_ARGV

    char	_com_line [512];	/* the command line */
    #define	MAX_ARGC 32
    static void	build_argv (void);	/* computes argc and builds argv array*/

#else	/* ! INCLUDE_BUILD_ARGV */

    #define	MAX_ARGC 1

#endif	/* INCLUDE_BUILD_ARGV */



/************************************************************************/
/*	The following data passes parameters to main().			*/
/************************************************************************/

char	       **_environ;			/* the environment vector */
static unsigned  argc;				/* the argument count */
static char	*argv [MAX_ARGC];		/* the argument vector */



/************************************************************************/
/*	The following data is used by the atexit() function.		*/
/************************************************************************/

#if EXCLUDE_ATEXIT
    /* No code inserted here */
#else
    #define	_ATEXIT_MAX	32		/* Do not modify */
    void	(*_atexit_stack[_ATEXIT_MAX]) (void);	/* atexit function */
							/* stack */
    short	_atexit_top;			/* index used by atexit */
#endif



/************************************************************************/
/*	The following data is used by the signal() and raise()		*/
/*	functions.							*/
/************************************************************************/

#if EXCLUDE_SIGNAL_RAISE
    /* No code inserted here */
#else
    void	(*_sig_functs[6]) (int);	/* signal handler addresses */
						/* used by signal() and */
						/* raise() */
#endif


/************************************************************************/
/*	The following is used by C i/o streams.				*/
/************************************************************************/

#if EXCLUDE_IOB
    /* No code inserted here */
#else
    FILE	  _iob[FOPEN_MAX];
#endif


/************************************************************************/
/*	Definition of errno.						*/
/************************************************************************/

#if EXCLUDE_ERRNO
    /* No code inserted here */
#else
    int		errno;
#endif



/************************************************************************/
/*	The following data is used by strtok().				*/
/************************************************************************/

#if EXCLUDE_STRTOK
    /* No code inserted here */
#else
    char	*_lastp;	/* variable to be used in 'strtok' */
#endif



/************************************************************************/
/*	The following is used by rand() and srand().			*/
/************************************************************************/

#if EXCLUDE_RAND
    /* No code inserted here */
#else
    long	_randx;		/* variable to be used in 'rand' */
#endif



/************************************************************************/
/*	The following data is used by the <time.h> functions.		*/
/************************************************************************/

#if EXCLUDE_TIME
    /* No code inserted here */
#else
    char	_ctbuf[26];	/* array used by asctime() and ctime() to */
				/* return time-date string */
    struct tm	_xtm;		/* return structure used by gmtime() and */
				/* localtime() functions */
#endif



/************************************************************************/
/*	The following is used by the memory allocation routines:	*/
/*	malloc(), calloc(), free(), realloc(), and zalloc().		*/
/************************************************************************/

#if EXCLUDE_MALLOC
    /* No code inserted here */
#else
    #define HEADER struct mem
    
    HEADER {
    	HEADER    *next;
    	unsigned  size;
    	};
    
    HEADER	 _membase;
    HEADER	*_avail;
    char	 _badlist;
#endif


_START (void)

{
#if ! _MCC86
    #ifdef EXCLUDE_INITDATA
	/* No code inserted here */
    #else
	int _initcopy ();
	int initReturn;

	initReturn = _initcopy ();
    #endif
#endif

#if EXCLUDE_RAND
    /* No code inserted here */
#else
    _randx = 1;					/* initialize for 'rand'  */
#endif

#if EXCLUDE_MALLOC
    /* No code inserted here */
#else
    _avail = _membase.next = &_membase;		/* initialize for malloc */
#endif

            /* OPEN THE STANDARD I/O FILES */

#if EXCLUDE_IOB
    /* No code inserted here */
#else
    #if EXCLUDE_LINE_BUFFER_DEFAULT
    
        stdin->_file = 0;
        stdin->_flag = _IOREAD | _IONBF;
        stdout->_file = 1;
        stdout->_flag = _IOWRT | _IONBF;
    
    #else
    
        stdin->_file = 0;
        stdin->_flag = _IOREAD | _IOLBF;
        stdout->_file = 1;
        stdout->_flag = _IOWRT | _IOLBF;
    
    #endif
    
    #if ! _MCC960
        stderr->_file = 2;
        stderr->_flag = _IOWRT | _IONBF;
        stdaux->_file = 3;
        stdaux->_flag = _IOWRT | _IOREAD | _IONBF;
        stdprn->_file = 4;
        stdprn->_flag = _IOWRT | _IONBF;
    #endif
#endif

#if ! _MCC86
    #ifdef EXCLUDE_INITDATA
	/* No code inserted here */
    #else
	if (initReturn)
      	{
	    static const char init[]  = "Data initialization failed - ";
	    static const char start[] = "No starting point\n";
	    static const char known[] = "Unknown operation\n";
	    static const char end[]   = "No end mark\n";

	    write (2, (char *) init, sizeof(init)-1);	/* Write to stderr */
	    if (initReturn == 1)
	        write (2, (char *) start, sizeof(start)-1);
	    else if (initReturn == 2)
	        write (2, (char *) known, sizeof(known)-1);
	    else
	        write (2, (char *) end, sizeof(end)-1);
	    exit (1);			/* fatal error -- abort execution */
	    }
    #endif
#endif

#ifdef INCLUDE_BUILD_ARGV
    build_argv ();
#else
    argv[0] = (char *) NULL;		/* As per ANSI */
#endif

    exit (main (argc, argv, _environ));
}

#ifdef INCLUDE_BUILD_ARGV

    static void build_argv (void)	/* Constructs the argv array from the */
    {					/* command line (_com_line). */
        char * line = _com_line;
    
        for (argc = 0; argc < MAX_ARGC; ) {

	    while (*line == ' ' ||	/* strip white space */
	    	    (*line >= '\t' && *line <= '\r'))
		line++;

	    if (*line == '\0') break ;
    
	    argv [argc++] = line ;	/* point to next token */
    
	    while (*line != '\0'	/* skip over and terminate this token */
	    	    && *line != ' '
	    	    && !(*line >= '\t' && *line <= '\r'))
		line++ ;

	    if (*line == '\0')
		break ;
	    *line++ = '\0' ;
        }
        argv[argc] = (char *) NULL;	/* as per ANSI */
    }

#endif		/* INCLUDE_BUILD_ARGV */
 
#endif /* EXCLUDE_csys */
 
