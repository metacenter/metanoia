// file: tests-frame.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-frame.h"
#include "tests-suit.h"
#include "mock-surface-manager.h"

#include <malloc.h>

#define NOIA_MAKE_FRAMES() \
    NoiaPosition pos = {0,0}; \
    NoiaSize size = {0,0}; \
    NoiaFrame* r  = noia_frame_new(); \
    NoiaFrame* v  = noia_frame_new(); \
    NoiaFrame* h  = noia_frame_new(); \
    NoiaFrame* s  = noia_frame_new(); \
    NoiaFrame* v1 = noia_frame_new(); \
    NoiaFrame* v2 = noia_frame_new(); \
    NoiaFrame* v3 = noia_frame_new(); \
    NoiaFrame* v4 = noia_frame_new(); \
    NoiaFrame* v5 = noia_frame_new(); \
    NoiaFrame* h1 = noia_frame_new(); \
    NoiaFrame* h2 = noia_frame_new(); \
    NoiaFrame* h3 = noia_frame_new(); \
    NoiaFrame* h4 = noia_frame_new(); \
    NoiaFrame* h5 = noia_frame_new(); \
    NoiaFrame* s1 = noia_frame_new(); \
    NoiaFrame* s2 = noia_frame_new(); \
    NoiaFrame* s3 = noia_frame_new(); \
    NoiaFrame* s4 = noia_frame_new(); \
    NoiaFrame* s5 = noia_frame_new(); \
    noia_frame_configure(r, NOIA_FRAME_TYPE_NONE, \
                         scInvalidSurfaceId, pos, size); \
    noia_frame_configure(v, NOIA_FRAME_TYPE_VERTICAL, \
                         scInvalidSurfaceId, pos, size); \
    noia_frame_configure(h, NOIA_FRAME_TYPE_HORIZONTAL, \
                         scInvalidSurfaceId, pos, size); \
    noia_frame_configure(s,  NOIA_FRAME_TYPE_STACKED, \
                         scInvalidSurfaceId, pos, size); \
    noia_frame_configure(v1, NOIA_FRAME_TYPE_LEAF, 11, pos, size); \
    noia_frame_configure(v2, NOIA_FRAME_TYPE_LEAF, 12, pos, size); \
    noia_frame_configure(v3, NOIA_FRAME_TYPE_LEAF, 13, pos, size); \
    noia_frame_configure(v4, NOIA_FRAME_TYPE_LEAF, 14, pos, size); \
    noia_frame_configure(v5, NOIA_FRAME_TYPE_LEAF, 15, pos, size); \
    noia_frame_configure(h1, NOIA_FRAME_TYPE_LEAF, 21, pos, size); \
    noia_frame_configure(h2, NOIA_FRAME_TYPE_LEAF, 22, pos, size); \
    noia_frame_configure(h3, NOIA_FRAME_TYPE_LEAF, 23, pos, size); \
    noia_frame_configure(h4, NOIA_FRAME_TYPE_LEAF, 24, pos, size); \
    noia_frame_configure(h5, NOIA_FRAME_TYPE_LEAF, 25, pos, size); \
    noia_frame_configure(s1, NOIA_FRAME_TYPE_LEAF, 31, pos, size); \
    noia_frame_configure(s2, NOIA_FRAME_TYPE_LEAF, 32, pos, size); \
    noia_frame_configure(s3, NOIA_FRAME_TYPE_LEAF, 33, pos, size); \
    noia_frame_configure(s4, NOIA_FRAME_TYPE_LEAF, 34, pos, size); \
    noia_frame_configure(s5, NOIA_FRAME_TYPE_LEAF, 35, pos, size); \
    noia_frame_append (s,  s3); \
    noia_frame_prepend(s,  s2); \
    noia_frame_append (s,  s4); \
    noia_frame_prepend(s3, s1); \
    noia_frame_append (s3, s5); \
    noia_frame_append (r,  h);  \
    noia_frame_prepend(r,  v);  \
    noia_frame_append (r,  s);  \
    noia_frame_append (v,  v3); \
    noia_frame_prepend(v,  v2); \
    noia_frame_append (v,  v4); \
    noia_frame_prepend(v3, v1); \
    noia_frame_append (v3, v5); \
    noia_frame_append (h,  h3); \
    noia_frame_prepend(h,  h2); \
    noia_frame_append (h,  h4); \
    noia_frame_prepend(h3, h1); \
    noia_frame_append (h3, h5); \

