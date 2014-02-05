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
/*----------------------------------------------------------------------------*
 *                      The exernal element interface                         *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _ender_element_object_tag_name_get(
		Egueb_Dom_Node *node, void *data)
{
	return egueb_dom_string_ref(ENDER_ELEMENT_OBJECT);
}

static Eina_Bool _ender_element_object_child_appendable(Egueb_Dom_Node *n,
		void *data, Egueb_Dom_Node *child)
{
	Egueb_Dom_String *name;
	Eina_Bool ret = EINA_FALSE;

	if (egueb_dom_node_type_get(child) != EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
		return ret;

	name = egueb_dom_element_name_get(child);
	if (name == ENDER_ELEMENT_STATES)
		ret = EINA_TRUE;
	egueb_dom_string_unref(name);

	return ret;
}

static Egueb_Dom_Element_External_Descriptor _descriptor = {
	/* init 		= */ NULL,
	/* deinit 		= */ NULL,
	/* tag_name_get		= */ _ender_element_object_tag_name_get,
	/* child_appendable 	= */ _ender_element_object_child_appendable,
	/* process 		= */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool ender_element_is_object(Egueb_Dom_Node *n)
{
	Egueb_Dom_String *name;
	Eina_Bool ret = EINA_FALSE;

	if (egueb_dom_node_type_get(n) != EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
		return ret;

	name = egueb_dom_element_tag_name_get(n);
	if (name == ENDER_ELEMENT_OBJECT)
		ret = EINA_TRUE;
	egueb_dom_string_unref(name);
	return ret;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * ender_element_object_new(void)
{
	return egueb_dom_element_external_new(&_descriptor, NULL);
}
