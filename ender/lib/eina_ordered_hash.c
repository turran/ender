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
/* A very simplistic and error capable ordered hash type, but is enough for
 * our use case
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Ordered_Hash * eina_ordered_hash_new(Eina_Free_Cb cb)
{
	Eina_Ordered_Hash *thiz;

	thiz = calloc(1, sizeof(Eina_Ordered_Hash));
	thiz->hash = eina_hash_string_superfast_new(cb);

	return thiz;
}

EAPI void eina_ordered_hash_free(Eina_Ordered_Hash *thiz)
{
	eina_hash_free(thiz->hash);
	eina_list_free(thiz->order);
	free(thiz);
}

EAPI int eina_ordered_hash_count(Eina_Ordered_Hash *thiz)
{
	return eina_list_count(thiz->order);
}

EAPI void * eina_ordered_hash_nth_get(Eina_Ordered_Hash *thiz, int nth)
{
	return eina_list_nth(thiz->order, nth);
}

EAPI void * eina_ordered_hash_find(Eina_Ordered_Hash *thiz, const char *key)
{
	return eina_hash_find(thiz->hash, key);
}

EAPI void eina_ordered_hash_del(Eina_Ordered_Hash *thiz, const char *name, void *data)
{
	eina_hash_del(thiz->hash, name, data);
	thiz->order = eina_list_remove(thiz->order, data);
}

EAPI void eina_ordered_hash_add(Eina_Ordered_Hash *thiz, const char *name, void *data)
{
	eina_hash_add(thiz->hash, name, data);
	thiz->order = eina_list_append(thiz->order, data);
}
