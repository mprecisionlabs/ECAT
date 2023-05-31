#ifndef __BURST_H				/* Avoid multiple inclusions */
#define __BURST_H

#include <shortdef.h>

#define	BURST_CHANNEL		0
#define	LED_CHANNEL			1
#define	PHASE_CHANNEL		5
#define	SCRAP_CHANNEL		7
#define	TPU_TICK				477E-9
#define	EXTRA_TICKS			(0.015629859)

#define	OFF		0
#define	ON			1
#define	CHIRP		2
#define	MOM		3

#define	TRG_STD				((uchar)1)				/* Burst mode	*/
#define	TRG_DEG				((uchar)2)
#define	TRG_CONT				((uchar)1)				/* Burst types	*/
#define	TRG_MOM				((uchar)2)

typedef	struct
{
	uchar	state;
	int	channel;
	int	on_time;
	int	off_time;
	int	period;
	int	phase;
	uchar	mode;
	uchar	type;
} PRIV_BURST;
extern volatile uchar BURST_ENDED;

int	one_burst(float, float, float, uchar, uint);
int	start_burst(float, float, float, uchar, uint, uchar);
void	update_burst(float, float, float, uint, uchar);
void	stop_burst(void);
void	doburst(void);
void	killLED(void);
void	start_phase(void);

#endif							/* ifndef __BURST_H */

