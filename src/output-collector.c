// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "output-collector.h"

#include "event-signals.h"
#include "utils-log.h"
#include "device-drm.h"
#include "device-fb.h"
#include "output-drm.h"
#include "backend-offscreen.h"
#include "config.h"

#include <limits.h>
#include <memory.h>

//------------------------------------------------------------------------------

/// List of all currently available outputs.
/// @todo This should not be a global variable.
NoiaList* outputs = NULL;

//------------------------------------------------------------------------------

/// Find space in global output coordinates to place new output.
///
/// @todo Currently it places new output to the right of the most right one.
///       This behavior should be more configurable.
NoiaPosition noia_output_collector_allocate_position(void)
{
    NoiaPosition result = {INT_MIN, INT_MIN};
    FOR_EACH (outputs, link) {
        NoiaOutput* output = (NoiaOutput*) link->data;
        NoiaArea area = noia_output_get_area(output);
        int x = area.pos.x + area.size.width;
        if (x > result.x) {
            result.x = x + 10;
            result.y = area.pos.y;
        }
    }

    if (result.x == INT_MIN) {
        result.x = 0;
        result.y = 0;
    }
    return result;
}

//------------------------------------------------------------------------------

/// Create DRM output from DRM bundles.
int noia_output_collector_update_drm_devices(NoiaList* actual_outputs)
{
    NoiaPool* drm_bundles = noia_pool_create(8, sizeof(NoiaDRMBundle));
    noia_drm_update_devices(drm_bundles);

    unsigned i;
    NoiaDRMBundle* bundle;
    NOIA_ITERATE_POOL(drm_bundles, i, bundle) {
        NoiaOutputDRM* output_drm = noia_output_drm_create(bundle);
        noia_list_append(actual_outputs, output_drm);
    }

    int num = noia_pool_get_size(drm_bundles);
    noia_pool_destroy(drm_bundles);
    return num;
}

//------------------------------------------------------------------------------

/// Fetch all available (active) outputs from all possible sources.
/// If running in simulator, fetch only simulated outputs.
NoiaList* noia_output_collector_fetch_actual_outputs(void)
{
    int num = 0;
    NoiaList* actual_outputs = noia_list_new(NULL);

    if (!noia_settings()->run_in_test_mode) {
        if (noia_settings()->use_drm) {
            num = noia_output_collector_update_drm_devices(actual_outputs);
        }
        if (num < 1) {
            num = noia_devfb_setup_framebuffer(actual_outputs);
        }
    } else {
        num = noia_backend_offscreen_get_outputs(actual_outputs);
    }

    if (num < 1) {
        LOG_WARN1("No valid outputs!");
    }

    return actual_outputs;
}

//------------------------------------------------------------------------------

/// Notify about newly found outputs.
/// @param found_outputs - a list of outputs to notify about
void noia_output_collector_notify_outputs_found(NoiaList* found_outputs)
{
    FOR_EACH (found_outputs, link) {
        NoiaOutput* output = (NoiaOutput*) link->data;
        if (not output) {
            LOG_WARN1("Invalid output found!");
            continue;
        }

        // Assign position to output
        NoiaPosition pos = noia_output_collector_allocate_position();
        noia_output_set_position(output, pos);
        noia_list_append(outputs, output);

        // Notify about new output
        noia_event_signal_emit(SIGNAL_OUTPUT_FOUND, (NoiaObject*) output);
    }
}

//------------------------------------------------------------------------------

/// Notify about lost outputs.
/// @param lost_outputs - a list of outputs to notify about
void noia_output_collector_notify_outputs_lost(NoiaList* lost_outputs)
{
    FOR_EACH (lost_outputs, link) {
        NoiaOutput* output = (NoiaOutput*) link->data;
        if (not output) {
            LOG_WARN1("Invalid output found!");
            continue;
        }

        LOG_INFO1("Removing output '%s'", noia_output_get_name(output));
        noia_list_remove(outputs, output, (NoiaCompareFunc)noia_output_compare);
        noia_event_signal_emit(SIGNAL_OUTPUT_LOST, (NoiaObject*) output);
    }
}

//------------------------------------------------------------------------------

/// Update outputs:
/// -# fetch all outputs
/// -# compare with existing outputs
/// -# process findings and loses
void noia_output_collector_update(void)
{
    LOG_INFO1("Updating outputs");

    NoiaList* actual_outputs = noia_output_collector_fetch_actual_outputs();

    NoiaList* found_outputs = noia_list_subtract(actual_outputs, outputs,
                                   (NoiaCompareFunc) noia_output_compare, NULL);

    NoiaList* lost_outputs = noia_list_subtract(outputs, actual_outputs,
                                   (NoiaCompareFunc) noia_output_compare, NULL);

    noia_output_collector_notify_outputs_found(found_outputs);
    noia_output_collector_notify_outputs_lost(lost_outputs);

    noia_list_free(found_outputs);
    noia_list_free(lost_outputs);
    noia_list_free(actual_outputs);
}

//------------------------------------------------------------------------------

/// Handle SIGNAL_OUTPUTS_CHANGED signal.
void noia_output_on_change(void* edata NOIA_UNUSED, void* sdata NOIA_UNUSED)
{
    noia_output_collector_update();
}

//------------------------------------------------------------------------------

/// Finalize loop.
void noia_output_collector_finalize(void* edata NOIA_UNUSED,
                                    void* sdata NOIA_UNUSED)
{
    noia_list_free(outputs);
}

//------------------------------------------------------------------------------

/// Initialize Output Collector.
void noia_output_collector_initialize(NoiaLoop* this_loop)
{
    NOIA_ENSURE(this_loop, return);

    // initialize
    outputs = noia_list_new(NULL);
    noia_output_collector_update();

    // subscribe for signals
    noia_event_signal_subscribe(SIGNAL_OUTPUTS_CHANGED,
         noia_task_create(noia_output_on_change, this_loop, NULL));

    noia_loop_add_finalizer(this_loop, noia_output_collector_finalize, NULL);
}

//------------------------------------------------------------------------------

