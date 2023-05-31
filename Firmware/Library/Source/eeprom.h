#ifndef __EEPROM_H			/* Avoid multiple inclusions */
#define __EEPROM_H

/*
 *  General definitions
 */

#define	EEPROMSIZE			8192
#define	MAXEEPROM			8191				/* EEPROM size - 1 */

/*
 * PUBLIC function prototypes
 *
 *		write_memory	-- Store a block of data to the EEPROM
 *		read_memory		-- Retrieve a block of data from the EEPROM
 */

int	write_memory(int, void *,int );
int	read_memory(int, void *, int);

#endif							/* ifndef __EEPROM_H */
