#include <shortdef.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "display.h"
#include "front.h"
#include "switches.h"
#include "module.h"
#include "ecat.h"
#include "intface.h"
#include "timer.h"
#include "eft.h"
#include "key.h"
#include "emc.h"

const char 	IL_RDOOR[]		= "The Door on the RIGHT Hand\n\r";
const char	IL_LDOOR[]		= "The Door on the LEFT Hand\n\r";
const char 	IL_GDOOR[]		= "     Side of Drawer #%1d is OPEN";
const char 	IL_BDOOR[]		= "     Side of the BASE unit is OPEN";
const char	IL_RearDoor[]	= "The Rear Door of the E4556 is OPEN";
const char 	IL_LCABLE[]		= "The EFT Cable on the LEFT Side\n\r";
const char	IL_RCABLE[]		= "The EFT Cable on the RIGHT Side\n\r";
const char	IL_GCABLE[]		= "     of Drawer #%1d is DISCONNECTED";
const char	IL_PQFDOOR[]	= "One of the PQF Doors is OPEN\n\r";
const char	IL_PQFSNUB[]	= "The SNUB Bus Interlock is OPEN\n\r";
const char	IL_PQFTEMP[]	= "The OVER-Temperature Interlock is OPEN\n\r";
const char 	IL_PQFPWR[]		= "The POWER Module Interlock is OPEN\n\r";
const char	IL_AMPTEMP0[]	= "The Reference Choke OVER-Temperature\n\r     Interlock is OPEN";
const char	IL_AMPTEMP1[]	= "The Reference Resistor OVER-Temperature\n\r     Interlock is OPEN";

static uchar new_entry;  /* Used for display control during numeric entry. */
static uchar end_of_field;  /* Triggers BEEP at end of numeric field. */

int	current_field;
uchar	keyenable;
uchar	menukeyenable;
uchar	old_word[10];
uchar	contrast;
uchar	(*func_manager)(char);
ONE_FIELD		field_list[32];

extern uchar			calibration;
extern uchar			ecatmode;
extern MODDATA		moddata[];
extern DISDATA		display;
extern CHGDATA		chgdata;
extern SRGCHEAT		srgCheat;
extern EFTCHEAT		eftCheat;
extern PKDATA			peakdata;
extern NUMERIC_REC	fp_srgVLT;

extern volatile uint	ilockstate;

extern void	blue_off(void);

