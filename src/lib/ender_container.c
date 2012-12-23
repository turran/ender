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
#include "tpl.h"

/* In order to serialize values we need:
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
 *
 * Problems with eet tha ive seen so far:
 * 1. You can only serialize full structs, not single values
 * 2. Given the nature of the compound containers (structs and unions)
 * where you can add elements incrementally, you need to know beforehand
 * the full siz eof your struct which is false of course. An option
 * is to destroy the previous descriptor and create a new one, but then
 * every container referencing this (struct inside struct) must be aware of
 * it
 * Other alternatives:
 * http://tpl.sourceforge.net/userguide.html
 * http://gwlib.com/
 * http://s11n.net/c11n/
 * http://avro.apache.org/docs/current/api/c/index.html
 *
 * Ok, now that we have committed tpl here, we need to start doing the
 * encoding/decoding of the values.
 * 1. Every container must have a string referring to its mapping
 * 2. Looks like we can unmap single struct fields, if that's the case
 * when unmapping structs, instead of unmapping the whole struct just iterate
 * over it an decode single values, when a an object is decoded, trigger the
 * callback. We can mark a struct as if it has or not a foreign reference
 *
 * We need to avoid the user to modify a container after it is assigned
 * into a property. So we can fixate a container when it is assigned.
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Ender_Container_Sub
{
	Ender_Container *c;
	char *name;
	size_t offset;
} Ender_Container_Sub;

static Ender_Container *_basic_containers[ENDER_LIST - ENDER_BOOL];

#ifdef BUILD_SERIALIZE
static void _ender_container_serialize_add(Ender_Value_Type t)
{
	switch (t)
	{
		case ENDER_BOOL:
		break;

		case ENDER_UINT32:
		case ENDER_COLOR:
		case ENDER_ARGB:
		break;

		case ENDER_INT32:
		break;

		case ENDER_UINT64:
		case ENDER_OBJECT:
		case ENDER_SURFACE:
		case ENDER_ENDER:
		case ENDER_POINTER:
		break;

		case ENDER_INT64:
		break;

		case ENDER_DOUBLE:
		break;

		case ENDER_STRING:
		break;

		/* the compound sub types will be added on its own function */
		case ENDER_LIST:
		case ENDER_STRUCT:
		case ENDER_UNION:
		break;

		case ENDER_MATRIX:
		case ENDER_VALUE:
		ERR("value not supported yet");
		break;
	}
}

static void _ender_container_serialize_new(Ender_Container_Serialize *s, Ender_Value_Type t)
{
	switch (t)
	{
		case ENDER_BOOL:
		s->signature = "c";
		break;

		case ENDER_UINT32:
		case ENDER_COLOR:
		case ENDER_ARGB:
		s->signature = "u";
		break;

		case ENDER_INT32:
		s->signature = "i";
		break;

		case ENDER_UINT64:
		s->signature = "U";
		break;

		/* for this we need an id */
		case ENDER_OBJECT:
		case ENDER_SURFACE:
		case ENDER_ENDER:
		case ENDER_POINTER:
		s->signature = "U";
		s->external = EINA_TRUE;
		break;

		case ENDER_INT64:
		s->signature = "I";
		break;

		case ENDER_DOUBLE:
		s->signature = "f";
		break;

		case ENDER_STRING:
		s->signature = "s";
		break;

		case ENDER_MATRIX:
		s->signature = "S(fffffffff)";
		break;

		/* the compound sub types will be added on its own function */
		case ENDER_LIST:
		s->signature = "A()";
		break;

		case ENDER_STRUCT:
		case ENDER_UNION:
		s->signature = "S()";
		break;

		case ENDER_VALUE:
		WRN("value not supported yet");
		break;
	}
}
#endif


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

static void _ender_container_list_add(Ender_Container *thiz, const char *name,
		Ender_Container_Sub *sub)
{
	/* for list type, only limit the number
	 * of child to one  */
	if (thiz->elements)
	{
		if (sub->name)
			free(sub->name);
		free(sub);
		return;
	}

	thiz->elements = eina_list_append(thiz->elements, sub);
	sub->offset = 0;
#if BUILD_SERIALIZE
#if 0
	switch (sub->c->type)
	{
		case ENDER_BOOL:
		EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Ender_Value, "bool", data.i32, EET_T_CHAR);
		break;

		case ENDER_UINT32:
		case ENDER_COLOR:
		case ENDER_ARGB:
		EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Ender_Value, "uint32", data.i32, EET_T_UINT);
		break;

		case ENDER_INT32:
		EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Ender_Value, "int32", data.i32, EET_T_INT);
		break;

		case ENDER_UINT64:
		/* for this we need an id */
		case ENDER_OBJECT:
		case ENDER_SURFACE:
		case ENDER_ENDER:
		case ENDER_POINTER:
		EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Ender_Value, "uint64", data.u64, EET_T_ULONG_LONG);
		break;

		case ENDER_INT64:
		EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Ender_Value, "int64", data.i64, EET_T_LONG_LONG);
		break;

		case ENDER_DOUBLE:
		EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Ender_Value, "d", data.d, EET_T_DOUBLE);
		break;

		case ENDER_STRING:
		EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Ender_Value, "s", data.d, EET_T_INLINED_STRING);
		break;

		case ENDER_MATRIX:
		EET_DATA_DESCRIPTOR_ADD_LIST(thiz->edd, Ender_Value, "list", data.ptr, _matrix_descriptor);
		break;

		/* the compound sub types will be added on its own function */
		case ENDER_STRUCT:
		case ENDER_UNION:
		EET_DATA_DESCRIPTOR_ADD_LIST(thiz->edd, Ender_Value, "list", data.ptr, sub->c->compound);
		break;

		case ENDER_LIST:
		case ENDER_VALUE:
		ERR("value not supported yet");
		break;
	}
