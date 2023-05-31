#ifndef __QSM_H				/* Avoid multiple inclusions */
#define __QSM_H

#include  <shortdef.h>

/*
 * PRIVATE function prototypes
 *
 *		QSM_TRAP -- interrupt routine to handle QSM subsystem
 */

interrupt void	QSM_trap(void);

/*
 * PUBLIC function prototypes
 *
 *		QSM_init		-- Initializes the queued serial I/O subsystem
 *		QSM_halt		-- Stops the serial I/O subsystem
 *		DAC_set		-- Sets the value of one of the 8-bit DACs
 *		DAC_read		-- Gets the last recorded A/D value
 *		DAC_avg		-- Gets the average value of an A/D channel
 *		DAC_rms		-- Gets the RMS value of an A/D channel
 *		DAC_reset	-- Resets the MAX and MIN values of an A/D channel
 *		DAC_max		-- Gets the maximum A/D value recorded on a channel
 *		DAC_min		-- Gets the minimum A/D value recorded on a channel
 *		DAC_set12	-- Sets the value of one of the 12-bit DACs
 *		DAC_start_calc -- Starts extended calculations on a A/D channel
 */

void	QSM_init(void);
void	QSM_halt(void);
void	DAC_set(int ch,int value);
void	DAC_set12(int value);
void	DAC_start_calc(int ch, int counts);
void	DAC_reset(int ch);
int	DAC_read(int ch);
int	DAC_max(int ch);
int	DAC_min(int ch);
float	DAC_avg(int ch);
float	DAC_rms(int ch);
extern volatile uchar AD_HIT;

#endif							/* ifndef __QSM_H */

