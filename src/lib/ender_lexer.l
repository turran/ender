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

%%
rel			return REL;
uint			return UINT;
int			return INT;
argb			return ARGB;
double			return DOUBLE;
coord			return COORD;
surface			return SURFACE;
path			return PATH;
string			return STRING;
matrix			return MATRIX;
namespace		return NAMESPACE;
ender			return ENDER;
class			return CLASS;
abstract		return ABSTRACT;
using			return USING;
[a-zA-Z0-9_]*           { yylval_param->s = strdup(yytext); return WORD; }
\n                      /* ignore end of line */
[ \t]+                  /* ignore whitespace */;
%%
