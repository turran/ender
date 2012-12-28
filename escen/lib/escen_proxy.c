/* ESCEN - Ender's based scene loader
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
#include "Escen.h"
#include "escen_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Escen_Proxy
{
	char *name;
	char *dst_name;
	Escen_Ender *ee;
	Eina_List *sets;
	Eina_List *animations;
	Eina_List *scripts;
};

typedef struct _Escen_Proxy_Instance 
{
	Escen_Proxy *proxy;
	Escen_Instance *instance;
} Escen_Proxy_Instance;
/*----------------------------------------------------------------------------*
 *                     The element property interface                         *
 *----------------------------------------------------------------------------*/
static void _element_property_set(Ender_Element *e, Ender_Property *p, Ender_Value *v, void *data)
{
	Escen_Proxy_Instance *epi = data;
	Escen_Setter *setter;
	Eina_List *l;

	ender_element_value_set_simple(e, epi->proxy->dst_name, v);
	/* call the setters and the animations */
	EINA_LIST_FOREACH(epi->proxy->sets, l, setter)
	{
		escen_setter_instance_set(setter, epi->instance);
	}
}

static void _element_property_get(Ender_Element *e, Ender_Property *p, Ender_Value *v, void *data)
{
	Escen_Proxy *ep = data;

	ender_element_value_get_simple(e, ep->dst_name, v);
}

static void _element_property_add(Ender_Element *e, Ender_Property *p, Ender_Value *v, void *data)
{
	Escen_Proxy *ep = data;

}

static void _element_property_remove(Ender_Element *e, Ender_Property *p, Ender_Value *v, void *data)
{
	Escen_Proxy *ep = data;

}

static void _element_property_clear(Ender_Element *e, Ender_Property *p, void *data)
{
	Escen_Proxy *ep = data;

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Escen_Proxy * escen_proxy_new(Escen_Ender *ee, const char *name, const char *dst_name)
{
	Escen_Proxy *ep;

	ep = calloc(1, sizeof(Escen_Proxy));
	ep->name = strdup(name);
	ep->dst_name = strdup(dst_name);

	return ep;
}

void escen_proxy_setup(Escen_Proxy *ep, Escen_Instance *ei)
{
	Escen_Proxy_Instance *epi;
	Ender_Property *dst;
	Ender_Property *src;
	Ender_Element *ee;
	Ender_Container *ec;

	ee = escen_instance_ender_get(ei);
	dst = ender_element_property_get(ee, ep->dst_name);
	if (!dst) return;
	ec = ender_property_container_get(dst);
	/* create a new property of the same type of the
	 * origin and with the new name
	 */
	epi = calloc(1, sizeof(Escen_Proxy_Instance));
	epi->proxy = ep;
	epi->instance = ei;

	src = ender_element_property_add(ee, ep->name,
			ec,
			_element_property_get,
			_element_property_set,
			_element_property_add,
			_element_property_remove,
			_element_property_clear,
			NULL,
			EINA_FALSE,
			epi);
	if (!src)
	{
		free(epi);
		return;
	}
	/* TODO on setup we must return a pointer which is
	 * the allocated data
	 */
}

void escen_proxy_cleanup(Escen_Proxy *ep, Escen_Instance *ei)
{
	/* TODO */
	/* we must remove the newly created property */
	/* remove the private data associated with the callbacks */
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_Ender * escen_proxy_ender_get(Escen_Proxy *ep)
{
	if (!ep) return NULL;
	return ep->ee;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * escen_proxy_name_get(Escen_Proxy *ep)
{
	if (!ep) return NULL;
	return ep->name;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * escen_proxy_destination_name_get(Escen_Proxy *ep)
{
	if (!ep) return NULL;
	return ep->dst_name;
}

/* FIXME this code is the same as the state, we must refactor it to be common, maybe add some kind of interface??
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_proxy_setter_add(Escen_Proxy *es, Escen_Setter *et)
{
	if (!es) return;
	if (!et) return;

	es->sets = eina_list_append(es->sets, et);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void escen_proxy_animation_add(Escen_Proxy *es, Escen_Animation *ea)
{
	if (!es) return;
	if (!ea) return;

	es->animations = eina_list_append(es->animations, ea);
}