#define NOIA_ASSERT_FRAME_ARRAY(ARRAY, POOL) \
    NOIA_ASSERT(ARRAY_LEN(ARRAY) == noia_pool_get_size(POOL), \
                "Number of elements should be '%lu' (is '%u')", \
                ARRAY_LEN(ARRAY), noia_pool_get_size(POOL)); \
    for (unsigned i = 0; i < ARRAY_LEN(ARRAY); ++i) { \
        NoiaSurfaceContext* c1 = &ARRAY[i]; \
        NoiaSurfaceContext* c2 = noia_pool_get(POOL, i); \
        NOIA_ASSERT(c1->sid == c2->sid && \
                    c1->pos.x == c2->pos.x && \
                    c1->pos.y == c2->pos.y, \
                    "Element should be {'%lu', '%u', '%u'} " \
                    "(is {'%lu', '%u', %u})", \
                    c1->sid, c1->pos.x, c1->pos.y, \
                    c2->sid, c2->pos.x, c2->pos.y); }

#define NOIA_ASSERT_FRAME_POINTER(FRAME1, FRAME2) \
    NOIA_ASSERT(FRAME1 == FRAME2, \
                "Frame should be '%p', (is '%p')", \
                (void*) FRAME1, (void*) FRAME2); \

//------------------------------------------------------------------------------

/// Check if frames are assigned to correct parent.
/// Content check will be done in `should_translate_to_array`
NoiaTestResult should_append_and_prepend_values()
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES();

    NOIA_ASSERT_TRUNK(r,  NULL);
    NOIA_ASSERT_TRUNK(h,  r);
    NOIA_ASSERT_TRUNK(v,  r);
    NOIA_ASSERT_TRUNK(s,  r);
    NOIA_ASSERT_TRUNK(v1, v);
    NOIA_ASSERT_TRUNK(v2, v);
    NOIA_ASSERT_TRUNK(v3, v);
    NOIA_ASSERT_TRUNK(v4, v);
    NOIA_ASSERT_TRUNK(v5, v);
    NOIA_ASSERT_TRUNK(h1, h);
    NOIA_ASSERT_TRUNK(h2, h);
    NOIA_ASSERT_TRUNK(h3, h);
    NOIA_ASSERT_TRUNK(h4, h);
    NOIA_ASSERT_TRUNK(h5, h);
    NOIA_ASSERT_TRUNK(s1, s);
    NOIA_ASSERT_TRUNK(s2, s);
    NOIA_ASSERT_TRUNK(s3, s);
    NOIA_ASSERT_TRUNK(s4, s);
    NOIA_ASSERT_TRUNK(s5, s);

    NOIA_ASSERT_CHAIN_LEN(r->twigs, 3);
    NOIA_ASSERT_CHAIN_LEN(v->twigs, 5);
    NOIA_ASSERT_CHAIN_LEN(h->twigs, 5);
    NOIA_ASSERT_CHAIN_LEN(s->twigs, 5);

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check order and position when translating whole frame to array.
NoiaTestResult should_translate_frame_to_array()
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES();

    NoiaSurfaceContext a[] = {
            {11, {0, 0}},
            {12, {0, 0}},
            {13, {0, 0}},
            {14, {0, 0}},
            {15, {0, 0}},
            {21, {0, 0}},
            {22, {0, 0}},
            {23, {0, 0}},
            {24, {0, 0}},
            {25, {0, 0}},
            {31, {0, 0}},
            {32, {0, 0}},
            {33, {0, 0}},
            {34, {0, 0}},
            {35, {0, 0}},
        };

    NoiaPool* pool = noia_pool_create(ARRAY_LEN(a), sizeof(NoiaSurfaceContext));

    noia_frame_to_array(r, pool);
    noia_frame_free(r);

    NOIA_ASSERT_FRAME_ARRAY(a, pool);

    noia_pool_destroy(pool);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check order and position when translating subframes to array.
