#include <stdio.h>
#include <math.h>
#include "intface.h"
#include "QSM.h"

/*
 *	External variables
 */
extern volatile ushort	ldo_state;
extern volatile ushort	PEAK_LD5;
extern volatile uint	interlockData;

/*
 *      Routine:	ECAT_InterfaceCheck
 *       Module:	INTFACE.C
 * Date Created:	August 05, 1992
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Checks to see if the designated interface exists or not.
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	bay		- Indicates bay (0 - 15)
 *      Globals:	None
 *       Locals:	tvar		- Temporary scratch variable
 *      Returns:	 0	- Success
 *					-1	- Illegal Bay choice
 *					-2	- Interface board does not exist
 */
char	  ECAT_InterfaceCheck(
uchar  bay)
{
	 uchar	tvar;

 	 if(bay >= MAX_MODULES)
  	  return(-1);

 	 for(tvar = 0; tvar < 5; tvar++)
	 {
		ECAT_9346KernelWrite(bay,WRAP_BIT_WRITE);
		if(!(ECAT_9346KernelRead(bay) & WRAP_BIT_READ))
			return(-2);
		ECAT_9346KernelWrite(bay,0x00);
		if((ECAT_9346KernelRead(bay) & WRAP_BIT_READ))
			return(-2);
	 }
	 return(0);
}

/*
 *      Routine:	ECAT_ClkWrite
 *       Module:	INTFACE.C
 * Date Created:	August 06, 1992
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Clocks a data bit into the 9346 Serial EEPROM
 *  Assumptions:	This is the lowest level 9346 kernel routine so it assumes
 *					that the passed bay value is valid (this may change in
 *					the future)
 *   Known Bugs:	None
 *   Parameters:	bay		- Indicates the bay (0 - 15)
 *					value		- The data bit to be clocked into the 9346
 *      Globals:	None
 *       Locals:	tvar		- Temporary scratch variable
 *      Returns:	None
 */
void	ECAT_ClkWrite(
uchar	bay,
ushort	value)
{
	ushort	tvar;
 
	tvar = ((value & 0x01)?(SERIAL_DIN):(NO_SERIAL_DIN));
	ECAT_9346KernelWrite(bay,tvar | SERIAL_CS);
	ECAT_9346KernelWrite(bay,tvar | SERIAL_CS | SERIAL_CLK);
	ECAT_9346KernelWrite(bay,tvar | SERIAL_CS);
}

/*
 *      Routine:	ECAT_9346Command
 *       Module:	INTFACE.C
 * Date Created:	August 06, 1992
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sends a 9346 Serial EEPROM command to the chip
 *  Assumptions:	This is the lowest level 9346 kernel routine so it assumes
 *					that the passed bay value is valid (this may change in
 *					the future)
 *   Known Bugs:	None
 *   Parameters:	bay		- Indicates the bay (0 - 15)
 *					command	- The 9346 command to be clocked into the EEPROM
 *      Globals:	None
 *       Locals:	bitcnt	- Temporary variable to track bit counting
 *      Returns: None
 */
char	ECAT_9346Command(
uchar	bay,
ushort	command)
{
	char	bitcnt;

	for(bitcnt = 8; bitcnt >= 0; bitcnt--)
		ECAT_ClkWrite(bay,((command >> bitcnt) & 0x01));
}

/*
 *      Routine:	ECAT_9346Read
 *       Module:	INTFACE.C
 * Date Created:	August 06, 1992
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	This routine reads the 9346 Serial EEPROM and transfers
 *					a specified number of bytes into the passed data structure
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	bay		- Indicates the bay (0 - 15)
 *					data		- A pointer to the VOID data segment to fill
 *					size		- Size of the data structure to fill
 *      Globals:	None
 *       Locals:	bitcnt	- Temporary variable to track bit counting
 *					addrcnt	- Temporary variable to track address counting
 *					errval	- Temporary variable to hold error codes
 *					checksum	- Temporary variable for checksum computations
 *					dataptr	- An unsigned short pointer used for transferring
 *								  data into or out of the passed data structure
 *					tmpdata	- Temporary data structure for checksum purposes
 *					tmpptr	- Pointer to temporary storage area
 *      Returns:	 0	- Success
 *					-1	- Illegal bay choice
 *					-2 - Interface board does not exist
 *					-3	- Storage pointer is TOO large ( >126 bytes)
 *					-4	- Temporary storage allocation failed
 *					-5 - READ operation failed (CHECKSUM failure)
 *    Revisions: 01/14/93 - James Murphy (JMM)
 *					- The passed data type was changed to a VOID type to
 *					  allow any type of data to be read
 *					- A SIZE parameter has been added to support varying
 *					  lengths of data structures (126 bytes maxmimum)
 *					- The checksum is now maintained separately so there
 *					  is no need for the passed data to support one
 *					- The checksum is now a short instead of an int
 *					01/13/95 - James Murphy (JMM)
 *						The checksum has been inverted to keep the "OLD"
 *						serial EEPROM format from being used with the
 *						"NEW" format
 */
