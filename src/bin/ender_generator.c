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

/* The idea here is to generate the .c code from an ender descriptor
 * file  (.ender). This way we can avoid the need to parse the file
 * everytime when ender initialize and register the different
 * namespaces statically
 */
const char *ns_name;
const char *name;
FILE *out;

static void help(void)
{
	printf("Run: ender_generator INPUT NAME OUTPUT\n");
	printf("Where INPUT is the ender file to load\n");
	printf("NAME is the element name to dump\n");
	printf("and OUTPUT is the c file to save\n");
}

static void _tabs(int count)
{
	int i;

	for (i = 0; i < count; i++)
		fprintf(out, "\t");
}

static void _dump_container_recursive(Ender_Container *container, int level)
{
	Ender_Value_Type type;
	int i;

	type = ender_container_type_get(container);
	_tabs(level);
	fprintf(out, "Ender_Container *tmp%d;\n", level);
	_tabs(level);
	fprintf(out, "tmp = ender_container_new(ENDER_%s);\n", ender_value_type_name_get(type));
	if (ender_container_is_compound(container))
	{
		Ender_Container *sub;
		int i;

		_tabs(level);
		fprintf(out, "{\n");
		for (i = 0; i < ender_container_compound_count(container); i++)
		{
			sub = ender_container_compound_get(container, i);
			_tabs(level + 1);
			fprintf(out, "{\n");
			_dump_container_recursive(sub, level + 2);
			_tabs(level + 2);
			fprintf(out, "ender_container_add(tmp%d, tmp%d);\n", level, level + 2);
			_tabs(level + 1);
			fprintf(out, "}\n");
		}
		_tabs(level);
		fprintf(out, "}\n");
	}
}

static void _dump_container(Ender_Property *p)
{
	Ender_Container *container;

	container = ender_property_container_get(p);
	fprintf(out, "\t{\n");
	_dump_container_recursive(container, 2);
	fprintf(out, "\t\tec = tmp2;\n");
	fprintf(out, "\t}\n");
}

static void _dump_property(Ender_Property *p, void *data)
{
	Ender_Property_Flag flags;
	const char *property_name;

	property_name = ender_property_name_get(p);
	flags = ender_property_flags_get(p);
	_dump_container(p);
	fprintf(out, "\tp = ender_descriptor_property_add(d, \"%s\",\n", property_name);
	fprintf(out, "\t\t\tec,\n");
	if (flags & ENDER_GET)
		fprintf(out, "\t\t\t%s_%s_%s_get\n", ns_name, name, property_name);
	else
		fprintf(out, "\t\t\tNULL,\n");
	if (flags & ENDER_SET)
		fprintf(out, "\t\t\t%s_%s_%s_set\n", ns_name, name, property_name);
	else
		fprintf(out, "\t\t\tNULL,\n");
	if (flags & ENDER_ADD)
		fprintf(out, "\t\t\t%s_%s_%s_add\n", ns_name, name, property_name);
	else
		fprintf(out, "\t\t\tNULL,\n");
	if (flags & ENDER_REMOVE)
		fprintf(out, "\t\t\t%s_%s_%s_remove\n", ns_name, name, property_name);
	else
		fprintf(out, "\t\t\tNULL,\n");
	if (flags & ENDER_CLEAR)
		fprintf(out, "\t\t\t%s_%s_%s_clear\n", ns_name, name, property_name);
	else
		fprintf(out, "\t\t\tNULL,\n");
	fprintf(out, "\t\t\t);\n");
}

static void dump(void)
{
	Ender_Descriptor *descriptor;
	Ender_Descriptor *descriptor_parent;
	Ender_Namespace *ns;
	Ender_Type descriptor_type;

	descriptor = ender_descriptor_find(name);
	if (!descriptor) return;
	ns = ender_descriptor_namespace_get(descriptor);
	ns_name = ender_namespace_name_get(ns);
	fprintf(out, "void %s_%s_init(void)\n", ns_name, name);
	fprintf(out, "{\n");
	fprintf(out, "\tEnder_Namespace *ns;\n");
	fprintf(out, "\tEnder_Container *ec;\n");
	fprintf(out, "\tEnder_Property *ep;\n");
	fprintf(out, "\tEnder_Descriptor *parent = NULL;\n");
	fprintf(out, "\tstatic Ender_Descriptor *d = NULL;\n");
	fprintf(out, "\tif (d) return;\n");
	fprintf(out, "\tns = %s_namespace_get();\n", ns_name);
	/* the descriptor itself */
	descriptor_type = ender_descriptor_type(descriptor);
	descriptor_parent = ender_descriptor_parent(descriptor);
	if (descriptor_parent)
	{
		fprintf(out, "\tparent = ender_descriptor_find(\"%s\");\n", ender_descriptor_name_get(descriptor_parent));
	}
	fprintf(out, "\td = ender_namespace_descriptor_add(ns, \"%s\", %s_%s_new, parent, ENDER_%s);\n", ns_name, ns_name, name, ender_type_name_get(descriptor_type));
	fprintf(out, "\tif (!d) return;\n");
	/* the properties */
	ender_descriptor_property_list(descriptor, _dump_property, descriptor);
	fprintf(out, "}\n");
}

int main(int argc, char **argv)
{
	int myargc;
	char **myargv;

	/* append the disable-parse option */
	myargc = argc + 1;
	myargv = malloc(sizeof(char *) * myargc);
	memcpy(myargv, argv, argc * sizeof(char *));
	myargv[myargc - 1] = strdup("--disable-parse");
	/* we force to disable the parse */
	ender_init(&myargc, &myargv);
	/* now parse the file input output */
	if (argc < 4)
	{
		help();
		return -1;
	}
	ender_parser_load(argv[1]);
	name = argv[2];
	out = fopen(argv[3], "w");
	dump();
	ender_shutdown();

	return 0;
}

