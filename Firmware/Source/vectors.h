#ifndef  __VECTORS_H			/* Avoid multiple inclusions */
#define  __VECTORS_H

#define	TPUARB				0x01		/* The IRQ Arbitration number				*/
#define	TPU_IRQ_LEVEL		0x06		/* The IRQ level for TPU interrupts		*/

												/* CAUTION										*/
#define	TPU_VECTOR_BASE	0x60		/* The IRQ vector for TPU IRQ's			*/
                                	/* Must be divisible by 16					*/

#define	TIMEARB				0x02		/* The IRQ Arbitration number				*/
#define	TIME_IRQ_LEVEL		0x02		/* The IRQ level for periodic ints 		*/
#define	TIME_VECTOR_BASE	0x40		/* The IRQ vector for periodic IRQ's	*/
#define	QSM_ARB				0x01		/* The IRQ Arbitration number				*/
#define	QSM_IRQ_LEVEL		0x05		/* The IRQ level for QSM interrupts		*/
#define	SCI_IRQ_LEVEL		0x05		/* The IRQ level for SCI interrupts		*/

#define	QSM_VECTOR_BASE	0x44		/* The IRQ vector for QSM IRQ's			*/
												/* Must be divisible by 2!!				*/

#endif							/* ifndef __VECTORS_H */
