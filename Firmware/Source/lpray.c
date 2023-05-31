#include <shortdef.h>
#include "switches.h"

extern volatile ulong	vectors[];
extern volatile ushort	SCDR;
extern volatile ushort	SCSR;
extern volatile ushort	SCCR0;
extern volatile ushort	SCCR1;
extern volatile ushort	QMCR;
extern volatile ushort	QILR_QIVR;
typedef void	(*PF)(void);

interrupt void	MY_SCI_TRAP(void);
volatile uchar	fin[64];
volatile uint		head;
volatile uint		tail;
volatile uint		tx_active;
volatile uint		tx_wait;
volatile uchar	tx_val;
volatile uint		chars;
uint		old_vector;
uint		old_qilr;
uint		old_scsr;
ushort	old_qmcr;
ushort	old_sccr0;
ushort	old_sccr1;

#define LOCAL_IRQ_LEVEL		(7)
#define LOCAL_VECTOR_BASE		(64)

void main();

void	init_my_int_sio()
{
	int	i;
	int	baud = 19200;

	head	= 0;
	tail	= 0;
	chars	= 0;
	for(i = 0; i < 64; i++)
		fin[i] = '~';

	old_qmcr = QMCR;
	QMCR = 0x08;						/* QSM configuration register		*/
	old_qilr = QILR_QIVR;
	i = 0;      						/* Clear SCI_IRQ level */
	i |= (LOCAL_IRQ_LEVEL * 256) + LOCAL_VECTOR_BASE;
	QILR_QIVR = i;
	/* IRQ vector */
	old_vector = vectors[LOCAL_VECTOR_BASE & 254];
	vectors[LOCAL_VECTOR_BASE & 254] = (long)MY_SCI_TRAP;
	vectors[256+(LOCAL_VECTOR_BASE & 254)] = (long)MY_SCI_TRAP;
	old_sccr0 = SCCR0;
	old_sccr1 = SCCR1;

	SCCR0 = 524288 / baud;
	SCCR1 = 0x002C;   /* 0000 0000 0010 1100 */
	tx_active= 0;
	tx_wait	= 0;
}

int	one_avail()
{
	return(chars);
}

uchar	get_one()
{
	char	c;

	while(!chars)
		;
	c = fin[head];
	head = (head + 1) % 64;
	chars--;
	return(c);
}

interrupt void	MY_SCI_TRAP(void)
{
	ushort	status;
	uchar		c;
	ushort	t;

	status = SCSR;
	if(status & 0x40)
	{
		t = SCDR;
		c = (uchar)t;
		fin[tail] = c;
		tail = (tail + 1) % 64;
		chars++;
	}
}
       
void	restore_serint(void)
{
	QMCR = old_qmcr;
	QILR_QIVR = old_qilr;
	vectors[LOCAL_VECTOR_BASE & 254] = old_vector;
	SCCR0 = old_sccr0;
	SCCR1 = old_sccr1;
}

void	tx(uchar c)
{
	ushort	status;

	status = 0;
	while(!(status & 0x100))
		status = SCSR;
	SCDR = c;
}
 
int load_and_pray()
{
	uchar	c;
	uint	addr;
	uint	cnt;
	uint	csum;
	uchar	*cp;

	LCD_reset();
	LCD_cursor(0);
	LCD_gotoxy(8,1);
	LCD_puts("<<< UPGRADE WARNING >>>");
	LCD_gotoxy(0,3);
	LCD_puts("   Do NOT interrupt system power and\n\r");
	LCD_puts("   fiber optic connections while the\n\r");
	LCD_puts("   system EEPROMS are being UPGRADED");

	asm(" move.w #$2700,SR");				/* Enable TPU interrupts */
	init_my_int_sio();
	addr = 0x123456;
	cnt  = 0x654321;
	tx('S');
	tx('t');
	tx('a');
	tx('r');
	tx('t');
	tx(':');
	cp=(uchar *)&addr;
	while(1)
	{
		c = get_one();
		if(c == 0xff)
		{
			tx('d');
			tx('o');
			tx('n');
			tx('e');
			tx('.');
			tx(13);
			tx(10);
			restore_serint();
			return(0);
		}
		else
			if(c == 0xfe)
			{
				cnt = (c & 0x3f);
				cp[0] = get_one();
				cp[1] = get_one();
				cp[2] = get_one();
				cp[3] = get_one();
				restore_serint();
				((PF)(addr))();
			}
			else
				if(c == 0xfd)
				{
					SCCR0 = 524288 / 38400;
				}
				else
				{
					if(c & 0x80)
					{
						cnt = (c & 0x3f);
						cp[0] = get_one();
						cp[1] = get_one();
						cp[2] = get_one();
						cp[3] = get_one();
					}
					else
						if(c & 0x40)
						{
							addr++;
							cnt = (c & 0x3F);
						}
						else
						{
							cnt = c;
						}
					csum  = cp[0];
					csum += cp[1];
					csum += cp[2];
					csum += cp[3];
					csum += (cnt+5);
					for(cnt; cnt > 0; cnt--)
					{
						c = get_one();
						*(uchar *)(addr) = c;
						csum += c;
						addr++;
					}
					c  = csum;
					c ^= 0xff;
					tx(c);
				}
	}
}