void	ReportInterlockError(
uint	state,
FILE	*dest)
{
	char	faultmodule;
	uchar	faultlock;

	if(dest == NULL)
		return;
	
  if(!(state & 4))
	{
		fputs("The Interlock FUSE is OPEN",dest);
		return;
	}
	if(!(state & 8))
	{
		faultmodule	= (char)((state >> 16) & 0x0F);
		faultlock	= (state >> 8) & 0x1f;
		/* Surge Module Interlocks */
		if(moddata[faultmodule].modinfo.typePrimary & TYPE_SRG)
		{
			/*
			 * Faultlock = IL0 - IL1 (0x1E) ::= RIGHT Door
			 * FaultLock = IL2 - Il3 (0x1C) ::= Not Used
			 * Faultlock = IL4 - IL5 (0x18) ::= LEFT Door (FULL-WIDE Only)
			 * FaultLock = IL6 - IL7 (0x10) ::= Not Used
			 */
			if((moddata[faultmodule].modinfo.id == E522S_BOX) || (moddata[faultmodule].modinfo.id == E521S_BOX))
			{
				fputs("One of the SIDE COVERS on the\n\r",dest);
				if(moddata[faultmodule].modinfo.id == E522S_BOX)
					fputs("     E522 Main Cabinet is OPEN",dest);
				else
					fputs("     E521 Main Cabinet is OPEN",dest);
				return;
			}
			else
			{
                uchar left_interlock = IL_ILOCK4;  // all modules except E505B
                if ( moddata[faultmodule].modinfo.id == E505B_BOX )
                    left_interlock = IL_ILOCK2;  // E505B is different because it has a built-in CDN
				if ( ( faultmodule & 0x01 ) || ( faultlock == left_interlock ) )
					fputs( IL_LDOOR, dest );
				else
					fputs( IL_RDOOR, dest );
				fprintf( dest, IL_GDOOR, ( (state>>17) & 0xff ) + 1 );
			}
			return;
		}
		/* Coupler Module Interlocks (Except EFT/COUPLER combinations) */
		if((moddata[faultmodule].modinfo.typePrimary & TYPE_CPLGEN) &&
			(!(moddata[faultmodule].modinfo.typePrimary & TYPE_EFT)) &&
			(!(moddata[faultmodule].modinfo.typePrimary & TYPE_EMC)))
		{
			/*
			 * Faultlock = IL0 - IL1 (0x1E) ::= RIGHT Door
			 * FaultLock = IL2 - Il3 (0x1C) ::= LEFT Door (FULL-WIDE Only)
			 * Faultlock = IL4 - IL5 (0x18) ::= Not Used
			 * FaultLock = IL6 - IL7 (0x10) ::= Not Used
			 */
			if((moddata[faultmodule].modinfo.id == E4556_BOX) && (faultlock == IL_ILOCK0))
			{
				LCD_gotoxy(2,4);
				fputs(IL_RearDoor,dest);
			}
			else
			{
				if((faultmodule & 0x01) || (faultlock == IL_ILOCK2))
					fputs(IL_LDOOR,dest);
				else
					fputs(IL_RDOOR,dest);
				fprintf(dest,IL_GDOOR,((state>>17) & 0xff)+1);
			}
			return;
		}
		/* PQF Module Interlocks */
		if(moddata[faultmodule].modinfo.typePrimary & TYPE_PQF)
		{
			switch(moddata[faultmodule].modinfo.id)
			{
				case EP61_BOX:
				case EP62_BOX:
					switch(faultlock)
					{
						case IL_ILOCK0:	/* Door */
							fputs(IL_PQFDOOR,dest);
							break;
						case IL_ILOCK2:	/* Snub */
							fputs(IL_PQFSNUB,dest);
							break;
						case IL_ILOCK4:	/* Temp */
							fputs(IL_PQFTEMP,dest);
							break;
						case IL_ILOCK6:	/* Power*/
							fputs(IL_PQFPWR,dest);
							break;
					}
					break;
				case EP3_BOX:
					if((faultmodule & 0x01) || (faultlock == IL_ILOCK2))
						fputs(IL_LDOOR,dest);
					else
						fputs(IL_RDOOR,dest);
					fprintf(dest,IL_GDOOR,((state>>17) & 0xff)+1);
					break;
			}
			return;
		}
		/* EFT Module Interlocks */
		if(moddata[faultmodule].modinfo.typePrimary & TYPE_EFT)
		{
			switch(moddata[faultmodule].modinfo.id)
			{
				case E421_BOX:
				case E422_BOX:
					switch(faultlock)
					{
						case IL_ILOCK0:	/* Right Hand Door	*/
						case IL_ILOCK2:	/* Left Hand Door		*/
							if((faultmodule & 0x01) || (faultlock == IL_ILOCK2))
		 						fputs(IL_LDOOR,dest);
							else
								fputs(IL_RDOOR,dest);
							fprintf(dest,IL_GDOOR,((state>>17) & 0xff)+1);
							break;
						case IL_ILOCK4:	/* Cable */
							if(faultmodule & 0x01)
 								fputs(IL_LCABLE,dest);
							else
								fputs(IL_RCABLE,dest);
							fprintf(dest,IL_GCABLE,((state>>17) & 0xff)+1);
							break;
						case IL_ILOCK6:	/* UNUSED */
						default:
							break;
					}
					return;
				default:
					switch(faultlock)
					{
						case IL_ILOCK0:	/* Right Hand Door	*/
						case IL_ILOCK4:	/* Left Hand Door		*/
							if((faultmodule & 0x01) || (faultlock == IL_ILOCK4))
		 						fputs(IL_LDOOR,dest);
							else
								fputs(IL_RDOOR,dest);
							fprintf(dest,IL_GDOOR,((state>>17) & 0xff)+1);
							break;
						case IL_ILOCK2:	/* Cable */
							if(faultmodule & 0x01)
 								fputs(IL_LCABLE,dest);
							else
								fputs(IL_RCABLE,dest);
							fprintf(dest,IL_GCABLE,((state>>17) & 0xff)+1);
							break;
						case IL_ILOCK6:	/* UNUSED */
						default:
							break;
					}
					return;
			}
		}
		if(moddata[faultmodule].modinfo.typePrimary & TYPE_EMC)
		{
			switch(moddata[faultmodule].modinfo.id)
			{
				case ERI1_BOX:
					switch(faultlock)
					{
						case IL_ILOCK0:	/* Right Hand Door	*/
							if(faultmodule & 0x01)
								fputs(IL_LDOOR,dest);
							else
								fputs(IL_RDOOR,dest);
							fprintf(dest,IL_GDOOR,((state>>17) & 0xff)+1);
							break;
						case IL_ILOCK4:	/* Thermal Interlock		*/
							fputs(IL_PQFTEMP,dest);
							break;
					}
					return;
				case ERI3_BOX:
					switch(faultlock)
					{
						case IL_ILOCK0:	/* Right Hand Door	*/
						case IL_ILOCK2:	/* Left Hand Door		*/
							if(faultlock == IL_ILOCK2)
								fputs(IL_LDOOR,dest);
							else
								fputs(IL_RDOOR,dest);
							fprintf(dest,IL_GDOOR,((state>>17) & 0xff)+1);
							break;
						case IL_ILOCK4:	/* Thermal Choke Interlock		*/
							fputs(IL_AMPTEMP0,dest);
							break;
						case IL_ILOCK6:	/* Thermal Resistor Interlock	*/
							fputs(IL_AMPTEMP1,dest);
							break;
					}
					return;
				case EP71_BOX:
				case EP72_BOX:
				case EP73_BOX:
				case EP91_BOX:
				case EP92_BOX:
				case EP93_BOX:
				case EP94_BOX:
					switch(faultlock)
					{
						case IL_ILOCK0:	/* Right Hand Door	*/
						case IL_ILOCK2:	/* Left Hand Door		*/
							if(faultlock == IL_ILOCK2)
								fputs(IL_LDOOR,dest);
							else
								fputs(IL_RDOOR,dest);
							fputs(IL_BDOOR,dest);
							break;
						case IL_ILOCK4:	/* Panel Covers		*/
							fputs("One of the SIDE COVERS on the\n\r",dest);
 							fputs("     BASE Main Module is OPEN",dest);
							break;
					}
					return;
			}
		}
		if(moddata[faultmodule].modinfo.typePrimary & TYPE_CLAMP)
		{
			switch(faultlock)
			{
				case IL_ILOCK0:	/* Right Hand Door	*/
				case IL_ILOCK2:	/* Left Hand Door		*/
					if(faultlock == IL_ILOCK2)
						fputs(IL_LDOOR,dest);
					else
						fputs(IL_RDOOR,dest);
					fprintf(dest,IL_GDOOR,((state>>17) & 0xff)+1);
					break;
			}
			return;
		}
		/* Unknown Interlock */
		fputs("UNKNOWN Interlock Condition\n\r",dest);
		fprintf(dest,"     Interlock Code: %#05X",state);
	}
	else
		if(!(state & 16))
			fputs("The Micro Board is LOOSE",dest);
		else
			if(!(state & 32))
				fputs("The Peak Detector Board is LOOSE",dest);
			else
				if(!(state & 64))
					fputs("The EXTERNAL Interlock is OPEN",dest);
	return;
}

