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
static Ender_Property_Type type;
 
%}

%token UNSIGNED INT COLOR DOUBLE FLOAT IMAGE SURFACE COORD PATH STRING
%token WORD OBRACE EBRACE SEMICOLON EQUAL QUOTE COLON DOT
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


type_specifier
	: UNSIGNED INT
	{
		type = ENDER_UINT32;
	}
	| INT
	{
		type = ENDER_INT32;
	}
	| COLOR
	{
		type = ENDER_COLOR;
	}
	| DOUBLE
	{
		type = ENDER_DOUBLE;
	}
	| FLOAT
	{
		type = ENDER_FLOAT;
	}
	| STRING
	{
		type = ENDER_STRING;
	}
	| WORD
	{
		Ender_Descriptor *external = NULL;

		external = ender_descriptor_get($1);
		/* TODO check that the descriptor exists */
		type  = ENDER_RENDERER;
	}
	;

declaration
	: type_specifier WORD SEMICOLON
	{
		ender_parser_property_add(namespace, current, $2, type);
	}
	;

declaration_list
	:
	| declaration declaration_list
	;
%%
