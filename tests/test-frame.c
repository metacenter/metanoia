// file: tests-frame.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-frame.h"
#include "tests-suit.h"
#include "mock-surface-manager.h"

#include <malloc.h>

//------------------------------------------------------------------------------

void noia_test_frame_config(NoiaFrame* frame,
                            NoiaFrameType type,
                            NoiaSurfaceId sid)
{
    NoiaPosition pos = {0,0};
    NoiaSize size = {0,0};
    noia_frame_configure(frame, type, sid, pos, size);
}

//------------------------------------------------------------------------------

#define NOIA_MAKE_FRAMES_SIMPLE \
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
    noia_test_frame_config(r, NOIA_FRAME_TYPE_NONE,       scInvalidSurfaceId); \
    noia_test_frame_config(v, NOIA_FRAME_TYPE_VERTICAL,   scInvalidSurfaceId); \
    noia_test_frame_config(h, NOIA_FRAME_TYPE_HORIZONTAL, scInvalidSurfaceId); \
    noia_test_frame_config(s,  NOIA_FRAME_TYPE_STACKED,   scInvalidSurfaceId); \
    noia_test_frame_config(v1, NOIA_FRAME_TYPE_LEAF, 11); \
    noia_test_frame_config(v2, NOIA_FRAME_TYPE_LEAF, 12); \
    noia_test_frame_config(v3, NOIA_FRAME_TYPE_LEAF, 13); \
    noia_test_frame_config(v4, NOIA_FRAME_TYPE_LEAF, 14); \
    noia_test_frame_config(v5, NOIA_FRAME_TYPE_LEAF, 15); \
    noia_test_frame_config(h1, NOIA_FRAME_TYPE_LEAF, 21); \
    noia_test_frame_config(h2, NOIA_FRAME_TYPE_LEAF, 22); \
    noia_test_frame_config(h3, NOIA_FRAME_TYPE_LEAF, 23); \
    noia_test_frame_config(h4, NOIA_FRAME_TYPE_LEAF, 24); \
    noia_test_frame_config(h5, NOIA_FRAME_TYPE_LEAF, 25); \
    noia_test_frame_config(s1, NOIA_FRAME_TYPE_LEAF, 31); \
    noia_test_frame_config(s2, NOIA_FRAME_TYPE_LEAF, 32); \
    noia_test_frame_config(s3, NOIA_FRAME_TYPE_LEAF, 33); \
    noia_test_frame_config(s4, NOIA_FRAME_TYPE_LEAF, 34); \
    noia_test_frame_config(s5, NOIA_FRAME_TYPE_LEAF, 35); \
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