void	field_validation(
ONE_FIELD	*pof,
uchar		on)
{
	uchar	ox,oy;
	uchar	tvar;

	ox = LCD_getx();
	oy = LCD_gety();
	if(on)
/*	if(on && !pof->valid) */
	{
		pof->valid = 1;
		if(pof->left)
			(*pof->left)(pof);
	}
	else
		if(!on)
		{
			pof->valid = 0;
			LCD_gotoxy(pof->x_pos,pof->y_pos);
			for(tvar = 0; tvar <= (pof->x_end-pof->x_pos); tvar++)
				LCD_putc(' ');
			LCD_gotoxy(pof->x_pos,pof->y_pos);
			if((pof->x_end-pof->x_pos) <= 1)
				LCD_puts("NA");
			else
				LCD_puts("N/A");
		}
	LCD_gotoxy(ox,oy);
}

void	field_horizontal(
uchar	field,
uchar	prev,
uchar	next)
{
	field_list[field].xPrev = prev;
	field_list[field].xNext = next;
}

void	field_vertical(
uchar	field,
uchar	prev,
uchar	next)
{
	field_list[field].yPrev = prev;
	field_list[field].yNext = next;
}

void	list_entered(
ONE_FIELD	*pof)
{
	LCD_gotoxy(pof->x_pos-1,pof->y_pos);
	((LIST_REC *)(pof->ref_rec))->dir = 1;
	if(((LIST_REC *)(pof->ref_rec))->dflag)
		LCD_putc(((((LIST_REC *)(pof->ref_rec))->dir)?('>'):('<')));
	LCD_hilight(pof->x_pos,pof->y_pos,pof->x_pos+((LIST_REC *)(pof->ref_rec))->loc_len,pof->y_end);
	LCD_cursor(0);
}


void	list_left(
ONE_FIELD	*pof)
{
	int	oldx,oldy;

	if(pof->valid)
	{
		oldx = LCD_getx();
		oldy = LCD_gety();
		LCD_gotoxy(pof->x_pos,pof->y_pos);
		LCD_puts(((LIST_REC *)(pof->ref_rec))->labels[((LIST_REC *)(pof->ref_rec))->lv]);
		LCD_gotoxy(oldx,oldy);
	}
	if(((LIST_REC *)(pof->ref_rec))->dflag)
	{
		LCD_gotoxy(pof->x_pos-1,pof->y_pos);
		LCD_putc(' ');
	}
	LCD_normal(pof->x_pos,pof->y_pos,pof->x_pos+((LIST_REC *)(pof->ref_rec))->loc_len,pof->y_end);
}

