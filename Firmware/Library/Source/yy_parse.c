#include <stdio.h>
#include <float.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"

P_PARAMT	params[MAXPARAMS];

/*
 * The parser works as follows:
 *
 * when called the first time:
 * It clears the command_pos varible to 1;
 * Sets last_token to none;
 * Then
 * It does lexical analisys of the input until:
 *    1)It hits end.
 *       It then parses all of the stored lexical elements and returns;
 *
 *    2)It hits ;
 *       It parses all of the stored lexical elements and keeps parsing.
 *       the command pos variable is not effected.
 *
 *    3)It hits :
 *       IF the last token was: WORD then
 *       {
 *          if command_level+':'+WORD a valid command set command_level.
 *          else error(unknown command level)
 *       }
 *       If the last token was NONE or SEMI then set command_level to 1
 *
 */

int	parse(
char	*b,
const PE_LIST	*p_table)
{
	int	command_level = 1;
	char	*p_start;				/* The begining of the buffer we are scanning*/
	char	*p_end;					/* The end of the buffer we are scanning		*/
	TOKEN	tokens[MAXTOKENS];	/* The list of tokens								*/
	int	last_token;				/* The index into the token list					*/
	int	error_level = 0;		/* The error level									*/
	int	matched_length;		/* Used to hold the length of the last match	*/
	int	last_match;				/* Last entry to match								*/
	int	i,j,k;					/* General pourpose variable						*/

	p_end = p_start = b;

	do
	{
      last_token = -1;
      do		/* Get tokens */
      {
			tokens[++last_token] = lex(p_start,&p_end);
			p_start = p_end;
			if(tokens[last_token].type == YY_ERROR)
				error_level = tokens[last_token].data.idata;
		}		/*Get tokens */
		while(tokens[last_token].type > YY_COLON);

		if((tokens[last_token].type==YY_END) || (tokens[last_token].type == YY_SEMI))
		{
			if(tokens[0].type == YY_COMCMD)
				error_level |= local_parse(tokens,p_table[0].ptable,&p_start);
			else
				error_level |= local_parse(tokens,p_table[command_level].ptable,&p_start);
		}
		else
			if(tokens[last_token].type == YY_COLON)
			{
				if(last_token == 0)
					command_level = 1;
				else
					if(tokens[last_token-1].type == YY_WORD)
					{
						matched_length = 0;
						last_match = 0;
						for(i = 1; p_table[i].ptable != NULL; i++)
						{	/* for all commands in list */
							for(j = 0; ((p_table[command_level].level[j]) &&
								(p_table[command_level].level[j] == p_table[i].level[j])); j++)
								;
							if((p_table[command_level].level[j] == '\0') &&
								(p_table[i].level[j] == ':'))
							{	/* Matched prefix */
								k = 0;
								j++;
								while(toupper(p_table[i].level[j]) == toupper(tokens[last_token-1].data.str[k]))
								{
									j++;
									k++;
								}
								if(p_table[i].level[j] != '\0')	/* Check for more UPPERS */
									for(; p_table[i].level[j]; j++)
										if(isupper(p_table[i].level[j]))
											k = 0;
								if(k == matched_length)
									last_match = 0;					/* Non unique */
								else
									if(k > matched_length)
									{
										last_match = i;
										matched_length = k;
									}
							}	/* Matched prefix */
						}	/* for all commands in list */
						if(last_match)
							command_level = last_match;
						else
						{
							tokens[last_token].type = YY_ERROR;
							error_level = tokens[last_token].data.idata = BAD_COMMAND_LEVEL;
						}
					}
			}
			else
				if(tokens[last_token].type != YY_ERROR)
				{
					tokens[last_token].type = YY_ERROR;
					error_level = tokens[last_token].data.idata = BAD_COMMAND_LEVEL;
				}
	}	/*Parse buffer */
	while((tokens[last_token].type!=YY_END) && (tokens[last_token].type!=YY_ERROR));
	return(error_level);
}
