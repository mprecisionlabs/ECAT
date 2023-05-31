#ifndef __PARSER_H			/* Avoid multiple inclusions */
#define __PARSER_H

#include  <shortdef.h>

/*
 * General definitions
 */

#define	FALSE						0
#define	TRUE						1
#define	BADCHAR					1
#define	BADNUM					2
#define	UNTERM_STRING			4
#define	UNKNOWNCMD				8
#define	BAD_VALUE				16
#define	MAXPARAMS				10
#define	MAXTOKENS				12
#define	MAX_ELEMENTS			10
#define	BAD_COMMAND_LEVEL		32

enum	token_types
{
	YY_NONE,			/* The no token token!						*/
	YY_END,			/* \0 or \n program message terminator	*/
	YY_SEMI,			/* ; Program message seperator			*/
	YY_ERROR,		/* any lexical error							*/
	YY_COLON,		/* : Program command separator			*/
	YY_QUEST,		/* ? Query character							*/
	YY_COMMA,		/* , Program data seperator				*/
	YY_WORD,			/* Program mnemonic							*/
	YY_COMCMD,		/* '*' common command						*/
	YY_STRING,		/* String as defined in 7.7.5				*/
	YY_NUMBER,		/* Number as defined in 7.7.2,7.74		*/
};

typedef union paramt
{
	int	idata;
	char	chr;
	char	*str;
	float	f;
} P_PARAMT;

typedef struct token
{
	int		type;
	P_PARAMT	data;
} TOKEN;

typedef struct parse_element
{
	char	*s;							/* Format string */
	int	(*f)(P_PARAMT *p);		/* Function returning error code */
} P_ELEMENT;

typedef struct parse_element_list
{
	char	*level;
	const P_ELEMENT	*ptable;
} PE_LIST;

/*
 * MACRO function definitions
 * The prototypes for the following are:
 *
 *		int	EXTRACT_INT(int n);
 *		float	EXTRACT_FLOAT(int n);
 *		char	*EXTRACT_STRING(int n);
 *		char	EXTRACT_CHAR(int n);
 */

#define	EXTRACT_INT(n)			((int)params[n].idata)
#define	EXTRACT_FLOAT(n)		(params[n].f)
#define	EXTRACT_STRING(n)		(params[n].str)
#define	EXTRACT_CHAR(n)		(params[n].chr)

/*
 * PRIVATE function definitions
 *
 *		show_token	-- Debugging aid to send TOKENs out serial port
 *		local_parse	-- Small parse functions used by parse function
 *		lex			-- Used by parse to extract TOKENs for parser
 */

void	show_token(TOKEN);
TOKEN	lex(char *, char **);
int	local_parse(TOKEN *, const P_ELEMENT *, char **);

/*
 * PUBLIC function prototypes
 *
 *		report_errors	-- Report any errors through the specified destination
 *		parse				-- Parse the GPIB input buffer for command sequences
 */

void	report_errors(FILE *, int);
int	parse(char *, const PE_LIST *);

#endif							/* ifndef __PARSER_H */
