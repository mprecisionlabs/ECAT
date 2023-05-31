
/************************************************************************/
/* Copyright (c) 1988-1989  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

/*  <stdio.h>	Microtec Research, Inc. ANSI C Compiler  */
/* @(#)stdio.h	1.17 9/18/90 */

#ifndef __STDIO_H /* avoid multiple inclusions */
#define __STDIO_H

typedef unsigned size_t;
typedef unsigned long fpos_t;
typedef char *va_list;

typedef struct _iobuf
	{
	char *_ptr;
	int   _cnt;
	char *_base;
	char  _flag;
	char  _file;
	}
	FILE;

#define NULL		((void*)0)
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

#define _IONBF    0x04		/* no buffer */
#define _IOFBF    0x00		/* fully buffered (unimplemented) */
#define _IOLBF    0x00		/* line buffered (unimplemented) */

extern FILE _iob[];

#undef getc
#undef putc
#undef getchar
#undef putchar
#undef clearerr
#undef feof
#undef ferror

#if _FPU		/* Convert functions to '881 versions */

#undef	printf
#undef	fprintf
#undef	sprintf
#undef	vprintf
#undef	vfprintf
#undef	vsprintf
#undef	fscanf
#undef	sscanf
#undef	scanf

#define	printf		_printf881
#define	fprintf		_fprintf881
#define	sprintf		_sprintf881
#define	vprintf		_vprintf881
#define	vfprintf	_vfprintf881
#define	vsprintf	_vsprintf881
#define	fscanf		_fscanf881
#define sscanf		_sscanf881
#define	scanf		_scanf881

#endif	/* _FPU */

#ifdef __STDC__

#if __cplusplus
extern "C" {
#endif

extern int    remove(const char *filename);
extern int    rename(const char *old, const char *next);
extern FILE  *tmpfile(void);
extern char  *tmpnam(char *s);
extern int    fclose(FILE *stream);
extern int    fflush(FILE *stream);
extern FILE  *fopen(const char *filename, const char *mode);
extern FILE  *freopen(const char *filename, const char *mode, FILE *stream);
extern void   setbuf(FILE *stream, char *buf);
extern int    setvbuf(FILE *stream, char *buf, int mode, size_t size);
extern int    fprintf(FILE *stream, const char *format, ...);
extern int    fscanf(FILE *stream, const char *format, ...);
extern int    printf(const char *format, ...);
extern int    scanf(const char *format, ...);
extern int    sprintf(char *s, const char *format, ...);
extern int    sscanf(const char *s, const char *format, ...);
extern int    vfprintf(FILE *stream, const char *format, va_list arg);
extern int    vprintf(const char *format, va_list arg);
extern int    vsprintf(char *s, const char *format, va_list arg);
extern int    fgetc(FILE *stream);
extern char  *fgets(char *s, int n, FILE *stream);
extern int    fputc(int c, FILE *stream);
extern int    fputs(const char *s, FILE *stream);
extern int    getc(FILE *stream);
extern int    getchar(void);
extern char  *gets(char *s);
extern int    putc(int c, FILE *stream);
extern int    putchar(int c);
extern int    puts(const char *s);
extern int    ungetc(int c, FILE *stream);
extern size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
extern size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
extern int    fgetpos(FILE *stream, fpos_t *pos);
extern int    fseek(FILE *stream, long offset, int whence);
extern int    fsetpos(FILE *stream, const fpos_t *pos);
extern long   ftell(FILE *stream);
extern void   rewind(FILE *stream);
extern void   clearerr(FILE *stream);
extern int    feof(FILE *stream);
extern int    ferror(FILE *stream);
extern void   perror(const char *s);

extern int    _flsbuf(int ch, FILE *stream);
extern int    _filbuf(FILE *stream);

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
extern size_t fread();
extern size_t fwrite();
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

#define getc(f)	   (--(f)->_cnt >= 0 ? 0xFF & *(f)->_ptr++ : _filbuf(f))
#define putc(c,f)  (--(f)->_cnt >= 0 ? (*(f)->_ptr++ = (c)) & 0xFF : \
			_flsbuf((c),(f)))

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
#define _IOSTRG   0x40		/* this stream is actually a string */
#define _IORW     0x80		/* reserved word */

#endif /* ifndef __STDIO_H */

