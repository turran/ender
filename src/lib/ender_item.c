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
#include "ender_private.h"

#include "ender_main.h"
#include "ender_item.h"
#include "ender_lib.h"

#include "ender_main_private.h"
#include "ender_item_private.h"
#include "ender_lib_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Item * ender_item_new(Ender_Item_Descriptor *desc, void *data)
{
	Ender_Item *thiz;

	if (!desc)
		return NULL;

	thiz = calloc(1, sizeof(Ender_Item));
	thiz->ref = 1;
	thiz->desc = desc;
	thiz->data = data;

	if (thiz->desc->init)
		thiz->desc->init(thiz);

	return thiz;
}

void * ender_item_data_get(Ender_Item *thiz)
{
	return thiz->data;
}

void ender_item_type_set(Ender_Item *thiz, Ender_Item_Type type)
{
	thiz->type = type;
}

void ender_item_name_set(Ender_Item *thiz, const char *name)
{
	if (thiz->name)
	{
		free(thiz->name);
		thiz->name = NULL;
	}
	if (name)
	{
		thiz->name = strdup(name);
	}
}

void ender_item_parent_set(Ender_Item *thiz, Ender_Item *parent)
{
	if (thiz->parent && parent)
	{
		CRI("Item already has a parent");
		return;
	}
	thiz->parent = parent;
}

void * ender_item_sym_get(Ender_Item *thiz, const char *name)
{
	if (!thiz->lib)
	{
		if (!thiz->parent)
			return NULL;
		return ender_item_sym_get(thiz->parent, name);
	}
	else
	{
		return ender_lib_sym_get(thiz->lib, name);
	}

}

void ender_item_cycle_ref_set(Ender_Item *thiz, int ref)
{
	thiz->cycle_ref = ref;
}

int ender_item_ref_count(Ender_Item *thiz)
{
	return thiz->ref;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * @brief Increase the reference counter of an item
 * @param[in] thiz The item
 * @return The input parameter @a thiz for programming convenience
 */
EAPI Ender_Item * ender_item_ref(Ender_Item *thiz)
{
	if (!thiz) return thiz;
	thiz->ref++;
	return thiz;
}

/**
 * @brief Decrease the reference counter of an item
 * @param[in] thiz The item
 */
EAPI void ender_item_unref(Ender_Item *thiz)
{
	if (!thiz) return;
	thiz->ref--;
	if (thiz->ref == thiz->cycle_ref)
	{
		if (thiz->desc->deinit)
			thiz->desc->deinit(thiz);
		if (thiz->parent)
		{
			CRI("Removing last reference of '%s' with a parent", thiz->name);
		}
		if (thiz->name)
			free(thiz->name);

		free(thiz);
	}
}

/**
 * Get the name of an item
 * @param thiz The item to get the name from
 * @return The name of the item
 */
EAPI const char * ender_item_name_get(Ender_Item *thiz)
{
	return thiz->name;
}

/**
 * Get the full name of an item
 *
 * The full name is the actual item's name appended
 * to every parent's name
 *
 * @param thiz The item to get the full name from
 * @return The full name @ender_transfer[full]
 */
EAPI char * ender_item_full_name_get(Ender_Item *thiz)
{
	Ender_Item *parent;
	char *ret;
	int len;

	parent = ender_item_parent_get(thiz);
	ret = strdup(ender_item_name_get(thiz));
	len = strlen(ret);

	/* simples case */
	if (!parent)
		return ret;
	/* append the name to every ancestor name */
	while (parent)
	{
		Ender_Item *tmp;
		char *tret;
		const char *pname;
		int plen;

		pname = ender_item_name_get(parent);
		plen = strlen(pname);
		tret = malloc(plen + 1 + len + 1); // + 1 '.' + 1 '\0'
		strcpy(tret, pname);
		tret[plen] = '.';
		strcpy(&tret[plen + 1], ret);
		tret[plen + 1 + len] = '\0';

		free(ret);
		ret = tret;
		len = plen + 1 + len;

		tmp = ender_item_parent_get(parent);
		ender_item_unref(parent);
		parent = tmp;
	}

	return ret;
}

/**
 * Get the type of an item
 * @param thiz The item to get the type from
 * @return The item type
 */
EAPI Ender_Item_Type ender_item_type_get(Ender_Item *thiz)
{
	return thiz->type;
}

/**
 * Get the parent of an item
 *
 * Depending on the type of item, the item might have or not
 * a parent. The @ref Ender_Arg_Group for example has always
 * a @ref Ender_Function_Group as a parent.
 *
 * @param thiz The item to get the parent from
 * @return The parent of the item. @ender_transfer{none}
 */
EAPI Ender_Item * ender_item_parent_get(Ender_Item *thiz)
{
	return ender_item_ref(thiz->parent);
}

EAPI const char * ender_item_type_name_get(Ender_Item_Type type)
{
	switch (type)
	{
		case ENDER_ITEM_TYPE_BASIC:
		return "basic";

		case ENDER_ITEM_TYPE_FUNCTION:
		return "function";

		case ENDER_ITEM_TYPE_ATTR:
		return "attr";

		case ENDER_ITEM_TYPE_ARG:
		return "arg";

		case ENDER_ITEM_TYPE_OBJECT:
		return "object";

		case ENDER_ITEM_TYPE_STRUCT:
		return "struct";

		case ENDER_ITEM_TYPE_ENUM:
		return "enum";

		case ENDER_ITEM_TYPE_CONSTANT:
		return "constant";

		case ENDER_ITEM_TYPE_DEF:
		return "def";

		case ENDER_ITEM_TYPE_INVALID:
		default:
		return "invalid";
	}
}

/**
 * Get the library an item belongs to
 * @param thiz The item to get the library from
 * @return The library the item belongs to
 */
EAPI const Ender_Lib * ender_item_lib_get(Ender_Item *thiz)
{
	if (!thiz->lib)
	{
		if (!thiz->parent)
			return NULL;
		return ender_item_lib_get(thiz);
	}
	else
	{
		return thiz->lib;
	}
}

/**
 * Check if the item is an exception item
 * An exception item is an item of type eina.error
 *
 * @param i The item to check
 * @return EINA_TRUE if the item is an exception, EINA_FALSE otherwise
 */
EAPI Eina_Bool ender_item_is_exception(Ender_Item *i)
{
	if (!i) return EINA_FALSE;

	if (!strcmp(ender_item_name_get(i), "eina.error"))
		return EINA_TRUE;
	else
		return EINA_FALSE;
}
