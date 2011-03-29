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
typedef void (*Ender_Value_Set)(Ender_Value *v, Ender_Setter set,
		Ender *e, Ender *parent, va_list arg);
typedef void (*Ender_Value_Get)(Ender_Value *v, Ender_Getter get,
		Ender *e, Ender *parent, va_list arg);

struct _Ender_Value
{
	Ender_Container *container;
	union {
		int32_t i32;
		uint32_t u32;
		double d;
		void *ptr;
	} data;
};

static Ender_Value * _ender_value_new(Ender_Container *ec)
{
	Ender_Value *ev;

	ev = calloc(1, sizeof(Ender_Value));
	ev->container = ec;

	return ev;
}


static void _ender_value_uint32_set(Ender_Value *v, Ender *e, Ender *parent,
		va_list arg)
{
	if (parent)
		set(parent, e, va_arg(arg, uint32_t));
}

static _setters[ENDER_PROPERTY_TYPES] = {
};

static _getters[ENDER_PROPERTY_TYPES] = {
};

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Value * ender_basic_new(Ender_Property_Type type)
{
	Ender_Container *ec;
	Ender_Value *ev;

	if (type == ENDER_LIST || type == ENDER_STRUCT)
		return NULL;

	ec = ender_container_new(type);
	return _ender_value_new(ec);
}

EAPI Ender_Value * ender_new_container_from(Ender_Container *ec)
{
	return _ender_value_new(ec);	
}

EAPI Ender_Value * ender_list_new(Ender_Property_Type child_type)
{

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

EAPI void ender_value_argb_set(Ender_Value *value, Enesim_Color argb)
{
	if (value->container->type != ENDER_ARGB)
		return;
	value->data.u32 = argb;
}

EAPI void ender_value_matrix_set(Ender_Value *value, Enesim_Matrix *matrix)
{

}

EAPI void ender_value_string_set(Ender_Value *value, char * string)
{

}

EAPI void ender_value_const_string_set(Ender_Value *value, const char * string)
{

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

}

EAPI void ender_value_list_add(Ender_Value *value, Ender_Value *child)
{

}

EAPI void ender_value_free(Ender_Value *v)
{
	free(v);
}
