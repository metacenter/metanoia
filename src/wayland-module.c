// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "wayland-module.h"
#include "wayland-context.h"
#include "wayland-gateway.h"
#include "wayland-facade.h"

#include "event-signals.h"
#include "utils-log.h"
#include "utils-time.h"
#include "global-objects.h"

//------------------------------------------------------------------------------

void noia_wayland_module_on_screen_refresh(void* edata, void* sdata)
{
    NoiaWaylandContext* ctx = (NoiaWaylandContext*) sdata;
    NoiaSurfaceId sid = noia_uint_unref_get((NoiaIntObject*) edata);
    LOG_WAYL4("Wayland: handling screen refresh");
    NoiaMilliseconds ms = noia_time_get_monotonic_milliseconds();
    noia_wayland_gateway_screen_refresh(ctx->cache, sid, ms);
}

//------------------------------------------------------------------------------

void noia_wayland_module_on_keyboard_focus_changed(void* edata, void* sdata)
{
    NoiaWaylandContext* ctx = (NoiaWaylandContext*) sdata;
    NoiaSurfaceId sid = noia_uint_unref_get((NoiaIntObject*) edata);
    LOG_WAYL2("Wayland: handling keyboard focus change (sid: %d)", sid);
    noia_wayland_gateway_keyboard_focus_update
                   (ctx->state, ctx->cache, ctx->engine, ctx->coordinator, sid);
}

//------------------------------------------------------------------------------

void noia_wayland_module_on_keyboard_event(void* edata, void* sdata)
{
    LOG_WAYL4("Wayland: handling keyboard event");

    NoiaWaylandContext* ctx = (NoiaWaylandContext*) sdata;
    NoiaKeyObject* object = (NoiaKeyObject*) edata;
    NOIA_ENSURE(object, return);
    noia_wayland_gateway_key(ctx->state,
                             ctx->cache,
                             ctx->engine,
                             object->keydata.time,
                             object->keydata.code,
                             object->keydata.value);
    noia_object_unref((NoiaObject*) object);
}

//------------------------------------------------------------------------------

void noia_wayland_module_on_pointer_focus_changed(void* edata, void* sdata)
{
    LOG_WAYL4("Wayland: handling pointer focus change");
    NoiaWaylandContext* ctx = (NoiaWaylandContext*) sdata;
    NoiaMotionObject* object = (NoiaMotionObject*) edata;
    NOIA_ENSURE(object, return);
    noia_wayland_gateway_pointer_focus_update
                (ctx->state, ctx->cache, ctx->engine, object->sid, object->pos);
    noia_object_unref((NoiaObject*) object);
}

//------------------------------------------------------------------------------

void noia_wayland_module_on_pointer_relative_motion(void* edata, void* sdata)
{
    LOG_WAYL4("Wayland: handling pointer motion");
    NoiaWaylandContext* ctx = (NoiaWaylandContext*) sdata;
    NoiaMotionObject* object = (NoiaMotionObject*) edata;
    NOIA_ENSURE(object, return);
    NoiaMilliseconds ms = noia_time_get_monotonic_milliseconds();
    noia_wayland_gateway_pointer_motion
                          (ctx->cache, object->sid, object->pos, ms);
    noia_object_unref((NoiaObject*) object);
}

//------------------------------------------------------------------------------

void noia_wayland_module_on_pointer_button(void* edata, void* sdata)
{
    LOG_WAYL4("Wayland: handling pointer button");
    NoiaWaylandContext* ctx = (NoiaWaylandContext*) sdata;
    NoiaButtonObject* object = (NoiaButtonObject*) edata;
    NOIA_ENSURE(object, return);
    noia_wayland_gateway_pointer_button(ctx->state,
                                        ctx->cache,
                                        ctx->engine,
                                        object->buttondata.time,
                                        object->buttondata.code,
                                        object->buttondata.value);
    noia_object_unref((NoiaObject*) object);
}

//------------------------------------------------------------------------------

