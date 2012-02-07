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
#include <getopt.h>

static void _list_cb(const char *name, void *data)
{
	printf("%s\n", name);
}

static void _container_compound_dump(Ender_Container *c, int level)
{
	int j;
	int i;

	printf(" {\n");
	for (i = 0; i < ender_container_compound_count(c); i++)
	{
		Ender_Container *sub;
		const char *name;

		ender_container_compound_get_extended(c, i, &sub, &name);
		printf("\t");
		for (j = 0; j < level; j++)
			printf("   ");
		printf("%s", ender_value_type_name_get(ender_container_type_get(sub)));
		if (name)
			printf(" %s", name);

		if (ender_container_is_compound(sub))
			_container_compound_dump(sub, level + 1);
		else
		{
			printf(";\n");
		}
	}
	printf("\t");
	for (i = 0; i < level - 1; i++)
		printf("   ");
	printf("};\n");
}

static void _container_dump(Ender_Container *c, int level)
{
	printf("%s", ender_value_type_name_get(ender_container_type_get(c)));
	if (ender_container_is_compound(c))
		_container_compound_dump(c, level + 1);
	printf("\n");
}

static void _prop_cb(Ender_Property *p, void *data)
{
	Ender_Container *c;
	const char *name;

	c = ender_property_container_get(p);
	name = ender_property_name_get(p);
	printf("\t%s%s: ", ender_property_is_relative(p) ? "rel " : "", name);
	_container_dump(c, 0);
}

static void _inheritance_dump(Ender_Descriptor *e, int level)
{
	int i;

	for (i = 0; i < level; i++)
		printf("   ");
	printf("+---- %s\n", ender_descriptor_name_get(e));
}

static void _descriptor_dump(const char *name)
{
	Ender_Descriptor *ed;
	Ender_Descriptor *parent;
	int level = 0;
	Eina_Array *inheritance;

	ed = ender_descriptor_find(name);
	if (!ed) return;

	printf("Inheritance:\n");
	parent = ed;
	inheritance = eina_array_new(1);
	do
	{
		eina_array_push(inheritance, parent);
	}
	while ((parent = ender_descriptor_parent(parent)));
	while ((parent = eina_array_pop(inheritance)))
	{
		_inheritance_dump(parent, level);
		level++;
	}
	eina_array_free(inheritance);
	printf("Information:\n");
	printf("\ttype = %s\n", ender_descriptor_type_name_get(ender_descriptor_type(ed)));
	printf("Properties:\n");
	ender_descriptor_property_list(ed, _prop_cb, ed);
}

int main(int argc, char **argv)
{
	/* if no argument is passed, get every ender and list it */
	/* if we have an ender name, first try to match a renderer
	 * then an abstract and then a namespace
	 */
	ender_init(&argc, &argv);
	if (argc < 2)
	{
		ender_descriptor_list(_list_cb, NULL);
	}
	else
	{
		_descriptor_dump(argv[1]);
	}
	ender_shutdown();
	return 0;
}
