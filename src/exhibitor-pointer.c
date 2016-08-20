// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "exhibitor-pointer.h"
#include "exhibitor-display.h"
#include "exhibitor.h"

#include "utils-log.h"
#include "surface-coordinator.h"
#include "event-signals.h"

#include <pthread.h>

#define DEFAULT_CURSOR_SIZE 15

//------------------------------------------------------------------------------
// PRIVATE

/// @see NoiaPointer
struct NoiaPointerStruct {
    /// Position in global coordinates.
    NoiaPosition position;

    /// Last position received from input device.
    NoiaPosition last_pos;

    /// Last relative position inside focused surface.
    NoiaPosition last_rel;

    /// Area of display on which the pointer is placed.
    NoiaArea display_area;

    /// Surface ID of cursor surface.
    NoiaSurfaceId csid;

    /// Surface ID of pointer-focused surface.
    NoiaSurfaceId pfsid;

    /// Surface ID of keyboard-focused surface.
    NoiaSurfaceId kfsid;

    /// Default surface ID of cursor surface.
    NoiaSurfaceId default_csid;

    /// Reference to coordinator.
    NoiaCoordinator* coordinator;

    /// Pointer functions may be called from display threads.
    pthread_mutex_t mutex;
};

//------------------------------------------------------------------------------

/// Find the output inside which output pointer is places.
/// If found - nothing to do.
/// If no output found - update cursor position with closest on inside
/// previously active output.
/// @see noia_exhibitor_pointer_on_motion
NoiaPosition noia_exhibitor_pointer_cast(NoiaPointer* self,
                                         NoiaExhibitor* exhibitor,
                                         NoiaPosition position)
{
    NOIA_BLOCK {
        if (noia_position_is_inside(position, self->display_area)) {
            // Nothing to do - pointer inside active output
            break;
        }

        bool found = false;
        NoiaList* displays = noia_exhibitor_get_displays(exhibitor);
        FOR_EACH (displays, link) {
            NoiaDisplay* display = (NoiaDisplay*) link->data;
            NoiaArea area = noia_display_get_area(display);
            if (noia_position_is_inside(position, area)) {
                // Set new active output and exit
                self->display_area = area;
                found = true;
                break;
            }
        }

        if (not found) {
            // Pointer outside any known output - cast it to the previous active
            position = noia_position_cast(position, self->display_area);
        }
    }

    return position;
}

//------------------------------------------------------------------------------
// PUBLIC

