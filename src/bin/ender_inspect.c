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

static const char * value_type_dump(Ender_Value_Type vtype)
{
	switch (vtype)
	{

		case ENDER_VALUE_TYPE_BOOL:
		return "bool";

		case ENDER_VALUE_TYPE_UINT32:
		return "uint32";

		case ENDER_VALUE_TYPE_INT32:
		return "int32";

		case ENDER_VALUE_TYPE_UINT64:
		return "uint64";

		case ENDER_VALUE_TYPE_INT64:
		return "int64";

		case ENDER_VALUE_TYPE_DOUBLE:
		return "double";

		case ENDER_VALUE_TYPE_STRING:
		return "string";

		case ENDER_VALUE_TYPE_POINTER:
		return "pointer";

		default:
		return "unknown";
	}
}

static const char * item_type_dump(Ender_Item *i)
{
	switch (ender_item_type_get(i))
	{
		case ENDER_ITEM_TYPE_BASIC:
		return value_type_dump(ender_item_basic_value_type_get(i));
		break;

		case ENDER_ITEM_TYPE_OBJECT:
		case ENDER_ITEM_TYPE_STRUCT:
		return ender_item_name_get(i);

		default:
		return "unknown";
		break;
	}

}

static void attr_dump(Ender_Item *i, int indent)
{
	Ender_Item *type;

	type = ender_item_attr_type_get(i);
	printf("%*c%s: %s\n", indent, ' ', ender_item_name_get(i), item_type_dump(type));
	ender_item_unref(type);
}

static void function_dump(Ender_Item *i, int indent)
{
	Ender_Item *ret;

	printf("%*c", indent, ' ');
	ret = ender_item_function_ret_get(i);
	if (ret)
	{
		printf("%s ", item_type_dump(ret));
	}
	else
	{
		printf("void ");
	}
	printf("%s (", ender_item_name_get(i));
	/* TODO args */
	printf(")\n");
}

static void lib_dump(const Ender_Lib *l)
{
	Ender_Item *i;
	Eina_List *items;

	/* structs */
	printf("Structs:\n");
	items = ender_lib_item_list(l, ENDER_ITEM_TYPE_STRUCT);
	EINA_LIST_FREE(items, i)
	{
		Ender_Item *f;
		Eina_List *subitems;

		printf("  %s\n", ender_item_name_get(i));
		subitems = ender_item_struct_fields_get(i);
		EINA_LIST_FREE(subitems, f)
		{
			attr_dump(f, 4);
			ender_item_unref(f);
		}
		printf("    Functions:\n");
		subitems = ender_item_struct_functions_get(i);
		EINA_LIST_FREE(subitems, f)
		{
			function_dump(f, 8);
			ender_item_unref(f);
		}
		ender_item_unref(i);
	}
	/* objects */
	printf("Objects:\n");
	items = ender_lib_item_list(l, ENDER_ITEM_TYPE_OBJECT);
	EINA_LIST_FREE(items, i)
	{
		Ender_Item *f;
		Eina_List *subitems;

		printf("  %s\n", ender_item_name_get(i));

		printf("    Functions:\n");
		subitems = ender_item_object_functions_get(i);
		EINA_LIST_FREE(subitems, f)
		{
			function_dump(f, 8);
			ender_item_unref(f);
		}
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
