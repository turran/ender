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

#if BUILD_SERIALIZER_EET
#include "Eet.h"
#endif
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#if BUILD_SERIALIZER_EET
typedef enum _Ender_Serializer_Eet_Value_Type
{
	ENDER_SERIALIZER_EET_BOOL,
	ENDER_SERIALIZER_EET_UINT32,
	ENDER_SERIALIZER_EET_INT32,
	ENDER_SERIALIZER_EET_UINT64,
	ENDER_SERIALIZER_EET_INT64,
	ENDER_SERIALIZER_EET_DOUBLE,
	ENDER_SERIALIZER_EET_STRING,
	ENDER_SERIALIZER_EET_ELEMENT,
	ENDER_SERIALIZER_EET_VALUE,
	ENDER_SERIALIZER_EET_LIST,
	ENDER_SERIALIZER_EET_VELUE_TYPES,
} Ender_Serializer_Eet_Value_Type;

typedef union _Ender_Serializer_Eet_Value_Data
{
	int32_t i32;
	uint32_t u32;
	int64_t i64;
	uint64_t u64;
	double d;
	void *ptr;
} Ender_Serializer_Eet_Value_Data;

/* description used for a property */
typedef struct _Ender_Serializer_Eet_Property
{
	Ender_Serializer_Eet_Value_Type type;
	Ender_Serializer_Eet_Value_Data data;
} Ender_Serializer_Eet_Property;

/* description of an element
 * it is composed of a list of int -> value (static properties)
 * or string -> value (dynamic properties) tuples
 */
typedef struct _Ender_Serializer_Element
{
	Eina_List *properties;
} Ender_Serializer_Element;

/* common eet descriptors */
/* our own representation of a serialized basic value */
typedef struct _Ender_Serializer_Eet_Value
{
	Ender_Value_Type type;
	Ender_Value_Data data;
} Ender_Serializer_Eet_Value;

static Eet_Data_Descriptor *_ender_value_descriptor;
static Eet_Data_Descriptor *_ender_value_data_descriptor;
static Eet_Data_Descriptor *_ender_element_descriptor;
/*----------------------------------------------------------------------------*
 *                      Ender_Value_Data descriptor                           *
 *----------------------------------------------------------------------------*/
struct
{
	Ender_Value_Type d;
	const char *name;
} _ender_value_data_mapping[] = {
	{ ENDER_BOOL, "b" },
	{ ENDER_UINT32, "u32" },
	{ ENDER_COLOR, "c" },
	{ ENDER_ARGB, "a" },
	{ ENDER_INT32, "i32" },
	{ ENDER_UINT64, "u64" },
	{ ENDER_INT64, "i64" },
	{ ENDER_DOUBLE, "d" },
	{ ENDER_STRING, "s" },
	{ ENDER_VALUE, NULL }
};

static const char * _serializer_eet_value_type_get(const void *data,
		Eina_Bool *unknown)
{
	const Ender_Value_Type *d = data;
	int i;

	//printf("getting type for %s\n", ender_value_type_string_to(*d));
	if (unknown)
		*unknown = EINA_FALSE;

	for (i = 0; _ender_value_data_mapping[i].name != NULL; ++i)
	{
		if (*d == _ender_value_data_mapping[i].d)
			return _ender_value_data_mapping[i].name;
	}

	if (unknown)
		*unknown = EINA_TRUE;

	return NULL;
}

static Eina_Bool _serializer_eet_value_type_set(const char *type,
		void *data, Eina_Bool unknown)
{
	Ender_Value_Type *d = data;
	int i;

	//printf("setting type for %s\n", type);
	if (unknown)
		return EINA_FALSE;

	for (i = 0; _ender_value_data_mapping[i].name != NULL; ++i)
	{
		if (strcmp(_ender_value_data_mapping[i].name, type) == 0)
		{
			*d = _ender_value_data_mapping[i].d;
			return EINA_TRUE;
		}
	}

	return EINA_FALSE;
}

