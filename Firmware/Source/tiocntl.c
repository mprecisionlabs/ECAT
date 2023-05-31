#include <stdio.h>
#include "tiocntl.h"
#include "intface.h"
#include "ecat.h"

extern volatile unsigned int	interlockData;
extern MODDATA	moddata[];

uchar TI_selbits[16];			/* Holds the TIx tristate enables*/
uchar TI_mapbits[16];			/* Holds the TILCLx to TIx bits	*/
uchar TO_mapbits[16];			/* Holds the TOx to TOLCLx bits	*/

int	exist(
int	d)
{
	int	base;
	unsigned short	temp;

	base = 0x60000+(d*0x1000);
	*((unsigned short *)(base+0x600)) = 0x00;
	temp = *((unsigned short *)(base+0x600));
	if(temp & 0x80)
		return(0);
	*((unsigned short *)(base+0x600)) = 0x01;
	temp = *((unsigned short *)(base+0x600));
	if(!(temp & 0x80))
		return(0);
	return(1);
}

int	istat(
int	d)
{
	int	base;
	unsigned short	temp;

	base = 0x60000+(d*0x1000);
	temp = *((unsigned short *)(base+0x600));
	temp = (temp & 0x1f) ^ 0x1f;
	return(temp);
}

int	ilock_stat(void)
{
	int	il;
	int	is;
	int	i;

	il = interlockData;
	il &= 0x7C;
	if((il & 0x04) && (!(il & 0x08)))
	{
		for(i = 0; i < 16; i++)
		if(exist(i))
		{
			is = istat(i);
			if((is) && (!(is & 0x01)))
			{
				il = (il & 0xff) | (i << 16) | (istat(i) << 8);
				i = 20;
			}
		}  
	}
	return(il);
}

/* Module TPU mapper functions	*/

char	TIO_TIMap(
uchar	module,
uchar	INmap)
{
	if(module > 15)
		return(-1);

	TI_mapbits[module] = INmap & 0x01;
	return(TIO_Refresh(module));
}

char	TIO_TOMap(
uchar	module,
uchar	TOmap)
{
	if(module > 15)
		return(-1);

	TO_mapbits[module] = TOmap & 0x03;
	return(TIO_Refresh(module));
}

char	TIO_TISelect(
uchar	module,
uchar	driver,
uchar	enable)
{
	uchar	mask;

	if((module > 15) || (driver > 3))
		return(-1);

	mask = 1 << driver;
	if(enable)
		TI_selbits[module] |= mask;
	else
		TI_selbits[module] &= (~mask);
	return(TIO_Refresh(module));
}

void	TIO_Reset(void)
{
	int	cntr;
	int	tvar;

	for(cntr = 0; cntr < 16; cntr++)
	{
		TIO_TOMap(cntr,TO0_DISABLE);
		TIO_TIMap(cntr,TILCL0_TI0);
		for(tvar = 0; tvar < 4; tvar++)
			TIO_TISelect(cntr,tvar,DISABLE);
	}
}

char	TIO_Refresh(
uchar	module)
{
	uchar	regval;

	if((module < 0) || (module > 15))
		return;

	regval = ((TO_mapbits[module] & 0x03) << 5) | ((TI_mapbits[module] & 1) << 3) & 0x78;
	if(TI_selbits[module] & 1)
		ECAT_TimerSelReg(module,regval);
	else
		ECAT_TimerSelReg(module,(regval | 4));

	if(TI_selbits[module] & 2)
		ECAT_TimerSelReg(module,(regval | 1));
	else
		ECAT_TimerSelReg(module,(regval | 5));

	if(TI_selbits[module] & 4)
		ECAT_TimerSelReg(module,(regval | 2));
	else
		ECAT_TimerSelReg(module,(regval | 6));

	if(TI_selbits[module] & 8)
		ECAT_TimerSelReg(module,(regval | 3));
	else
		ECAT_TimerSelReg(module,(regval | 7));
}

void	TIO_SRGDisable(void)
{
	uchar		timer_input, timer_input_max;
	char		module_index;

	for(module_index = 0; module_index < MAX_MODULES; module_index++)
    {
		if(moddata[module_index].modinfo.typePrimary & TYPE_SRG)
        {
            /* For E505B only, do not disable TI3_SELECT=3 (as for TIO_CPLDisable below),
               because E505B is a combined surge and coupling module */
            if( moddata[module_index].modinfo.id == E505B_BOX )
                timer_input_max = 3;
            else
                timer_input_max = 4;
            for(timer_input = 0; timer_input < timer_input_max; timer_input++)
                TIO_TISelect(module_index,timer_input,DISABLE);
            TIO_TOMap(module_index,TO0_DISABLE);
            TIO_TIMap(module_index,TILCL0_TI0);
        }
    }
}

void	TIO_CPLDisable(void)
{
	uchar		timer_input;
	char		module_index;

	for(module_index = 0; module_index < MAX_MODULES; module_index++)
		if(moddata[module_index].modinfo.typePrimary & TYPE_CPLGEN)
		{
			for(timer_input = 0; timer_input < 3; timer_input++)   /*do not disable TI3_SELECT=3*/
				TIO_TISelect(module_index,timer_input,DISABLE);
			TIO_TOMap(module_index,TO0_DISABLE);
			TIO_TIMap(module_index,TILCL0_TI0);
		}
}

void	TIO_EFTDisable(void)
{
	uchar		timer_input;
	char		module_index;

	for(module_index = 0; module_index < MAX_MODULES; module_index++)
		if(moddata[module_index].modinfo.typePrimary & TYPE_EFT)
		{
			for(timer_input = 0; timer_input < 3; timer_input++)
				TIO_TISelect(module_index,timer_input,DISABLE);

			/* Skip TI3 for Coupler combos */
			if(!(moddata[module_index].modinfo.typePrimary & TYPE_CPLGEN))
				TIO_TISelect(module_index,TI3_SELECT,DISABLE);

			TIO_TOMap(module_index,TO0_DISABLE);
			TIO_TIMap(module_index,TILCL0_TI0);
		}
}

void	TIO_PQFDisable(void)
{
	uchar		timer_input;
	char		module_index;

	for(module_index = 0; module_index < MAX_MODULES; module_index++)
		if(moddata[module_index].modinfo.typePrimary & TYPE_PQF)
		{
			for(timer_input = 0; timer_input < 4; timer_input++)
				TIO_TISelect(module_index,timer_input,DISABLE);
			TIO_TOMap(module_index,TO0_DISABLE);
			TIO_TIMap(module_index,TILCL0_TI0);
		}
}

void	TIO_SelectLinesync(
uchar	bayaddr)
{
	char bay;

	if(bayaddr >= MAX_MODULES)
		return;

	for(bay = 0; bay < MAX_MODULES; bay++)
		if((bay != bayaddr) && (moddata[bay].modinfo.typePrimary & TYPE_CPLGEN))
			TIO_TISelect(bay,TI3_SELECT,DISABLE);

	if(moddata[(char)(bayaddr)].modinfo.typePrimary & TYPE_CPLGEN)
		TIO_TISelect(bayaddr,TI3_SELECT,ENABLE);
}
/******* << END TIOCNTL.C >> *******/
