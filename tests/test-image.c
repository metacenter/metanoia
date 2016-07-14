// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-image.h"
#include "tests-suit.h"

//------------------------------------------------------------------------------

const char* noia_test_image_format_to_string(NoiaImageFormat format)
{
    const char* str = "unknown";
    if (format == NOIA_IMAGE_PNG) {
        str = "PNG";
    } else if (format == NOIA_IMAGE_JPEG) {
        str = "JPEG";
    }
    return str;
}

//------------------------------------------------------------------------------

#define NOIA_ASSERT_IMAGE_FORMAT(PATH, EXPECTATION) { \
    NoiaImageFormat actual = noia_image_guess_format(PATH); \
    NOIA_ASSERT(actual == EXPECTATION, \
                "Format of '%s' is '%s', should be '%s'", PATH, \
                noia_test_image_format_to_string(actual), \
                noia_test_image_format_to_string(EXPECTATION)); }

//------------------------------------------------------------------------------

NoiaTestResult should_recognize_png(void)
{
    NOIA_ASSERT_IMAGE_FORMAT("file.png", NOIA_IMAGE_PNG);
    NOIA_ASSERT_IMAGE_FORMAT("file.PNG", NOIA_IMAGE_PNG);
    NOIA_ASSERT_IMAGE_FORMAT(".png", NOIA_IMAGE_PNG);
    NOIA_ASSERT_IMAGE_FORMAT(".PNG", NOIA_IMAGE_PNG);

    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_recognize_jpeg(void)
{
    NOIA_ASSERT_IMAGE_FORMAT("file.jpg", NOIA_IMAGE_JPEG);
    NOIA_ASSERT_IMAGE_FORMAT("file.JPG", NOIA_IMAGE_JPEG);
    NOIA_ASSERT_IMAGE_FORMAT("file.jpeg", NOIA_IMAGE_JPEG);
    NOIA_ASSERT_IMAGE_FORMAT("file.JPEG", NOIA_IMAGE_JPEG);
    NOIA_ASSERT_IMAGE_FORMAT(".jpg", NOIA_IMAGE_JPEG);
    NOIA_ASSERT_IMAGE_FORMAT(".jpeg", NOIA_IMAGE_JPEG);

    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaTestResult should_ignore_others(void)
{
    NOIA_ASSERT_IMAGE_FORMAT("filepng", NOIA_IMAGE_UNKNOWN);
    NOIA_ASSERT_IMAGE_FORMAT("filejpg", NOIA_IMAGE_UNKNOWN);
    NOIA_ASSERT_IMAGE_FORMAT("file.pngx", NOIA_IMAGE_UNKNOWN);
    NOIA_ASSERT_IMAGE_FORMAT("file.jpgx", NOIA_IMAGE_UNKNOWN);
    NOIA_ASSERT_IMAGE_FORMAT("file.jpegx", NOIA_IMAGE_UNKNOWN);

    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    NOIA_INIT_TESTS;

    NoiaTest test[] = {
            NOIA_TEST(should_recognize_png),
            NOIA_TEST(should_recognize_jpeg),
            NOIA_TEST(should_ignore_others),
        };

    return noia_test_run("Image", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

