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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Ender_Function
{
	Ender_Accessor f;
	Ender_Marshaller marshaller;
	Ender_Container *ret;
	Eina_List *args;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Function * ender_function_new(const char *name,
		Ender_Accessor f,
		Ender_Marshaller marshaller,
		Ender_Container *ret, Eina_List *args)
{
	Ender_Function *thiz;
	Ender_Container *arg;

	if (!f) return NULL;

	if (!marshaller)
	{
		marshaller = ender_marshaller_find_list(ret, args);
		if (!marshaller) return NULL;
	}

	thiz = calloc(1, sizeof(Ender_Function));
	thiz->args = args;
	thiz->ret = ret;
	thiz->marshaller = marshaller;
	thiz->f = f;

	return thiz;
}

Eina_Bool ender_function_call(Ender_Function *thiz, void *o,
		Ender_Value *ret, Eina_List *args)
{
	return thiz->marshaller(o, thiz->f, ret, args);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI int ender_function_args_count(Ender_Function *thiz)
{
	return eina_list_count(thiz->args);
}

EAPI const Eina_List * ender_function_args_get(Ender_Function *thiz)
{
	return thiz->args;
}
