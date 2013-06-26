%option prefix="escen_"
%option yylineno
%option bison-bridge
%option reentrant
%option noyywrap
%option bison-locations
%x SCRIPT_STATE

%{
#include <stdio.h>
#include "Escen.h"
#include "escen_private.h"
#include "escen_lib_libescen_la-escen_grammar.h"

#define YY_USER_ACTION { yylloc->first_line = yylineno; yylloc->last_line = yylineno; }

%}

_operator [\[\]{}.,;=\:\(\)]

%%

\<!		BEGIN(SCRIPT_STATE);

<SCRIPT_STATE>[^!\>]*		{
	yylval_param->s = strdup(yytext);
	printf("script data found = %s\n", yylval_param->s);
	return SCRIPT_DATA;
}
<SCRIPT_STATE>!\>	BEGIN(INITIAL);

TRUE			{
	yylval_param->boolean = EINA_TRUE;
	return T_TRUE;
}
FALSE			{
	yylval_param->boolean = EINA_FALSE;
	return T_FALSE;
}

state			return STATE;
default			return DEFAULT;

proxy			return PROXY;

set			return SET;

animate			return ANIMATE;
property		return PROPERTY;
value			return VALUE;
time			return TIME;
type			return TYPE;
key			return KEY;
repeat			return REPEAT;
wait			return WAIT;

namespace		return NAMESPACE;

DISCRETE		{
	yylval_param->atype = ETCH_INTERPOLATOR_DISCRETE;
	return DISCRETE;
}
LINEAR			{
	yylval_param->atype = ETCH_INTERPOLATOR_LINEAR;
	return LINEAR;
}
COSIN			{
	yylval_param->atype = ETCH_INTERPOLATOR_COSIN;
	return COSIN;
}
QUADRATIC		{
	yylval_param->atype = ETCH_INTERPOLATOR_QUADRATIC;
	return QUADRATIC;
}
CUBIC			{
	yylval_param->atype = ETCH_INTERPOLATOR_CUBIC;
	return CUBIC;
}


script			return SCRIPT;

escen			return ESCEN;

0[xX][a-fA-F0-9]+ {
	yylval_param->d = strtod(yytext, NULL);
	return NUMBER;
}


[+-]?[0-9]+"."?[0-9]* {
	yylval_param->d = strtod(yytext, NULL);
	return NUMBER;
}

(\"[^\"]*\")* {
	char *tmp = NULL;
	int i = 0;

	yylval_param->s = (char *)calloc(yyleng - 1, sizeof(char));

	for (i = 0; i < yyleng; i++) {
		if (yytext[i] == '"') {
			if (tmp != NULL) {
				yytext[i] = '\0';
				strcat(yylval_param->s, tmp);
				tmp = NULL;
			}
			continue;
		}

		if (tmp == NULL)
			tmp = yytext + i;
	}
	if (tmp != NULL)
	    strcat(yylval_param->s, tmp);

	return STRING;
}

[a-zA-Z0-9_.]*		{ yylval_param->s = strdup(yytext); return T_WORD; }
{_operator}		{ return *yytext; }
\n                      /* ignore end of line */
[ \t]+                  /* ignore whitespace */;
%%