char	ECAT_9346Read2(
uchar	bay,
void		*data,
uchar	size,
uchar	dosum)
{
	char		bitcnt;
	char		errval;
	uchar		addrcnt;
	ushort	checksum;
	ushort	*dataptr;
	ushort	*tmpdata;
	ushort	*tmpptr;

	if(bay >= MAX_MODULES)
		return(-1);
	if(ECAT_InterfaceCheck(bay))
		return(-2);
	if(size > 126 && dosum)
		return(-3);
	if((tmpdata = (ushort *)malloc(sizeof(ushort) * SIZE_9346)) == NULL)
		return(-4);

	tmpptr = (ushort *)tmpdata;
	for(addrcnt = 0; addrcnt < SIZE_9346; addrcnt++)
	{
		ECAT_9346KernelWrite(bay,0x00);
		ECAT_9346KernelWrite(bay,SERIAL_CS);
		ECAT_9346Command(bay,(READ_9346|(addrcnt & 0x003F)));
		*tmpptr = 0;
		for(bitcnt = 0; bitcnt < BITS_9346; bitcnt++)
		{
			ECAT_9346KernelWrite(bay,SERIAL_CS);
			ECAT_9346KernelWrite(bay,SERIAL_CS | SERIAL_CLK);
			*tmpptr = (*tmpptr << 1) | ((ECAT_9346KernelRead(bay) & 0x40)?(1):(0));
		}
		tmpptr++;
	}
	ECAT_9346KernelWrite(bay,0x00);			/* Chip Deselect	*/

	/* Verify the CHECKSUM value */
	errval = 0;
	if(dosum)
	{
		checksum	= 0;
		tmpptr	= (ushort *)tmpdata;
		for(addrcnt = 0; addrcnt < (SIZE_9346-1); addrcnt++, tmpptr++)
			checksum += *tmpptr;
		checksum = ~(checksum);
		if(tmpdata[63] != checksum)
			errval = -5;
	}
	if(!errval)									/* Move bytes into USER data */
	{ 
		tmpptr = (ushort *)tmpdata;
		dataptr= (ushort *)data;
		size	/= sizeof(ushort);
		for(addrcnt = 0; addrcnt < size; addrcnt++)
			*dataptr++ = *tmpptr++;
	}
	free(tmpdata);
	return(errval);
}

/*
 *      Routine:	ECAT_9346Write
 *       Module:	INTFACE.C
 * Date Created:	August 06, 1992
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	This routine WRITES a number of bytes into the 9346 EEPROM
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	bay		- Indicates the bay (0 - 15)
 *					data		- A VOID pointer to the data segment to fill
 *					size		- Size of the data structure to fill
 *      Globals:	None
 *       Locals:	bitcnt	- Temporary variable to track bit counting
 *					addrcnt	- Temporary variable to track address counting
 *					errval	- Temporary variable to hold error codes
 *					checksum	- Temporary variable for checksum computations
 *					dataptr	- An unsigned short pointer used for transferring
 *								  data into or out of the passed data structure
 *					newval	- Temporary variable for 9346 codes
 *					tmpdata	- Temporary data structure for checksum purposes
 *					tmpptr	- Pointer to temporary storage area
 *      Returns:	 0	- Success
 *					-1	- Illegal bay choice
 *					-2 - Interface board does not exist
 *					-3	- Size parameter is TOO large ( >126 bytes)
 *					-4	- Temporary storage allocation failed
 *					-5	- ERASE operation failed
 *					-6 - WRITE operation failed (CHECKSUM failure)
 *    Revisions: 01/14/93 - James Murphy (JMM)
 *					- The passed data type was changed to a VOID type to
 *					  allow any type of data to be written
 *					- A SIZE parameter has been added to support varying
 *					  lengths of data structures (126 bytes maxmimum)
 *					- The function now ERASES the EEPROM for convenience
 *					- The checksum is now maintained separately so there
 *					  is no need for the passed data to support one
 *					- The checksum is now a short instead of an int
 *					01/13/95 - James Murphy (JMM)
 *						The checksum has been inverted to keep the "OLD"
 *						serial EEPROM format from being used with the
 *						"NEW" format
 */
