// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "global-types.h"
#include "tests-suit.h"

//------------------------------------------------------------------------------

/// Check if `noia_position_is_inside` works correctly.
NoiaTestResult should_correctly_check_if_point_is_inside_area(void)
{
    NoiaArea area = {{10, 10}, {10, 10}};

    NoiaPosition inside_positions[] = {
            {15, 15},
            {10, 10},
            {10, 19},
            {19, 10},
            {19, 19},
        };

    NoiaPosition outside_positions[] = {
            { 0,  0},
            { 9, 15},
            {20, 15},
            {15,  9},
            {15, 20},
        };

    for (unsigned i = 0; i < NOIA_SIZEOF_ARRAY(inside_positions); ++i) {
        NoiaPosition pos = inside_positions[i];
        NOIA_ASSERT(noia_position_is_inside(pos, area),
                    "Position {'%d', '%d'} should be inside", pos.x, pos.y);
    }

    for (unsigned i = 0; i < NOIA_SIZEOF_ARRAY(outside_positions); ++i) {
        NoiaPosition pos = outside_positions[i];
        NOIA_ASSERT(!noia_position_is_inside(pos, area),
                    "Position {'%d', '%d'} should be outside", pos.x, pos.y);
    }

    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check if `noia_position_cast` works correctly.
NoiaTestResult should_correctly_cast_point_into_area(void)
{
    NoiaArea area = {{10, 10}, {10, 10}};

    NoiaPosition positions[] = {
            {15, 15}, {15, 15},
            { 0, 15}, {10, 15},
            {30, 15}, {19, 15},
            {15,  0}, {15, 10},
            {15, 30}, {15, 19},
            { 0,  0}, {10, 10},
            {30,  0}, {19, 10},
            { 0, 30}, {10, 19},
            {30, 30}, {19, 19},
        };

    for (unsigned i = 0; i < NOIA_SIZEOF_ARRAY(positions); i += 2) {
        NoiaPosition orig_pos = positions[i+0];
        NoiaPosition corr_pos = positions[i+1];
        NoiaPosition cast_pos = noia_position_cast(orig_pos, area);

        NOIA_ASSERT((cast_pos.x == corr_pos.x) && (cast_pos.y == corr_pos.y),
                    "Casted position should be {'%d', '%d'} (is {%d, %d})",
                    corr_pos.x, corr_pos.y, cast_pos.x, cast_pos.y);
    }

    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    NOIA_INIT_TESTS;

    NoiaTest test[] = {
            NOIA_TEST(should_correctly_check_if_point_is_inside_area),
            NOIA_TEST(should_correctly_cast_point_into_area),
        };

    return noia_test_run("Globals", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

