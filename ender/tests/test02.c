/* This test should check that the element/value marshallers works correctly */
#include "test_dummy.h"

#define V_I32 -123456
#define V_U32 123456
#define V_B EINA_TRUE
#define V_D 3.14159

/*
ENDER_BOOL,
ENDER_UINT32,
ENDER_INT32,
ENDER_DOUBLE,
ENDER_COLOR,
ENDER_ARGB,
ENDER_STRING,
ENDER_OBJECT,
ENDER_ENDER,
ENDER_POINTER,
ENDER_VALUE,
ENDER_LIST,
ENDER_STRUCT,
ENDER_PROPERTY_TYPES,
*/

static Eina_Bool serialize = EINA_TRUE;

static Ender_Value * test_serialize(Ender_Value *v)
{
	Ender_Container *c;
	Ender_Value *other;
	void *data;
	unsigned int len;

	data = ender_value_marshal(v, &len);
	if (!data) return NULL;

	c = ender_value_container_get(v);
	other = ender_container_value_unmarshal(c, data, len);
	if (!other) return NULL;

	ender_value_unref(v);
	return other;
}

static Eina_Bool test_bool(void)
{
	Ender_Value *v;
	Eina_Bool b;

	v = ender_value_basic_new(ENDER_BOOL);
	ender_value_bool_set(v, EINA_TRUE);
	if (serialize)
		v = test_serialize(v);

	if (ender_value_bool_get(v) != EINA_TRUE)
	{
		printf("value is %d\n", ender_value_bool_get(v));
		return EINA_FALSE;
	}
	ender_value_unref(v);
	printf("bool ok\n");

	return EINA_TRUE;
}

static Eina_Bool test_uint32(void)
{
	Ender_Value *v;
	uint32_t vv;

	v = ender_value_basic_new(ENDER_UINT32);
	ender_value_uint32_set(v, 123456);
	if (serialize)
		v = test_serialize(v);
	vv = ender_value_uint32_get(v);
	if (vv != 123456)
	{
		printf("value is %d\n", vv);
		return EINA_FALSE;
	}
	ender_value_unref(v);
	printf("uint32 ok\n");

	return EINA_TRUE;
}

static Eina_Bool test_int32(void)
{
	Ender_Value *v;
	int32_t vv;

	v = ender_value_basic_new(ENDER_INT32);
	ender_value_int32_set(v, -123456);
	if (serialize)
		v = test_serialize(v);

	vv = ender_value_int32_get(v);
	if (vv != -123456)
	{
		printf("value is %d\n", vv);
		return EINA_FALSE;
	}
	ender_value_unref(v);
	printf("int32 ok\n");

	return EINA_TRUE;
}

static Eina_Bool test_string(void)
{
	Ender_Value *v;
	const char *vv;

	v = ender_value_basic_new(ENDER_STRING);
	ender_value_string_set(v, "Hello");
	if (serialize)
		v = test_serialize(v);

	vv = ender_value_string_get(v);
	if (strcmp(vv, "Hello"))
	{
		printf("value is %s\n", vv);
		return EINA_FALSE;
	}
	ender_value_unref(v);
	printf("string ok\n");

	return EINA_TRUE;
}

int main(int argc, char **argv)
{
	Eina_Bool ret;
	Ender_Element *e;
	Ender_Namespace *ns;
	Ender_Descriptor *descriptor;
	void *data;
	unsigned int len;

	dummy_init();
	/* first get the namespace */
	ns = ender_namespace_find("Dummy", 0);
	if (!ns)
	{
		printf("T: Impossible to find the namespace");
		return -1;
	}
	/* create the element */
	e = ender_namespace_element_new(ns, "Dummy_Object");
	if (!e)
	{
		printf("T: Impossible to create the dummy object");
		return -1;
	}
	/* set the properties */
	ender_element_value_set(e, "b", V_B, NULL);
	ender_element_value_set(e, "u32", V_U32, NULL);
	ender_element_value_set(e, "i32", V_I32, NULL);
	ender_element_value_set(e, "d", V_D, NULL);

	/* marshal the element itself */
	data = ender_element_marshal(e, &len);
	printf("T: Marshalled at %p with %d size\n", data, len);

	/* unmarshal the element */
	descriptor = ender_element_descriptor_get(e);
	e = ender_descriptor_element_unmarshal(descriptor, data, len);
	printf("T: Unmarshalled\n");
	/* get the properties and marshal them too */

	/* just test the value types */
	ret = test_bool();
	ret &= test_uint32();
	ret &= test_int32();
	ret &= test_string();

	/* new values, set and get */
	dummy_shutdown();
	return !ret;
}
