#include <stdio.h>
#include "parser.h"

/*
 *	LEX parses the buffer pointed to by s and returns the apropriate token.
 *	If it experiences a bad token it returns:
 *		token.type = ERROR and
 *		token.data.idata = ERROR#
 */
TOKEN	lex(
char	*s,
char	**end_ptr)
{
	TOKEN	rtoken;
	int	i;
	char	temp_char;
	char	*temp_ptr;
	char	*eptr;

	rtoken.type = YY_ERROR;
	rtoken.data.idata = 0;

	/* Eat leading white space */
	while((*s <= ' ') && (*s != '\n') && (*s != '\0'))(*(s++) = '\0');

	switch(*s)
	{
		case ':':
			*s++ = '\0';
			rtoken.type = YY_COLON;
			break;
		case ';':
			*s++ = '\0';
			rtoken.type = YY_SEMI;
			break;
		case ',':
			*s++ = '\0';
			rtoken.type = YY_COMMA;
			break;
		case '?':
			*s++ = '\0';
			rtoken.type = YY_QUEST;
			break;
		case '\n':
		case '\r':
		case '\0':
			*s = '\0';
			rtoken.type = YY_END;
			break;
		case '"':
		case '\'':				/* String */
			temp_char = (*s);
			*(s++) = '\0';
			eptr = s;
			while((*eptr != temp_char) || (*(eptr+1) == temp_char))
			{
				if(*eptr == '\0')
				{
					rtoken.data.idata = UNTERM_STRING;
					break;
				}
				if((*eptr == temp_char) && (*(eptr+1) == temp_char))
					for(temp_ptr = eptr; *temp_ptr; temp_ptr++)
						*temp_ptr = *(temp_ptr+1);
				eptr++;
			}
			if(*eptr)			/* Not = '\0' */
			{
				*eptr = '\0';
				rtoken.type = YY_STRING;
				rtoken.data.str = s;
				s = eptr+1;
			}
			break;
		case '#':				/* Number */
			*s = '\0';
			s++;
			switch(toupper(*s))
			{ /* # switch */
				case 'H':		/* Hexadecimal */
					eptr = ++s;
					while(isxdigit(*eptr))
						eptr++;
					temp_char = *eptr;
					*eptr = '\0';
					sscanf(s,"%x",&i);
					*eptr = temp_char;
					rtoken.type   = YY_NUMBER;
					rtoken.data.f = (float)i;
					break;
				case 'Q':		/* Octal */
					eptr = ++s;
					while((isdigit(*eptr)) && (*eptr < '8'))
						eptr++;
					temp_char = *eptr;
					*eptr = '\0';
					sscanf(s,"%o",&i);
					*eptr = temp_char;
					rtoken.type   = YY_NUMBER;
					rtoken.data.f = (float)i;
					break;
				case 'B':
					eptr = ++s;
					i = 0;
					while((*eptr == '0') || (*eptr == '1'))
					{
						i = (i*2) + (*eptr-'0');
						eptr++;
					}
					rtoken.type   = YY_NUMBER;
					rtoken.data.f = (float)i;
					break;
				default :
					rtoken.data.idata = BADNUM;
			} /* # Switch */
			s = eptr;
			break;
		case '+':
		case '-':
		case '.':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':						/* Floating point number */
			eptr = s;
			if((*s == '+') || (*s == '-'))
				eptr++;					/* Eat +/- */
			while(isdigit(*eptr))
				eptr++;					/* Optional digits */
			if(*eptr == '.')
				eptr++;					/* Decimal */
			while(isdigit(*eptr))
				eptr++;					/* Optional digits */
			temp_ptr = eptr;
			while((*eptr <= ' ') && (*eptr != '\n'))
				eptr++;					/* White space */
			if(toupper(*eptr) == 'E')
			{								/* We have an exponent */
				eptr++;					/* Skip E */
				while((*eptr <= ' ') && (*eptr != '\n'))
					eptr++;				/* White space */
				if((*eptr == '+') || (*eptr == '-'))
					eptr++;				/* Eat +/- */
				while(isdigit(*eptr))
					eptr++;
			}
			else
				eptr = temp_ptr;
			temp_char = *eptr;
			*eptr = '\0';
			i = sscanf(s,"%f",&rtoken.data.f);
			*eptr = temp_char;
			s = eptr;
			if(i)
				rtoken.type = YY_NUMBER;
			else
				rtoken.data.idata = BADNUM;
			break;
		default:							/* Program Mnemonic */
			if(isalpha(*s) || (*s == '*'))
			{
				eptr = s + 1;
				while(isalnum(*eptr) || (*eptr == '_'))
					eptr++;
				if(*s == '*')
					rtoken.type = YY_COMCMD;
				else
					rtoken.type = YY_WORD;
				rtoken.data.str = s;
				s = eptr;
			}
			else
			{
				rtoken.type = YY_ERROR;
				rtoken.data.idata = BADCHAR;
			}
			break;
	} /* Switch */
	*end_ptr = s;
	return(rtoken);
}

