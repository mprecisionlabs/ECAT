#include <stdio.h>
#include "parser.h"

void	show_token(
TOKEN	t)
{
	switch(t.type)
	{
		case YY_NONE:
			printf("Token=NONE\n");
			break;
		case YY_END:
			printf("Token=END\n");
			break;
		case YY_SEMI:
			printf("Token=SEMI\n");
			break;
		case YY_ERROR:
			printf("Token=ERROR#%d\n",t.data.idata);
			break;
		case YY_COLON:
			printf("Token=COLON\n");
			break;
		case YY_COMMA:
			printf("Token=COMMA\n");
			break;
		case YY_QUEST:
			printf("Token=QUEST\n");
			break;
		case YY_WORD:
			printf("Token=WORD '%s'\n",t.data.str);
			break;
		case YY_COMCMD:
			printf("Token=COMCMD '%s'\n",t.data.str);
			break;
		case YY_STRING:
			printf("Token=STRING '%s'\n",t.data.str);
			break;
		case YY_NUMBER:
			printf("Token=NUMBER =%f\n",t.data.f);
			break;
	}
}
