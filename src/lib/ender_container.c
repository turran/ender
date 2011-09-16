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
static Ender_Container *_basic_containers[ENDER_LIST - ENDER_BOOL];

static Ender_Container * _ender_container_new(Ender_Value_Type t)
{
	Ender_Container *ec;

	ec = malloc(sizeof(Ender_Container));
	ec->type = t;
	ec->elements = NULL;
	return ec;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
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
	int i;

	_structs = eina_hash_string_superfast_new(NULL);
	/* define the common (basic) containers here */
	for (i = 0; i < sizeof(_basic_containers) / sizeof(Ender_Container *); i++)
	{
		_basic_containers[i] = _ender_container_new(i);
	}
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
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_container_new(Ender_Value_Type t)
{
	Ender_Container *ec;

	/* if it is a basic type, just get it from the list */
	if (t >= ENDER_BOOL && t < ENDER_LIST)
	{
		return _basic_containers[t];
	}
	return _ender_container_new(t);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_container_is_compound(Ender_Container *ec)
{
	//printf("is %d compound (%d %d)\n", ec->type, ENDER_LIST, ENDER_STRUCT);
	if (ec->type == ENDER_LIST || ec->type == ENDER_STRUCT)
		return EINA_TRUE;
	return EINA_FALSE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_container_compound_get(Ender_Container *ec, unsigned int idx)
{
	if (!ender_container_is_compound(ec))
		return NULL;
	return eina_list_nth(ec->elements, idx);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI size_t ender_container_size_get(Ender_Container *ec)
{
	size_t size = 0;

	switch (ec->type)
	{
		case ENDER_INT32:
		case ENDER_UINT32:
		case ENDER_COLOR:
		case ENDER_ARGB:
		case ENDER_BOOL:
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
		case ENDER_POINTER:
		size = sizeof(void *);
		break;

		default:
		break;
	}
	return size;
}

/**
 * To be documented
 * FIXME: To be fixed
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
 * To be documented
 * FIXME: To be fixed
 */
EAPI unsigned int ender_container_compound_count(Ender_Container *ec)
{
	if (!ender_container_is_compound(ec))
		return 0;
	return eina_list_count(ec->elements);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_container_add(Ender_Container *ec, Ender_Container *sub)
{
	ssize_t prev_offset = 0;
	size_t prev_size = 0;

	if (!ender_container_is_compound(ec))
		return;
	if (!sub)
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

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Value_Type ender_container_type_get(Ender_Container *c)
{
	return c->type;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_container_ref(Ender_Container *c)
{

}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_container_unref(Ender_Container *c)
{

}