char	ECAT_9346Write(
uchar	bay,
void		*data,
uchar	size)
{
	uchar		addrcnt;
	char		bitcnt;
	char		errval;
	ushort	*dataptr;
	ushort	*tmpdata;
	ushort	*tmpptr;
	ushort	newval;
	ushort	checksum;

	if(bay >= MAX_MODULES)
		return(-1);
	if(ECAT_InterfaceCheck(bay))
		return(-2);
	if(size > 126)
		return(-3);
	if((tmpdata = (ushort *)malloc(sizeof(ushort) * SIZE_9346)) == NULL)
		return(-4);
	if(ECAT_9346Erase(bay))
		return(-5);

	for(addrcnt = 0; addrcnt < SIZE_9346; addrcnt++)
		tmpdata[addrcnt] = 0;

	size	/= sizeof(ushort);
	dataptr= (ushort *)data;
	tmpptr = (ushort *)tmpdata;
	for(addrcnt = 0; addrcnt < size; addrcnt++)
		*tmpptr++ = *dataptr++;

	checksum	= 0;
	tmpptr	= (ushort *)tmpdata;
	for(addrcnt = 0; addrcnt < (SIZE_9346-1); addrcnt++, tmpptr++)
		checksum += *tmpptr;
	checksum = ~(checksum);
	tmpdata[63] = checksum;

	/* Enable programmability */
	ECAT_9346KernelWrite(bay,0x00);								/* KILL all Input */
	ECAT_9346KernelWrite(bay,SERIAL_CS);						/* Chip Select		*/
	ECAT_9346Command(bay,EWEN_9346);								/* Send EWEN cmd	*/
	ECAT_9346KernelWrite(bay,0x00);								/* Chip Deselect	*/

	tmpptr = tmpdata;
	for(addrcnt = 0; addrcnt < SIZE_9346; addrcnt++)
	{
		ECAT_9346KernelWrite(bay,0x00);							/* Kill All Input	*/
		ECAT_9346KernelWrite(bay,SERIAL_CS);					/* Chip Select		*/
		newval = WRTE_9346 | (addrcnt & 0x003F);				/* Fix WRTE_9346	*/
		ECAT_9346Command(bay,newval);								/* Send WRTE cmd	*/
		for(bitcnt = BITS_9346-1; bitcnt >= 0; bitcnt--)
			ECAT_ClkWrite(bay,((*tmpptr >> bitcnt) & 0x01));
		ECAT_9346KernelWrite(bay,0x00);							/* Chip Deselect	*/
		ECAT_9346KernelWrite(bay,SERIAL_CS);					/* Chip Select		*/
		while((ECAT_9346KernelRead(bay) & 0x40) != 0x40)
			;
		ECAT_9346KernelWrite(bay,0x00);							/* Chip Deselect	*/
		tmpptr++;
	}
	ECAT_9346KernelWrite(bay,0x00);								/* Chip Deselect	*/

	/* Disable Programmability */
	ECAT_9346KernelWrite(bay,SERIAL_CS);						/* Chip Select		*/
	ECAT_9346Command(bay,EWDS_9346);								/* Send EWDS cmd	*/
	ECAT_9346KernelWrite(bay,0x00);								/* Chip Deselect	*/

	errval = ECAT_9346Read(bay,tmpdata,126);					/* Verify data		*/
	free(tmpdata);
	return(errval);
}

