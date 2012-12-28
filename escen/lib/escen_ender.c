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
struct _Escen_Ender
{
	Escen *escen;
	Ender_Descriptor *descriptor;
	char *name;
	Eina_Ordered_Hash *states;
	Eina_List *proxies;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Escen_Ender * escen_ender_new(Escen *escen, Ender_Descriptor *descriptor, const char *name)
{
	Escen_Ender *ee;

	ee = calloc(1, sizeof(Escen_Ender));
	ee->escen = escen;
	ee->descriptor = descriptor;
	ee->name = strdup(name);
	ee->states = eina_ordered_hash_new(NULL);

	return ee;
}

void escen_ender_instance_setup(Escen_Ender *ee, Escen_Instance *ei)
{
	Escen_Proxy *p;
	Eina_List *l;

	EINA_LIST_FOREACH(ee->proxies, l, p)
	{
		escen_proxy_setup(p, ei);
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen * escen_ender_escen_get(Escen_Ender *ee)
{
	if (!ee) return NULL;
	return ee->escen;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * escen_ender_name_get(Escen_Ender *e)
{
	if (!e) return NULL;
	return e->name;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_Proxy * escen_ender_proxy_add(Escen_Ender *ee, const char *name, const char *dst_name)
{
	Escen_Proxy *ep;

	ep = escen_proxy_new(ee, name, dst_name);
	ee->proxies = eina_list_append(ee->proxies, ep);

	return ep;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_State * escen_ender_state_add(Escen_Ender *ee, const char *name)
{
	Escen_State *es;

	if (!ee) return NULL;

	es = escen_state_new(ee, name);
	if (!es) return NULL;

	eina_ordered_hash_add(ee->states, name, es);

	return es;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_State * escen_ender_state_alias_add(Escen_Ender *ee, const char *name, const char *alias)
{
	Escen_State *es;

	if (!ee) return NULL;

	es = eina_ordered_hash_find(ee->states, name);
	if (!es) return NULL;

	eina_ordered_hash_add(ee->states, alias, es);

	return es;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI int escen_ender_state_count(Escen_Ender *ee)
{
	if (!ee) return 0;
	return eina_ordered_hash_count(ee->states);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_State * escen_ender_state_nth_get(Escen_Ender *ee, int nth)
{
	Escen_State *es = NULL;

	if (!ee) return NULL;
	return eina_ordered_hash_nth_get(ee->states, nth);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Descriptor * escen_ender_descriptor_get(Escen_Ender *ee)
{
	if (!ee) return NULL;
	return ee->descriptor;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Escen_State * escen_ender_state_get(Escen_Ender *ee, const char *name)
{
	Escen_State *es;

	if (!ee) return NULL;
	es = eina_ordered_hash_find(ee->states, name);

	return es;
}

