// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EXHIBITOR_COMPOSITOR_H
#define NOIA_EXHIBITOR_COMPOSITOR_H

#include "exhibitor-frame.h"
#include "exhibitor.h"

/// Compositor constructor.
NoiaCompositor* noia_compositor_new(NoiaExhibitor* exhibitor);

/// Compositor destructor.
void noia_compositor_free(NoiaCompositor* self);

/// Creates new frame, places it in proper place in frame tree and initializes
/// is as a display frame.
NoiaFrame* noia_compositor_create_new_display(NoiaCompositor* self,
                                              NoiaArea area,
                                              const char* title);

/// Creates new workspace with not existing name.
NoiaFrame* noia_compositor_create_next_workspace(NoiaCompositor* self,
                                                 NoiaFrame* display);

/// Handles creation of new surface.
bool noia_compositor_manage_surface(NoiaCompositor* self, NoiaSurfaceId sid);

/// Handles destruction of surface.
void noia_compositor_unmanage_surface(NoiaCompositor* self, NoiaSurfaceId sid);

/// Pop up a surface if covered with others.
/// @see noia_frame_find_with_sid
void noia_compositor_pop_surface(NoiaCompositor* self, NoiaSurfaceId sid);

/// Moves, jumps, resizes, changes order, anchorizes, changes parameters
/// of currently selected frame basing on given action.
/// @see NoiaCommand
void noia_compositor_execute(NoiaCompositor* self, NoiaCommand* command);

#endif // NOIA_EXHIBITOR_COMPOSITOR_H

