%option prefix="ender_"
%option yylineno
%option bison-bridge
%option reentrant
%option noyywrap
%option bison-locations

%{
#include <stdio.h>
#include "Ender.h"
#include "ender_private.h"

#define YY_USER_ACTION { yylloc->first_line = yylineno; yylloc->last_line = yylineno; }

#include "libender_la-ender_grammar.h"
#define YY_NO_UNISTD_H
%}

%x C_COMMENT
_operator 		[\[\]{}.,;=\:\(\)]
D			[0-9]

%%

<C_COMMENT>{
	"*/"	{ BEGIN(INITIAL); }
	\n	/* ignore end of line */
	.	/* everything else ignored */
}

rel			return T_REL;
bool			{
				yylval_param->vtype = ENDER_BOOL;
				return T_BOOL;
			}
uint 			{
				yylval_param->vtype = ENDER_UINT32;
				return T_UINT32;
			}
int			{
				yylval_param->vtype = ENDER_INT32;
				return T_INT32;
			}
ulong 			{
				yylval_param->vtype = ENDER_UINT64;
				return T_UINT64;
			}
long			{
				yylval_param->vtype = ENDER_INT64;
				return T_INT64;
			}
argb			{
				yylval_param->vtype = ENDER_ARGB;
				return T_ARGB;
			}
color			{
				yylval_param->vtype = ENDER_COLOR;
				return T_COLOR;
			}
double			{
				yylval_param->vtype = ENDER_DOUBLE;
				return T_DOUBLE;
			}
surface			{

				yylval_param->vtype = ENDER_SURFACE;
				return T_SURFACE;
			}

string			{
				yylval_param->vtype = ENDER_STRING;
				return T_STRING;
			}
matrix			{
				yylval_param->vtype = ENDER_MATRIX;
				return T_MATRIX;
			}
object			{
				yylval_param->vtype = ENDER_OBJECT;
				return T_OBJECT;
			}
ender			{
				yylval_param->vtype = ENDER_ENDER;
				return T_ENDER;
			}
struct			{
				yylval_param->vtype = ENDER_STRUCT;
				return T_STRUCT;
			}
union			{
				yylval_param->vtype = ENDER_UNION;
				return T_UNION;
			}
namespace		return T_NAMESPACE;
class			{
				yylval_param->dtype = ENDER_TYPE_CLASS;
				return T_CLASS;
			}
abstract		{
				yylval_param->dtype = ENDER_TYPE_ABSTRACT;
				return T_ABSTRACT;
			}
using			return T_USING;
[0-9]* 			{
				yylval_param->number = atoi(yytext);
				return T_NUMBER;
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

	return T_INLINE_STRING;
}

[a-zA-Z0-9_]*           {
				yylval_param->s = strdup(yytext);
				return T_WORD;
			}
{_operator}		{ return *yytext; }
\n                      /* ignore end of line */
[ \t]+                  /* ignore whitespace */
"/*"            	{ BEGIN(C_COMMENT); }
.			{ printf("found2 %s\n", yytext); } /* everything else ignored */

%%
