%name-prefix="escen_"
%error-verbose
%defines
%locations
%pure-parser
%parse-param { void * scanner }
%parse-param { Escen_Parser *parser }
%lex-param { void * scanner }

/* ESCEN - Ender's based scene loader
 * Copyright (C) 2010 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

#include "Escen.h"
#include "escen_private.h"

void escen_error(void *data, void *scanner, Escen_Parser *parser, const char *str);

%}

%union {
	Ender_Value *value;
	Ender_Descriptor *edesc;
	Escen_Ender *ed;
	Eina_List *list;
	Escen_Time time;
	Etch_Interpolator_Type atype;
	Eina_Bool boolean;
	double d;
	char *s;
	Escen_Setter *set;
	Escen_Animation *animation;
	Escen_Script *script;
}

/* parser related tokens */
%token NAMESPACE
/* state related tokens */
%token STATE
%token DEFAULT
/* proxy related tokens */
%token PROXY
/* set related tokens */
%token SET
/* animate related tokens */
%token ANIMATE
%token PROPERTY
%token VALUE
%token TIME
%token TYPE
%token KEY
%token REPEAT
%token WAIT
/* animation key types */
%token <atype> DISCRETE
%token <atype> LINEAR
%token <atype> COSIN
%token <atype> QUADRATIC
%token <atype> CUBIC
/* script related tokens */
%token SCRIPT
%token <s> SCRIPT_DATA
/* symbols */
%token <s> T_WORD
%token <s> STRING
%token <d> NUMBER
%token <boolean> T_TRUE
%token <boolean> T_FALSE
/* escen related tokens */
%token ESCEN

%type <value> value
%type <list> state_inherit_list
%type <list> state_inherit
%type <ed> ender
%type <edesc> ender_descriptor
%type <s> property
%type <time> key_time
%type <value> key_value
%type <atype> key_type_etch
%type <atype> key_type
%type <value> list
%type <value> struct
%type <list> value_list
%type <boolean> boolean
%type <boolean> animate_wait
%type <s> animate_property
%type <set> set
%type <list> sets
%type <animation> animate
%type <list> animates
%type <script> script
%%

escen
	: ESCEN
	{ parser->escen = escen_new(); }
	'{' escen_contents '}' ';'
	;

escen_contents
	:
	| namespace enders
	;

namespace
	:
	| NAMESPACE '=' STRING ';'
	{
		parser->ns = $3;
	}
	;

enders
	:
	| ender ';' enders
	;

proxy
	: PROXY STRING ',' STRING
	{
		Escen_Proxy *ep;
		Escen_Ender *ed;

		printf("proxy found with %s -> %s\n", $2, $4);
		ed = eina_array_pop(parser->escen_ender);
		ep = escen_ender_proxy_add(ed, $2, $4);
		eina_array_push(parser->escen_proxy, ep);
		eina_array_push(parser->escen_ender, ed);
	}
	'{' sets animates scripts '}'
	{
		/* FIXME this code is the same as the one from the state, merge them */
		Escen_Proxy *ep;
		Eina_List *l;
		Escen_Setter *setter;
		Escen_Animation *animation;
		Escen_Script *script;

		ep = eina_array_pop(parser->escen_proxy);
		EINA_LIST_FOREACH($7, l, setter)
		{
			escen_proxy_setter_add(ep, setter);
		}
		EINA_LIST_FOREACH($8, l, animation)
		{
			escen_proxy_animation_add(ep, animation);
		}
#if 0
		EINA_LIST_FOREACH($8, l, script)
		{
			printf("script\n");
		}
#endif
		eina_array_push(parser->escen_proxy, ep);
	}
	;

proxies
	:
	| proxy ';' proxies
	;

ender
	: ender_descriptor STRING
	{
		Escen_Ender *ee;

		ee = escen_ender_add(parser->escen, $1, $2);
		eina_array_push(parser->escen_ender, ee);
	}
	'{' proxies default states '}'
	{
		$$ = eina_array_pop(parser->escen_ender);
	}
	;

ender_descriptor
	: T_WORD
	{
		Ender_Namespace *ns;

		/* FIXME we should use a non-version function */
		ns = ender_namespace_find(parser->ns, 0);
		if (!ns) $$ = NULL;
		else $$ = ender_namespace_descriptor_find(ns, $1);
	}
	| T_WORD ':' T_WORD
	{
		Ender_Namespace *ns;

		/* FIXME we should use a non-version function */
		ns = ender_namespace_find($1, 0);
		if (!ns) $$ = NULL;
		else $$ = ender_namespace_descriptor_find(ns, $3);
	}
	;
	

default
	:
	| DEFAULT
	{
		Escen_State *es;
		Escen_Ender *ed;

		ed = eina_array_pop(parser->escen_ender);
		es = escen_ender_state_add(ed, "default");
		eina_array_push(parser->escen_state, es);
		eina_array_push(parser->escen_ender, ed);
	}
	state_inherit
	{
		Escen_Ender *ed;
		Eina_List *l;
		char *alias;

		ed = eina_array_pop(parser->escen_ender);
		EINA_LIST_FOREACH($3, l, alias)
		{
			escen_ender_state_alias_add(ed, "default", alias);
		}
		eina_array_push(parser->escen_ender, ed);
	}
	state_common ';'
	;

states
	:
	| state ';' states
	;

state
	: STATE STRING
	{
		Escen_Ender *ee;
		Escen_State *es;

		ee = eina_array_pop(parser->escen_ender);
		es = escen_ender_state_add(ee, $2);
		eina_array_push(parser->escen_state, es);
		eina_array_push(parser->escen_ender, ee);
	}
	state_inherit
	{
		Escen_Ender *ee;
		Eina_List *l;
		char *alias;

		ee = eina_array_pop(parser->escen_ender);
		EINA_LIST_FOREACH($4, l, alias)
		{
			escen_ender_state_alias_add(ee, $2, alias);
		}
		eina_array_push(parser->escen_ender, ee);
	}
	state_common
	;

