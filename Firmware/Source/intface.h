#ifndef __INTFACE_H						/* Avoid multiple inclusions */
#define __INTFACE_H

#include <shortdef.h>

#define	MAX_MODULES			(16)

#define	RELAY_BANK_0		(0x060000)
#define	RELAY_BANK_1		(0x060100)
#define	TIMER_SEL_REG		(0x060200)
#define	TIMER_SEQUENCER	(0x060300)
#define	DIP_SWITCH			(0x060400)
#define	ADC_CONTROL			(0x060500)
#define	SERIAL_LATCH		(0x060600)
#define DIN_REGISTER		(0x060600)
#define	EXP_CONNECTOR		(0x060700)
#define	EXP_RELAY0			(0x060700)
#define	EXP_RELAY1			(0x060720)

#define	MAX180_BUSY			(0x0020)			/* Busy Bits in Digital Latch		*/
#define	MAX180_BIPP			(0x0008)			/* Bipolar or Unipolar				*/
#define	MAX180_UNIP			(0x0000)
#define	MAX180_SINGLE		(0x0000)			/* Single-Ended or Differential	*/
#define	MAX180_DIFF			(0x0010)

#define	SERIAL_CS			(0x0002)
#define	SERIAL_CLK			(0x0004)
#define	SERIAL_DIN			(0x0008)
#define NO_SERIAL_DIN		(0x0000)

#define	READ_9346			(0x0180)			/* 9346 Serial EE2PROM codes	*/
#define	WRTE_9346			(0x0140)
#define	EWEN_9346			(0x0130)
#define	EWDS_9346			(0x0100)
#define	ERAS_9346			(0x01C0)
#define	ERAL_9346			(0x0120)
#define	WRAL_9346			(0x0110)

#define SIZE_9346			(64)				/* 9346 Serial EE2PROM sizes	*/
#define	BITS_9346			(16)

#define	WRAP_BIT_READ		(0x80)			/* Interface checking bits		*/
#define	WRAP_BIT_WRITE		(0x01)

#define	LCLWR1_ADDR			(0x048000)
#define	FLSH_Reset()		((*(uchar *)LCLWR1_ADDR) = 0x00)
#define	FLSH_Write()		((*(uchar *)LCLWR1_ADDR) = 0x03)

/*
 * MACRO function definitions
 * The prototypes for the following are:
 *
 *	uchar		ECAT_DipSwitch(uchar bay)
 *	ushort	ECAT_MAX180Read(uchar bay)
 *	ushort	ECAT_MAX180ReadUni(uchar bay)
 *	void		ECAT_MAX180Write(uchar bay, ushort data)
 *	void		ECAT_WriteRelay0(uchar bay, ushort data)
 *	void		ECAT_WriteRelay1(uchar bay, ushort data)
 *	void		ECAT_TimerSelReg(uchar bay, ushort data)
 *
 * Function usage:
 *
 *	ECAT_WriteRelay0		-- Writes to Relay Drivers 00 - 15
 *	ECAT_WriteRelay1		-- Writes to Relay Drivers 16 - 23
 *	ECAT_TimerSelReg		-- Writes setup data to the interface TIMER controller
 *	ECAT_DipSwitch			-- Reads the dip switch on an interface board
 *	ECAT_MAX180Read		-- Reads data from a MAXIM 180 ADC
 *	ECAT_MAX180Write		-- Writes setup data to the MAXIM 180 ADC
 */

#define	ECAT_WriteRelay0(b,x)		((*(ushort *)((RELAY_BANK_0	+(b*0x1000)))=x))
#define	ECAT_WriteRelay1(b,x)		((*(ushort *)((RELAY_BANK_1	+(b*0x1000)))=(x&0x00FF)))
#define	ECAT_TimerSelReg(b,x)		((*(ushort *)((TIMER_SEL_REG	+(b*0x1000)))=(x&0x00FF)))
#define	ECAT_TimerSequencer(b,x)	((*(ushort *)((TIMER_SEQUENCER+(b*0x1000)))=(x&0xFFFF)))
#define	ECAT_MAX180Write(b,x)		((*(ushort *)((ADC_CONTROL		+(b*0x1000)))=x))
#define	ECAT_MAX180Read(b)			(*(ushort  *)((ADC_CONTROL		+(b*0x1000))))
#define	ECAT_MAX180ReadUni(b)   	(*(ushort  *)((ADC_CONTROL		+(b*0x1000))))
#define	ECAT_WriteExpansion(b,x)	((*(ushort *)((EXP_CONNECTOR	+(b*0x1000)))=x))
#define	ECAT_ReadExpansion(b)		(*(ushort  *)((EXP_CONNECTOR	+(b*0x1000))))
#define	ECAT_DipSwitch(b)				(*(uchar   *)((DIP_SWITCH		+(b*0x1000))))
#define	ECAT_9346KernelRead(b)		(*(ushort  *)((SERIAL_LATCH	+(b*0x1000))))
#define	ECAT_9346KernelWrite(b,x)	((*(ushort *)((SERIAL_LATCH	+(b*0x1000)))=x))
#define	ECAT_9346Read(b,d,s)			ECAT_9346Read2(b,d,s,1)
#define	ECAT_ReadDINByte(b)			(((*(ushort*)(DIN_REGISTER		+(b*0x1000)))>>8)&0x00FF)
#define	ECAT_WriteEXPRelay0(b,x)	((*(ushort *)((EXP_RELAY0		+(b*0x1000)))=x))
#define	ECAT_WriteEXPRelay1(b,x)	((*(ushort *)((EXP_RELAY1		+(b*0x1000)))=(x&0x00FF)))

/*
 * Private Functions
 *		ECAT_ClkWrite		-- Clocks a single data BIT into the 9346 EEPROM
 *		ECAT_9346Command	-- Sends a COMMAND sequence to the 9346 EEPROM
 */

void	ECAT_ClkWrite(uchar,ushort);
char	ECAT_9346Command(uchar,ushort);

/*
 * Public Functions
 *		ECAT_InterfaceCheck	-- Checks the existance of an interface card
 *		ECAT_9346Erase			-- Erases the 9346 EEPROM in a module
 *		ECAT_9346Write			-- Writes data to the 9346 EEPROM in a module
 *		ECAT_9346Read			-- Reads data from the 9346 EEPROM in a module
 *		ECAT_180Read			-- Reads data from the MAXIM 180 in a module
 *		ECAT_HVProgram			-- Programs the 12-bit DAC on the microboard
 */

char		ECAT_InterfaceCheck(uchar);
char		ECAT_9346Erase(uchar);
char		ECAT_9346Write(uchar, void *, uchar);
char		ECAT_9346Read2(uchar, void *, uchar, uchar);
short	ECAT_180Read(uchar,uchar,uchar);
char		ECAT_HVProgram(int,uchar);

#endif										/* ifndef __INTFACE_H */
