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

#ifndef _ENDER_DESCRIPTOR_H
#define _ENDER_DESCRIPTOR_H

/**
 * @defgroup Ender_Descriptor_Group Descriptor
 * @{
 */

typedef struct _Ender_Descriptor Ender_Descriptor;

typedef void (*Ender_Accessor)(void *o, ...);
typedef Eina_Bool (*Ender_Is_Set)(void *o);
typedef Ender_Accessor Ender_Getter;
typedef Ender_Accessor Ender_Setter;
typedef Ender_Accessor Ender_Add;
typedef Ender_Accessor Ender_Remove;
typedef void (*Ender_Clear)(void *o);

#define ENDER_IS_SET(f) ((Ender_Is_Set)(f))
#define ENDER_GETTER(f) ((Ender_Getter)(f))
#define ENDER_SETTER(f) ((Ender_Setter)(f))
#define ENDER_ADD(f) ((Ender_Add)(f))
#define ENDER_REMOVE(f) ((Ender_Remove)(f))
#define ENDER_CLEAR(f) ((Ender_Clear)(f))


typedef enum _Ender_Descriptor_Type
{
	ENDER_TYPE_ABSTRACT,
	ENDER_TYPE_CLASS,
	ENDER_TYPE_STRUCT,
	ENDER_TYPE_UNION,
	ENDER_TYPES,
} Ender_Descriptor_Type;

#define ENDER_DESCRIPTOR_PROPERTY_ADD_SIMPLE(d, name, vtype, prefix)		\
	ender_descriptor_property_add(d, #name, ender_container_new(vtype),	\
			prefix##_##name##_get, prefix##_##name##_set, 		\
			NULL, NULL, NULL,					\
			prefix##_##_name##_is_set,				\
			EINA_FALSE, -1)
#define ENDER_DESCRIPTOR_PROPERTY_ADD_DESCRIPTOR(d, name, pd, prefix)		\
	ender_descriptor_property_add(d, #name, 				\
			ender_container_new_descriptor_from(pd), 		\
			prefix##_##name##_get, prefix##_##name##_set, 		\
			NULL, NULL, NULL,					\
			prefix##_##_name##_is_set,				\
			EINA_FALSE, -1)

typedef void (*Ender_Property_List_Callback)(Ender_Property *prop, void *data);
typedef void (*Ender_Function_List_Callback)(Ender_Function *func, void *data);

EAPI Ender_Descriptor * ender_descriptor_find(const char *name);
EAPI Ender_Descriptor * ender_descriptor_find_with_namespace(const char *name, const char *ns, int version);
EAPI Ender_Property * ender_descriptor_property_add(Ender_Descriptor *edesc,
		const char *name,
		Ender_Container *ec,
		Ender_Getter get,
		Ender_Setter set,
		Ender_Add add,
		Ender_Remove remove,
		Ender_Clear clear,
		Ender_Is_Set is_set,
		Eina_Bool relative,
		int offset);
EAPI Ender_Property * ender_descriptor_struct_property_add(
		Ender_Descriptor *thiz,
		const char *name, Ender_Container *ec, int offset);
EAPI void ender_descriptor_property_list(Ender_Descriptor *thiz, Ender_Property_List_Callback cb, void *data);
EAPI void ender_descriptor_property_list_recursive(Ender_Descriptor *thiz, Ender_Property_List_Callback cb, void *data);
EAPI Ender_Property * ender_descriptor_property_get(Ender_Descriptor *thiz, const char *name);
EAPI Ender_Property * ender_descriptor_property_get_at(Ender_Descriptor *thiz, int idx);

EAPI Ender_Function * ender_descriptor_function_add(Ender_Descriptor *edesc, const char *name,
		Ender_Accessor f, Ender_Marshaller marshaller, Ender_Container *ret, ...);
EAPI Ender_Function * ender_descriptor_function_add_list(Ender_Descriptor *edesc, const char *name,
		Ender_Accessor f, Ender_Marshaller marshaller, Ender_Container *ret,
		Eina_List *args);
EAPI Ender_Function * ender_descriptor_function_get(Ender_Descriptor *thiz, const char *name);
EAPI void ender_descriptor_function_list(Ender_Descriptor *thiz, Ender_Function_List_Callback cb, void *data);

EAPI void ender_descriptor_list(Ender_Descriptor_List_Callback cb, void *data);
EAPI Eina_Bool ender_descriptor_exists(const char *name);
EAPI Ender_Descriptor_Type ender_descriptor_type(Ender_Descriptor *thiz);
EAPI const char * ender_descriptor_name_get(Ender_Descriptor *thiz);
EAPI size_t ender_descriptor_size_get(Ender_Descriptor *thiz);
EAPI Ender_Descriptor * ender_descriptor_parent(Ender_Descriptor *thiz);
EAPI Ender_Namespace * ender_descriptor_namespace_get(Ender_Descriptor *thiz);
EAPI Ender_Element * ender_descriptor_element_new(Ender_Descriptor *thiz);
EAPI Ender_Element * ender_descriptor_element_new_from_data(
		Ender_Descriptor *thiz, void *data);

EAPI Ender_Element * ender_descriptor_element_unmarshal(Ender_Descriptor *thiz,
		void *data, unsigned int len);
EAPI void * ender_descriptor_native_marshal(Ender_Descriptor *thiz,
		void *data, unsigned int *len);
EAPI void * ender_descriptor_native_unmarshal(Ender_Descriptor *thiz,
		const void *data, unsigned int len);
EAPI void ender_descriptor_native_unmarshal_inplace(Ender_Descriptor *thiz,
		void *native, const void *data, unsigned int len);
EAPI void * ender_descriptor_native_new(Ender_Descriptor *thiz);
EAPI void ender_descriptor_native_free(Ender_Descriptor *thiz, void *object);
EAPI void ender_descriptor_data_set(Ender_Descriptor *thiz, void *data);
EAPI void * ender_descriptor_data_get(Ender_Descriptor *thiz);


/**
 * @}
 */


#endif