void noia_wayland_module_on_surface_reconfigured(void* edata, void* sdata)
{
    NoiaWaylandContext* ctx = (NoiaWaylandContext*) sdata;
    NoiaSurfaceId sid = noia_uint_unref_get((NoiaIntObject*) edata);
    noia_wayland_gateway_surface_reconfigured
                   (ctx->state, ctx->cache, ctx->engine, ctx->coordinator, sid);
}

//------------------------------------------------------------------------------

void noia_wayland_module_on_output_found(void* edata, void* sdata)
{
    NoiaWaylandContext* ctx = (NoiaWaylandContext*) sdata;
    NoiaOutput* output = (NoiaOutput*) edata;
    NOIA_ENSURE(output, return);
    noia_wayland_engine_advertise_output(ctx->engine, output);
    noia_object_unref((NoiaObject*) output);
}

//------------------------------------------------------------------------------

void noia_wayland_module_on_output_lost(void* edata, void* sdata)
{
    NoiaWaylandContext* ctx = (NoiaWaylandContext*) sdata;
    NoiaOutput* output = (NoiaOutput*) edata;
    NOIA_ENSURE(output, return);
    noia_wayland_engine_destroy_output(ctx->engine, output);
}

//------------------------------------------------------------------------------

void noia_wayland_module_finalize(void* edata NOIA_UNUSED, void* sdata)
{
    NoiaWaylandContext* ctx = (NoiaWaylandContext*) sdata;
    noia_wayland_facade_finalize();
    noia_wayland_context_finalize(ctx);
    noia_wayland_context_free(ctx);
}

//------------------------------------------------------------------------------

void noia_wayland_initialize(NoiaLoop* this_loop, NoiaCoordinator* coordinator)
{
    LOG_INFO1("Initializing Wayland...");
    NOIA_ENSURE(this_loop, return);

    // Init Wayland
    NoiaWaylandContext* ctx = noia_wayland_context_new();
    NoiaResult result = noia_wayland_context_initialize(ctx, coordinator);
    if (result != NOIA_RESULT_SUCCESS) {
        LOG_ERROR("Initializing Wayland: Failed to create context!");
        noia_wayland_context_finalize(ctx);
        return;
    }
    noia_wayland_facade_initialize(ctx);

    // Subscribe for events
    noia_event_signal_subscribe(SIGNAL_SCREEN_REFRESH,
               noia_task_create(noia_wayland_module_on_screen_refresh,
                                this_loop, ctx));

    noia_event_signal_subscribe(SIGNAL_KEYBOARD_FOCUS_CHANGED,
               noia_task_create(noia_wayland_module_on_keyboard_focus_changed,
                                this_loop, ctx));

    noia_event_signal_subscribe(SIGNAL_KEYBOARD_EVENT,
               noia_task_create(noia_wayland_module_on_keyboard_event,
                                this_loop, ctx));

    noia_event_signal_subscribe(SIGNAL_POINTER_FOCUS_CHANGED,
               noia_task_create(noia_wayland_module_on_pointer_focus_changed,
                                this_loop, ctx));

    noia_event_signal_subscribe(SIGNAL_POINTER_RELATIVE_MOTION,
               noia_task_create(noia_wayland_module_on_pointer_relative_motion,
                                this_loop, ctx));

    noia_event_signal_subscribe(SIGNAL_POINTER_BUTTON,
               noia_task_create(noia_wayland_module_on_pointer_button,
                                this_loop, ctx));

    noia_event_signal_subscribe(SIGNAL_SURFACE_RECONFIGURED,
               noia_task_create(noia_wayland_module_on_surface_reconfigured,
                                this_loop, ctx));

    noia_event_signal_subscribe(SIGNAL_OUTPUT_FOUND,
               noia_task_create(noia_wayland_module_on_output_found,
                                this_loop, ctx));

    noia_event_signal_subscribe(SIGNAL_OUTPUT_LOST,
               noia_task_create(noia_wayland_module_on_output_lost,
                                this_loop, ctx));

    noia_loop_add_finalizer(this_loop, noia_wayland_module_finalize, ctx);

    LOG_INFO1("Initializing Wayland: SUCCESS");
}

//------------------------------------------------------------------------------

