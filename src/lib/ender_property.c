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
struct _Ender_Property
{
	char *name;
	Ender_Property_Getter get;
	Ender_Property_Setter set;
	Ender_Property_Add add;
	Ender_Property_Remove remove;
	Ender_Property_Clear clear;
	Ender_Property_Is_Set is_set;
	Ender_Container *container;
	Eina_Bool relative;
	Ender_Property_Free free;
	void *data;
};

static void _ender_property_mutation_dispatch(Ender_Element *e,
		Ender_Value *v,
		Ender_Property *p,
		Ender_Event_Mutation_Type type)
{
	Ender_Event_Mutation ev_mutation;
	Ender_Event_Mutation_Property ev_prop;
	char ev_prop_name[PATH_MAX];

	ev_mutation.name = p->name;
	ev_mutation.value = v;
	ev_mutation.type = type;
	ender_event_dispatch(e, "Mutation", &ev_mutation);

	ev_prop.value = v;
	ev_prop.type = type;
	strcpy(ev_prop_name, "Mutation:");
	strcat(ev_prop_name, p->name);
	ender_event_dispatch(e, ev_prop_name, &ev_prop);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Property * ender_property_new(const char *name,
		Ender_Container *ec,
		Ender_Property_Getter get,
		Ender_Property_Setter set,
		Ender_Property_Add add,
		Ender_Property_Remove remove,
		Ender_Property_Clear clear,
		Ender_Property_Is_Set is_set,
		Eina_Bool relative,
		Ender_Property_Free free,
		void *data)
{
	Ender_Property *prop;

	prop = calloc(1, sizeof(Ender_Property));
	prop->name = strdup(name);
	prop->get = get;
	prop->set = set;
	prop->add = add;
	prop->remove = remove;
	prop->clear = clear;
	/* own the container */
	prop->container = ec;
	prop->relative = relative;
	prop->data = data;
	prop->free = free;

	return prop;
}

void ender_property_free(Ender_Property *thiz)
{
	if (thiz->name)
		free(thiz->name);
	if (thiz->free)
		thiz->free(thiz->data);
	ender_container_unref(thiz->container);
}

/* TODO add guards, the value type must be equal to the property type */
void ender_property_element_value_set(Ender_Property *ep, Ender_Element *e,
		Ender_Value *v)
{
	if (!ep->set) return;
	ep->set(ep, e, v, ep->data);

	_ender_property_mutation_dispatch(e, v, ep, ENDER_EVENT_MUTATION_SET);
}

void ender_property_element_value_get(Ender_Property *ep, Ender_Element *e,
		Ender_Value *v)
{
	if (!ep->get) return;
	ep->get(ep, e, v, ep->data);
}

void ender_property_element_value_add(Ender_Property *ep, Ender_Element *e,
		Ender_Value *v)
{
	if (!ep->add) return;
	ep->add(ep, e, v, ep->data);

	_ender_property_mutation_dispatch(e, v, ep, ENDER_EVENT_MUTATION_ADD);
}

void ender_property_element_value_remove(Ender_Property *ep, Ender_Element *e,
		Ender_Value *v)
{
	if (!ep->remove) return;
	ep->remove(ep, e, v, ep->data);

	_ender_property_mutation_dispatch(e, v, ep, ENDER_EVENT_MUTATION_REMOVE);
}

void ender_property_element_value_clear(Ender_Property *ep, Ender_Element *e)
{
	if (!ep->clear) return;
	ep->clear(ep, e, ep->data);

	_ender_property_mutation_dispatch(e, NULL, ep, ENDER_EVENT_MUTATION_CLEAR);
}

Eina_Bool ender_property_element_value_is_set(Ender_Property *ep, Ender_Element *e)
{
	if (!ep->is_set) return EINA_TRUE;
	return ep->is_set(ep, e, ep->data);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_property_container_get(Ender_Property *p)
{
	return p->container;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Value_Type ender_property_type_get(Ender_Property *p)
{
	return p->container->type;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_property_is_relative(Ender_Property *p)
{
	return p->relative;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * ender_property_name_get(Ender_Property *p)
{
	return p->name;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Property_Flag ender_property_flags_get(Ender_Property *p)
{
	Ender_Property_Flag flags = 0;

	if (!p) return flags;
	if (p->get) flags |= ENDER_GET;
	if (p->set) flags |= ENDER_SET;
	if (p->add) flags |= ENDER_ADD;
	if (p->remove) flags |= ENDER_REMOVE;
	if (p->clear) flags |= ENDER_CLEAR;
	if (p->is_set) flags |= ENDER_IS_SET;

	return flags;
}
