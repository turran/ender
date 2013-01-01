#include "Ender.h"

typedef struct _Dummy_Object {
	/* the properties */
	Eina_Bool b;
	uint32_t u32;
	double d;
	uint32_t color;
	char *string;
	Ender_Element *ender;
	/* the list */
	Eina_List *childs;
} Dummy_Object;
/*----------------------------------------------------------------------------*
 *                              The dummy object                              *
 *----------------------------------------------------------------------------*/
void dummy_object_free(Dummy_Object *thiz)
{
	free(thiz);
}

Dummy_Object * dummy_object_new(void)
{
	Dummy_Object *thiz;

	thiz = calloc(1, sizeof(Dummy_Object));
	printf("dummy object created\n");
	return thiz;
}

void dummy_object_bool_set(Dummy_Object *thiz, Eina_Bool b)
{
	printf("bool_set called with %d\n", b);
	thiz->b = b;
}

void dummy_object_bool_get(Dummy_Object *thiz, Eina_Bool *b)
{
	*b = thiz->b;
	printf("bool_get called with %p %d\n", b, *b);
}

void dummy_object_uint32_set(Dummy_Object *thiz, uint32_t u32)
{
	thiz->u32 = u32;
}

void dummy_object_uint32_get(Dummy_Object *thiz, uint32_t *u32)
{
	*u32 = thiz->u32;
}

void dummy_object_double_set(Dummy_Object *thiz, double d)
{
	thiz->d = d;
}

void dummy_object_double_get(Dummy_Object *thiz, double *d)
{
	*d = thiz->d;
}

void dummy_object_color_set(Dummy_Object *thiz, uint32_t c)
{
	thiz->color = c;
}

void dummy_object_color_get(Dummy_Object *thiz, uint32_t *c)
{
	*c = thiz->color;
}

void dummy_object_ender_set(Dummy_Object *thiz, Ender_Element *e)
{
}

void dummy_object_ender_get(Dummy_Object *thiz, Ender_Element **e)
{
}


/* functions */
void dummy_object_function_01(Dummy_Object *thiz)
{
	printf("function 01 called\n");
}

/*----------------------------------------------------------------------------*
 *                             Namespace helpers                              *
 *----------------------------------------------------------------------------*/
static void test01_namespace_register(void)
{
	Ender_Namespace *ns;

	/* create a new namespace */
	ns = ender_namespace_new("test01_namespace", 0);
}

static Ender_Namespace * test01_namespace_get(void)
{
	Ender_Namespace *ns;

	ns = ender_namespace_find("test01_namespace", 0);
	return ns;
}
/*----------------------------------------------------------------------------*
 *                            Properties helpers                              *
 *----------------------------------------------------------------------------*/
static void test01_object_properties_register(Ender_Descriptor *descriptor)
{
	Ender_Container *ec;
	Ender_Property *prop;

	/* add the properties */
	ec = ender_container_new(ENDER_BOOL);
	prop = ender_descriptor_property_add(descriptor, "b",
			ec,
			ENDER_GETTER(dummy_object_bool_get),
			ENDER_SETTER(dummy_object_bool_set),
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE, -1);
	ec = ender_container_new(ENDER_UINT32);
	prop = ender_descriptor_property_add(descriptor, "u32",
			ec,
			ENDER_GETTER(dummy_object_uint32_get),
			ENDER_SETTER(dummy_object_uint32_set),
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE, -1);
	ec = ender_container_new(ENDER_DOUBLE);
	prop = ender_descriptor_property_add(descriptor, "d",
			ec,
			ENDER_GETTER(dummy_object_double_get),
			ENDER_SETTER(dummy_object_double_set),
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE, -1);
	ec = ender_container_new(ENDER_COLOR);
	prop = ender_descriptor_property_add(descriptor, "color",
			ec,
			ENDER_GETTER(dummy_object_color_get),
			ENDER_SETTER(dummy_object_color_set),
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE, -1);
	ec = ender_container_new(ENDER_ENDER);
	prop = ender_descriptor_property_add(descriptor, "ender",
			ec,
			ENDER_GETTER(dummy_object_ender_get),
			ENDER_SETTER(dummy_object_ender_set),
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE, -1);
}

static void test01_struct_properties_register(Ender_Descriptor *descriptor)
{
	Ender_Container *ec;
	Ender_Property *prop;

	/* add the properties */
	ec = ender_container_new(ENDER_BOOL);
	prop = ender_descriptor_property_add(descriptor, "b",
			ec,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE, -1);
	ec = ender_container_new(ENDER_UINT32);
	prop = ender_descriptor_property_add(descriptor, "u32",
			ec,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE, -1);
	ec = ender_container_new(ENDER_DOUBLE);
	prop = ender_descriptor_property_add(descriptor, "d",
			ec,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE, -1);
	ec = ender_container_new(ENDER_COLOR);
	prop = ender_descriptor_property_add(descriptor, "color",
			ec,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE, -1);
	ec = ender_container_new(ENDER_STRING);
	prop = ender_descriptor_property_add(descriptor, "s",
			ec,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE, -1);
	ec = ender_container_new(ENDER_ENDER);
	prop = ender_descriptor_property_add(descriptor, "ender",
			ec,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE, -1);
}

