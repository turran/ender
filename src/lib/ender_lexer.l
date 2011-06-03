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
#include "libender_la-ender_grammar.h"
%}

_operator [\[\]{}.,;=\:]

%%

rel			return REL;
bool			{
				yylval_param->vtype = ENDER_BOOL;
				return BOOL;
			}
uint 			{ 
				yylval_param->vtype = ENDER_UINT32;
				return UINT;
			}
int			{
				yylval_param->vtype = ENDER_INT32;
				return INT;
			}
argb			{
				yylval_param->vtype = ENDER_ARGB;
				return ARGB;
			}
color			{
				yylval_param->vtype = ENDER_COLOR;
				return COLOR;
			}
double			{
				yylval_param->vtype = ENDER_DOUBLE;
				return DOUBLE;
			}
surface			{

				yylval_param->vtype = ENDER_SURFACE;
				return SURFACE;
			}

string			{
				yylval_param->vtype = ENDER_STRING;
				return STRING;
			}
matrix			{
				yylval_param->vtype = ENDER_MATRIX;
				return MATRIX;
			}
renderer		{
				yylval_param->vtype = ENDER_RENDERER;
				return RENDERER;
			}
ender			{
				yylval_param->vtype = ENDER_ENDER;
				return ENDER;
			}
struct			{
				yylval_param->vtype = ENDER_STRUCT;
				return STRUCT;
			}
namespace		return NAMESPACE;
class			{
				yylval_param->etype = ENDER_CLASS;
				return CLASS;
			}
abstract		{
				yylval_param->etype = ENDER_ABSTRACT;
				return ABSTRACT;
			}
using			return USING;

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

	return INLINE_STRING;
}

[a-zA-Z0-9_]*           {
				yylval_param->s = strdup(yytext);
				return WORD;
			}
{_operator}		{ return *yytext; }
\n                      /* ignore end of line */
[ \t]+                  /* ignore whitespace */

%%
