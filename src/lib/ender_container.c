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

/* In order to serlaize values we need:
 * 1. Simple container types (int, double, etc) just require the
 * main eet data descriptor to be the Ender_Value_Data union
 * so we just "decode" into it directly
 * 2. Lists, require that the children are allocated and thus
 * we need to create a data descriptor for each basic type. In case the
 * list has a compound type like a struct or union we need to
 * be able to create such malloced data by eet itself
 * 3. Unions/Structs requires eet to define its descriptor dynamically
 * based on the children container type not the container descriptor
 * 4. Pointers (enders, objects, surfaces, etc) require the user to
 * provide an "id" number for them. This "id" must be provided
 * and calculated by the user. Given that it is the most
 * difficult part, it will be done at the end.
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Ender_Container_Sub
{
	char *name;
	Ender_Container *c;
} Ender_Container_Sub;

static Eina_Hash *_structs = NULL;
static Ender_Container *_basic_containers[ENDER_LIST - ENDER_BOOL];
#ifdef BUILD_SERIALIZE
static Eet_Data_Descriptor *_value_descriptors[ENDER_PROPERTY_TYPES];

static Eet_Data_Descriptor * _ender_container_value_descriptor_new(Ender_Value_Type t)
{
	Eet_Data_Descriptor *edd = NULL;
	Eet_Data_Descriptor_Class eddc;

       	eet_eina_stream_data_descriptor_class_set(&eddc, sizeof(eddc),
			ender_value_type_string_to(t),
			sizeof(Ender_Value));
	edd = eet_data_descriptor_stream_new(&eddc);
	switch (t)
	{
		case ENDER_BOOL:
		EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Ender_Value, "name", data.i32, EET_T_CHAR);
		break;

		case ENDER_UINT32:
		case ENDER_INT32:
		case ENDER_UINT64:
		case ENDER_INT64:
		case ENDER_DOUBLE:
		case ENDER_COLOR:
		case ENDER_ARGB:
		case ENDER_STRING:
		case ENDER_MATRIX:
		case ENDER_OBJECT:
		case ENDER_SURFACE:
		case ENDER_ENDER:
		case ENDER_POINTER:
		case ENDER_VALUE:
		/* compound types */
		case ENDER_LIST:
		case ENDER_STRUCT:
		case ENDER_UNION:
		break;
	}
	return edd;
}
#endif


static Ender_Container * _ender_container_new(Ender_Value_Type t)
{
	Ender_Container *thiz;
#if BUILD_SERIALIZE
	Eet_Data_Descriptor *edd = NULL;
	Eet_Data_Descriptor_Class eddc;
#endif

	thiz = calloc(1, sizeof(Ender_Container));
	thiz->type = t;
	thiz->ref = 1;
#if BUILD_SERIALIZE
	switch (t)
	{
		case ENDER_BOOL:
		break;

		case ENDER_UINT32:
		case ENDER_INT32:
		case ENDER_UINT64:
		case ENDER_INT64:
		case ENDER_DOUBLE:
		case ENDER_COLOR:
		case ENDER_ARGB:
		case ENDER_STRING:
		case ENDER_MATRIX:
		case ENDER_OBJECT:
		case ENDER_SURFACE:
		case ENDER_ENDER:
		case ENDER_POINTER:
		case ENDER_VALUE:
		/* compound types */
		case ENDER_LIST:
		case ENDER_STRUCT:
		case ENDER_UNION:
		break;
	}
	thiz->eet = edd;
#endif
	return thiz;
}