static Eet_Data_Descriptor * _serializer_eet_ender_value_data_descriptor_get(void)
{
	Eet_Data_Descriptor_Class eddc;
	Eet_Data_Descriptor *ret;
	Eet_Data_Descriptor *d;

	EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Ender_Value_Data);
	eddc.version = EET_DATA_DESCRIPTOR_CLASS_VERSION;
	eddc.func.type_get = _serializer_eet_value_type_get;
	eddc.func.type_set = _serializer_eet_value_type_set;
	ret = eet_data_descriptor_stream_new(&eddc);

	d = eet_data_descriptor_stream_new(&eddc);
	EET_DATA_DESCRIPTOR_ADD_BASIC(d, Ender_Value_Data, "b", i32, EET_T_INT);
	EET_DATA_DESCRIPTOR_ADD_MAPPING(ret, "b", d);

	d = eet_data_descriptor_stream_new(&eddc);
	EET_DATA_DESCRIPTOR_ADD_BASIC(d, Ender_Value_Data, "i32", i32, EET_T_INT);
	EET_DATA_DESCRIPTOR_ADD_MAPPING(ret, "i32", d);

	d = eet_data_descriptor_stream_new(&eddc);
	EET_DATA_DESCRIPTOR_ADD_BASIC(d, Ender_Value_Data, "u32", u32, EET_T_UINT);
	EET_DATA_DESCRIPTOR_ADD_MAPPING(ret, "u32", d);

	d = eet_data_descriptor_stream_new(&eddc);
	EET_DATA_DESCRIPTOR_ADD_BASIC(d, Ender_Value_Data, "u32", u32, EET_T_UINT);
	EET_DATA_DESCRIPTOR_ADD_MAPPING(ret, "c", d);

	d = eet_data_descriptor_stream_new(&eddc);
	EET_DATA_DESCRIPTOR_ADD_BASIC(d, Ender_Value_Data, "u32", u32, EET_T_UINT);
	EET_DATA_DESCRIPTOR_ADD_MAPPING(ret, "a", d);

	d = eet_data_descriptor_stream_new(&eddc);
	EET_DATA_DESCRIPTOR_ADD_BASIC(d, Ender_Value_Data, "u64", u64, EET_T_LONG_LONG);
	EET_DATA_DESCRIPTOR_ADD_MAPPING(ret, "u64", d);

	d = eet_data_descriptor_stream_new(&eddc);
	EET_DATA_DESCRIPTOR_ADD_BASIC(d, Ender_Value_Data, "i64", i64, EET_T_ULONG_LONG);
	EET_DATA_DESCRIPTOR_ADD_MAPPING(ret, "i64", d);

	d = eet_data_descriptor_stream_new(&eddc);
	EET_DATA_DESCRIPTOR_ADD_BASIC(d, Ender_Value_Data, "d", d, EET_T_DOUBLE);
	EET_DATA_DESCRIPTOR_ADD_MAPPING(ret, "d", d);

	d = eet_data_descriptor_stream_new(&eddc);
	EET_DATA_DESCRIPTOR_ADD_BASIC(d, Ender_Value_Data, "s", ptr, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_MAPPING(ret, "s", d);

	return ret;
}
/*----------------------------------------------------------------------------*
 *                        Ender_Element descriptor                             *
 *----------------------------------------------------------------------------*/
static Eet_Data_Descriptor * _serializer_eet_ender_element_descriptor_get(void)
{
	/* TODO */
	return NULL;
}
/*----------------------------------------------------------------------------*
 *                       The serializer interface                             *
 *----------------------------------------------------------------------------*/
