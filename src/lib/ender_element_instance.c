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
typedef struct _Ender_Element_Instance {
	Egueb_Dom_Node *rel;
	void *object;
} Ender_Element_Instance;

static Eina_Bool _ender_element_instance_state_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *state, Eina_Error *err)
{
	Egueb_Dom_Node *child;
	Eina_Bool ret = EINA_TRUE;

	/* clone every child 'live' */
	child = egueb_dom_node_child_first_get(state);
	while (child)
	{
		Egueb_Dom_Node *clone;
		Egueb_Dom_Node *next;

		/* add the children to the instance */
		clone = egueb_dom_node_clone(child, EINA_TRUE, EINA_TRUE, err);
		if (!clone)
		{
			egueb_dom_node_unref(child);
			ret = EINA_FALSE;
			goto done;
		}
		if (!egueb_dom_node_child_append(n, clone, err))
		{
			egueb_dom_node_unref(child);
			ret = EINA_FALSE;
			goto done;
		}
		next = egueb_dom_node_sibling_next_get(child);
		egueb_dom_node_unref(child);
		child = next;
	}
done:
	/* process ourselves */
	egueb_dom_element_process(n);
	return ret;
}
/*----------------------------------------------------------------------------*
 *                      The exernal element interface                         *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _ender_element_instance_tag_name_get(
		Egueb_Dom_Node *node, void *data)
{
	return egueb_dom_string_ref(ENDER_ELEMENT_INSTANCE);
}

static void * _ender_element_instance_init(Egueb_Dom_Node *node)
{
	Ender_Element_Instance *thiz;

	/* TODO create the object */
	thiz = calloc(1, sizeof(Ender_Element_Instance));

	return thiz;
}

static void _ender_element_instance_deinit(Egueb_Dom_Node *node,
		void *data)
{
	Ender_Element_Instance *thiz = data;
	egueb_dom_node_unref(thiz->rel);
	/* TODO remove the object */
}

static Eina_Bool _ender_element_instance_process(Egueb_Dom_Node *node,
		void *data)
{
	Egueb_Dom_Node *child;

	printf("processing\n");
	/* iterate over the children and process there too */
	child = egueb_dom_node_child_first_get(node);
	while (child)
	{
		Egueb_Dom_Node *next;
		if (egueb_dom_node_type_get(child) != EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
			goto next;
		/* in case of a smil element, set the etch there */
		if (egueb_smil_is_animation(child))
		{
#if 0
			Etch *etch;
			egueb_smil_animation_etch_set(child, etch);
#endif
		}

		egueb_dom_element_process(child);
next:
		next = egueb_dom_node_sibling_next_get(child);
		egueb_dom_node_unref(child);
		child = next;
	}
}

static Egueb_Dom_Element_External_Descriptor _descriptor = {
	/* init 		= */ _ender_element_instance_init,
	/* deinit 		= */ _ender_element_instance_deinit,
	/* tag_name_get		= */ _ender_element_instance_tag_name_get,
	/* child_appendable 	= */ NULL,
	/* process 		= */ _ender_element_instance_process,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Node * ender_element_instance_new(void)
{
	Egueb_Dom_Node *n;

	n = egueb_dom_element_external_new(&_descriptor);
	return n;
}

void ender_element_instance_relative_set(Egueb_Dom_Node *n, Egueb_Dom_Node *rel)
{
	Ender_Element_Instance *thiz;
	thiz = egueb_dom_element_external_data_get(n);
	thiz->rel = rel;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void * ender_element_instance_object_get(Egueb_Dom_Node *n)
{
	Ender_Element_Instance *thiz;

	thiz = egueb_dom_element_external_data_get(n);
	return thiz->object;
}

EAPI Eina_Bool ender_element_instance_state_set(Egueb_Dom_Node *n, const char *s,
		Eina_Error *err)
{
	Ender_Element_Instance *thiz;
	Egueb_Dom_Node *states;
	Egueb_Dom_Node *state;
	Eina_Bool ret = EINA_FALSE;

	printf("setting state %s\n", s);

	thiz = egueb_dom_element_external_data_get(n);
	if (!thiz) return ret;

	if (!thiz->rel) return ret;

	/* TODO remove every current child */

	states = egueb_dom_node_child_first_get(thiz->rel);
	if (!states) return ret;

	/* look for the state child that has the name 'state' */
	state = egueb_dom_node_child_first_get(states);
	while (state)
	{
		Egueb_Dom_Node *next;
		Egueb_Dom_String *element_name;
		Egueb_Dom_String *state_name;
		Egueb_Dom_String *name;
		Eina_Bool is_state = EINA_FALSE;

		if (egueb_dom_node_type_get(state) != EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
			goto next;
		element_name = egueb_dom_element_name_get(state);
		if (element_name == ENDER_ELEMENT_STATE)
			is_state = EINA_TRUE;
		egueb_dom_string_unref(element_name);
		if (!is_state)
			goto next;

		/* TODO instead of getting the string, get the list of strings directly (for the alias) */
		name = egueb_dom_string_new_with_static_string("name");
		state_name = egueb_dom_element_attribute_get(state, name);
		if (egueb_dom_string_is_valid(state_name) && !strcmp(s,
				egueb_dom_string_string_get(state_name)))
		{
			ret = _ender_element_instance_state_set(n, state, err);
			egueb_dom_node_unref(state);
			goto done;
		}
next:
		next = egueb_dom_node_sibling_next_get(state);
		egueb_dom_node_unref(state);
		state = next;
	}
done:
	egueb_dom_node_unref(states);
	return ret;
}
