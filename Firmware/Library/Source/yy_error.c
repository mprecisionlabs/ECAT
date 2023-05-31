#include <stdio.h>
#include "parser.h"

void	report_errors(
FILE	*destination,
int	err)
{
	if(err)
	{
		fputs("(ERR)-",destination);
		if(err & BADCHAR)
			fputs("CHAR",destination);
		if(err & BADNUM)
			fputs("NUMBER",destination);
		if(err & UNTERM_STRING)
			fputs("STRING",destination);
		if(err & UNKNOWNCMD)
			fputs("COMMAND",destination);
		if(err & BAD_VALUE)
			fputs("VALUE",destination);
		if(err & BAD_COMMAND_LEVEL)
			fputs("CMD_LEVEL",destination);
	}
}
