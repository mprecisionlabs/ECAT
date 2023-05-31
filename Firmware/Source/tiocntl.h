#ifndef __TIOCNTL_H				/* Avoid multiple inclusions */
#define __TIOCNTL_H

#include  <shortdef.h>

#define	TI0_SELECT		0x00				/* Timer Input enable lines			*/
#define	TI1_SELECT		0x01
#define	TI2_SELECT		0x02
#define	TI3_SELECT		0x03

#define	TILCL0_TI0		0x00				/* Timer Input Mapping selections	*/
#define	TILCL0_TI2		0x01

#define	TO0_TOLCL0		0x00				/* Timer Output Mapping Selections	*/
#define	TO0_TOLCL1		0x01
#define	TO0_TOLCL2		0x02
#define	TO0_DISABLE		0x03

#define	DISABLE			0
#define	ENABLE			1

char	TIO_TIMap(uchar,uchar);
char	TIO_TOMap(uchar,uchar);
char	TIO_TISelect(uchar,uchar,uchar);
char	TIO_Refresh(uchar);
void	TIO_Reset(void);
void	TIO_EFTDisable(void);
void	TIO_SRGDisable(void);
void	TIO_CPLDisable(void);
void  TIO_SelectLinesync(uchar);

int	istat(int);

#endif								/* ifndef TIOCNTL_H */
