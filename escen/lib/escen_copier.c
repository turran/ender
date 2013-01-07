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
#include "Enesim.h"
#include "Escen.h"
#include "escen_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef Eina_Bool (*Ender_Value_Copy)(Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type);

static Ender_Value_Copy _copiers[ENDER_PROPERTY_TYPES];

static Eina_Bool _value_is_allocable(Ender_Value_Type type)
{
	if (type == ENDER_LIST || type == ENDER_STRUCT || type == ENDER_OBJECT
			|| type == ENDER_UNION || type == ENDER_ENDER)
		return EINA_FALSE;
	return EINA_TRUE;
}

/* TODO add functions to set the fields of a struct */
static void _parsed_value_set(Ender_Container *src_c, Ender_Value *v, void *data)
{
	Ender_Value_Type src_t;

	src_t = ender_container_type_get(src_c);
	switch (src_t)
	{
		/* basic cases */
		case ENDER_UINT32:
		*(uint32_t *)data = ender_value_uint32_get(v);
		break;

		case ENDER_INT32:
		*(int32_t *)data = ender_value_int32_get(v);
		break;

		case ENDER_DOUBLE:
		*(double *)data = ender_value_double_get(v);
		break;

		case ENDER_COLOR:
		*(uint32_t *)data = ender_value_argb_get(v);
		break;

		case ENDER_STRING:
                  *(char **)data = (char *)ender_value_string_get(v);
		break;

		case ENDER_ARGB:
		*(uint32_t *)data = ender_value_argb_get(v);
		break;

		case ENDER_BOOL:
		*(Eina_Bool *)data = ender_value_bool_get(v);
		break;

		/* compound cases */
		case ENDER_STRUCT:
		*(void **)data = ender_value_struct_get(v);
		break;

		case ENDER_UNION:
		*(void **)data = ender_value_union_get(v, NULL);
		break;

		case ENDER_LIST:
                  *(Eina_List **)data = (Eina_List *)ender_value_list_get(v);
		break;

		/* specific cases */
		case ENDER_OBJECT:
		*(void **)data = ender_value_object_get(v);
		break;

		case ENDER_POINTER:
		*(void **)data = ender_value_pointer_get(v);
		break;

		case ENDER_ENDER:
		*(Ender_Element **)data = ender_value_ender_get(v);
		break;

		default:
		printf("UNSUPPORTED TYPE\n");
		break;
	}
}

static Eina_Bool _struct_from_list(Eina_List *l,
			Ender_Container *dst_cont,
			void *data)
{
	int sub_count;
	int i;
	char *s;
	void *strc;

	strc = s = data;
	/* iterate over the list of sub containers */
	sub_count = ender_container_compound_count(dst_cont);
	for (i = 0; i < sub_count; i++)
	{
		Ender_Container *sc;
		Ender_Value_Type st;
		Ender_Value_Type lt;
		Ender_Value *sv;
		Ender_Value *lv;

		/* if not enough elements, break */
		if (!l)
		{
			printf("Not enough struct members\n");
			break;
		}
		lv = eina_list_data_get(l);
		if (!lv)
		{
			printf("Empty value??\n");
			break;
		}
		sc = ender_container_compound_get(dst_cont, i, NULL);
		st = ender_container_type_get(sc);
		lt = ender_value_type_get(lv);

		/* transform the list data into a value of the same type of the struct */
		if (!escen_copier_copy(st, sc, &sv, lv, lt))
		{
			/* TODO Delete parsed list and its contents */
			break;
		}
		/* get the data and store it */
		_parsed_value_set(sc, sv, s);
		s += ender_container_size_get(sc);
		/* get the next list element */
		l = eina_list_next(l);
	}
}

/*----------------------------------------------------------------------------*
 * The Ender_Value copy functions needed to transform values from different   *
 * types                                                                      *
 *----------------------------------------------------------------------------*/
