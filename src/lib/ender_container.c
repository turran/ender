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
static Eina_Hash *_structs = NULL;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Container * ender_container_new(Ender_Property_Type t)
{
	Ender_Container *prop;

	prop = malloc(sizeof(Ender_Property));
	prop->type = t;
	prop->elements = NULL;
	return prop;
}

void ender_container_delete(Ender_Container *d)
{
	/* FIXME call the delete for each descriptor */
	free(d);
}

void ender_container_register(const char *name, Ender_Container *ec)
{
	if (ec->type != ENDER_STRUCT) return;
	eina_hash_add(_structs, name, ec);
}

Ender_Container * ender_container_find(const char *name)
{
	Ender_Container *ec;

	ec = eina_hash_find(_structs, name);
	return ec;
}

void ender_container_init(void)
{
	_structs = eina_hash_string_superfast_new(NULL);
}

void ender_container_shutdown(void)
{
	/* TODO remove every container */
	eina_hash_free(_structs);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 *
 */
EAPI Eina_Bool ender_container_is_compound(Ender_Container *ec)
{
	//printf("is %d compound (%d %d)\n", ec->type, ENDER_LIST, ENDER_STRUCT);
	if (ec->type == ENDER_LIST || ec->type == ENDER_STRUCT)
		return EINA_TRUE;
	return EINA_FALSE;
}

/**
 *
 */
EAPI Ender_Container * ender_container_compound_get(Ender_Container *ec, unsigned int idx)
{
	if (!ender_container_is_compound(ec))
		return NULL;
	return eina_list_nth(ec->elements, idx);
}

/**
 *
 */
EAPI size_t ender_container_size_get(Ender_Container *ec)
{
	size_t size = 0;

	switch (ec->type)
	{
		case ENDER_INT32:
		case ENDER_UINT32:
		case ENDER_ARGB:
		size = sizeof(int32_t);
		break;

		case ENDER_DOUBLE:
		size = sizeof(double);
		break;

		case ENDER_STRING:
		size = sizeof(char *);
		break;

		case ENDER_MATRIX:
		case ENDER_RENDERER:
		case ENDER_SURFACE:
		case ENDER_ENDER:
		case ENDER_LIST:
		case ENDER_STRUCT:
		size = sizeof(void *);
		break;
	}
	return size;
}

/**
 *
 */
EAPI size_t ender_container_compound_size_get(Ender_Container *ec)
{
	Ender_Container *sub;
	Eina_List *l;
	size_t size = 0;

	if (!ender_container_is_compound(ec))
		return size;
	EINA_LIST_FOREACH(ec->elements, l, sub)
	{
		size += ender_container_size_get(sub);
	}
	return size;
}

/**
 *
 */
EAPI unsigned int ender_container_compound_count(Ender_Container *ec)
{
	if (!ender_container_is_compound(ec))
		return 0;
	return eina_list_count(ec->elements);
}

/**
 *
 */
EAPI void ender_container_add(Ender_Container *ec, Ender_Container *sub)
{
	ssize_t prev_offset = 0;
	size_t prev_size = 0;

	if (!ender_container_is_compound(ec))
		return;
	/* FIXME for list type, only limit the number
	 * of child to one
	 */
	if (ec->elements)
	{
		Ender_Container *prev;

		prev = eina_list_data_get(eina_list_last(ec->elements));
		prev_offset = prev->offset;
		prev_size = ender_container_size_get(prev);
	}
	ec->elements = eina_list_append(ec->elements, sub);
	sub->offset = prev_offset + prev_size;
}

EAPI Ender_Property_Type ender_container_type(Ender_Container *c)
{
	return c->type;
}
