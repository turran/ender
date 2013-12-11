/* ENDER - Enesim's descriptor library
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

#ifndef _ENDER_PARSER_H
#define _ENDER_PARSER_H

/**
 * @defgroup Ender_Parser_Group Parser
 * @{
 */

typedef struct _Ender_Parser_Definition
{
	const char *name;
	const char *alias;
} Ender_Parser_Definition;

typedef struct _Ender_Parser_Container
{
	Ender_Value_Type type;
	Eina_List *subcontainers;
	char *defined;
} Ender_Parser_Container;

typedef struct _Ender_Parser_Property
{
	Ender_Parser_Definition def;
	Ender_Parser_Container *container;
	Eina_Bool rel;
} Ender_Parser_Property;

typedef struct _Ender_Parser_Function
{
	Ender_Parser_Definition def;
	Ender_Parser_Container *ret;
	Eina_List *args;
} Ender_Parser_Function;

/* FIXME this functions will be evolving until all of them
 * are similar to the add_property
 */
typedef void (*Ender_Parser_Add_Using)(void *data, const char *name);
typedef void (*Ender_Parser_Add_Namespace)(void *data, const char *name,
		int version);
typedef void (*Ender_Parser_Add_Native)(void *data, const char *name,
		const char *alias, Ender_Descriptor_Type type, const char *parent);
typedef void (*Ender_Parser_Add_Property)(void *data, Ender_Parser_Property *p);
typedef void (*Ender_Parser_Add_Function)(void *data, Ender_Parser_Function *f);

typedef struct _Ender_Parser_Descriptor
{
	Ender_Parser_Add_Using add_using;
	Ender_Parser_Add_Namespace add_namespace;
	Ender_Parser_Add_Native add_native;
	Ender_Parser_Add_Property add_property;
	Ender_Parser_Add_Function add_function;
} Ender_Parser_Descriptor;

EAPI Eina_Bool ender_parser_parse(const char *file, Ender_Parser_Descriptor *descriptor, void *data);


/**
 * @}
 */


#endif

