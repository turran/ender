%name-prefix="ender_"
%error-verbose
%defines
%locations

%{
#include <stdio.h>
#include <string.h>
#include "Ender.h"
#include "ender_private.h"

#define YYSTYPE char *
#define YYERROR_VERBOSE
extern int ender_lineno;

void ender_error(const char *str)
{
        ERR("Parsing error (%d) %s", ender_lineno, str);
}

int ender_wrap()
{
        return 1;
}

static Ender_Descriptor *current = NULL;
static char *name = NULL;
static char *namespace = NULL;
extern Eina_Array *pproperties;

%}

%token UINT INT ARGB DOUBLE IMAGE SURFACE COORD PATH STRING MATRIX ENDER
%token WORD OBRACE EBRACE SEMICOLON EQUAL QUOTE COLON DOT OBRACKET EBRACKET COMMA
%token ABSTRACT CLASS NAMESPACE REL

%%

namespace_list
	:
	| namespace namespace_list
	;

namespace_name
	:
	| DOT WORD
	{
		namespace = realloc(namespace, strlen(namespace) + strlen($2) + 1);
		strcat(namespace, ".");
		strcat(namespace, $2);
	}
	namespace_name
	;

namespace
	: NAMESPACE WORD
	{
		namespace = strdup($2);
	} namespace_name OBRACE renderer_list EBRACE SEMICOLON
	{
		free(namespace);
		namespace = NULL;
	}
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
	: definition WORD
	{
		name = strdup($2);
	}
	renderer_inheritance OBRACE declaration_list EBRACE SEMICOLON
	{
		free(name);
	}
	;

renderer_inheritance
	:
	{
		current = ender_parser_register(namespace, name, NULL);
	}
	| COLON WORD
	{
		Ender_Descriptor *parent = NULL;

		parent = ender_descriptor_get($2);
		/* TODO check if the parent exists */
		current = ender_parser_register(namespace, name, parent);
	}
	;

types
	: type_specifier
	{
		Ender_Parser_Property *pprop;
		Ender_Property *prop;
		Ender_Property *parent;

		pprop = eina_array_data_get(pproperties, eina_array_count_get(pproperties) - 1);
		prop = eina_array_pop(pprop->properties);
		parent = eina_array_data_get(pprop->properties, eina_array_count_get(pprop->properties) - 1);
		ender_property_add(parent, prop);
	}
	COMMA types
	| type_specifier
	{
		Ender_Parser_Property *pprop;
		Ender_Property *prop;
		Ender_Property *parent;

		pprop = eina_array_data_get(pproperties, eina_array_count_get(pproperties) - 1);
		prop = eina_array_pop(pprop->properties);
		parent = eina_array_data_get(pprop->properties, eina_array_count_get(pprop->properties) - 1);
		ender_property_add(parent, prop);
	}
	;

type_specifier
	: UINT
	{
		Ender_Parser_Property *pprop;
		Ender_Property *prop;

		pprop = eina_array_data_get(pproperties, eina_array_count_get(pproperties) - 1);
		prop = ender_property_new(ENDER_UINT32);
		eina_array_push(pprop->properties, prop);
	}
	| INT
	{
		Ender_Parser_Property *pprop;
		Ender_Property *prop;

		pprop = eina_array_data_get(pproperties, eina_array_count_get(pproperties) - 1);
		prop = ender_property_new(ENDER_INT32);
		eina_array_push(pprop->properties, prop);
	}
	| ARGB
	{
		Ender_Parser_Property *pprop;
		Ender_Property *prop;

		pprop = eina_array_data_get(pproperties, eina_array_count_get(pproperties) - 1);
		prop = ender_property_new(ENDER_ARGB);
		eina_array_push(pprop->properties, prop);
	}
	| DOUBLE
	{
		Ender_Parser_Property *pprop;
		Ender_Property *prop;

		pprop = eina_array_data_get(pproperties, eina_array_count_get(pproperties) - 1);
		prop = ender_property_new(ENDER_DOUBLE);
		eina_array_push(pprop->properties, prop);
	}
	| STRING
	{
		Ender_Parser_Property *pprop;
		Ender_Property *prop;

		pprop = eina_array_data_get(pproperties, eina_array_count_get(pproperties) - 1);
		prop = ender_property_new(ENDER_STRING);
		eina_array_push(pprop->properties, prop);
	}
	| SURFACE
	{
		Ender_Parser_Property *pprop;
		Ender_Property *prop;

		pprop = eina_array_data_get(pproperties, eina_array_count_get(pproperties) - 1);
		prop = ender_property_new(ENDER_SURFACE);
		eina_array_push(pprop->properties, prop);
	}
	| WORD
	{
		Ender_Parser_Property *pprop;
		Ender_Property *prop;

		pprop = eina_array_data_get(pproperties, eina_array_count_get(pproperties) - 1);
		prop = ender_property_new(ENDER_RENDERER);
		eina_array_push(pprop->properties, prop);
	}
	| ENDER
	{
		Ender_Parser_Property *pprop;
		Ender_Property *prop;

		pprop = eina_array_data_get(pproperties, eina_array_count_get(pproperties) - 1);
		prop = ender_property_new(ENDER_ENDER);
		eina_array_push(pprop->properties, prop);
	}
	| MATRIX
	{
		Ender_Parser_Property *pprop;
		Ender_Property *prop;

		pprop = eina_array_data_get(pproperties, eina_array_count_get(pproperties) - 1);
		prop = ender_property_new(ENDER_MATRIX);
		eina_array_push(pprop->properties, prop);

	}
	| OBRACKET
	{
		Ender_Parser_Property *pprop;
		Ender_Property *prop;

		pprop = eina_array_data_get(pproperties, eina_array_count_get(pproperties) - 1);
		prop = ender_property_new(ENDER_LIST);
		eina_array_push(pprop->properties, prop);
	}
	types EBRACKET
	;

type_relative
	:
	| REL
	{
		Ender_Parser_Property *pprop;
		pprop = eina_array_data_get(pproperties, eina_array_count_get(pproperties) - 1);
		pprop->relative = EINA_TRUE;
	}
	;

declaration
	: 
	{
		Ender_Parser_Property *pprop;

		pprop = calloc(1, sizeof(Ender_Parser_Property));
		pprop->properties = eina_array_new(1);
		eina_array_push(pproperties, pprop);
	}
	type_relative type_specifier WORD SEMICOLON
	{
		Ender_Parser_Property *pprop;
		Ender_Property *prop;

		pprop = eina_array_pop(pproperties);
		prop = eina_array_pop(pprop->properties);
		ender_parser_property_add(namespace, current, $3, prop);
		eina_array_free(pprop->properties);
		free(pprop);
	}
	;

declaration_list
	:
	| declaration declaration_list
	;
%%