void	init_list_field(
int	x,
int	y,
uchar		dflag,
LIST_REC	*lr,
void	(*fch)(ONE_FIELD *pof),
char	(*lfunc)(uchar),
char	**labels,
uchar	displayFlag)
{
	int	i,lf,lt;
	char	tvar;

	for(i = 0; (i < 32) && (field_list[i].valid); i++)
		;
	if(field_list[i].valid)
		printf("(ERR) Fld list FULL");
	else
	{
		field_list[i].valid = 1;
		field_list[i].x_pos = x;
		field_list[i].y_pos = y;
		lr->loc_len = strlen(labels[0])-1;
		for(tvar = 0; tvar < 10 && *labels[tvar]; tvar++)
		{
			lt = strlen(labels[tvar])-1;
			if(lt > lr->loc_len)
				lr->loc_len = lt;
		}
		field_list[i].x_end		= x + lr->loc_len;
		field_list[i].y_end		= y;
		field_list[i].ref_rec	= lr;
		field_list[i].process	= list_process;
		field_list[i].changed	= fch;
		field_list[i].entered	= list_entered;
		field_list[i].left		= list_left;
		field_list[i].leftfunc	= lfunc;
		lr->labels = labels;
		if(displayFlag)
		{
			LCD_gotoxy(x,y);
			LCD_puts(lr->labels[0]);
		}
		lr->field= i;
		lr->dflag= dflag;
		lr->dir	= 1;
	}
}

char	list_process(
char	c,
int	x,
int	y,
ONE_FIELD	*pof)
{
	short	tvar;

	if(!pof->valid)
	{
		KEY_beep();
		return(0);
	}
	if(c == 13)
	{
		LCD_gotoxy(pof->x_pos,pof->y_pos);
		if(!((LIST_REC *)(pof->ref_rec))->dir)
		{
			tvar = ((LIST_REC *)(pof->ref_rec))->lv - 1;
			if(tvar < 0)
			{
				tvar = -1;
				while((((LIST_REC *)(pof->ref_rec))->labels[++tvar]))
					;
				tvar--;
			}
			((LIST_REC *)(pof->ref_rec))->dir = 0;
		}
		else
		{
			tvar = ((LIST_REC *)(pof->ref_rec))->lv + 1;
			((LIST_REC *)(pof->ref_rec))->dir = 1;
		}
		if(!(((LIST_REC *)(pof->ref_rec))->labels[tvar]) || tvar < 0)
			tvar = 0;
		((LIST_REC *)(pof->ref_rec))->lv = tvar;
		LCD_puts(((LIST_REC *)(pof->ref_rec))->labels[tvar]);
		LCD_gotoxy(pof->x_pos,pof->y_pos);
		LCD_hilight(pof->x_pos,pof->y_pos,pof->x_pos+((LIST_REC *)(pof->ref_rec))->loc_len,pof->y_end);
		(pof->changed)();
	}
	else
		if(c == '-')
		{
			((LIST_REC *)(pof->ref_rec))->dir ^= 1;
			if(((LIST_REC *)(pof->ref_rec))->dflag)
			{
				LCD_gotoxy(pof->x_pos-1,pof->y_pos);
				LCD_putc(((((LIST_REC *)(pof->ref_rec))->dir)?('>'):('<')));
			}
		}
	return(0);
}


void	init_numeric_field(
int	x,
int	y,
int	len,
NUMERIC_REC	*fr,
void	(*fch)(ONE_FIELD *pof),
char	(*lfunc)(uchar),
uchar	displayFlag)
{
	int	i;

	for(i = 0; (i < 32) && (field_list[i].valid); i++)
		;
	if(field_list[i].valid)
		printf("(ERR) Fld list FULL");
	else
	{
		field_list[i].valid		= 1;
		field_list[i].x_pos		= x;
		field_list[i].y_pos		= y;
		field_list[i].x_end		= x + len - 1;
		field_list[i].y_end		= y;
		field_list[i].ref_rec	= fr;
		field_list[i].process	= numeric_process;
		field_list[i].changed	= fch;
		field_list[i].entered	= numeric_entered;
		field_list[i].left		= numeric_left;
		field_list[i].leftfunc	= lfunc;
		if(displayFlag)
		{
			LCD_gotoxy(x,y);
			if(fr->minus)
				sprintf(fr->buf,"%+-d",fr->f);
			else
				sprintf(fr->buf,"%-d",fr->f);
			LCD_puts(fr->buf);
		}
		fr->field = i;
	}
}

uchar get_new_entry( void )
{
	return( new_entry );
}

