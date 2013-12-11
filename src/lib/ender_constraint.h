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

#ifndef _ENDER_CONSTRAINT_H
#define _ENDER_CONSTRAINT_H

/**
 * @defgroup Ender_Constraint_Group Constraint
 * @{
 */

typedef struct _Ender_Constraint Ender_Constraint;

typedef enum _Ender_Constraint_Type
{
	ENDER_CONSTRAINT_RANGE,
	ENDER_CONSTRAINT_ENUM,
	ENDER_CONSTRAINT_DESCRIPTOR,
	ENDER_CONSTRAINTS,
} Ender_Constraint_Type;

EAPI Ender_Constraint_Type ender_constraint_type_get(const Ender_Constraint *thiz);

EAPI Ender_Constraint * ender_constraint_range_new(Ender_Value_Type type);

EAPI Ender_Constraint * ender_constraint_enum_new(void);
EAPI void ender_constraint_enum_append(Ender_Constraint *thiz, const char *name, int value);

EAPI Ender_Constraint * ender_constraint_descriptor_new(
	Ender_Descriptor *descriptor);
EAPI Ender_Descriptor * ender_constraint_descriptor_descriptor_get(
	const Ender_Constraint *thiz);


/**
 * @}
 */


