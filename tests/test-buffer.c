// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-buffer.h"
#include "tests-suit.h"

//------------------------------------------------------------------------------

#define NOIA_ASSERT_BUFFERS_EQUAL(ACTUAL,EXPECTED) { \
    size_t size = ACTUAL.stride * ACTUAL.height; \
    NOIA_ASSERT(((ACTUAL.width == EXPECTED.width) && \
                 (ACTUAL.height == EXPECTED.height) && \
                 (ACTUAL.stride == EXPECTED.stride)), \
                "Buffer size is {%d, %d, %d} should be {%d, %d, %d}", \
                ACTUAL.width, ACTUAL.height, ACTUAL.stride, \
                EXPECTED.width, EXPECTED.height, EXPECTED.stride); \
    NOIA_ASSERT(ACTUAL.format == EXPECTED.format, \
                "Buffer format is '%d' should be '%d'", \
                ACTUAL.format, EXPECTED.format); \
    NOIA_ASSERT(memcmp(ACTUAL.data, EXPECTED.data, size) == 0, \
                "Comparing buffers data"); }


//------------------------------------------------------------------------------

NoiaTestResult should_transform_identically(void)
{
    uint8_t original_data[] = {
            111,112,113,114, 121,122,123,124, 131,132,133,134, 141,142,143,144,
            211,212,213,214, 221,222,223,224, 231,232,233,234, 241,242,243,244,
        };

    uint8_t expected_data[] = {
            111,112,113,114, 121,122,123,124, 131,132,133,134, 141,142,143,144,
            211,212,213,214, 221,222,223,224, 231,232,233,234, 241,242,243,244,
        };

    NoiaBuffer original;
    noia_buffer_setup(&original, 3, 2, 16, NOIA_FORMAT_BGRA, original_data);

    NoiaBuffer expected;
    noia_buffer_setup(&expected, 3, 2, 16, NOIA_FORMAT_BGRA, expected_data);

    NoiaBuffer actual =
               noia_buffer_copy(&original, NOIA_TRANSFORMATION_IDENTITY);

    NOIA_ASSERT_BUFFERS_EQUAL(actual, expected);

    noia_buffer_release(&actual);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_transform_horizontally(void)
{
    uint8_t original_data[] = {
            111,112,113,114, 121,122,123,124, 131,132,133,134, 141,142,143,144,
            211,212,213,214, 221,222,223,224, 231,232,233,234, 241,242,243,244,
        };

    uint8_t expected_data[] = {
            131,132,133,134, 121,122,123,124, 111,112,113,114, 0,0,0,0,
            231,232,233,234, 221,222,223,224, 211,212,213,214, 0,0,0,0,
        };

    NoiaBuffer original;
    noia_buffer_setup(&original, 3, 2, 16, NOIA_FORMAT_BGRA, original_data);

    NoiaBuffer expected;
    noia_buffer_setup(&expected, 3, 2, 16, NOIA_FORMAT_BGRA, expected_data);

    NoiaBuffer actual =
               noia_buffer_copy(&original, NOIA_TRANSFORMATION_HORIZONTAL);

    // We don't care about padding, so make it the same
    *((int32_t*) &actual.data[12]) = 0;
    *((int32_t*) &actual.data[28]) = 0;

    NOIA_ASSERT_BUFFERS_EQUAL(actual, expected);

    noia_buffer_release(&actual);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_transform_vertically(void)
{
    uint8_t original_data[] = {
            111,112,113,114, 121,122,123,124, 131,132,133,134, 141,142,143,144,
            211,212,213,214, 221,222,223,224, 231,232,233,234, 241,242,243,244,
        };

    uint8_t expected_data[] = {
            211,212,213,214, 221,222,223,224, 231,232,233,234, 241,242,243,244,
            111,112,113,114, 121,122,123,124, 131,132,133,134, 141,142,143,144,
        };

    NoiaBuffer original;
    noia_buffer_setup(&original, 3, 2, 16, NOIA_FORMAT_BGRA, original_data);

    NoiaBuffer expected;
    noia_buffer_setup(&expected, 3, 2, 16, NOIA_FORMAT_BGRA, expected_data);

    NoiaBuffer actual =
               noia_buffer_copy(&original, NOIA_TRANSFORMATION_VERTICAL);

    NOIA_ASSERT_BUFFERS_EQUAL(actual, expected);

    noia_buffer_release(&actual);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_transform_horizontally_and_vertically(void)
{
    uint8_t original_data[] = {
            111,112,113,114, 121,122,123,124, 131,132,133,134, 141,142,143,144,
            211,212,213,214, 221,222,223,224, 231,232,233,234, 241,242,243,244,
        };

    uint8_t expected_data[] = {
            231,232,233,234, 221,222,223,224, 211,212,213,214, 0,0,0,0,
            131,132,133,134, 121,122,123,124, 111,112,113,114, 0,0,0,0,
        };

    NoiaBuffer original;
    noia_buffer_setup(&original, 3, 2, 16, NOIA_FORMAT_BGRA, original_data);

    NoiaBuffer expected;
    noia_buffer_setup(&expected, 3, 2, 16, NOIA_FORMAT_BGRA, expected_data);

    NoiaBuffer actual = noia_buffer_copy(&original,
                 NOIA_TRANSFORMATION_VERTICAL | NOIA_TRANSFORMATION_HORIZONTAL);

    // We don't care about padding, so make it the same
    *((int32_t*) &actual.data[12]) = 0;
    *((int32_t*) &actual.data[28]) = 0;

    NOIA_ASSERT_BUFFERS_EQUAL(actual, expected);

    noia_buffer_release(&actual);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_reformat_from_rgba_to_bgra(void)
{
    uint8_t actual_data[] = {
            111,112,113,114, 121,122,123,124, 131,132,133,134, 141,142,143,144,
            211,212,213,214, 221,222,223,224, 231,232,233,234, 241,242,243,244,
        };

    uint8_t expected_data[] = {
            113,112,111,114, 123,122,121,124, 133,132,131,134, 141,142,143,144,
            213,212,211,214, 223,222,221,224, 233,232,231,234, 241,242,243,244,
        };

    NoiaBuffer actual;
    noia_buffer_setup(&actual, 3, 2, 16, NOIA_FORMAT_RGBA, actual_data);

    NoiaBuffer expected;
    noia_buffer_setup(&expected, 3, 2, 16, NOIA_FORMAT_BGRA, expected_data);

    noia_buffer_reformat(&actual, NOIA_FORMAT_BGRA);

    NOIA_ASSERT_BUFFERS_EQUAL(actual, expected);

    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_reformat_from_bgra_to_rgba(void)
{
    uint8_t actual_data[] = {
            111,112,113,114, 121,122,123,124, 131,132,133,134, 141,142,143,144,
            211,212,213,214, 221,222,223,224, 231,232,233,234, 241,242,243,244,
        };

    uint8_t expected_data[] = {
            113,112,111,114, 123,122,121,124, 133,132,131,134, 141,142,143,144,
            213,212,211,214, 223,222,221,224, 233,232,231,234, 241,242,243,244,
        };

    NoiaBuffer actual;
    noia_buffer_setup(&actual, 3, 2, 16, NOIA_FORMAT_BGRA, actual_data);

    NoiaBuffer expected;
    noia_buffer_setup(&expected, 3, 2, 16, NOIA_FORMAT_RGBA, expected_data);

    noia_buffer_reformat(&actual, NOIA_FORMAT_RGBA);

    NOIA_ASSERT_BUFFERS_EQUAL(actual, expected);

    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    NOIA_INIT_TESTS;

    NoiaTest test[] = {
            NOIA_TEST(should_transform_identically),
            NOIA_TEST(should_transform_horizontally),
            NOIA_TEST(should_transform_vertically),
            NOIA_TEST(should_transform_horizontally_and_vertically),
            NOIA_TEST(should_reformat_from_rgba_to_bgra),
            NOIA_TEST(should_reformat_from_bgra_to_rgba),
        };

    return noia_test_run("Buffer", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

