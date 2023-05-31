#include "key.h"

char	KEY_getchar(void)
{
	int	i;

	i = KEY_getraw();
	switch (i)
	{
		case 3 : return 'a'; break;
		case 2 : return 'b'; break;
		case 5 : return 'c'; break;
		case 1 : return 'd'; break;
		case 0 : return 'e'; break;
		case 7 : return 'f'; break;
		case 14: return 'g'; break;
		case 4 : return 'X'; break;
		case 6 : return 'G'; break;
		case 13: return 'S'; break;
		case 12: return 'R'; break;
		case 11: return 'M'; break;
		case 18: return '7'; break;
		case 29: return '8'; break;
		case 26: return '9'; break;
		case 19: return '4'; break;
		case 30: return '5'; break;
		case 27: return '6'; break;
		case 20: return '1'; break;
		case 16: return '2'; break;
		case 28: return '3'; break;
		case 21: return '.'; break;
		case 17: return '0'; break;
		case 37: return '\r';break;
		case 10: return '^'; break;
		case  9: return 'v'; break;
		case 22: return '>'; break;
		case  8: return '<'; break;
		case 25: return ','; break;
		case 24: return '-'; break;
		case 38: return 'C'; break;
		default: return 0xFF;break;
	}  
}
