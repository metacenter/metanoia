// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "exhibitor-frame-internal.h"
#include "mock-surface-coordinator.h"
#include "tests-suit.h"

#include "surface-coordinator.h"
#include "global-macros.h"

#include <malloc.h>

#define INV scInvalidSurfaceId

//------------------------------------------------------------------------------

void noia_test_frame_config(NoiaFrame* frame,
                            NoiaCoordinator* coordinator,
                            NoiaFrameType type,
                            NoiaSurfaceId sid)
{
    NoiaArea area = {{0,0}, {0,0}};
    noia_frame_configure(frame, coordinator, type, sid, area, "");
}

//------------------------------------------------------------------------------

bool noia_test_streq(const char * s1, const char * s2)
{
    return ((s1 == NULL) and (s2 == NULL))
        or ((s1 != NULL) and (s2 != NULL) and (strcmp(s1, s2) == 0));
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
    noia_frame_configure(r,  co, NOIA_FRAME_TYPE_VERTICAL, INV, \
                         (NoiaArea) {{  0,  0}, {180, 60}}, "r"); \
    noia_frame_configure(v,  co, NOIA_FRAME_TYPE_VERTICAL, INV, \
                         (NoiaArea) {{  0,  0}, { 60, 60}}, "v"); \
    noia_frame_configure(h,  co, NOIA_FRAME_TYPE_HORIZONTAL, INV, \
                         (NoiaArea) {{ 60,  0}, { 60, 60}}, "h"); \
    noia_frame_configure(s,  co, NOIA_FRAME_TYPE_STACKED, INV, \
                         (NoiaArea) {{120,  0}, { 60, 60}}, "s"); \
    noia_frame_configure(v1, co, NOIA_FRAME_TYPE_LEAF, 11, \
                         (NoiaArea) {{  0,  0}, { 60, 12}}, "v1"); \
    noia_frame_configure(v2, co, NOIA_FRAME_TYPE_LEAF, 12, \
                         (NoiaArea) {{  0, 12}, { 60, 12}}, "v2"); \
    noia_frame_configure(v3, co, NOIA_FRAME_TYPE_LEAF, 13, \
                         (NoiaArea) {{  0, 24}, { 60, 12}}, "v3"); \
    noia_frame_configure(v4, co, NOIA_FRAME_TYPE_LEAF, 14, \
                         (NoiaArea) {{  0, 36}, { 60, 12}}, "v4"); \
    noia_frame_configure(v5, co, NOIA_FRAME_TYPE_LEAF, 15, \
                         (NoiaArea) {{  0, 48}, { 60, 12}}, "v5"); \
    noia_frame_configure(h1, co, NOIA_FRAME_TYPE_LEAF, 21, \
                         (NoiaArea) {{ 60,  0}, { 12, 60}}, "h1"); \
    noia_frame_configure(h2, co, NOIA_FRAME_TYPE_LEAF, 22, \
                         (NoiaArea) {{ 72,  0}, { 12, 60}}, "h2"); \
    noia_frame_configure(h3, co, NOIA_FRAME_TYPE_LEAF, 23, \
                         (NoiaArea) {{ 84,  0}, { 12, 60}}, "h3"); \
    noia_frame_configure(h4, co, NOIA_FRAME_TYPE_LEAF, 24, \
                         (NoiaArea) {{ 96,  0}, { 12, 60}}, "h4"); \
    noia_frame_configure(h5, co, NOIA_FRAME_TYPE_LEAF, 25, \
                         (NoiaArea) {{108,  0}, { 12, 60}}, "h5"); \
    noia_frame_configure(s1, co, NOIA_FRAME_TYPE_LEAF, 31, \
                         (NoiaArea) {{120,  0}, { 60, 60}}, "s1"); \
    noia_frame_configure(s2, co, NOIA_FRAME_TYPE_LEAF, 32, \
                         (NoiaArea) {{120,  0}, { 60, 60}}, "s2"); \
    noia_frame_configure(s3, co, NOIA_FRAME_TYPE_LEAF, 33, \
                         (NoiaArea) {{120,  0}, { 60, 60}}, "s3"); \
    noia_frame_configure(s4, co, NOIA_FRAME_TYPE_LEAF, 34, \
                         (NoiaArea) {{120,  0}, { 60, 60}}, "s4"); \
    noia_frame_configure(s5, co, NOIA_FRAME_TYPE_LEAF, 35, \
                         (NoiaArea) {{120,  0}, { 60, 60}}, "s5"); \
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

#define NOIA_MAKE_FRAMES_FOR_DERAMIFYING \
    NoiaFrame* r  = noia_frame_new(); \
    NoiaFrame* a1 = noia_frame_new(); \
    NoiaFrame* a2 = noia_frame_new(); \
    NoiaFrame* a3 = noia_frame_new(); \
    NoiaFrame* f = noia_frame_new(); \
    NoiaFrame* b  = noia_frame_new(); \
    NoiaFrame* c  = noia_frame_new(); \
    NoiaFrame* d1 = noia_frame_new(); \
    NoiaFrame* d2 = noia_frame_new(); \
    NoiaFrame* d3 = noia_frame_new(); \
    noia_test_frame_config(r,  co, NOIA_FRAME_TYPE_VERTICAL, INV); \
    noia_test_frame_config(a1, co, NOIA_FRAME_TYPE_STACKED,  INV); \
    noia_test_frame_config(a2, co, NOIA_FRAME_TYPE_VERTICAL, INV); \
    noia_test_frame_config(a3, co, NOIA_FRAME_TYPE_LEAF,     INV); \
    noia_test_frame_config(f,  co, NOIA_FRAME_TYPE_LEAF,  INV); \
    noia_test_frame_config(b,  co, NOIA_FRAME_TYPE_STACKED,  INV); \
    noia_test_frame_config(c,  co, NOIA_FRAME_TYPE_STACKED,  INV); \
    noia_test_frame_config(d1, co, NOIA_FRAME_TYPE_LEAF,     INV); \
    noia_test_frame_config(d2, co, NOIA_FRAME_TYPE_LEAF,     INV); \
    noia_test_frame_config(d3, co, NOIA_FRAME_TYPE_LEAF,     INV); \
    noia_frame_append(r,  a1); \
    noia_frame_append(r,  a2); \
    noia_frame_append(r,  a3); \
    noia_frame_append(a1, f); \
    noia_frame_append(a2, b); \
    noia_frame_append(b,  c); \
    noia_frame_append(c,  d1); \
    noia_frame_append(c,  d2); \
    noia_frame_append(c,  d3);

/// Frame set for testing search.
///
///
///     ┌───────────────────────┐
///     │┌───────┬─────────────┐│
///     ││┌─────┐│┌─────┬─────┐││
///     │││ ABC │││  D  │  E  │││
///     ││└─────┘│└─────┴─────┘││
///     │└───────┴─────────────┘│
///     ├───────────────────────┤
///     │┌────────────────┐     │
///     ││        F       │     │
///     │└────────────────┘     │
///     └───────────────────────┘
///
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
    noia_frame_configure(a, co, NOIA_FRAME_TYPE_LEAF, 1, \
                         (NoiaArea) {{ 0,  0}, { 40,  60}}, ""); \
    noia_frame_configure(b, co, NOIA_FRAME_TYPE_LEAF, 2, \
                         (NoiaArea) {{ 0,  0}, { 40,  60}}, ""); \
    noia_frame_configure(c, co, NOIA_FRAME_TYPE_LEAF, 3, \
                         (NoiaArea) {{ 0,  0}, { 40,  60}}, ""); \
    noia_frame_configure(d, co, NOIA_FRAME_TYPE_LEAF, 4, \
                         (NoiaArea) {{40,  0}, { 30,  60}}, ""); \
    noia_frame_configure(e, co, NOIA_FRAME_TYPE_LEAF, 5, \
                         (NoiaArea) {{40,  0}, { 30,  60}}, ""); \
    noia_frame_configure(f, co, NOIA_FRAME_TYPE_LEAF, 6, \
                         (NoiaArea) {{ 0, 60}, { 70,  60}}, ""); \
    noia_frame_configure(r, co, \
                         NOIA_FRAME_TYPE_VERTICAL | NOIA_FRAME_TYPE_SPECIAL, \
                         INV, (NoiaArea) {{ 0,  0}, {100, 120}}, ""); \
    noia_frame_configure(abc, co, NOIA_FRAME_TYPE_STACKED, INV, \
                         (NoiaArea) {{ 0,  0}, { 40,  60}}, ""); \
    noia_frame_configure(de, co, NOIA_FRAME_TYPE_HORIZONTAL, INV, \
                         (NoiaArea) {{40,  0}, { 60,  60}}, ""); \
    noia_frame_configure(abcde, co, NOIA_FRAME_TYPE_HORIZONTAL, INV, \
                         (NoiaArea) {{ 0,  0}, {100,  60}}, "");

/// Frame set for testing iteration.
///
///
///     ┌───────────────────────┐
///     │┌───────┬─────────────┐│
///     ││       │┌─────┬─────┐││
///     ││   A   ││ BCD │  E  │││
///     ││       │└─────┴─────┘││
///     │└───────┴─────────────┘│
///     ├───────────────────────┤
///     │┌─────────────────────┐│
///     ││          F          ││
///     │├─────────────────────┤│
///     ││         GHI         ││
///     │└─────────────────────┘│
///     └───────────────────────┘
///
#define NOIA_MAKE_FRAMES_FOR_JUMPING \
    NoiaFrame* r     = noia_frame_new(); \
    NoiaFrame* a     = noia_frame_new(); \
    NoiaFrame* b     = noia_frame_new(); \
    NoiaFrame* c     = noia_frame_new(); \
    NoiaFrame* d     = noia_frame_new(); \
    NoiaFrame* e     = noia_frame_new(); \
    NoiaFrame* f     = noia_frame_new(); \
    NoiaFrame* g     = noia_frame_new(); \
    NoiaFrame* h     = noia_frame_new(); \
    NoiaFrame* i     = noia_frame_new(); \
    NoiaFrame* bcd   = noia_frame_new(); \
    NoiaFrame* bcde  = noia_frame_new(); \
    NoiaFrame* abcde = noia_frame_new(); \
    NoiaFrame* ghi   = noia_frame_new(); \
    NoiaFrame* fghi  = noia_frame_new(); \
    noia_frame_append(bcd,   b); \
    noia_frame_append(bcd,   c); \
    noia_frame_append(bcd,   d); \
    noia_frame_append(bcde,  bcd); \
    noia_frame_append(bcde,  e); \
    noia_frame_append(abcde, a); \
    noia_frame_append(abcde, bcde); \
    noia_frame_append(ghi,   g); \
    noia_frame_append(ghi,   h); \
    noia_frame_append(ghi,   i); \
    noia_frame_append(fghi,  f); \
    noia_frame_append(fghi,  ghi); \
    noia_frame_append(r,     abcde); \
    noia_frame_append(r,     fghi); \
    noia_frame_configure(a, co, NOIA_FRAME_TYPE_LEAF, 1, \
                         (NoiaArea) {{ 0,  0}, { 40,  40}}, ""); \
    noia_frame_configure(b, co, NOIA_FRAME_TYPE_LEAF, 2, \
                         (NoiaArea) {{40,  0}, { 40,  40}}, ""); \
    noia_frame_configure(c, co, NOIA_FRAME_TYPE_LEAF, 3, \
                         (NoiaArea) {{40,  0}, { 40,  40}}, ""); \
    noia_frame_configure(d, co, NOIA_FRAME_TYPE_LEAF, 4, \
                         (NoiaArea) {{40,  0}, { 40,  40}}, ""); \
    noia_frame_configure(e, co, NOIA_FRAME_TYPE_LEAF, 5, \
                         (NoiaArea) {{80,  0}, { 40,  40}}, ""); \
    noia_frame_configure(f, co, NOIA_FRAME_TYPE_LEAF, 6, \
                         (NoiaArea) {{ 0, 40}, {120,  40}}, ""); \
    noia_frame_configure(g, co, NOIA_FRAME_TYPE_LEAF, 7, \
                         (NoiaArea) {{ 0, 80}, {120,  40}}, ""); \
    noia_frame_configure(h, co, NOIA_FRAME_TYPE_LEAF, 8, \
                         (NoiaArea) {{ 0, 80}, {120,  40}}, ""); \
    noia_frame_configure(i, co, NOIA_FRAME_TYPE_LEAF, 9, \
                         (NoiaArea) {{ 0, 80}, {120,  40}}, ""); \
    noia_frame_configure(bcd, co, NOIA_FRAME_TYPE_STACKED, INV, \
                         (NoiaArea) {{40,  0}, { 40,  40}}, ""); \
    noia_frame_configure(bcde, co, NOIA_FRAME_TYPE_HORIZONTAL, INV,\
                         (NoiaArea) {{40,  0}, { 80,  40}}, ""); \
    noia_frame_configure(abcde, co, NOIA_FRAME_TYPE_HORIZONTAL, INV,\
                         (NoiaArea) {{ 0,  0}, {120,  40}}, ""); \
    noia_frame_configure(ghi, co, NOIA_FRAME_TYPE_STACKED, INV,\
                         (NoiaArea) {{ 0, 80}, {120,  40}}, ""); \
    noia_frame_configure(fghi, co, NOIA_FRAME_TYPE_VERTICAL, INV,\
                         (NoiaArea) {{ 0, 40}, {120,  80}}, ""); \
    noia_frame_configure(r, co, \
                         NOIA_FRAME_TYPE_VERTICAL | NOIA_FRAME_TYPE_SPECIAL, \
                         INV, (NoiaArea) {{ 0,  0}, {120, 120}}, "");

/// Frame set for testing resizing with horizontal floating.
///
///
///     ┌───────────────┬─────┬─────────────┐
///     │┌─────────────┐│     │┌─────┬─────┐│
///     ││      A      ││     ││     │     ││
///     │├─────────────┤│     ││     │     ││
///     ││     BCD     ││     ││     │     ││
///     │├─────────────┤│  G  ││  H  │  I  ││
///     ││┌─────┬─────┐││     ││     │     ││
///     │││  E  │  F  │││     ││     │     ││
///     ││└─────┴─────┘││     ││     │     ││
///     │└─────────────┘│     │└─────┴─────┘│
///     └───────────────┴─────┴─────────────┘
///
#define NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_HORIZONTAL_FLOATING \
    NoiaFrame* r      = noia_frame_new(); \
    NoiaFrame* a      = noia_frame_new(); \
    NoiaFrame* b      = noia_frame_new(); \
    NoiaFrame* c      = noia_frame_new(); \
    NoiaFrame* d      = noia_frame_new(); \
    NoiaFrame* e      = noia_frame_new(); \
    NoiaFrame* f      = noia_frame_new(); \
    NoiaFrame* g      = noia_frame_new(); \
    NoiaFrame* h      = noia_frame_new(); \
    NoiaFrame* i      = noia_frame_new(); \
    NoiaFrame* bcd    = noia_frame_new(); \
    NoiaFrame* ef     = noia_frame_new(); \
    NoiaFrame* abcdef = noia_frame_new(); \
    NoiaFrame* hi     = noia_frame_new(); \
    noia_frame_append(bcd,    b); \
    noia_frame_append(bcd,    c); \
    noia_frame_append(bcd,    d); \
    noia_frame_append(ef,     e); \
    noia_frame_append(ef,     f); \
    noia_frame_append(abcdef, a); \
    noia_frame_append(abcdef, bcd); \
    noia_frame_append(abcdef, ef); \
    noia_frame_append(hi,     h); \
    noia_frame_append(hi,     i); \
    noia_frame_append(r,      abcdef); \
    noia_frame_append(r,      g); \
    noia_frame_append(r,      hi); \
    noia_frame_configure(a, co, NOIA_FRAME_TYPE_LEAF, 1, \
                         (NoiaArea) {{  0,  0}, {120,  40}}, ""); \
    noia_frame_configure(b, co, NOIA_FRAME_TYPE_LEAF, 2, \
                         (NoiaArea) {{  0, 40}, {120,  40}}, ""); \
    noia_frame_configure(c, co, NOIA_FRAME_TYPE_LEAF, 3, \
                         (NoiaArea) {{  0, 40}, {120,  40}}, ""); \
    noia_frame_configure(d, co, NOIA_FRAME_TYPE_LEAF, 4, \
                         (NoiaArea) {{  0, 40}, {120,  40}}, ""); \
    noia_frame_configure(e, co, NOIA_FRAME_TYPE_LEAF, 5, \
                         (NoiaArea) {{  0, 80}, { 60,  40}}, ""); \
    noia_frame_configure(f, co, NOIA_FRAME_TYPE_LEAF, 6, \
                         (NoiaArea) {{ 60, 80}, { 60,  40}}, ""); \
    noia_frame_configure(g, co, NOIA_FRAME_TYPE_LEAF, 7, \
                         (NoiaArea) {{120,  0}, { 60, 120}}, ""); \
    noia_frame_configure(h, co, NOIA_FRAME_TYPE_LEAF, 8, \
                         (NoiaArea) {{180,  0}, { 60, 120}}, ""); \
    noia_frame_configure(i, co, NOIA_FRAME_TYPE_LEAF, 9, \
                         (NoiaArea) {{240,  0}, { 60, 120}}, ""); \
    noia_frame_configure(bcd, co, NOIA_FRAME_TYPE_STACKED, INV, \
                         (NoiaArea) {{  0, 40}, {120,  40}}, ""); \
    noia_frame_configure(ef, co, NOIA_FRAME_TYPE_HORIZONTAL, INV, \
                         (NoiaArea) {{  0, 80}, {120,  40}}, ""); \
    noia_frame_configure(abcdef, co, NOIA_FRAME_TYPE_VERTICAL, INV, \
                         (NoiaArea) {{  0,  0}, {120, 120}}, ""); \
    noia_frame_configure(hi, co, NOIA_FRAME_TYPE_HORIZONTAL, INV, \
                         (NoiaArea) {{180,  0}, {120, 120}}, ""); \
    noia_frame_configure(r, co, \
                         NOIA_FRAME_TYPE_HORIZONTAL | NOIA_FRAME_TYPE_FLOATING,\
                         INV, (NoiaArea) {{  0,  0}, {300, 120}}, ""); \

/// Frame set for testing resizing with vertical floating.
///
///
///     ┌─────────────────────┐
///     │┌─────┬─────┬───────┐│
///     ││     │     │┌─────┐││
///     ││     │     ││  E  │││
///     ││  A  │ BCD │├─────┤││
///     ││     │     ││  F  │││
///     ││     │     │└─────┘││
///     │└─────┴─────┴───────┘│
///     ├─────────────────────┤
///     │          G          │
///     ├─────────────────────┤
///     │┌───────────────────┐│
///     ││         H         ││
///     │├───────────────────┤│
///     ││         I         ││
///     │└───────────────────┘│
///     └─────────────────────┘
///
#define NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_VERTICAL_FLOATING \
    NoiaFrame* r      = noia_frame_new(); \
    NoiaFrame* a      = noia_frame_new(); \
    NoiaFrame* b      = noia_frame_new(); \
    NoiaFrame* c      = noia_frame_new(); \
    NoiaFrame* d      = noia_frame_new(); \
    NoiaFrame* e      = noia_frame_new(); \
    NoiaFrame* f      = noia_frame_new(); \
    NoiaFrame* g      = noia_frame_new(); \
    NoiaFrame* h      = noia_frame_new(); \
    NoiaFrame* i      = noia_frame_new(); \
    NoiaFrame* bcd    = noia_frame_new(); \
    NoiaFrame* ef     = noia_frame_new(); \
    NoiaFrame* abcdef = noia_frame_new(); \
    NoiaFrame* hi     = noia_frame_new(); \
    noia_frame_append(bcd,    b); \
    noia_frame_append(bcd,    c); \
    noia_frame_append(bcd,    d); \
    noia_frame_append(ef,     e); \
    noia_frame_append(ef,     f); \
    noia_frame_append(abcdef, a); \
    noia_frame_append(abcdef, bcd); \
    noia_frame_append(abcdef, ef); \
    noia_frame_append(hi,     h); \
    noia_frame_append(hi,     i); \
    noia_frame_append(r,      abcdef); \
    noia_frame_append(r,      g); \
    noia_frame_append(r,      hi); \
    noia_frame_configure(a, co, NOIA_FRAME_TYPE_LEAF, 1, \
                         (NoiaArea) {{  0,   0}, { 40, 120}}, "");\
    noia_frame_configure(b, co, NOIA_FRAME_TYPE_LEAF, 2, \
                         (NoiaArea) {{ 40,   0}, { 40, 120}}, "");\
    noia_frame_configure(c, co, NOIA_FRAME_TYPE_LEAF, 3, \
                         (NoiaArea) {{ 40,   0}, { 40, 120}}, "");\
    noia_frame_configure(d, co, NOIA_FRAME_TYPE_LEAF, 4, \
                         (NoiaArea) {{ 40,   0}, { 40, 120}}, "");\
    noia_frame_configure(e, co, NOIA_FRAME_TYPE_LEAF, 5, \
                         (NoiaArea) {{ 80,   0}, { 40,  60}}, "");\
    noia_frame_configure(f, co, NOIA_FRAME_TYPE_LEAF, 6, \
                         (NoiaArea) {{ 80,  60}, { 40,  60}}, "");\
    noia_frame_configure(g, co, NOIA_FRAME_TYPE_LEAF, 7, \
                         (NoiaArea) {{  0, 120}, {120,  60}}, "");\
    noia_frame_configure(h, co, NOIA_FRAME_TYPE_LEAF, 8, \
                         (NoiaArea) {{  0, 180}, {120,  60}}, "");\
    noia_frame_configure(i, co, NOIA_FRAME_TYPE_LEAF, 9, \
                         (NoiaArea) {{  0, 240}, {120,  60}}, "");\
    noia_frame_configure(bcd, co, NOIA_FRAME_TYPE_STACKED, INV, \
                         (NoiaArea) {{ 40,   0}, { 40, 120}}, "");\
    noia_frame_configure(ef, co, NOIA_FRAME_TYPE_VERTICAL, INV, \
                         (NoiaArea) {{ 80,   0}, { 40, 120}}, "");\
    noia_frame_configure(abcdef, co, NOIA_FRAME_TYPE_HORIZONTAL, INV,\
                         (NoiaArea) {{  0,   0}, {120, 120}}, "");\
    noia_frame_configure(hi, co, NOIA_FRAME_TYPE_VERTICAL, INV, \
                         (NoiaArea) {{  0, 180}, {120, 120}}, "");\
    noia_frame_configure(r, co, \
                         NOIA_FRAME_TYPE_VERTICAL | NOIA_FRAME_TYPE_FLOATING, \
                         INV, (NoiaArea) {{  0,   0}, {120, 300}}, "");

/// Frame set for testing relaxing.
/// The same layout as frame set for testing resizing with horizontal
/// floating but with different frame sizes.
#define NOIA_MAKE_FRAMES_FOR_RELAXING \
    NoiaFrame* r      = noia_frame_new(); \
    NoiaFrame* a      = noia_frame_new(); \
    NoiaFrame* b      = noia_frame_new(); \
    NoiaFrame* c      = noia_frame_new(); \
    NoiaFrame* d      = noia_frame_new(); \
    NoiaFrame* e      = noia_frame_new(); \
    NoiaFrame* f      = noia_frame_new(); \
    NoiaFrame* g      = noia_frame_new(); \
    NoiaFrame* h      = noia_frame_new(); \
    NoiaFrame* i      = noia_frame_new(); \
    NoiaFrame* bcd    = noia_frame_new(); \
    NoiaFrame* ef     = noia_frame_new(); \
    NoiaFrame* abcdef = noia_frame_new(); \
    NoiaFrame* hi     = noia_frame_new(); \
    noia_frame_append(bcd,    b); \
    noia_frame_append(bcd,    c); \
    noia_frame_append(bcd,    d); \
    noia_frame_append(ef,     e); \
    noia_frame_append(ef,     f); \
    noia_frame_append(abcdef, a); \
    noia_frame_append(abcdef, bcd); \
    noia_frame_append(abcdef, ef); \
    noia_frame_append(hi,     h); \
    noia_frame_append(hi,     i); \
    noia_frame_append(r,      abcdef); \
    noia_frame_append(r,      g); \
    noia_frame_append(r,      hi); \
    noia_frame_configure(a, co, NOIA_FRAME_TYPE_LEAF, 1, \
                         (NoiaArea) {{  0,   0}, {180, 120}}, "");\
    noia_frame_configure(b, co, NOIA_FRAME_TYPE_LEAF, 2, \
                         (NoiaArea) {{  0, 120}, {180, 180}}, "");\
    noia_frame_configure(c, co, NOIA_FRAME_TYPE_LEAF, 3, \
                         (NoiaArea) {{  0, 120}, {180, 180}}, "");\
    noia_frame_configure(d, co, NOIA_FRAME_TYPE_LEAF, 4, \
                         (NoiaArea) {{  0, 120}, {180, 180}}, "");\
    noia_frame_configure(e, co, NOIA_FRAME_TYPE_LEAF, 5, \
                         (NoiaArea) {{  0, 300}, { 60,  60}}, "");\
    noia_frame_configure(f, co, NOIA_FRAME_TYPE_LEAF, 6, \
                         (NoiaArea) {{ 60, 300}, {120,  60}}, "");\
    noia_frame_configure(g, co, NOIA_FRAME_TYPE_LEAF, 7, \
                         (NoiaArea) {{180,   0}, { 60, 360}}, "");\
    noia_frame_configure(h, co, NOIA_FRAME_TYPE_LEAF, 8, \
                         (NoiaArea) {{240,   0}, { 80, 360}}, "");\
    noia_frame_configure(i, co, NOIA_FRAME_TYPE_LEAF, 9, \
                         (NoiaArea) {{320,   0}, { 40, 360}}, "");\
    noia_frame_configure(bcd, co, NOIA_FRAME_TYPE_STACKED, INV, \
                         (NoiaArea) {{  0, 120}, {180, 180}}, "");\
    noia_frame_configure(ef, co, NOIA_FRAME_TYPE_HORIZONTAL, INV, \
                         (NoiaArea) {{  0, 300}, {180,  60}}, "");\
    noia_frame_configure(abcdef, co, NOIA_FRAME_TYPE_VERTICAL, INV, \
                         (NoiaArea) {{  0,   0}, {180, 360}}, "");\
    noia_frame_configure(hi, co, NOIA_FRAME_TYPE_HORIZONTAL, INV, \
                         (NoiaArea) {{240,   0}, {120, 360}}, "");\
    noia_frame_configure(r, co, \
                         NOIA_FRAME_TYPE_HORIZONTAL | NOIA_FRAME_TYPE_FLOATING,\
                         INV, (NoiaArea) {{  0,   0}, {360, 360}}, "");


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
                    "Element should be {'%lu', {'%u', '%u'}} " \
                    "(is {'%lu', {'%u', '%u'}})", \
                    c1->sid, c1->pos.x, c1->pos.y, \
                    c2->sid, c2->pos.x, c2->pos.y); }

