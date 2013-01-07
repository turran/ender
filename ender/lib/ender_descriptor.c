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
typedef struct _Ender_Descriptor_List_Data
{
	Eina_Bool ret;
	Ender_Descriptor_List_Callback real_cb;
	void *real_data;
} Ender_Descriptor_List_Data;

typedef struct _Ender_Descriptor_Find_Data
{
	Ender_Descriptor *ret;
	const char *name;
} Ender_Descriptor_Find_Data;

typedef void (*Ender_Value_Accessor)(Ender_Value *v, Ender_Accessor acc,
		void *e);
typedef void (*Ender_Value_Relative_Accessor)(Ender_Value *v, Ender_Accessor acc,
		Ender_Element *e, void *parent);

static Ender_Value_Accessor _setters[ENDER_PROPERTY_TYPES];
static Ender_Value_Accessor _getters[ENDER_PROPERTY_TYPES];
static Ender_Value_Relative_Accessor _relative_accessors[ENDER_PROPERTY_TYPES];
static Ender_Descriptor_Backend _backends[ENDER_TYPES];

static Ender_Property * _descriptor_property_get(Ender_Descriptor *e, const char *name);

static inline void _value_set(Ender_Property *p, Ender_Element *e, Ender_Value *v,
		Ender_Accessor acc)
{
	if (ender_property_is_relative(p))
	{
		Ender_Element *parent;

		parent = ender_element_parent_get(e);
		if (parent)
		{
			void *rparent;

			rparent = ender_element_object_get(parent);
			_relative_accessors[v->container->type](v, acc, e, rparent);
		}
		else WRN("Trying to set a relative property with no parent");
	}
	else
	{
		void *object;

		object = ender_element_object_get(e);
		_setters[v->container->type](v, acc, object);
	}

}

static Eina_Bool _descriptor_list_namespace_cb(Ender_Descriptor *thiz, void *user_data)
{
	Ender_Descriptor_List_Data *data = user_data;

	data->ret = data->real_cb(thiz, data->real_data);
	return data->ret;
}
static Eina_Bool _descriptor_list_cb(Ender_Namespace *ns, void *user_data)
{
	Ender_Descriptor_List_Data *data = user_data;

	ender_namespace_descriptor_list(ns, _descriptor_list_namespace_cb, data);
	return data->ret;
}

