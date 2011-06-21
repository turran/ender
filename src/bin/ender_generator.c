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
 * everytime whene ender initialize and register the different
 * namespaces statically
 */
const char *ns_name;
const char *name = "element";

static void help(void)
{
	printf("Run: ender_generator INPUT OUTPUT\n");
	printf("Where INPUT is the ender file to load\n");
	printf("and OUTPUT is the c file to save\n");
}

static void _dump_property(Ender_Property *p, void *data)
{
	Ender_Container *container;
	Ender_Property_Flag flags;
	const char *property_name;

	property_name = ender_property_name_get(p);
	container = ender_property_container_get(p);
	flags = ender_property_flags_get(p);
	printf("\tp = ender_descriptor_property_add(d, \"%s\",\n", property_name);
	if (flags & ENDER_GET)
		printf("\t\t\t%s_%s_%s_get\n", ns_name, name, property_name);
	else
		printf("\t\t\tNULL,\n");
	if (flags & ENDER_SET)
		printf("\t\t\t%s_%s_%s_set\n", ns_name, name, property_name);
	else
		printf("\t\t\tNULL,\n");
	if (flags & ENDER_ADD)
		printf("\t\t\t%s_%s_%s_add\n", ns_name, name, property_name);
	else
		printf("\t\t\tNULL,\n");
	if (flags & ENDER_REMOVE)
		printf("\t\t\t%s_%s_%s_remove\n", ns_name, name, property_name);
	else
		printf("\t\t\tNULL,\n");
	if (flags & ENDER_CLEAR)
		printf("\t\t\t%s_%s_%s_clear\n", ns_name, name, property_name);
	else
		printf("\t\t\tNULL,\n");
	printf("\t\t\t);\n");
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
	printf("void %s_%s_init(void)\n", ns_name, name);
	printf("{\n");
	printf("\tEnder_Namespace *ns;\n");
	printf("\tEnder_Container *ec;\n");
	printf("\tEnder_Property *ep;\n");
	printf("\tEnder_Descriptor *parent = NULL;\n");
	printf("\tstatic Ender_Descriptor *d = NULL;\n");
	printf("\tif (d) return;\n");
	printf("\tns = %s_namespace_get();\n", ns_name);
	/* the descriptor itself */
	descriptor_type = ender_descriptor_type(descriptor);
	descriptor_parent = ender_descriptor_parent(descriptor);
	if (descriptor_parent)
	{
		printf("\tparent = ender_descriptor_find(\"%s\");\n", ender_descriptor_name_get(descriptor_parent));
	}
	printf("\td = ender_namespace_descriptor_add(ns, \"%s\", %s_%s_new, parent, ENDER_%s);\n", ns_name, ns_name, name, ender_type_name_get(descriptor_type));
	printf("\tif (!d) return;\n");
	/* the properties */
	ender_descriptor_property_list(descriptor, _dump_property, descriptor);
	printf("}\n");
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
	if (argc < 3)
	{
		help();
		return -1;
	}
	ender_parser_load(argv[1]);
	dump();
	ender_shutdown();

	return 0;
}

