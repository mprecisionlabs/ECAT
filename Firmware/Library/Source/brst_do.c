#include <float.h>
#include "burst.h"
#include "timer.h"
#include "switches.h"

extern volatile uchar	AD_HIT;

volatile PRIV_BURST	priv_burst[2];
volatile char			priv_switch;
volatile uint			Redo_on_time;
volatile uchar		BURST_ENDED;

void	killLED(void)
{
	TPU_write(LED_CHANNEL,0);
}

void redo_burst(void)
{
	TPU_one_shot(12,Redo_on_time-786,1,doburst);
}


void	start_phase(void)
{
	PRIV_BURST	*pb_ptr;

	pb_ptr = &(priv_burst[priv_switch]);
	if(pb_ptr->on_time > 32000)
	{
		Redo_on_time = pb_ptr->on_time >> 1;
		TPU_one_shot(12,Redo_on_time,1,redo_burst);
	}
	else
		TPU_one_shot(12,pb_ptr->on_time,1,doburst);
	TPU_set_pwm(BURST_CHANNEL,pb_ptr->period,(pb_ptr->period >>1));
	TPU_write(LED_CHANNEL,1);
	TPU_delay_int(41928,killLED);
}

void  doburst(void)
{
	PRIV_BURST	*pb_ptr;

	pb_ptr = &(priv_burst[priv_switch]);
	if(pb_ptr->state)
	{	/* We are shutting down the burst */
		AD_HIT = 0;
		BURST_ENDED = 1;
	}
	if(pb_ptr->type == TRG_STD)
	{
		if(pb_ptr->mode == TRG_CONT)
		{
			if(pb_ptr->state)
			{
				pb_ptr->state = 0;
				TPU_write(BURST_CHANNEL,0);
				pb_ptr->channel = TPU_delay_int(pb_ptr->off_time,doburst);
			}
			else
			{
				pb_ptr->state = 1;
				if(pb_ptr->on_time > 32000)
				{
					Redo_on_time = pb_ptr->on_time >> 1;
					TPU_one_shot(12,Redo_on_time,1,redo_burst);
				}
				else
					TPU_one_shot(12,pb_ptr->on_time,1,doburst);
				TPU_set_pwm(BURST_CHANNEL,pb_ptr->period,(pb_ptr->period >>1));
/*				pb_ptr->channel = TPU_delay_int(pb_ptr->on_time,doburst);*/
				TPU_write(LED_CHANNEL,1);
				TPU_delay_int(41928,killLED);
			}
		}
		else
		{
			if(pb_ptr->mode == TRG_MOM)
			{
				pb_ptr->mode = OFF;
				if(pb_ptr->on_time > 32000)
				{
					Redo_on_time = pb_ptr->on_time >> 1;
					TPU_one_shot(12,Redo_on_time,1,redo_burst);
				}
				else
					TPU_one_shot(12,pb_ptr->on_time,1,doburst);
				TPU_set_pwm(BURST_CHANNEL,pb_ptr->period,(pb_ptr->period >>1));
/*				pb_ptr->channel = TPU_delay_int(pb_ptr->on_time,doburst); */
				TPU_write(LED_CHANNEL,1);
				TPU_delay_int(41928,killLED);
			}
			else
			{
				TPU_kill(pb_ptr->channel);
				pb_ptr->mode	= OFF;
				pb_ptr->state	= 0;
				TPU_write(BURST_CHANNEL,0);
			}
		}
	}
	else
		if(pb_ptr->type == TRG_DEG)
		{
			if(pb_ptr->mode == TRG_CONT)
			{
				if(pb_ptr->state)
				{
					pb_ptr->state = 0;
					TPU_write(BURST_CHANNEL,0);
					pb_ptr->channel = TPU_delay_int(pb_ptr->off_time,doburst);
				}
				else
				{
					pb_ptr->state = 1;
					TPU_phase_trigger(PHASE_CHANNEL,SCRAP_CHANNEL,1,6291,pb_ptr->phase,2,start_phase);
				}
			}
			else
			{
				if(pb_ptr->mode == TRG_MOM)
				{
					pb_ptr->mode = OFF;
					TPU_phase_trigger(PHASE_CHANNEL,SCRAP_CHANNEL,1,6291,pb_ptr->phase,2,start_phase);
				}
				else
				{
					TPU_kill(pb_ptr->channel);
					pb_ptr->mode	= OFF;
					pb_ptr->state	= 0;
					TPU_write(BURST_CHANNEL,0);
				}
			}
		}
}