void clear_numeric_field( ONE_FIELD * pof )
{
	uchar i;

	LCD_gotoxy(pof->x_pos,pof->y_pos);
	for ( i = 0; i <= (pof->x_end - pof->x_pos); i++ )
	{
		POF_BUF[i] = 0;
		LCD_putc(' ');
	}
	POF_BUF[i] = 0;
	LCD_gotoxy(pof->x_pos,pof->y_pos);
	if(POF_MINUS)
	{
		POF_BUF[0] = '+';
		LCD_putc(POF_BUF[0]);
		LCD_gotoxy(pof->x_pos+1,pof->y_pos);
	}
	end_of_field = FALSE;  /* Triggers BEEP at end of numeric field. */
}

char	numeric_process(
char	c,
int	x,
int	y,
ONE_FIELD	*pof)
{
	char	temp_buf;
	int	i,j;
	int	tval;

	if(!pof->valid)
	{
		KEY_beep();
		return(0);
	}
	if(c == 'C')  /* Clear Entry key */
	{
		clear_numeric_field( pof );
		new_entry = TRUE;
	}
	else
	{
		if(c == 13)  /* <ENTER> key */
		{
			tval = atoi(POF_BUF);
			if((abs(tval) <= POF_MAX) && (abs(tval) >= POF_MIN))
			{
				POF_NUMERIC = tval;
				(pof->changed)();
				/* Leave & reenter field to acknowledge <ENTER> keypress
					(positions the cursor at the start of the field, and
					clears the end_of_field flag) */
				(*pof->left)( pof );
				(*pof->entered)( pof );
			}
			else	/* Invalid entry */
			{
				KEY_beep();
				clear_numeric_field( pof );
			}

			new_entry = TRUE;
		}
		else
		{
			if(c == '-')
			{
				if( !POF_MINUS )
				{
					KEY_beep();
				}
				else	/* POF_MINUS == TRUE:  sign key is allowed */
				{
					/* Change sign */
					(POF_BUF[0] == '-') ? (POF_BUF[0] = '+') : (POF_BUF[0] = '-');

					LCD_gotoxy(pof->x_pos,pof->y_pos);
					fprintf(DISPLAY,POF_BUF);
					LCD_gotoxy(x, pof->y_pos);

					new_entry = FALSE;
				}
			}
			else	/* Numeric digit key */
			{
				/* Cursor at start of buffer:  clear buffer, add char,
						& shift cursor right.
					Cursor at end of buf:  add char to end & shift cursor right.
					Cursor beyond end of field:  BEEP. */

				if ( end_of_field )
				{
					KEY_beep();
				}
				else
				{
					/* Clear buffer if this is first digit of new entry (clear
						buffer even if sign has been changed already, indicated
						by (!new_entry) AND cursor still at position 1). */
					i = x - pof->x_pos;
					if ( new_entry )
					{
						clear_numeric_field( pof );
					}
					else
					{
						if ( ( POF_MINUS ) && ( i == 1 ) )
						{
							/* Clear buffer but restore sign value. */
							temp_buf = POF_BUF[0];
							clear_numeric_field( pof );
							POF_BUF[0] = temp_buf;
						}
					}

					/* Insert new char into buffer, and redisplay whole buffer. */
					POF_BUF[i] = c;
					LCD_gotoxy(pof->x_pos,pof->y_pos);
					fprintf(DISPLAY,POF_BUF);

					/* If new cursor position is beyond end of field,
						then set flag and reposition cursor at end of field.
						Else increment cursor position. */
					if ( (x + 1) > (pof->x_end) )
					{
						end_of_field = TRUE;
						j = pof->x_end;
					}
					else
					{
						j = x + 1;
					}

					/* Reposition cursor. */
					LCD_gotoxy( j, pof->y_pos );
				}
				new_entry = FALSE;  /* Reset flag. */
			}
		}
	}
	return(0);
}


void	numeric_entered(
ONE_FIELD	*pof)
{
	int	i;
	char	*cptr;

	new_entry = TRUE;  /* Used for display control during numeric entry. */
	end_of_field = FALSE;  /* Triggers BEEP at end of numeric field. */

	cptr = ((NUMERIC_REC *)(pof->ref_rec))->buf;
	i = 0;
	while((*cptr == ' ') && (i < (pof->x_end-pof->x_pos)))
	{
		i++;
		cptr++;
	}
	if(((NUMERIC_REC *)(pof->ref_rec))->minus)
		i++;
	LCD_gotoxy(pof->x_pos+i,pof->y_pos);
	LCD_hilight(pof->x_pos,pof->y_pos,pof->x_end,pof->y_end);
	LCD_cursor(1);
}


