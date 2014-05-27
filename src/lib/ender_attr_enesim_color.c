/* ENDER - Enesim's descriptor library
 * Copyright (C) 2010 - 2012 Jorge Luis Zapata
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
typedef struct _Ender_Attr_Enesim_Color
{
	Ender_Attr_Enesim_Color_Set set;
	Ender_Attr_Enesim_Color_Get get;
} Ender_Attr_Enesim_Color;
/*----------------------------------------------------------------------------*
 *                        The exernal attr interface                           *
 *----------------------------------------------------------------------------*/
static void * _ender_attr_enesim_color_init(Egueb_Dom_Node *n)
{
	Ender_Attr_Enesim_Color *thiz;

	thiz = calloc(1, sizeof(Ender_Attr_Enesim_Color));
	return thiz;
}

static void _ender_attr_enesim_color_deinit(Egueb_Dom_Node *n, void *data)
{
	free(data);
}

static Eina_Bool _ender_attr_enesim_color_value_get(Egueb_Dom_Node *n, void *data,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	Ender_Attr_Enesim_Color *thiz = data;
	Egueb_Dom_Node *owner;
	Enesim_Color c;
	void *o;

	owner = egueb_dom_attr_owner_get(n);
	if (!owner) return EINA_FALSE;

	o = ender_element_instance_object_get(owner);
	c = thiz->get(o);
	/* convert it to argb */
	value->data.i32 = enesim_color_argb_to(c);
	egueb_dom_node_unref(owner);

	return EINA_TRUE;
}

static Eina_Bool _ender_attr_enesim_color_value_set(Egueb_Dom_Node *n, void *data,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	Ender_Attr_Enesim_Color *thiz = data;
	Egueb_Dom_Node *owner;
	Enesim_Color c;
	void *o;

	owner = egueb_dom_attr_owner_get(n);
	if (!owner) return EINA_FALSE;

	o = ender_element_instance_object_get(owner);
	/* convert it from argb */
	c = enesim_color_argb_from(value->data.i32);
	thiz->set(o, c);
	egueb_dom_node_unref(owner);

	return EINA_TRUE;
}

static const Egueb_Dom_Value_Descriptor * _ender_attr_enesim_color_value_descriptor_get(
		Egueb_Dom_Node *n, void *data)
{
	return egueb_css_color_descriptor_get();
}

static Egueb_Dom_Attr_External_Descriptor _descriptor = {
	/* init 		= */ _ender_attr_enesim_color_init,
	/* deinit 		= */ _ender_attr_enesim_color_deinit,
	/* value_descriptor_get	= */ _ender_attr_enesim_color_value_descriptor_get,
	/* value_get 		= */ _ender_attr_enesim_color_value_get,
	/* value_set	 	= */ _ender_attr_enesim_color_value_set,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * ender_attr_enesim_color_new(const char *name,
		Ender_Attr_Enesim_Color_Get get, Ender_Attr_Enesim_Color_Set set)
{
	Ender_Attr_Enesim_Color *thiz;
	Egueb_Dom_String *s;
	Egueb_Dom_Node *n;

	n = egueb_dom_attr_external_new(&_descriptor);
	s = egueb_dom_string_new_with_static_string(name);
	egueb_dom_attr_init(n, s, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	thiz = egueb_dom_attr_external_data_get(n);
	thiz->set = set;
	thiz->get = get;
	
	return n;
}

