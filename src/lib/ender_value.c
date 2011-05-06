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
#include "Ender.h"
#include "ender_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Value * _ender_value_new(Ender_Container *ec)
{
	Ender_Value *ev;

	ev = calloc(1, sizeof(Ender_Value));
	ev->container = ec;

	return ev;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Value * ender_value_basic_new(Ender_Property_Type type)
{
	Ender_Container *ec;
	Ender_Value *ev;

	if (type == ENDER_LIST || type == ENDER_STRUCT)
		return NULL;

	ec = ender_container_new(type);
	return _ender_value_new(ec);
}

EAPI Ender_Value * ender_value_new_container_from(Ender_Container *ec)
{
	return _ender_value_new(ec);	
}

EAPI Ender_Value * ender_value_list_new(Ender_Property_Type child_type)
{

}

EAPI Ender_Container * ender_value_container_get(Ender_Value *value)
{
	return value->container;
}

EAPI Ender_Property_Type ender_value_type_get(Ender_Value *value)
{
	return value->container->type;
}

EAPI void ender_value_int32_set(Ender_Value *value, int32_t i32)
{
	if (value->container->type != ENDER_INT32)
		return;
	value->data.i32 = i32;
}

EAPI int32_t ender_value_int32_get(Ender_Value *value)
{
	if (value->container->type != ENDER_INT32)
		return 0;
	return value->data.i32;
}

EAPI void ender_value_uint32_set(Ender_Value *value, uint32_t u32)
{
	if (value->container->type != ENDER_UINT32)
		return;
	value->data.u32 = u32;
}

EAPI uint32_t ender_value_uint32_get(Ender_Value *value)
{
	if (value->container->type != ENDER_UINT32)
		return 0;
	return value->data.u32;
}

EAPI void ender_value_double_set(Ender_Value *value, double d)
{
	if (value->container->type != ENDER_DOUBLE)
		return;
	value->data.d = d;
}

EAPI double ender_value_double_get(Ender_Value *value)
{
	if (value->container->type != ENDER_DOUBLE)
		return 0;
	return value->data.d;
}

EAPI void ender_value_argb_set(Ender_Value *value, Enesim_Color argb)
{
	if (value->container->type != ENDER_ARGB)
		return;
	value->data.u32 = argb;
}

EAPI Enesim_Color ender_value_argb_get(Ender_Value *value)
{
	if (value->container->type != ENDER_ARGB)
		return 0;
	return value->data.u32;
}

EAPI void ender_value_matrix_set(Ender_Value *value, Enesim_Matrix *matrix)
{
	if (value->container->type != ENDER_MATRIX)
		return;

	value->data.ptr = matrix;
}

EAPI void ender_value_static_matrix_set(Ender_Value *value, const Enesim_Matrix *matrix)
{
	Enesim_Matrix *m;

	if (value->container->type != ENDER_MATRIX)
		return;

	m = malloc(sizeof(Enesim_Matrix));
	*m = *matrix;
	value->data.ptr = m;
	value->owned = EINA_TRUE;
}

EAPI Enesim_Matrix * ender_value_matrix_get(Ender_Value *value)
{
	if (value->container->type != ENDER_MATRIX)
		return NULL;

	return value->data.ptr;
} 

EAPI void ender_value_string_set(Ender_Value *value, char * string)
{
	if (value->container->type != ENDER_STRING)
		return;
	value->data.ptr = string;
}

EAPI void ender_value_static_string_set(Ender_Value *value, const char * string)
{
	if (value->container->type != ENDER_STRING)
		return;
	value->data.ptr = strdup(string);
	value->owned = EINA_TRUE;
}

EAPI char * ender_value_string_get(Ender_Value *value)
{
	if (value->container->type != ENDER_STRING)
		return NULL;
	return value->data.ptr;
}

EAPI void ender_value_struct_set(Ender_Value *value, void * structure)
{

}

EAPI void ender_value_const_struct_set(Ender_Value *value, void * structure)
{

}

EAPI void ender_value_renderer_set(Ender_Value *value, Enesim_Renderer *renderer)
{

}

EAPI void ender_value_ender_set(Ender_Value *value, Ender *ender)
{

}

EAPI void ender_value_surface_set(Ender_Value *value, Enesim_Surface *surface)
{
	if (value->container->type != ENDER_SURFACE)
		return;
	value->data.ptr = surface;
}

EAPI Enesim_Surface * ender_value_surface_get(Ender_Value *value)
{
	if (value->container->type != ENDER_SURFACE)
		return NULL;
	return value->data.ptr;
}

EAPI void ender_value_pointer_set(Ender_Value *value, void *ptr, Ender_Value_Free free_cb, void *user_data)
{
	if (value->container->type != ENDER_POINTER)
		return;
	value->data.ptr = ptr;
	value->owned = EINA_TRUE;
	value->free_cb = free_cb;
	value->free_cb_data = user_data;
}

EAPI void * ender_value_pointer_get(Ender_Value *value)
{
	if (value->container->type != ENDER_POINTER)
		return NULL;
	return value->data.ptr;
}

EAPI void ender_value_list_add(Ender_Value *value, Ender_Value *child)
{

}

EAPI void ender_value_free(Ender_Value *value)
{
	if (value->owned)
	{
		if (value->free_cb)
			value->free_cb(value, value->free_cb_data);
		else
			free(value->data.ptr);
	}
	free(value);
}