static void * _serializer_eet_container_new(Ender_Container *c)
{
	void *ret = NULL;
	switch (c->type)
	{
		/* for simple types, just marshal the Ender_Value */
		case ENDER_BOOL:
		case ENDER_UINT32:
		case ENDER_COLOR:
		case ENDER_ARGB:
		case ENDER_INT32:
		case ENDER_UINT64:
		case ENDER_INT64:
		case ENDER_DOUBLE:
		case ENDER_STRING:
		ret = _ender_value_descriptor;
		break;

		/* for element types, marshal a list of eet values */
		case ENDER_OBJECT:
		case ENDER_STRUCT:
		case ENDER_ENDER:
		case ENDER_UNION:
		break;

		/* for lists, marshal a list of its sub container
		 * for simple sub containers just wrap all the possible
		 * simple types. For element types, use the element
		 * description
		 */
		case ENDER_LIST:
		break;

		/* a pointer is a not known type, we can not marshal it */
		/* for a value, we dont know yet :P */ 
		case ENDER_VALUE:
		case ENDER_POINTER:
		ERR("value not supported yet");
		break;
	}
	return ret;
}

static void * _serializer_eet_element_marshal(Ender_Descriptor *d,
		Ender_Element *e, unsigned int *len)
{
	/* iterate over the descriptor properties and marshal them */
	/* iterate over the element properties only and marshal them */
	/* finally encode it */
	return NULL;
}

static Ender_Element * _serializer_eet_element_unmarshal(
		Ender_Descriptor *d, void *data, unsigned int len)
{
	/* decode the data */
	/* create an element */
	/* iterate over the generate struct and set the properties */
	return NULL;
}

static void * _serializer_eet_value_marshal(void *sd,
		const Ender_Value *v, unsigned int *len)
{
	Ender_Serializer_Eet_Value nv;
	void *ret = NULL;

	if (!sd) return NULL;

	nv.data = v->data;
	nv.type = v->container->type;

	ret = eet_data_descriptor_encode(sd, &nv, len);
	/* check if the descriptor is a basic descriptor */
	/* if so, encode directly */
	/* if it is a value that holds an element, check the
	 * descriptor constraint and marshal it if possible
	 */
	/* if it is a list, marshal it using its own descriptor */
	return ret;

}

static Ender_Value * _serializer_eet_value_unmarshal(
		Ender_Container *c, void *sd, void *data, unsigned int len)
{
	Ender_Serializer_Eet_Value *nv;
	Ender_Value *ret;

	if (!sd) return NULL;

	/* TODO we need to avoid this double malloc, for that
	 * we need to wrap Ender_Value instead of Ender_Serializer_Eet_Value
	 * that means, we need to add a "type" field
	 */
	nv = eet_data_descriptor_decode(sd, data, len);

	ret = ender_value_new_container_from(c);
	ret->data = nv->data;
	free(nv);

	return ret;
}

static Ender_Serializer _ender_serializer_eet = {
	/* .container_new	= */ _serializer_eet_container_new,
	/* .element_marshal	= */ _serializer_eet_element_marshal,
	/* .element_unmarshal	= */ _serializer_eet_element_unmarshal,
	/* .value_marshal	= */ _serializer_eet_value_marshal,
	/* .value_unmarshal 	= */ _serializer_eet_value_unmarshal,
};
#endif
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Serializer * ender_serializer_eet_get(void)
{
#if BUILD_SERIALIZER_EET
	static int _init = 0;
	if (!_init++)
	{
		Eet_Data_Descriptor_Class eddc;
		Eet_Data_Descriptor *d;
		eet_init();

		//* we need a common eet descriptor for basic types */
		/* first the Ender_Value_Data descriptor */
		d = _serializer_eet_ender_value_data_descriptor_get();
		_ender_value_data_descriptor = d;

		/* now the Ender_Serializer_Eet_Value descriptor */
		EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Ender_Serializer_Eet_Value);
		d = eet_data_descriptor_stream_new(&eddc);

		EET_DATA_DESCRIPTOR_ADD_UNION(d, Ender_Serializer_Eet_Value, "data",
				data, type, _ender_value_data_descriptor);
		_ender_value_descriptor = d;

		 /* we need another eet descriptor for structs, unions and
		 * enders, i.e property list
		 */
		d = _serializer_eet_ender_element_descriptor_get();
		_ender_element_descriptor = d;
		 /* and lastly a new descriptor for each kind of list
		 */

	}
	return &_ender_serializer_eet;
#else
	return NULL;
#endif
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

