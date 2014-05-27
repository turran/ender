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
typedef struct _Ender_Attr_Font
{
	Ender_Attr_Font_Set set;
	Ender_Attr_Font_Get get;
	Egueb_Css_Font font;
} Ender_Attr_Font;
/*----------------------------------------------------------------------------*
 *                        The exernal attr interface                           *
 *----------------------------------------------------------------------------*/
static void * _ender_attr_font_init(Egueb_Dom_Node *n)
{
	Ender_Attr_Font *thiz;

	thiz = calloc(1, sizeof(Ender_Attr_Font));
	return thiz;
}

static void _ender_attr_font_deinit(Egueb_Dom_Node *n, void *data)
{
	free(data);
}

static Eina_Bool _ender_attr_font_value_get(Egueb_Dom_Node *n, void *data,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	Ender_Attr_Font *thiz = data;
	Egueb_Dom_Node *owner;
	void *o;

	owner = egueb_dom_attr_owner_get(n);
	if (!owner) return EINA_FALSE;

	o = ender_element_instance_object_get(owner);
	thiz->get(o, &thiz->font);

	value->data.ptr = &thiz->font;
	value->owned = EINA_FALSE;

	egueb_dom_node_unref(owner);

	return EINA_TRUE;
}

static Eina_Bool _ender_attr_font_value_set(Egueb_Dom_Node *n, void *data,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	Ender_Attr_Font *thiz = data;
	Egueb_Dom_Node *owner;
	void *o;

	owner = egueb_dom_attr_owner_get(n);
	if (!owner) return EINA_FALSE;

	o = ender_element_instance_object_get(owner);
	thiz->set(o, value->data.ptr);
	egueb_dom_node_unref(owner);

	return EINA_TRUE;
}

static const Egueb_Dom_Value_Descriptor * _ender_attr_font_value_descriptor_get(
		Egueb_Dom_Node *n, void *data)
{
	return egueb_css_font_descriptor_get();
}

static Egueb_Dom_Attr_External_Descriptor _descriptor = {
	/* init 		= */ _ender_attr_font_init,
	/* deinit 		= */ _ender_attr_font_deinit,
	/* value_descriptor_get	= */ _ender_attr_font_value_descriptor_get,
	/* value_get 		= */ _ender_attr_font_value_get,
	/* value_set	 	= */ _ender_attr_font_value_set,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * ender_attr_font_new(const char *name,
		Ender_Attr_Font_Get get, Ender_Attr_Font_Set set)
{
	Ender_Attr_Font *thiz;
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
