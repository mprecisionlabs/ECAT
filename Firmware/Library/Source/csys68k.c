/*
 *	MC 68000 C Library - Copyright 1985 to 1987 Microtec Research, Inc.
 *	This program is the property of Microtec Research, Inc,
 */
/*
 *	This library assumes that the following UNIX style system routines exist:
 *
 *	char *sbrk (size) int size;
 *		This should increment the heap pointer by size bytes
 *		and return the old heap pointer.  If there is not enough space on the
 *		heap, it should return -1.
 *
 *	int open(filename,mode) char *filename; int mode;
 *		Opens the specified file, depending on the indicated mode,the modes are
 *		0	=> open for reading
 *		1	=> open for writing
 *		2	=> open for reading & writing (not used)
 *		1|_IO_EOF	=> open for writing and seek to eof
 *		open should return a number between 0 and 19 that may be associated
 *		with an element in the iob array.  On failure it should return -1.
 *
 *	int creat(filename,prot) char *filename; int prot;
 *		Creates and opens (for writing) a file named filename with protection
 *		as specified by prot.  For things like terminals that cannot be created
 *		it should just open the device.  It should return a number between 0-19
 *		that may be associated with an iob array element.  On failure it should
 *		return -1.
 *
 *	int read(fno,buf,nbyte) int fno, nbyte; char *buf;
 *		Reads at most nbyte bytes into buf from the file connected to fno
 *		(where fno is one of the file descriptors returned by open() or creat()).
 *		It should return the number of bytes read (a value of 0 is used to 
 *		indicate EOF), or a -1 on error.
 *
 *	int write(fno,buf,nbyte) int fno, nbyte; char *buf;
 *		Writes at most nbyte bytes into the file connected to fno (where fno is
 *		one of the file descriptors returned by open() or creat()) into buf.
 *		It should return the number of bytes written, or a -1 to indicate an error.
 *
 *	int close(fno) int fno;
 *		Closes the file associated with the file descriptors fno (returned
 *		by open() or creat()).  Should return 0 if all goes well or -1 on error.
 *
 *	void _exit(code) int code;
 *		Exits from the program, with a status code specified by code.
 *		Should not return.
 *		NOTE: Code for _exit previously was located in this module.  _exit has
 *		been relocated to "entry.s".
 *
 *	These routines should use the variable errno to explain what went
 *	wrong when problems occur (the various error codes are defined in errno.h).
 *
 *	The following is a very trivial version of these calls (except sbrk())
 *	that assumes the following:
 *
 *		1)	There is one input device of interest, and characters can be read
 *			from it by calling _INCHRW() which returns an integer of value
 *			0..255 or -1 for error.
 *		2)	There is one output device of interest, and characters can be
 *			written to it by calling _OUTCHR (ch).  Its return will be ignored.
 *		3)	There is a way of entering the program, setting up the heap and
 *			stack pointers, and then it will call START() which in turn calls
 *			the user's main() function.
 *
 *	The user still has to provide 4 routines (written in assembly language):
 *		1)	Initialization routine
 *		2)	sbrk()
 *		3)	_INCHRW()
 *		4)	_OUTCHR().
 */

#include <stdio.h>
#include <errno.h>
#include <mriext.h>
#include <string.h>
#include "serint.h"
/*#include "gpib.h"*/
#include "key.h"
#include "display.h"


#define eoln 1
/* eoln is used to indicate the end of line characters for the host system.
 * 	0	carriage return (13) as end of line, ex: Microtec 68000 simulator
 *		1	carriage return (13) followed by line feed (10) as end of line, ex: MSDOS, debugger
 *		2	line feed (10) as end of line, ex: UNIX
 */                                   

FILE	_iob[FOPEN_MAX];
volatile int errno;
int	reset_flags;

int	_lastp;			/* variable to be used in 'strtok'	*/
long	_randx;			/* variable to be used in 'rand'		*/
char	_ctbuf[26];		/* array used by ctime to return time date string */

#define HEADER struct mem

HEADER
{
	HEADER	*next;
	unsigned	size;
};

HEADER	_membase;
HEADER	*_avail;
char		_badlist;

unsigned short	_fp_status;
unsigned short	_fp_control;

/*
 * This is a sample of the _START() rotuine which is called by the user's
 * initialization routine to start a C program.  It opens stdin, stdout and
 * stderr to the console.  Then it calls the user's main() function.
 *
 * new added: 
 *		defines and initializes variables for run-time functions
 *		'strtok', 'malloc', 'rand'.
 *		stdin, stdout, and stderr are opened as unbuffered.
 *
 * Before entering _START(), the execution environment should already be
 * set up, including the heap, the stack, and, optionally, the command line.
 */

