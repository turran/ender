%option prefix="ender_"
%option yylineno

%{
#include <stdio.h>
#include "libender_la-ender_grammar.h"
%}

%%
(\"[^\"]*\")*       {{
                        char *tmp = NULL;
                        int i = 0;

                        ender_lval = (char *)calloc(ender_leng - 1, sizeof(char));

                        for(i = 0; i < ender_leng; i++) {
                            if (ender_text[i] == '"') {
                                if (tmp != NULL) {
                                    ender_text[i] = '\0';
                                    strcat(ender_lval, tmp);
                                    tmp = NULL;
                                }
                                continue;
                            }

                            if (tmp == NULL)
                                tmp = ender_text + i;
                        }
                        if (tmp != NULL)
                            strcat(ender_lval, tmp);

			    return STRING;
                    }}                           
unsigned		return UNSIGNED;
int			return INT;
color			return COLOR;
double			return DOUBLE;
coord			return COORD;
float			return FLOAT;
surface			return SURFACE;
path			return PATH;
string			return STRING;
namespace		return NAMESPACE;
class			return CLASS;
abstract		return ABSTRACT;
[a-zA-Z0-9_]*            ender_lval = strdup(ender_text); return WORD;
\=                      return EQUAL;
\"                      return QUOTE;
\{                      return OBRACE;
\}                      return EBRACE;
;                       return SEMICOLON;
:                       return COLON;
\.			return DOT;
\[			return OBRACKET;
\]			return EBRACKET;
,			return COMMA;
\n                      /* ignore end of line */
[ \t]+                  /* ignore whitespace */;
%%
