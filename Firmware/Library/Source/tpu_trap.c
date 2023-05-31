#include "timer.h"

extern volatile ushort	TCR1_READ;
extern volatile ushort	TPU_rams[16][8];
extern volatile uint	TPU_rami[16][4];
extern volatile ulong	param1[];
extern volatile ulong	param2[];
extern volatile ulong	param3[];

extern volatile uchar	tpuFlag[];
extern volatile uchar	happened[];
extern TPU_STRUCT		*TPU;					/* This is VOLATILE */

void	process_Surge_Sync(uchar ch);
void	process_Surge_Help(uchar ch);
void	process_Surge_Out(uchar ch);
void	process_list_Sync(uchar ch);
void	process_list_Help(uchar ch);
void	process_list_Out(uchar ch);
void	process_Sync_Channel(uchar ch);


/*
 *  This is the interrupt location for all of the TPU interrupts
 *  It does different things depending on the value of the mode
 *  set for each channel.
 */

interrupt void	TPU_trap(void)
{
	volatile ushort	i;
	volatile ushort	j;
	volatile uchar		n;
	volatile long		l;

	asm(" movem.l a0-a7/d0-d7,-(sp)");
	i = TPU->CISR;
	while(i)
	{
		j = 1;
		for(n = 0; (((j & i) == 0) && (n < 16)); n++)
			j *= 2;
		if((n != 16) && (tpuFlag[(char)n]))
		{
			switch(mode[n])
			{
				case ONE_SHOT:
					((PF)(param1[n]))();
					break;
				case SURGE_SYNC:
					process_Surge_Sync(n);
					break;
				case SURGE_HELPER:
					process_Surge_Help(n);
					break;
				case SURGE_OUTPUT:
					process_Surge_Out(n);
					break;
				case ECAT_FREQ:
					process_Ecat_Freq(n);
					break;
				case SYNC_CHANNEL:
					process_Sync_Channel(n);
					break;
				case LIST_SYNC:
					process_list_Sync(n);
					break;
				case LIST_HELPER:
					process_list_Help(n);
					break;
				case LIST_OUT:
					process_list_Out(n);
					break;
				case COUNT:
					param1[n]++;							/* The Transition count		*/
					break;
				case FREQ:
					param1[n]  = TPU_rams[n][1];		/* Number of transitions	*/
					param2[n]  = TPU_rami[n][2];		/* Accumulated value			*/
					param2[n] &= 0x00ffffff;			/* Keep only 24 valid bits	*/
					TPU_rams[n][4]=0xff00;
					happened[n] = 1;
					break;
				case TRIGSYNC:
					if(param2[n] & 0x10000)				/* First time */
					{
						param2[n] = TPU_rams[n][4];
						break;
					}
					else
					{												/* Not the first time */
						l = (TPU_rams[n][4]-(ushort)param2[n]);
						if(l < 0)
							l += 0x10000;
						if(l <= 25157)							/* Avoid noisy transitions */
							break;
						l = (l * (long)param1[n]) / 360;
						param2[n] = TPU_rams[n][4];		/* Time is now set up */
						param1[param3[n]] = (l / 2);   
						TPU_rams[param3[n]][1] = (l - (l / 2));
						l = param3[n];
						mode[l] = LONGTRIG;
						TPU_rams[l][0] = 0x083;
						TPU_rams[l][2] = (n * 16) + 0x08;
						if(l >= 8)
							TPU->HSSRR[0] |= (1 << ((l-8)*2));
						else
							TPU->HSSRR[1] |= (1 << (l*2));
						if(param2[l])
						{
							param2[l]--;
							if(!(param2[l]))
							{
								TPU_kill(n);				/* Kill it if finished	*/
								TPU_kill(l);				/* Kill helper ch			*/
								TPU_kill(happened[n]);	/* Kill output channel	*/
							}
						}
					}
					break;
				case TRIGOUTP:			/* The beginning of the triggered pulse	*/
					TPU_rams[n][2] = (n * 16) + 0x0A;	/* Address to compare	*/
					TPU_rams[n][1] = (ushort)param1[n];	/* Time						*/
					TPU_rams[n][0] = 0x8F;					/* Toggle each event		*/
					mode[n] = TRIGOUTM;						/* Reserve channel		*/
					if(n >= 8)
						TPU->HSSRR[0] |= (1 << ((n-8)*2));
					else
						TPU->HSSRR[1] |= (1 << (n*2));
					if(param3[n])
						((PF)(param3[n]))();
					break;
				case TRIGOUTM:
				case TRIGOUTS:
					break;
				case LONGTRIG:
					l=happened[n];
					if(happened[l])
						TPU_rams[l][0] = 0x87;					/* 0100 001 11 */
					else
						TPU_rams[l][0] = 0x8B;					/* 0100 010 11 */
					TPU_rams[l][1] = param1[n];
					TPU_rams[l][2] = (n * 16 + 0x0A);
					mode[l] = TRIGOUTP;
					if(l >= 8)
						TPU->HSSRR[0] |= (1 << ((l-8)*2));
					else
						TPU->HSSRR[1]|=(1 << (l*2));
					break;
				case INT_GEN:
					if(TPU_rams[n][1] & 0x8000)
						happened[n] = 1;
					else
						happened[n] = 0;
					((PF)(param1[n]))();
					break;
    			case REPEAT_INT_GEN:
					/* param1 = pointer to function					*/
					/* param2 = interval value							*/
					/* param3 = time remaining to next interval	*/
					l = param3[n];					/* save for later	*/
					if(!l) 
						param3[n] = param2[n];
					if(param3[n] >= 60000)
					{
						param3[n] -= 32767;
						TPU_rams[n][1]=32767;
					}
					else
						if(param3[n] >= 32767)
						{
							TPU_rams[n][1] = param3[n] / 2;
							param3[n] = param3[n] - TPU_rams[n][1];
						}
						else
						{
							TPU_rams[n][1] = param3[n];
							param3[n] = 0;
						}
					TPU_rams[n][2] = (n*16+0x0A);		  /* set reference address */
					if(n >= 8)
						TPU->HSSRR[0] |= (1<<((n-8)*2));
					else
						TPU->HSSRR[1]|=(1<<(n*2));
					if(!l)
						((PF)(param1[n]))(); 
					break;

				case DELAY_INT:
					if(param2[n])
						param2[n]--;
					else
					{
						TPU_kill(n);
						((PF)(param1[n]))();
					}
					break;

				case REPEAT_DELAYINT:
					if(param2[n])
						param2[n]--;
					else
					{
						TPU_channel_init(n,0x00,0,0,0,0);
						((PF)(param1[n]))();
					}
					break;

				case CATCH:
					param1[n] = TCR1_READ;
					mode[n] = NONE;
					break;
				case DELAY:
					mode[n] = CATCH;
					break;
			} /* switch */
		} /* End if */
		TPU->CISR = TPU->CISR & ~j;
		i = TPU->CISR;
	}
	asm(" movem.l (sp)+,a0-a7/d0-d7");
}