NoiaTestResult should_translate_subframes_to_array()
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES();

    NoiaSurfaceContext av[] = {
            {11, {0, 0}},
            {12, {0, 0}},
            {13, {0, 0}},
            {14, {0, 0}},
            {15, {0, 0}},
        };
    NoiaSurfaceContext ah[] = {
            {21, {0, 0}},
            {22, {0, 0}},
            {23, {0, 0}},
            {24, {0, 0}},
            {25, {0, 0}},
        };
    NoiaSurfaceContext as[] = {
            {31, {0, 0}},
            {32, {0, 0}},
            {33, {0, 0}},
            {34, {0, 0}},
            {35, {0, 0}},
        };

    NoiaPool* pv = noia_pool_create(ARRAY_LEN(ah), sizeof(NoiaSurfaceContext));
    NoiaPool* ph = noia_pool_create(ARRAY_LEN(av), sizeof(NoiaSurfaceContext));
    NoiaPool* ps = noia_pool_create(ARRAY_LEN(as), sizeof(NoiaSurfaceContext));

    noia_frame_to_array(v, pv);
    noia_frame_to_array(h, ph);
    noia_frame_to_array(s, ps);
    noia_frame_free(r);

    NOIA_ASSERT_FRAME_ARRAY(av, pv)
    NOIA_ASSERT_FRAME_ARRAY(ah, ph)
    NOIA_ASSERT_FRAME_ARRAY(as, ps)

    noia_pool_destroy(pv);
    noia_pool_destroy(ph);
    noia_pool_destroy(ps);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check correctness of frame after removing several leafs.
NoiaTestResult should_remove_some_leaf_frames()
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES();

    noia_frame_remove_self(v1);
    noia_frame_remove_self(v2);
    noia_frame_remove_self(v3);
    noia_frame_remove_self(h5);
    noia_frame_remove_self(h4);
    noia_frame_remove_self(h3);
    noia_frame_remove_self(s3);
    noia_frame_remove_self(s4);
    noia_frame_remove_self(s2);

    NOIA_ASSERT_TRUNK(r,  NULL);
    NOIA_ASSERT_TRUNK(h,  r);
    NOIA_ASSERT_TRUNK(v,  r);
    NOIA_ASSERT_TRUNK(s,  r);
    NOIA_ASSERT_TRUNK(v1, NULL);
    NOIA_ASSERT_TRUNK(v2, NULL);
    NOIA_ASSERT_TRUNK(v3, NULL);
    NOIA_ASSERT_TRUNK(v4, v);
    NOIA_ASSERT_TRUNK(v5, v);
    NOIA_ASSERT_TRUNK(h1, h);
    NOIA_ASSERT_TRUNK(h2, h);
    NOIA_ASSERT_TRUNK(h3, NULL);
    NOIA_ASSERT_TRUNK(h4, NULL);
    NOIA_ASSERT_TRUNK(h5, NULL);
    NOIA_ASSERT_TRUNK(s1, s);
    NOIA_ASSERT_TRUNK(s2, NULL);
    NOIA_ASSERT_TRUNK(s3, NULL);
    NOIA_ASSERT_TRUNK(s4, NULL);
    NOIA_ASSERT_TRUNK(s5, s);

    NOIA_ASSERT_CHAIN_LEN(r->twigs, 3);
    NOIA_ASSERT_CHAIN_LEN(v->twigs, 2);
    NOIA_ASSERT_CHAIN_LEN(h->twigs, 2);
    NOIA_ASSERT_CHAIN_LEN(s->twigs, 2);

    noia_frame_free(v1);
    noia_frame_free(v2);
    noia_frame_free(v3);
    noia_frame_free(h5);
    noia_frame_free(h4);
    noia_frame_free(h3);
    noia_frame_free(s3);
    noia_frame_free(s4);
    noia_frame_free(s2);
    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check correctness of frame after removing frame with leafs.
