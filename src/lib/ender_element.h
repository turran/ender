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

#ifndef _ENDER_ELEMENT_H
#define _ENDER_ELEMENT_H

/**
 * @defgroup Ender_Element_Group Element
 * @{
 */

typedef struct _Ender_Element Ender_Element;

typedef void (*Ender_Element_Accessor)(Ender_Element *e, Ender_Property *ep, Ender_Value *v, void *data);
typedef Eina_Bool (*Ender_Element_Is_Set)(Ender_Element *e, Ender_Property *ep, void *data);
typedef Ender_Element_Accessor Ender_Element_Getter;
typedef Ender_Element_Accessor Ender_Element_Setter;
typedef Ender_Element_Accessor Ender_Element_Add;
typedef Ender_Element_Accessor Ender_Element_Remove;
typedef void (*Ender_Element_Clear)(Ender_Element *e, Ender_Property *ep, void *data);

EAPI Ender_Element * ender_element_new_with_namespace(const char *name, const char *ns_name, int version);
EAPI Ender_Element * ender_element_ref(Ender_Element *e);
EAPI Ender_Element * ender_element_unref(Ender_Element *e);
EAPI void * ender_element_data_set(Ender_Element *e, const char *key, void *data);
EAPI void * ender_element_data_get(Ender_Element *e, const char *key);
EAPI const char * ender_element_name_get(Ender_Element *e);
EAPI Ender_Descriptor * ender_element_descriptor_get(Ender_Element *e);

EAPI Ender_Property * ender_element_property_add(Ender_Element *e, const char *name,
		Ender_Container *ec,
		Ender_Element_Getter get,
		Ender_Element_Setter set,
		Ender_Element_Add add,
		Ender_Element_Remove remove,
		Ender_Element_Clear clear,
		Ender_Element_Is_Set is_set,
		Eina_Bool relative, void *data);

EAPI Ender_Property * ender_element_property_get(Ender_Element *e, const char *name);
EAPI void ender_element_property_list(Ender_Element *e, Ender_Property_List_Callback cb, void *data);
EAPI void ender_element_property_value_set_valist(Ender_Element *e, Ender_Property *prop, va_list va_args);
EAPI void ender_element_property_value_set(Ender_Element *e, Ender_Property *prop, ...);
EAPI void ender_element_property_value_set_simple(Ender_Element *e, Ender_Property *prop, Ender_Value *value);

EAPI void ender_element_property_value_get_valist(Ender_Element *e, Ender_Property *prop, va_list va_args);
EAPI void ender_element_property_value_get(Ender_Element *e, Ender_Property *prop, ...);
EAPI void ender_element_property_value_get_simple(Ender_Element *e, Ender_Property *prop, Ender_Value **value);

EAPI void ender_element_property_value_add_valist(Ender_Element *e, Ender_Property *prop, va_list var_args);
EAPI void ender_element_property_value_add(Ender_Element *e, Ender_Property *prop, ...);
EAPI void ender_element_property_value_add_simple(Ender_Element *e, Ender_Property *prop, Ender_Value *value);

EAPI void ender_element_property_value_remove_valist(Ender_Element *e, Ender_Property *prop, va_list var_args);
EAPI void ender_element_property_value_remove(Ender_Element *e, Ender_Property *prop, ...);
EAPI void ender_element_property_value_remove_simple(Ender_Element *e, Ender_Property *prop, Ender_Value *value);

EAPI Eina_Bool ender_element_property_value_is_set(Ender_Element *e, Ender_Property *prop);

EAPI void ender_element_value_get(Ender_Element *e, const char *name, ...);
EAPI void ender_element_value_get_valist(Ender_Element *e, const char *name, va_list var_args);
EAPI void ender_element_value_get_simple(Ender_Element *e, const char *name, Ender_Value **value);

EAPI void ender_element_value_set(Ender_Element *e, const char *name, ...);
EAPI void ender_element_value_set_valist(Ender_Element *e, const char *name, va_list var_args);
EAPI void ender_element_value_set_simple(Ender_Element *e, const char *name, Ender_Value *value);

EAPI void ender_element_value_add(Ender_Element *e, const char *name, ...);
EAPI void ender_element_value_add_valist(Ender_Element *e, const char *name, va_list var_args);
EAPI void ender_element_value_add_simple(Ender_Element *e, const char *name, Ender_Value *value);

EAPI void ender_element_value_remove(Ender_Element *e, const char *name, ...);
EAPI void ender_element_value_remove_valist(Ender_Element *e, const char *name, va_list var_args);
EAPI void ender_element_value_remove_simple(Ender_Element *e, const char *name, Ender_Value *value);

EAPI void ender_element_value_clear(Ender_Element *e, const char *name);

EAPI Eina_Bool ender_element_value_is_set(Ender_Element *e, const char *name);

EAPI void * ender_element_object_get(Ender_Element *e);

EAPI Ender_Element * ender_element_parent_get(Ender_Element *e);

EAPI Eina_Bool ender_element_call_valist(Ender_Element *e, const char *name, void *ret, va_list va_args);
EAPI Eina_Bool ender_element_call(Ender_Element *e, const char *name, void *ret, ...);
EAPI Eina_Bool ender_element_function_call_valist(Ender_Element *e, Ender_Function *f, void *ret, va_list va_args);
EAPI Eina_Bool ender_element_function_call(Ender_Element *e, Ender_Function *f, void *ret, ...);
EAPI Eina_Bool ender_element_function_call_simple(Ender_Element *e,
		Ender_Function *f, Ender_Value *ret, Eina_List *args);

EAPI void * ender_element_marshal(Ender_Element *e, unsigned int *len);

/**
 * @}
 */


#endif
