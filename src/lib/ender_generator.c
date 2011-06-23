/* ENDER - Enesim's descriptor library
 * Copyright (C) 2010 - 2011 Jorge Luis Zapata
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
typedef struct _Ender_Generator
{
	const char *ns_name;
	const char *name;
	FILE *out;
	Eina_Bool found;
	Eina_Bool name_matched;
} Ender_Generator;

static void _tabs(Ender_Generator *thiz, int count)
{
	int i;

	for (i = 0; i < count; i++)
		fprintf(thiz->out, "\t");
}

static void _dump_container_recursive(Ender_Generator *thiz, Ender_Container *container, int level)
{
	Ender_Value_Type type;
	int i;

	type = ender_container_type_get(container);
	_tabs(thiz, level);
	fprintf(thiz->out, "Ender_Container *tmp%d;\n", level);
	_tabs(thiz, level);
	fprintf(thiz->out, "tmp = ender_container_new(ENDER_%s);\n", ender_value_type_name_get(type));
	if (ender_container_is_compound(container))
	{
		Ender_Container *sub;
		int i;

		_tabs(thiz, level);
		fprintf(thiz->out, "{\n");
		for (i = 0; i < ender_container_compound_count(container); i++)
		{
			sub = ender_container_compound_get(container, i);
			_tabs(thiz, level + 1);
			fprintf(thiz->out, "{\n");
			_dump_container_recursive(thiz, sub, level + 2);
			_tabs(thiz, level + 2);
			fprintf(thiz->out, "ender_container_add(tmp%d, tmp%d);\n", level, level + 2);
			_tabs(thiz, level + 1);
			fprintf(thiz->out, "}\n");
		}
		_tabs(thiz, level);
		fprintf(thiz->out, "}\n");
	}
}
/*----------------------------------------------------------------------------*
 *                           The parser insterface                            *
 *----------------------------------------------------------------------------*/
static void _generator_on_using(void *data, const char *file)
{
}

static void _generator_on_namespace(void *data, const char *name)
{
	Ender_Generator *thiz;

	thiz = data;
	thiz->ns_name = name;
}

static void _generator_on_renderer(void *data, const char *name, Ender_Type type, const char *parent)
{
	Ender_Generator *thiz;

	thiz = data;
	if (strcmp(name, thiz->name))
	{
		thiz->name_matched = EINA_FALSE;
		return;
	}
	thiz->name_matched = EINA_TRUE;
	thiz->found = EINA_TRUE;
	fprintf(thiz->out, "void %s_%s_init(void)\n", thiz->ns_name, thiz->name);
	fprintf(thiz->out, "{\n");
	fprintf(thiz->out, "\tEnder_Namespace *ns;\n");
	fprintf(thiz->out, "\tEnder_Container *ec;\n");
	fprintf(thiz->out, "\tEnder_Property *ep;\n");
	fprintf(thiz->out, "\tEnder_Descriptor *parent = NULL;\n");
	fprintf(thiz->out, "\tstatic Ender_Descriptor *d = NULL;\n");
	fprintf(thiz->out, "\tif (d) return;\n");
	fprintf(thiz->out, "\tns = %s_namespace_get();\n", thiz->ns_name);
	/* the descriptor itself */
	if (parent)
	{
		fprintf(thiz->out, "\tparent = ender_descriptor_find(\"%s\");\n", parent);
	}
	fprintf(thiz->out, "\td = ender_namespace_descriptor_add(ns, \"%s\", %s_%s_new, parent, ENDER_%s);\n", thiz->ns_name, thiz->ns_name, name, ender_type_name_get(type));
	fprintf(thiz->out, "\tif (!d) return;\n");
	/* the properties */
	//ender_descriptor_property_list(descriptor, _dump_property, descriptor);
	//fprintf(out, "}\n");
}

static void _generator_on_property(void *data, const char *name, Eina_Bool relative, Ender_Container *container)
{
	Ender_Generator *thiz;

	thiz = data;
	if (!thiz->name_matched) return;
	/* first the container */
	fprintf(thiz->out, "\t{\n");
	_dump_container_recursive(thiz, container, 2);
	fprintf(thiz->out, "\t\tec = tmp2;\n");
	fprintf(thiz->out, "\t}\n");
	fprintf(thiz->out, "\tp = ender_descriptor_property_add(d, \"%s\",\n", name);
	fprintf(thiz->out, "\t\t\tec,\n");
	fprintf(thiz->out, "\t\t\t%s_%s_%s_get\n", thiz->ns_name, thiz->name, name);
	fprintf(thiz->out, "\t\t\t%s_%s_%s_set\n", thiz->ns_name, thiz->name, name);
	fprintf(thiz->out, "\t\t\t%s_%s_%s_add\n", thiz->ns_name, thiz->name, name);
	fprintf(thiz->out, "\t\t\t%s_%s_%s_remove\n", thiz->ns_name, thiz->name, name);
	fprintf(thiz->out, "\t\t\t%s_%s_%s_clear\n", thiz->ns_name, thiz->name, name);
	fprintf(thiz->out, "\t\t\t);\n");
}

static Ender_Parser_Descriptor _generator_parser = {
	.on_using = _generator_on_using,
	.on_namespace = _generator_on_namespace,
	.on_renderer = _generator_on_renderer,
	.on_property = _generator_on_property,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void ender_generator_run(const char *in, const char *element, FILE *out)
{
	Ender_Generator thiz;

	thiz.name = element;
	thiz.out = out;
	thiz.found = EINA_FALSE;
	thiz.name_matched = EINA_FALSE;

	ender_parser_parse(in, &_generator_parser, &thiz);
	if (thiz.found)
	{
		fprintf(thiz.out, "}\n");
	}
}