NoiaTestResult should_remove_frame_with_subframes()
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES();

    noia_frame_remove_self(h);

    NOIA_ASSERT_TRUNK(r,  NULL);
    NOIA_ASSERT_TRUNK(h,  NULL);
    NOIA_ASSERT_TRUNK(v,  r);
    NOIA_ASSERT_TRUNK(s,  r);
    NOIA_ASSERT_TRUNK(v1, v);
    NOIA_ASSERT_TRUNK(v2, v);
    NOIA_ASSERT_TRUNK(v3, v);
    NOIA_ASSERT_TRUNK(v4, v);
    NOIA_ASSERT_TRUNK(v5, v);
    NOIA_ASSERT_TRUNK(h1, h);
    NOIA_ASSERT_TRUNK(h2, h);
    NOIA_ASSERT_TRUNK(h3, h);
    NOIA_ASSERT_TRUNK(h4, h);
    NOIA_ASSERT_TRUNK(h5, h);
    NOIA_ASSERT_TRUNK(s1, s);
    NOIA_ASSERT_TRUNK(s2, s);
    NOIA_ASSERT_TRUNK(s3, s);
    NOIA_ASSERT_TRUNK(s4, s);
    NOIA_ASSERT_TRUNK(s5, s);

    NOIA_ASSERT_CHAIN_LEN(r->twigs, 2);
    NOIA_ASSERT_CHAIN_LEN(v->twigs, 5);
    NOIA_ASSERT_CHAIN_LEN(h->twigs, 5);
    NOIA_ASSERT_CHAIN_LEN(s->twigs, 5);

    noia_frame_free(h);
    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Resettle one frame without children and check if trunks are OK.