static Eina_Bool _descriptor_find_namespace_cb(Ender_Descriptor *thiz, void *user_data)
{
	Ender_Descriptor_Find_Data *data = user_data;

	if (!strcmp(thiz->name, data->name))
	{
		data->ret = thiz;
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _descriptor_find_cb(Ender_Namespace *ns, void *user_data)
{
	Ender_Descriptor_Find_Data *data = user_data;

	ender_namespace_descriptor_list(ns, _descriptor_find_namespace_cb, data);
	return data->ret ? EINA_FALSE : EINA_TRUE;
}

static Ender_Property * _descriptor_property_get(Ender_Descriptor *e, const char *name)
{
	Ender_Property *prop;

	prop = eina_ordered_hash_find(e->properties, name);
	if (prop) return prop;
	if (!e->parent) return NULL;

	return _descriptor_property_get(e->parent, name);
}
/*----------------------------------------------------------------------------*
 *                                  int32                                     *
 *----------------------------------------------------------------------------*/
static void _ender_int32_get(Ender_Value *v, Ender_Getter get, void *e)
{
	get(e, &v->data.i32);
}

static void _ender_int32_set(Ender_Value *v, Ender_Setter set, void *e)
{
	set(e, v->data.i32);
}

static void _ender_relative_int32_set(Ender_Value *v, Ender_Setter set,
		Ender_Element *e, void *parent)
{
	set(parent, e, v->data.i32);
}
/*----------------------------------------------------------------------------*
 *                         uint32 / argb / bool                               *
 *----------------------------------------------------------------------------*/
static void _ender_uint32_get(Ender_Value *v, Ender_Getter get, void *e)
{
	get(e, &v->data.u32);
}

static void _ender_uint32_set(Ender_Value *v, Ender_Setter set, void *e)
{
	set(e, v->data.u32);
}

static void _ender_relative_uint32_set(Ender_Value *v, Ender_Setter set,
		Ender_Element *e, void *parent)
{
	set(parent, e, v->data.u32);
}
/*----------------------------------------------------------------------------*
 *                              uint64 / in64                                 *
 *----------------------------------------------------------------------------*/
static void _ender_int64_get(Ender_Value *v, Ender_Getter get, void *e)
{
	get(e, &v->data.i64);
}

static void _ender_int64_set(Ender_Value *v, Ender_Setter set, void *e)
{
	set(e, v->data.i64);
}

static void _ender_relative_int64_set(Ender_Value *v, Ender_Setter set,
		Ender_Element *e, void *parent)
{
	set(parent, e, v->data.i64);
}
/*----------------------------------------------------------------------------*
 *                                   double                                   *
 *----------------------------------------------------------------------------*/
static void _ender_double_get(Ender_Value *v, Ender_Setter set, void *e)
{
	set(e, &v->data.d);
}

static void _ender_double_set(Ender_Value *v, Ender_Setter set, void *e)
{
	set(e, v->data.d);
}

static void _ender_relative_double_set(Ender_Value *v, Ender_Setter set, Ender_Element *e,
		void *parent)
{
	set(parent, e, v->data.d);
}
/*----------------------------------------------------------------------------*
 *                                  pointer                                   *
 *----------------------------------------------------------------------------*/
/* used for known pointers: struct, unions, etc */
static void _ender_pointer_get(Ender_Value *v, Ender_Getter get,
		void *e)
{
	/* FIXME here it all depends if the struct is alloc'ed or not */
	get(e, v->data.ptr);
}

/* used for string, struct, union, object */
static void _ender_pointer_set(Ender_Value *v, Ender_Setter set,
		void *e)
{
	set(e, v->data.ptr);
}

/* used for string, struct, union, object */
static void _ender_relative_pointer_set(Ender_Value *v, Ender_Setter set,
		Ender_Element *e, void *parent)
{
	set(parent, e, v->data.ptr);
}
/*----------------------------------------------------------------------------*
 *                                 objects                                    *
 *----------------------------------------------------------------------------*/
/* valid for every object: ender, object, etc ... */
static void _ender_object_get(Ender_Value *v, Ender_Getter get,
		void *object)
{
	get(object, &v->data.ptr);
}
static void _ender_object_set(Ender_Value *v, Ender_Setter set,
		void *object)
{
	set(object, v->data.ptr);
}

static void _ender_relative_object_set(Ender_Value *v, Ender_Setter set,
		Ender_Element *e, void *parent)
{
	set(parent, e, v->data.ptr);
}
/*----------------------------------------------------------------------------*
 *                                   dummy                                    *
 *----------------------------------------------------------------------------*/
static void _ender_dummy_set(Ender_Value *v, Ender_Setter set,
		void *o)
{

}

static void _ender_dummy_get(Ender_Value *v, Ender_Getter set,
		void *o)
{

}

static void _ender_relative_dummy_set(Ender_Value *v, Ender_Setter set,
		Ender_Element *e, void *parent)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                        The property interface                              *
 *----------------------------------------------------------------------------*/
/* this one is used for set/add/remove */
void ender_descriptor_object_property_set(Ender_Property *p,
		Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Descriptor_Property *dprop = data;

	_value_set(p, e, v, dprop->set);
}

void ender_descriptor_object_property_add(Ender_Property *p,
		Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Descriptor_Property *dprop = data;

	_value_set(p, e, v, dprop->add);
}

void ender_descriptor_object_property_remove(Ender_Property *p,
		Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Descriptor_Property *dprop = data;

	_value_set(p, e, v, dprop->remove);
}

void ender_descriptor_object_property_get(Ender_Property *p,
		Ender_Element *e, Ender_Value *v, void *data)
{
	Ender_Descriptor_Property *dprop = data;
	void *object;

	object = ender_element_object_get(e);
	_getters[v->container->type](v, dprop->get, object);
}

void ender_descriptor_object_property_clear(Ender_Property *p,
		Ender_Element *e, void *data)
{
	Ender_Descriptor_Property *dprop = data;
	void *object;

	object = ender_element_object_get(e);
	dprop->clear(object);
}

Eina_Bool ender_descriptor_object_property_is_set(Ender_Property *p,
		Ender_Element *e, void *data)
{
	Ender_Descriptor_Property *dprop = data;
	void *object;

	object = ender_element_object_get(e);
	return dprop->is_set(object);
}

void ender_descriptor_object_property_free(void *data)
{
	Ender_Descriptor_Property *dprop = data;
	free(dprop);
}
/*----------------------------------------------------------------------------*
 *                       The constructor interface                            *
 *----------------------------------------------------------------------------*/
Ender_Descriptor * ender_descriptor_new(const char *name, Ender_Namespace *ns,
		Ender_Creator creator,
		Ender_Destructor destructor,
		Ender_Descriptor *parent, Ender_Descriptor_Type type,
		int size)
{
	Ender_Descriptor *thiz;

	/* first create the backend descriptor */
	if (!_backends[type].validate(name, ns, creator, destructor, parent,
			type))
		return NULL;

	thiz = calloc(1, sizeof(Ender_Descriptor));
	thiz->name = strdup(name);
	thiz->size = size;
	thiz->parent = parent;
	thiz->type = type;
	thiz->create = creator;
	thiz->destroy = destructor;
	thiz->ns = ns;
	thiz->properties = eina_ordered_hash_new((Eina_Free_Cb)ender_property_free);
	thiz->functions = eina_ordered_hash_new(NULL);

	return thiz;
}

void ender_descriptor_free(Ender_Descriptor *thiz)
{
	if (thiz->name)
		free(thiz->name);
	eina_ordered_hash_free(thiz->properties);
	free(thiz);
}

void ender_descriptor_init(void)
{
	int i;

	/* initialize to some sane values */
	for (i = 0; i < ENDER_PROPERTY_TYPES; i++)
	{
		_setters[i] = _ender_dummy_set;
		_getters[i] = _ender_dummy_get;
		_relative_accessors[i] = _ender_relative_dummy_set;
	}
	/* setters */
	_setters[ENDER_BOOL] = _ender_uint32_set;
	_setters[ENDER_UINT32] = _ender_uint32_set;
	_setters[ENDER_INT32] = _ender_int32_set;
	_setters[ENDER_UINT64] = _ender_int64_set;
	_setters[ENDER_INT64] = _ender_int64_set;
	_setters[ENDER_DOUBLE] = _ender_double_set;
	_setters[ENDER_ARGB] = _ender_uint32_set;
	_setters[ENDER_COLOR] = _ender_uint32_set;
	_setters[ENDER_STRING] = _ender_pointer_set;
	_setters[ENDER_OBJECT] = _ender_object_set;
	_setters[ENDER_ENDER] = _ender_object_set;
	_setters[ENDER_LIST] = _ender_pointer_set;
	_setters[ENDER_STRUCT] = _ender_pointer_set;
	_setters[ENDER_UNION] = _ender_pointer_set;
	/* getters */
	_getters[ENDER_BOOL] = _ender_uint32_get;
	_getters[ENDER_UINT32] = _ender_uint32_get;
	_getters[ENDER_INT32] = _ender_uint32_get;
	_getters[ENDER_UINT64] = _ender_int64_get;
	_getters[ENDER_INT64] = _ender_int64_get;
	_getters[ENDER_DOUBLE] = _ender_double_get;
	_getters[ENDER_ARGB] = _ender_uint32_get;
	_getters[ENDER_COLOR] = _ender_uint32_get;
	_getters[ENDER_STRING] = _ender_object_get;
	_getters[ENDER_OBJECT] = _ender_object_get;
	_getters[ENDER_ENDER] = _ender_object_get;
	_getters[ENDER_LIST] = _ender_object_get;
	_getters[ENDER_STRUCT] = _ender_pointer_get;
	_getters[ENDER_UNION] = _ender_pointer_get;
	/* relative setters */
	_relative_accessors[ENDER_BOOL] = _ender_relative_uint32_set;
	_relative_accessors[ENDER_UINT32] = _ender_relative_uint32_set;
	_relative_accessors[ENDER_INT32] = _ender_relative_int32_set;
	_relative_accessors[ENDER_UINT64] = _ender_relative_int64_set;
	_relative_accessors[ENDER_INT64] = _ender_relative_int64_set;
	_relative_accessors[ENDER_DOUBLE] = _ender_relative_double_set;
	_relative_accessors[ENDER_ARGB] = _ender_relative_uint32_set;
	_relative_accessors[ENDER_COLOR] = _ender_relative_uint32_set;
	_relative_accessors[ENDER_STRING] = _ender_relative_pointer_set;
	_relative_accessors[ENDER_OBJECT] = _ender_relative_object_set;
	_relative_accessors[ENDER_ENDER] = _ender_relative_object_set;
	_relative_accessors[ENDER_LIST] = _ender_relative_pointer_set;
	_relative_accessors[ENDER_STRUCT] = _ender_relative_pointer_set;
	_relative_accessors[ENDER_UNION] = _ender_relative_pointer_set;

	/* now the specific ones */
	ender_struct_init();
	/* now the backends */
	_backends[ENDER_TYPE_ABSTRACT] = ender_object_backend;
	_backends[ENDER_TYPE_CLASS] = ender_object_backend;
	_backends[ENDER_TYPE_STRUCT] = ender_struct_backend;
	_backends[ENDER_TYPE_UNION] = ender_union_backend;
}

void ender_descriptor_shutdown(void)
{
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Get the enders registered on the library
 * @param cb
 * @param data
 */
EAPI void ender_descriptor_list(Ender_Descriptor_List_Callback cb, void *user_data)
{
	Ender_Descriptor_List_Data data;

	if (!cb) return;

	data.ret = EINA_TRUE;
	data.real_cb = cb;
	data.real_data = user_data;

	ender_namespace_list(_descriptor_list_cb, &data);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Descriptor * ender_descriptor_find(const char *name)
{
	Ender_Descriptor_Find_Data data;

	if (!name) return NULL;

	data.ret = NULL;
	data.name = name;

	ender_namespace_list(_descriptor_find_cb, &data);
	return data.ret;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Descriptor * ender_descriptor_find_with_namespace(const char *name,
		const char *ns_name, int version)
{
	Ender_Namespace *ns;

	if (!ns_name) return ender_descriptor_find(name);
	ns = ender_namespace_find(ns_name, version);
	if (!ns) return NULL;
	return ender_namespace_descriptor_find(ns, name);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Property * ender_descriptor_property_add(Ender_Descriptor *thiz,
		const char *name, Ender_Container *ec, Ender_Getter get,
		Ender_Setter set, Ender_Add add, Ender_Remove remove,
		Ender_Clear clear, Ender_Is_Set is_set, Eina_Bool relative,
		int offset)
{
	Ender_Property *prop;

	if (!thiz) return NULL;
	prop = eina_ordered_hash_find(thiz->properties, name);
	if (prop)
	{
		WRN("Property '%s' already found on '%s'. Not adding it", name,
				thiz->name);
		return NULL;
	}
	prop = _backends[thiz->type].property_add(thiz, name, ec, get, set,
			add, remove, clear, is_set, relative, offset);
	if (!prop)
	{
		ERR("Property '%s' can not be added on '%s'", name, thiz->name);
		return NULL;
	}

	eina_ordered_hash_add(thiz->properties, name, prop);
	DBG("Property '%s' of type '%s' added to '%s'", name,
			ender_value_type_string_to(ec->type), thiz->name);

	return prop;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Function * ender_descriptor_function_add(Ender_Descriptor *thiz,
		const char *name, Ender_Accessor f,
		Ender_Marshaller marshaller, Ender_Container *ret, ...)
{
	Ender_Container *arg;
	Ender_Function *function;
	Eina_List *args = NULL;
	va_list va_args;

	va_start(va_args, ret);
	while ((arg = va_arg(va_args, Ender_Container *)))
	{
		args = eina_list_append(args, arg);
	}
	va_end(va_args);
	function = ender_descriptor_function_add_list(thiz, name, f, marshaller, ret, args);
	if (args)
	{
		eina_list_free(args);
	}
	return function;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Function * ender_descriptor_function_add_list(Ender_Descriptor *thiz,
		const char *name, Ender_Accessor f, Ender_Marshaller marshaller,
		Ender_Container *ret, Eina_List *args)
{
	Ender_Function *function;

	function = eina_ordered_hash_find(thiz->functions, name);
	if (function)
	{
		WRN("Function '%s' already foind on '%s'. Not adding it", name, thiz->name);
		return NULL;
	}

	function = ender_function_new(name, f, marshaller, ret, args);
	if (!function)
	{
		ERR("Impossible to add the function '%s'", name);
		return function;
	}

	eina_ordered_hash_add(thiz->functions, name, function);
	DBG("Function '%s' added to '%s'", name, thiz->name);
	return function;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Function * ender_descriptor_function_get(Ender_Descriptor *thiz, const char *name)
{
	if (!thiz) return NULL;
	return eina_ordered_hash_find(thiz->functions, name);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_descriptor_function_list(Ender_Descriptor *thiz,
		Ender_Function_List_Callback cb, void *data)
{
	Ender_Function *f;
	Eina_List *l;

	if (!thiz || !cb) return;

	EINA_LIST_FOREACH(thiz->functions->order, l, f)
	{
		cb(f, data);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Descriptor_Type ender_descriptor_type(Ender_Descriptor *thiz)
{
	if (!thiz) return 0;
	return thiz->type;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * ender_descriptor_name_get(Ender_Descriptor *thiz)
{
	if (!thiz) return NULL;

	return thiz->name;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Descriptor * ender_descriptor_parent(Ender_Descriptor *thiz)
{
	if (!thiz) return NULL;

	return thiz->parent;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_descriptor_property_list(Ender_Descriptor *thiz,
		Ender_Property_List_Callback cb, void *data)
{
	Ender_Property *prop;
	Eina_List *l;

	if (!thiz || !cb) return;

	EINA_LIST_FOREACH(thiz->properties->order, l, prop)
	{
		cb(prop, data);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_descriptor_property_list_recursive(Ender_Descriptor *thiz,
		Ender_Property_List_Callback cb, void *data)
{
	Ender_Property *prop;
	Ender_Descriptor *ed;
	Eina_List *l;

	ed = thiz;
	while (ed)
	{
		ender_descriptor_property_list(ed, cb, data);
		ed = ed->parent;
	}
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Property * ender_descriptor_property_get(Ender_Descriptor *thiz,
		const char *name)
{
	if (!thiz) return NULL;
	return _descriptor_property_get(thiz, name);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Property * ender_descriptor_property_get_at(Ender_Descriptor *thiz,
		int idx)
{
	Ender_Property *ret = NULL;
	Eina_List *hierarchy = NULL;
	int start = 0;

	if (!thiz) return NULL;
	/* create a list with the first level of the hierarchy first, then the
	 * second, etc, etc
	 */
	while (thiz)
	{
		hierarchy = eina_list_prepend(hierarchy, thiz);
		thiz = thiz->parent;
	}
	EINA_LIST_FREE(hierarchy, thiz)
	{
		int length;

		length = eina_ordered_hash_count(thiz->properties);
		if (idx >= length)
		{
			start = length;
			continue;
		}
		ret = eina_ordered_hash_nth_get(thiz->properties, idx - start);
	}
	return ret;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Namespace * ender_descriptor_namespace_get(Ender_Descriptor *thiz)
{
	if (!thiz) return NULL;
	return thiz->ns;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI size_t ender_descriptor_size_get(Ender_Descriptor *thiz)
{
	if (!thiz) return 0;
	if (thiz->size > 0)
		return thiz->size;
	return _backends[thiz->type].size_get(thiz);
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * ender_descriptor_element_new(Ender_Descriptor *thiz)
{
	return ender_namespace_element_new_from_descriptor(thiz->ns, thiz);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * ender_descriptor_element_new_from_data(
		Ender_Descriptor *thiz, void *data)
{
	return ender_namespace_element_new_from_descriptor_and_data(thiz->ns,
		thiz, data);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * ender_descriptor_element_unmarshal(Ender_Descriptor *thiz,
		void *data, unsigned int len)
{
	return ender_serializer_element_unmarshal(thiz, data, len);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void * ender_descriptor_native_marshal(Ender_Descriptor *thiz,
		void *data, unsigned int *len)
{
	return ender_serializer_native_marshal(thiz, data, len);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void * ender_descriptor_native_unmarshal(Ender_Descriptor *thiz,
		const void *data, unsigned int len)
{
	void *native;

	/* create a native representation */
	native = ender_descriptor_native_new(thiz);
	if (!native) return NULL;

	ender_serializer_native_unmarshal(thiz, native, data, len);
	return native;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_descriptor_native_unmarshal_inplace(Ender_Descriptor *thiz,
		void *native, const void *data, unsigned int len)
{
	ender_serializer_native_unmarshal(thiz, native, data, len);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void * ender_descriptor_native_new(Ender_Descriptor *thiz)
{
	if (!thiz) return NULL;
	return _backends[thiz->type].creator(thiz);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ender_descriptor_native_free(Ender_Descriptor *thiz, void *object)
{
	if (!thiz) return;
	_backends[thiz->type].destructor(thiz, object);
}


