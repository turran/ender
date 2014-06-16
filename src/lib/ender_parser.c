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
#include "ender_value.h"
#include "ender_item.h"
#include "ender_item_struct.h"
#include "ender_item_attr.h"
#include "ender_item_function.h"
#include "ender_item_arg.h"
#include "ender_item_object.h"
#include "ender_item_constant.h"
#include "ender_item_enum.h"
#include "ender_lib.h"
#include "ender_parser.h"

#include "ender_main_private.h"
#include "ender_lib_private.h"
#include "ender_item_attr_private.h"
#include "ender_item_struct_private.h"
#include "ender_item_function_private.h"
#include "ender_item_arg_private.h"
#include "ender_item_object_private.h"
#include "ender_item_enum_private.h"
#include "ender_item_constant_private.h"
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
	Eina_Bool failed;
} Ender_Parser;

struct _Ender_Parser_Context {
	Ender_Parser *parser;
	Ender_Parser_Tag *tag;
	Ender_Item *i;
	void *prv;
};

typedef struct _Ender_Parser_Field {
	char *type;
} Ender_Parser_Field;

typedef struct _Ender_Parser_Function {
	char *symname;
} Ender_Parser_Function;

typedef struct _Ender_Parser_Value {
	char *value;
} Ender_Parser_Value;

/*----------------------------------------------------------------------------*
 *                                 helpers                                    *
 *----------------------------------------------------------------------------*/
static Ender_Parser_Context * _ender_parser_parent_context_get(Ender_Parser *thiz)
{
	Ender_Parser_Context *c = NULL;;
	int count;

	count = eina_array_count(thiz->context);
	if (count)
	{
		c = eina_array_data_get(thiz->context, count - 1);
	}
	return c;
}

static char * _ender_parser_symname_generate(Ender_Parser *thiz, Ender_Item *i)
{
	Ender_Item *parent;
	char *ret;

	parent = ender_item_parent_get(i);
	if (parent)
	{
		char *parent_name;
		int count;

		parent_name = _ender_parser_symname_generate(thiz, parent);
		ender_item_unref(parent);
		count = asprintf(&ret, "%s.%s", parent_name, ender_item_name_get(i));
		free(parent_name);
		if (count < 0)
			return NULL;
	}
	else
	{
		if (asprintf(&ret, "%s", ender_item_name_get(i)) < 0)
			return NULL;
	}
	return ret;
}

/* TODO handle the library case type */
static char * _ender_parser_symname_get(Ender_Parser *thiz, Ender_Item *i)
{
	char *ret;
	char *c;

	ret = _ender_parser_symname_generate(thiz, i);
	for (c = ret; *c != '\0'; c++)
	{
		if (*c == '.')
			*c = '_';
	}
	return ret;
}
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

/*----------------------------------------------------------------------------*
 *                           common function attrs                            *
 *----------------------------------------------------------------------------*/
static void _ender_parser_common_function_ctor(Ender_Parser_Context *c)
{
	Ender_Parser_Function *thiz;

	thiz = calloc(1, sizeof(Ender_Parser_Function));
	c->i = ender_item_function_new();
	c->prv = thiz;
}

static void _ender_parser_common_function_dtor(Ender_Parser_Context *c)
{
	Ender_Parser_Function *thiz;

	thiz = c->prv;
	/* set the symbol name */
	if (!thiz->symname)
		thiz->symname = _ender_parser_symname_get(c->parser, c->i);
	ender_item_function_symname_set(c->i, thiz->symname);

	/* free our private context */
	if (thiz->symname)
		free(thiz->symname);
	free(thiz);
	c->prv = NULL;
}

static Eina_Bool _ender_parser_common_function_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	Ender_Parser_Function *thiz;

	thiz = c->prv;
	if (_ender_parser_item_attrs_set(c, key, value))
		return EINA_TRUE;
	else if (!strcmp(key, "symname"))
	{
		thiz->symname = strdup(value);
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
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
}

