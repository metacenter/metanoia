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
AuraList* outputs = 0;

//------------------------------------------------------------------------------

/// Find space in global output coordinates to place new output.
///
/// @todo Currently it places new output to the right of the most right one.
///       This behavior should be more configurable.
AuraPosition aura_output_collector_allocate_position()
{
    AuraPosition result = {INT_MIN, INT_MIN};
    FOR_EACH (outputs, link) {
        AuraOutput* output = (AuraOutput*) link->data;
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
AuraList* aura_output_collector_fetch_actual_outputs()
{
    int num = 0;
    AuraList* actual_outputs = aura_list_new(NULL);

    if (!aura_settings()->run_in_test_window) {
        num = aura_drm_update_devices(actual_outputs);
        if (num < 1) {
            num = aura_devfb_setup_framebuffer(actual_outputs);
        }
    } else {
        num = aura_backend_gtk_get_outputs(actual_outputs);
    }

    if (num < 1) {
        LOG_WARN1("No valid outputs!");
    }

    return actual_outputs;
}

//------------------------------------------------------------------------------

/// Notify about newly found outputs.
/// @param found_outputs - a list of outputs to notify about
void aura_output_collector_notify_outputs_found(AuraList* found_outputs)
{
    FOR_EACH (found_outputs, link) {
        AuraOutput* output = (AuraOutput*) link->data;
        if (!output || !output->unique_name) {
            LOG_WARN1("Invalid output found!");
            continue;
        }

        // assign position to output
        output->global_position = aura_output_collector_allocate_position();

        // initialize output (create renderer)
        LOG_INFO1("Initializing output '%s'", output->unique_name);
        output->renderer =
                      output->initialize(output, output->width, output->height);

        // store the output
        aura_list_append(outputs, output);

        // notify about new output
        if (output->renderer) {
            aura_event_signal_emit(SIGNAL_DISPLAY_FOUND, (AuraObject*) output);
        } else {
            LOG_WARN1("Invalid renderer!");
        }
    }
}

//------------------------------------------------------------------------------

/// Notify about lost outputs.
/// @param lost_outputs - a list of outputs to notify about
void aura_output_collector_notify_outputs_lost(AuraList* lost_outputs)
{
    FOR_EACH (lost_outputs, link) {
        AuraOutput* output = (AuraOutput*) link->data;
        if (!output || !output->unique_name) {
            LOG_WARN1("Invalid output found!");
            continue;
        }

        LOG_INFO1("Removing output '%s'", output->unique_name);
        aura_list_remove(outputs, output, (AuraCompareFunc)aura_output_compare);
        aura_event_signal_emit(SIGNAL_DISPLAY_LOST, (AuraObject*) output);
    }
}

//------------------------------------------------------------------------------

/// Update outputs:
/// -# fetch all outputs
/// -# compare with existing outputs
/// -# process findings and loses
void aura_output_collector_update()
{
    LOG_INFO1("Updating outputs");

    AuraList* actual_outputs = aura_output_collector_fetch_actual_outputs();

    AuraList* found_outputs = aura_list_subtract(actual_outputs, outputs,
                                         (AuraCompareFunc) aura_output_compare);

    AuraList* lost_outputs = aura_list_subtract(outputs, actual_outputs,
                                         (AuraCompareFunc) aura_output_compare);

    aura_output_collector_notify_outputs_found(found_outputs);
    aura_output_collector_notify_outputs_lost(lost_outputs);

    aura_list_free(found_outputs);
    aura_list_free(lost_outputs);
    aura_list_free(actual_outputs);
}

//------------------------------------------------------------------------------

/// Handle SIGNAL_DISPLAY_DISCOVERED signal
void aura_outputs_on_display_discovered(AURA_UNUSED void* data)
{
    aura_output_collector_update();
}

//------------------------------------------------------------------------------

void aura_output_collector_finalize(AURA_UNUSED void* data)
{
    aura_list_free(outputs);
}

//------------------------------------------------------------------------------

/// Initialize Output Collector
void aura_output_collector_initialize(AuraLoop* this_loop)
{
    if (!this_loop) {
        LOG_ERROR("Invalid loop!");
        return;
    }

    // initialize
    outputs = aura_list_new(NULL);
    aura_output_collector_update();

    // subscribe for signals
    aura_event_signal_subscribe(SIGNAL_DISPLAY_DISCOVERED,
         aura_task_create(aura_outputs_on_display_discovered, this_loop, NULL));

    aura_loop_add_finalizer(this_loop, aura_output_collector_finalize);
}

//------------------------------------------------------------------------------

