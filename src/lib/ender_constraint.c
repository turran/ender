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
/*
 * @TODO
 * We need to implement this
 * We can also add a constraint for the type an object can be
 * We can also add a constraint of type "file", this way we can have several
 * optmiziations on the escen side for this, like knowing the path where
 * to find this, etc
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Ender_Constraint_Range
{
	Ender_Value min;
	Ender_Value max;
} Ender_Constraint_Range;

typedef struct _Ender_Constraint_Enum_Value
{
	char *name;
	int v;
} Ender_Constraint_Enum_Value;

typedef struct _Ender_Constraint_Enum
{
	Eina_List *values;
} Ender_Constraint_Enum;

typedef struct _Ender_Constraint_Descriptor
{
	Ender_Descriptor *descriptor;
} Ender_Contraint_Descriptor;

struct _Ender_Constraint
{
	Ender_Constraint_Type type;
	union {
		Ender_Constraint_Range range;
		Ender_Constraint_Enum en;
	} data;
};

static Ender_Constraint * _ender_constraint_new(Ender_Constraint_Type type)
{
	Ender_Constraint *thiz;

	thiz = calloc(1, sizeof(Ender_Constraint));
	thiz->type = type;

	return thiz;
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
EAPI Ender_Constraint_Type ender_constraint_type_get(Ender_Constraint *thiz)
{
	return thiz->type;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Constraint * ender_constraint_range_new(Ender_Value_Type type)
{
	Ender_Constraint *c;

	/* TODO check for the type */
	c = _ender_constraint_new(ENDER_CONSTRAINT_RANGE);

	return c;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Constraint * ender_constraint_enum_new(void)
{
	Ender_Constraint *c;

	/* TODO check for the type */
	c = _ender_constraint_new(ENDER_CONSTRAINT_ENUM);

	return c;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_constraint_enum_append(Ender_Constraint *thiz, const char *name, int value)
{
	Ender_Constraint_Enum_Value *v;
	Ender_Constraint_Enum *en;

	if (!thiz || thiz->type != ENDER_CONSTRAINT_ENUM) return;

	en = &thiz->data.en;

	v = malloc(sizeof(Ender_Constraint_Enum_Value));
	v->v = value;
	v->name = strdup(name);
	en->values = eina_list_append(en->values, v);
}
