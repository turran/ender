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
 
void ender_error(const char *str)
{
        fprintf(stderr,"error: %s\n",str);
}
 
int ender_wrap()
{
        return 1;
} 

static Ender_Descriptor *current = NULL;
static char *name = NULL;
static char *namespace = NULL;
extern Eina_Array *properties;
 
%}

%token UNSIGNED INT COLOR DOUBLE FLOAT IMAGE SURFACE COORD PATH STRING
%token WORD OBRACE EBRACE SEMICOLON EQUAL QUOTE COLON DOT OBRACKET EBRACKET COMMA
%token ABSTRACT CLASS NAMESPACE

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
		Ender_Property *prop;
		Ender_Property *parent;
		prop = eina_array_pop(properties);
		parent = eina_array_data_get(properties, eina_array_count_get(properties) - 1);
		ender_property_add(parent, prop);
	}
	COMMA types
	| type_specifier
	{
		Ender_Property *prop;
		Ender_Property *parent;
		prop = eina_array_pop(properties);
		parent = eina_array_data_get(properties, eina_array_count_get(properties) - 1);
		ender_property_add(parent, prop);
	}
	;

type_specifier
	: UNSIGNED INT
	{
		Ender_Property *prop;
		prop = ender_property_new(ENDER_UINT32);
		eina_array_push(properties, prop);
	}
	| INT
	{
		Ender_Property *prop;
		prop = ender_property_new(ENDER_INT32);
		eina_array_push(properties, prop);
	}
	| COLOR
	{
		Ender_Property *prop;
		prop = ender_property_new(ENDER_COLOR);
		eina_array_push(properties, prop);
	}
	| DOUBLE
	{
		Ender_Property *prop;
		prop = ender_property_new(ENDER_DOUBLE);
		eina_array_push(properties, prop);
	}
	| FLOAT
	{
		Ender_Property *prop;
		prop = ender_property_new(ENDER_FLOAT);
		eina_array_push(properties, prop);
	}
	| STRING
	{
		Ender_Property *prop;
		prop = ender_property_new(ENDER_STRING);
		eina_array_push(properties, prop);
	}
	| WORD
	{
		Ender_Property *prop;
		Ender_Descriptor *external = NULL;

		external = ender_descriptor_get($1);
		/* TODO check that the propriptor exists */
		prop = ender_property_new(ENDER_RENDERER);
		printf("renderer found\n");
		eina_array_push(properties, prop);
	}
	| OBRACKET
	{ 
		Ender_Property *prop;

		prop = ender_property_new(ENDER_LIST);
		eina_array_push(properties, prop);
		printf("list found\n");
	}
	types EBRACKET
	;

declaration
	: type_specifier WORD SEMICOLON
	{
		Ender_Property *prop;

		prop = eina_array_pop(properties);
		ender_parser_property_add(namespace, current, $2, prop);
	}
	;

declaration_list
	:
	| declaration declaration_list
	;
%%
