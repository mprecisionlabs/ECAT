#ifndef __GPIB_H				/* Avoid multiple inclusions */
#define __GPIB_H

/*
 * Other includes
 */

#include <stdio.h>
#include <stdlib.h>
#include <shortdef.h>
#include "buffer.h"

/*
 * General definitions
 */

#define	MAV					16
#define	ESB					32
#define	SRQ					64
#define	gpibin				(&_iob[5])
#define	gpibout				(&_iob[4])
#define	GPIB_BUFFER_SIZE	255

typedef	void	(*PF)(void);

typedef struct	gpib_buf_type
{
	BUF_TYPE	*gpib_buf;
	struct gpib_buf_type	*next;
} GPIB_BUF;


/*
 *	PRIVATE function prototypes
 *
 *		gpib_interrupt -- Interrupt handler for the GPIB subsystem
 */

void interrupt	gpib_interrupt();

/*
 *  PUBLIC function prototypes
 *
 *		gpib_init			-- Initializes the GPIB subsystem
 *		gpib_send_buffer	-- Sends a buffer on the GPIB bus
 *		gpib_get_buffer	-- Gets a buffer from the GPIB bus
 *		gpib_send_srq		-- Sends a request for service to the controller
 *		gpib_file_init		-- Initilaizes the GPIB file pointers
 *		gpib_get_char		-- Gets a char from the GPIB buffer
 *		gpib_write			-- Writes a char to the GPIB buffer
 *		gpib_char_avail	-- Sees if an incoming char is available on the GPIB
 *		gpib_set_status	-- Sets a bit in the serial poll byte (SPB)
 *		gpib_clr_status	-- Clears a bit in the serial poll byte (SPB)
 */

BUF_TYPE	*gpib_get_buffer(void);
void	gpib_init(int,void (*resetp)(),void (*go_remote)(),void (*go_local)(),
					 void (*grp_trg)(),void (*spoll)());
void	gpib_send_buffer(BUF_TYPE *);
void	gpib_send_srq(uchar);
void	gpib_file_init(void);
char	gpib_get_char(void);
void	gpib_write(char);
int	gpib_char_avail(void);
int	fcavail(FILE *);
void	gpib_set_status(uchar);
void	gpib_clr_status(uchar);

#endif							/* ifndef __GPIB_H */