static void _ender_container_delete(Ender_Container *thiz)
{
	Ender_Container_Sub *s;

	EINA_LIST_FREE(thiz->elements, s)
	{
		if (s->name)
			free(s->name);
		ender_container_unref(s->c);
		free(s);
	}
	if (thiz->registered_name)
		free(thiz->registered_name);
	free(thiz);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ender_container_init(void)
{
	int i;

	_structs = eina_hash_string_superfast_new((Eina_Free_Cb)ender_container_unref);
	/* define the common (basic) containers here */
	for (i = 0; i < sizeof(_basic_containers) / sizeof(Ender_Container *); i++)
	{
		_basic_containers[i] = _ender_container_new(i);
	}
}

void ender_container_shutdown(void)
{
	int i;

	eina_hash_free(_structs);
	for (i = 0; i < sizeof(_basic_containers) / sizeof(Ender_Container *); i++)
	{
		_basic_containers[i] = ender_container_unref(_basic_containers[i]);
		if (_basic_containers[i])
			printf("refcount error %d on type %s\n",
					_basic_containers[i]->ref,
					ender_value_type_string_to(_basic_containers[i]->type));
	}
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
		return ender_container_ref(_basic_containers[t]);
	}
	return _ender_container_new(t);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_container_ref(Ender_Container *thiz)
{
	thiz->ref++;
	return thiz;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_container_unref(Ender_Container *thiz)
{
	thiz->ref--;
	if (!thiz->ref)
	{
		_ender_container_delete(thiz);
		return NULL;
	}
	return thiz;
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_container_register(Ender_Container *thiz, const char *name)
{
	if (thiz->type != ENDER_STRUCT && thiz->type != ENDER_UNION) return;
	if (!name) return;

	thiz->registered_name = strdup(name);
	eina_hash_add(_structs, thiz->registered_name, thiz);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_container_find(const char *name)
{
	Ender_Container *ec;

	ec = eina_hash_find(_structs, name);
	if (!ec) return NULL;
	return ender_container_ref(ec);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_container_is_compound(Ender_Container *ec)
{
	if (ec->type == ENDER_LIST || ec->type == ENDER_STRUCT || ec->type == ENDER_UNION)
		return EINA_TRUE;
	return EINA_FALSE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_container_compound_get(Ender_Container *ec, unsigned int idx)
{
	Ender_Container_Sub *sub;

	if (!ender_container_is_compound(ec))
		return NULL;
	sub = eina_list_nth(ec->elements, idx);
	return sub->c;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_container_compound_get_by_name(Ender_Container *ec, const char *name, unsigned int *idx)
{
	Ender_Container_Sub *sub = NULL;
	Eina_List *l;
	int i = 0;

	if (!ender_container_is_compound(ec))
		return NULL;
	if (!name) return NULL;
	EINA_LIST_FOREACH(ec->elements, l, sub)
	{
		if (!strcmp(sub->name, name))
		{
			if (idx) *idx = i;
			return sub->c;
		}
		i++;
	}
	return NULL;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_container_compound_get_extended(Ender_Container *ec, unsigned int idx, Ender_Container **c, const char **name)
{
	Ender_Container_Sub *sub;

	if (!ender_container_is_compound(ec))
	{
		if (c) *c = NULL;
		if (name) *name = NULL;
		return;
	}
	sub = eina_list_nth(ec->elements, idx);
	if (c) *c = sub->c;
	if (name) *name = sub->name;
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

		case ENDER_INT64:
		case ENDER_UINT64:
		size = sizeof(int64_t);
		break;

		case ENDER_DOUBLE:
		size = sizeof(double);
		break;

		case ENDER_STRING:
		size = sizeof(char *);
		break;

		case ENDER_MATRIX:
		size = sizeof(Enesim_Matrix);
		break;

		case ENDER_OBJECT:
		case ENDER_SURFACE:
		case ENDER_ENDER:
		case ENDER_LIST:
		case ENDER_STRUCT:
		case ENDER_UNION:
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
	Ender_Container_Sub *sub;
	Eina_List *l;
	size_t size = 0;

	switch (ec->type)
	{
		case ENDER_STRUCT:
		EINA_LIST_FOREACH(ec->elements, l, sub)
		{
			if (sub->c->type == ENDER_STRUCT || sub->c->type == ENDER_UNION)
				size += ender_container_compound_size_get(sub->c);
			else
				size += ender_container_size_get(sub->c);
		}
		break;

		case ENDER_LIST:
		if (!ec->elements) break;
		sub = eina_list_data_get(ec->elements);
		size = ender_container_size_get(sub->c);
		break;

		case ENDER_UNION:
		EINA_LIST_FOREACH(ec->elements, l, sub)
		{
			size_t new_size;

			if (sub->c->type == ENDER_STRUCT || sub->c->type == ENDER_UNION)
				new_size = ender_container_compound_size_get(sub->c);
			else
				new_size = ender_container_size_get(sub->c);
			if (new_size > size)
				size = new_size;
		}
		/* add the space for the integer type */
		size += 4;
		break;

		default:
		return 0;
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
	if (!ec->elements) return 0;
	return eina_list_count(ec->elements);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_container_add(Ender_Container *ec, const char *name, Ender_Container *s)
{
	Ender_Container_Sub *sub;
	ssize_t prev_offset = 0;
	size_t prev_size = 0;

	if (!ec) return;
	if (!s) return;

	if (!ender_container_is_compound(ec))
		return;

	sub = calloc(1, sizeof(Ender_Container_Sub));
	if (name)
		sub->name = strdup(name);
	/* own the sub container */
	sub->c = s;
	switch (ec->type)
	{
		case ENDER_STRUCT:
		if (ec->elements)
		{
			Ender_Container_Sub *prev;

			prev = eina_list_data_get(eina_list_last(ec->elements));
			prev_offset = prev->c->offset;
			prev_size = ender_container_size_get(prev->c);
		}
		ec->elements = eina_list_append(ec->elements, sub);
		sub->c->offset = prev_offset + prev_size;
		break;

		case ENDER_UNION:
		ec->elements = eina_list_append(ec->elements, sub);
		/* always leave 4 bytes for the integer type */
		sub->c->offset = 4;
		break;

		case ENDER_LIST:
		/* for list type, only limit the number
		 * of child to one  */
		if (ec->elements) return;
		ec->elements = eina_list_append(ec->elements, sub);
		sub->c->offset = 0;
		break;

		default:
		return;
	}
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
EAPI const char * ender_container_registered_name_get(Ender_Container *thiz)
{
	if (!thiz) return NULL;

	if (!thiz->registered_name)
		return "anonymous";
	return thiz->registered_name;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_container_constraint_set(Ender_Container *thiz, Ender_Constraint *c)
{

}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const Ender_Constraint * ender_container_constraint_get(Ender_Container *thiz)
{
	return NULL;
}
