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
typedef struct _Ender_Serializer_Eet_Value
{
	Ender_Serializer_Eet_Value_Type type;
	Ender_Serializer_Eet_Value_Data data;
} Ender_Serializer_Value;

/* description of an element
 * it is composed of a list of int -> value (static properties)
 * or string -> value (dynamic properties) tuples
 */
typedef struct _Ender_Serializer_Element
{
	Eina_List *properties;
} Ender_Serializer_Element;
/*----------------------------------------------------------------------------*
 *                       The serializer interface                             *
 *----------------------------------------------------------------------------*/
static void * _serializer_eet_container_new(Ender_Container *c)
{
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
	/* check if the descriptor is a basic descriptor */
	/* if so, encode directly */
	/* if it is a value that holds an element, check the
	 * descriptor constraint and marshal it if possible
	 */
	/* if it is a list, marshal it using its own descriptor */
	return NULL;

}

static Ender_Value * _serializer_eet_value_unmarshal(
		void *sd, void *data, unsigned int len)
{
	return NULL;
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
		eet_init();
		/* we need a common eet descriptor for basic types
		 * we need another eet descriptor for structs, unions and
		 * enders
		 * and lastly a new descriptor for each kind of list
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

