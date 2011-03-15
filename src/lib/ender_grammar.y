%name-prefix="ender_"
%error-verbose
%defines
%locations
%pure-parser
%parse-param { void * scanner }
%parse-param { Ender_Parser *parser }
%lex-param { void * scanner }

%{
#include <stdio.h>
#include <string.h>
#include "Ender.h"
#include "ender_private.h"

%}

%union {
	Ender_Property_Type type;
	char *s;
	Eina_Bool b;
}

%token <type> UINT
%token <type> INT
%token <type> ARGB
%token <type> DOUBLE
%token <type> IMAGE
%token <type> SURFACE
%token <type> COORD
%token <type> PATH
%token <type> STRING
%token <type> MATRIX
%token <type> ENDER
%token ABSTRACT CLASS NAMESPACE REL USING
%token <s> WORD
%type <b> type_relative
%type <type> type_specifier
%type <s>namespace_list
%type <s>namespace_name
%type <s>namespace
%type <s>renderer_list
%type <s>definition
%type <s>renderer
%type <s>renderer_inheritance
%type <s>types
%type <s>declaration
%type <s>declaration_list

%%

using
	:
	| USING WORD ';' using
	;

namespace_list
	:
	| namespace namespace_list
	;

namespace_name
	:
	| '.' WORD namespace_name
	;

namespace
	: NAMESPACE WORD namespace_name '{' renderer_list '}' ';'
	;

renderer_list
	:
	| renderer renderer_list
	;

definition
	: CLASS
	| ABSTRACT
	;

renderer
	: definition WORD renderer_inheritance '{' declaration_list '}' ';'
	;

renderer_inheritance
	:
	| ':' WORD
	;

types
	: type_specifier ',' types
	| type_specifier
	;

type_specifier
	: UINT { $$ = $1 }
	| INT { $$ = $1 }
	| ARGB
	| DOUBLE
	| STRING
	| SURFACE
	| WORD
	| ENDER
	| MATRIX
	| '[' types ']'
	;

type_relative
	: {$$=EINA_FALSE; }
	| REL { $$=EINA_TRUE; }
	;

declaration
	: 
	type_relative type_specifier WORD ';'
	;

declaration_list
	:
	| declaration declaration_list
	;

main
	: using namespace_list
	;

%%

void ender_error(YYLTYPE *lloc, Ender_Parser *parser, const char *str)
{
        ERR("Parsing error (%d) %s", lloc->last_line, str);
}
