#include <stdio.h>
#include <float.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <mriext.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "serint.h"
#include "buffer.h"
#include "timer.h"
#include "parser.h"
#include "qsm.h"
#include "switches.h"
#include "intface.h"
#include "display.h"
#include "key.h"
#include "duart.h"

void	show_fun(
uint	mode,
uint	decodea,
char	*label)
{
	uint	offset;
	uint	decode;

	decode =(*(uint *)decodea);

	printf("%s",label);
	printf(":%08X ",decode);

	switch (mode & 3)
	{
		case 0:printf("Out   "); break;
		case 1:printf("Def   "); break;
		case 2:printf("8Bit  "); break;
		case 3:printf("16Bit "); break;
	}
	printf("%08X ",(decode & 0xFFF80000) >> 8);

	switch((decode >> 16) & 7)
	{
		case 0: offset = 2047;		break;
		case 1: offset = 8191;		break;
		case 2: offset = 16383;		break;
		case 3: offset = 65535;		break;
		case 4: offset = 131071;	break;
		case 5: offset = 262143;	break;
		case 6: offset = 524287;	break;
		case 7: offset = 1048575;	break;
	}
	printf("to %08X ",((decode & 0xFFF80000 ) >> 8) + offset);
	if(decode & 0x8000)
		printf("SYNC  ");
	else
		printf("ASYNC ");

	switch((decode >> 13) & 3)
	{
		case 0: printf("Off  "); break;
		case 1: printf("Low  "); break;
		case 2: printf("Up   "); break;
		case 3: printf("Both "); break;
	}

	switch((decode >> 11) & 3)
	{
		case 0: printf("BAD  "); break;
		case 1: printf("Read "); break;
		case 2: printf("Wrte "); break;
		case 3: printf("Both "); break;
	}

	if(decode & 0x0400)
		printf("DS ");
	else
		printf("AS ");

	switch((decode >> 6) & 15)
	{
		case 0: 
		case 1: 
		case 2: 
		case 3: 
		case 4: 
		case 5: 
		case 6: 
		case 7: 
		case 8: 
		case 9: 
		case 10:
		case 11:
		case 12:
		case 13: printf("%2dW ",(decode >> 6) & 15); break;
		case 14: printf("FST "); break;
		case 15: printf("EXT "); break;
	}

	switch ((decode >> 04) & 3)
	{
		case 0: printf("CPU "); break;
		case 1: printf("USR "); break;
		case 2: printf("SUP "); break;
		case 3: printf("S/U "); break;
	}

	if(decode & 1)
		printf("AVEC %d\n",(decode >> 1) & 7);
	else
		printf("\n");
} 

void	show_csel(void)
{
	uint	v;

	v = (*(uint *)(0xfffa44));

	show_fun(v >> 16,0xFFFa48+0, "CSB ");
	show_fun(v >> 18,0xFFFA48+4, "CS0 ");
	show_fun(v >> 20,0xFFFA48+8, "CS1 ");
	show_fun(v >> 22,0xFFFA48+12,"CS2 ");
	show_fun(v >> 24,0xFFFA48+16,"CS3 ");
	show_fun(v >> 26,0xFFFA48+20,"CS4 ");
	show_fun(v >> 28,0xFFFA48+24,"CS5 ");
	show_fun(v,      0xFFFA48+28,"CS6 ");
	show_fun(v >> 2, 0xFFFA48+32,"CS7 ");
	show_fun(v >> 4, 0xFFFA48+36,"CS8 ");
	show_fun(v >> 6, 0xFFFA48+40,"CS9 ");
	show_fun(v >> 8, 0xFFFA48+44,"CS10");
}
