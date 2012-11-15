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
	Ender_Function_Marshaller *marshaller;
	Ender_Container *ret;
	Eina_List *args;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Function * ender_function_new(const char *name,
		Ender_Container *ret, Eina_List *args)
{
	Ender_Function *thiz;
	Ender_Container *arg;

	thiz = calloc(1, sizeof(Ender_Function));
	thiz->args = args;
	thiz->ret = ret;

	return thiz;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

