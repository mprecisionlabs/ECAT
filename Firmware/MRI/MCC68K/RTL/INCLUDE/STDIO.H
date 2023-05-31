
/************************************************************************/
/* Copyright (c) 1988-1991 Microtec Research, Inc.			*/
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
/*  <stdio.h>	Microtec Research, Inc. ANSI C Compiler  */
/* %W% %G% */

#ifndef __STDIO_H	/* avoid multiple inclusions */
#define __STDIO_H	1

#if __STDC__

    typedef unsigned long fpos_t;

    #ifndef _SIZE_T_DEFINED
	#ifdef _sun
	    typedef int size_t;
	#else
	    typedef _SIZE_T size_t;
	#endif
	#define	_SIZE_T_DEFINED		1
    #endif

#endif /* __STDC__ */

#ifndef _VA_LIST_DEFINED
    #if defined(_MCC80) || defined(_MCC960)
	typedef struct	{
	    char *list;
	    int  arg;
	    } va_list;
    #else
	typedef char *va_list;
    #endif
    #define	_VA_LIST_DEFINED	1
#endif /* __STDARG_H */

typedef struct _iobuf
	{
	unsigned char	*_ptr;
	int		_cnt;
	unsigned char	*_base;
	unsigned char	_flag;
	char  		_file;
#if defined(_MCC68K) || defined(_CCC68K) || defined (_MCC386)
	char  _pad1;		/* padding */
	char  _pad2;
#endif
	}
	FILE;

#if __cplusplus
    #define	NULL		(0)
#else
    #define	NULL		((void *)0)
#endif

#define BUFSIZ		512
#define EOF		(-1)
#define FOPEN_MAX	20
#define FILENAME_MAX	256
#define L_tmpnam	32
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2
#define TMP_MAX		0x7FFFFFFF
#define stdin		(&_iob[0])
#define stdout		(&_iob[1])
#define stderr		(&_iob[2])

#define _IONBF    (0x04)	/* no buffer */
#define _IOFBF    (0x00)	/* fully buffered */
#define _IOLBF    (0x40)	/* line buffered */

extern FILE _iob[];

#undef getc
#undef putc
#undef getchar
#undef putchar
#undef clearerr
#undef feof
#undef ferror

#ifdef __STDC__

    #if __cplusplus
	extern "C" {
    #endif

    extern int    remove(const char *);
    extern int    rename(const char *, const char *);
    extern FILE  *tmpfile(void);
    extern char  *tmpnam(char *);
    extern int    fclose(FILE *);
    extern int    fflush(FILE *);
    extern FILE  *fopen(const char *, const char *);
    extern FILE  *freopen(const char *, const char *, FILE *);
    extern void   setbuf(FILE *, char *);
    extern int    setvbuf(FILE *, char *, int, size_t);
    extern int    fprintf(FILE *, const char *, ...);
    extern int    fscanf(FILE *, const char *, ...);
    extern int    printf(const char *, ...);
    extern int    scanf(const char *, ...);
    extern int    sprintf(char *, const char *, ...);
    extern int    sscanf(const char *, const char *, ...);
    extern int    vfprintf(FILE *, const char *, va_list);
    extern int    vprintf(const char *, va_list);
    extern int    vsprintf(char *, const char *, va_list);
    extern int    fgetc(FILE *);
    extern char  *fgets(char *, int, FILE *);
    extern int    fputc(int, FILE *);
    extern int    fputs(const char *, FILE *);
    extern int    getc(FILE *);
    extern int    getchar(void);
    extern char  *gets(char *);
    extern int    putc(int, FILE *);
    extern int    putchar(int);
    extern int    puts(const char *);
    extern int    ungetc(int, FILE *);
    extern size_t fread(void *, size_t, size_t, FILE *);
    extern size_t fwrite(const void *, size_t, size_t, FILE *);
    extern int    fgetpos(FILE *, fpos_t *);
    extern int    fseek(FILE *, long, int);
    extern int    fsetpos(FILE *, const fpos_t *);
    extern long   ftell(FILE *);
    extern void   rewind(FILE *);
    extern void   clearerr(FILE *);
    extern int    feof(FILE *);
    extern int    ferror(FILE *);
    extern void   perror(const char *);

    extern int    _flsbuf(int, FILE *);
    extern int    _filbuf(FILE *);

    #if __cplusplus
	}
    #endif

#else /* !__STDC__ */

    extern int    remove();
    extern int    rename();
    extern FILE  *tmpfile();
    extern char  *tmpnam();
    extern int    fclose();
    extern int    fflush();
    extern FILE  *fopen();
    extern FILE  *freopen();
    extern void   setbuf();
    extern int    setvbuf();
    extern int    fprintf();
    extern int    fscanf();
    extern int    printf();
    extern int    scanf();
    extern int    sprintf();
    extern int    sscanf();
    extern int    vfprintf();
    extern int    vprintf();
    extern int    vsprintf();
    extern int    fgetc();
    extern char  *fgets();
    extern int    fputc();
    extern int    fputs();
    extern int    getc();
    extern int    getchar();
    extern char  *gets();
    extern int    putc();
    extern int    putchar();
    extern int    puts();
    extern int    ungetc();
    extern int	  fread();
    extern int	  fwrite();
    extern int    fgetpos();
    extern int    fseek();
    extern int    fsetpos();
    extern long   ftell();
    extern void   rewind();
    extern void   clearerr();
    extern int    feof();
    extern int    ferror();
    extern void   perror();

#endif /* __STDC__ */

#define getc(f)	  (--(f)->_cnt >= 0 ? (int) *(f)->_ptr++ : _filbuf(f))

#define putc(c,f)   ( --(f)->_cnt >= 0 					\
			?   ( 						\
			    (_IOLBF & (f)->_flag) 			\
				?   ( 					\
				    ((*(f)->_ptr = (c)) == '\n') 	\
					? 				\
					    ( _flsbuf((*(f)->_ptr),(f)))\
					: 				\
					    ((int) (*(f)->_ptr++)) 	\
				    ) 					\
				: 					\
				    ((int) (*(f)->_ptr++ = (c))) 	\
			    ) 						\
			: 						\
			    ( _flsbuf((c),(f))) 			\
		    )

#define getchar()	getc(stdin)
#define putchar(c)	putc((c),stdout)
#define clearerr(f)	((f)->_flag &= ~(_IOERR | _IOEOF))
#define feof(f)		((f)->_flag & _IOEOF)
#define ferror(f)	((f)->_flag & _IOERR)

/* Microtec extensions: */

#define _IOREAD   0x01		/* readable file */
#define _IOWRT    0x02		/* writable file */
#define _IOMYBUF  0x08		/* system supplied buffer from heap */
#define _IOEOF    0x10		/* end of file has been detected */
#define _IOERR    0x20		/* error has been detected */
#define _IORW     0x80		/* reserved word */

#if (defined(_MCC86) && _MRI_EXTENSIONS)

    #ifdef	fileno
	#undef	fileno
    #endif

    #define	fileno(f)	((f)->_file)

#endif /* (_MCC86 && _MRI_EXTENSIONS) */

#endif /* ifndef __STDIO_H */

