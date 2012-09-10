#include "Ender.h"

typedef struct _Dummy_Object {
	/* the properties */
	Eina_Bool b;
	uint32_t u32;
	double d;
	Enesim_Color color;
	char *string;
	Enesim_Surface *s;
	Ender_Element *ender;
	Enesim_Matrix matrix;
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
	return thiz;
}

void dummy_object_bool_set(Dummy_Object *thiz, Eina_Bool b)
{
	thiz->b = b;
}

void dummy_object_bool_get(Dummy_Object *thiz, Eina_Bool *b)
{
	*b = thiz->b;
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

void dummy_object_color_set(Dummy_Object *thiz, Enesim_Color c)
{
	thiz->color = c;
}

void dummy_object_color_get(Dummy_Object *thiz, Enesim_Color *c)
{
	*c = thiz->color;
}

void dummy_object_matrix_set(Dummy_Object *thiz, Enesim_Matrix *m)
{
}

void dummy_object_matrix_get(Dummy_Object *thiz, Enesim_Matrix *m)
{
}

void dummy_object_surface_set(Dummy_Object *thiz, Enesim_Surface *s)
{
	thiz->s = s;
}

void dummy_object_surface_get(Dummy_Object *thiz, Enesim_Surface **s)
{
	*s = thiz->s;
}

static void test01_object_register(void)
{
	Ender_Namespace *ns;
	Ender_Descriptor *descriptor;
	Ender_Container *ec;
	Ender_Property *prop;

	/* create a new namespace */
	ns = ender_namespace_new("test01_namespace", 0);
	/* add the test01 object */
	descriptor = ender_namespace_descriptor_add(
			ns,
			"test01_object",
			ENDER_CREATOR(dummy_object_new),
			ENDER_DESTRUCTOR(dummy_object_free),
			NULL,
			ENDER_CLASS);
	/* add the properties */
	ec = ender_container_new(ENDER_BOOL);
	prop = ender_descriptor_property_add(descriptor, "bool",
			ec,
			ENDER_GETTER(dummy_object_bool_get),
			ENDER_SETTER(dummy_object_bool_set),
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE);
	ec = ender_container_new(ENDER_UINT32);
	prop = ender_descriptor_property_add(descriptor, "uint32",
			ec,
			ENDER_GETTER(dummy_object_uint32_get),
			ENDER_SETTER(dummy_object_uint32_set),
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE);
	ec = ender_container_new(ENDER_DOUBLE);
	prop = ender_descriptor_property_add(descriptor, "double",
			ec,
			ENDER_GETTER(dummy_object_double_get),
			ENDER_SETTER(dummy_object_double_set),
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE);
	ec = ender_container_new(ENDER_COLOR);
	prop = ender_descriptor_property_add(descriptor, "color",
			ec,
			ENDER_GETTER(dummy_object_color_get),
			ENDER_SETTER(dummy_object_color_set),
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE);
	ec = ender_container_new(ENDER_MATRIX);
	prop = ender_descriptor_property_add(descriptor, "matrix",
			ec,
			ENDER_GETTER(dummy_object_matrix_get),
			ENDER_SETTER(dummy_object_matrix_set),
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE);
	ec = ender_container_new(ENDER_SURFACE);
	prop = ender_descriptor_property_add(descriptor, "surface",
			ec,
			ENDER_GETTER(dummy_object_surface_get),
			ENDER_SETTER(dummy_object_surface_set),
			NULL,
			NULL,
			NULL,
			NULL,
			EINA_FALSE);
}

/* test that the namespace is created */
Eina_Bool test01_namespace(void)
{
	Ender_Namespace *ns;

	ns = ender_namespace_find("test01_namespace", 0);
	if (!ns)
	{
		printf("namespace not found\n");
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

Eina_Bool test01_properties(void)
{
	Ender_Property *prop;
	Ender_Descriptor *desc;

	desc = ender_descriptor_find("test01_object");
	if (!desc)
	{
		printf("descriptor not found\n");
		return EINA_FALSE;
	}
	prop = ender_descriptor_property_get(desc, "bool");
	if (!prop)
	{
		printf("no bool property\n");
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

Eina_Bool test01_setters_getters(void)
{
	Ender_Element *e;
	Eina_Bool b;
	uint32_t u32;
	double d;
	Enesim_Color color;
	char *string;
	Enesim_Surface *s;
	Ender_Element *ender;
	Enesim_Matrix matrix;

	e = ender_element_new("test01_object");
	if (!e)
	{
		printf("impossible to instantiate an element\n");
		return EINA_FALSE;
	}
	ender_element_value_set(e, "bool", EINA_TRUE);
	ender_element_value_get(e, "bool", &b);
	if (b != EINA_TRUE)
	{
		printf("failed setting/getting the bool\n");
		return EINA_FALSE;
	}
	ender_element_value_set(e, "uint32", 123456);
	ender_element_value_get(e, "uint32", &u32);
	if (u32 != 123456)
	{
		printf("failed setting/getting the u32\n");
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

int main(int argc, char **argv)
{
	ender_init(&argc, &argv);

	test01_object_register();
	if (!test01_namespace()) return -1;
	if (!test01_properties()) return -1;
	if (!test01_setters_getters()) return -1;
	/* test that the object is part of that namespace */
	/* create the ender */
	/* add dynamic properties */
	/* set the different properties */
	/* get the different properties */

	ender_shutdown();
	return 0;
}