_START(argc, argv, env)
int	argc;
char	*argv[];
char	*env[];
{
	int	yyy;
	FILE	*xxx;

	_randx = 1;								/* Initialize for 'rand'	*/

	_membase.next = &_membase;			/* Initialize for malloc	*/
	_avail = &_membase;
	 reset_flags = 0;

	fopen("/dev/tty", "r");					/* stdin		*/
	fopen("/dev/tty", "w");					/* stdout	*/
	xxx = (fopen ("/dev/tty", "w"));		/* stderr	*/
	yyy = fileno(xxx);

	/* Make unbuffered */
	setbuf(stdin,  NULL);					/* stdin		*/
	setbuf(stdout, NULL);					/* stdout	*/
	setbuf(stderr, NULL);					/* stderr	*/

	main(argc, argv, env);
}

/* Returns the number of free _iob structures */

static	find_free()
{
	register int	i;

	for(i = 0; i < FOPEN_MAX; i++)
	{
		if(!(_iob[i]._flag&(_IOREAD|_IOWRT)))
			return(i);
	}
	errno = EMFILE;
	return(-1);
}

open(ignore, mode)
char	*ignore;
int	mode;
{
	register int	fno;

	if(!strcmp(ignore,"KEYS"))
		fno = 6;
	else
#ifdef DOGPIB
 		if(!strcmp(ignore,"GPIBIN"))
			fno = 5;
		else
			if(!strcmp(ignore,"GPIBOUT"))
				fno = 4; 
			else
#endif
				if(!strcmp(ignore,"DISPLAY"))
					fno = 9;
				else
					if((fno = find_free()) == -1)
						return (-1);
	mode &= 3;
	_iob[fno]._file = fno;
	if(mode != 1)
		_iob[fno]._flag = _IOREAD;
	if(mode != 0)
		_iob[fno]._flag |= _IOWRT;
#ifdef DOGPIB
	if(fno == 4)
		gpib_file_init();				/* GPIB out		*/
	if(fno == 5)
		gpib_file_init();				/* GPIB in		*/
#endif
	if(fno == 6)
		KEY_reset();					/* Keyboard		*/
	if(fno == 9)
		LCD_reset();					/* New display	*/
	return(fno);
}

creat(ignore, prot)
char	*ignore;
int	prot;
{
	return(open(ignore, 1));
}

close(fno)
int	fno;
{
	if(!(_iob[fno]._flag&(_IOREAD|_IOWRT)))
	{
		errno = EBADF;
		return(-1);
	}
	_iob[fno]._flag = 0;
	return(0);
}

read(fno,buf,size)
int	fno;
register char	*buf;
register int	size;
{
	register int	cnt = 0;
	register int	ch;

	if(!(_iob[fno]._flag&_IOREAD))
	{
		errno = EBADF;
		return(-1);
	}
	while(--size >= 0) 
	{
		if(fno < 3)
			ch = _INCHRW();
		else
#ifdef DOGPIB
			if(fno == 5)
				ch = gpib_get_char();
			else
#endif
				if(fno == 6)
					ch = KEY_getchar();
		if(ch == -1) 
		{	/* Read in one char */
			errno = EIO;
			return (-1);
		}
		if(eoln == 0 && ch == '\r')
			ch = '\n';							/* Replace '\r' by '\n' */
		*(buf++) = ch;
		cnt++;
		if(ch == '\r' || ch == '\n')		/* CR or LF terminates the read */
			break;
	}
	return(cnt);
}

write(fno,buf,size)
int	fno;
register char	*buf;
register int	size;
{
	register int	cnt = 0;

	if(!(_iob[fno]._flag&_IOWRT)) 
	{   
		errno = EBADF;
		return(-1);
	}
	if(fno < 3)
	{
		while(--size >= 0) 
		{
			if(eoln == 0 && *buf == '\n')
				*buf = '\r';							/* Replace by '\r'		*/
			if(eoln == 1 && *buf == '\n')
				_OUTCHR('\r');							/* Output a '\r' first	*/
			_OUTCHR(*(buf++));						/* Output one char		*/
			cnt++;
		}
	}
	else 
	{	/* fno >= 3 */
		switch (fno)
		{ 
#ifdef DOGPIB			
			case 4:
				while(--size >= 0) 
				{
					gpib_write(*(buf++));
					cnt++;
					break;
				}
#endif
			case 9:
				while(--size >= 0) 
				{
					LCD_putc(*(buf++));
					cnt++;
					break;
				}
		}
	}
	return(cnt);
}

int	fcavail(FILE *file)
{
	int	n = fileno(file);

	switch(n)
	{
		case 0:
		case 1:
		case 2:
			return(SCI_charavail());
		case 3:				/* Display	*/
 			return (0);
#ifdef DOGPIB
 		case 4:				/* Gpibout	*/
			return(0);		/* Never a char avail */
		case 5:				/* GPIBin	*/
			return(gpib_char_avail());
#endif
		case 6:				/* Keyboard	*/
			return(KEY_hit());
	}
	return(0);
}