#endif
#endif
}

static void _ender_container_union_add(Ender_Container *thiz, const char *name,
		Ender_Container_Sub *sub)
{
	thiz->elements = eina_list_append(thiz->elements, sub);
	/* always leave 4 bytes for the integer type */
	sub->offset = 4;
#if BUILD_SERIALIZE
#endif
}

static void _ender_container_struct_add(Ender_Container *thiz, const char *name,
		Ender_Container_Sub *sub)
{
	ssize_t prev_offset = 0;
	size_t prev_size = 0;

	if (thiz->elements)
	{
		Ender_Container_Sub *prev;

		prev = eina_list_data_get(eina_list_last(thiz->elements));
		prev_offset = prev->offset;
		prev_size = ender_container_size_get(prev->c);
	}
	thiz->elements = eina_list_append(thiz->elements, sub);
	sub->offset = prev_offset + prev_size;
#if BUILD_SERIALIZE
	/* destroy the old definition and create a new one */
	/* replace the descriptor sub with this one */
	//EET_DATA_DESCRIPTOR_ADD_SUB(edd, struct_type, name, member, subtype);
#endif
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ender_container_init(void)
{
	int i;

	/* define the common (basic) containers here */
	for (i = 0; i < sizeof(_basic_containers) / sizeof(Ender_Container *); i++)
	{
		_basic_containers[i] = _ender_container_new(i);
	}
}

void ender_container_shutdown(void)
{
	int i;

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
	/* FIXME this can be changed to is_compound */
	if (t >= ENDER_BOOL && t < ENDER_LIST)
	{
		return ender_container_ref(_basic_containers[t]);
	}
	ec = _ender_container_new(t);
	return ec;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Container * ender_container_list_new(Ender_Value_Type child)
{

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

#if 0
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
#endif

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

#if 0
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_container_add(Ender_Container *thiz, const char *name, Ender_Container *s)
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
	switch (thiz->type)
	{
		case ENDER_STRUCT:

		_ender_container_struct_add(thiz, name, sub);
		break;

		case ENDER_UNION:
		_ender_container_union_add(thiz, name, sub);
		break;

		case ENDER_LIST:
		_ender_container_list_add(thiz, name, sub);
		break;

		default:
		return;
	}
}
#endif

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

}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const Ender_Constraint * ender_container_constraint_get(Ender_Container *thiz)
{
	return NULL;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void * ender_container_value_marshal(Ender_Container *thiz, const Ender_Value *v, unsigned int *len)
{
#if BUILD_SERIALIZE
	tpl_node *n;
	void *data = NULL;

	switch (thiz->type)
	{
		case ENDER_INT32:
		case ENDER_UINT32:
		case ENDER_COLOR:
		case ENDER_ARGB:
		case ENDER_BOOL:
		case ENDER_INT64:
		case ENDER_UINT64:
		case ENDER_DOUBLE:
		case ENDER_STRING:
		case ENDER_MATRIX:
		n = tpl_map(thiz->serialize.signature, &v->data);
		tpl_pack(n, 0);
		tpl_dump(n, TPL_MEM, &data, len);
		tpl_free(n);
		break;

		/* for external objects, fetch it into a local var
		 * then call the callback and set it on the value
		 */
		case ENDER_OBJECT:
		case ENDER_SURFACE:
		case ENDER_ENDER:
		case ENDER_POINTER:

		/* for structs/unions allocate the needed size and assign it to the void * */
		/* for lists, iterate over each element on the array and add it to the list, then assign the
		 * the list to the void * */
		case ENDER_LIST:
		case ENDER_STRUCT:
		case ENDER_UNION:
		break;
	}
	return data;
#else
	return NULL;
#endif
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Value * ender_container_value_unmarshal(Ender_Container *thiz, void *data, unsigned int len)
{
#if BUILD_SERIALIZE
	Ender_Value *v;
	tpl_node *n;

	v = ender_value_new_container_from(thiz);
	switch (thiz->type)
	{
		case ENDER_INT32:
		case ENDER_UINT32:
		case ENDER_COLOR:
		case ENDER_ARGB:
		case ENDER_BOOL:
		case ENDER_INT64:
		case ENDER_UINT64:
		case ENDER_DOUBLE:
		case ENDER_STRING:
		case ENDER_MATRIX:
		n = tpl_map(thiz->serialize.signature, &v->data);
		tpl_load(n, TPL_MEM, data, len);
		tpl_unpack(n, 0);
		tpl_free(n);
		break;

		/* for external objects, fetch it into a local var
		 * then call the callback and set it on the value
		 */
		case ENDER_OBJECT:
		case ENDER_SURFACE:
		case ENDER_ENDER:
		case ENDER_POINTER:

		/* for structs/unions allocate the needed size and assign it to the void * */
		/* for lists, iterate over each element on the array and add it to the list, then assign the
		 * the list to the void * */
		case ENDER_LIST:
		case ENDER_STRUCT:
		case ENDER_UNION:
		break;
	}
	return v;
#else
	return NULL;
#endif
}


