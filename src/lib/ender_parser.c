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
#include "ender_lib_private.h"
#include "ender_item_struct_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

typedef enum _Ender_Library_Case {
	ENDER_LIBRARY_CASE_CAMEL, /* backColor */
	ENDER_LIBRARY_CASE_PASCAL, /* BackColor */
	ENDER_LIBRARY_CASE_UNDERSCORE, /* back_color */
} Ender_Library_Case;

typedef enum _Ender_Library_Notation {
	ENDER_LIBRARY_NOTATION_LATIN, /* foo_back_color_get */
} Ender_Library_Notation;

typedef struct _Ender_Parser_Context Ender_Parser_Context;

typedef Eina_Bool (*Ender_Parser_Tag_Ctor_Cb)(Ender_Parser_Context *c);
typedef Eina_Bool (*Ender_Parser_Tag_Attrs_Set_Cb)(Ender_Parser_Context *c, const char *key,
		const char *value);
typedef void (*Ender_Parser_Tag_Dtor_Cb)(Ender_Parser_Context *c);

typedef struct _Ender_Parser_Tag
{
	const char *name;
	Ender_Parser_Tag_Ctor_Cb ctor_cb;
	Ender_Parser_Tag_Dtor_Cb dtor_cb;
	Ender_Parser_Tag_Attrs_Set_Cb attrs_set_cb;
} Ender_Parser_Tag;

typedef struct _Ender_Parser
{
	Eina_Array *context;
	Ender_Lib *lib;
	Ender_Library_Case lcase;
	Ender_Library_Notation lnotation;
	Enesim_Stream *s;
} Ender_Parser;

struct _Ender_Parser_Context {
	Ender_Parser *parser;
	Ender_Parser_Tag *tag;
	Ender_Item *i;
	void *prv;
};

typedef struct _Ender_Parser_Field {
	char *type;
	char *name;
} Ender_Parser_Field;

/*----------------------------------------------------------------------------*
 *                            common item attrs                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_parser_item_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	if (!strcmp(key, "name"))
	{
		ender_item_name_set(c->i, value);
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static void _ender_parser_item_dtor(Ender_Parser_Context *c)
{
	ender_lib_item_add(c->parser->lib, ender_item_ref(c->i));
}

/*----------------------------------------------------------------------------*
 *                                struct tag                                  *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_parser_struct_ctor(Ender_Parser_Context *c)
{
	c->i = ender_item_struct_new();

	return EINA_TRUE;
}

static void _ender_parser_struct_dtor(Ender_Parser_Context *c)
{
	_ender_parser_item_dtor(c);
}

static Eina_Bool _ender_parser_struct_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	if (_ender_parser_item_attrs_set(c, key, value))
		return EINA_TRUE;
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                                field tag                                  *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_parser_field_ctor(Ender_Parser_Context *c)
{
	Ender_Parser_Field *f;

	f = calloc(1, sizeof(Ender_Parser_Field));
	c->prv = f;
	return EINA_TRUE;
}

static void _ender_parser_field_dtor(Ender_Parser_Context *c)
{
	Ender_Parser_Field *f = c->prv;
	Ender_Item *i;

	/* create the item based on the type */
	i = ender_lib_item_find(c->parser->lib, f->type);
	/* TODO check if it is a basic type, if so, we can create a new
	 * property based on it
	 */
	ender_item_unref(i);

	if (f->name)
		free(f->name);
	if (f->type)
		free(f->type);
	free(f);
	c->prv = NULL;
}

static Eina_Bool _ender_parser_field_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	Ender_Parser_Field *f = c->prv;

	if (!strcmp(key, "name"))
	{
		f->name = strdup(value);
	}
	else if (!strcmp(key, "type"))
	{
		f->type = strdup(value);
	}

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                                 lib tag                                    *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_parser_lib_ctor(Ender_Parser_Context *c)
{
	if (c->parser->lib)
		return EINA_FALSE;

	DBG("New lib created");
	c->parser->lib = ender_lib_new();
	return EINA_TRUE;
}

static void _ender_parser_lib_dtor(Ender_Parser_Context *c)
{
	ender_lib_register(c->parser->lib);
}

static Eina_Bool _ender_parser_lib_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	if (!strcmp(key, "case"))
	{
		if (!strcmp(value, "underscore"))
			c->parser->lcase = ENDER_LIBRARY_CASE_UNDERSCORE;
		else if (!strcmp(value, "camel"))
			c->parser->lcase = ENDER_LIBRARY_CASE_CAMEL;
		else if (!strcmp(value, "pascal"))
			c->parser->lcase = ENDER_LIBRARY_CASE_PASCAL;
	}
	else if (!strcmp(key, "name"))
	{
		ender_lib_name_set(c->parser->lib, value);
	}
	else if (!strcmp(key, "version"))
	{
		int version = atoi(value);
		ender_lib_version_set(c->parser->lib, version);
	}
	return EINA_TRUE;
}