Eina_Bool test01_properties(Ender_Descriptor *desc)
{
	Ender_Property *prop;
	prop = ender_descriptor_property_get(desc, "b");
	if (!prop)
	{
		printf("no bool property\n");
		return EINA_FALSE;
	}
	printf("Properties found\n");
	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                           Function helpers                                 *
 *----------------------------------------------------------------------------*/
static void test01_functions_register(Ender_Descriptor *descriptor)
{
	Ender_Function *function;
	/* add the functions */
	ender_descriptor_function_add(descriptor, "function_01",
			ENDER_FUNCTION(dummy_object_function_01),
			NULL, NULL, NULL);
}

Eina_Bool test01_functions(Ender_Element *e)
{
	ender_element_call(e, "function_01", NULL, NULL);
	printf("Functions called\n");
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object helpers                                *
 *----------------------------------------------------------------------------*/
static void test01_object_register(void)
{
	Ender_Namespace *ns;
	Ender_Descriptor *descriptor;

	/* get the namespace */
	ns = test01_namespace_get();
	if (!ns) return;
	/* add the object */
	descriptor = ender_namespace_descriptor_add(
			ns,
			"test01_object",
			ENDER_CREATOR(dummy_object_new),
			ENDER_DESTRUCTOR(dummy_object_free),
			NULL,
			ENDER_TYPE_CLASS, -1);
	test01_object_properties_register(descriptor);
	test01_functions_register(descriptor);
}

static Ender_Descriptor * test01_object_get(void)
{
	Ender_Namespace *ns;
	Ender_Descriptor *d;

	ns = test01_namespace_get();
	if (!ns) return NULL;
	d = ender_namespace_descriptor_find(ns, "test01_object");
	return d;
}

static Ender_Element * test01_object_new(void)
{
	Ender_Descriptor *d;
	Ender_Element *e;

	d = test01_object_get();
	if (!d) return NULL;
	e = ender_descriptor_element_new(d);
	return e;
}
/*----------------------------------------------------------------------------*
 *                              Struct helpers                                *
 *----------------------------------------------------------------------------*/
static void test01_struct_register(void)
{
	Ender_Namespace *ns;
	Ender_Descriptor *descriptor;

	/* get the namespace */
	ns = test01_namespace_get();
	if (!ns) return;
	/* add the struct */
	descriptor = ender_namespace_descriptor_add(
			ns,
			"test01_struct",
			NULL,
			NULL,
			NULL,
			ENDER_TYPE_STRUCT, -1);
	test01_struct_properties_register(descriptor);
	test01_functions_register(descriptor);
}

static Ender_Descriptor * test01_struct_get(void)
{
	Ender_Namespace *ns;
	Ender_Descriptor *d;

	ns = test01_namespace_get();
	if (!ns) return NULL;
	d = ender_namespace_descriptor_find(ns, "test01_struct");
	return d;
}

static Ender_Element * test01_struct_new(void)
{
	Ender_Descriptor *d;
	Ender_Element *e;

	d = test01_struct_get();
	if (!d) return NULL;
	e = ender_descriptor_element_new(d);
	if (e)
	{
		printf("dummy struct created\n");
	}
	return e;
}
/*----------------------------------------------------------------------------*
 *                                  Tests                                     *
 *----------------------------------------------------------------------------*/
/* test that the namespace is created */
Eina_Bool test01_namespace(void)
{
	Ender_Namespace *ns;

	ns = test01_namespace_get();
	if (!ns)
	{
		printf("namespace not found\n");
		return EINA_FALSE;
	}
	printf("Namespace 'test01' found\n");
	return EINA_TRUE;
}

Eina_Bool test01_object_properties(void)
{
	Ender_Descriptor *desc;

	desc = test01_object_get();
	if (!desc)
	{
		printf("descriptor not found\n");
		return EINA_FALSE;
	}
	return test01_properties(desc);
}

Eina_Bool test01_struct_properties(void)
{
	Ender_Descriptor *desc;

	desc = test01_struct_get();
	if (!desc)
	{
		printf("descriptor not found\n");
		return EINA_FALSE;
	}
	return test01_properties(desc);
}

Eina_Bool test01_setters_getters(Ender_Element *e)
{
	Eina_Bool b;
	uint32_t u32;
	double d;
	uint32_t color;
	char *string;
	Ender_Element *ender;

	/* first set the different properties */
	ender_element_value_set(e, "b", EINA_TRUE, NULL);
	ender_element_value_set(e, "u32", 123456, NULL);
	/* now get them */
	ender_element_value_get(e, "b", &b, NULL);
	if (b != EINA_TRUE)
	{
		printf("failed setting/getting the bool\n");
		return EINA_FALSE;
	}
	ender_element_value_get(e, "u32", &u32, NULL);
	if (u32 != 123456)
	{
		printf("failed setting/getting the u32\n");
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

Eina_Bool test01_struct_setters_getters(void)
{
	Ender_Element *e;

	e = test01_struct_new();
	return test01_setters_getters(e);
}

Eina_Bool test01_object_setters_getters(void)
{
	Ender_Element *e;

	e = test01_object_new();
	return test01_setters_getters(e);
}

Eina_Bool test01_object_functions(void)
{
	Ender_Element *e;

	e = test01_object_new();
	return test01_functions(e);
}

int main(int argc, char **argv)
{
	ender_init();

	test01_namespace_register();
	test01_object_register();
	test01_struct_register();

	if (!test01_namespace()) return -1;
	if (!test01_struct_properties()) return -1;
	if (!test01_struct_setters_getters()) return -1;
	if (!test01_object_properties()) return -1;
	if (!test01_object_setters_getters()) return -1;
	if (!test01_object_functions()) return -1;
	/* test that the object is part of that namespace */
	/* create the ender */
	/* add dynamic properties */
	/* set the different properties */
	/* get the different properties */

	ender_shutdown();
	return 0;
}
