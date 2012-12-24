%name-prefix="ender_"
%defines
%error-verbose
%locations
%pure-parser
%parse-param { void * scanner }
%parse-param { Ender_Parser *parser }
%lex-param { void * scanner }

%{
#include "Ender.h"
#include "ender_private.h"
#include <stdio.h>
#include <string.h>

static void _ender_grammar_type_free(Ender_Grammar_Type *t)
{
	if (t->name)
		free(t->name);
	if (t->alias)
		free(t->alias);
	if (t->container)
		ender_parser_container_free(t->container);
}

static void _ender_grammar_function_args_free(Eina_List *l)
{
	Ender_Parser_Container *c;
	EINA_LIST_FREE(l, c)
	{
		ender_parser_container_free(c);
	}
}

void ender_error(void *lloc, void *scanner, Ender_Parser *parser, const char *str);

%}

%union {
	Ender_Grammar_Type *type;
	Ender_Parser_Container *container;
	Ender_Descriptor_Type dtype;
	Ender_Value_Type vtype;
	char *s;
	Eina_Bool b;
	Eina_List *list; // use this for every _list nonterminal
	int number;
}

%token <vtype> T_BOOL
%token <vtype> T_UINT32
%token <vtype> T_INT32
%token <vtype> T_UINT64
%token <vtype> T_INT64
%token <vtype> T_COLOR
%token <vtype> T_ARGB
%token <vtype> T_DOUBLE
%token <vtype> T_IMAGE
%token <vtype> T_SURFACE
%token <vtype> T_STRING
%token <vtype> T_MATRIX
%token <vtype> T_OBJECT
%token <vtype> T_ENDER
%token <dtype> T_ABSTRACT
%token <dtype> T_CLASS
%token <dtype> T_STRUCT
%token <dtype> T_UNION
%token <number> T_NUMBER
%token T_NAMESPACE
%token T_REL
%token T_USING
%token <s> T_WORD
%token <s> T_INLINE_STRING
%type <dtype>descriptor_type
%type <list>using
%type <b> type_relative
%type <container> type_specifier
%type <vtype> basic_type
%type <container> list_type
%type <s> defined_type
%type <s> object_inheritance
%type <list> function_args
%type <type> type
%type <s> alias

%%

main
	: using
	{
		char *name;

		ender_parser_add_using(parser, $1);
		EINA_LIST_FREE($1, name)
			free(name);
	}
	namespace_list
	;

using
	: { $$ = NULL; }
	| T_USING T_INLINE_STRING ':' T_NUMBER ';' using { $$ = eina_list_append($6, $2); }
	;

namespace_list
	:
	| namespace namespace_list
	;

namespace
	: T_NAMESPACE T_INLINE_STRING ':' T_NUMBER
	{
		ender_parser_add_namespace(parser, $2, $4);
		free($2);
	}
	'{' definition_list '}' ';'
	;

descriptor_type
	: T_CLASS { $$ = $1; }
	| T_ABSTRACT { $$ = $1; }
	| T_STRUCT { $$ = $1; }
	| T_UNION { $$ = $1; }
	;

descriptor
	: descriptor_type T_INLINE_STRING alias object_inheritance
	{
		ender_parser_add_native(parser, $2, $3, $1, $4);
		if ($3)
			free($3);
		free($2);
	}
	'{' declaration_list '}' ';'
	;


definition_list
	:
	| descriptor definition_list
	;

object_inheritance
	: { $$ = NULL; }
	| ':' T_INLINE_STRING { $$ = $2; }
	;

basic_type
	: T_BOOL { $$ = $1; }
	| T_UINT32 { $$ = $1; }
	| T_INT32 { $$ = $1; }
	| T_UINT64 { $$ = $1; }
	| T_INT64 { $$ = $1; }
	| T_COLOR { $$ = $1; }
	| T_ARGB { $$ = $1; }
	| T_DOUBLE { $$ = $1; }
	| T_STRING { $$ = $1; }
	| T_SURFACE { $$ = $1; }
	| T_OBJECT { $$ = $1; }
	| T_ENDER { $$ = $1; }
	| T_MATRIX { $$ = $1; }
	;

defined_type
	: T_WORD { $$ = $1; }
	;

list_type
	: '[' type_specifier ']' { $$ = $2; }
	;

constraint_range
	:
	;

constraint_enum
	: T_INLINE_STRING
	| T_INLINE_STRING ',' constraint_enum
	;

constraint_definition
	: constraint_enum
	| constraint_range
	;

constraint
	: '[' constraint_definition ']'
	|
	;

type_specifier
	: basic_type
	{
		$$ = calloc(1, sizeof(Ender_Parser_Container));
		$$->type = $1;
	}
	| defined_type
	{
		$$ = calloc(1, sizeof(Ender_Parser_Container));
		$$->type = ENDER_VALUE;
		$$->defined = $1;
	}
	| list_type
	{
		$$ = calloc(1, sizeof(Ender_Parser_Container));
		$$->type = ENDER_LIST;
		$$->subcontainers = eina_list_append(NULL, $1);
	}
	;

type_relative
	: T_REL { $$ = EINA_TRUE; }
	;

alias
	: ',' T_INLINE_STRING { $$ = $2; }
	| { $$ = NULL; }
	;

type
	: type_specifier T_INLINE_STRING alias
	{
		$$ = calloc(1, sizeof(Ender_Grammar_Type));
		$$->container = $1;
		$$->name = $2;
		$$->alias = $3;
	}
	;

property
	: type_relative type constraint ';'
	{
		Ender_Parser_Property p;
		p.rel = EINA_TRUE;
		p.def.name = $2->name;
		p.def.alias = $2->alias;
		p.container = $2->container;
		ender_parser_add_property(parser, &p);
		_ender_grammar_type_free($2);
	}
	| type constraint ';'
	{
		Ender_Parser_Property p;
		p.rel = EINA_FALSE;
		p.def.name = $1->name;
		p.def.alias = $1->alias;
		p.container = $1->container;
		ender_parser_add_property(parser, &p);
		_ender_grammar_type_free($1);
	}
	;

function_args
	: { $$ = NULL; }
	| type_specifier ',' function_args { $$ = eina_list_append($3, $1); }
	| type_specifier { $$ = eina_list_append(NULL, $1); }
	;

function
	: T_INLINE_STRING alias '(' function_args ')' ';'
	{
		Ender_Parser_Function f;
		f.def.name = $1;
		f.def.alias = $2;
		f.ret = NULL;
		f.args = $4;
		ender_parser_add_function(parser, &f);
		_ender_grammar_function_args_free($4);
	}
	| type '(' function_args ')' ';'
	{
		Ender_Parser_Function f;
		f.def.name = $1->name;
		f.def.alias = $1->alias;
		f.ret = $1->container;
		f.args = $3;
		ender_parser_add_function(parser, &f);
		_ender_grammar_type_free($1);
		_ender_grammar_function_args_free($3);
	}
	;

declaration
	: property
	| function
	;

declaration_list
	:
	| declaration declaration_list
	;
%%

void ender_error(void *data, void *scanner, Ender_Parser *parser, const char *str)
{
	YYLTYPE *lloc = (YYLTYPE *)data;
	ender_parser_error(parser, str, lloc->last_line, lloc->first_column);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