static Eina_Bool _ender_parser_struct_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	if (!strcmp(key, "name"))
	{
		ender_item_name_set(c->i, value);
		ender_lib_item_add(c->parser->lib, ender_item_ref(c->i));
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                               function tag                                 *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_parser_function_ctor(Ender_Parser_Context *c)
{
	Ender_Parser_Context *parent;

	parent = _ender_parser_parent_context_get(c->parser);
	/* the lib case */
	if (!parent)
	{
		ERR("A function must have a parent");
		return EINA_FALSE;
	}
	if (parent->i)
	{
		Ender_Item_Type type = ender_item_type_get(parent->i);
		if (type != ENDER_ITEM_TYPE_STRUCT && type != ENDER_ITEM_TYPE_OBJECT)
		{
			ERR("The parent must be of type struct or object");
			return EINA_FALSE;
		}
	}
	_ender_parser_common_function_ctor(c);

	return EINA_TRUE;
}

static void _ender_parser_function_dtor(Ender_Parser_Context *c)
{
	Ender_Parser_Context *parent;

	parent = _ender_parser_parent_context_get(c->parser);
	/* add the function, we wait for the dtor because we need
	 * the name parsed
	 */
	if (parent->i)
	{
		switch (ender_item_type_get(parent->i))
		{
			case ENDER_ITEM_TYPE_STRUCT:
			ender_item_struct_function_add(parent->i, ender_item_ref(c->i));
			break;

			case ENDER_ITEM_TYPE_OBJECT:
			ender_item_object_function_add(parent->i, ender_item_ref(c->i));
			break;

			default:
			ERR("Unsupported parent type");
			break;
		}
	}
	else
	{
		ender_lib_item_add(c->parser->lib, ender_item_ref(c->i));
	}

	_ender_parser_common_function_dtor(c);
}

static Eina_Bool _ender_parser_function_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	if (_ender_parser_common_function_attrs_set(c, key, value))
		return EINA_TRUE;
	return EINA_FALSE;
}
/*----------------------------------------------------------------------------*
 *                                object tag                                  *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_parser_object_function_ctor(Ender_Parser_Context *c)
{
	Ender_Parser_Context *parent;

	parent = _ender_parser_parent_context_get(c->parser);
	if ((!parent) || (!parent->i) ||
			!(ender_item_type_get(parent->i) == ENDER_ITEM_TYPE_OBJECT))
	{
		ERR("A ctor must be a child of an object");
		return EINA_FALSE;
	}
	_ender_parser_common_function_ctor(c);
	return EINA_TRUE;
}

static void _ender_parser_object_function_dtor(Ender_Parser_Context *c)
{
	Ender_Parser_Context *parent;

	parent = _ender_parser_parent_context_get(c->parser);
	ender_item_object_function_add(parent->i, ender_item_ref(c->i));
	_ender_parser_common_function_dtor(c);
}

static Eina_Bool _ender_parser_object_ctor(Ender_Parser_Context *c)
{
	c->i = ender_item_object_new();

	return EINA_TRUE;
}

static void _ender_parser_object_dtor(Ender_Parser_Context *c)
{
}

static Eina_Bool _ender_parser_object_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	if (!strcmp(key, "name"))
	{
		ender_item_name_set(c->i, value);
		ender_lib_item_add(c->parser->lib, ender_item_ref(c->i));
	}
	else if (!strcmp(key, "inherits"))
	{
		Ender_Item *inherits;

		inherits = ender_lib_item_find(c->parser->lib, value);
		if (!inherits)
		{
			WRN("Inherit '%s' not found", value);
		}
		else
		{
			ender_item_object_inherit_set(c->i, inherits);
		}
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                                arg tag                                     *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_parser_arg_ctor(Ender_Parser_Context *c)
{
	Ender_Parser_Context *parent;

	parent = _ender_parser_parent_context_get(c->parser);
	if ((!parent) || (!parent->i) ||
			!(ender_item_type_get(parent->i) == ENDER_ITEM_TYPE_FUNCTION))
	{
		ERR("An arg must be a child of a function");
		return EINA_FALSE;
	}
	c->i = ender_item_arg_new();
	/* add the arg to the function */
	parent = _ender_parser_parent_context_get(c->parser);
	ender_item_function_arg_add(parent->i, ender_item_ref(c->i));

	return EINA_TRUE;
}

static void _ender_parser_arg_dtor(Ender_Parser_Context *c)
{
}

