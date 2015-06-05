// file: output-collector.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "output-collector.h"
#include "event-signals.h"
#include "utils-log.h"
#include "utils-list.h"
#include "device-drm.h"
#include "device-fb.h"
#include "backend-gtk.h"
#include "config.h"

#include <limits.h>
#include <memory.h>

//------------------------------------------------------------------------------

/// List of all currently available outputs.
/// @todo This should not be a global variable.
NoiaList* outputs = 0;

//------------------------------------------------------------------------------

/// Find space in global output coordinates to place new output.
///
/// @todo Currently it places new output to the right of the most right one.
///       This behavior should be more configurable.
NoiaPosition noia_output_collector_allocate_position()
{
    NoiaPosition result = {INT_MIN, INT_MIN};
    FOR_EACH (outputs, link) {
        NoiaOutput* output = (NoiaOutput*) link->data;
        int x = output->global_position.x + output->width;
        if (x > result.x) {
            result.x = x;
            result.y = output->global_position.y;
        }
    }

    if (result.x == INT_MIN) {
        result.x = 0;
        result.y = 0;
    }
    return result;
}

//------------------------------------------------------------------------------

/// Fetch all available (active) outputs from all possible sources.
/// If running in simulator, fetch only simulated outputs.
NoiaList* noia_output_collector_fetch_actual_outputs()
{
    int num = 0;
    NoiaList* actual_outputs = noia_list_new(NULL);

    if (!noia_settings()->run_in_test_window) {
        num = noia_drm_update_devices(actual_outputs);
        if (num < 1) {
            num = noia_devfb_setup_framebuffer(actual_outputs);
        }
    } else {
        num = noia_backend_gtk_get_outputs(actual_outputs);
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
        if (!output || !output->unique_name) {
            LOG_WARN1("Invalid output found!");
            continue;
        }

        // assign position to output
        output->global_position = noia_output_collector_allocate_position();

        // initialize output (create renderer)
        LOG_INFO1("Initializing output '%s'", output->unique_name);
        output->renderer =
                      output->initialize(output, output->width, output->height);

        // store the output
        noia_list_append(outputs, output);

        // notify about new output
        if (output->renderer) {
            noia_event_signal_emit(SIGNAL_OUTPUT_FOUND, (NoiaObject*) output);
        } else {
            LOG_WARN1("Invalid renderer!");
        }
    }
}

//------------------------------------------------------------------------------

/// Notify about lost outputs.
/// @param lost_outputs - a list of outputs to notify about
void noia_output_collector_notify_outputs_lost(NoiaList* lost_outputs)
{
    FOR_EACH (lost_outputs, link) {
        NoiaOutput* output = (NoiaOutput*) link->data;
        if (!output || !output->unique_name) {
            LOG_WARN1("Invalid output found!");
            continue;
        }

        LOG_INFO1("Removing output '%s'", output->unique_name);
        noia_list_remove(outputs, output, (NoiaCompareFunc)noia_output_compare);
        noia_event_signal_emit(SIGNAL_OUTPUT_LOST, (NoiaObject*) output);
    }
}

//------------------------------------------------------------------------------

/// Update outputs:
/// -# fetch all outputs
/// -# compare with existing outputs
/// -# process findings and loses
void noia_output_collector_update()
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

/// Handle SIGNAL_OUTPUTS_CHANGED signal
void noia_output_on_change(NOIA_UNUSED void* data)
{
    noia_output_collector_update();
}

//------------------------------------------------------------------------------

void noia_output_collector_finalize(NOIA_UNUSED void* data)
{
    noia_list_free(outputs);
}

//------------------------------------------------------------------------------

/// Initialize Output Collector
void noia_output_collector_initialize(NoiaLoop* this_loop)
{
    if (!this_loop) {
        LOG_ERROR("Invalid loop!");
        return;
    }

    // initialize
    outputs = noia_list_new(NULL);
    noia_output_collector_update();

    // subscribe for signals
    noia_event_signal_subscribe(SIGNAL_OUTPUTS_CHANGED,
         noia_task_create(noia_output_on_change, this_loop, NULL));

    noia_loop_add_finalizer(this_loop, noia_output_collector_finalize);
}

//------------------------------------------------------------------------------