/*
 *      Routine:	ECAT_9346Erase
 *       Module:	INTFACE.C
 * Date Created:	August 06, 1992
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	This routine erases the 9346 SERIAL EEPROM
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	bay		- Indicates the bay (0 - 15)
 *      Globals:	None
 *		 Locals:	addrcnt	- Temporary variable to track address counting
 *					errval	- Temporary variable to hold error codes
 *					tmpptr	- An unsigned short pointer used for verifying
 *								  the data in the 9346 SERIAL EEPROM
 *					tmpdata	- Temporary DATA9346 structure for verification
 *								  purposes
 *      Returns:	 0	- Success
 *					-1	- Illegal bay choice
 *					-2 - Interface board does not exist
 *					-3	- Temporary storage allocation failed
 *					-4 - ERASE operation failed
 */
char	ECAT_9346Erase(
uchar	bay)
{
	uchar		addrcnt;
	char		errval;
	ushort	*tmpptr;
	ushort	*tmpdata;

	if(bay >= MAX_MODULES)
		return(-1);
	if(ECAT_InterfaceCheck(bay))
		return(-2);
	if((tmpdata = (ushort *)malloc(sizeof(ushort) * SIZE_9346)) == NULL)
		return(-3);

	/* Enable programmability capability */
	ECAT_9346KernelWrite(bay,0x00);								/* KILL all Input */
	ECAT_9346KernelWrite(bay,SERIAL_CS);						/* Chip Select		*/
	ECAT_9346Command(bay,EWEN_9346);								/* Send EWEN cmd	*/
	ECAT_9346KernelWrite(bay,0x00);								/* Chip Deselect	*/

	/* Erase the whole EEPROM */
	ECAT_9346KernelWrite(bay,SERIAL_CS);						/* Chip Select		*/
	ECAT_9346Command(bay,ERAL_9346);								/* Send ERAL cmd	*/
	ECAT_9346KernelWrite(bay,0x00);								/* Chip Deselect	*/
	ECAT_9346KernelWrite(bay,SERIAL_CS);						/* Chip Select		*/
	while((ECAT_9346KernelRead(bay) & 0x40) != 0x40)
		;
	ECAT_9346KernelWrite(bay,0x00);								/* Chip Deselect	*/
	
	/* Disable Programmability */
	ECAT_9346KernelWrite(bay,SERIAL_CS);						/* Chip Select		*/
	ECAT_9346Command(bay,EWDS_9346);								/* Send EWDS cmd	*/
	ECAT_9346KernelWrite(bay,0x00);								/* Chip Deselect	*/

	/* Read the data back for verification */
	ECAT_9346Read2(bay,tmpdata,128,0);
	errval = 0;
	tmpptr = tmpdata;
	for(addrcnt = 0; addrcnt < SIZE_9346; addrcnt++, tmpptr++)
	{
		if(*tmpptr != 0xFFFF)
		{
			errval = -4;
			break;
		}
	}
	free(tmpdata);
	return(errval);
}

/*
 *      Routine:	ECAT_MAX180Read
 *       Module:	INTFACE.C
 * Date Created:	August 06, 1992
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	This routine reads the designated channel of the MAXIM
 *					ADC180 on the designated interface board.
 *  Assumptions:	This routine returns the RAW ADC data which has been
 *					adjusted so the 
 *   Known Bugs:	None
 *   Parameters:	bay		- Indicates the bay		(0 - 15)
 *					channel	- Indicates the channel	(0 - 7)
 *					mode		- Indicates operation	(UNIPOLAR (0) / BIPOLAR (1))
 *      Globals:	None
 *       Locals:	timecnt	- MAXIM 180 time out counter
 *      Returns:	 +	- Readback value
 *					-1	- Illegal bay choice
 *					-2	- Interface board does not exist
 *					-3	- Illegal channel number
 *					-4	- MAXIM 180 Busy line timed out
 *Modifications:
 *    Revisions: 01/15/93 - James Murphy (JMM)
 *						The BUSY line of the 180 chip was monitored for
 *						time outs to ensure this function will not hang-up.
 *						The return value for a time-out has been set for -4
 *					02/18/94 - James Murphy (JMM)
 *						The parameter list was changed to add the ability
 *						for UNIPOLAR or BIPOLAR operation, see parameter list
 *						above.
 */
