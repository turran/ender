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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Hash *_marshallers = NULL;

static void _ender_marshaller_arg_append(char *dst, Ender_Container *c)
{
	Ender_Value_Type type;
	const char *vname;

	type = ender_container_type_get(c);
	vname = ender_value_type_string_to(type);
	strcat(dst, vname);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ender_marshaller_init(void)
{
	_marshallers = eina_hash_string_superfast_new(NULL);
	eina_hash_add(_marshallers, "void__void", ender_marshaller_void__void);
	eina_hash_add(_marshallers, "void__STRING_STRING", ender_marshaller_void__string_string);
	eina_hash_add(_marshallers, "ENDER__STRING", ender_marshaller_ender__string);
	eina_hash_add(_marshallers, "STRING__STRING", ender_marshaller_string__string);
	eina_hash_add(_marshallers, "ENDER__void", ender_marshaller_ender__void);
}

void ender_marshaller_shutdown(void)
{
	eina_hash_free(_marshallers);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Marshaller ender_marshaller_find_list(Ender_Container *ret,
		Eina_List *args)
{
	Ender_Container *arg;
	Ender_Marshaller marshaller;
	Eina_List *l;
	char name[PATH_MAX] = { 0 };

	if (ret)
	{
		_ender_marshaller_arg_append(name, ret);
	}
	else
	{
		strcpy(name, "void");
	}
	/* add a __ */
	strcat(name, "__");

	if (args)
	{
		_ender_marshaller_arg_append(name, args->data);
		args = args->next;
	}
	else
	{
		strcat(name, "void");
	}


	EINA_LIST_FOREACH(args, l, arg)
	{
		strcat(name, "_");
		_ender_marshaller_arg_append(name, arg);
	}
	DBG("Looking for %s %p", name, marshaller);
	marshaller = eina_hash_find(_marshallers, name);
	if (!marshaller)
	{
		ERR("No marshaller registered for '%s' format", name);
	}
	return marshaller;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Marshaller ender_marshaller_find(Ender_Container *ret,
		...)
{
	Ender_Container *arg;
	Ender_Marshaller marshaller;
	Eina_List *args = NULL;
	va_list va_args;

	va_start(va_args, ret);
	while ((arg = va_arg(va_args, Ender_Container *)))
	{
		args = eina_list_append(args, arg);
	}
	va_end(va_args);
	marshaller = ender_marshaller_find_list(ret, args);
	if (args)
	{
		eina_list_free(args);
	}
	return marshaller;
}

/* generic marshallers */
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_marshaller_void__void(void *data, Ender_Accessor f,
		Ender_Value *ret, Eina_List *args)
{
	typedef void (*_Function)(void *o);
	_Function _f;

	_f = (_Function)(f);
	_f(data);
	return EINA_TRUE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool ender_marshaller_string__string(void *data, Ender_Accessor f,
		Ender_Value *ret, Eina_List *args)
{
	typedef char * (*_Function)(void *o, const char *string);
	_Function _f;
	char *r;
	Ender_Value *arg0;
	const char *str;

	_f = (_Function)(f);
	/* get the args */
	if (!args) return EINA_FALSE;

	arg0 = args->data;
	str = ender_value_string_get(arg0);
	r = _f(data, str);
	/* set the return value */
	ender_value_string_set(ret, r);

	return EINA_TRUE;
}

EAPI Eina_Bool ender_marshaller_void__string_string(void *data, Ender_Accessor f,
		Ender_Value *ret, Eina_List *args)
{
	typedef void (*_Function)(void *o, const char *arg0, const char *arg1);
	_Function _f;
	Ender_Value *arg0;
	Ender_Value *arg1;
	const char *str0;
	const char *str1;

	_f = (_Function)(f);
	/* get the args */
	if (eina_list_count(args) != 2)
		return EINA_FALSE;

	arg0 = eina_list_nth(args, 0);
	arg1 = eina_list_nth(args, 1);
	str0 = ender_value_string_get(arg0);
	str1 = ender_value_string_get(arg1);
	_f(data, str0, str1);

	return EINA_TRUE;
}

EAPI Eina_Bool ender_marshaller_ender__string(void *data, Ender_Accessor f,
		Ender_Value *ret, Eina_List *args)
{
	typedef Ender_Element * (*_Function)(void *o, const char *string);
	_Function _f;
	Ender_Element *e;
	Ender_Value *arg0;
	const char *str;

	_f = (_Function)(f);
	/* get the args */
	if (!args) return EINA_FALSE;

	arg0 = args->data;
	str = ender_value_string_get(arg0);
	e = _f(data, str);
	/* set the return value */
	ender_value_ender_set(ret, e);

	return EINA_TRUE;
}

EAPI Eina_Bool ender_marshaller_ender__void(void *data, Ender_Accessor f,
		Ender_Value *ret, Eina_List *args)
{
	typedef Ender_Element * (*_Function)(void *o);
	_Function _f;
	Ender_Element *e;
	const char *str;

	_f = (_Function)(f);
	e = _f(data);
	/* set the return value */
	ender_value_ender_set(ret, e);

	return EINA_TRUE;
}
