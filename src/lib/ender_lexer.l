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
				yylval_param->ptype = ENDER_UINT32;
				return UINT;
			}
int			{
				yylval_param->ptype = ENDER_INT32;
				return INT;
			}
argb			{
				yylval_param->ptype = ENDER_ARGB;
				return ARGB;
			}
double			{
				yylval_param->ptype = ENDER_DOUBLE;
				return DOUBLE;
			}
surface			{

				yylval_param->ptype = ENDER_SURFACE;
				return SURFACE;
			}

string			{
				yylval_param->ptype = ENDER_STRING;
				return STRING;
			}
matrix			{
				yylval_param->ptype = ENDER_MATRIX;
				return MATRIX;
			}
ender			{
				yylval_param->ptype = ENDER_ENDER;
				return ENDER;
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
[a-zA-Z0-9_]*           {
				yylval_param->s = strdup(yytext);
				return WORD;
			}
{_operator}		{ return *yytext; }
\n                      /* ignore end of line */
[ \t]+                  /* ignore whitespace */

%%
