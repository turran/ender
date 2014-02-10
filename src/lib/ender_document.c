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
typedef struct _Ender_Document
{
	Etch *etch;
} Ender_Document;

static void _ender_document_instance_animation_node_removed_from_document_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Node *target;
	Egueb_Dom_Node *current;

	target = egueb_dom_event_target_get(ev);
	current = egueb_dom_event_target_current_get(ev);

	if (target != current)
		goto no_ourselves;
	/* remove the etch */
	printf("smil instance removed from document\n");
	egueb_smil_animation_etch_set(target, NULL);
	egueb_dom_element_process(target);
no_ourselves:
	egueb_dom_node_unref(target);
	egueb_dom_node_unref(current);
}

static void _ender_document_instance_node_inserted_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Ender_Document *thiz;
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Node *current;
	Egueb_Dom_Node *target;
	Egueb_Dom_Node *rel;
	Egueb_Dom_Node *child;

	/* check if the parent of the node inserted is the target */
	rel = egueb_dom_event_mutation_related_get(ev);
	current = egueb_dom_event_target_current_get(ev);

	if (rel != current)
		goto done;

	/* check if it is an animation */
	target = egueb_dom_event_target_get(ev);
	if (egueb_dom_node_type_get(target) != EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
		goto no_smil;
	if (!egueb_smil_is_animation(target))
		goto no_smil;

	printf("smil instance node inserted\n");
	thiz = egueb_dom_document_external_data_get(n);
	egueb_smil_animation_etch_set(target, thiz->etch);
	egueb_dom_node_event_listener_add(target,
				EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
				_ender_document_instance_animation_node_removed_from_document_cb,
				EINA_TRUE, NULL);
no_smil:
	egueb_dom_node_unref(target);
done:
	egueb_dom_node_unref(rel);
	egueb_dom_node_unref(current);
}

static void _ender_document_instance_node_removed_from_document_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Egueb_Dom_Node *n = data;

	printf("removed from document\n");
#if 0
	egueb_dom_node_event_listener_remove(ret,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_ender_document_instance_node_inserted_cb,
			EINA_TRUE, n);
#endif
}

/*----------------------------------------------------------------------------*
 *                     The exernal document interface                         *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Node * _ender_document_element_create(Egueb_Dom_Node *n,
		void *data, const char *name)
{
	/* we can only create states/state/set/animate */
	/* any other object needs to be created using the topmost ender */
	printf("creating child of name %s\n", name);
	if (!strcmp(name, "states"))
	{
		return ender_element_states_new();
	}
	else if (!strcmp(name, "state"))
	{
		return ender_element_state_new();
	}
	else if (!strcmp(name, "object"))
	{
		return ender_element_object_new();
	}
	else if (!strcmp(name, "ender"))
	{
		return ender_element_ender_new();
	}
	else if (!strcmp(name, "instance"))
	{
		return ender_element_instance_new();
	}
	else if (!strcmp(name, "scene"))
	{
		return ender_element_scene_new();
	}
	else if (!strcmp(name, "set"))
	{
		return egueb_smil_set_new();
	}
	else if (!strcmp(name, "animate"))
	{
		return egueb_smil_animate_new();
	}

	return NULL;
}

static void _ender_document_init(Egueb_Dom_Node *n, void *data)
{
}

static void _ender_document_deinit(Egueb_Dom_Node *n, void *data)
{
	Ender_Document *thiz = data;

	etch_delete(thiz->etch);
	free(thiz);
}

static Egueb_Dom_Document_External_Descriptor _descriptor = {
	/* init 		= */ _ender_document_init,
	/* deinit 		= */ _ender_document_deinit,
	/* element_create	= */ _ender_document_element_create,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * ender_document_new(void)
{
	Ender_Document *thiz;

	thiz = calloc(1, sizeof(Ender_Document));
	thiz->etch = etch_new();

	return egueb_dom_document_external_new(&_descriptor, thiz);
}

EAPI void ender_document_tick(Egueb_Dom_Node *n)
{
	Ender_Document *thiz;

	thiz = egueb_dom_document_external_data_get(n);
	etch_timer_tick(thiz->etch);
}

EAPI Egueb_Dom_Node * ender_document_instance_new(Egueb_Dom_Node *n,
		const char *id, Eina_Error *err)
{
	Egueb_Dom_Node *ret;
	Egueb_Dom_Node *rel;
	Egueb_Dom_String *sid;
	
	sid = egueb_dom_string_new_with_static_string(id);
	rel = egueb_dom_document_element_get_by_id(n, sid, err);
	egueb_dom_string_unref(sid);
	if (!rel) return NULL;

	if (!ender_element_is_object(rel))
	{
		egueb_dom_node_unref(rel);
		return NULL;
	}

	ret = egueb_dom_document_element_create(n, ENDER_ELEMENT_INSTANCE, err);
	if (!ret)
	{
		egueb_dom_node_unref(rel);
		return NULL;
	}

	ender_element_instance_relative_set(ret, rel);
	/* register some events */
	if (ret)
	{
		egueb_dom_node_event_listener_add(ret,
				EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
				_ender_document_instance_node_inserted_cb,
				EINA_TRUE, n);
		egueb_dom_node_event_listener_add(ret,
				EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
				_ender_document_instance_node_removed_from_document_cb,
				EINA_TRUE, n);
	}
	return ret;
}
