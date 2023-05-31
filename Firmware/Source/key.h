#ifndef __KEY_H				/* Avoid multiple inclusions */
#define __KEY_H

#include  <shortdef.h>

extern int	khead;			/* Pointer into the keyboard buffer	*/
extern int	ktail;			/* Pointer into the keyboard buffer	*/

/*
 *  General definitions
 */

#define	key					(&_iob[6])		/* FILE pointer for keyboard		*/
#define	MAX_KEYBUFF			20					/* Maximum keyboard buffer			*/
#define	BEEP_DELAY			209715			/* Sound interval in TPU ticks	*/
#define	AUDIO_DAC			2					/* Micro DAC channel for speaker	*/


/*
 *	Macro Function Definitions
 *	The prototypes for the following are:
 *
 *		int	KEY_hit(void);
 *		int	KEY_flush(void);
 */

#define	KEY_flush()			(khead = ktail)
#define	KEY_hit()			(khead != ktail)

/*
 *  PRIVATE function prototypes
 *
 *		KEY_trap	--	Interrupt handler for keyboard subsystem
 */

void interrupt	KEY_trap(void);

/*
 *  PUBLIC function prototypes
 *
 *		KEY_reset		-- Resets/initializes the keyboard controller
 *		KEY_beep			-- Sound a beep through the speaker
 *		KEY_led			-- Turns a front panel LED ON or OFF
 *		KEY_ledstate	-- Returns the state of a front panel LED
 *		KEY_getraw		-- Returns an unconverted key code
 *		KEY_getchar		-- Returns a converted key code
 *		KEY_wait			-- Wait for a key press and flush the key buffer
 */

void	KEY_reset(void);
void	KEY_beep(void);
void	KEY_wait(void);
void	KEY_led(int, uchar);
char	KEY_getchar(void);
int	KEY_getraw(void);
int	KEY_ledstate(int);

#endif							/* ifndef __KEY_H */
