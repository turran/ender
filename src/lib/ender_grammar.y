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

void ender_error(void *lloc, void *scanner, Ender_Parser *parser, const char *str);

static inline void _on_using(Ender_Parser *parser, Eina_List *list)
{
	Eina_List *l;
	char *str;

	if (!parser->descriptor->on_using)
		return;
	EINA_LIST_FOREACH (list, l, str)
	{
		parser->descriptor->on_using(parser->data, str);
	}
}

static inline void _on_namespace(Ender_Parser *parser, const char *name, int version)
{
	if (parser->descriptor->on_namespace)
		parser->descriptor->on_namespace(parser->data, name, version);
}

static inline void _on_object(Ender_Parser *parser, const char *name, Ender_Descriptor_Type type, const char *parent)
{
	if (parser->descriptor->on_object)
		parser->descriptor->on_object(parser->data, name, type, parent);
}

static inline void _on_property(Ender_Parser *parser, const char *name, const char *alias,
		Eina_Bool relative, Ender_Container *container)
{
	if (parser->descriptor->on_property)
		parser->descriptor->on_property(parser->data, name, alias, relative, container);
}

static inline void _on_function(Ender_Parser *parser, const char *name,
		const char *alias, Ender_Container *ret, Eina_List *args)
{
	if (parser->descriptor->on_function)
		parser->descriptor->on_function(parser->data, name, alias, ret, args);
}

static inline void _on_container(Ender_Parser *parser, const char *name,
		Ender_Container *container)
{
	if (parser->descriptor->on_container)
		parser->descriptor->on_container(parser->data, name, container);
}

%}

%union {
	Ender_Descriptor_Type dtype;
	Ender_Value_Type vtype;
	Ender_Container *container;
	Ender_Parser_Type *type;
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
%token <vtype> T_STRUCT
%token <vtype> T_UNION
%token <vtype> T_OBJECT
%token <vtype> T_ENDER
%token <dtype> T_ABSTRACT
%token <dtype> T_CLASS
%token <number> T_NUMBER
%token T_NAMESPACE
%token T_REL
%token T_USING
%token <s> T_WORD
%token <s> T_INLINE_STRING
%type <list>using
%type <b> type_relative
%type <container> type_specifier
%type <vtype> basic_type
%type <dtype> definition
%type <container> list_type
%type <container> struct_union_type
%type <s> object_inheritance
%type <list> types
%type <list> function_args
%type <type> type
%type <type> extended_type
%type <s> alias

%%

main
	: using
	{
		char *name;

		_on_using(parser, $1);
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
		_on_namespace(parser, $2, $4);
		free($2);
	}
	'{' definition_list '}' ';'
	;

definition_list
	:
	| struct definition_list
	| union definition_list
	| object definition_list
	;

struct
	: T_STRUCT T_INLINE_STRING '{' types '}' ';'
	{
		Ender_Container *c;
		Ender_Parser_Type *t;
		Eina_List *l;
		Eina_List *ll;

		c = ender_container_new(ENDER_STRUCT);
		EINA_LIST_FOREACH_SAFE($4, l, ll, t)
		{
			ender_container_add(c, t->name, t->container);
			free(t->name);
			free(t);
 			$4 = eina_list_remove_list($4, l);
		}
		ender_container_register(c, $2);
		_on_container(parser, $2, c);
		free($2);
	}
	;

union
	: T_UNION T_INLINE_STRING '{' types '}' ';'
	{
		Ender_Container *c;
		Ender_Parser_Type *t;
		Eina_List *l;
		Eina_List *ll;

		c = ender_container_new(ENDER_UNION);
		EINA_LIST_FOREACH_SAFE($4, l, ll, t)
		{
			ender_container_add(c, t->name, t->container);
			free(t->name);
			free(t);
 			$4 = eina_list_remove_list($4, l);
		}
		ender_container_register(c, $2);
		_on_container(parser, $2, c);
		free($2);
	}
	;

object
	: definition T_INLINE_STRING object_inheritance
	{
		_on_object(parser, $2, $1, $3);
		if ($3)
			free($3);
		free($2);
	}
	'{' declaration_list '}' ';'
	;


definition
	: T_CLASS { $$ = $1; }
	| T_ABSTRACT { $$ = $1; }
	;

object_inheritance
	: { $$ = NULL; }
	| ':' T_INLINE_STRING { $$ = $2; }
	;

types
	: type ';' types
	{
		$$ = eina_list_prepend($3, $1);
	}
	| type ';'
	{
		$$ = eina_list_append(NULL, $1);
	}
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

struct_union_type
	: T_WORD
	{
		$$ = ender_container_find($1);
		free($1);
	}
	;

list_type
	: '[' type_specifier ']'
	{
		$$ = ender_container_new(ENDER_LIST);
		ender_container_add($$, NULL, $2);
	}
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
	: basic_type { $$ = ender_container_new($1); }
	| struct_union_type { $$ = $1; }
	| list_type { $$ = $1; }
	;

type_relative
	: T_REL { $$ = EINA_TRUE; }
	;

alias
	: ',' T_INLINE_STRING { $$ = $2; }
	| { $$ = NULL; }
	;

extended_type
	: type alias
	{
		$$ = $1;
		$$->alias = $2;
	}
	;

type
	: type_specifier T_INLINE_STRING
	{
		Ender_Parser_Type *type = malloc(sizeof(Ender_Parser_Type));

		type->container = $1;
		type->name = $2;
		type->alias = NULL;

		$$ = type;
	}
	;

property
	: type_relative extended_type constraint ';'
	{
		_on_property(parser, $2->name, $2->alias, EINA_TRUE, $2->container);
		free($2->name);
			free($2->alias);
		free($2);
	}
	| extended_type constraint ';'
	{
		_on_property(parser, $1->name, $1->alias, EINA_FALSE, $1->container);
		free($1->name);
		if ($1->alias)
			free($1->alias);
		free($1);
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
		_on_function(parser, $1, $2, NULL, $4);
	}
	| extended_type '(' function_args ')' ';'
	{
		_on_function(parser, $1->name, $1->alias, $1->container, $3);
		free($1->name);
		if ($1->alias)
			free($1->alias);
		free($1);
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
	ERR("Parsing error at %s %d: %d.%d %s", parser->file, lloc->last_line, lloc->first_column, lloc->last_column, str);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_parser_parse(const char *file,
		Ender_Parser_Descriptor *descriptor, void *data)
{
	Ender_Parser parser;
	void *scanner;
	FILE *f;
	int ret;

	f = fopen(file, "r");
	if (!f) return EINA_FALSE;

	parser.descriptor = descriptor;
	parser.file = file;
	parser.data = data;

	ender_lex_init(&scanner);
	ender_set_in(f, scanner);
	ret = ender_parse(scanner, &parser);

	ender_lex_destroy(scanner);
	fclose(f);

	return !ret;
}

