// file: exhibitor-pointer.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-pointer.h"
#include "exhibitor-display.h"
#include "exhibitor.h"

#include "utils-log.h"
#include "surface-manager.h"
#include "event-signals.h"

#include <pthread.h>

static const int scInvalidPointerValue = -1;

//------------------------------------------------------------------------------
// PRIVATE

struct NoiaPointerStruct {
    /// Position in global coordinates.
    NoiaPosition position;

    /// Last absolute position received from input device.
    NoiaPosition last_abs;

    /// Last relative position inside focused surface.
    NoiaPosition last_rel;

    /// Area of display on which the pointer is placed.
    NoiaArea active_area;

    /// Surface ID of cursor surface.
    NoiaSurfaceId cursor_sid;

    /// Surface ID of focused surface.
    NoiaSurfaceId focused_sid;

    /// Pointer functions may be called from display threads.
    pthread_mutex_t mutex;
};

//------------------------------------------------------------------------------

/// Find the output inside which is pointer.
/// If found - nothing to do.
/// If no output found - update cursor position with closest on inside
/// previously active output.
/// @see noia_exhibitor_pointer_on_motion_x, noia_exhibitor_pointer_on_motion_y
NoiaPosition noia_exhibitor_pointer_cast(NoiaPointer* self,
                                         NoiaExhibitor* exhibitor,
                                         NoiaPosition position)
{
    NOIA_BLOCK {
        if (noia_position_is_inside(position, self->active_area)) {
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
                self->active_area = area;
                found = true;
                break;
            }
        }

        if (not found) {
            // Pointer outside any known output - cast it to the previous active
            position = noia_position_cast(position, self->active_area);
        }
    }

    return position;
}

//------------------------------------------------------------------------------
// PUBLIC

NoiaPointer* noia_exhibitor_pointer_new()
{
    NoiaPointer* self = malloc(sizeof(NoiaPointer));

    self->position.x = 100;
    self->position.y = 100;
    self->last_abs.x = scInvalidPointerValue;
    self->last_abs.y = scInvalidPointerValue;
    self->last_rel.x = scInvalidPointerValue;
    self->last_rel.y = scInvalidPointerValue;
    noia_area_invalidate(&self->active_area);
    self->cursor_sid = scInvalidSurfaceId;
    self->focused_sid = scInvalidSurfaceId;
    pthread_mutex_init(&self->mutex, NULL);

    return self;
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_free(NoiaPointer* self NOIA_UNUSED)
{
    // Nothing to do
}

//------------------------------------------------------------------------------

NoiaPosition noia_exhibitor_pointer_get_global_position(NoiaPointer* self)
{
    return self->position;
}

//------------------------------------------------------------------------------

NoiaSurfaceId noia_exhibitor_pointer_get_sid(NoiaPointer* self)
{
    return self->cursor_sid;
}

//------------------------------------------------------------------------------

/// @todo Find better moment to update hover state.
void noia_exhibitor_pointer_update_hover_state(NoiaPointer* self,
                                               NoiaArea area,
                                               NoiaPool* visible_surfaces)
{
    // Check if this update is for display on which this pointer is placed
    if (not noia_area_is_equal(self->active_area, area)) {
        return;
    }

    pthread_mutex_lock(&self->mutex);

    NoiaPosition relative_position = {self->position.x - area.pos.x,
                                      self->position.y - area.pos.y};

    NoiaPosition rel = {-1, -1};
    NoiaSurfaceId sid = scInvalidSurfaceId;

    int i;
    NoiaSurfaceContext* context;
    NOIA_ITERATE_POOL_REVERSE(visible_surfaces, i, context) {
        NoiaSurfaceData* data = noia_surface_get(context->sid);
        /// @todo Use noia_position_is_inside function
        if (data
        &&  relative_position.x >= context->pos.x
        &&  relative_position.y >= context->pos.y
        &&  relative_position.x <= context->pos.x + data->desired_size.width
        &&  relative_position.y <= context->pos.y + data->desired_size.height) {
            sid = context->sid;
            rel.x = relative_position.x - context->pos.x + data->offset.x;
            rel.y = relative_position.y - context->pos.y + data->offset.y;
            break;
        }
    }

    if (sid != self->focused_sid) {
        LOG_INFO2("Pointer focus changed "
                  "(old sid: %d, new sid: %d, x: %d, y: %d)",
                  self->focused_sid, sid, rel.x, rel.y);
        self->focused_sid = sid;
        self->cursor_sid = scInvalidSurfaceId;
        noia_event_signal_emit(SIGNAL_POINTER_FOCUS_CHANGED,
                               (NoiaObject*) noia_motion_create(sid, rel));
    } else if ((self->focused_sid != scInvalidSurfaceId)
           and ((rel.x != self->last_rel.x) or (rel.y != self->last_rel.y))) {
        LOG_INFO3("Pointer surface relative position (x: %d, y: %d)",
                  rel.x, rel.y);
        noia_event_signal_emit(SIGNAL_POINTER_RELATIVE_MOTION,
                               (NoiaObject*) noia_motion_create(sid, rel));
        self->last_rel = rel;
    }

    pthread_mutex_unlock(&self->mutex);
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_on_motion_reset(NoiaPointer* self)
{
    pthread_mutex_lock(&self->mutex);

    self->last_abs.x = scInvalidPointerValue;
    self->last_abs.y = scInvalidPointerValue;

    pthread_mutex_unlock(&self->mutex);
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_on_motion_x(NoiaPointer* self,
                                        NoiaExhibitor* exhibitor,
                                        int abs_value)
{
    pthread_mutex_lock(&self->mutex);

    if (self->last_abs.x != scInvalidPointerValue) {
        self->position.x += abs_value - self->last_abs.x;
        self->position =
                   noia_exhibitor_pointer_cast(self, exhibitor, self->position);
    }
    self->last_abs.x = abs_value;

    pthread_mutex_unlock(&self->mutex);
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_on_motion_y(NoiaPointer* self,
                                        NoiaExhibitor* exhibitor,
                                        int abs_value)
{
    pthread_mutex_lock(&self->mutex);

    if (self->last_abs.y != scInvalidPointerValue) {
        self->position.y += abs_value - self->last_abs.y;
        self->position =
                   noia_exhibitor_pointer_cast(self, exhibitor, self->position);
    }
    self->last_abs.y = abs_value;

    pthread_mutex_unlock(&self->mutex);
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_on_surface_change(NoiaPointer* self,
                                              NoiaSurfaceId sid)
{
    pthread_mutex_lock(&self->mutex);

    NoiaSurfaceData* surface_data = noia_surface_get(sid);
    if (surface_data) {
        self->cursor_sid = sid;
    }

    pthread_mutex_unlock(&self->mutex);
}

//------------------------------------------------------------------------------

void noia_exhibitor_pointer_on_surface_destroyed(NoiaPointer* self,
                                                 NoiaSurfaceId sid)
{
    pthread_mutex_lock(&self->mutex);

    if (self->cursor_sid == sid) {
        self->cursor_sid = scInvalidSurfaceId;
    }

    pthread_mutex_unlock(&self->mutex);
}

//------------------------------------------------------------------------------

