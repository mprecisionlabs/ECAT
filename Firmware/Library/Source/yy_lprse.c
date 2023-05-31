#include <stdio.h>
#include "parser.h"

extern P_PARAMT	params[];

int	local_parse(
TOKEN	*tokens,
const P_ELEMENT	*e,
char	**remaining_buffer)
{
	int	i,j;
	int	token_pos;
	int	parameter_count;
	char	*template_ptr;							/* Template pointer */

	for(i = 0; e[i].s; i++)
	{
		template_ptr = e[i].s;
		token_pos = 0;
		parameter_count = 0;
		while(template_ptr)
		{	/*Check one template */
			if(tokens[token_pos].type == YY_COMMA)
				token_pos++;
			while(*template_ptr == ' ')
				template_ptr++;
			if(*template_ptr == '?')
			{
				if(tokens[token_pos].type == YY_QUEST)
				{
					template_ptr++;
					token_pos++;
				}
				else
					template_ptr = NULL;
			}
			else
				if(*template_ptr == '%')
					switch(toupper(*(++template_ptr)))
					{
						case 'C':			/* Check for a single char word */
							if((tokens[token_pos].type == YY_WORD) &&
								(tokens[token_pos].data.str[1]=='\0'))
							{
								params[parameter_count++].chr = tokens[token_pos++].data.str[1];
								template_ptr++;			/* At 'C' +1 */
							}
							else
								template_ptr = NULL;
							break;
						case 'W':
							if(tokens[token_pos].type == YY_WORD)
							{
								params[parameter_count++].str = tokens[token_pos++].data.str;
								while(*template_ptr>' ')
									template_ptr++;
							}
							else
								template_ptr = NULL;
							break;
						case 'I':
						case 'D':
							if(tokens[token_pos].type == YY_NUMBER)
							{
								params[parameter_count++].idata = (int)tokens[token_pos++].data.f;
								template_ptr++;
							}
							else
								template_ptr = NULL;
							break;
						case 'F':
							if(tokens[token_pos].type == YY_NUMBER)
							{
								params[parameter_count++].f = tokens[token_pos++].data.f;
								template_ptr++;
							}
							else
								template_ptr = NULL;
							break;
						case 'B':
							params[parameter_count++].str = *remaining_buffer;
							template_ptr++;
							return(e[i].f(params));
						case 'S':
							if(tokens[token_pos].type == YY_STRING)
							{
								params[parameter_count++].str = tokens[token_pos++].data.str;
								template_ptr++;
							}
							else
								template_ptr = NULL;
							break;
					}	/* End of Switch */
				else
					if(*template_ptr == '\0')
						if(e[i].f)
							return(e[i].f(params));
						else
							return(UNKNOWNCMD);
					else
					{	/* Check a command word */
						if((tokens[token_pos].type==YY_WORD) || (tokens[token_pos].type==YY_COMCMD))
						{
							j = 0;
							while((toupper(template_ptr[j]) == toupper(tokens[token_pos].data.str[j])) &&
									(template_ptr[j] != '\0') && (template_ptr[j] != ' ') &&
									(template_ptr[j] != '?')  && (tokens[token_pos].data.str[j]!='\0'))
								j++;
							if((tokens[token_pos].data.str[j] == '\0') &&
								((template_ptr[j] == '\0') || (template_ptr[j] == ' ')  ||
								(template_ptr[j] == '?') || (islower(template_ptr[j]))))
							{	/* Found a plain token */
								token_pos++;
								template_ptr += j;
								while((*template_ptr > ' ') && (*template_ptr != '?'))
									template_ptr++;
							}
							else
								if((template_ptr[j] == '%') && (toupper(template_ptr[j+1]) == 'C'))
								{	/* We have to handle %c */
									params[parameter_count++].chr = tokens[token_pos].data.str[j];
									j++;
									while((tokens[token_pos].data.str[j] == template_ptr[j+1]) &&
											(tokens[token_pos].data.str[j]))
										j++;
									if((tokens[token_pos].data.str[j] == '\0') && (!islower(template_ptr[j+1])))
									{	/* Found a match with %c */
										template_ptr += j+1;
										token_pos++;
									}
									else	/* The word did not match */
										template_ptr = NULL;
								}
								else
									template_ptr = NULL;
						}
						else
							template_ptr = NULL;
					}
		}  /* Check one template */
	}
	return(UNKNOWNCMD);
}