static Eina_Bool _copy_uint32(Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
	*dst = ender_value_basic_new(ENDER_UINT32);
	switch (src_type)
	{
		case ENDER_DOUBLE:
		ender_value_uint32_set(*dst, ender_value_double_get(src));
		break;

		case ENDER_UINT32:
		ender_value_uint32_set(*dst, ender_value_uint32_get(src));
		break;

		case ENDER_INT32:
		ender_value_uint32_set(*dst, ender_value_int32_get(src));
		break;

		default:
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _copy_int32(Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
	*dst = ender_value_basic_new(ENDER_INT32);
	switch (src_type)
	{
		case ENDER_DOUBLE:
		ender_value_int32_set(*dst, ender_value_double_get(src));
		break;

		case ENDER_UINT32:
		ender_value_int32_set(*dst, ender_value_uint32_get(src));
		break;

		case ENDER_INT32:
		ender_value_int32_set(*dst, ender_value_int32_get(src));
		break;

		default:
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _copy_argb(Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
	*dst = ender_value_basic_new(ENDER_ARGB);
	switch (src_type)
	{
		case ENDER_DOUBLE:
		ender_value_argb_set(*dst, ender_value_double_get(src));
		break;

		case ENDER_UINT32:
		ender_value_argb_set(*dst, ender_value_uint32_get(src));
		break;

		case ENDER_INT32:
		ender_value_argb_set(*dst, ender_value_int32_get(src));
		break;

		case ENDER_ARGB:
		*dst = ender_value_ref(src);
		break;

		case ENDER_COLOR:
		ender_value_argb_set(*dst, enesim_color_argb_to(ender_value_color_get(src)));
		break;

		case ENDER_STRING:
		ender_value_argb_set(*dst, enesim_color_lut_get(ender_value_string_get(src)));
		break;

		default:
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _copy_color(Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
	*dst = ender_value_basic_new(ENDER_COLOR);
	switch (src_type)
	{
		case ENDER_DOUBLE:
		ender_value_color_set(*dst, enesim_color_argb_from(ender_value_double_get(src)));
		break;

		case ENDER_UINT32:
		ender_value_color_set(*dst, enesim_color_argb_from(ender_value_uint32_get(src)));
		break;

		case ENDER_INT32:
		ender_value_color_set(*dst, enesim_color_argb_from(ender_value_int32_get(src)));
		break;

		case ENDER_ARGB:
		ender_value_color_set(*dst, enesim_color_argb_from(ender_value_argb_get(src)));
		break;

		case ENDER_COLOR:
		*dst = ender_value_ref(src);
		break;

		case ENDER_STRING:
		ender_value_color_set(*dst, enesim_color_lut_get(ender_value_string_get(src)));
		break;

		default:
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _copy_double(Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
	*dst = ender_value_new_container_from(dst_cont);
	switch (src_type)
	{
		case ENDER_DOUBLE:
		*dst = ender_value_ref(src);
		break;

		case ENDER_UINT32:
		ender_value_double_set(*dst, ender_value_uint32_get(src));
		break;

		case ENDER_INT32:
		ender_value_double_set(*dst, ender_value_int32_get(src));
		break;

		default:
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _copy_string(Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
	*dst = ender_value_new_container_from(dst_cont);
	switch (src_type)
	{
		case ENDER_STRING:
		ender_value_static_string_set(*dst, ender_value_string_get(src));
		break;

		default:
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _copy_ender(Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
	Escen_Ender *ee;
	Escen_Instance *ei;
	Ender_Element *e;

	switch (src_type)
	{
		case ENDER_ENDER:
		*dst = ender_value_ref(src);
		break;

		/* here we assume the pointer is an escen ender */
		case ENDER_POINTER:
		*dst = ender_value_basic_new(ENDER_ENDER);
		ee = ender_value_pointer_get(src);
		ei = escen_instance_new(ee, EINA_TRUE);
		e = escen_instance_ender_get(ei);
		ender_value_ender_set(*dst, e);
		break;

		default:
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _copy_object(Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
	Ender_Element *e;
	Escen_Ender *ee;
	Escen_Instance *ei;
	void *native;

	switch (src_type)
	{
		case ENDER_ENDER:
		*dst = ender_value_basic_new(ENDER_OBJECT);
		e = ender_value_ender_get(src);
		native = ender_element_object_get(e);
		ender_value_object_set(*dst, native);
		break;

		case ENDER_OBJECT:
		*dst = ender_value_basic_new(ENDER_OBJECT);
		native = ender_value_object_get(src);
		ender_value_object_set(*dst, native);
		break;

		/* here we assume the pointer is an escen ender */
		case ENDER_POINTER:
		*dst = ender_value_basic_new(ENDER_OBJECT);
		ee = ender_value_pointer_get(src);
		ei = escen_instance_new(ee, EINA_TRUE);
		e = escen_instance_ender_get(ei);
		native = ender_element_object_get(e);
		ender_value_object_set(*dst, native);
		break;

		default:
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _copy_union(Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
#if 0
	/* a list should always be formed of two values
	 * one that describes the specific union member to use
	 * and the other the list of the union member data
	 */
	if (src_type == ENDER_LIST)
	{
		Ender_Value *uvalue;
		Ender_Value_Type utype;
		Ender_Container *ucont = NULL;
		Eina_List *l;
		int sub_count;
		void *d;
		int uid = 2;
		size_t ss;

		/* iterate over the list of sub containers */
		l = (Eina_List *)ender_value_list_get(src);
		if (!l)
		{
			printf("empty list\n");
			return EINA_FALSE;
		}
		/* the list should have the first element as the union type */
		uvalue = eina_list_data_get(l);
		l = eina_list_next(l);
		if (!l)
		{
			printf("only type but not data list\n");
			return EINA_FALSE;
		}

		/* based on that type, get the matching union container */
		utype = ender_value_type_get(uvalue);
		/* get the container from the name */
		if (utype == ENDER_STRING)
		{
			const char *uname;

			uname = ender_value_string_get(uvalue);
			ucont = ender_container_compound_get_by_name(dst_cont, uname, &uid);
		}
		/* get the container from the id */
		else if (utype == ENDER_INT32)
		{
			uid = ender_value_int32_get(uvalue);
			ucont = ender_container_compound_get(dst_cont, uid, NULL);
		}

		if (!ucont)
		{
			printf("container not found\n");
			return EINA_FALSE;
		}

		/* now fetch the data */
		uvalue = eina_list_data_get(l);
		utype = ender_value_type_get(uvalue);
		if (utype != ENDER_LIST)
		{
			printf("union data is not of type list\n");
			return EINA_FALSE;
		}
		l = (Eina_List *)ender_value_list_get(uvalue);
		if (!l)
		{
			printf("empty list of data\n");
			return EINA_FALSE;
		}

		/* everything went ok so far, start filling the void pointer */
		*dst = ender_value_new_container_from(dst_cont);
		/* alloc a struct to match the compound size */
		ss = ender_container_compound_size_get(dst_cont);
		d = calloc(1, ss);

		/* only fill the data after the first 4 bytes */
		if (!_struct_from_list(l, ucont, ((int *)d) + 1))
		{
			printf("something went wrong when parsing the struct\n");
			return EINA_FALSE;
		}
		/* set the data allocated as the data of the struct value */
		ender_value_union_set(*dst, uid, d);
	}
	else if (src_type == ENDER_UNION)
	{
		/* the passed value is already a struct */
		/* compare the containers, if same then return the same struct */
		printf("already an union!\n");
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
#endif
	return EINA_FALSE;
}

static Eina_Bool _copy_struct(Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
#if 0
	if (src_type == ENDER_LIST)
	{
		Eina_List *l;
		size_t ss;
		void *d;

		l = (Eina_List *)ender_value_list_get(src);
		*dst = ender_value_new_container_from(dst_cont);
		/* alloc a struct to match the compound size */
		ss = ender_container_compound_size_get(dst_cont);
		d = calloc(1, ss);

		if (!_struct_from_list(l, dst_cont, d))
		{
			printf("something went wrong when parsing the struct\n");
			return EINA_FALSE;
		}
		/* set the data allocated as the data of the struct value */
		ender_value_struct_set(*dst, d);
	}
	else if (src_type == ENDER_STRUCT)
	{
		/* the passed value is already a struct */
		/* compare the containers, if same then return the same struct */
		printf("already a struct!\n");
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
#endif
	return EINA_FALSE;
}

/* for the list case is just a matter of creating a new list
 * but with all the possible values resolved
 */
static Eina_Bool _copy_list(Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
	Ender_Value_Type lt;
	Ender_Container *lc;
	Eina_List *l;
	Eina_List *ll;
	Eina_List *ld = NULL;
	Ender_Value *lv;
	size_t s;

	if (src_type != ENDER_LIST)
		return EINA_FALSE;

	/*  the src list has the same type as the dst list */
#if 0
	if (ender_container_type_get(element_container) == ender_container_type_get(value_container))
	{
		/* TODO ref it and return */
		printf("same!\n");
	}
#endif

	lc = ender_container_compound_get(dst_cont, 0, NULL);
	s = ender_container_size_get(lc);
	*dst = ender_value_list_new(lc);

	l = (Eina_List *)ender_value_list_get(src);
	EINA_LIST_FOREACH(l, ll, lv)
	{
		Ender_Value *new_value;
		void *data;

		if (!escen_copier_copy(lt,
				lc, &new_value, lv,
				ender_value_type_get(lv)))
		{
			/* TODO Delete parsed list and its l */
			continue;
		}

		/* we should not alloc this types */
		if (!_value_is_allocable(lt))
		{
			_parsed_value_set(lc, new_value, &data);
		}
		else
		{
			data = calloc(1, s);
			_parsed_value_set(lc, new_value, data);
		}

		/* get the data and store it */
		ld = eina_list_append(ld, data);
	}
	ender_value_list_set(*dst, ld);

	return EINA_TRUE;
}

static Eina_Bool _copy_bool(Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
	Ender_Element *e;
	Eina_Bool b;

	switch (src_type)
	{
		case ENDER_BOOL:
		*dst = ender_value_ref(src);
		break;

		case ENDER_DOUBLE:
		*dst = ender_value_basic_new(ENDER_BOOL);
		b = (Eina_Bool)ender_value_double_get(src);
		ender_value_bool_set(*dst, b);
		break;

		default:
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _copy_value(Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
	return EINA_FALSE;
}

static Eina_Bool _copy_dummy(Ender_Container *dst_cont, Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
	printf("unsupported value type %d\n", src_type);
	return EINA_FALSE;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void escen_copier_init(void)
{
	int i;

	/* initialize to sane values */
	for (i = 0; i < ENDER_PROPERTY_TYPES; i++)
		_copiers[i] = _copy_dummy;

	/* what we support */
	_copiers[ENDER_UINT32] = _copy_uint32;
	_copiers[ENDER_INT32] = _copy_int32;
	_copiers[ENDER_DOUBLE] = _copy_double;
	_copiers[ENDER_ARGB] = _copy_argb;
	_copiers[ENDER_COLOR] = _copy_color;
	_copiers[ENDER_STRING] = _copy_string;
	_copiers[ENDER_OBJECT] = _copy_object;
	_copiers[ENDER_ENDER] = _copy_ender;
	_copiers[ENDER_LIST] = _copy_list;
	_copiers[ENDER_STRUCT] = _copy_struct;
	_copiers[ENDER_UNION] = _copy_union;
	_copiers[ENDER_BOOL] = _copy_bool;
}

Eina_Bool escen_copier_copy(Ender_Value_Type dst_type,
		Ender_Container *dst_cont,
		Ender_Value **dst, Ender_Value *src,
		Ender_Value_Type src_type)
{
	Eina_Bool ret;
	ret = _copiers[dst_type](dst_cont, dst, src, src_type);
	if (!ret)
	{
		printf("Copying from type %s to type %s failed\n", ender_value_type_string_to(src_type), ender_value_type_string_to(dst_type));
	}
	return ret;
}

