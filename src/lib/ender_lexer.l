%option prefix="ender_"
%option yylineno
%x using

%{
#include <stdio.h>
#include "Ender.h"
#include "ender_private.h"
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
using			BEGIN(using);
<using>[ \t]*		/* eat the whitespace */
<using>[^ \t\n]+	{{
				/* discard the semicolon */
				if (yytext[strlen(yytext) - 1] == ';')
					yytext[strlen(yytext) - 1] = '\0';
				ender_in = ender_parser_locate(yytext);
				if (!ender_in)
				{
					// yyerminate?
				}
				/* got the include file name */
				DBG("Including %s", yytext);
				yypush_buffer_state(yy_create_buffer(ender_in, YY_BUF_SIZE));
				BEGIN(INITIAL);
			}}
<<EOF>>			{{
				yypop_buffer_state();
				if (!YY_CURRENT_BUFFER)
				{
					yyterminate();
				}
			}}
[a-zA-Z0-9_]*           ender_lval = strdup(ender_text); return WORD;
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