NoiaPointer* noia_exhibitor_pointer_new(NoiaExhibitor* exhibitor)
{
    NoiaPointer* self = malloc(sizeof(NoiaPointer));

    self->position.x = 100;
    self->position.y = 100;
    self->last_pos.x = scInvalidPointerPosition;
    self->last_pos.y = scInvalidPointerPosition;
    self->last_rel.x = scInvalidPointerPosition;
    self->last_rel.y = scInvalidPointerPosition;
    noia_area_invalidate(&self->display_area);
    self->csid = scInvalidSurfaceId;
    self->pfsid = scInvalidSurfaceId;
    self->kfsid = scInvalidSurfaceId;
    self->default_csid = scInvalidSurfaceId;
    self->coordinator = noia_exhibitor_get_coordinator(exhibitor);
    pthread_mutex_init(&self->mutex, NULL);

    return self;
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_free(NoiaPointer* self)
{
    NOIA_ENSURE(self, return);
    free(self);
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_setup(NoiaPointer* self)
{
    NOIA_ENSURE(self, return);

    uint8_t data[4 * DEFAULT_CURSOR_SIZE * DEFAULT_CURSOR_SIZE];
    for (int z = 0; z < (DEFAULT_CURSOR_SIZE * DEFAULT_CURSOR_SIZE); ++z) {
        int p = 4 * z;
        data[p+0] = 255;
        data[p+1] = 255;
        data[p+2] = 255;
        data[p+3] = 100;
    }

    self->default_csid = noia_surface_create(self->coordinator);
    noia_surface_attach(self->coordinator, self->default_csid,
                        DEFAULT_CURSOR_SIZE, DEFAULT_CURSOR_SIZE,
                        4 * DEFAULT_CURSOR_SIZE, data, NULL);
    noia_surface_commit(self->coordinator, self->default_csid);
    self->csid = self->default_csid;
}

//------------------------------------------------------------------------------

NoiaPosition noia_exhibitor_pointer_get_global_position(NoiaPointer* self)
{
    return self->position;
}

//------------------------------------------------------------------------------

NoiaSurfaceId noia_exhibitor_pointer_get_sid(NoiaPointer* self)
{
    return self->csid;
}

//------------------------------------------------------------------------------

/// @note Applications ignore excess motion events and react only to last before
///       frame to avoid constant redrawing therefore motion event is emitted
///       here (just before display redraw) instead of in reaction to every
///       input event (where we only ensure pointer is inside display).
void noia_exhibitor_pointer_update_hover_state(NoiaPointer* self,
                                               NoiaArea display_area,
                                               NoiaPool* visible_surfaces)
{
    // Check if this update is for display on which this pointer is placed
    if (not noia_area_is_equal(self->display_area, display_area)) {
        return;
    }

    pthread_mutex_lock(&self->mutex);

    NoiaPosition relative_position = {self->position.x - display_area.pos.x,
                                      self->position.y - display_area.pos.y};

    NoiaPosition rel = {scInvalidPointerPosition, scInvalidPointerPosition};
    NoiaSurfaceId sid = scInvalidSurfaceId;
    NoiaArea surface_area;
    noia_area_invalidate(&surface_area);

    // Find surface pointer hovers
    int i;
    NoiaSurfaceContext* context;
    NOIA_ITERATE_POOL_REVERSE(visible_surfaces, i, context) {
        NoiaSurfaceData* data = noia_surface_get(self->coordinator,
                                                 context->sid);
        if (data) {
            surface_area.pos.x = context->pos.x;
            surface_area.pos.y = context->pos.y;
            surface_area.size = data->desired_size;
            if (noia_position_is_inside(relative_position, surface_area)) {
                sid = context->sid;
                rel.x = relative_position.x - context->pos.x + data->offset.x;
                rel.y = relative_position.y - context->pos.y + data->offset.y;
                break;
            }
        }
    }

    // Handle focus change if hovered surface is different than current one
    // or handle motion otherwise
    if (sid != self->pfsid) {
        LOG_INFO2("Pointer focus changed "
                  "(old sid: %d, new sid: %d, x: %d, y: %d)",
                  self->pfsid, sid, rel.x, rel.y);
        self->pfsid = sid;
        self->csid = self->default_csid;
        NoiaObject* object = (NoiaObject*) noia_motion_create(sid, rel);
        noia_event_signal_emit(SIGNAL_POINTER_FOCUS_CHANGED, object);
        noia_object_unref(object);
    } else if ((self->pfsid != scInvalidSurfaceId)
           and ((rel.x != self->last_rel.x) or (rel.y != self->last_rel.y))) {
        LOG_INFO3("Pointer surface relative position (x: %d, y: %d)",
                  rel.x, rel.y);
        NoiaObject* object = (NoiaObject*) noia_motion_create(sid, rel);
        noia_event_signal_emit(SIGNAL_POINTER_RELATIVE_MOTION, object);
        noia_object_unref(object);
        self->last_rel = rel;
    }

    pthread_mutex_unlock(&self->mutex);
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_on_motion(NoiaPointer* self,
                                      NoiaExhibitor* exhibitor,
                                      NoiaPosition vector)
{
    pthread_mutex_lock(&self->mutex);

    self->position.x += vector.x;
    self->position.y += vector.y;
    self->position =
                   noia_exhibitor_pointer_cast(self, exhibitor, self->position);

    noia_coordinator_notify(self->coordinator);
    pthread_mutex_unlock(&self->mutex);
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_on_position_reset(NoiaPointer* self)
{
    pthread_mutex_lock(&self->mutex);

    self->last_pos.x = scInvalidPointerPosition;
    self->last_pos.y = scInvalidPointerPosition;

    pthread_mutex_unlock(&self->mutex);
}

//------------------------------------------------------------------------------

/// Pointer moved: update position taking into account output boundaries.
void noia_exhibitor_pointer_on_position(NoiaPointer* self,
                                        NoiaExhibitor* exhibitor,
                                        NoiaPosition pos)
{
    pthread_mutex_lock(&self->mutex);

    // Update X-axis part of position
    if (pos.x != scInvalidPointerPosition) {
        if (self->last_pos.x != scInvalidPointerPosition) {
            self->position.x += pos.x - self->last_pos.x;
            self->position =
                   noia_exhibitor_pointer_cast(self, exhibitor, self->position);
        }
        self->last_pos.x = pos.x;
    }

    // Update Y-axis part of position
    if (pos.y != scInvalidPointerPosition) {
        if (self->last_pos.y != scInvalidPointerPosition) {
            self->position.y += pos.y - self->last_pos.y;
            self->position =
                   noia_exhibitor_pointer_cast(self, exhibitor, self->position);
        }
        self->last_pos.y = pos.y;
    }

    // Notify coordinator screen must be redrawn
    noia_coordinator_notify(self->coordinator);
    pthread_mutex_unlock(&self->mutex);
}

//------------------------------------------------------------------------------

/// User pressed mouse button: if currently focused surface is not the same
/// pointer hovers - pop clicked surface Do not update `self->kfsid` - if pop
/// succeeds we will be notified.
void noia_exhibitor_pointer_on_button(NoiaPointer* self,
                                      NoiaExhibitor* exhibitor)
{
    NOIA_ENSURE(self, return);

    pthread_mutex_lock(&self->mutex);

    if (self->kfsid != self->pfsid) {
        NoiaCompositor* compositor = noia_exhibitor_get_compositor(exhibitor);
        noia_compositor_pop_surface(compositor, self->pfsid);
    }

    pthread_mutex_unlock(&self->mutex);
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_on_surface_change(NoiaPointer* self,
                                              NoiaSurfaceId sid)
{
    pthread_mutex_lock(&self->mutex);

    NoiaSurfaceData* surface_data = noia_surface_get(self->coordinator, sid);
    if (surface_data) {
        self->csid = sid;
    }

    noia_coordinator_notify(self->coordinator);
    pthread_mutex_unlock(&self->mutex);
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_on_surface_destroyed(NoiaPointer* self,
                                                 NoiaSurfaceId sid)
{
    pthread_mutex_lock(&self->mutex);

    if (self->csid == sid) {
        self->csid = self->default_csid;
    }

    pthread_mutex_unlock(&self->mutex);
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_on_keyboard_focus_changed(NoiaPointer* self,
                                                      NoiaSurfaceId new_sid)
{
    NOIA_ENSURE(self, return);
    pthread_mutex_lock(&self->mutex);
    self->kfsid = new_sid;
    pthread_mutex_unlock(&self->mutex);
}

//------------------------------------------------------------------------------