#define NOIA_ASSERT_FRAME_POINTER(FRAME1, FRAME2) \
    NOIA_ASSERT(FRAME1 == FRAME2, \
                "Frame should be '%p' (is '%p')", \
                (void*) FRAME2, (void*) FRAME1); \

#define NOIA_ASSERT_FRAME_ITERATOR(ITER, FRAME, POSITION) \
    NOIA_ASSERT_FRAME_POINTER(ITER.frame, FRAME); \
    NOIA_ASSERT(ITER.position == POSITION, \
                "Position should be '%d'` (is '%d')", \
                POSITION, ITER.position);

#define NOIA_ASSERT_FRAME_AREA(FRAME, X, Y, W, H) { \
    NoiaArea area = noia_frame_get_area(FRAME); \
    NOIA_ASSERT(((area.pos.x      == X) and (area.pos.y       == Y) \
             and (area.size.width == W) and (area.size.height == H)), \
                "Frame size should be {x='%d', y='%d', w='%d', h='%d'} " \
                "(is {x='%d', y='%d', w='%d', h='%d'})", \
                X, Y, W, H, area.pos.x, area.pos.y, \
                area.size.width, area.size.height); }

#define NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(FRAME, X, Y, W, H) { \
    NoiaArea area = noia_frame_get_area(FRAME); \
    NoiaSurfaceId sid = noia_frame_get_sid(FRAME); \
    NoiaSize size = noia_surface_get_desired_size(co, sid); \
    NOIA_ASSERT(((area.pos.x      == X) and (area.pos.y       == Y) \
             and (area.size.width == W) and (area.size.height == H) \
             and (size.width      == W) and (size.height      == H)), \
                "Frame size should be {x='%d', y='%d', w='%d', h='%d'} " \
                "(is {x='%d', y='%d', w='%d', h='%d'}; " \
                "surface size is {w='%d', h='%d'})", \
                X, Y, W, H, area.pos.x, area.pos.y, \
                area.size.width, area.size.height, size.width, size.height); }

