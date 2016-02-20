// file: exhibitor-compositor.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_EXHIBITOR_COMPOSITOR_H
#define NOIA_EXHIBITOR_COMPOSITOR_H

#include "exhibitor-frame.h"
#include "exhibitor.h"

/// NoiaCompositor manages surfaces and decides which ones should be drawn on
/// which output.
typedef struct NoiaCompositorStruct NoiaCompositor;

/// Compositor constructor.
NoiaCompositor* noia_compositor_new(NoiaExhibitor* exhibitor);

/// Compositor destructor.
void noia_compositor_free(NoiaCompositor* self);

/// Creates new frame, places it in proper place in frame tree and initializes
/// is as a workspace.
NoiaFrame* noia_compositor_create_new_workspace(NoiaCompositor* self,
                                                NoiaSize size);

/// Handles creation of new surface.
bool noia_compositor_manage_surface(NoiaCompositor* self, NoiaSurfaceId sid);

/// Handles destruction of surface.
void noia_compositor_unmanage_surface(NoiaCompositor* self, NoiaSurfaceId sid);

/// Pop up a surface if covered with others.
/// @see noia_frame_find_with_sid
void noia_compositor_pop_surface(NoiaCompositor* self, NoiaSurfaceId sid);

/// Moves, jumps, resizes, changes order, anchorizes, changes parameters
/// of currently selected frame basing on given action.
/// @see NoiaAction
void noia_compositor_execute(NoiaCompositor* self, NoiaAction* action);

#endif // NOIA_EXHIBITOR_COMPOSITOR_H