short	ECAT_180Read(
uchar	bay,
uchar	channel,
uchar	mode)
{
	uchar	timecnt = 0;

	if(bay >= MAX_MODULES)
		return(-1);
	if(ECAT_InterfaceCheck(bay))
		return(-2);
	if(channel > 7)
		return(-3);

	ECAT_MAX180Write(bay,(channel | MAX180_SINGLE | ((mode)?(MAX180_BIPP):(MAX180_UNIP))));
	while(!(ECAT_9346KernelRead(bay) & MAX180_BUSY) && (timecnt <= 100))
		timecnt++;

	if(timecnt > 100)
		return(-4);
	return((((short)(ECAT_MAX180Read(bay))) & 0x0FFF));
}

/*
 *      Routine:	ECAT_HVProgram
 *       Module:	INTFACE.C
 * Date Created:	October 01, 1993
 *       Author:	Paul Breed
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	This routine is responsible for setting the 12-bit
 *					DAC on the microprocessor board.
 *  Assumptions:	If the DAC value OVERFLOWS - val > 4095; then the DAC
 *					will be set to the maximum of 4095.  If the DAC value
 *					UNDERFLOWS - val < 0; then the DAC value will be set
 *					to zero (0). Also, the HV_ENABLE line is controlled
 *					by the DAC value; a ZERO (0) value will DISABLE the
 *					line while any other value will ENABLE the line.
 *   Known Bugs:	None
 *   Parameters:	[ ] voltage				- Voltage to program HV supply to
 *      Globals:	[ ] interlockData		- Used to read the peak dip-switches
 *					[X] ldo_state			- Used to control the HV_ENABLE line
 *       Locals:	dacval	- Temporary variable to hold the DAC value
 *					errval	- Temporary variable to hold error codes
 *      Returns:	0	- Success
 *					-3	- DAC Overflow
 *    Revisions: 12/15/94 - James Murphy (JMM)
 *						The calculation of the 12-bit DAC value has been
 *						reverted back to an integer calculation.  All floating
 *						point references are gone.  The HV enable control has
 *						been moved into this function and the return error
 *						codes have been added.  Also, the HV enable control
 *						has been added
 *					02/10/95 - James Murphy (JMM)
 *						A parameter has been added yada,yada,yada
 */
char	ECAT_HVProgram(
int		voltage,
uchar	enableFlag)
{
	uint	dacval;
	char	errval;
/*
 *	There are now 3 versions of peak boards floating around with
 *	varying gain factors for the high voltage power supply
 *	The gain is now controlled via the SW1 dip-switch located on the
 *	front of the peak detector board.  Setting the dip-switch as follows
 *	causes the voltage to be translated for the referenced gains
 *
 *			8 7 6 5 4 3 2 1		Gain
 *
 *			0 0 0 0 0 0 0 0		1.00	(DEFAULT)
 *			1 0 0 0 0 0 0 0		1.10
 *			0 1 0 0 0 0 0 0		1.15
 */
	if((interlockData & 0x8000))
		dacval = (uint)(abs((voltage * 4095) / 11000));
	else
		if((interlockData & 0x4000))
			dacval = (uint)(abs((voltage * 4095) / 11500));
		else
			dacval = (uint)(abs((voltage * 4095) / 10000));

	errval = 0;
	if(dacval > 4095)
	{
		dacval = 4095;
		errval = -3;
	}
/*
 *	The following sets the 12-bit DAC value and then SETS or CLEARS the
 *	HV_ENABLE line on the peak detector.  The line is DISABLED if the
 *	desired value is ZERO (0) so the HV supply is truly SHUT DOWN.  Any
 *	other value will enable the line and allow the HV supply to TURN ON.
 */
	DAC_set12(dacval);						/* Set 12-bit DAC Value	*/
	if((dacval = 0) || (!enableFlag))
		ldo_state |= 0x0101;					/* DISABLE  HV Supply	*/
	else
		ldo_state &= ~(0x0101);				/* ENABLE HV Supply		*/
	PEAK_LD5 = ldo_state;
	return(errval);
}
/******* << END INTFACE.C >> *******/