#define NOIA_ASSERT_FRAME_PARAMETERS(FRAME, SID, TYPE, X, Y, W, H, TITLE) { \
    NOIA_ASSERT((FRAME->sid              == SID) \
            and (FRAME->type             == TYPE) \
            and (FRAME->area.pos.x       == X) \
            and (FRAME->area.pos.y       == Y) \
            and (FRAME->area.size.width  == W) \
            and (FRAME->area.size.height == H) \
            and (noia_test_streq(FRAME->title, TITLE)), \
                "Frame should be {sid='%lu', type='0x%04x', x='%d', y='%d', " \
                "width='%d', height='%d', title='%s'} (is {sid='%lu', " \
                "type='0x%04x', x='%d', y='%d', width='%d', height='%d', " \
                "title='%s'})", SID, TYPE, X, Y, W, H, TITLE, \
                FRAME->sid, FRAME->type, \
                FRAME->area.pos.x, FRAME->area.pos.y, \
                FRAME->area.size.width, FRAME->area.size.height, \
                FRAME->title); }

#define NOIA_ASSERT_FRAME_TRUNK(FRAME, TRUNK) { \
    NoiaFrame* trunk = noia_frame_get_trunk(FRAME); \
    NOIA_ASSERT(trunk == (TRUNK), \
                "Trunk of %p should be '%p', (is '%p')", \
                (void*) FRAME, (void*) TRUNK, (void*) trunk); }

#define NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(FRAME, NUM) \
    NOIA_ASSERT_CHAIN_LEN(((FRAME)->branch.spatial_twigs), NUM); \
    NOIA_ASSERT_CHAIN_LEN(((FRAME)->branch.temporal_twigs), NUM);

//------------------------------------------------------------------------------

