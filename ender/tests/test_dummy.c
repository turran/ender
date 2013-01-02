#include "test_dummy.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Namespace * dummy_namespace_register(void)
{
	Ender_Namespace *ns = NULL;
	ns = ender_namespace_new("Dummy", 0);
	return ns;
}

static void dummy_functions_register(Ender_Descriptor *descriptor)
{
	Ender_Function *function;
	/* add the functions */
	ender_descriptor_function_add(descriptor, "function_01",
			ENDER_FUNCTION(dummy_object_function_01),
			NULL, NULL, NULL);
}

static void dummy_struct_register(Ender_Namespace *ns)
{
	Ender_Descriptor *descriptor;
	Ender_Container *ec;
	Ender_Property *prop;

	/* add the struct */
	descriptor = ender_namespace_descriptor_add(
			ns,
			"Dummy_Struct",
			NULL,
			NULL,
			NULL,
			ENDER_TYPE_STRUCT, -1);

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
	dummy_functions_register(descriptor);
}

static void dummy_object_register(Ender_Namespace *ns)
{
	Ender_Descriptor *descriptor;
	Ender_Container *ec;
	Ender_Property *prop;

	descriptor = ender_namespace_descriptor_add(
			ns,
			"Dummy_Object",
			ENDER_CREATOR(dummy_object_new),
			ENDER_DESTRUCTOR(dummy_object_free),
			NULL,
			ENDER_TYPE_CLASS, -1);

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
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *                              The dummy object                              *
 *----------------------------------------------------------------------------*/
void dummy_object_free(Dummy_Object *thiz)
{
	printf("D: Destroying the object\n");
	free(thiz);
}

Dummy_Object * dummy_object_new(void)
{
	Dummy_Object *thiz;

	thiz = calloc(1, sizeof(Dummy_Object));
	printf("D: Creating the object\n");
	return thiz;
}

void dummy_object_bool_set(Dummy_Object *thiz, Eina_Bool b)
{
	printf("D: Setting bool with %d\n", b);
	thiz->b = b;
}

void dummy_object_bool_get(Dummy_Object *thiz, Eina_Bool *b)
{
	*b = thiz->b;
	printf("D: Getting bool with %p %d\n", b, *b);
}

void dummy_object_uint32_set(Dummy_Object *thiz, uint32_t u32)
{
	printf("D: Setting uint32 with %u\n", u32);
	thiz->u32 = u32;
}

void dummy_object_uint32_get(Dummy_Object *thiz, uint32_t *u32)
{
	*u32 = thiz->u32;
	printf("D: Getting uint32 with %p %u\n", u32, *u32);
}

void dummy_object_double_set(Dummy_Object *thiz, double d)
{
	printf("D: Setting double with %g\n", d);
	thiz->d = d;
}

void dummy_object_double_get(Dummy_Object *thiz, double *d)
{
	*d = thiz->d;
	printf("D: Getting double with %p %g\n", d, *d);
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

void dummy_init(void)
{
	Ender_Namespace *ns;

	ender_init();
	/* register the namespace */
	ns = dummy_namespace_register();
	/* register the object */
	dummy_object_register(ns);
	/* register the struct */
	dummy_struct_register(ns);
}

void dummy_shutdown(void)
{
	ender_shutdown();
}
