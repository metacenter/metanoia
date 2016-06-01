// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_SURFACE_DATA_H
#define NOIA_SURFACE_DATA_H

#include "utils-store.h"
#include "utils-list.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

typedef enum {
    NOIA_SURFACE_SHOW_DRAWABLE = 0x1,
    NOIA_SURFACE_SHOW_IN_SHELL = 0x2,

    NOIA_SURFACE_SHOW_FULL = NOIA_SURFACE_SHOW_DRAWABLE
                           | NOIA_SURFACE_SHOW_IN_SHELL,
} NoiaSurfaceShowReason;

typedef enum {
    NOIA_SURFACE_STATE_MAXIMIZED  = 0x1,
    NOIA_SURFACE_STATE_FULLSCREEN = 0x2,
    NOIA_SURFACE_STATE_RESIZING   = 0x3,
} NoiaSurfaceState;

/// Container for all data required to draw the surface.
typedef struct {
    int width;
    int height;
    int stride;
    uint8_t* data;
    void* resource;
    EGLImageKHR image;
} NoiaDrawBuffer;

/// Container for all data related to single surface.
typedef struct {
    /// Make this data structure storable in NoiaStore.
    NoiaItem base;

    /// Offset used to move coordinate system of surface.
    NoiaPosition offset;

    /// Size desired by compositor.
    NoiaSize desired_size;

    /// Size requested by client.
    NoiaSize requested_size;

    /// Position requested by client.
    NoiaPosition requested_position;

    /// Position requested by client relative to parent surface.
    /// For surfaces without parent this must be {0, 0}.
    NoiaPosition relative_position;

    /// ID of parent surface.
    NoiaSurfaceId parent_sid;

    /// List of IDs of satelliting surfaces.
    NoiaList* satellites;

    /// Data required for draw.
    NoiaDrawBuffer buffer;

    /// Data to be used after commit.
    NoiaDrawBuffer pending_buffer;

    /// Flags describing logical state of surface
    uint8_t state_flags;

    /// Flags indicating if surface is ready to be shown.
    uint8_t show_flags;
} NoiaSurfaceData;

/// Allocate Surface Data.
NoiaSurfaceData* noia_surface_data_new(void);

/// Deallocate Surface Data.
void noia_surface_data_free(NoiaSurfaceData* self);

#endif // NOIA_SURFACE_DATA_H

