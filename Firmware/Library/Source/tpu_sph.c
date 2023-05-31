#include "timer.h"
#include <stdio.h>

extern volatile ushort	TPU_rams[16][8];
extern volatile ulong	param1[];
extern volatile ulong	param2[];
extern volatile ulong	param3[];
extern volatile uchar	happened[16];
extern volatile int		SYNC_RISING_EDGE;
extern volatile TPU_MODE	prev_mode[16];

long	SURGE_TIMES[4];
int	done_with_pulse;

/*
Initialize()
Set TPU out to the desired B4 event value;
Set the Mode of the Time Sync Channel to SURGE_SYNC
Start Input Capture on Time Sync channel for both edges.
set Happened[SURGE_SYNC] =0;
    param1[SURGE_SYNC] = Phase in Degrees;
    param2[SURGE_SYNC] = Time Delay in TPU counts
    param3[SURGE_SYNC] HIWORD = Helper Channel
    param3[SURGE_SYNC] LOWORD = Output Channel 
    param1[SURGE_HELPER]=1 for low to Hi  and 0 for Hi to Low
    param2[SURGE_HELPER]=Delay for real pulse 
    param3[SURGE_OUTPUT]= (long)function();
    param1[SURGE_OUTPUT]=sync_ch;
    param2[SURGE_OUTPUT]=helper_ch;
return; 
 */

void  TPU_ECAT_phase(char sync_ch,char out_ch,int phase, long predelay,uchar slope,void (*p)())
{ int helper_ch;
  int i;
  done_with_pulse=0;
do
 for(helper_ch = 15; ((mode[helper_ch]!=NONE) && (helper_ch > MAX_TPU_USED)); helper_ch--)
			;
	while(mode[helper_ch]!=NONE);
if (slope) TPU_write(out_ch,0);
 else TPU_write(out_ch,1);

 prev_mode[sync_ch] = mode[sync_ch];
 mode[helper_ch]    = SURGE_HELPER;
 param1[sync_ch]    = (phase % 360);
 param2[sync_ch]    = predelay;
 param3[sync_ch]    = (helper_ch*0x10000) | out_ch;
 param1[helper_ch]  = slope;
 param3[out_ch]     = (long)p;
 param3[helper_ch]  = out_ch;

 happened[sync_ch] = 0;
 mode[sync_ch]     = SURGE_SYNC;

 if (prev_mode[sync_ch] != SYNC_CHANNEL)
	{
	 /*Now to set up the input channel to be input capture on both edges! */
	 TPU_rams[sync_ch][0]=0x07; /*Detect rising edge */
	 TPU_rams[sync_ch][1]=0x0E; /*Start link and count =0, Bank - 0E (unused) */
	 TPU_rams[sync_ch][2]=0x01; /*Count 1 Transition */
   	                     /*fun  HSR HSQ PRIO INT */
	 TPU_channel_init(sync_ch,0x0A, 1 , 1 , 3 , 1);
	}
 return;
}

#define SAFETIME 0x1000
void process_Surge_Sync(uchar ch)
{
   long P;
   long PT;
   uchar helper_ch;
   uchar out_ch;

   static long ADT;


if (happened[ch]<4)
  {
   SURGE_TIMES[happened[ch]]=TPU_rams[ch][4];
   happened[ch]++;
  }
switch (happened[ch])
 { case 1: TPU_rams[ch][0]=0x0F; /*Detect either edge */
                             /*fun  HSR HSQ PRIO INT */
           TPU_channel_init(ch,0x0A, 1 , 1 , 3 , 1);
   case 3:
   case 5: 
	        SYNC_RISING_EDGE = 1;		  				/* rising edge flag 	*/
           return;
   case 2: if (SURGE_TIMES[0]>SURGE_TIMES[1]) SURGE_TIMES[1]+=0x10000;
           SURGE_TIMES[0]=(SURGE_TIMES[1]+SURGE_TIMES[0])/2;
         /*Surge time [0] now has the zero # 1 Time! */
           return;
   case 4: if (SURGE_TIMES[2]>SURGE_TIMES[3]) SURGE_TIMES[3]+=0x10000;
           SURGE_TIMES[2]=(SURGE_TIMES[3]+SURGE_TIMES[2])/2;
         /*Surge time [2] now has the zero # 2 Time! */
           while  (SURGE_TIMES[2]<SURGE_TIMES[0]) SURGE_TIMES[2]+=0x10000;
           while  (SURGE_TIMES[3]<SURGE_TIMES[2]) SURGE_TIMES[3]+=0x10000;

           P   = SURGE_TIMES[2]-SURGE_TIMES[0];		  /* period  */
           PT  = param1[ch]*P/360;						  /* angle   */
           ADT = PT-param2[ch];							  /* delay   */
           while (ADT < (SURGE_TIMES[3]-SURGE_TIMES[2]+ SAFETIME) ) ADT+=P;
           ADT = ADT + SURGE_TIMES[2]-SURGE_TIMES[3];	  /* ADT -= 90 deg */
           
           helper_ch=((param3[ch]>>16) & 255);
           out_ch   =param3[ch] & 255;
           param3[helper_ch]=out_ch;
           param2[helper_ch]=ADT/2;	  	  		/* half of ADT to out    ch. */
           ADT=ADT-(ADT/2);				  		/* half of ADT to helper ch. */
           /*ADT now has the time from the last transition to the next SURGE_HELPER event */
           /*Schedule Surge helper to happen at the next event */
           /*Use the OC function */

           TPU_rams[helper_ch][0]=0x8C;
           TPU_rams[helper_ch][1]=ADT;
           TPU_rams[helper_ch][2]=ch*0x10+0x08;/*Address of last tran in  ch */
           TPU_rams[helper_ch][3]=0;

                                     /*fun  HSR HSQ PRIO INT */
           TPU_channel_init(helper_ch,0x0E, 1 , 0 , 3 , 1);

           if (prev_mode[ch] != SYNC_CHANNEL)
              TPU_kill(ch);
			  else
				  {
				  mode[ch] = SYNC_CHANNEL;
      		  TPU_rams[ch][0]=0x07; /*Detect rising edge */
                                 /*fun  HSR HSQ PRIO INT */
              TPU_channel_init(ch,0x0A, 1 , 1 , 3 , 1);
				  }
           return;
 }/*End of case */ 
}

void process_Surge_Help(uchar ch)
{
uchar out_ch;
int dlytime;
int slope;

/*Schedule SURGE_OUTPUT to happen at SURGE_HELPER+param2[ch];*/
out_ch=param3[ch];
slope=param1[ch];
dlytime=param2[ch];

if (slope) TPU_rams[out_ch][0]=0x86;
else TPU_rams[out_ch][0]=0x89;
TPU_rams[out_ch][1]=dlytime;
TPU_rams[out_ch][2]=ch*0x10+0x0A;/*Address of last tran in  ch */
TPU_rams[out_ch][3]=0;
mode[out_ch]=   SURGE_OUTPUT;
                          /*fun  HSR HSQ PRIO INT */
TPU_channel_init(out_ch,0x0E, 1 , 0 , 3 , 1);
TPU_kill(ch);
}

void process_Surge_Out(uchar ch)
{
if(param3[ch])	((PF)(param3[ch]))();
 TPU_kill(ch);
 done_with_pulse=1;
}
int   TPU_ECAT_phase_done(void)
{
  return done_with_pulse;
}
