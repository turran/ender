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
typedef struct _Ender_Attr_Primitive
{
	Ender_Attr_Primitive_Set set;
	Ender_Attr_Primitive_Get get;
	const Egueb_Dom_Value_Descriptor *descriptor;
	void *tmp_primitive;
} Ender_Attr_Primitive;
/*----------------------------------------------------------------------------*
 *                        The exernal attr interface                           *
 *----------------------------------------------------------------------------*/
static void * _ender_attr_primitive_init(Egueb_Dom_Node *n)
{
	Ender_Attr_Primitive *thiz;

	thiz = calloc(1, sizeof(Ender_Attr_Primitive));
	return thiz;
}

static void _ender_attr_primitive_deinit(Egueb_Dom_Node *n, void *data)
{
	Ender_Attr_Primitive *thiz = data;

	free(thiz->tmp_primitive);
	free(thiz);
}

static Eina_Bool _ender_attr_primitive_value_get(Egueb_Dom_Node *n, void *data,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	Ender_Attr_Primitive *thiz = data;
	Egueb_Dom_Node *owner;
	void *o;

	owner = egueb_dom_attr_owner_get(n);
	if (!owner) return EINA_FALSE;

	o = ender_element_instance_object_get(owner);
	thiz->get(o, thiz->tmp_primitive);
	value->data.ptr = thiz->tmp_primitive;
	value->owned = EINA_FALSE;
	egueb_dom_node_unref(owner);

	return EINA_TRUE;
}

static Eina_Bool _ender_attr_primitive_value_set(Egueb_Dom_Node *n, void *data,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	Ender_Attr_Primitive *thiz = data;
	Egueb_Dom_Node *owner;
	void *o;

	owner = egueb_dom_attr_owner_get(n);
	if (!owner) return EINA_FALSE;

	o = ender_element_instance_object_get(owner);
	thiz->set(o, value->data.ptr);
	egueb_dom_node_unref(owner);

	return EINA_TRUE;
}

static const Egueb_Dom_Value_Descriptor * _ender_attr_primitive_value_descriptor_get(
		Egueb_Dom_Node *n, void *data)
{
	Ender_Attr_Primitive *thiz = data;

	return thiz->descriptor;
}

static Egueb_Dom_Attr_External_Descriptor _descriptor = {
	/* init 		= */ _ender_attr_primitive_init,
	/* deinit 		= */ _ender_attr_primitive_deinit,
	/* value_descriptor_get	= */ _ender_attr_primitive_value_descriptor_get,
	/* value_get 		= */ _ender_attr_primitive_value_get,
	/* value_set	 	= */ _ender_attr_primitive_value_set,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * ender_attr_primitive_new(Egueb_Dom_String *name,
		Ender_Attr_Primitive_Get get, Ender_Attr_Primitive_Set set,
		size_t size, const Egueb_Dom_Value_Descriptor *descriptor)
{
	Ender_Attr_Primitive *thiz;
	Egueb_Dom_Node *n;

	n = egueb_dom_attr_external_new(&_descriptor);
	egueb_dom_attr_init(n, name, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	thiz = egueb_dom_attr_external_data_get(n);
	thiz->set = set;
	thiz->get = get;
	thiz->descriptor = descriptor;
	thiz->tmp_primitive = calloc(1, size);
	
	return n;
}

