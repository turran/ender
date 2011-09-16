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
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Value * ender_value_basic_new(Ender_Value_Type type)
{
	Ender_Container *ec;

	if (type == ENDER_LIST || type == ENDER_STRUCT)
		return NULL;

	ec = ender_container_new(type);
	return _ender_value_new(ec);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Value * ender_value_new_container_from(Ender_Container *ec)
{
	return _ender_value_new(ec);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Value * ender_value_list_new(Ender_Value_Type child_type)
{
	Ender_Container *ec;
	Ender_Container *sub;

	ec = ender_container_new(ENDER_LIST);
	sub = ender_container_new(child_type);
	ender_container_add(ec, sub);

	return _ender_value_new(ec);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_value_container_get(Ender_Value *value)
{
	return value->container;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Value_Type ender_value_type_get(Ender_Value *value)
{
	return value->container->type;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_int32_set(Ender_Value *value, int32_t i32)
{
	if (value->container->type != ENDER_INT32)
		return;
	value->data.i32 = i32;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI int32_t ender_value_int32_get(Ender_Value *value)
{
	if (value->container->type != ENDER_INT32)
		return 0;
	return value->data.i32;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_uint32_set(Ender_Value *value, uint32_t u32)
{
	if (value->container->type != ENDER_UINT32)
		return;
	value->data.u32 = u32;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI uint32_t ender_value_uint32_get(Ender_Value *value)
{
	if (value->container->type != ENDER_UINT32)
		return 0;
	return value->data.u32;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_double_set(Ender_Value *value, double d)
{
	if (value->container->type != ENDER_DOUBLE)
		return;
	value->data.d = d;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI double ender_value_double_get(Ender_Value *value)
{
	if (value->container->type != ENDER_DOUBLE)
		return 0;
	return value->data.d;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_argb_set(Ender_Value *value, Enesim_Argb argb)
{
	if (value->container->type != ENDER_ARGB)
		return;
	value->data.u32 = argb;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Argb ender_value_argb_get(Ender_Value *value)
{
	if (value->container->type != ENDER_ARGB)
		return 0;
	return value->data.u32;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_color_set(Ender_Value *value, Enesim_Color color)
{
	if (value->container->type != ENDER_COLOR)
		return;
	value->data.u32 = color;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Color ender_value_color_get(Ender_Value *value)
{
	if (value->container->type != ENDER_COLOR)
		return 0;
	return value->data.u32;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_matrix_set(Ender_Value *value, Enesim_Matrix *matrix)
{
	if (value->container->type != ENDER_MATRIX)
		return;

	value->data.ptr = matrix;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
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

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Matrix * ender_value_matrix_get(Ender_Value *value)
{
	if (value->container->type != ENDER_MATRIX)
		return NULL;

	return value->data.ptr;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_string_set(Ender_Value *value, char * string)
{
	if (value->container->type != ENDER_STRING)
		return;
	value->data.ptr = string;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_static_string_set(Ender_Value *value, const char * string)
{
	if (value->container->type != ENDER_STRING)
		return;
	value->data.ptr = strdup(string);
	value->owned = EINA_TRUE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI char * ender_value_string_get(Ender_Value *value)
{
	if (value->container->type != ENDER_STRING)
		return NULL;
	return value->data.ptr;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_struct_set(Ender_Value *value, void * structure)
{

}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_const_struct_set(Ender_Value *value, void * structure)
{

}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_renderer_set(Ender_Value *value, Enesim_Renderer *renderer)
{
	if (value->container->type != ENDER_RENDERER)
		return;
	value->data.ptr = renderer;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Renderer * ender_value_renderer_get(Ender_Value *value)
{
	if (value->container->type != ENDER_RENDERER)
		return NULL;
	return value->data.ptr;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_ender_set(Ender_Value *value, Ender_Element *ender)
{
	if (value->container->type != ENDER_ENDER)
		return;
	value->data.ptr = ender;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * ender_value_ender_get(Ender_Value *value)
{
	if (value->container->type != ENDER_ENDER)
		return NULL;
	return value->data.ptr;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_surface_set(Ender_Value *value, Enesim_Surface *surface)
{
	if (value->container->type != ENDER_SURFACE)
		return;
	value->data.ptr = surface;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Surface * ender_value_surface_get(Ender_Value *value)
{
	if (value->container->type != ENDER_SURFACE)
		return NULL;
	return value->data.ptr;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_pointer_set(Ender_Value *value, void *ptr, Ender_Value_Free free_cb, void *user_data)
{
	if (value->container->type != ENDER_POINTER)
		return;
	value->data.ptr = ptr;
	value->free_cb = free_cb;
	value->free_cb_data = user_data;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void * ender_value_pointer_get(Ender_Value *value)
{
	if (value->container->type != ENDER_POINTER)
		return NULL;
	return value->data.ptr;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_list_add(Ender_Value *value, Ender_Value *child)
{
	Ender_Container *sub;

	if (value->container->type != ENDER_LIST)
		return;

	sub = ender_container_compound_get(value->container, 0);
	if (sub->type == ENDER_VALUE)
	{
		value->data.ptr = eina_list_append(value->data.ptr, child);
	}
	else
	{
		printf("%s TODO\n", __FILE__);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_list_set(Ender_Value *value, Eina_List *list)
{
	if (value->container->type != ENDER_LIST)
		return;
	value->data.ptr = list;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const Eina_List * ender_value_list_get(Ender_Value *value)
{
	if (value->container->type != ENDER_LIST)
		return NULL;
	return value->data.ptr;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_value_bool_get(Ender_Value *value)
{
	if (value->container->type != ENDER_BOOL)
		return EINA_FALSE;
	return value->data.u32;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_bool_set(Ender_Value *value, Eina_Bool boolean)
{
	if (value->container->type != ENDER_BOOL)
		return;
	value->data.u32 = boolean;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_value_free(Ender_Value *value)
{
	if (value->owned)
	{
		free(value->data.ptr);
	}
	else if (value->free_cb)
	{
		value->free_cb(value, value->free_cb_data);
	}
	free(value);
}