static Ender_Parser_Tag _tags[] = {
	{ "lib", _ender_parser_lib_ctor, _ender_parser_lib_dtor, _ender_parser_lib_attrs_set },
	{ "type", NULL, NULL, NULL },
	{ "include", NULL, NULL, NULL },
	{ "namespace", NULL, NULL, NULL },
	{ "struct", _ender_parser_struct_ctor, _ender_parser_struct_dtor, _ender_parser_struct_attrs_set },
	{ "field", _ender_parser_field_ctor, _ender_parser_field_dtor, _ender_parser_field_attrs_set },
	{ "method", NULL, NULL, NULL },
	{ "class", NULL, NULL, NULL },
};

static Ender_Parser_Tag * _ender_parser_get_tag(const char *name)
{
	Ender_Parser_Tag *ret = NULL;
	int i;

	for (i = 0; i < sizeof(_tags) / sizeof(Ender_Parser_Tag); i++)
	{
		Ender_Parser_Tag *t = &_tags[i];
		if (!strncmp(name, t->name, strlen(t->name)))
		{
			ret = t;
			break;
		}
	}
	return ret;
}

static Eina_Bool _ender_parser_attrs_set_cb(void *data, const char *key,
		const char *value)
{
	Ender_Parser_Context *c = data;

	if (!c->tag) return EINA_FALSE;
	if (c->tag->attrs_set_cb)
	{
		return c->tag->attrs_set_cb(c, key, value); 
	}
	return EINA_FALSE;
}

/*----------------------------------------------------------------------------*
 *                         context related functions                          *
 *----------------------------------------------------------------------------*/
static Ender_Parser_Context * _ender_parser_tag_new(Ender_Parser *thiz,
		const char *content, unsigned int length)
{
	Ender_Parser_Context *c;
	const char *attrs = NULL;
	int attr_length = 0;

	/* TODO check if we already have a context, if so, check if it can create a new child */
	c = calloc(1, sizeof(Ender_Parser_Context));
	c->tag = _ender_parser_get_tag(content);
	c->parser = thiz;

	/* TODO call the creator */
	if (c->tag)
	{
		if (c->tag->ctor_cb)
			c->tag->ctor_cb(c);
		attrs = eina_simple_xml_tag_attributes_find(content, length);
		eina_simple_xml_attributes_parse(attrs, length, _ender_parser_attrs_set_cb, c);
	}

	return c;
}

static void _ender_parser_context_free(Ender_Parser_Context *c)
{
	if (c)
	{
		if (c->tag)
		{
			if (c->tag->dtor_cb)
				c->tag->dtor_cb(c);
		}
		if (c->i)
			ender_item_unref(c->i);
		free(c);
	}
}
/*----------------------------------------------------------------------------*
 *                      Eina's simple XML interface                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_parser_parse_cb(void *data, Eina_Simple_XML_Type type,
		const char *content, unsigned int offset,
		unsigned int length)
{
	Ender_Parser *thiz = data;
	Ender_Parser_Context *c;

	switch (type)
	{
		case EINA_SIMPLE_XML_OPEN:
		c = _ender_parser_tag_new(thiz, content, length);
		eina_array_push(thiz->context, c);
		break;

		case EINA_SIMPLE_XML_OPEN_EMPTY:
		c = _ender_parser_tag_new(thiz, content, length);
		_ender_parser_context_free(c);
		break;

		case EINA_SIMPLE_XML_CLOSE:
		c = eina_array_pop(thiz->context);
		_ender_parser_context_free(c);
		break;

		case EINA_SIMPLE_XML_DATA:
		case EINA_SIMPLE_XML_CDATA:
		case EINA_SIMPLE_XML_DOCTYPE_CHILD:
		case EINA_SIMPLE_XML_IGNORED:
		case EINA_SIMPLE_XML_COMMENT:
		case EINA_SIMPLE_XML_DOCTYPE:
		default:
		break;
	}
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool ender_parser_parse(Enesim_Stream *s)
{
	Ender_Parser *thiz;
	Eina_Bool ret = EINA_FALSE;
	void *content;
	size_t len = 0;

	if (!s) return EINA_FALSE;

	thiz = calloc(1, sizeof(Ender_Parser));
	thiz->context = eina_array_new(1);
	thiz->s = s;

	content = enesim_stream_mmap(thiz->s, &len);
	if (!content) goto no_mmap;

	eina_simple_xml_parse(content, len, EINA_TRUE, _ender_parser_parse_cb, thiz);
	enesim_stream_munmap(thiz->s, content);
	ret = EINA_TRUE;
no_mmap:
	eina_array_free(thiz->context);
	enesim_stream_unref(thiz->s);
	free(thiz);

	return ret;
}
