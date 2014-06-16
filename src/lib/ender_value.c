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
#include "ender_private.h"

#include "ender_main.h"
#include "ender_value.h"

#include "ender_main_private.h"
#include "ender_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
size_t ender_value_type_size_get(Ender_Value_Type t)
{
	switch (t)
	{
		case ENDER_VALUE_TYPE_BOOL:
		return sizeof(Eina_Bool);

		case ENDER_VALUE_TYPE_UINT8:
		return sizeof(uint8_t);

		case ENDER_VALUE_TYPE_INT8:
		return sizeof(int8_t);

		case ENDER_VALUE_TYPE_UINT32:
		return sizeof(uint32_t);

		case ENDER_VALUE_TYPE_INT32:
		return sizeof(int32_t);

		case ENDER_VALUE_TYPE_UINT64:
		return sizeof(uint64_t);

		case ENDER_VALUE_TYPE_INT64:
		return sizeof(int64_t);

		case ENDER_VALUE_TYPE_DOUBLE:
		return sizeof(double);

		case ENDER_VALUE_TYPE_STRING:
		case ENDER_VALUE_TYPE_POINTER:
		return sizeof(void *);

		default:
		CRI("Unsupported value %d", t);
		return 0;
	}
}

ssize_t ender_value_type_alignment_get(Ender_Value_Type t)
{
	switch (t)
	{
		case ENDER_VALUE_TYPE_BOOL:
		return __alignof__(Eina_Bool);

		case ENDER_VALUE_TYPE_UINT8:
		return __alignof__(uint8_t);

		case ENDER_VALUE_TYPE_INT8:
		return __alignof__(int8_t);

		case ENDER_VALUE_TYPE_UINT32:
		return __alignof__(uint32_t);

		case ENDER_VALUE_TYPE_INT32:
		return __alignof__(int32_t);

		case ENDER_VALUE_TYPE_UINT64:
		return __alignof__(uint64_t);

		case ENDER_VALUE_TYPE_INT64:
		return __alignof__(int64_t);

		case ENDER_VALUE_TYPE_DOUBLE:
		return __alignof__(double);

		case ENDER_VALUE_TYPE_STRING:
		case ENDER_VALUE_TYPE_POINTER:
		return __alignof__(void *);

		default:
		CRI("Unsupported value %d", t);
		return 0;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