NoiaTestResult should_resettle_one_frame()
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES();

    noia_frame_resettle(h3, s);

    NOIA_ASSERT_TRUNK(r,  NULL);
    NOIA_ASSERT_TRUNK(h,  r);
    NOIA_ASSERT_TRUNK(v,  r);
    NOIA_ASSERT_TRUNK(s,  r);
    NOIA_ASSERT_TRUNK(v1, v);
    NOIA_ASSERT_TRUNK(v2, v);
    NOIA_ASSERT_TRUNK(v3, v);
    NOIA_ASSERT_TRUNK(v4, v);
    NOIA_ASSERT_TRUNK(v5, v);
    NOIA_ASSERT_TRUNK(h1, h);
    NOIA_ASSERT_TRUNK(h2, h);
    NOIA_ASSERT_TRUNK(h3, s);
    NOIA_ASSERT_TRUNK(h4, h);
    NOIA_ASSERT_TRUNK(h5, h);
    NOIA_ASSERT_TRUNK(s1, s);
    NOIA_ASSERT_TRUNK(s2, s);
    NOIA_ASSERT_TRUNK(s3, s);
    NOIA_ASSERT_TRUNK(s4, s);
    NOIA_ASSERT_TRUNK(s5, s);

    NOIA_ASSERT_CHAIN_LEN(r->twigs, 3);
    NOIA_ASSERT_CHAIN_LEN(v->twigs, 5);
    NOIA_ASSERT_CHAIN_LEN(h->twigs, 4);
    NOIA_ASSERT_CHAIN_LEN(s->twigs, 6);

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Resettle one frame with children and check if trunks are OK.
NoiaTestResult should_resettle_frame_with_subframes()
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES();

    noia_frame_resettle(h, s);

    NOIA_ASSERT_TRUNK(r,  NULL);
    NOIA_ASSERT_TRUNK(h,  s);
    NOIA_ASSERT_TRUNK(v,  r);
    NOIA_ASSERT_TRUNK(s,  r);
    NOIA_ASSERT_TRUNK(v1, v);
    NOIA_ASSERT_TRUNK(v2, v);
    NOIA_ASSERT_TRUNK(v3, v);
    NOIA_ASSERT_TRUNK(v4, v);
    NOIA_ASSERT_TRUNK(v5, v);
    NOIA_ASSERT_TRUNK(h1, h);
    NOIA_ASSERT_TRUNK(h2, h);
    NOIA_ASSERT_TRUNK(h3, h);
    NOIA_ASSERT_TRUNK(h4, h);
    NOIA_ASSERT_TRUNK(h5, h);
    NOIA_ASSERT_TRUNK(s1, s);
    NOIA_ASSERT_TRUNK(s2, s);
    NOIA_ASSERT_TRUNK(s3, s);
    NOIA_ASSERT_TRUNK(s4, s);
    NOIA_ASSERT_TRUNK(s5, s);

    NOIA_ASSERT_CHAIN_LEN(r->twigs, 2);
    NOIA_ASSERT_CHAIN_LEN(v->twigs, 5);
    NOIA_ASSERT_CHAIN_LEN(h->twigs, 5);
    NOIA_ASSERT_CHAIN_LEN(s->twigs, 6);

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check if `noia_frame_find_with_sid` returns correct frames.
NoiaTestResult should_find_with_sid()
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES();

    NOIA_ASSERT_FRAME_POINTER(NULL, noia_frame_find_with_sid(r, 666))

    NOIA_ASSERT_FRAME_POINTER(v1, noia_frame_find_with_sid(r, 11))
    NOIA_ASSERT_FRAME_POINTER(v2, noia_frame_find_with_sid(r, 12))
    NOIA_ASSERT_FRAME_POINTER(v3, noia_frame_find_with_sid(r, 13))
    NOIA_ASSERT_FRAME_POINTER(v4, noia_frame_find_with_sid(r, 14))
    NOIA_ASSERT_FRAME_POINTER(v5, noia_frame_find_with_sid(r, 15))
    NOIA_ASSERT_FRAME_POINTER(h1, noia_frame_find_with_sid(r, 21))
    NOIA_ASSERT_FRAME_POINTER(h2, noia_frame_find_with_sid(r, 22))
    NOIA_ASSERT_FRAME_POINTER(h3, noia_frame_find_with_sid(r, 23))
    NOIA_ASSERT_FRAME_POINTER(h4, noia_frame_find_with_sid(r, 24))
    NOIA_ASSERT_FRAME_POINTER(h5, noia_frame_find_with_sid(r, 25))
    NOIA_ASSERT_FRAME_POINTER(s1, noia_frame_find_with_sid(r, 31))
    NOIA_ASSERT_FRAME_POINTER(s2, noia_frame_find_with_sid(r, 32))
    NOIA_ASSERT_FRAME_POINTER(s3, noia_frame_find_with_sid(r, 33))
    NOIA_ASSERT_FRAME_POINTER(s4, noia_frame_find_with_sid(r, 34))
    NOIA_ASSERT_FRAME_POINTER(s5, noia_frame_find_with_sid(r, 35))

    NOIA_ASSERT_FRAME_POINTER(NULL, noia_frame_find_with_sid(s, 11))
    NOIA_ASSERT_FRAME_POINTER(NULL, noia_frame_find_with_sid(s, 13))
    NOIA_ASSERT_FRAME_POINTER(NULL, noia_frame_find_with_sid(s, 22))

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Empty test
NoiaTestResult should()
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES();

    // ...

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    NOIA_INIT_TESTS();

    NoiaTest test[] = {
            NOIA_TEST(should),
            NOIA_TEST(should_append_and_prepend_values),
            NOIA_TEST(should_translate_frame_to_array),
            NOIA_TEST(should_translate_subframes_to_array),
            NOIA_TEST(should_remove_some_leaf_frames),
            NOIA_TEST(should_remove_frame_with_subframes),
            NOIA_TEST(should_resettle_one_frame),
            NOIA_TEST(should_resettle_frame_with_subframes),
            NOIA_TEST(should_find_with_sid),
        };

    return noia_test_run("Frame", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

