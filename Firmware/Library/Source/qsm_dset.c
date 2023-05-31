#include "qsm.h"

extern volatile ushort  QSM_tx[];
extern volatile ushort  SPCR0;
extern volatile ushort  SPCR1;
extern volatile ushort  SPCR2;
extern volatile uchar   SPSR;

/*
 *  DAC_set -- Sets the value of one of the eight bit DAC's
 *
 *       Syntax: void DAC_set(char channel,unsigned char value);
 *  Description: Sets the DAC number 'channel' to 'value'.
 *               a value of 0 = 0 volts and 255 = 5.0 volts.
 *   Exceptions: The QSM_init() function must have been called before
 *               setting the D/A values.
 *  Source Code: QSM.C
 *      Include: QSM.H
 *       Status: Complete and tested.
 */

void	DAC_set(
int	ch,
int	value)
{
	ushort	flags;

											/* Stop the system						*/
	SPCR1 = 0x0008;					/* QSPI control register 1				*/
											/* Change the channel range to 10,12*/
	SPCR2 = 0x2C0A;					/* QSPI control register 2				*/
											/* Change the clock polarity			*/
	SPCR0 = 0x8208;					/* QSPI control register 0				*/

	flags = SPSR;
	SPSR  = 0x0000;					/* clear the flags */
											/* Set the value	 */
	QSM_tx[10] = ((ch & 7)*256) + (value & 255);
											/* Restart the QSM */
	SPCR1 = 0x8008;					/* QSPI control register 1 */


	while(!(SPSR & 128))				/* Wait until it completes */
		;
											/* Stop the system			*/
	SPCR1 = 0x0008;					/* QSPI control register 1 */


											/* Set up the old stuff again */
	SPCR0 = 0xB008;					/* QSPI control register 0		*/
	SPCR2 = 0xC800;					/* QSPI control register 2		*/
	SPCR1 = 0x8008;					/* QSPI control register 1		*/
}

