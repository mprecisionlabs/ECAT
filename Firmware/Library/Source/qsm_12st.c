#include "qsm.h"

extern volatile ushort  QPDR;
extern volatile ushort  QSM_tx[];
extern volatile ushort  SPCR1;
extern volatile ushort  SPCR2;
extern volatile uchar   SPSR;

/*
 * DAC_set12 -- Sets the value of the twelve bit DAC.
 *
 *       Syntax: void DAC_set12(unsigned short value);
 *  Description: Sets the DAC to 'value'.
 *               a value of 0 = 0 volts and 4095 =  5.0 volts. ( 5 volt scale)
 *               a value of 0 = 0 volts and 4095 = 10.0 volts. (10 volt scale)
 *   Exceptions: The QSM_init() function must have been called before
 *               setting the D/A values.
 *  Source Code: QSM.C
 *      Include: QSM.H
 *       Status: Complete and tested.
 */

void	DAC_set12(
int	value)
{
	QSM_tx[9] = value;
	SPCR1 = 0x0008;					/* QSPI control register 1 */
	SPCR2 = 0x0909;					/* QSPI control register 2 */
	value = SPSR;
	SPSR  = 0;
	SPCR1 = 0x8008;					/* QSPI control register 1 */

	while((SPSR & 0x80 ) == 0)
		;
	QPDR  = 0x0058;					/* Load 12 bit DAC */
	QPDR  = 0x0078;					/* Reset to normal */
	SPCR1 = 0x0008;					/* QSPI control register 1 */
	SPCR2 = 0xC800;					/* QSPI control register 2 */
	value = SPSR;
	SPSR  = 0;
	SPCR1 = 0x8008;					/* QSPI control register 1 */
}