/// Check if appended and prepended frames are assigned to correct trunk.
/// Content check will be done in `should_translate_to_array`
NoiaTestResult should_append_and_prepend_values(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_SIMPLE;

    NOIA_ASSERT_FRAME_TRUNK(r,  NULL);
    NOIA_ASSERT_FRAME_TRUNK(h,  r);
    NOIA_ASSERT_FRAME_TRUNK(v,  r);
    NOIA_ASSERT_FRAME_TRUNK(s,  r);
    NOIA_ASSERT_FRAME_TRUNK(v1, v);
    NOIA_ASSERT_FRAME_TRUNK(v2, v);
    NOIA_ASSERT_FRAME_TRUNK(v3, v);
    NOIA_ASSERT_FRAME_TRUNK(v4, v);
    NOIA_ASSERT_FRAME_TRUNK(v5, v);
    NOIA_ASSERT_FRAME_TRUNK(h1, h);
    NOIA_ASSERT_FRAME_TRUNK(h2, h);
    NOIA_ASSERT_FRAME_TRUNK(h3, h);
    NOIA_ASSERT_FRAME_TRUNK(h4, h);
    NOIA_ASSERT_FRAME_TRUNK(h5, h);
    NOIA_ASSERT_FRAME_TRUNK(s1, s);
    NOIA_ASSERT_FRAME_TRUNK(s2, s);
    NOIA_ASSERT_FRAME_TRUNK(s3, s);
    NOIA_ASSERT_FRAME_TRUNK(s4, s);
    NOIA_ASSERT_FRAME_TRUNK(s5, s);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r, 3u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(v, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(h, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(s, 5u);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check if inserted frames are assigned to correct trunk.
NoiaTestResult should_insert_values(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NoiaFrame* r  = noia_frame_new();
    NoiaFrame* f1 = noia_frame_new();
    NoiaFrame* f2 = noia_frame_new();
    NoiaFrame* f3 = noia_frame_new();
    NoiaFrame* f4 = noia_frame_new();
    NoiaFrame* f5 = noia_frame_new();
    noia_test_frame_config(r,  co, NOIA_FRAME_TYPE_NONE, scInvalidSurfaceId);
    noia_test_frame_config(f1, co, NOIA_FRAME_TYPE_LEAF, 1);
    noia_test_frame_config(f2, co, NOIA_FRAME_TYPE_LEAF, 2);
    noia_test_frame_config(f3, co, NOIA_FRAME_TYPE_LEAF, 3);
    noia_test_frame_config(f4, co, NOIA_FRAME_TYPE_LEAF, 4);
    noia_test_frame_config(f5, co, NOIA_FRAME_TYPE_LEAF, 5);
    noia_frame_append(r, f2);
    noia_frame_insert_after(f2, f4);
    noia_frame_insert_before(f4, f3);
    noia_frame_insert_after(f4, f5);
    noia_frame_insert_before(f2, f1);

    NOIA_ASSERT_FRAME_TRUNK(f1, r);
    NOIA_ASSERT_FRAME_TRUNK(f2, r);
    NOIA_ASSERT_FRAME_TRUNK(f3, r);
    NOIA_ASSERT_FRAME_TRUNK(f4, r);
    NOIA_ASSERT_FRAME_TRUNK(f5, r);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r, 5u);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check order and position when translating whole frame to array.
/// Array is created in temporal order.
NoiaTestResult should_translate_root_frame_to_array(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();

    NOIA_MAKE_FRAMES_SIMPLE;

    NoiaSurfaceContext a[] = {
            {35, {120,  0}},
            {31, {120,  0}},
            {34, {120,  0}},
            {32, {120,  0}},
            {33, {120,  0}},
            {15, {  0, 48}},
            {11, {  0,  0}},
            {14, {  0, 36}},
            {12, {  0, 12}},
            {13, {  0, 24}},
            {25, {108,  0}},
            {21, { 60,  0}},
            {24, { 96,  0}},
            {22, { 72,  0}},
            {23, { 84,  0}},
        };

    NoiaPool* pool = noia_pool_create(NOIA_SIZEOF_ARRAY(a),
                                      sizeof(NoiaSurfaceContext));

    noia_frame_to_array(r, pool, co);
    noia_frame_free(r);

    NOIA_ASSERT_FRAME_ARRAY(a, pool);

    noia_pool_destroy(pool);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check order and position when translating subframes to array.
NoiaTestResult should_translate_twig_frame_to_array(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();

    NOIA_MAKE_FRAMES_SIMPLE;

    NoiaSurfaceContext av[] = {
            {15, {0, 48}},
            {11, {0,  0}},
            {14, {0, 36}},
            {12, {0, 12}},
            {13, {0, 24}},
        };
    NoiaSurfaceContext ah[] = {
            {25, {108, 0}},
            {21, { 60, 0}},
            {24, { 96, 0}},
            {22, { 72, 0}},
            {23, { 84, 0}},
        };
    NoiaSurfaceContext as[] = {
            {35, {120, 0}},
            {31, {120, 0}},
            {34, {120, 0}},
            {32, {120, 0}},
            {33, {120, 0}},
        };

    NoiaPool* pv = noia_pool_create(NOIA_SIZEOF_ARRAY(ah),
                                    sizeof(NoiaSurfaceContext));
    NoiaPool* ph = noia_pool_create(NOIA_SIZEOF_ARRAY(av),
                                    sizeof(NoiaSurfaceContext));
    NoiaPool* ps = noia_pool_create(NOIA_SIZEOF_ARRAY(as),
                                    sizeof(NoiaSurfaceContext));

    noia_frame_to_array(v, pv, co);
    noia_frame_to_array(h, ph, co);
    noia_frame_to_array(s, ps, co);
    noia_frame_free(r);

    NOIA_ASSERT_FRAME_ARRAY(av, pv);
    NOIA_ASSERT_FRAME_ARRAY(ah, ph);
    NOIA_ASSERT_FRAME_ARRAY(as, ps);

    noia_pool_destroy(pv);
    noia_pool_destroy(ph);
    noia_pool_destroy(ps);

    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check correctness of frame after removing several leafs.
NoiaTestResult should_remove_some_leaf_frames(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();

    NOIA_MAKE_FRAMES_SIMPLE;

    noia_frame_remove_self(v1, co);
    noia_frame_remove_self(v2, co);
    noia_frame_remove_self(v3, co);
    noia_frame_remove_self(h5, co);
    noia_frame_remove_self(h4, co);
    noia_frame_remove_self(h3, co);
    noia_frame_remove_self(s3, co);
    noia_frame_remove_self(s4, co);
    noia_frame_remove_self(s2, co);

    NOIA_ASSERT_FRAME_TRUNK(r,  NULL);
    NOIA_ASSERT_FRAME_TRUNK(h,  r);
    NOIA_ASSERT_FRAME_TRUNK(v,  r);
    NOIA_ASSERT_FRAME_TRUNK(s,  r);
    NOIA_ASSERT_FRAME_TRUNK(v1, NULL);
    NOIA_ASSERT_FRAME_TRUNK(v2, NULL);
    NOIA_ASSERT_FRAME_TRUNK(v3, NULL);
    NOIA_ASSERT_FRAME_TRUNK(v4, v);
    NOIA_ASSERT_FRAME_TRUNK(v5, v);
    NOIA_ASSERT_FRAME_TRUNK(h1, h);
    NOIA_ASSERT_FRAME_TRUNK(h2, h);
    NOIA_ASSERT_FRAME_TRUNK(h3, NULL);
    NOIA_ASSERT_FRAME_TRUNK(h4, NULL);
    NOIA_ASSERT_FRAME_TRUNK(h5, NULL);
    NOIA_ASSERT_FRAME_TRUNK(s1, s);
    NOIA_ASSERT_FRAME_TRUNK(s2, NULL);
    NOIA_ASSERT_FRAME_TRUNK(s3, NULL);
    NOIA_ASSERT_FRAME_TRUNK(s4, NULL);
    NOIA_ASSERT_FRAME_TRUNK(s5, s);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r, 3u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(v, 2u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(h, 2u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(s, 2u);

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

    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check correctness of frame after removing frame with leafs.
NoiaTestResult should_remove_frame_with_subframes(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();

    NOIA_MAKE_FRAMES_SIMPLE;

    noia_frame_remove_self(h, co);

    NOIA_ASSERT_FRAME_TRUNK(r,  NULL);
    NOIA_ASSERT_FRAME_TRUNK(h,  NULL);
    NOIA_ASSERT_FRAME_TRUNK(v,  r);
    NOIA_ASSERT_FRAME_TRUNK(s,  r);
    NOIA_ASSERT_FRAME_TRUNK(v1, v);
    NOIA_ASSERT_FRAME_TRUNK(v2, v);
    NOIA_ASSERT_FRAME_TRUNK(v3, v);
    NOIA_ASSERT_FRAME_TRUNK(v4, v);
    NOIA_ASSERT_FRAME_TRUNK(v5, v);
    NOIA_ASSERT_FRAME_TRUNK(h1, h);
    NOIA_ASSERT_FRAME_TRUNK(h2, h);
    NOIA_ASSERT_FRAME_TRUNK(h3, h);
    NOIA_ASSERT_FRAME_TRUNK(h4, h);
    NOIA_ASSERT_FRAME_TRUNK(h5, h);
    NOIA_ASSERT_FRAME_TRUNK(s1, s);
    NOIA_ASSERT_FRAME_TRUNK(s2, s);
    NOIA_ASSERT_FRAME_TRUNK(s3, s);
    NOIA_ASSERT_FRAME_TRUNK(s4, s);
    NOIA_ASSERT_FRAME_TRUNK(s5, s);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r, 2u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(v, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(h, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(s, 5u);

    noia_frame_free(h);
    noia_frame_free(r);

    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Resettle one frame without children and check if trunks are OK.
NoiaTestResult should_resettle_one_frame(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();

    NOIA_MAKE_FRAMES_SIMPLE;

    noia_frame_resettle(h3, s, co);

    NOIA_ASSERT_FRAME_TRUNK(r,  NULL);
    NOIA_ASSERT_FRAME_TRUNK(h,  r);
    NOIA_ASSERT_FRAME_TRUNK(v,  r);
    NOIA_ASSERT_FRAME_TRUNK(s,  r);
    NOIA_ASSERT_FRAME_TRUNK(v1, v);
    NOIA_ASSERT_FRAME_TRUNK(v2, v);
    NOIA_ASSERT_FRAME_TRUNK(v3, v);
    NOIA_ASSERT_FRAME_TRUNK(v4, v);
    NOIA_ASSERT_FRAME_TRUNK(v5, v);
    NOIA_ASSERT_FRAME_TRUNK(h1, h);
    NOIA_ASSERT_FRAME_TRUNK(h2, h);
    NOIA_ASSERT_FRAME_TRUNK(h3, s);
    NOIA_ASSERT_FRAME_TRUNK(h4, h);
    NOIA_ASSERT_FRAME_TRUNK(h5, h);
    NOIA_ASSERT_FRAME_TRUNK(s1, s);
    NOIA_ASSERT_FRAME_TRUNK(s2, s);
    NOIA_ASSERT_FRAME_TRUNK(s3, s);
    NOIA_ASSERT_FRAME_TRUNK(s4, s);
    NOIA_ASSERT_FRAME_TRUNK(s5, s);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r, 3u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(v, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(h, 4u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(s, 6u);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Resettle one frame with children and check if trunks are OK.
NoiaTestResult should_resettle_frame_with_subframes(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();

    NOIA_MAKE_FRAMES_SIMPLE;

    noia_frame_resettle(h, s, co);

    NOIA_ASSERT_FRAME_TRUNK(r,  NULL);
    NOIA_ASSERT_FRAME_TRUNK(h,  s);
    NOIA_ASSERT_FRAME_TRUNK(v,  r);
    NOIA_ASSERT_FRAME_TRUNK(s,  r);
    NOIA_ASSERT_FRAME_TRUNK(v1, v);
    NOIA_ASSERT_FRAME_TRUNK(v2, v);
    NOIA_ASSERT_FRAME_TRUNK(v3, v);
    NOIA_ASSERT_FRAME_TRUNK(v4, v);
    NOIA_ASSERT_FRAME_TRUNK(v5, v);
    NOIA_ASSERT_FRAME_TRUNK(h1, h);
    NOIA_ASSERT_FRAME_TRUNK(h2, h);
    NOIA_ASSERT_FRAME_TRUNK(h3, h);
    NOIA_ASSERT_FRAME_TRUNK(h4, h);
    NOIA_ASSERT_FRAME_TRUNK(h5, h);
    NOIA_ASSERT_FRAME_TRUNK(s1, s);
    NOIA_ASSERT_FRAME_TRUNK(s2, s);
    NOIA_ASSERT_FRAME_TRUNK(s3, s);
    NOIA_ASSERT_FRAME_TRUNK(s4, s);
    NOIA_ASSERT_FRAME_TRUNK(s5, s);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r, 2u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(v, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(h, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(s, 6u);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test swapping surfaces.
///
///
///     ┌───────────────────────┐
///     │┌───────┬─────────────┐│
///     ││┌─────┐│┌─────┬─────┐││
///     │││ ABC │││  D  │  E  │││
///     ││└─────┘│└─────┴─────┘││
///     │└───────┴─────────────┘│
///     ├───────────────────────┤
///     │┌────────────────┐     │
///     ││        F       │     │
///     │└────────────────┘     │
///     └───────────────────────┘
///
NoiaTestResult should_swap_frames(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_POSITIONED;

    NoiaSize size_b = noia_frame_get_area(b).size;
    NoiaSize size_f = noia_frame_get_area(f).size;

    noia_frame_swap(b, f, co);

    NOIA_ASSERT_FRAME_TRUNK(b, abc);
    NOIA_ASSERT_FRAME_TRUNK(f, r);

    NOIA_ASSERT(noia_frame_get_sid(b) == 6,
                "SID of frame B should be 6 (is '%lu')",
                noia_frame_get_sid(b));
    NOIA_ASSERT(noia_frame_get_sid(f) == 2,
                "SID of frame B should be 2 (is '%lu')",
                noia_frame_get_sid(f));

    NoiaSize size_2 = noia_surface_get_desired_size(co, 2);
    NoiaSize size_6 = noia_surface_get_desired_size(co, 6);

    NOIA_ASSERT(((size_b.width  == size_6.width)
             and (size_b.height == size_6.height)),
                "Frame size should be {'%d', '%d'} (is {'%d', '%d'})",
                size_b.width, size_b.height, size_6.width, size_6.height);

    NOIA_ASSERT(((size_f.width  == size_2.width)
             and (size_f.height == size_2.height)),
                "Frame size should be {'%d', '%d'} (is {'%d', '%d'})",
                size_f.width, size_f.height, size_2.width, size_2.height);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check if frame covered with other frames correctly pops up.
NoiaTestResult should_pop_frame_recursively(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NoiaFrame* r  = noia_frame_new();
    NoiaFrame* u  = noia_frame_new();
    NoiaFrame* s  = noia_frame_new();
    NoiaFrame* z  = noia_frame_new();
    NoiaFrame* u1 = noia_frame_new();
    NoiaFrame* u3 = noia_frame_new();
    NoiaFrame* s1 = noia_frame_new();
    NoiaFrame* s2 = noia_frame_new();
    NoiaFrame* s3 = noia_frame_new();
    NoiaFrame* z1 = noia_frame_new();
    NoiaFrame* z2 = noia_frame_new();
    NoiaFrame* z3 = noia_frame_new();
    noia_test_frame_config(r,  co, NOIA_FRAME_TYPE_STACKED, scInvalidSurfaceId);
    noia_test_frame_config(u,  co, NOIA_FRAME_TYPE_VERTICAL, scInvalidSurfaceId);
    noia_test_frame_config(s,  co, NOIA_FRAME_TYPE_STACKED, scInvalidSurfaceId);
    noia_test_frame_config(z,  co, NOIA_FRAME_TYPE_STACKED, scInvalidSurfaceId);
    noia_test_frame_config(s1, co, NOIA_FRAME_TYPE_LEAF, 11);
    noia_test_frame_config(s2, co, NOIA_FRAME_TYPE_LEAF, 12);
    noia_test_frame_config(s3, co, NOIA_FRAME_TYPE_LEAF, 13);
    noia_test_frame_config(u1, co, NOIA_FRAME_TYPE_LEAF, 21);
    noia_test_frame_config(u3, co, NOIA_FRAME_TYPE_LEAF, 23);
    noia_test_frame_config(z1, co, NOIA_FRAME_TYPE_LEAF, 31);
    noia_test_frame_config(z2, co, NOIA_FRAME_TYPE_LEAF, 32);
    noia_test_frame_config(z3, co, NOIA_FRAME_TYPE_LEAF, 33);
    noia_frame_prepend(r, s);
    noia_frame_prepend(r, u);
    noia_frame_prepend(u, u1);
    noia_frame_prepend(u, z);
    noia_frame_prepend(u, u3);
    noia_frame_prepend(s, s1);
    noia_frame_prepend(s, s2);
    noia_frame_prepend(s, s3);
    noia_frame_prepend(z, z1);
    noia_frame_prepend(z, z2);
    noia_frame_prepend(z, z3);

    NoiaSurfaceContext a1[] = {
            {23, {0, 0}},
            {33, {0, 0}},
            {32, {0, 0}},
            {31, {0, 0}},
            {21, {0, 0}},
            {13, {0, 0}},
            {12, {0, 0}},
            {11, {0, 0}},
        };

    NoiaSurfaceContext a2[] = {
            {13, {0, 0}},
            {12, {0, 0}},
            {11, {0, 0}},
            {23, {0, 0}},
            {21, {0, 0}},
            {33, {0, 0}},
            {31, {0, 0}},
            {32, {0, 0}},
        };

    NoiaPool* pool1 = noia_pool_create(NOIA_SIZEOF_ARRAY(a1),
                                      sizeof(NoiaSurfaceContext));

    NoiaPool* pool2 = noia_pool_create(NOIA_SIZEOF_ARRAY(a2),
                                      sizeof(NoiaSurfaceContext));


    noia_frame_to_array(r, pool1, co);
    noia_frame_pop_recursively(r, z2);
    noia_frame_to_array(r, pool2, co);
    noia_frame_free(r);

    NOIA_ASSERT_FRAME_ARRAY(a1, pool1);
    NOIA_ASSERT_FRAME_ARRAY(a2, pool2);

    noia_pool_destroy(pool1);
    noia_pool_destroy(pool2);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check if leaf frame is correctly ramified.
NoiaTestResult should_ramify_leaf(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_SIMPLE;

    NoiaFrameType type = NOIA_FRAME_TYPE_HORIZONTAL;
    NoiaFrame* d = noia_frame_ramify(v3, type, co);

    NOIA_ASSERT_FRAME_POINTER(noia_frame_get_trunk(v3), d);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_get_trunk(d), v);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_get_trunk(v), r);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r, 3u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(v, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(h, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(s, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(d, 1u);

    NOIA_ASSERT_FRAME_PARAMETERS(d, scInvalidSurfaceId, type,
                                 0, 24, 60, 12, NULL);
    NOIA_ASSERT_FRAME_PARAMETERS(v3, 13ul, NOIA_FRAME_TYPE_LEAF,
                                 0, 24, 60, 12, "v3");

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check if non-leaf frame is correctly ramified.
NoiaTestResult should_ramify_nonleaf(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_SIMPLE;

    NoiaFrameType type = NOIA_FRAME_TYPE_HORIZONTAL;
    NoiaFrame* d = noia_frame_ramify(v, type, co);

    NOIA_ASSERT_FRAME_POINTER(noia_frame_get_trunk(v), d);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_get_trunk(d), r);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r, 3u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(v, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(h, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(s, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(d, 1u);

    NOIA_ASSERT_FRAME_PARAMETERS(d, scInvalidSurfaceId, type,
                                 0, 0, 60, 60, NULL);
    NOIA_ASSERT_FRAME_PARAMETERS(v, scInvalidSurfaceId,
                                 NOIA_FRAME_TYPE_VERTICAL,
                                 0, 0, 60, 60, "v");

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Should deramify frame with single non-leaf twig.
NoiaTestResult should_deramify(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_DERAMIFYING;

    noia_frame_deramify(a2);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r,  3u)
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(a1, 1u)
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(a2, 1u)
    NOIA_ASSERT_FRAME_TRUNK(a2, r);
    //NOIA_ASSERT_FRAME_TRUNK(b,  NULL); <- invalid read
    NOIA_ASSERT_FRAME_TRUNK(c,  a2);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Should deramify frame with single leaf twig.
NoiaTestResult should_deramify_with_one_leaf(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_DERAMIFYING;

    noia_frame_deramify(a1);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r,  3u)
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(a1, 0u)
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(a2, 1u)
    NOIA_ASSERT_FRAME_TRUNK(a2, r);
    NOIA_ASSERT_FRAME_TRUNK(a1, r);
    //NOIA_ASSERT_FRAME_TRUNK(f,  a1); <- invalid read
    NOIA_ASSERT_FRAME_TRUNK(b,  a2);
    NOIA_ASSERT_FRAME_TRUNK(c,  b);
    NOIA_ASSERT_FRAME_TRUNK(d1, c);
    NOIA_ASSERT_FRAME_TRUNK(d2, c);
    NOIA_ASSERT_FRAME_TRUNK(d3, c);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Should not deramify frame with many twigs.
NoiaTestResult should_not_deramify_not_single(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_DERAMIFYING;

    noia_frame_deramify(r);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r,  3u)
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(a1, 1u)
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(a2, 1u)
    NOIA_ASSERT_FRAME_TRUNK(a2, r);
    NOIA_ASSERT_FRAME_TRUNK(b,  a2);
    NOIA_ASSERT_FRAME_TRUNK(f,  a1);
    NOIA_ASSERT_FRAME_TRUNK(b,  a2);
    NOIA_ASSERT_FRAME_TRUNK(c,  b);
    NOIA_ASSERT_FRAME_TRUNK(d1, c);
    NOIA_ASSERT_FRAME_TRUNK(d2, c);
    NOIA_ASSERT_FRAME_TRUNK(d3, c);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Should not deramify frame with many leaf twigs.
NoiaTestResult should_not_deramify_single_with_many_leafs(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_DERAMIFYING;

    noia_frame_deramify(b);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r,  3u)
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(a1, 1u)
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(a2, 1u)
    NOIA_ASSERT_FRAME_TRUNK(a2, r);
    NOIA_ASSERT_FRAME_TRUNK(a1, r);
    NOIA_ASSERT_FRAME_TRUNK(f,  a1);
    NOIA_ASSERT_FRAME_TRUNK(b,  a2);
    NOIA_ASSERT_FRAME_TRUNK(c,  b);
    NOIA_ASSERT_FRAME_TRUNK(d1, c);
    NOIA_ASSERT_FRAME_TRUNK(d2, c);
    NOIA_ASSERT_FRAME_TRUNK(d3, c);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check if `noia_frame_find_with_sid` returns correct frames.
NoiaTestResult should_find_with_sid(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_SIMPLE;

    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 666), NULL)

    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 11), v1);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 12), v2);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 13), v3);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 14), v4);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 15), v5);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 21), h1);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 22), h2);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 23), h3);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 24), h4);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 25), h5);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 31), s1);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 32), s2);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 33), s3);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 34), s4);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(r, 35), s5);

    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(s, 11), NULL);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(s, 13), NULL);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_with_sid(s, 22), NULL);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Find first frame above given with type Special
