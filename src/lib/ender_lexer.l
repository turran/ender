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
uint 			{ 
				yylval_param->type = ENDER_UINT32;
				return UINT;
			}
int			{
				yylval_param->type = ENDER_INT32;
				return INT;
			}
argb			{
				yylval_param->type = ENDER_ARGB;
				return ARGB;
			}
double			{
				yylval_param->type = ENDER_DOUBLE;
				return DOUBLE;
			}
coord			return COORD;
surface			{

				yylval_param->type = ENDER_SURFACE;
				return SURFACE;
			}

path			{ return PATH; }
string			{
				yylval_param->type = ENDER_STRING;
				return STRING;
			}
matrix			{
				yylval_param->type = ENDER_MATRIX;
				return MATRIX;
			}
namespace		return NAMESPACE;
ender			return ENDER;
class			return CLASS;
abstract		return ABSTRACT;
using			return USING;
[a-zA-Z0-9_]*           {
				yylval_param->s = strdup(yytext);
				return WORD;
			}
{_operator}		{ return *yytext; }
\n                      /* ignore end of line */
[ \t]+                  /* ignore whitespace */

%%
