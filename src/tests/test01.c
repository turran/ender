#include "Ender.h"

typedef struct _Dummy_Renderer {
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
} Dummy_Renderer;

static inline Dummy_Renderer * _dummy_renderer_get(Enesim_Renderer *r)
{
	Dummy_Renderer *thiz;

	thiz = enesim_renderer_data_get(r);
	return thiz;
}

static void _span(Enesim_Renderer *r, int x, int y,
		unsigned int len, uint32_t *dst)
{
	*dst = 0;
}
/*----------------------------------------------------------------------------*
 *                      The Enesim's renderer interface                       *
 *----------------------------------------------------------------------------*/
static Eina_Bool _dummy_renderer_state_setup(Enesim_Renderer *r,
		Enesim_Renderer_Sw_Fill *fill)
{
	*fill = _span;
	return EINA_TRUE;
}

static void _dummy_renderer_state_cleanup(Enesim_Renderer *r)
{
}

static void _dummy_renderer_free(Enesim_Renderer *r)
{
	Dummy_Renderer *thiz;

	thiz = _dummy_renderer_get(r);
	free(thiz);
}

static Enesim_Renderer_Descriptor _dummy_renderer_descriptor = {
	/* .sw_setup =   */ _dummy_renderer_state_setup,
	/* .sw_cleanup = */ _dummy_renderer_state_cleanup,
	/* .free =       */ _dummy_renderer_free,
	/* .boundings =  */ NULL,
	/* .flags =      */ NULL,
	/* .is_inside =  */ NULL
};

Enesim_Renderer * dummy_renderer_new(void)
{
	Enesim_Renderer *r;
	Dummy_Renderer *thiz;

	thiz = calloc(1, sizeof(Dummy_Renderer));
	if (!thiz) return NULL;
	r = enesim_renderer_new(&_dummy_renderer_descriptor, thiz);
	return r;
}

void dummy_renderer_bool_set(Enesim_Renderer *r, Eina_Bool b)
{
	Dummy_Renderer *thiz;

	thiz = _dummy_renderer_get(r);
	thiz->b = b;
}

void dummy_renderer_bool_get(Enesim_Renderer *r, Eina_Bool *b)
{
	Dummy_Renderer *thiz;

	thiz = _dummy_renderer_get(r);
	*b = thiz->b;
}

void dummy_renderer_uint32_set(Enesim_Renderer *r, uint32_t u32)
{
	Dummy_Renderer *thiz;

	thiz = _dummy_renderer_get(r);
	thiz->u32 = u32;
}

void dummy_renderer_uint32_get(Enesim_Renderer *r, uint32_t *u32)
{
	Dummy_Renderer *thiz;

	thiz = _dummy_renderer_get(r);
	*u32 = thiz->u32;
}

void dummy_renderer_double_set(Enesim_Renderer *r, double d)
{
	Dummy_Renderer *thiz;

	thiz = _dummy_renderer_get(r);
	thiz->d = d;
}

void dummy_renderer_double_get(Enesim_Renderer *r, double *d)
{
	Dummy_Renderer *thiz;

	thiz = _dummy_renderer_get(r);
	*d = thiz->d;
}

void dummy_renderer_color_set(Enesim_Renderer *r, Enesim_Color c)
{
	Dummy_Renderer *thiz;

	thiz = _dummy_renderer_get(r);
	thiz->color = c;
}

void dummy_renderer_color_get(Enesim_Renderer *r, Enesim_Color *c)
{
	Dummy_Renderer *thiz;

	thiz = _dummy_renderer_get(r);
	*c = thiz->color;
}

void dummy_renderer_matrix_set(Enesim_Renderer *r, Enesim_Matrix *m)
{
	Dummy_Renderer *thiz;

	thiz = _dummy_renderer_get(r);
}

void dummy_renderer_matrix_get(Enesim_Renderer *r, Enesim_Matrix *m)
{
	Dummy_Renderer *thiz;

	thiz = _dummy_renderer_get(r);
}

void dummy_renderer_surface_set(Enesim_Renderer *r, Enesim_Surface *s)
{
	Dummy_Renderer *thiz;

	thiz = _dummy_renderer_get(r);
	thiz->s = s;
}

void dummy_renderer_surface_get(Enesim_Renderer *r, Enesim_Surface **s)
{
	Dummy_Renderer *thiz;

	thiz = _dummy_renderer_get(r);
	*s = thiz->s;
}

static void test01_renderer_register(void)
{
	Ender_Namespace *ns;
	Ender_Descriptor *descriptor;
	Ender_Container *ec;
	Ender_Property *prop;

	/* create a new namespace */
	ns = ender_namespace_new("test01_namespace");
	/* add the test01 renderer */
	descriptor = ender_namespace_descriptor_add(
			ns,
			"test01_renderer",
			dummy_renderer_new,
			NULL,
			ENDER_CLASS);
	/* add the properties */
	ec = ender_container_new(ENDER_BOOL);
	prop = ender_descriptor_property_add(descriptor, "bool",
			ec,
			ENDER_GETTER(dummy_renderer_bool_get),
			ENDER_SETTER(dummy_renderer_bool_set),
			NULL,
			NULL,
			NULL,
			EINA_FALSE);
	ec = ender_container_new(ENDER_UINT32);
	prop = ender_descriptor_property_add(descriptor, "uint32",
			ec,
			ENDER_GETTER(dummy_renderer_uint32_get),
			ENDER_SETTER(dummy_renderer_uint32_set),
			NULL,
			NULL,
			NULL,
			EINA_FALSE);
	ec = ender_container_new(ENDER_DOUBLE);
	prop = ender_descriptor_property_add(descriptor, "double",
			ec,
			ENDER_GETTER(dummy_renderer_double_get),
			ENDER_SETTER(dummy_renderer_double_set),
			NULL,
			NULL,
			NULL,
			EINA_FALSE);
	ec = ender_container_new(ENDER_COLOR);
	prop = ender_descriptor_property_add(descriptor, "color",
			ec,
			ENDER_GETTER(dummy_renderer_color_get),
			ENDER_SETTER(dummy_renderer_color_set),
			NULL,
			NULL,
			NULL,
			EINA_FALSE);
	ec = ender_container_new(ENDER_MATRIX);
	prop = ender_descriptor_property_add(descriptor, "matrix",
			ec,
			ENDER_GETTER(dummy_renderer_matrix_get),
			ENDER_SETTER(dummy_renderer_matrix_set),
			NULL,
			NULL,
			NULL,
			EINA_FALSE);
	ec = ender_container_new(ENDER_SURFACE);
	prop = ender_descriptor_property_add(descriptor, "surface",
			ec,
			ENDER_GETTER(dummy_renderer_surface_get),
			ENDER_SETTER(dummy_renderer_surface_set),
			NULL,
			NULL,
			NULL,
			EINA_FALSE);
}

/* test that the namespace is created */
Eina_Bool test01_namespace(void)
{
	Ender_Namespace *ns;

	ns = ender_namespace_find("test01_namespace");
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

	desc = ender_descriptor_find("test01_renderer");
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

	e = ender_element_new("test01_renderer");
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

	test01_renderer_register();
	if (!test01_namespace()) return -1;
	if (!test01_properties()) return -1;
	if (!test01_setters_getters()) return -1;
	/* test that the renderer is part of that namespace */
	/* create the ender */
	/* add dynamic properties */
	/* set the different properties */
	/* get the different properties */

	return 0;
}
