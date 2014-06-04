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

static void help(void)
{
	printf("Run: ender_inspect NAME\n");
}

static void lib_dump(const Ender_Lib *l)
{
	Ender_Item *i;
	Eina_List *items;

	/* structs */
	printf("lib dump\n");
	items = ender_lib_item_list(l, ENDER_ITEM_TYPE_STRUCT);
	EINA_LIST_FREE(items, i)
	{
		printf("> %s\n", ender_item_name_get(i));
		ender_item_unref(i);
	}
}

int main(int argc, char **argv)
{
	const Ender_Lib *lib;

	ender_init();
	if (argc < 2)
	{
		help();
		return -1;
	}
	lib = ender_lib_find(argv[1]);
	if (!lib)
	{
		printf("No such lib '%s'\n", argv[1]);
	}
	else
	{
		lib_dump(lib);
	}
	ender_shutdown();

	return 0;
}
