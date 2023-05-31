#ifndef __SERINT_H			/* Avoid multiple inclusions */
#define __SERINT_H

#include <stdio.h>
#include <shortdef.h>

extern volatile ushort	SCCR0;

#define	SCI_setbaud(b)		(SCCR0=(524288/b))
#define	SIN_SIZE				1000
#define	SOUT_SIZE			200
#define RX_FIFO_SIZE		1024
#define TX_FIFO_SIZE		256
#define	RDR					0x0040
#define	TDRE					0x0100

/*
 *	Private function prototypes
 */
interrupt void	SCI_trap(void);

/*
 *	Function prototypes
 */
uchar	SCI_charavail(void);
void		SCI_init(int, uchar);
void		SCI_kill(void);
char		_INCHRW(void);
void		_OUTCHR(char);

#endif							/* ifndef __SERINT_H */