static Eina_Bool _ender_parser_arg_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	if (_ender_parser_item_attrs_set(c, key, value))
		return EINA_TRUE;
	else if (!strcmp(key, "direction"))
	{
		Ender_Item_Arg_Direction dir = ENDER_ITEM_ARG_DIRECTION_IN;

		if (!strcmp(value, "in"))
			dir = ENDER_ITEM_ARG_DIRECTION_IN;
		else if (!strcmp(value, "out"))
			dir = ENDER_ITEM_ARG_DIRECTION_OUT;
		else if (!strcmp(value, "inout"))
			dir = ENDER_ITEM_ARG_DIRECTION_IN_OUT;
		else
		{
			WRN("Unknown direction '%s'", value);
		}
		ender_item_arg_direction_set(c->i, dir); 
	}
	else if (!strcmp(key, "type"))
	{
		Ender_Item *type;

		type = ender_lib_item_find(c->parser->lib, value);
		if (!type)
		{
			ERR("Can not find type '%s'", value);
			return EINA_FALSE;
		}
		ender_item_arg_type_set(c->i, ender_item_ref(type));
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                                return tag                                     *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_parser_return_ctor(Ender_Parser_Context *c)
{
	Ender_Parser_Context *parent;

	parent = _ender_parser_parent_context_get(c->parser);
	if ((!parent) || (!parent->i) ||
			!(ender_item_type_get(parent->i) == ENDER_ITEM_TYPE_FUNCTION))
	{
		ERR("An return must be a child of a function");
		return EINA_FALSE;
	}
	c->i = ender_item_arg_new();
	ender_item_arg_flags_set(c->i, ENDER_ITEM_ARG_FLAG_IS_RETURN);
	/* add the return to the function */
	parent = _ender_parser_parent_context_get(c->parser);
	ender_item_function_ret_set(parent->i, ender_item_ref(c->i));

	return EINA_TRUE;
}

static void _ender_parser_return_dtor(Ender_Parser_Context *c)
{
}

static Eina_Bool _ender_parser_return_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	if (_ender_parser_item_attrs_set(c, key, value))
		return EINA_TRUE;
	else if (!strcmp(key, "type"))
	{
		Ender_Item *type;

		type = ender_lib_item_find(c->parser->lib, value);
		if (!type)
		{
			ERR("Can not find type '%s'", value);
			return EINA_FALSE;
		}
		ender_item_arg_type_set(c->i, ender_item_ref(type));
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                                method tag                                  *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_parser_method_ctor(Ender_Parser_Context *c)
{
	Ender_Parser_Function *thiz;
	Ender_Parser_Context *parent;
	Ender_Item_Type type;

	/* safety checks */
	parent = _ender_parser_parent_context_get(c->parser);
	if (!parent)
	{
		ERR("A method must have a parent");
		return EINA_FALSE;
	}
	if (!parent->i)
	{
		ERR("The parent must be an item");
		return EINA_FALSE;
	}
	type = ender_item_type_get(parent->i);
	if ((type != ENDER_ITEM_TYPE_STRUCT) && (type != ENDER_ITEM_TYPE_OBJECT))
	{
		ERR("A method must be a child of a struct or object");
		return EINA_FALSE;
	}

	/* our private context */
	thiz = calloc(1, sizeof(Ender_Parser_Field));
	c->prv = thiz;
	c->i = ender_item_function_new();

	/* set it as method */
	ender_item_function_flags_set(c->i, ENDER_ITEM_FUNCTION_FLAG_IS_METHOD);

	/* add the function */
	switch (ender_item_type_get(parent->i))
	{
		case ENDER_ITEM_TYPE_STRUCT:
		ender_item_struct_function_add(parent->i, ender_item_ref(c->i));
		break;

		case ENDER_ITEM_TYPE_OBJECT:
		ender_item_object_function_add(parent->i, ender_item_ref(c->i));
		break;

		default:
		ERR("Unsupported parent type");
		break;
	}

	return EINA_TRUE;
}

static void _ender_parser_method_dtor(Ender_Parser_Context *c)
{
	Ender_Parser_Function *thiz = c->prv;
	Ender_Item *p;

	/* in case there is no parent, register the function, otherwise
	 * the function will be included on the parent (struct, object, whaveter)
	  */
	p = ender_item_parent_get(c->i);
	if (!p) ender_lib_item_add(c->parser->lib, ender_item_ref(c->i));
	ender_item_unref(p);

	/* set the symbol name */
	if (!thiz->symname)
		thiz->symname = _ender_parser_symname_get(c->parser, c->i);
	ender_item_function_symname_set(c->i, thiz->symname);
	
	/* free our private context */
	if (thiz->symname)
		free(thiz->symname);
	free(thiz);
	c->prv = NULL;
}

static Eina_Bool _ender_parser_method_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	if (_ender_parser_common_function_attrs_set(c, key, value))
		return EINA_TRUE;
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                                 ctor tag                                   *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_parser_ctor_ctor(Ender_Parser_Context *c)
{
	if (!_ender_parser_object_function_ctor(c))
		return EINA_FALSE;

	ender_item_function_flags_set(c->i, ENDER_ITEM_FUNCTION_FLAG_CTOR);

	return EINA_TRUE;
}

static void _ender_parser_ctor_dtor(Ender_Parser_Context *c)
{
	_ender_parser_object_function_dtor(c);
}

static Eina_Bool _ender_parser_ctor_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	if (_ender_parser_common_function_attrs_set(c, key, value))
		return EINA_TRUE;
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                                 value tag                                  *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_parser_value_ctor(Ender_Parser_Context *c)
{
	Ender_Parser_Value *thiz;
	Ender_Parser_Context *parent;
	Ender_Item *i32;

	/* a value can only be child of the main lib */
	parent = _ender_parser_parent_context_get(c->parser);
	if (!parent)
		return EINA_FALSE;
	if (parent->i)
	{
		Ender_Item_Type type = ender_item_type_get(parent->i);
		if (type != ENDER_ITEM_TYPE_ENUM)
			return EINA_FALSE;
	}

	/* our own private data */
	thiz = calloc(1, sizeof(Ender_Parser_Value));

	c->i = ender_item_constant_new();
	c->prv = thiz;

	/* for enum values the type is always an int32 */
	i32 = ender_lib_item_find(c->parser->lib, "int32");
	ender_item_constant_type_set(c->i, i32);

	return EINA_TRUE;
}

static void _ender_parser_value_dtor(Ender_Parser_Context *c)
{
	Ender_Parser_Value *thiz;
	Ender_Parser_Context *parent;
	
	parent = _ender_parser_parent_context_get(c->parser);
	thiz = c->prv;
	if (thiz->value)
	{
		/* TODO parse the value */
		WRN("TODO");
		free(thiz->value);
	}
	else
	{
		Ender_Item *i;
		Ender_Value v;
		Eina_List *values;

		/* pick the last value and autoincrement it */
		values = ender_item_enum_values_get(parent->i);
		if (values)
		{
			Eina_List *last;

			last = eina_list_last(values);
			ender_item_constant_value_get(last->data, &v);
			/* auto increment the value */
			v.i32++;
		}
		else
		{
			v.i32 = 0;
		}
		ender_item_constant_value_set(c->i, &v);
		EINA_LIST_FREE(values, i)
			ender_item_unref(i);
	}
	ender_item_enum_value_add(parent->i, ender_item_ref(c->i));
	free(thiz);
}

static Eina_Bool _ender_parser_value_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	Ender_Parser_Value *thiz;
	
	thiz = c->prv;
	if (_ender_parser_item_attrs_set(c, key, value))
		return EINA_TRUE;
	else if (!strcmp(key, "value"))
	{
		thiz->value = strdup(value);
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                                 enum tag                                  *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_parser_enum_ctor(Ender_Parser_Context *c)
{
	Ender_Parser_Context *parent;

	/* a enum can only be child of the main lib */
	parent = _ender_parser_parent_context_get(c->parser);
	if (parent->i)
	{
		ERR("An enum must be child of the lib only");
		return EINA_FALSE;
	}

	c->i = ender_item_enum_new();

	return EINA_TRUE;
}

static void _ender_parser_enum_dtor(Ender_Parser_Context *c)
{
}

static Eina_Bool _ender_parser_enum_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	if (!strcmp(key, "name"))
	{
		ender_item_name_set(c->i, value);
		ender_lib_item_add(c->parser->lib, ender_item_ref(c->i));
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                                 field tag                                  *
 *----------------------------------------------------------------------------*/
static Eina_Bool _ender_parser_field_ctor(Ender_Parser_Context *c)
{
	Ender_Parser_Field *f;
	Ender_Parser_Context *parent;

	/* a field can only be child of a struct */
	parent = _ender_parser_parent_context_get(c->parser);
	if ((!parent) || (!parent->i) || (ender_item_type_get(parent->i) != ENDER_ITEM_TYPE_STRUCT))
	{
		ERR("A field must be a child of a struct");
		return EINA_FALSE;
	}

	/* our private context */
	f = calloc(1, sizeof(Ender_Parser_Field));
	c->prv = f;
	c->i = ender_item_attr_new();

	return EINA_TRUE;
}

static void _ender_parser_field_dtor(Ender_Parser_Context *c)
{
	Ender_Parser_Context *parent;
	Ender_Parser_Field *field = c->prv;
	Ender_Item *i;

	/* set the type */
	i = ender_lib_item_find(c->parser->lib, field->type);
	if (!i)
	{
		ERR("Can not find type '%s'", field->type);
		goto done;
	}
	ender_item_attr_type_set(c->i, ender_item_ref(i));

	/* add the field */
	parent = _ender_parser_parent_context_get(c->parser);
	ender_item_struct_field_add(parent->i, ender_item_ref(c->i));
	ender_item_unref(i);
done:
	/* free our private context */
	if (field->type)
		free(field->type);
	free(field);
	c->prv = NULL;
}

static Eina_Bool _ender_parser_field_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	Ender_Parser_Field *f = c->prv;

	if (_ender_parser_item_attrs_set(c, key, value))
		return EINA_TRUE;
	else if (!strcmp(key, "type"))
	{
		f->type = strdup(value);
	}
	else
	{
		return EINA_FALSE;
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

static Eina_Bool _ender_parser_include_ctor(Ender_Parser_Context *c)
{
	return EINA_TRUE;
}

static Eina_Bool _ender_parser_include_attrs_set(Ender_Parser_Context *c,
		const char *key, const char *value)
{
	if (!strcmp(key, "name"))
	{
		const Ender_Lib *dep;
		Enesim_Stream *s;
		char *file = NULL;

		if (asprintf(&file, "%s/%s.ender", PACKAGE_DATA_DIR, value) < 0)
			return EINA_FALSE;

		s = enesim_stream_file_new(file, "r");
		DBG("Including %s %p", file, s);
		ender_parser_parse(s);
		free(file);

		dep = ender_lib_find(value);
		if (dep)
		{
			ender_lib_dependency_add(c->parser->lib, dep);
		}
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Ender_Parser_Tag _tags[] = {
	{ "lib", _ender_parser_lib_ctor, _ender_parser_lib_dtor, _ender_parser_lib_attrs_set },
	{ "type", NULL, NULL, NULL },
	{ "include", _ender_parser_include_ctor, NULL, _ender_parser_include_attrs_set },
	{ "object", _ender_parser_object_ctor, _ender_parser_object_dtor, _ender_parser_object_attrs_set },
	{ "struct", _ender_parser_struct_ctor, _ender_parser_struct_dtor, _ender_parser_struct_attrs_set },
	{ "enum", _ender_parser_enum_ctor, _ender_parser_enum_dtor, _ender_parser_enum_attrs_set },
	{ "value", _ender_parser_value_ctor, _ender_parser_value_dtor, _ender_parser_value_attrs_set },
	{ "field", _ender_parser_field_ctor, _ender_parser_field_dtor, _ender_parser_field_attrs_set },
	{ "method", _ender_parser_method_ctor, _ender_parser_method_dtor, _ender_parser_method_attrs_set },
	{ "function", _ender_parser_function_ctor, _ender_parser_function_dtor, _ender_parser_function_attrs_set },
	{ "ctor", _ender_parser_ctor_ctor, _ender_parser_ctor_dtor, _ender_parser_ctor_attrs_set },
	{ "arg", _ender_parser_arg_ctor, _ender_parser_arg_dtor, _ender_parser_arg_attrs_set },
	{ "return", _ender_parser_return_ctor, _ender_parser_return_dtor, _ender_parser_return_attrs_set },
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

	c = calloc(1, sizeof(Ender_Parser_Context));
	c->parser = thiz;

	/* if the parser has failed, keep failing */
	if (thiz->failed)
	{
		thiz->failed++;
		goto done;
	}

	c->tag = _ender_parser_get_tag(content);
	if (c->tag)
	{
		if (c->tag->ctor_cb)
		{
			if (c->tag->ctor_cb(c))
			{
				attrs = eina_simple_xml_tag_attributes_find(content, length);
				eina_simple_xml_attributes_parse(attrs, length, _ender_parser_attrs_set_cb, c);
			}
			else
			{
				ERR("The parsing of tag '%s' has failed", c->tag->name);
				thiz->failed++;
			}
		}
	}

done:		
	return c;
}

static void _ender_parser_context_free(Ender_Parser_Context *c)
{
	Ender_Parser *thiz;

	thiz = c->parser;
	if (c)
	{
		if (c->tag)
		{
			if (thiz->failed)
			{
				thiz->failed--;
			}
			else
			{
				if (c->tag->dtor_cb)
					c->tag->dtor_cb(c);
			}
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
