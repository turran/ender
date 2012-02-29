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

typedef void (*Ender_Init)(void);
typedef void (*Ender_Shutdown)(void);

struct _Ender_Container
{
	/* common */
	Ender_Value_Type type;
	/* structs */
	int num_elements;
	Eina_List *elements;
	/* inner element */
	ssize_t offset;
};

struct _Ender_Value
{
	Ender_Container *container;
	int ref;
	Ender_Value_Free free_cb;
	void *free_cb_data;
	Eina_Bool owned;
	union {
		int32_t i32;
		uint32_t u32;
		double d;
		void *ptr;
		Enesim_Matrix matrix;
	} data;
};

struct _Ender_Descriptor
{
	char *name;
	Ender_Descriptor_Type type;
	Ender_Descriptor *parent;
	Ender_Creator create;
	Eina_Ordered_Hash *properties;
	Ender_Namespace *ns;
};

/* descriptor */
Ender_Descriptor * ender_descriptor_new(const char *name, Ender_Namespace *ns,
		Ender_Creator creator,
		Ender_Descriptor *parent, Ender_Descriptor_Type type);
const char * ender_descriptor_name_get(Ender_Descriptor *edesc);
Ender_Descriptor * ender_descriptor_find(const char *name);
void ender_descriptor_init(void);
void ender_descriptor_shutdown(void);

/* property */
typedef void (*Ender_Property_Accessor)(Ender_Property *ep, Ender_Element *e, Ender_Value *v, void *data);
typedef Ender_Property_Accessor Ender_Property_Getter;
typedef Ender_Property_Accessor Ender_Property_Setter;
typedef Ender_Property_Accessor Ender_Property_Add;
typedef Ender_Property_Accessor Ender_Property_Remove;
typedef void (*Ender_Property_Clear)(Ender_Property *ep, Ender_Element *e, void *data);

Ender_Property * ender_property_new(const char *name,
		Ender_Container *ec,
		Ender_Property_Getter get,
		Ender_Property_Setter set,
		Ender_Property_Add add,
		Ender_Property_Remove remove,
		Ender_Property_Clear clear,
		Eina_Bool relative, void *data);

void ender_property_element_value_set(Ender_Property *ep, Ender_Element *e,
		Ender_Value *v);
void ender_property_element_value_get(Ender_Property *ep, Ender_Element *e,
		Ender_Value *v);
void ender_property_element_value_add(Ender_Property *ep, Ender_Element *e,
		Ender_Value *v);
void ender_property_element_value_remove(Ender_Property *ep, Ender_Element *e,
		Ender_Value *v);
void ender_property_element_value_clear(Ender_Property *ep, Ender_Element *e);
/* container */
void ender_property_container_delete(Ender_Container *p);
void ender_container_init(void);
void ender_container_shutdown(void);

/* namespace */
void ender_namespace_init(void);
void ender_namespace_shutdown(void);

/* element */
void ender_element_parent_set(Ender_Element *e, Ender_Element *parent);

/* the parser */
typedef struct _Ender_Parser_Descriptor
{
	/* using */
	void (*on_using)(void *data, const char *file);
	/* namespace */
	void (*on_namespace)(void *data, const char *name);
	/* renderer */
	void (*on_renderer)(void *data, const char *name, Ender_Descriptor_Type type, const char *parent);
	/* property */
	void (*on_property)(void *data, const char *name, Eina_Bool relative, Ender_Container *container); 
} Ender_Parser_Descriptor;

Eina_Bool ender_parser_parse(const char *file, Ender_Parser_Descriptor *descriptor, void *data);

/* the loader */
void ender_loader_init(void);
void ender_loader_shutdown(void);
void ender_loader_load_all(void);

#endif
