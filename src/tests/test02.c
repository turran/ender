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

static Eina_Bool test_bool(void)
{
	Ender_Value *v;
	Eina_Bool b;

	v = ender_value_basic_new(ENDER_BOOL);
	ender_value_bool_set(v, EINA_TRUE);
	if (ender_value_bool_get(v) != EINA_TRUE)
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool test_uint32(void)
{
	Ender_Value *v;
	uint32_t u32;

	v = ender_value_basic_new(ENDER_UINT32);
	ender_value_uint32_set(v, 123456);
	if (ender_value_uint32_get(v) != 123456)
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

int main(int argc, char **argv)
{
	/* just test the value types */
	if (!test_bool())
		return -1;
	if (!test_uint32())
		return -1;
	/* new values, set and get */
	return 0;
}
