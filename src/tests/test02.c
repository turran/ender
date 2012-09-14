#include "Ender.h"

/*
ENDER_BOOL,
ENDER_UINT32,
ENDER_INT32,
ENDER_DOUBLE,
ENDER_COLOR,
ENDER_ARGB,
ENDER_STRING,
ENDER_MATRIX,
ENDER_RENDERER,
ENDER_SURFACE,
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

	ender_init(&argc, &argv);

	/* just test the value types */
	ret = test_bool();
	ret &= test_uint32();
	ret &= test_int32();
	ret &= test_string();
	/* new values, set and get */
	ender_shutdown();
	return !ret;
}
