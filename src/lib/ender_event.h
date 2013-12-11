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

#ifndef _ENDER_EVENT_H
#define _ENDER_EVENT_H

/**
 * @defgroup Ender_Event_Group Event
 * @{
 */

typedef void (*Ender_Event_Callback)(Ender_Element *e, const char *event_name, void *event_data, void *data);

typedef enum _Ender_Event_Mutation_Type
{
	ENDER_EVENT_MUTATION_SET,
	ENDER_EVENT_MUTATION_ADD,
	ENDER_EVENT_MUTATION_REMOVE,
	ENDER_EVENT_MUTATION_CLEAR,
} Ender_Event_Mutation_Type;

typedef struct _Ender_Event_Mutation
{
	const char *name;
	const Ender_Value *value;
	Ender_Event_Mutation_Type type;
} Ender_Event_Mutation;

typedef struct _Ender_Event_Mutation_Property
{
	const Ender_Value *value;
	Ender_Event_Mutation_Type type;
} Ender_Event_Mutation_Property;

typedef struct _Ender_Listener Ender_Listener;

EAPI Ender_Listener * ender_event_listener_add(Ender_Element *e, const char *event_name, Ender_Event_Callback cb, void *data);
EAPI void ender_event_listener_remove(Ender_Listener *l);
EAPI void ender_event_listener_remove_full(Ender_Element *e, const char *event_name, Ender_Event_Callback cb, void *data);
EAPI void ender_event_dispatch(Ender_Element *e, const char *event_name, void *event_data);


/**
 * @}
 */