void	numeric_left(
ONE_FIELD	*pof)
{
	int	i;
	int	oldx;
	int	oldy;

	NUMERIC_REC	*fr = ((NUMERIC_REC *)(pof->ref_rec));
	if(pof->valid)
	{
		oldx = LCD_getx();
		oldy = LCD_gety();
		LCD_gotoxy(pof->x_pos,pof->y_pos);
		for(i = 0; i <= (pof->x_end-pof->x_pos); i++)
			LCD_putc(' ');
		LCD_gotoxy(pof->x_pos,pof->y_pos);
		if(fr->minus)
			sprintf(fr->buf,"%+-d",fr->f);
		else
			sprintf(fr->buf,"%-d",fr->f);
		LCD_puts(fr->buf);
		LCD_gotoxy(oldx,oldy);
	}
	LCD_normal(pof->x_pos,pof->y_pos,pof->x_end,pof->y_end);
}


char	bool_process(
char	c,
int	x,
int	y,
ONE_FIELD	*pof)
{
	if(!pof->valid)
	{
		KEY_beep();
		return(0);
	}
	if(c == 13)
	{
		LCD_gotoxy(pof->x_pos,pof->y_pos);
		if(((BOOL_REC *)(pof->ref_rec))->bv ^= 1)
			LCD_puts(((BOOL_REC *)(pof->ref_rec))->true_label);
		else
			LCD_puts(((BOOL_REC *)(pof->ref_rec))->false_label);
		LCD_gotoxy(pof->x_pos,pof->y_pos);
		LCD_hilight(pof->x_pos,pof->y_pos,pof->x_pos+((BOOL_REC *)(pof->ref_rec))->loc_len,pof->y_end);
		(pof->changed)();
	}
	return(0);
}


void	bool_entered(
ONE_FIELD	*pof)
{
	LCD_hilight(pof->x_pos,pof->y_pos,pof->x_pos+((BOOL_REC *)(pof->ref_rec))->loc_len,pof->y_end);
	LCD_cursor(0);
}


void	bool_left(
ONE_FIELD	*pof)
{
	int	oldx,oldy;

	if(pof->valid)
	{
		oldx = LCD_getx();
		oldy = LCD_gety();
		LCD_gotoxy(pof->x_pos,pof->y_pos);
		if(((BOOL_REC *)(pof->ref_rec))->bv)
			LCD_puts(((BOOL_REC *)(pof->ref_rec))->true_label);
		else
			LCD_puts(((BOOL_REC *)(pof->ref_rec))->false_label);
		LCD_gotoxy(oldx,oldy);
	}
	LCD_normal(pof->x_pos,pof->y_pos,pof->x_pos+((BOOL_REC *)(pof->ref_rec))->loc_len,pof->y_end);
}


void	init_bool_field(
int			x,
int			y,
BOOL_REC	*br,
void			(*fch)(ONE_FIELD *pof),
char			(*lfunc)(uchar),
char			*false_label_in,
char			*true_label_in,
uchar		displayFlag)
{
	int	i,lf,lt;

	for(i = 0; (i < 32) && (field_list[i].valid); i++)
		;
	if(field_list[i].valid)
		printf("(ERR) Fld list FULL");
	else
	{
		field_list[i].valid = 1;
		field_list[i].x_pos = x;
		field_list[i].y_pos = y;
		lf = strlen(false_label_in)-1;
		lt = strlen(true_label_in)-1;
		if(lf > lt)
			br->loc_len = lf;
		else
			br->loc_len = lt;
		field_list[i].x_end		= x + br->loc_len;
		field_list[i].y_end		= y;
		field_list[i].ref_rec	= br;
		field_list[i].process	= bool_process;
		field_list[i].changed	= fch;
		field_list[i].entered	= bool_entered;
		field_list[i].left		= bool_left;
		field_list[i].leftfunc	= lfunc;
		br->true_label = true_label_in;
		br->false_label= false_label_in;
		if(displayFlag)
		{
			LCD_gotoxy(x,y);
			if(br->bv)
				LCD_puts(true_label_in);
			else
				LCD_puts(false_label_in);
		}
		br->field = i;
		br->dir	 = 1;
	}
}


