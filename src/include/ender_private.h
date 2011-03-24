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
#ifndef _ENDER_PRIVATE_H
#define _ENDER_PRIVATE_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dlfcn.h>

/* core */
#define ERR(...) EINA_LOG_DOM_ERR(ender_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(ender_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(ender_log_dom, __VA_ARGS__)
extern int ender_log_dom;

typedef Enesim_Renderer * (*Ender_Creator)(void);
typedef void (*Ender_Getter)(Enesim_Renderer *r, ...);
typedef void (*Ender_Setter)(Enesim_Renderer *r, ...);
typedef void (*Ender_Add)(Enesim_Renderer *r, ...);
typedef void (*Ender_Remove)(Enesim_Renderer *r, ...);
typedef void (*Ender_Clear)(Enesim_Renderer *r);
typedef void (*Ender_Init)(void);
typedef void (*Ender_Shutdown)(void);

struct _Ender_Container
{
	/* common */
	Ender_Property_Type type;
	/* structs */
	int num_elements;
	Eina_List *elements;
	/* inner element */
	ssize_t offset;
};

struct _Ender_Descriptor
{
	char *name;
	Ender_Type type;
	Ender_Descriptor *parent;
	Ender_Creator create;
	Eina_Hash *properties;
};

struct _Ender_Property
{
	char *name;
	Ender_Getter get;
	Ender_Setter set;
	Ender_Add add;
	Ender_Remove remove;
	Ender_Clear clear;
	Ender_Container *prop;
	Eina_Bool relative;
};


Ender_Property * ender_descriptor_property_get_internal(Ender_Descriptor *e, const char *name);

/* descriptor */
Ender_Descriptor * ender_descriptor_register(const char *name, Ender_Creator creator,
		Ender_Descriptor *parent, Ender_Type type);
void ender_descriptor_unregister(Ender_Descriptor *edesc);
const char * ender_descriptor_name_get(Ender_Descriptor *edesc);
Ender_Descriptor * ender_descriptor_find(const char *name);
void ender_descriptor_property_add(Ender_Descriptor *edesc, const char *name, Ender_Container *p,
	Ender_Getter get, Ender_Setter set,
	Ender_Add add, Ender_Remove remove, Ender_Clear clear,
	Eina_Bool relative);

/* property */
Ender_Container * ender_container_new(Ender_Property_Type t);
void ender_property_container_delete(Ender_Container *p);
void ender_container_add(Ender_Container *p, Ender_Container *sub);

/* the parser */
typedef struct _Ender_Parser Ender_Parser;
typedef struct _Ender_Parser_Property Ender_Parser_Property;

struct _Ender_Parser
{
	char *file;
	char ns[PATH_MAX];
	Ender_Descriptor *descriptor;
};

struct _Ender_Parser_Property
{
	char *name;
	Eina_Bool relative;
	Eina_Array *properties;
};

Ender_Descriptor * ender_parser_register(const char *ns, const char *name, Ender_Descriptor * parent, Ender_Type type);
void ender_parser_init(void);
void ender_parser_shutdown(void);
void ender_parser_parse(const char *file);

#endif
