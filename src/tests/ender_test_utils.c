#include "Ender.h"
#include "check.h"

START_TEST(ender_test_utils_to_upper)
{
	char *upper;

	upper = ender_utils_to_upper("my string");
	ck_assert_ptr_ne(upper, NULL);
	ck_assert_str_eq(upper, "MY STRING");
	free(upper);

	upper = ender_utils_to_upper("My String");
	ck_assert_ptr_ne(upper, NULL);
	ck_assert_str_eq(upper, "MY STRING");
	free(upper);
}
END_TEST

START_TEST(ender_test_utils_to_lower)
{
	char *lower;

	lower = ender_utils_to_lower("my string");
	ck_assert_ptr_ne(lower, NULL);
	ck_assert_str_eq(lower, "my string");
	free(lower);

	lower = ender_utils_to_lower("My String");
	ck_assert_ptr_ne(lower, NULL);
	ck_assert_str_eq(lower, "my string");
	free(lower);
}
END_TEST

START_TEST(ender_test_utils_convert)
{
	char *converted;

	converted = ender_utils_name_convert("enesimRendererGetName",
		ENDER_CASE_CAMEL, ENDER_NOTATION_ENGLISH,
		ENDER_CASE_UNDERSCORE, ENDER_NOTATION_LATIN);
	ck_assert_ptr_ne(converted, NULL);
	ck_assert_str_eq(converted, "enesim_renderer_get_name");
	free(converted);

	converted = ender_utils_name_convert("enesim_renderer_name_get",
		ENDER_CASE_UNDERSCORE, ENDER_NOTATION_LATIN,
		ENDER_CASE_CAMEL, ENDER_NOTATION_ENGLISH);
	ck_assert_ptr_ne(converted, NULL);
	ck_assert_str_eq(converted, "enesimRendererGetName");
	free(converted);
}
END_TEST

Suite * ender_test_utils_suite(void)
{
	Suite *s;
	TCase *tcase;

	s = suite_create("Utils");

	tcase = tcase_create("Lower");
	tcase_add_test(tcase, ender_test_utils_to_lower);
	suite_add_tcase(s, tcase);

	tcase = tcase_create("Upper");
	tcase_add_test(tcase, ender_test_utils_to_upper);
	suite_add_tcase(s, tcase);

	tcase = tcase_create("Convert");
	tcase_add_test(tcase, ender_test_utils_convert);
	suite_add_tcase(s, tcase);

	return s;
}

int main(void)
{
	int number_failed;
	Suite *s;
	SRunner *sr;

	s = ender_test_utils_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_VERBOSE);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
