#define ENESIM_EXTENSION
#include "Escen.h"

static Ender_Property *ESCEN_TEST_DATATYPE_I;
static Ender_Property *ESCEN_TEST_DATATYPE_D;
static Ender_Property *ESCEN_TEST_DATATYPE_LS;
static Ender_Property *ESCEN_TEST_DATATYPE_LD;
static Ender_Property *ESCEN_TEST_DATATYPE_S;

typedef struct _Datatype
{
	/* properties */
	/* basic data types */
	int i;
	double d; 
	/* compound data types */
	Eina_List *ld; /* list of doubles */
	Eina_List *ls; /* list of structs */
	/* compound with compound data types */
} Datatype;

typedef struct _Datatype_Struct
{
	int i;
	double d;
} Datatype_Struct;

static Ender_Namespace * escen_test_namespace_get(void)
{
	static Ender_Namespace *namespace = NULL;

	if (!namespace)
	{
		namespace = ender_namespace_new("escen_test", 0);
	}
	return namespace;
}

static Datatype * _escen_test_datatype_get(Enesim_Renderer *r)
{
	Datatype *thiz;

	thiz = enesim_renderer_data_get(r);
	return thiz;
}
/*----------------------------------------------------------------------------*
 *                      The Ender's property functions                        *
 *----------------------------------------------------------------------------*/
static void _escen_test_datatype_i_set(Enesim_Renderer *r, int i)
{
	printf("setting i %d\n", i);
}

static void _escen_test_datatype_d_set(Enesim_Renderer *r, double d)
{
	printf("setting d %g\n", d);
}

static void _escen_test_datatype_s_set(Enesim_Renderer *r, Datatype_Struct *s)
{
	printf("setting s %p %g %d\n", s, s->d, s->i);
}

static void _escen_test_datatype_ld_set(Enesim_Renderer *r, Eina_List *ld)
{
	Eina_List *ll;
	double *d;

	printf("setting ld\n");
	EINA_LIST_FOREACH(ld, ll, d)
	{
		printf(" => d %g\n", *d);
	}
}

static void _escen_test_datatype_ls_set(Enesim_Renderer *r, Eina_List *ld)
{
	Eina_List *ll;
	Datatype_Struct *s;

	printf("setting ls\n");
	EINA_LIST_FOREACH(ld, ll, s)
	{
		printf("=> ");
		_escen_test_datatype_s_set(r, s);
	}
}

/*----------------------------------------------------------------------------*
 *                      The Enesim's renderer interface                       *
 *----------------------------------------------------------------------------*/
static const char * _escen_test_datatype_name(Enesim_Renderer *r)
{
	return "escen_test_datatype";
}

static void _escen_test_datatype_free(Enesim_Renderer *r)
{
	Datatype *thiz;

	thiz = _escen_test_datatype_get(r);
	free(thiz);
}

static Enesim_Renderer_Descriptor _descriptor = {
	/* .version =               */ ENESIM_RENDERER_API,
	/* .name =                  */ _escen_test_datatype_name,
	/* .free =                  */ _escen_test_datatype_free,
	/* .boundings =             */ NULL,
	/* .destination_boundings = */ NULL,
	/* .flags =                 */ NULL,
	/* .is_inside =             */ NULL,
	/* .damage =                */ NULL,
	/* .has_changed =           */ NULL,
	/* .sw_setup =              */ NULL,
	/* .sw_cleanup =            */ NULL,
};

/* the renderer interface */
static Enesim_Renderer * _escen_test_datatype_new(void)
{
	Datatype *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Datatype));
	r = enesim_renderer_new(&_descriptor, thiz);

	printf("renderer to test datatypes created successfully\n");

	return r;
}

/* only sets, not gets */
#define _escen_test_datatype_i_get NULL
#define _escen_test_datatype_d_get NULL
#define _escen_test_datatype_ls_add NULL
#define _escen_test_datatype_ls_remove NULL
#define _escen_test_datatype_ls_clear NULL
#define _escen_test_datatype_ls_get NULL
#define _escen_test_datatype_ld_add NULL
#define _escen_test_datatype_ld_remove NULL
#define _escen_test_datatype_ld_clear NULL
#define _escen_test_datatype_ld_get NULL
#define _escen_test_datatype_s_get NULL
#define _escen_test_datatype_delete NULL
#include "escen_datatype_ender.c"

int main(int argc, char **argv)
{
	Escen *e;
	Escen_Ender *ee;
	Escen_Instance *eei;
	Ender_Element *ender;
	Enesim_Renderer *r;

	escen_init();
	escen_test_datatype_init();
	ender_loader_load_all();
	e = escen_parser_load("escen_datatype.escen");
	if (!e)
	{
		printf("Impossible to parse the file\n");
		return 1;
	}
	ee = escen_ender_nth_get(e, 0);
	if (!ee)
	{
		printf("No escen ender available\n");
		return 1;
	}
	eei = escen_instance_new(ee, EINA_TRUE);
	ender = escen_instance_ender_get(eei);
	r = ender_element_object_get(ender);
	if (!r) return 1;

	return 0;
}
