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

		case ENDER_VALUE_TYPE_UINT8:
		return "uint8";

		case ENDER_VALUE_TYPE_INT8:
		return "int8";

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

		case ENDER_VALUE_TYPE_SIZE:
		return "size";

		default:
		return "unknown";
	}
}

static const char * item_type_dump(Ender_Item *i)
{
	if (!i)
		return "unknown not i";

	switch (ender_item_type_get(i))
	{
		case ENDER_ITEM_TYPE_BASIC:
		return value_type_dump(ender_item_basic_value_type_get(i));
		break;

		case ENDER_ITEM_TYPE_DEF:
		case ENDER_ITEM_TYPE_OBJECT:
		case ENDER_ITEM_TYPE_STRUCT:
		case ENDER_ITEM_TYPE_ENUM:
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
	Ender_Item *a;
	Eina_List *args;
	int count = 0;

	printf("%*c", indent, ' ');
	ret = ender_item_function_ret_get(i);
	if (ret)
	{
		Ender_Item *type;

		type = ender_item_arg_type_get(ret);
		printf("%s ", item_type_dump(type));
		ender_item_unref(type);
	}
	else
	{
		printf("void ");
	}
	printf("%s (", ender_item_name_get(i));
	args = ender_item_function_args_get(i);
	EINA_LIST_FREE(args, a)
	{
		Ender_Item *type;

		if (count != 0)
			printf(", ");
		type = ender_item_arg_type_get(a);
		printf("%s", item_type_dump(type));
		ender_item_unref(a);
		count++;
	}
	printf(")\n");
}

static void constant_dump(Ender_Item *i, int indent)
{
	Ender_Item *type;

	type = ender_item_attr_type_get(i);
	printf("%*c", indent, ' ');
	printf("%s: %s\n", ender_item_name_get(i), item_type_dump(type));
	ender_item_unref(type);
}

static void lib_dump(const Ender_Lib *l)
{
	Ender_Item *i;
	Eina_List *items;

	/* enums */
	printf("Enums:\n");
	items = ender_lib_item_list(l, ENDER_ITEM_TYPE_ENUM);
	EINA_LIST_FREE(items, i)
	{
		Ender_Item *lv;
		Eina_List *values;
		Eina_List *functions;

		printf("  %s\n", ender_item_name_get(i));
		/* values */
		printf("    Values:\n");
		values = ender_item_enum_values_get(i);
		EINA_LIST_FREE(values, lv)
		{
			Ender_Value v;

			ender_item_constant_value_get(lv, &v);
			printf("        %s %d\n", ender_item_name_get(lv), v.i32);
			ender_item_unref(lv);
		}
		/* functions */
		printf("    Functions:\n");
		functions = ender_item_enum_functions_get(i);
		EINA_LIST_FREE(functions, lv)
		{
			function_dump(lv, 8);
			ender_item_unref(lv);
		}
		ender_item_unref(i);
	}

	/* defs */
	printf("Defs:\n");
	items = ender_lib_item_list(l, ENDER_ITEM_TYPE_DEF);
	EINA_LIST_FREE(items, i)
	{
		Ender_Item *f;
		Eina_List *subitems;

		printf("  %s\n", ender_item_name_get(i));
		printf("    Functions:\n");
		subitems = ender_item_def_functions_get(i);
		EINA_LIST_FREE(subitems, f)
		{
			function_dump(f, 8);
			ender_item_unref(f);
		}
		ender_item_unref(i);
	}

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
		Ender_Item *inherit;
		Eina_List *subitems;

		inherit = ender_item_object_inherit_get(i);
		if (inherit)
		{
			printf("  %s:%s\n", ender_item_name_get(i), ender_item_name_get(inherit));
			ender_item_unref(inherit);
		}
		else
		{
			printf("  %s\n", ender_item_name_get(i));
		}
		subitems = ender_item_object_props_get(i);
		EINA_LIST_FREE(subitems, f)
		{
			attr_dump(f, 4);
			ender_item_unref(f);
		}

		printf("    Functions:\n");
		subitems = ender_item_object_functions_get(i);
		EINA_LIST_FREE(subitems, f)
		{
			function_dump(f, 8);
			ender_item_unref(f);
		}
		ender_item_unref(i);
	}
	/* functions */
	printf("Functions:\n");
	items = ender_lib_item_list(l, ENDER_ITEM_TYPE_FUNCTION);
	EINA_LIST_FREE(items, i)
	{
		function_dump(i, 4);
		ender_item_unref(i);
	}
	/* functions */
	printf("Constants:\n");
	items = ender_lib_item_list(l, ENDER_ITEM_TYPE_CONSTANT);
	EINA_LIST_FREE(items, i)
	{
		constant_dump(i, 4);
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