#define NOIA_MAKE_FRAMES_POSITIONED \
    NoiaFrame* r     = noia_frame_new(); \
    NoiaFrame* a     = noia_frame_new(); \
    NoiaFrame* b     = noia_frame_new(); \
    NoiaFrame* c     = noia_frame_new(); \
    NoiaFrame* d     = noia_frame_new(); \
    NoiaFrame* e     = noia_frame_new(); \
    NoiaFrame* f     = noia_frame_new(); \
    NoiaFrame* abc   = noia_frame_new(); \
    NoiaFrame* de    = noia_frame_new(); \
    NoiaFrame* abcde = noia_frame_new(); \
    noia_frame_append(abc,   a); \
    noia_frame_append(abc,   b); \
    noia_frame_append(abc,   c); \
    noia_frame_append(de,    d); \
    noia_frame_append(de,    e); \
    noia_frame_append(abcde, abc); \
    noia_frame_append(abcde, de); \
    noia_frame_append(r,     abcde); \
    noia_frame_append(r,     f); \
    noia_frame_configure(a, NOIA_FRAME_TYPE_LEAF, 1, \
                         (NoiaPosition) { 0,  0}, (NoiaSize) { 40,  60}); \
    noia_frame_configure(b, NOIA_FRAME_TYPE_LEAF, 2, \
                         (NoiaPosition) { 0,  0}, (NoiaSize) { 40,  60}); \
    noia_frame_configure(c, NOIA_FRAME_TYPE_LEAF, 3, \
                         (NoiaPosition) { 0,  0}, (NoiaSize) { 40,  60}); \
    noia_frame_configure(d, NOIA_FRAME_TYPE_LEAF, 4, \
                         (NoiaPosition) {40,  0}, (NoiaSize) { 30,  60}); \
    noia_frame_configure(e, NOIA_FRAME_TYPE_LEAF, 5, \
                         (NoiaPosition) {40,  0}, (NoiaSize) { 30,  60}); \
    noia_frame_configure(f, NOIA_FRAME_TYPE_LEAF, 6, \
                         (NoiaPosition) { 0, 60}, (NoiaSize) { 70,  60}); \
    noia_frame_configure(r, NOIA_FRAME_TYPE_VERTICAL, scInvalidSurfaceId, \
                         (NoiaPosition) { 0,  0}, (NoiaSize) {100, 120}); \
    noia_frame_configure(abc, NOIA_FRAME_TYPE_STACKED, scInvalidSurfaceId, \
                         (NoiaPosition) { 0,  0}, (NoiaSize) { 40,  60}); \
    noia_frame_configure(de,    NOIA_FRAME_TYPE_HORIZONTAL, scInvalidSurfaceId,\
                         (NoiaPosition) {40,  0}, (NoiaSize) { 60,  60}); \
    noia_frame_configure(abcde, NOIA_FRAME_TYPE_HORIZONTAL, scInvalidSurfaceId,\
                         (NoiaPosition) { 0,  0}, (NoiaSize) {100,  60});

