#include "test_dummy.h"

/*----------------------------------------------------------------------------*
 *                             Namespace helpers                              *
 *----------------------------------------------------------------------------*/
static Ender_Namespace * test01_namespace_get(void)
{
	Ender_Namespace *ns;

	ns = ender_namespace_find("Dummy", 0);
	return ns;
}
/*----------------------------------------------------------------------------*
 *                            Properties helpers                              *
 *----------------------------------------------------------------------------*/
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
static Eina_Bool test01_functions(Ender_Element *e)
{
	ender_element_call(e, "function_01", NULL, NULL);
	printf("Functions called\n");
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object helpers                                *
 *----------------------------------------------------------------------------*/
static Ender_Descriptor * test01_object_get(void)
{
	Ender_Namespace *ns;
	Ender_Descriptor *d;

	ns = test01_namespace_get();
	if (!ns) return NULL;
	d = ender_namespace_descriptor_find(ns, "Dummy_Object");
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
static Ender_Descriptor * test01_struct_get(void)
{
	Ender_Namespace *ns;
	Ender_Descriptor *d;

	ns = test01_namespace_get();
	if (!ns) return NULL;
	d = ender_namespace_descriptor_find(ns, "Dummy_Struct");
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
static Eina_Bool test01_namespace(void)
{
	Ender_Namespace *ns;

	ns = test01_namespace_get();
	if (!ns)
	{
		printf("namespace not found\n");
		return EINA_FALSE;
	}
	printf("Namespace found\n");
	return EINA_TRUE;
}

/* test that the namespace is created */
static Eina_Bool test01_object_properties(void)
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

static Eina_Bool test01_struct_properties(void)
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

static Eina_Bool test01_setters_getters(Ender_Element *e)
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

static Eina_Bool test01_struct_setters_getters(void)
{
	Ender_Element *e;

	e = test01_struct_new();
	return test01_setters_getters(e);
}

static Eina_Bool test01_object_setters_getters(void)
{
	Ender_Element *e;

	e = test01_object_new();
	return test01_setters_getters(e);
}

static Eina_Bool test01_object_functions(void)
{
	Ender_Element *e;

	e = test01_object_new();
	return test01_functions(e);
}

int main(int argc, char **argv)
{
	dummy_init();
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
	dummy_shutdown();
	return 0;
}
