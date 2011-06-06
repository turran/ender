%name-prefix="ender_"
%defines
%error-verbose
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
	Ender_Type etype;
	Ender_Value_Type vtype;
	Ender_Container *prop;
	Ender_Descriptor *descriptor;
	char *s;
	Eina_Bool b;
	Eina_List *list; // use this for every _list nonterminal
}

%token <vtype> BOOL 
%token <vtype> UINT
%token <vtype> INT
%token <vtype> COLOR
%token <vtype> ARGB
%token <vtype> DOUBLE
%token <vtype> IMAGE
%token <vtype> SURFACE
%token <vtype> STRING
%token <vtype> MATRIX
%token <vtype> STRUCT
%token <vtype> RENDERER
%token <vtype> ENDER
%token <etype> ABSTRACT
%token <etype> CLASS
%token NAMESPACE
%token REL
%token USING
%token <s> WORD
%token <s> INLINE_STRING
%type <list>using
%type <b> type_relative
%type <prop> type_specifier
%type <vtype> basic_type
%type <etype> definition
%type <prop> list_type
%type <prop> struct_type
%type <descriptor> renderer_inheritance
%type <list> types
%%

main
	: using
	{
		Eina_List *l;
		char *str;

		EINA_LIST_FOREACH ($1, l, str)
		{
			ender_parser_parse(str);
		}
	}
	namespace_list
	;

using
	: { $$ = NULL; }
	| USING INLINE_STRING ';' using { $$ = eina_list_append($4, $2); }
	;

namespace_list
	:
	| namespace namespace_list
	;

namespace
	: NAMESPACE INLINE_STRING
	{
		parser->lns = ender_parser_namespace_new($2);
	}
	'{' definition_list '}' ';'
	;

definition_list
	:
	| struct definition_list
	| renderer definition_list
	;

struct
	: STRUCT INLINE_STRING '{' types '}' ';'
	{
		Eina_List *l;
		Ender_Container *c, *sub;

		c = ender_container_new(ENDER_STRUCT);
		EINA_LIST_FOREACH ($4, l, sub)
		{
			ender_container_add(c, sub);
		}
		ender_container_register($2, c);
	}
	;

renderer
	: definition INLINE_STRING renderer_inheritance
	{ parser->descriptor = ender_parser_descriptor_new(parser->lns, $2, $3, $1); }
	'{' declaration_list '}' ';'
	;


definition
	: CLASS { $$ = $1; }
	| ABSTRACT { $$ = $1; }
	;

renderer_inheritance
	: { $$ = NULL; }
	| ':' INLINE_STRING { $$ = ender_descriptor_find($2); }
	;

types
	: type_specifier ',' types
	{
		$$ = eina_list_prepend($3, $1);
	}
	| type_specifier
	{
		$$ = eina_list_append(NULL, $1);
	}
	;

basic_type
	: BOOL { $$ = $1; }
	| UINT { $$ = $1; }
	| INT { $$ = $1; }
	| COLOR { $$ = $1; }
	| ARGB { $$ = $1; }
	| DOUBLE { $$ = $1; }
	| STRING { $$ = $1; }
	| SURFACE { $$ = $1; }
	| RENDERER { $$ = $1; }
	| ENDER { $$ = $1; }
	| MATRIX { $$ = $1; }
	;

struct_type
	: WORD { $$ = ender_container_find($1); }
	;

list_type
	: '[' type_specifier ']'
	{
		$$ = ender_container_new(ENDER_LIST);
		ender_container_add($$, $2);
	}
	;

type_specifier
	: basic_type { $$ = ender_container_new($1); }
	| struct_type { $$ = $1; }
	| list_type { $$ = $1; }
	;

type_relative
	: {$$=EINA_FALSE; }
	| REL { $$=EINA_TRUE; }
	;
declaration
	:
	type_relative type_specifier INLINE_STRING ';'
	{
		ender_parser_descriptor_property_add(parser->lns, parser->descriptor, $3, $2, $1);
	}
	;

declaration_list
	:
	| declaration declaration_list
	;
%%

void ender_error(YYLTYPE *lloc, void *scanner, Ender_Parser *parser, const char *str)
{
        ERR("Parsing error at %s %d: %d.%d %s", parser->file, lloc->last_line, lloc->first_column, lloc->last_column, str);
}