void	init_front(
uchar	mode)
{
	int	i;
	int module_type;
  LCD_reset();									/* Reset the display		*/
	KEY_led(GO_LED,0);
	KEY_led(STOP_LED,1);
	if(mode >= DISPLAY_COVER_INT)
	{
		switch(ecatmode)
		{
			case SRG_MODE:
				SRG_StopSystem();
				SRG_CPLCheck(current_field);
				break;
			case EFT_MODE:
				EFT_StopSystem();
				break;
			case PQF_MODE:
			default:
				break;
		}
		blue_off();
		LCD_gotoxy(13,1);
		KEY_beep();
		LCD_puts("INTERLOCK OPEN");
		LCD_gotoxy(5,4);
        ReportInterlockError(ilockstate,DISPLAY);
		return;
	}

	switch(mode)
	{
		case DISPLAY_WARMUP:
			FP_UpdateWarmup(0);
			break;
		case DISPLAY_REMOTE:
			FP_UpdateRemote();
			break;
		default:
			Show_State(mode);
			for(i = 0; (i < 32) && (field_list[i].valid == 0); i++)
				;
			if(i >= 32)
			{
				LCD_cursor(0);
				return;
			}
			LCD_cursor(1);
			LCD_gotoxy(field_list[i].x_pos,field_list[i].y_pos);
			current_field = 0;
			if(field_list[current_field].entered)
				(*field_list[current_field].entered)(&field_list[current_field]);
			break;
	}
}