#define NOIA_ASSERT_FRAME_ARRAY(ARRAY, POOL) \
    NOIA_ASSERT(NOIA_SIZEOF_ARRAY(ARRAY) == noia_pool_get_size(POOL), \
                "Number of elements should be '%lu' (is '%u')", \
                NOIA_SIZEOF_ARRAY(ARRAY), noia_pool_get_size(POOL)); \
    for (unsigned i = 0; i < NOIA_SIZEOF_ARRAY(ARRAY); ++i) { \
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

/// Check if appended and prepended frames are assigned to correct trunk.
/// Content check will be done in `should_translate_to_array`
NoiaTestResult should_append_and_prepend_values(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_SIMPLE;

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

/// Check if inserted frames are assigned to correct trunk.
NoiaTestResult should_insert_values(void)
{
    noia_mock_surface_manager_initialize();

    NoiaFrame* r  = noia_frame_new();
    NoiaFrame* f1 = noia_frame_new();
    NoiaFrame* f2 = noia_frame_new();
    NoiaFrame* f3 = noia_frame_new();
    NoiaFrame* f4 = noia_frame_new();
    NoiaFrame* f5 = noia_frame_new();
    noia_test_frame_config(r,  NOIA_FRAME_TYPE_NONE, scInvalidSurfaceId);
    noia_test_frame_config(f1, NOIA_FRAME_TYPE_LEAF, 1);
    noia_test_frame_config(f2, NOIA_FRAME_TYPE_LEAF, 2);
    noia_test_frame_config(f3, NOIA_FRAME_TYPE_LEAF, 3);
    noia_test_frame_config(f4, NOIA_FRAME_TYPE_LEAF, 4);
    noia_test_frame_config(f5, NOIA_FRAME_TYPE_LEAF, 5);
    noia_frame_append(r, f2);
    noia_frame_insert_after(f2, f4);
    noia_frame_insert_before(f4, f3);
    noia_frame_insert_after(f4, f5);
    noia_frame_insert_before(f2, f1);

    NOIA_ASSERT_TRUNK(f1, r);
    NOIA_ASSERT_TRUNK(f2, r);
    NOIA_ASSERT_TRUNK(f3, r);
    NOIA_ASSERT_TRUNK(f4, r);
    NOIA_ASSERT_TRUNK(f5, r);
    NOIA_ASSERT_CHAIN_LEN(r->twigs, 5);

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check order and position when translating whole frame to array.
NoiaTestResult should_translate_frame_to_array(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_SIMPLE;

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

    NoiaPool* pool = noia_pool_create(NOIA_SIZEOF_ARRAY(a),
                                      sizeof(NoiaSurfaceContext));

    noia_frame_to_array(r, pool);
    noia_frame_free(r);

    NOIA_ASSERT_FRAME_ARRAY(a, pool);

    noia_pool_destroy(pool);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check order and position when translating subframes to array.
NoiaTestResult should_translate_subframes_to_array(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_SIMPLE;

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

    NoiaPool* pv = noia_pool_create(NOIA_SIZEOF_ARRAY(ah),
                                    sizeof(NoiaSurfaceContext));
    NoiaPool* ph = noia_pool_create(NOIA_SIZEOF_ARRAY(av),
                                    sizeof(NoiaSurfaceContext));
    NoiaPool* ps = noia_pool_create(NOIA_SIZEOF_ARRAY(as),
                                    sizeof(NoiaSurfaceContext));

    noia_frame_to_array(v, pv);
    noia_frame_to_array(h, ph);
    noia_frame_to_array(s, ps);
    noia_frame_free(r);

    NOIA_ASSERT_FRAME_ARRAY(av, pv);
    NOIA_ASSERT_FRAME_ARRAY(ah, ph);
    NOIA_ASSERT_FRAME_ARRAY(as, ps);

    noia_pool_destroy(pv);
    noia_pool_destroy(ph);
    noia_pool_destroy(ps);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check correctness of frame after removing several leafs.
NoiaTestResult should_remove_some_leaf_frames(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_SIMPLE;

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
NoiaTestResult should_remove_frame_with_subframes(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_SIMPLE;

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
NoiaTestResult should_resettle_one_frame(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_SIMPLE;

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
NoiaTestResult should_resettle_frame_with_subframes(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_SIMPLE;

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
NoiaTestResult should_find_with_sid(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_SIMPLE;

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

/// Find contiguous frame on the same level.
///
///  - 0*South from A should be A.
///  - 1*South from A should be B.
///  - 1*North from B should be A.
///  - 1*South from B should be NULL.
///
///
///     ┌─────┐
///     │  A  │
///     ├─────┤
///     │  B  │
///     └─────┘
///
NoiaTestResult should_find_contiguous_on_the_same_level_one_further(void)
{
    noia_mock_surface_manager_initialize();

    NoiaFrame* p = NULL;
    NoiaFrame* r = noia_frame_new();
    NoiaFrame* a = noia_frame_new();
    NoiaFrame* b = noia_frame_new();
    noia_test_frame_config(r, NOIA_FRAME_TYPE_SPECIAL | NOIA_FRAME_TYPE_VERTICAL,
                           scInvalidSurfaceId);
    noia_test_frame_config(a, NOIA_FRAME_TYPE_LEAF, 1);
    noia_test_frame_config(b, NOIA_FRAME_TYPE_LEAF, 2);
    noia_frame_append(r, a);
    noia_frame_append(r, b);

    p = noia_frame_find_contiguous(a, NOIA_ARGMAND_S, 0);
    NOIA_ASSERT(p == a, "0*South from A should be A");

    p = noia_frame_find_contiguous(a, NOIA_ARGMAND_S, 1);
    NOIA_ASSERT(p == b, "1*South from A should be B");

    p = noia_frame_find_contiguous(b, NOIA_ARGMAND_N, 1);
    NOIA_ASSERT(p == a, "1*North from B should be A");

    p = noia_frame_find_contiguous(b, NOIA_ARGMAND_S, 1);
    NOIA_ASSERT(p == NULL, "1*South from B should be NULL");

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Find contiguous frame on the same level.
///
/// - 3*East from B should be E.
/// - 5*West from F should be A.
///
///
///     ┌─────┬─────┬─────┬─────┬─────┬─────┐
///     │  A  │  B  │  C  │  D  │  E  │  F  │
///     └─────┴─────┴─────┴─────┴─────┴─────┘
///
NoiaTestResult should_find_contiguous_on_the_same_level_many_further(void)
{
    noia_mock_surface_manager_initialize();

    NoiaFrame* p = NULL;
    NoiaFrame* r = noia_frame_new();
    NoiaFrame* a = noia_frame_new();
    NoiaFrame* b = noia_frame_new();
    NoiaFrame* c = noia_frame_new();
    NoiaFrame* d = noia_frame_new();
    NoiaFrame* e = noia_frame_new();
    NoiaFrame* f = noia_frame_new();
    noia_test_frame_config(r,
                           NOIA_FRAME_TYPE_SPECIAL | NOIA_FRAME_TYPE_HORIZONTAL,
                           scInvalidSurfaceId);
    noia_test_frame_config(a, NOIA_FRAME_TYPE_LEAF, 1);
    noia_test_frame_config(b, NOIA_FRAME_TYPE_LEAF, 2);
    noia_test_frame_config(c, NOIA_FRAME_TYPE_LEAF, 3);
    noia_test_frame_config(d, NOIA_FRAME_TYPE_LEAF, 4);
    noia_test_frame_config(e, NOIA_FRAME_TYPE_LEAF, 5);
    noia_test_frame_config(f, NOIA_FRAME_TYPE_LEAF, 6);
    noia_frame_append(r, a);
    noia_frame_append(r, b);
    noia_frame_append(r, c);
    noia_frame_append(r, d);
    noia_frame_append(r, e);
    noia_frame_append(r, f);

    p = noia_frame_find_contiguous(b, NOIA_ARGMAND_E, 3);
    NOIA_ASSERT(p == e, "3*West from B should be E");

    p = noia_frame_find_contiguous(f, NOIA_ARGMAND_W, 5);
    NOIA_ASSERT(p == a, "5*East from F should be A");

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Find contiguous frame on the second level.
///
///  - 1*East from B should be D.
///  - 1*East from A should be BC.
///  - 1*West from C should be A.
///  - 2*East from A should be D
///  - 1*Trunk from C should be BC.
///
///
///     ┌───────┬───────┬───────┐
///     │       │┌─────┐│       │
///     │       ││  B  ││       │
///     │   A   │├─────┤│   D   │
///     │       ││  C  ││       │
///     │       │└─────┘│       │
///     └───────┴───────┴───────┘
///
NoiaTestResult should_find_contiguous_on_the_second_level(void)
{
    noia_mock_surface_manager_initialize();

    NoiaFrame* p  = NULL;
    NoiaFrame* r  = noia_frame_new();
    NoiaFrame* a  = noia_frame_new();
    NoiaFrame* bc = noia_frame_new();
    NoiaFrame* b  = noia_frame_new();
    NoiaFrame* c  = noia_frame_new();
    NoiaFrame* d  = noia_frame_new();
    noia_test_frame_config(r,
                           NOIA_FRAME_TYPE_SPECIAL | NOIA_FRAME_TYPE_HORIZONTAL,
                           scInvalidSurfaceId);
    noia_test_frame_config(a,  NOIA_FRAME_TYPE_LEAF, 1);
    noia_test_frame_config(bc, NOIA_FRAME_TYPE_VERTICAL, scInvalidSurfaceId);
    noia_test_frame_config(b,  NOIA_FRAME_TYPE_LEAF, 2);
    noia_test_frame_config(c,  NOIA_FRAME_TYPE_LEAF, 3);
    noia_test_frame_config(d,  NOIA_FRAME_TYPE_LEAF, 4);
    noia_frame_append(r,  a);
    noia_frame_append(r,  bc);
    noia_frame_append(bc, b);
    noia_frame_append(bc, c);
    noia_frame_append(r,  d);

    p = noia_frame_find_contiguous(b, NOIA_ARGMAND_E, 1);
    NOIA_ASSERT(p == d, "1*East from B should be D");

    p = noia_frame_find_contiguous(a, NOIA_ARGMAND_E, 1);
    NOIA_ASSERT(p == bc, "1*West from A should be BC");

    p = noia_frame_find_contiguous(c, NOIA_ARGMAND_W, 1);
    NOIA_ASSERT(p == a, "1*East from C should be A");

    p = noia_frame_find_contiguous(a, NOIA_ARGMAND_E, 2);
    NOIA_ASSERT(p == d, "2*West from A should be D");

    p = noia_frame_find_contiguous(c, NOIA_ARGMAND_TRUNK, 1);
    NOIA_ASSERT(p == c->trunk, "1*Trunk from C should be BC");

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Find contiguous frame on the third level.
///
///  - 1*East from C should be D.
///
///
///     ┌─────────────────────┬─────┐
///     │┌─────┬─────────────┐│     │
///     ││     │┌─────┬─────┐││     │
///     ││  A  ││  B  │  C  │││  D  │
///     ││     │└─────┴─────┘││     │
///     │└─────┴─────────────┘│     │
///     └─────────────────────┴─────┘
///
NoiaTestResult should_find_contiguous_on_the_third_level(void)
{
    noia_mock_surface_manager_initialize();

    NoiaFrame* p    = NULL;
    NoiaFrame* r    = noia_frame_new();
    NoiaFrame* a    = noia_frame_new();
    NoiaFrame* b    = noia_frame_new();
    NoiaFrame* c    = noia_frame_new();
    NoiaFrame* d    = noia_frame_new();
    NoiaFrame* bc   = noia_frame_new();
    NoiaFrame* abc  = noia_frame_new();
    noia_test_frame_config(r,
                           NOIA_FRAME_TYPE_SPECIAL | NOIA_FRAME_TYPE_HORIZONTAL,
                           scInvalidSurfaceId);
    noia_test_frame_config(a,   NOIA_FRAME_TYPE_LEAF, 1);
    noia_test_frame_config(b,   NOIA_FRAME_TYPE_LEAF, 2);
    noia_test_frame_config(c,   NOIA_FRAME_TYPE_LEAF, 3);
    noia_test_frame_config(d,   NOIA_FRAME_TYPE_LEAF, 4);
    noia_test_frame_config(bc,  NOIA_FRAME_TYPE_VERTICAL, scInvalidSurfaceId);
    noia_test_frame_config(abc, NOIA_FRAME_TYPE_VERTICAL, scInvalidSurfaceId);
    noia_frame_append(bc, b);
    noia_frame_append(bc, c);
    noia_frame_append(abc, a);
    noia_frame_append(abc, bc);
    noia_frame_append(r, abc);
    noia_frame_append(r, d);

    p = noia_frame_find_contiguous(c, NOIA_ARGMAND_E, 1);
    NOIA_ASSERT(p == d, "1*East from C should be D");

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// When searching inside ABCDEF and point is inside ABC and ABC is stacked,
/// with A as most top frame, A should be found.
///
///
///     ┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓
///     ┃┌──────────┬─────────────┐┃
///     ┃│ ▛▀▀▀▀▀▀▜ │┌─────┬─────┐│┃
///     ┃│ ▌ ABC ×▐ ││  D  │  E  ││┃
///     ┃│ ▙▄▄▄▄▄▄▟ │└─────┴─────┘│┃
///     ┃└──────────┴─────────────┘┃
///     ┠──────────────────────────┨
///     ┃┌─────────────────┐       ┃
///     ┃│         F       │       ┃
///     ┃└─────────────────┘       ┃
///     ┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛
///
NoiaTestResult should_find_stacked_pointed_inside(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_POSITIONED;

    NoiaPosition point = {10, 10};
    NoiaFrame* p = noia_frame_find_pointed(r, point);
    NOIA_ASSERT(p == a, "Pointed inside ABC should be A");

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// When searching inside ABCDEF and point is inside D and D is inside flat
/// frame, D should be found.
///
///
///     ┏━━━━━━━━━━━━━━━━━━━━━━━━┓
///     ┃┌───────┬──────────────┐┃
///     ┃│┌─────┐│ ▛▀▀▀▀▀▜─────┐│┃
///     ┃││ ABC ││ ▌  D ×▐  E  ││┃
///     ┃│└─────┘│ ▙▄▄▄▄▄▟─────┘│┃
///     ┃└───────┴──────────────┘┃
///     ┠────────────────────────┨
///     ┃┌────────────────┐      ┃
///     ┃│        F       │      ┃
///     ┃└────────────────┘      ┃
///     ┗━━━━━━━━━━━━━━━━━━━━━━━━┛
///
NoiaTestResult should_find_flat_pointed_inside(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_POSITIONED;

    NoiaPosition point = {50, 10};
    NoiaFrame* p = noia_frame_find_pointed(r, point);

    NOIA_ASSERT(p == d, "Pointed inside D should be D");

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// When searching inside ABCDEF and point is outside ABCDEF and directly above
/// ABC and ABC is stacked, with A as most top frame, A should be found.
///
///
///           ×
///     ┏━━━━━━━━━━━━━━━━━━━━━━━━━┓
///     ┃┌─────────┬─────────────┐┃
///     ┃│ ▛▀▀▀▀▀▜ │┌─────┬─────┐│┃
///     ┃│ ▌ ABC ▐ ││  D  │  E  ││┃
///     ┃│ ▙▄▄▄▄▄▟ │└─────┴─────┘│┃
///     ┃└─────────┴─────────────┘┃
///     ┠─────────────────────────┨
///     ┃┌────────────────┐       ┃
///     ┃│        F       │       ┃
///     ┃└────────────────┘       ┃
///     ┗━━━━━━━━━━━━━━━━━━━━━━━━━┛
///
NoiaTestResult should_find_stacked_pointed_outside(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_POSITIONED;

    NoiaPosition point = {20, -10};
    NoiaFrame* p = noia_frame_find_pointed(r, point);

    NOIA_ASSERT(p == a, "Pointed outside ABCDEF above A should be A");

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// When searching inside ABCDEF and point is outside ABCDEF and directly above
/// D and D is inside flat frame, D should be found.
///
///
///                   ×
///     ┏━━━━━━━━━━━━━━━━━━━━━━━━┓
///     ┃┌───────┬──────────────┐┃
///     ┃│┌─────┐│ ▛▀▀▀▀▀▜─────┐│┃
///     ┃││ ABC ││ ▌  D  ▐  E  ││┃
///     ┃│└─────┘│ ▙▄▄▄▄▄▟─────┘│┃
///     ┃└───────┴──────────────┘┃
///     ┠────────────────────────┨
///     ┃┌────────────────┐      ┃
///     ┃│        F       │      ┃
///     ┃└────────────────┘      ┃
///     ┗━━━━━━━━━━━━━━━━━━━━━━━━┛
///
NoiaTestResult should_find_flat_pointed_outside(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_POSITIONED;

    NoiaPosition point = {60, -10};
    NoiaFrame* p = noia_frame_find_pointed(r, point);

    NOIA_ASSERT(p == d, "Pointed outside ABCDEF above D should be D");

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// When searching inside ABCDE and point is outside ABCDE and below D, but
/// inside another frame not contained in ABCDE, D should be found.
///
///
///     ┌────────────────────────┐
///     │┏━━━━━━━┯━━━━━━━━━━━━━━┓│
///     │┃┌─────┐│ ▛▀▀▀▀▀▜─────┐┃│
///     │┃│ ABC ││ ▌  D  ▐  E  │┃│
///     │┃└─────┘│ ▙▄▄▄▄▄▟─────┘┃│
///     │┗━━━━━━━┷━━━━━━━━━━━━━━┛│
///     ├────────────────────────┤
///     │┌────────────────┐      │
///     ││        F   ×   │      │
///     │└────────────────┘      │
///     └────────────────────────┘
///
NoiaTestResult should_find_frame_over_another(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_POSITIONED;

    NoiaPosition point = {50, 70};
    NoiaFrame* p = noia_frame_find_pointed(abcde, point);

    NOIA_ASSERT(p == d, "Pointed in F searching in ABCDE should be D");

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// When searching inside ABCDEF and point is outside of any frame contained in
/// ABCDEF, ABCDEF should be found.
///
///
///     ▐▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▌
///     ▐ ┌───────┬─────────────┐ ▌
///     ▐ │┌─────┐│┌─────┬─────┐│ ▌
///     ▐ ││ ABC │││  D  │  E  ││ ▌
///     ▐ │└─────┘│└─────┴─────┘│ ▌
///     ▐ └───────┴─────────────┘ ▌
///     ▐─────────────────────────▌
///     ▐ ┌────────────────┐      ▌
///     ▐ │        F       │  ×   ▌
///     ▐ └────────────────┘      ▌
///     ▐▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▌
///
NoiaTestResult should_find_frame_in_empty_space(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_POSITIONED;

    NoiaPosition point = {80, 80};
    NoiaFrame* p = noia_frame_find_pointed(r, point);

    NOIA_ASSERT(p == r, "Pointed in ABCDEF outside subframes should be ABCDEF");

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Find adjacent frames.
///
/// - 1*South from A should be C
/// - 1*South from C should be E
/// - 2*South from A should be F
/// - 1*South from CD should be F
///
///
///     ┌─────────────────────┐
///     │┌─────────────┬─────┐│
///     ││      A      │  B  ││
///     │└─────────────┴─────┘│
///     ├─────────────────────┤
///     │┌─────────┬─────────┐│
///     ││    C    │    D    ││
///     │└─────────┴─────────┘│
///     ├─────────────────────┤
///     │┌─────┬─────────────┐│
///     ││  E  │      F      ││
///     │└─────┴─────────────┘│
///     └─────────────────────┘
///
NoiaTestResult should_find_adjacent_frames(void)
{
    noia_mock_surface_manager_initialize();

    NoiaFrame* p  = NULL;
    NoiaFrame* r  = noia_frame_new();
    NoiaFrame* a  = noia_frame_new();
    NoiaFrame* b  = noia_frame_new();
    NoiaFrame* c  = noia_frame_new();
    NoiaFrame* d  = noia_frame_new();
    NoiaFrame* e  = noia_frame_new();
    NoiaFrame* f  = noia_frame_new();
    NoiaFrame* ab = noia_frame_new();
    NoiaFrame* cd = noia_frame_new();
    NoiaFrame* ef = noia_frame_new();
    noia_frame_append(ab, a);
    noia_frame_append(ab, b);
    noia_frame_append(cd, c);
    noia_frame_append(cd, d);
    noia_frame_append(ef, e);
    noia_frame_append(ef, f);
    noia_frame_append(r, ab);
    noia_frame_append(r, cd);
    noia_frame_append(r, ef);
    noia_frame_configure(a, NOIA_FRAME_TYPE_LEAF, 1, \
                         (NoiaPosition) { 0,  0}, (NoiaSize) { 70,  10}); \
    noia_frame_configure(b, NOIA_FRAME_TYPE_LEAF, 2, \
                         (NoiaPosition) {70,  0}, (NoiaSize) { 30,  10}); \
    noia_frame_configure(c, NOIA_FRAME_TYPE_LEAF, 3, \
                         (NoiaPosition) { 0, 10}, (NoiaSize) { 50,  10}); \
    noia_frame_configure(d, NOIA_FRAME_TYPE_LEAF, 4, \
                         (NoiaPosition) {50, 10}, (NoiaSize) { 50,  10}); \
    noia_frame_configure(e, NOIA_FRAME_TYPE_LEAF, 5, \
                         (NoiaPosition) { 0, 20}, (NoiaSize) { 30,  10}); \
    noia_frame_configure(f, NOIA_FRAME_TYPE_LEAF, 6, \
                         (NoiaPosition) {30, 20}, (NoiaSize) { 70,  10}); \
    noia_frame_configure(r, NOIA_FRAME_TYPE_VERTICAL, scInvalidSurfaceId, \
                         (NoiaPosition) { 0,  0}, (NoiaSize) {100, 30}); \
    noia_frame_configure(ab, NOIA_FRAME_TYPE_HORIZONTAL, scInvalidSurfaceId, \
                         (NoiaPosition) { 0,  0}, (NoiaSize) {100, 10}); \
    noia_frame_configure(cd, NOIA_FRAME_TYPE_HORIZONTAL, scInvalidSurfaceId,\
                         (NoiaPosition) { 0, 10}, (NoiaSize) {100, 10}); \
    noia_frame_configure(ef, NOIA_FRAME_TYPE_HORIZONTAL, scInvalidSurfaceId,\
                         (NoiaPosition) { 0, 20}, (NoiaSize) {100, 10});

    p = noia_frame_find_adjacent(a, NOIA_ARGMAND_S, 1);
    NOIA_ASSERT(p == c, "1*South from A should be C");

    p = noia_frame_find_adjacent(c, NOIA_ARGMAND_S, 1);
    NOIA_ASSERT(p == e, "1*South from C should be E");

    p = noia_frame_find_adjacent(a, NOIA_ARGMAND_S, 2);
    NOIA_ASSERT(p == f, "2*South from A should be F");

    p = noia_frame_find_adjacent(cd, NOIA_ARGMAND_S, 1);
    NOIA_ASSERT(p == f, "1*South from CD should be F");

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Empty test
NoiaTestResult should(void)
{
    noia_mock_surface_manager_initialize();

    NOIA_MAKE_FRAMES_POSITIONED;

    // ...

    noia_frame_free(r);

    noia_mock_surface_manager_finalize();
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    NOIA_INIT_TESTS;

    NoiaTest test[] = {
            NOIA_TEST(should),
            NOIA_TEST(should_append_and_prepend_values),
            NOIA_TEST(should_insert_values),
            NOIA_TEST(should_translate_frame_to_array),
            NOIA_TEST(should_translate_subframes_to_array),
            NOIA_TEST(should_remove_some_leaf_frames),
            NOIA_TEST(should_remove_frame_with_subframes),
            NOIA_TEST(should_resettle_one_frame),
            NOIA_TEST(should_resettle_frame_with_subframes),

            NOIA_TEST(should_find_with_sid),
            NOIA_TEST(should_find_contiguous_on_the_same_level_one_further),
            NOIA_TEST(should_find_contiguous_on_the_same_level_many_further),
            NOIA_TEST(should_find_contiguous_on_the_second_level),
            NOIA_TEST(should_find_contiguous_on_the_third_level),

            NOIA_TEST(should_find_stacked_pointed_inside),
            NOIA_TEST(should_find_flat_pointed_inside),
            NOIA_TEST(should_find_stacked_pointed_outside),
            NOIA_TEST(should_find_flat_pointed_outside),
            NOIA_TEST(should_find_frame_over_another),
            NOIA_TEST(should_find_frame_in_empty_space),

            NOIA_TEST(should_find_adjacent_frames),
        };

    return noia_test_run("Frame", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

