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

typedef struct _Ender_Serializer_Eet_Value
{
	Ender_Serializer_Eet_Value_Type type;
	Ender_Serializer_Eet_Value_Data data;
} Ender_Serializer_Value;
/*----------------------------------------------------------------------------*
 *                       The serializer interface                             *
 *----------------------------------------------------------------------------*/
static Ender_Serializer _ender_serializer_eet = {

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
		/* create the common eet descriptors for basic types and
		 * structs, unions, objects and ender elements. For list
		 * we need to create the descriptor for each case
		 *
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

