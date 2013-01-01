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
typedef struct _Ender_Container_Sub
{
	Ender_Container *c;
	char *name;
	size_t offset;
} Ender_Container_Sub;

typedef void * (*Ender_Container_Serializer)(Ender_Container *thiz,
		const Ender_Value *v, unsigned int *len);
typedef Ender_Value * (*Ender_Container_Unserializer)(Ender_Container *thiz,
		void *data, unsigned int len);

static Ender_Container_Serializer _serializer[ENDER_TYPES];
static Ender_Container_Unserializer _unserializer[ENDER_TYPES];

static _ender_container_add(Ender_Container *thiz, const char *name,
		Ender_Container *s)
{
	Ender_Container_Sub *sub;

	if (!thiz) return;
	if (!s) return;

	if (!ender_container_is_compound(thiz))
		return;

	sub = calloc(1, sizeof(Ender_Container_Sub));
	if (name)
		sub->name = strdup(name);
	/* own the sub container */
	sub->c = s;
	thiz->elements = eina_list_append(thiz->elements, sub);
}


static Ender_Container * _ender_container_new(Ender_Value_Type t)
{
	Ender_Container *thiz;

	thiz = calloc(1, sizeof(Ender_Container));
	thiz->type = t;
	thiz->ref = 1;

#if BUILD_SERIALIZE
	/* now create the descriptor for it */
	_ender_container_serialize_new(&thiz->serialize, t);
#endif
	return thiz;
}

static void _ender_container_delete(Ender_Container *thiz)
{
	Ender_Container_Sub *s;

#if 0
	if (thiz->constraint)
		ender_constraint_free(thiz->constraint);
#endif

	EINA_LIST_FREE(thiz->elements, s)
	{
		if (s->name)
			free(s->name);
		ender_container_unref(s->c);
		free(s);
	}
	free(thiz);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_container_new(Ender_Value_Type t)
{
	Ender_Container *thiz;

	thiz = _ender_container_new(t);
	thiz->serialize_data = ender_serializer_container_new(thiz);
	return thiz;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_container_list_new(Ender_Container *child)
{
	Ender_Container *thiz;

	if (!child)
	{
		ERR("Impossible to create a struct type without sub-container");
		return NULL;
	}
	thiz = _ender_container_new(ENDER_LIST);
	_ender_container_add(thiz, "data", child);
	thiz->serialize_data = ender_serializer_container_new(thiz);
	return thiz;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_container_struct_new(Ender_Descriptor *descriptor)
{
	Ender_Container *thiz;

	thiz = _ender_container_new(ENDER_STRUCT);
	if (descriptor)
	{
		Ender_Constraint *constraint;
		constraint = ender_constraint_descriptor_new(descriptor);
		ender_container_constraint_set(thiz, constraint);
	}
	thiz->serialize_data = ender_serializer_container_new(thiz);
	return thiz;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_container_union_new(Ender_Descriptor *descriptor)
{
	Ender_Container *thiz;

	thiz = _ender_container_new(ENDER_UNION);
	if (descriptor)
	{
		Ender_Constraint *constraint;
		constraint = ender_constraint_descriptor_new(descriptor);
		ender_container_constraint_set(thiz, constraint);
	}
	thiz->serialize_data = ender_serializer_container_new(thiz);
	return thiz;
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
EAPI Eina_Bool ender_container_is_compound(Ender_Container *ec)
{
	if (ec->type == ENDER_LIST)
		return EINA_TRUE;
	return EINA_FALSE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_container_compound_get(Ender_Container *ec, unsigned int idx, const char **name)
{
	Ender_Container_Sub *sub;

	if (!ender_container_is_compound(ec))
		return NULL;
	sub = eina_list_nth(ec->elements, idx);
	if (!sub)
	{
		ERR("The container '%s' does not have sub-container at %d",
				ender_value_type_string_to(ec->type), idx);
		return NULL;
	}
	if (name) *name = sub->name;
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

		case ENDER_OBJECT:
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
EAPI Ender_Value_Type ender_container_type_get(Ender_Container *c)
{
	return c->type;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_container_constraint_set(Ender_Container *thiz, Ender_Constraint *c)
{
#if 0
	if (thiz->contraint)
		ender_constraint_free(thiz->constraint);
#endif
	thiz->constraint = c;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const Ender_Constraint * ender_container_constraint_get(Ender_Container *thiz)
{
	return thiz->constraint;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Value * ender_container_value_unmarshal(Ender_Container *thiz, void *data, unsigned int len)
{
	return ender_serializer_value_unmarshal(thiz, data, len);
}
