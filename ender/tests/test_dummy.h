#ifndef _TEST_DUMMY_H
#define _TEST_DUMMY_H

#include "Ender.h"

typedef struct _Dummy_Object Dummy_Object;

struct _Dummy_Object {
	/* the properties */
	/* basic types */
	Eina_Bool b;
	uint32_t u32;
	int32_t i32;
	double d;
	uint32_t color;
	char *string;
	/* descriptor based */
	Ender_Element *ender;
	void *object;
	/* the list */
	Eina_List *childs;
};

Dummy_Object * dummy_object_new(void);
void dummy_object_free(Dummy_Object *thiz);
void dummy_object_bool_set(Dummy_Object *thiz, Eina_Bool b);
void dummy_object_bool_get(Dummy_Object *thiz, Eina_Bool *b);
void dummy_object_uint32_set(Dummy_Object *thiz, uint32_t u32);
void dummy_object_uint32_get(Dummy_Object *thiz, uint32_t *u32);
void dummy_object_int32_set(Dummy_Object *thiz, int32_t i32);
void dummy_object_int32_get(Dummy_Object *thiz, int32_t *i32);
void dummy_object_double_set(Dummy_Object *thiz, double d);
void dummy_object_double_get(Dummy_Object *thiz, double *d);
void dummy_object_color_set(Dummy_Object *thiz, uint32_t c);
void dummy_object_color_get(Dummy_Object *thiz, uint32_t *c);
void dummy_object_ender_set(Dummy_Object *thiz, Ender_Element *e);
void dummy_object_ender_get(Dummy_Object *thiz, Ender_Element **e);
void dummy_object_object_set(Dummy_Object *thiz, void *o);
void dummy_object_object_get(Dummy_Object *thiz, void **o);

/* functions */
void dummy_object_function_01(Dummy_Object *thiz);
void dummy_init(void);
void dummy_shutdown(void);

#endif