NoiaTestResult should_find_top(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_POSITIONED;

    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_top(a), abcde);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_top(b), abcde);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_top(c), abcde);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_top(d), abcde);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_top(e), abcde);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_top(f), f);
    NOIA_ASSERT_FRAME_POINTER(noia_frame_find_top(r), NULL);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
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
    NoiaArea area = {{0,0},{0,0}};
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NoiaFrame* p = NULL;
    NoiaFrame* r = noia_frame_new();
    NoiaFrame* a = noia_frame_new();
    NoiaFrame* b = noia_frame_new();
    noia_frame_configure(r, co,
                         NOIA_FRAME_TYPE_SPECIAL | NOIA_FRAME_TYPE_VERTICAL,
                         scInvalidSurfaceId, area, "R");
    noia_frame_configure(a, co, NOIA_FRAME_TYPE_LEAF, 1, area, "A");
    noia_frame_configure(b, co, NOIA_FRAME_TYPE_LEAF, 2, area, "B");
    noia_frame_append(r, a);
    noia_frame_append(r, b);

    p = noia_frame_find_contiguous(a, NOIA_DIRECTION_S, 0);
    NOIA_ASSERT(p == a, "0*South from A should be A (is '%s')",
                noia_frame_get_title(p));

    p = noia_frame_find_contiguous(a, NOIA_DIRECTION_S, 1);
    NOIA_ASSERT(p == b, "1*South from A should be B (is '%s')",
                noia_frame_get_title(p));

    p = noia_frame_find_contiguous(b, NOIA_DIRECTION_N, 1);
    NOIA_ASSERT(p == a, "1*North from B should be A (is '%s')",
                noia_frame_get_title(p));

    p = noia_frame_find_contiguous(b, NOIA_DIRECTION_S, 1);
    NOIA_ASSERT(p == NULL, "1*South from B should be NULL");

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
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
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NoiaFrame* p = NULL;
    NoiaFrame* r = noia_frame_new();
    NoiaFrame* a = noia_frame_new();
    NoiaFrame* b = noia_frame_new();
    NoiaFrame* c = noia_frame_new();
    NoiaFrame* d = noia_frame_new();
    NoiaFrame* e = noia_frame_new();
    NoiaFrame* f = noia_frame_new();
    noia_test_frame_config(r, co,
                           NOIA_FRAME_TYPE_SPECIAL | NOIA_FRAME_TYPE_HORIZONTAL,
                           scInvalidSurfaceId);
    noia_test_frame_config(a, co, NOIA_FRAME_TYPE_LEAF, 1);
    noia_test_frame_config(b, co, NOIA_FRAME_TYPE_LEAF, 2);
    noia_test_frame_config(c, co, NOIA_FRAME_TYPE_LEAF, 3);
    noia_test_frame_config(d, co, NOIA_FRAME_TYPE_LEAF, 4);
    noia_test_frame_config(e, co, NOIA_FRAME_TYPE_LEAF, 5);
    noia_test_frame_config(f, co, NOIA_FRAME_TYPE_LEAF, 6);
    noia_frame_append(r, a);
    noia_frame_append(r, b);
    noia_frame_append(r, c);
    noia_frame_append(r, d);
    noia_frame_append(r, e);
    noia_frame_append(r, f);

    p = noia_frame_find_contiguous(b, NOIA_DIRECTION_E, 3);
    NOIA_ASSERT(p == e, "3*West from B should be E");

    p = noia_frame_find_contiguous(f, NOIA_DIRECTION_W, 5);
    NOIA_ASSERT(p == a, "5*East from F should be A");

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
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
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NoiaFrame* p  = NULL;
    NoiaFrame* r  = noia_frame_new();
    NoiaFrame* a  = noia_frame_new();
    NoiaFrame* bc = noia_frame_new();
    NoiaFrame* b  = noia_frame_new();
    NoiaFrame* c  = noia_frame_new();
    NoiaFrame* d  = noia_frame_new();
    noia_test_frame_config(r, co,
                           NOIA_FRAME_TYPE_SPECIAL | NOIA_FRAME_TYPE_HORIZONTAL,
                           scInvalidSurfaceId);
    noia_test_frame_config(a,  co, NOIA_FRAME_TYPE_LEAF, 1);
    noia_test_frame_config(bc, co, NOIA_FRAME_TYPE_VERTICAL,
                           scInvalidSurfaceId);
    noia_test_frame_config(b,  co, NOIA_FRAME_TYPE_LEAF, 2);
    noia_test_frame_config(c,  co, NOIA_FRAME_TYPE_LEAF, 3);
    noia_test_frame_config(d,  co, NOIA_FRAME_TYPE_LEAF, 4);
    noia_frame_append(r,  a);
    noia_frame_append(r,  bc);
    noia_frame_append(bc, b);
    noia_frame_append(bc, c);
    noia_frame_append(r,  d);

    p = noia_frame_find_contiguous(b, NOIA_DIRECTION_E, 1);
    NOIA_ASSERT(p == d, "1*East from B should be D");

    p = noia_frame_find_contiguous(a, NOIA_DIRECTION_E, 1);
    NOIA_ASSERT(p == bc, "1*West from A should be BC");

    p = noia_frame_find_contiguous(c, NOIA_DIRECTION_W, 1);
    NOIA_ASSERT(p == a, "1*East from C should be A");

    p = noia_frame_find_contiguous(a, NOIA_DIRECTION_E, 2);
    NOIA_ASSERT(p == d, "2*West from A should be D");

    p = noia_frame_find_contiguous(c, NOIA_DIRECTION_TRUNK, 1);
    NOIA_ASSERT(p == noia_frame_get_trunk(c), "1*Trunk from C should be BC");

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
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
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NoiaFrame* p    = NULL;
    NoiaFrame* r    = noia_frame_new();
    NoiaFrame* a    = noia_frame_new();
    NoiaFrame* b    = noia_frame_new();
    NoiaFrame* c    = noia_frame_new();
    NoiaFrame* d    = noia_frame_new();
    NoiaFrame* bc   = noia_frame_new();
    NoiaFrame* abc  = noia_frame_new();
    noia_test_frame_config(r, co,
                           NOIA_FRAME_TYPE_SPECIAL | NOIA_FRAME_TYPE_HORIZONTAL,
                           scInvalidSurfaceId);
    noia_test_frame_config(a,   co, NOIA_FRAME_TYPE_LEAF, 1);
    noia_test_frame_config(b,   co, NOIA_FRAME_TYPE_LEAF, 2);
    noia_test_frame_config(c,   co, NOIA_FRAME_TYPE_LEAF, 3);
    noia_test_frame_config(d,   co, NOIA_FRAME_TYPE_LEAF, 4);
    noia_test_frame_config(bc,  co, NOIA_FRAME_TYPE_VERTICAL,
                           scInvalidSurfaceId);
    noia_test_frame_config(abc, co, NOIA_FRAME_TYPE_VERTICAL,
                           scInvalidSurfaceId);
    noia_frame_append(bc, b);
    noia_frame_append(bc, c);
    noia_frame_append(abc, a);
    noia_frame_append(abc, bc);
    noia_frame_append(r, abc);
    noia_frame_append(r, d);

    p = noia_frame_find_contiguous(c, NOIA_DIRECTION_E, 1);
    NOIA_ASSERT(p == d, "1*East from C should be D");

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
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
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_POSITIONED;

    NoiaPosition point = {10, 10};
    NoiaFrame* p = noia_frame_find_pointed(r, point);
    NOIA_ASSERT(p == a, "Pointed inside ABC should be A");

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
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
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_POSITIONED;

    NoiaPosition point = {50, 10};
    NoiaFrame* p = noia_frame_find_pointed(r, point);

    NOIA_ASSERT(p == d, "Pointed inside D should be D");

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
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
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_POSITIONED;

    NoiaPosition point = {20, -10};
    NoiaFrame* p = noia_frame_find_pointed(r, point);

    NOIA_ASSERT(p == a, "Pointed outside ABCDEF above A should be A");

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
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
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_POSITIONED;

    NoiaPosition point = {60, -10};
    NoiaFrame* p = noia_frame_find_pointed(r, point);

    NOIA_ASSERT(p == d, "Pointed outside ABCDEF above D should be D");

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
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
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_POSITIONED;

    NoiaPosition point = {50, 70};
    NoiaFrame* p = noia_frame_find_pointed(abcde, point);

    NOIA_ASSERT(p == d, "Pointed in F searching in ABCDE should be D");

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
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
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_POSITIONED;

    NoiaPosition point = {80, 80};
    NoiaFrame* p = noia_frame_find_pointed(r, point);

    NOIA_ASSERT(p == r, "Pointed in ABCDEF outside subframes should be ABCDEF");

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Find adjacent frames.
///
/// - 1*South from A should be C
/// - 1*South from C should be E
/// - 2*South from A should be F
/// - 1*South from CD should be F
/// - 1*North from AB should be NULL
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
    NoiaCoordinator* co = noia_coordinator_mock_new();
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
    noia_frame_configure(a, co, NOIA_FRAME_TYPE_LEAF, 1,
                         (NoiaArea) {{ 0,  0}, { 70, 10}}, "");
    noia_frame_configure(b, co, NOIA_FRAME_TYPE_LEAF, 2,
                         (NoiaArea) {{70,  0}, { 30, 10}}, "");
    noia_frame_configure(c, co, NOIA_FRAME_TYPE_LEAF, 3,
                         (NoiaArea) {{ 0, 10}, { 50, 10}}, "");
    noia_frame_configure(d, co, NOIA_FRAME_TYPE_LEAF, 4,
                         (NoiaArea) {{50, 10}, { 50, 10}}, "");
    noia_frame_configure(e, co, NOIA_FRAME_TYPE_LEAF, 5,
                         (NoiaArea) {{ 0, 20}, { 30, 10}}, "");
    noia_frame_configure(f, co, NOIA_FRAME_TYPE_LEAF, 6,
                         (NoiaArea) {{30, 20}, { 70, 10}}, "");
    noia_frame_configure(r, co, NOIA_FRAME_TYPE_VERTICAL, INV,
                         (NoiaArea) {{ 0,  0}, {100, 30}}, "");
    noia_frame_configure(ab, co, NOIA_FRAME_TYPE_HORIZONTAL, INV,
                         (NoiaArea) {{ 0,  0}, {100, 10}}, "");
    noia_frame_configure(cd, co, NOIA_FRAME_TYPE_HORIZONTAL, INV,
                         (NoiaArea) {{ 0, 10}, {100, 10}}, "");
    noia_frame_configure(ef, co, NOIA_FRAME_TYPE_HORIZONTAL, INV,
                         (NoiaArea) {{ 0, 20}, {100, 10}}, "");

    p = noia_frame_find_adjacent(a, NOIA_DIRECTION_S, 1);
    NOIA_ASSERT(p == c, "1*South from A should be C");

    p = noia_frame_find_adjacent(c, NOIA_DIRECTION_S, 1);
    NOIA_ASSERT(p == e, "1*South from C should be E");

    p = noia_frame_find_adjacent(a, NOIA_DIRECTION_S, 2);
    NOIA_ASSERT(p == f, "2*South from A should be F");

    p = noia_frame_find_adjacent(cd, NOIA_DIRECTION_S, 1);
    NOIA_ASSERT(p == f, "1*South from CD should be F");

    p = noia_frame_find_adjacent(ab, NOIA_DIRECTION_N, 1);
    NOIA_ASSERT(p == NULL, "1*North from AB should be NULL");

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test if frames for resizing with horizontal floating are correctly set up.
NoiaTestResult check_setup_for_resizing_with_horizontal_floating(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_HORIZONTAL_FLOATING;

    NOIA_ASSERT_FRAME_AREA(r,        0,  0, 300, 120);
    NOIA_ASSERT_FRAME_AREA(abcdef,   0,  0, 120, 120);
    NOIA_ASSERT_FRAME_AREA(g,      120,  0,  60, 120);
    NOIA_ASSERT_FRAME_AREA(hi,     180,  0, 120, 120);
    NOIA_ASSERT_FRAME_AREA(a,        0,  0, 120,  40);
    NOIA_ASSERT_FRAME_AREA(bcd,      0, 40, 120,  40);
    NOIA_ASSERT_FRAME_AREA(ef,       0, 80, 120,  40);
    NOIA_ASSERT_FRAME_AREA(h,      180,  0,  60, 120);
    NOIA_ASSERT_FRAME_AREA(i,      240,  0,  60, 120);
    NOIA_ASSERT_FRAME_AREA(b,        0, 40, 120,  40);
    NOIA_ASSERT_FRAME_AREA(c,        0, 40, 120,  40);
    NOIA_ASSERT_FRAME_AREA(d,        0, 40, 120,  40);
    NOIA_ASSERT_FRAME_AREA(e,        0, 80,  60,  40);
    NOIA_ASSERT_FRAME_AREA(f,       60, 80,  60,  40);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test if frames for resizing with vertical floating are correctly set up.
NoiaTestResult check_setup_for_resizing_with_vertical_floating(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_VERTICAL_FLOATING;

    NOIA_ASSERT_FRAME_AREA(r,       0,   0, 120, 300);
    NOIA_ASSERT_FRAME_AREA(abcdef,  0,   0, 120, 120);
    NOIA_ASSERT_FRAME_AREA(g,       0, 120, 120,  60);
    NOIA_ASSERT_FRAME_AREA(hi,      0, 180, 120, 120);
    NOIA_ASSERT_FRAME_AREA(a,       0,   0,  40, 120);
    NOIA_ASSERT_FRAME_AREA(bcd,    40,   0,  40, 120);
    NOIA_ASSERT_FRAME_AREA(ef,     80,   0,  40, 120);
    NOIA_ASSERT_FRAME_AREA(h,       0, 180, 120,  60);
    NOIA_ASSERT_FRAME_AREA(i,       0, 240, 120,  60);
    NOIA_ASSERT_FRAME_AREA(b,      40,   0,  40, 120);
    NOIA_ASSERT_FRAME_AREA(c,      40,   0,  40, 120);
    NOIA_ASSERT_FRAME_AREA(d,      40,   0,  40, 120);
    NOIA_ASSERT_FRAME_AREA(e,      80,   0,  40,  60);
    NOIA_ASSERT_FRAME_AREA(f,      80,  60,  40,  60);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test if frame correctly resizes when shrunk with south border.
NoiaTestResult should_shrink_from_south(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_HORIZONTAL_FLOATING;

    noia_frame_resize(g, co, NOIA_DIRECTION_S, -30);

    NOIA_ASSERT_FRAME_AREA(r,        0,  0, 300, 90);
    NOIA_ASSERT_FRAME_AREA(abcdef,   0,  0, 120, 90);
    NOIA_ASSERT_FRAME_AREA(g,      120,  0,  60, 90);
    NOIA_ASSERT_FRAME_AREA(hi,     180,  0, 120, 90);
    NOIA_ASSERT_FRAME_AREA(a,        0,  0, 120, 30);
    NOIA_ASSERT_FRAME_AREA(bcd,      0, 30, 120, 30);
    NOIA_ASSERT_FRAME_AREA(ef,       0, 60, 120, 30);
    NOIA_ASSERT_FRAME_AREA(h,      180,  0,  60, 90);
    NOIA_ASSERT_FRAME_AREA(i,      240,  0,  60, 90);
    NOIA_ASSERT_FRAME_AREA(b,        0, 30, 120, 30);
    NOIA_ASSERT_FRAME_AREA(c,        0, 30, 120, 30);
    NOIA_ASSERT_FRAME_AREA(d,        0, 30, 120, 30);
    NOIA_ASSERT_FRAME_AREA(e,        0, 60,  60, 30);
    NOIA_ASSERT_FRAME_AREA(f,       60, 60,  60, 30);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test if frame correctly resizes when expanded with south border.
NoiaTestResult should_expand_to_south(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_HORIZONTAL_FLOATING;

    noia_frame_resize(g, co, NOIA_DIRECTION_S, 30);

    NOIA_ASSERT_FRAME_AREA(r,        0,   0, 300, 150);
    NOIA_ASSERT_FRAME_AREA(abcdef,   0,   0, 120, 150);
    NOIA_ASSERT_FRAME_AREA(g,      120,   0,  60, 150);
    NOIA_ASSERT_FRAME_AREA(hi,     180,   0, 120, 150);
    NOIA_ASSERT_FRAME_AREA(a,        0,   0, 120,  50);
    NOIA_ASSERT_FRAME_AREA(bcd,      0,  50, 120,  50);
    NOIA_ASSERT_FRAME_AREA(ef,       0, 100, 120,  50);
    NOIA_ASSERT_FRAME_AREA(h,      180,   0,  60, 150);
    NOIA_ASSERT_FRAME_AREA(i,      240,   0,  60, 150);
    NOIA_ASSERT_FRAME_AREA(b,        0,  50, 120,  50);
    NOIA_ASSERT_FRAME_AREA(c,        0,  50, 120,  50);
    NOIA_ASSERT_FRAME_AREA(d,        0,  50, 120,  50);
    NOIA_ASSERT_FRAME_AREA(e,        0, 100,  60,  50);
    NOIA_ASSERT_FRAME_AREA(f,       60, 100,  60,  50);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test if frame correctly resizes when shrunk with north border.
NoiaTestResult should_shrink_from_north(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_HORIZONTAL_FLOATING;

    noia_frame_resize(g, co, NOIA_DIRECTION_N, -30);

    NOIA_ASSERT_FRAME_AREA             (r,        0, 30, 300, 90);
    NOIA_ASSERT_FRAME_AREA             (abcdef,   0, 30, 120, 90);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(g,      120, 30,  60, 90);
    NOIA_ASSERT_FRAME_AREA             (hi,     180, 30, 120, 90);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(a,        0, 30, 120, 30);
    NOIA_ASSERT_FRAME_AREA             (bcd,      0, 60, 120, 30);
    NOIA_ASSERT_FRAME_AREA             (ef,       0, 90, 120, 30);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(h,      180, 30,  60, 90);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(i,      240, 30,  60, 90);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(b,        0, 60, 120, 30);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(c,        0, 60, 120, 30);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(d,        0, 60, 120, 30);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(e,        0, 90,  60, 30);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(f,       60, 90,  60, 30);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test if frame correctly resizes when expanded with north border.
NoiaTestResult should_expand_to_north(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_HORIZONTAL_FLOATING;

    noia_frame_resize(g, co, NOIA_DIRECTION_N, 30);

    NOIA_ASSERT_FRAME_AREA             (r,        0, -30, 300, 150);
    NOIA_ASSERT_FRAME_AREA             (abcdef,   0, -30, 120, 150);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(g,      120, -30,  60, 150);
    NOIA_ASSERT_FRAME_AREA             (hi,     180, -30, 120, 150);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(a,        0, -30, 120,  50);
    NOIA_ASSERT_FRAME_AREA             (bcd,      0,  20, 120,  50);
    NOIA_ASSERT_FRAME_AREA             (ef,       0,  70, 120,  50);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(h,      180, -30,  60, 150);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(i,      240, -30,  60, 150);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(b,        0,  20, 120,  50);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(c,        0,  20, 120,  50);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(d,        0,  20, 120,  50);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(e,        0,  70,  60,  50);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(f,       60,  70,  60,  50);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test if frame correctly resizes when shrunk with east border.
NoiaTestResult should_shrink_from_east(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_VERTICAL_FLOATING;

    noia_frame_resize(g, co, NOIA_DIRECTION_E, -30);

    NOIA_ASSERT_FRAME_AREA             (r,       0,   0,  90, 300);
    NOIA_ASSERT_FRAME_AREA             (abcdef,  0,   0,  90, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(g,       0, 120,  90,  60);
    NOIA_ASSERT_FRAME_AREA             (hi,      0, 180,  90, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(a,       0,   0,  30, 120);
    NOIA_ASSERT_FRAME_AREA             (bcd,    30,   0,  30, 120);
    NOIA_ASSERT_FRAME_AREA             (ef,     60,   0,  30, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(h,       0, 180,  90,  60);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(i,       0, 240,  90,  60);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(b,      30,   0,  30, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(c,      30,   0,  30, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(d,      30,   0,  30, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(e,      60,   0,  30,  60);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(f,      60,  60,  30,  60);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test if frame correctly resizes when expanded with east border.
NoiaTestResult should_expand_to_east(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_VERTICAL_FLOATING;

    noia_frame_resize(g, co, NOIA_DIRECTION_E, 30);

    NOIA_ASSERT_FRAME_AREA             (r,       0,   0, 150, 300);
    NOIA_ASSERT_FRAME_AREA             (abcdef,  0,   0, 150, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(g,       0, 120, 150,  60);
    NOIA_ASSERT_FRAME_AREA             (hi,      0, 180, 150, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(a,       0,   0,  50, 120);
    NOIA_ASSERT_FRAME_AREA             (bcd,    50,   0,  50, 120);
    NOIA_ASSERT_FRAME_AREA             (ef,    100,   0,  50, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(h,       0, 180, 150,  60);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(i,       0, 240, 150,  60);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(b,      50,   0,  50, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(c,      50,   0,  50, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(d,      50,   0,  50, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(e,     100,   0,  50,  60);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(f,     100,  60,  50,  60);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test if frame correctly resizes when shrunk with west border.
NoiaTestResult should_shrink_from_west(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_VERTICAL_FLOATING;

    noia_frame_resize(g, co, NOIA_DIRECTION_W, -30);

    NOIA_ASSERT_FRAME_AREA             (r,      30,   0,  90, 300);
    NOIA_ASSERT_FRAME_AREA             (abcdef, 30,   0,  90, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(g,      30, 120,  90,  60);
    NOIA_ASSERT_FRAME_AREA             (hi,     30, 180,  90, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(a,      30,   0,  30, 120);
    NOIA_ASSERT_FRAME_AREA             (bcd,    60,   0,  30, 120);
    NOIA_ASSERT_FRAME_AREA             (ef,     90,   0,  30, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(h,      30, 180,  90,  60);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(i,      30, 240,  90,  60);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(b,      60,   0,  30, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(c,      60,   0,  30, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(d,      60,   0,  30, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(e,      90,   0,  30,  60);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(f,      90,  60,  30,  60);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test if frame correctly resizes when expanded with west border.
NoiaTestResult should_expand_to_west(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_VERTICAL_FLOATING;

    noia_frame_resize(g, co, NOIA_DIRECTION_W, 30);

    NOIA_ASSERT_FRAME_AREA             (r,      -30,   0, 150, 300);
    NOIA_ASSERT_FRAME_AREA             (abcdef, -30,   0, 150, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(g,      -30, 120, 150,  60);
    NOIA_ASSERT_FRAME_AREA             (hi,     -30, 180, 150, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(a,      -30,   0,  50, 120);
    NOIA_ASSERT_FRAME_AREA             (bcd,     20,   0,  50, 120);
    NOIA_ASSERT_FRAME_AREA             (ef,      70,   0,  50, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(h,      -30, 180, 150,  60);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(i,      -30, 240, 150,  60);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(b,       20,   0,  50, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(c,       20,   0,  50, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(d,       20,   0,  50, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(e,       70,   0,  50,  60);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(f,       70,  60,  50,  60);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test if frame correctly resizes when northern border moved south in
/// vertical frame.
NoiaTestResult should_vertical_shrink_from_north(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_VERTICAL_FLOATING;

    noia_frame_resize(g, co, NOIA_DIRECTION_N, -20);

    NOIA_ASSERT_FRAME_AREA             (r,       0,   0, 120, 300);
    NOIA_ASSERT_FRAME_AREA             (abcdef,  0,   0, 120, 140);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(g,       0, 140, 120,  40);
    NOIA_ASSERT_FRAME_AREA             (hi,      0, 180, 120, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(a,       0,   0,  40, 140);
    NOIA_ASSERT_FRAME_AREA             (bcd,    40,   0,  40, 140);
    NOIA_ASSERT_FRAME_AREA             (ef,     80,   0,  40, 140);
    NOIA_ASSERT_FRAME_AREA             (h,       0, 180, 120,  60);
    NOIA_ASSERT_FRAME_AREA             (i,       0, 240, 120,  60);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(b,      40,   0,  40, 140);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(c,      40,   0,  40, 140);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(d,      40,   0,  40, 140);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(e,      80,   0,  40,  70);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(f,      80,  70,  40,  70);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test if frame correctly resizes when northern border moved north in
/// vertical frame.
NoiaTestResult should_vertical_expand_to_north(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_VERTICAL_FLOATING;

    noia_frame_resize(g, co, NOIA_DIRECTION_N, 20);

    NOIA_ASSERT_FRAME_AREA             (r,       0,   0, 120, 300);
    NOIA_ASSERT_FRAME_AREA             (abcdef,  0,   0, 120, 100);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(g,       0, 100, 120,  80);
    NOIA_ASSERT_FRAME_AREA             (hi,      0, 180, 120, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(a,       0,   0,  40, 100);
    NOIA_ASSERT_FRAME_AREA             (bcd,    40,   0,  40, 100);
    NOIA_ASSERT_FRAME_AREA             (ef,     80,   0,  40, 100);
    NOIA_ASSERT_FRAME_AREA             (h,       0, 180, 120,  60);
    NOIA_ASSERT_FRAME_AREA             (i,       0, 240, 120,  60);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(b,      40,   0,  40, 100);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(c,      40,   0,  40, 100);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(d,      40,   0,  40, 100);
    NOIA_ASSERT_FRAME_AREA             (e,      80,   0,  40,  50);
    NOIA_ASSERT_FRAME_AREA             (f,      80,  50,  40,  50);


    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test if frame correctly resizes when western border moved east in
/// horizontal frame.
NoiaTestResult should_horizontal_shrink_from_west(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_HORIZONTAL_FLOATING;

    noia_frame_resize(g, co, NOIA_DIRECTION_W, -20);

    NOIA_ASSERT_FRAME_AREA             (r,        0,  0, 300, 120);
    NOIA_ASSERT_FRAME_AREA             (abcdef,   0,  0, 140, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(g,      140,  0,  40, 120);
    NOIA_ASSERT_FRAME_AREA             (hi,     180,  0, 120, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(a,        0,  0, 140,  40);
    NOIA_ASSERT_FRAME_AREA             (bcd,      0, 40, 140,  40);
    NOIA_ASSERT_FRAME_AREA             (ef,       0, 80, 140,  40);
    NOIA_ASSERT_FRAME_AREA             (h,      180,  0,  60, 120);
    NOIA_ASSERT_FRAME_AREA             (i,      240,  0,  60, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(b,        0, 40, 140,  40);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(c,        0, 40, 140,  40);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(d,        0, 40, 140,  40);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(e,        0, 80,  70,  40);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(f,       70, 80,  70,  40);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Test if frame correctly resizes when western border moved west in
/// horizontal frame.
NoiaTestResult should_horizontal_expand_to_west(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_HORIZONTAL_FLOATING;

    noia_frame_resize(g, co, NOIA_DIRECTION_W, 20);

    NOIA_ASSERT_FRAME_AREA             (r,        0,  0, 300, 120);
    NOIA_ASSERT_FRAME_AREA             (abcdef,   0,  0, 100, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(g,      100,  0,  80, 120);
    NOIA_ASSERT_FRAME_AREA             (hi,     180,  0, 120, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(a,        0,  0, 100,  40);
    NOIA_ASSERT_FRAME_AREA             (bcd,      0, 40, 100,  40);
    NOIA_ASSERT_FRAME_AREA             (ef,       0, 80, 100,  40);
    NOIA_ASSERT_FRAME_AREA             (h,      180,  0,  60, 120);
    NOIA_ASSERT_FRAME_AREA             (i,      240,  0,  60, 120);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(b,        0, 40, 100,  40);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(c,        0, 40, 100,  40);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(d,        0, 40, 100,  40);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(e,        0, 80,  50,  40);
    NOIA_ASSERT_FRAME_AREA_WITH_SURFACE(f,       50, 80,  50,  40);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check position of frame moved to north-west.
NoiaTestResult should_move_to_north_west(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_HORIZONTAL_FLOATING;

    noia_frame_move(r, NOIA_DIRECTION_N, 20);
    noia_frame_move(r, NOIA_DIRECTION_W, 30);

    NOIA_ASSERT_FRAME_AREA(r,      -30, -20, 300, 120);
    NOIA_ASSERT_FRAME_AREA(abcdef, -30, -20, 120, 120);
    NOIA_ASSERT_FRAME_AREA(g,       90, -20,  60, 120);
    NOIA_ASSERT_FRAME_AREA(hi,     150, -20, 120, 120);
    NOIA_ASSERT_FRAME_AREA(a,      -30, -20, 120,  40);
    NOIA_ASSERT_FRAME_AREA(bcd,    -30,  20, 120,  40);
    NOIA_ASSERT_FRAME_AREA(ef,     -30,  60, 120,  40);
    NOIA_ASSERT_FRAME_AREA(h,      150, -20,  60, 120);
    NOIA_ASSERT_FRAME_AREA(i,      210, -20,  60, 120);
    NOIA_ASSERT_FRAME_AREA(b,      -30,  20, 120,  40);
    NOIA_ASSERT_FRAME_AREA(c,      -30,  20, 120,  40);
    NOIA_ASSERT_FRAME_AREA(d,      -30,  20, 120,  40);
    NOIA_ASSERT_FRAME_AREA(e,      -30,  60,  60,  40);
    NOIA_ASSERT_FRAME_AREA(f,       30,  60,  60,  40);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check position of frame moved to south-east.
NoiaTestResult should_move_to_south_east(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_HORIZONTAL_FLOATING;

    noia_frame_move(r, NOIA_DIRECTION_S, 20);
    noia_frame_move(r, NOIA_DIRECTION_E, 30);

    NOIA_ASSERT_FRAME_AREA(r,       30,  20, 300, 120);
    NOIA_ASSERT_FRAME_AREA(abcdef,  30,  20, 120, 120);
    NOIA_ASSERT_FRAME_AREA(g,      150,  20,  60, 120);
    NOIA_ASSERT_FRAME_AREA(hi,     210,  20, 120, 120);
    NOIA_ASSERT_FRAME_AREA(a,       30,  20, 120,  40);
    NOIA_ASSERT_FRAME_AREA(bcd,     30,  60, 120,  40);
    NOIA_ASSERT_FRAME_AREA(ef,      30, 100, 120,  40);
    NOIA_ASSERT_FRAME_AREA(h,      210,  20,  60, 120);
    NOIA_ASSERT_FRAME_AREA(i,      270,  20,  60, 120);
    NOIA_ASSERT_FRAME_AREA(b,       30,  60, 120,  40);
    NOIA_ASSERT_FRAME_AREA(c,       30,  60, 120,  40);
    NOIA_ASSERT_FRAME_AREA(d,       30,  60, 120,  40);
    NOIA_ASSERT_FRAME_AREA(e,       30, 100,  60,  40);
    NOIA_ASSERT_FRAME_AREA(f,       90, 100,  60,  40);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check position of frame moved to north-east.
NoiaTestResult should_move_from_north_east(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_HORIZONTAL_FLOATING;

    noia_frame_move(r, NOIA_DIRECTION_N, -20);
    noia_frame_move(r, NOIA_DIRECTION_E, -30);

    NOIA_ASSERT_FRAME_AREA(r,      -30,  20, 300, 120);
    NOIA_ASSERT_FRAME_AREA(abcdef, -30,  20, 120, 120);
    NOIA_ASSERT_FRAME_AREA(g,       90,  20,  60, 120);
    NOIA_ASSERT_FRAME_AREA(hi,     150,  20, 120, 120);
    NOIA_ASSERT_FRAME_AREA(a,      -30,  20, 120,  40);
    NOIA_ASSERT_FRAME_AREA(bcd,    -30,  60, 120,  40);
    NOIA_ASSERT_FRAME_AREA(ef,     -30, 100, 120,  40);
    NOIA_ASSERT_FRAME_AREA(h,      150,  20,  60, 120);
    NOIA_ASSERT_FRAME_AREA(i,      210,  20,  60, 120);
    NOIA_ASSERT_FRAME_AREA(b,      -30,  60, 120,  40);
    NOIA_ASSERT_FRAME_AREA(c,      -30,  60, 120,  40);
    NOIA_ASSERT_FRAME_AREA(d,      -30,  60, 120,  40);
    NOIA_ASSERT_FRAME_AREA(e,      -30, 100,  60,  40);
    NOIA_ASSERT_FRAME_AREA(f,       30, 100,  60,  40);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check if frame is not moved when trying to move not-floating frame.
NoiaTestResult should_not_move_when_frame_is_not_floating(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RESIZING_WITH_HORIZONTAL_FLOATING;

    noia_frame_move(f, NOIA_DIRECTION_N, -20);
    noia_frame_move(f, NOIA_DIRECTION_E, -30);

    NOIA_ASSERT_FRAME_AREA(r,        0,  0, 300, 120);
    NOIA_ASSERT_FRAME_AREA(abcdef,   0,  0, 120, 120);
    NOIA_ASSERT_FRAME_AREA(g,      120,  0,  60, 120);
    NOIA_ASSERT_FRAME_AREA(hi,     180,  0, 120, 120);
    NOIA_ASSERT_FRAME_AREA(a,        0,  0, 120,  40);
    NOIA_ASSERT_FRAME_AREA(bcd,      0, 40, 120,  40);
    NOIA_ASSERT_FRAME_AREA(ef,       0, 80, 120,  40);
    NOIA_ASSERT_FRAME_AREA(h,      180,  0,  60, 120);
    NOIA_ASSERT_FRAME_AREA(i,      240,  0,  60, 120);
    NOIA_ASSERT_FRAME_AREA(b,        0, 40, 120,  40);
    NOIA_ASSERT_FRAME_AREA(c,        0, 40, 120,  40);
    NOIA_ASSERT_FRAME_AREA(d,        0, 40, 120,  40);
    NOIA_ASSERT_FRAME_AREA(e,        0, 80,  60,  40);
    NOIA_ASSERT_FRAME_AREA(f,       60, 80,  60,  40);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// When relaxing horizontal frame check if:
///
/// - main frame is relaxed
/// - subframes have proper sizes
/// - non-horizontal subframes are not relaxed
/// - horizontal subframes are relaxed if and only if the changed size
///
NoiaTestResult should_relax_horizontal(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RELAXING;

    noia_frame_relax(r, co);

    NOIA_ASSERT_FRAME_AREA(r,        0,   0, 360, 360);
    NOIA_ASSERT_FRAME_AREA(abcdef,   0,   0, 120, 360);
    NOIA_ASSERT_FRAME_AREA(g,      120,   0, 120, 360);
    NOIA_ASSERT_FRAME_AREA(hi,     240,   0, 120, 360);
    NOIA_ASSERT_FRAME_AREA(a,        0,   0, 120, 120);
    NOIA_ASSERT_FRAME_AREA(bcd,      0, 120, 120, 180);
    NOIA_ASSERT_FRAME_AREA(ef,       0, 300, 120,  60);
    NOIA_ASSERT_FRAME_AREA(h,      240,   0,  80, 360);
    NOIA_ASSERT_FRAME_AREA(i,      320,   0,  40, 360);
    NOIA_ASSERT_FRAME_AREA(b,        0, 120, 120, 180);
    NOIA_ASSERT_FRAME_AREA(c,        0, 120, 120, 180);
    NOIA_ASSERT_FRAME_AREA(d,        0, 120, 120, 180);
    NOIA_ASSERT_FRAME_AREA(e,        0, 300,  60,  60);
    NOIA_ASSERT_FRAME_AREA(f,       60, 300,  60,  60);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// When changing type of horizontal frame to stacked check if:
///
/// - main frame is relaxed and have correct type
/// - subframes have proper sizes
///
NoiaTestResult should_change_type_to_stacked(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RELAXING;

    noia_frame_change_type(r, co, NOIA_FRAME_TYPE_STACKED);

    NOIA_ASSERT_FRAME_AREA(r,      0,   0, 360, 360);
    NOIA_ASSERT_FRAME_AREA(abcdef, 0,   0, 360, 360);
    NOIA_ASSERT_FRAME_AREA(g,      0,   0, 360, 360);
    NOIA_ASSERT_FRAME_AREA(hi,     0,   0, 360, 360);
    NOIA_ASSERT_FRAME_AREA(a,      0,   0, 360, 120);
    NOIA_ASSERT_FRAME_AREA(bcd,    0, 120, 360, 180);
    NOIA_ASSERT_FRAME_AREA(ef,     0, 300, 360,  60);
    NOIA_ASSERT_FRAME_AREA(h,      0,   0, 180, 360);
    NOIA_ASSERT_FRAME_AREA(i,    180,   0, 180, 360);
    NOIA_ASSERT_FRAME_AREA(b,      0, 120, 360, 180);
    NOIA_ASSERT_FRAME_AREA(c,      0, 120, 360, 180);
    NOIA_ASSERT_FRAME_AREA(d,      0, 120, 360, 180);
    NOIA_ASSERT_FRAME_AREA(e,      0, 300, 180,  60);
    NOIA_ASSERT_FRAME_AREA(f,    180, 300, 180,  60);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// When changing type of horizontal frame to vertical check if:
///
/// - main frame is relaxed and have correct type
/// - subframes have proper sizes
///
NoiaTestResult should_change_type_to_vertical(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_RELAXING;

    noia_frame_change_type(r, co, NOIA_FRAME_TYPE_VERTICAL);

    NOIA_ASSERT_FRAME_AREA(r,      0,   0, 360, 360);
    NOIA_ASSERT_FRAME_AREA(abcdef, 0,   0, 360, 120);
    NOIA_ASSERT_FRAME_AREA(g,      0, 120, 360, 120);
    NOIA_ASSERT_FRAME_AREA(hi,     0, 240, 360, 120);
    NOIA_ASSERT_FRAME_AREA(a,      0,   0, 360,  40);
    NOIA_ASSERT_FRAME_AREA(bcd,    0,  40, 360,  40);
    NOIA_ASSERT_FRAME_AREA(ef,     0,  80, 360,  40);
    NOIA_ASSERT_FRAME_AREA(h,      0, 240, 180, 120);
    NOIA_ASSERT_FRAME_AREA(i,    180, 240, 180, 120);
    NOIA_ASSERT_FRAME_AREA(b,      0,  40, 360,  40);
    NOIA_ASSERT_FRAME_AREA(c,      0,  40, 360,  40);
    NOIA_ASSERT_FRAME_AREA(d,      0,  40, 360,  40);
    NOIA_ASSERT_FRAME_AREA(e,      0,  80, 180,  40);
    NOIA_ASSERT_FRAME_AREA(f,    180,  80, 180,  40);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check if new frame if correctly inserted before given frame.
NoiaTestResult should_jumpin_before(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_SIMPLE;

    NoiaFrame* f = noia_frame_new();
    NoiaArea area = {{0,0},{0,0}};
    noia_frame_configure(f, co, NOIA_FRAME_TYPE_LEAF, 66, area, "test");
    noia_frame_jumpin(f, NOIA_FRAME_POSITION_BEFORE, v3, co);

    NOIA_ASSERT_FRAME_TRUNK(f,  v);
    NOIA_ASSERT_FRAME_TRUNK(v3, v);
    NOIA_ASSERT_FRAME_TRUNK(v2, v);
    NOIA_ASSERT_FRAME_TRUNK(v,  r);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r, 3u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(v, 6u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(h, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(s, 5u);

    NoiaFrame* twig = noia_frame_get_first(v);
    NOIA_ASSERT_FRAME_POINTER(twig, v1);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, v2);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, f);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, v3);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, v4);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, v5);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, NULL);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check if new frame if correctly inserted after given frame.
NoiaTestResult should_jumpin_after(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_SIMPLE;

    NoiaFrame* f = noia_frame_new();
    NoiaArea area = {{0,0},{0,0}};
    noia_frame_configure(f, co, NOIA_FRAME_TYPE_LEAF, 66, area, "test");
    noia_frame_jumpin(f, NOIA_FRAME_POSITION_AFTER, v3, co);

    NOIA_ASSERT_FRAME_TRUNK(f,  v);
    NOIA_ASSERT_FRAME_TRUNK(v3, v);
    NOIA_ASSERT_FRAME_TRUNK(v4, v);
    NOIA_ASSERT_FRAME_TRUNK(v,  r);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r, 3u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(v, 6u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(h, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(s, 5u);

    NoiaFrame* twig = noia_frame_get_first(v);
    NOIA_ASSERT_FRAME_POINTER(twig, v1);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, v2);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, v3);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, f);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, v4);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, v5);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, NULL);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}


//------------------------------------------------------------------------------

/// Check if new frame if correctly inserted on given frame.
NoiaTestResult should_jumpin_on(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_SIMPLE;

    NoiaFrame* f = noia_frame_new();
    NoiaArea area = {{0,0},{0,0}};
    noia_frame_configure(f, co, NOIA_FRAME_TYPE_LEAF, 66, area, "test");
    noia_frame_jumpin(f, NOIA_FRAME_POSITION_ON, v3, co);

    NOIA_ASSERT_FRAME_POINTER(noia_frame_get_trunk(v3),noia_frame_get_trunk(f));
    NOIA_ASSERT_FRAME_TRUNK(noia_frame_get_trunk(v3), v);
    NOIA_ASSERT_FRAME_TRUNK(v, r);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(r, 3u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(v, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(h, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(s, 5u);
    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(noia_frame_get_trunk(f), 2u);

    NOIA_ASSERT_FRAME_PARAMETERS(v3, 13ul, NOIA_FRAME_TYPE_LEAF,
                                 0, 24, 60, 12, "v3");

    NoiaFrame* twig = noia_frame_get_first(noia_frame_get_trunk(f));
    NOIA_ASSERT_FRAME_POINTER(twig, v3);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, f);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, NULL);

    twig = noia_frame_get_first(v);
    NOIA_ASSERT_FRAME_POINTER(twig, v1);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, v2);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, noia_frame_get_trunk(f));
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, v4);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, v5);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, NULL);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check if new frame if correctly inserted after given frame.
NoiaTestResult should_jump_after_on_the_same_level(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_JUMPING;

    noia_frame_jump(f, NOIA_FRAME_POSITION_AFTER, ghi, co);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(fghi, 2u);
    NoiaFrame* twig = noia_frame_get_first(fghi);
    NOIA_ASSERT_FRAME_POINTER(twig, ghi);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, f);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, NULL);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Check if new frame if correctly inserted before given frame.
NoiaTestResult should_jump_before_on_the_same_level(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_FOR_JUMPING;

    noia_frame_jump(fghi, NOIA_FRAME_POSITION_BEFORE, abcde, co);

    NOIA_ASSERT_FRAME_NUMBER_OF_SUBFRAMES(fghi, 2u);
    NoiaFrame* twig = noia_frame_get_first(r);
    NOIA_ASSERT_FRAME_POINTER(twig, fghi);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, abcde);
    twig = noia_frame_get_next(twig);
    NOIA_ASSERT_FRAME_POINTER(twig, NULL);

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

/// Empty test
NoiaTestResult should(void)
{
    NoiaCoordinator* co = noia_coordinator_mock_new();
    NOIA_MAKE_FRAMES_POSITIONED;

    // ...

    noia_frame_free(r);
    noia_coordinator_mock_free(co);
    return NOIA_TEST_SUCCESS;
}

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    NOIA_INIT_TESTS;

    NoiaTest test[] = {
            NOIA_TEST(should),

            // general operations
            NOIA_TEST(should_append_and_prepend_values),
            NOIA_TEST(should_insert_values),
            NOIA_TEST(should_translate_root_frame_to_array),
            NOIA_TEST(should_translate_twig_frame_to_array),
            NOIA_TEST(should_remove_some_leaf_frames),
            NOIA_TEST(should_remove_frame_with_subframes),
            NOIA_TEST(should_resettle_one_frame),
            NOIA_TEST(should_resettle_frame_with_subframes),
            NOIA_TEST(should_swap_frames),
            NOIA_TEST(should_pop_frame_recursively),
            NOIA_TEST(should_ramify_leaf),
            NOIA_TEST(should_ramify_nonleaf),
            NOIA_TEST(should_deramify),
            NOIA_TEST(should_deramify_with_one_leaf),
            NOIA_TEST(should_not_deramify_not_single),
            NOIA_TEST(should_not_deramify_single_with_many_leafs),

            // finding
            NOIA_TEST(should_find_with_sid),
            NOIA_TEST(should_find_top),

            // finding contiguous
            NOIA_TEST(should_find_contiguous_on_the_same_level_one_further),
            NOIA_TEST(should_find_contiguous_on_the_same_level_many_further),
            NOIA_TEST(should_find_contiguous_on_the_second_level),
            NOIA_TEST(should_find_contiguous_on_the_third_level),

            // finding pointed
            NOIA_TEST(should_find_stacked_pointed_inside),
            NOIA_TEST(should_find_flat_pointed_inside),
            NOIA_TEST(should_find_stacked_pointed_outside),
            NOIA_TEST(should_find_flat_pointed_outside),
            NOIA_TEST(should_find_frame_over_another),
            NOIA_TEST(should_find_frame_in_empty_space),
            NOIA_TEST(should_find_adjacent_frames),

            // resizing
            NOIA_TEST(check_setup_for_resizing_with_horizontal_floating),
            NOIA_TEST(check_setup_for_resizing_with_vertical_floating),
            NOIA_TEST(should_shrink_from_south),
            NOIA_TEST(should_expand_to_south),
            NOIA_TEST(should_shrink_from_north),
            NOIA_TEST(should_expand_to_north),
            NOIA_TEST(should_shrink_from_east),
            NOIA_TEST(should_expand_to_east),
            NOIA_TEST(should_shrink_from_west),
            NOIA_TEST(should_expand_to_west),
            NOIA_TEST(should_vertical_shrink_from_north),
            NOIA_TEST(should_vertical_expand_to_north),
            NOIA_TEST(should_horizontal_shrink_from_west),
            NOIA_TEST(should_horizontal_expand_to_west),

            // moving
            NOIA_TEST(should_move_to_north_west),
            NOIA_TEST(should_move_to_south_east),
            NOIA_TEST(should_move_from_north_east),
            NOIA_TEST(should_not_move_when_frame_is_not_floating),

            // relaxing
            NOIA_TEST(should_relax_horizontal),
            NOIA_TEST(should_change_type_to_stacked),
            NOIA_TEST(should_change_type_to_vertical),

            // jumping
            NOIA_TEST(should_jumpin_before),
            NOIA_TEST(should_jumpin_after),
            NOIA_TEST(should_jumpin_on),
            NOIA_TEST(should_jump_after_on_the_same_level),
            NOIA_TEST(should_jump_before_on_the_same_level),
        };

    return noia_test_run("Frame", test, NOIA_NUM_TESTS(test));
}

//------------------------------------------------------------------------------