void	parse_front(
char	c)
{
	int	dx,dy;
	int	curx,cury;
	int	i,j,v,jv;
	int	old_field = current_field;

	if(display.current >= DISPLAY_COVER_INT)
		return;

	dx = dy = 0;
	curx = LCD_getx();
	cury = LCD_gety();

	for(i = 9; i > 0; i--)
		old_word[i] = old_word[i-1];
   
	old_word[0] = c;
/*
 *	The following enables/disables the Surge or EFT line check cheat feature
 *	to allow for feedback testing.  This mode will automatically disable
 *	upon entering or exiting the Surge or EFT front panel screen.  Also, this
 *	mode cannot be enabled while in calibration mode
 */
	if ( (old_word[4] == '1') &&
		 (old_word[3] == '.') &&
		 (old_word[2] == '7') &&
		 (old_word[1] == '3') &&
		 (old_word[0] == '2') &&
		 (!calibration) )
	{
		if ( display.current == DISPLAY_EFT )
			eftCheat.lineCheck ^= 1;
		else if ( display.current == DISPLAY_SRG )
			srgCheat.lineCheck ^= 1;

		LCD_gotoxy(0,7);
		if ( eftCheat.lineCheck || srgCheat.lineCheck )
			LCD_puts("__NO PHASE CHECK_");
		else
			LCD_puts("_____ _____ _____");
		KEY_beep();
		LCD_gotoxy(curx,cury);
		return;
	}

/*
 *	The following enables/disables the force of the EFT's high voltage
 *	range which is used for aligning two EFT switches.  This mode will
 *	automatically disable upon entering or exiting the EFT front panel
 *	screen.  Also, this mode cannot be enabled while in calibration mode
 */
	if((old_word[4] == '2') &&
		(old_word[3] == '.') &&
		(old_word[2] == '2') &&
		(old_word[1] == '3') &&
		(old_word[0] == '6') &&
		(!calibration) &&
		(display.current == DISPLAY_EFT))
	{
		eftCheat.forceHVRange ^= 1;
		LCD_gotoxy(18,7);
		if(!eftCheat.forceHVRange)
			LCD_puts("_____ ____ _____ MEAS");
		else
			LCD_puts("_Force HV-Range_ MEAS");
		KEY_beep();
		LCD_gotoxy(curx,cury);
		return;
	}

/*
 *	The following will cause a 5-channel measurement board to function
 *	as though it was a 3-channel board.  This mode allows for testing
 *	of the 3 and 5 channel V/I monitors in a special test module.  This
 *	mode will automatically disable upon entering or exiting the
 *	MEASUREMENT screen.
 */
	if((old_word[4] == '2') &&
		(old_word[3] == '.') &&
		(old_word[2] == '4') &&
		(old_word[1] == '4') &&
		(old_word[0] == '9') &&
		(display.current == DISPLAY_MEASURE))
	{
		peakdata.cheat ^= 1;
		LCD_gotoxy(27,3);
		if(!peakdata.cheat)
			LCD_puts("           ");
		else
			LCD_puts("(3-channel)");
		KEY_beep();
		LCD_gotoxy(curx,cury);
		return;
	}

/*
 *	The following enables/disables calibration mode which will keep the
 *	high voltage settings from being affected by any calibration factors.
 *	upon entering or exiting the EFT front panel screen.  This mode cannot
 *	be enabled while the EFT lineCheck or forceHVRange mode is active
 */
	if((old_word[4] == '1') &&
		(old_word[3] == '.') &&
		(old_word[2] == '4') &&
		(old_word[1] == '1') &&
		(old_word[0] == '4') &&
		(!srgCheat.lineCheck) &&
	    (!eftCheat.lineCheck) &&
		(!eftCheat.forceHVRange) &&
		(display.current != DISPLAY_REMOTE))
	{
		calibration ^= 1;
		LCD_gotoxy(0,7);
		switch(display.current)
		{
			case DISPLAY_SRG:
				if(!calibration)
				{
					LCD_puts("CHRGE _____ PULSE _____ ____ _____ MEAS");
					fp_srgVLT.maxv = 6600;
				}
				else
				{
					LCD_puts("CHRGE _____ PULSE ___CALIBRATION__ MEAS");
					fp_srgVLT.maxv = 8000;
				}
				break;
			case DISPLAY_EFT:
				if(!calibration)
					LCD_puts("_____ _____ _____ _____ ____ _____ MEAS");
				else
					LCD_puts("_____ _____ _____ ___CALIBRATION__ MEAS");
				break;
			case DISPLAY_LOCAL:
				if(calibration)
					LCD_puts("SURGE S/CAL EFT/B E/CAL ESD  _____ PQF");
				else
					LCD_puts("SURGE _____ EFT/B _____ ESD  _____ PQF");
				break;
		}
		KEY_beep();
		LCD_gotoxy(curx,cury);
		return;
	}

	if(display.current == DISPLAY_REMOTE)
	{
		if((menukeyenable) && (c == 'M'))
		{
			display.request = DISPLAY_LOCAL;
			/* Reset system later... */
		}
		return;
	}
	switch(c)
	{
		case '^':
			dy = -1;
			if(display.current == DISPLAY_LOCAL)
			{
				contrast += 2;
				DAC_set(0,contrast);
			}
			break;
		case 'v':
			dy = 1;
			if(display.current == DISPLAY_LOCAL)
			{
				contrast -= 2;
				DAC_set(0,contrast);
			}
			break;
		case '>':
			dx = 1;
			if(display.current == DISPLAY_LOCAL)
				return;
			break;
		case '<':
			dx = -1;
			if(display.current == DISPLAY_LOCAL)
				return;
			break;
		case ',':
			break;
		case '.':
		case 'R':	/* Recall	*/
		case 'S':	/* Store		*/
		case 'G':	/* Start		*/
		case 'X':	/* Stop		*/
		case 'M':	/* Menu		*/
		case 'a':	/* Funct 1	*/
		case 'b':	/* Funct 2	*/
		case 'c':	/* Funct 3	*/
		case 'd':	/* Funct 4	*/
		case 'e':	/* Funct 5	*/
		case 'f':	/* Funct 6	*/
		case 'g':	/* Funct 7	*/
			if(func_manager)
				(func_manager)(c);
			break;
		default:
			if(!keyenable)
				return;
			if   ( ( ecatmode == SRG_MODE )
			  &&   ( chgdata.flag != CHG_IDLE )
			  &&   ( chgdata.flag != CHG_DELAY ) )
			{
				SRG_StopSystem();
			}

			/* Process key */
			if((*field_list[current_field].process)(c,curx,cury,&field_list[current_field]))
				dx=99;

/* DELETE IF NOT NEEDED ************************************ FD 27Sep95
			if(LCD_getx() > field_list[current_field].x_end)
				LCD_gotoxy(field_list[current_field].x_end,field_list[current_field].y_pos);
*/
	}
	switch(c)
	{
		case '>':
		case '<':
			if(c == '>')
				j = field_list[current_field].xNext;
			else
				j = field_list[current_field].xPrev;
			if(j == 0xFF)
				j = current_field;
			if(j != current_field)
			{
				current_field = j;
				LCD_gotoxy(field_list[j].x_pos,field_list[j].y_pos);
			}
			break;
		case '^':	/* Find previous vertical field */
		case 'v':	/* Find next vertical field */
			if(c == 'v')
				j = field_list[current_field].yNext;
			else
				j = field_list[current_field].yPrev;
			if(j == 0xFF)
				j = current_field;
			if(j != current_field)
			{
				current_field = j;
				LCD_gotoxy(field_list[j].x_pos,field_list[j].y_pos);
			}
			break;
		 case ',':	/* Force to NEXT field */
			for(j = current_field+1; j < 32; j++)
				if(field_list[j].valid)
					break;
			if(j >= 32)
				j = 0;
			if(j != current_field)
			{
				current_field = j;
				LCD_gotoxy(field_list[j].x_pos,field_list[j].y_pos);
			}
			break;
		default:
			if(dx)
			{
				if(((curx+dx) >= (field_list[current_field].x_pos)) &&
					((curx+dx) <= (field_list[current_field].x_end)))
					LCD_gotoxy(curx+dx,cury);
			}
			break;
	}
	if(old_field != current_field)
	{
		if(field_list[old_field].left)
			(*field_list[old_field].left)(&field_list[old_field]);
		if(field_list[old_field].leftfunc)
			(*field_list[old_field].leftfunc)(old_field);
		if(field_list[current_field].entered)
			(*field_list[current_field].entered)(&field_list[current_field]);
	}
}