state_common
	: '{' sets animates scripts '}'
	{
		Escen_State *es;
		Eina_List *l;
		Escen_Setter *setter;
		Escen_Animation *animation;
		Escen_Script *script;

		es = eina_array_pop(parser->escen_state);
		EINA_LIST_FOREACH($2, l, setter)
		{
			escen_state_setter_add(es, setter);
		}
		EINA_LIST_FOREACH($3, l, animation)
		{
			escen_state_animation_add(es, animation);
		}
#if 0
		EINA_LIST_FOREACH($3, l, script)
		{
			printf("script\n");
		}
#endif
	}

state_inherit_list
	: { $$ = NULL; }
	| ',' STRING state_inherit_list { $$ = eina_list_append($3, $2); }
	;

state_inherit
	: ':' STRING state_inherit_list { $$ = eina_list_append($3, $2); }
	| { $$ = NULL; }
	;

script
	: SCRIPT STRING '{' SCRIPT_DATA '}' ';'
	{
		Escen_State *es;

		es = eina_array_pop(parser->escen_state);
		escen_state_script_add(es, $2, $4);
		eina_array_push(parser->escen_state, es);
	}
	;

scripts
	:
	| script scripts
	;

sets
	: { $$ = NULL; }
	| set sets { $$ = eina_list_prepend($2, $1); }
	;

property
	: T_WORD
	{
		$$ = $1;
#if 0
		Ender_Descriptor *desc;
		Escen_Ender *ee;

		/* simplest case */
		ee = eina_array_pop(parser->escen_ender);
		desc = escen_ender_descriptor_get(ee);
		$$ = ender_descriptor_property_get(desc, $1);
		eina_array_push(parser->escen_ender, ee);
#endif
	}
	| T_WORD '.' T_WORD
	{
		$$ = $1;
#if 0
		Ender_Descriptor *desc;

		/* relative properties */
		desc = ender_descriptor_find($1);
		$$ = ender_descriptor_property_get(desc, $3);
#endif
	}
	;

set
	: SET '{' property '=' value ';' '}' ';'
	{
		$$ = escen_setter_new($3, $5);
	}
	;

animates
	: { $$ = NULL; }
	| animate animates { $$ = eina_list_prepend($2, $1); }
	;

animate_property
	: PROPERTY '=' property ';' { $$ = $3; }
	;

boolean
	: T_TRUE { $$ = $1; }
	| T_FALSE { $$ = $1; }
	;

animate_wait
	: WAIT '=' boolean ';' { $$ = $3; }
	| { $$ = EINA_FALSE; }
	;

animate
	: ANIMATE '{' animate_property animate_wait
	{
		parser->animation = escen_animation_new($3, $4);
	}
 	animate_keys '}' ';'
	{
		$$ = parser->animation;
	}
	;

key_time
	: TIME '=' NUMBER ';'
	{
		$$ = ETCH_SECOND * $3;
	}
	;

key_value
	: VALUE '=' value ';'
	{
		$$ = $3;
	}
	;

key_type_etch
	: DISCRETE { $$ = $1; }
	| LINEAR { $$ = $1; }
	| COSIN { $$ = $1; }
	| QUADRATIC { $$ = $1; }
	| CUBIC { $$ = $1; }
	;

key_type
	: TYPE '=' key_type_etch ';'
	{
		$$ = $3;
	}
	;

animate_keys
	:
	| KEY '{' key_value key_time key_type '}' ';' animate_keys
	{
		escen_animation_key_add(parser->animation, $3, $4, $5);
	}
	;

struct
	: '{' value_list '}'
	{
		Eina_List *l, *l_next;
		Ender_Value *v;
		Ender_Container *c;

		printf("handling the struct/union case\n");
		c = ender_container_new(ENDER_VALUE);
		$$ = ender_value_list_new(c);
		EINA_LIST_FOREACH_SAFE($2, l, l_next, v)
		{
			ender_value_list_add($$, v);
			$2 = eina_list_remove_list($2, l);
		}
	}
	;

list	: '[' value_list ']'
	{
		Eina_List *l, *l_next;
		Ender_Value *v;
		Ender_Container *c;

		printf("handling the list case\n");
		c = ender_container_new(ENDER_VALUE);
		$$ = ender_value_list_new(c);
		EINA_LIST_FOREACH_SAFE($2, l, l_next, v)
		{
			ender_value_list_add($$, v);
			$2 = eina_list_remove_list($2, l);
		}
	}
	;

value
	: NUMBER
	{
		$$ = ender_value_basic_new(ENDER_DOUBLE);
		ender_value_double_set($$, $1);
	}
	| STRING
	{
		$$ = ender_value_basic_new(ENDER_STRING);
		ender_value_string_set($$, $1);
	}
	| ender
	{
		$$ = ender_value_basic_new(ENDER_POINTER);
		ender_value_pointer_set($$, $1, NULL, NULL);
	}
	| list
	{
		$$ = $1;
	}
	| struct
	{
		$$ = $1;
	}
	;

value_list
	: value
	{
		$$ = eina_list_append(NULL, $1);
	}
	| value ',' value_list
	{
		$$ = eina_list_prepend($3, $1);
	}
	;

%%

void escen_error(void *data, void *scanner, Escen_Parser *parser, const char *str)
{
	YYLTYPE *lloc = (YYLTYPE *)data;
	ERR("Parsing error (%d) %s", lloc->first_line, str);
}


