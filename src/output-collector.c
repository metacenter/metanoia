// file: output-collector.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "output-collector.h"
#include "event-signals.h"
#include "utils-log.h"
#include "device-drm.h"
#include "device-fb.h"
#include "backend-gtk.h"
#include "config.h"

#include <limits.h>

//------------------------------------------------------------------------------

/// List of all currently available outputs.
/// @todo This should not be a global variable.
Chain* outputs = 0;

//------------------------------------------------------------------------------

/// Find space in global output coordinates to place new output.
///
/// @todo Currently it places new output to the right of the most right one.
///       This behavior should be more configurable.
AuraPosition aura_output_collector_allocate_position()
{
    AuraPosition result = {INT_MIN, INT_MIN};
    for (Link* link = outputs->first; link; link = link->next) {
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
Chain* aura_output_collector_fetch_actual_outputs()
{
    int num = 0;
    Chain* actual_outputs = chain_new(0);

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
void aura_output_collector_notify_outputs_found(Chain* found_outputs)
{
    for (Link* link = found_outputs->first; link; link = link->next) {
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
        chain_append(outputs, output);

        // notify about new output
        if (output->renderer) {
            aura_event_signal_emit(SIGNAL_DISPLAY_FOUND, output);
        } else {
            LOG_WARN1("Invalid renderer!");
        }
    }
}

//------------------------------------------------------------------------------

/// Notify about lost outputs.
/// @param lost_outputs - a list of outputs to notify about
void aura_output_collector_notify_outputs_lost(Chain* lost_outputs)
{
    for (Link* link = lost_outputs->first; link; link = link->next) {
        AuraOutput* output = (AuraOutput*) link->data;
        if (!output || !output->unique_name) {
            LOG_WARN1("Invalid output found!");
            continue;
        }

        LOG_INFO1("Removing output '%s'", output->unique_name);
        chain_remove(outputs, output, (AuraCompareFunc) aura_output_compare);
        aura_event_signal_emit(SIGNAL_DISPLAY_LOST, output);
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

    Chain* actual_outputs = aura_output_collector_fetch_actual_outputs();

    Chain* found_outputs = chain_subtract(actual_outputs, outputs,
                                         (AuraCompareFunc) aura_output_compare);

    Chain* lost_outputs = chain_subtract(outputs, actual_outputs,
                                         (AuraCompareFunc) aura_output_compare);

    aura_output_collector_notify_outputs_found(found_outputs);
    aura_output_collector_notify_outputs_lost(lost_outputs);

    chain_free(found_outputs);
    chain_free(lost_outputs);
    chain_free(actual_outputs);
}

//------------------------------------------------------------------------------

/// Handle SIGNAL_DISPLAY_DISCOVERED signal
void aura_outputs_on_display_discovered(AURA_UNUSED void* data)
{
    aura_output_collector_update();
}

//------------------------------------------------------------------------------

/// Initialize Output Collector
void aura_output_collector_initialize(AuraLoop* this_loop)
{
    if (this_loop == 0) {
        LOG_ERROR("Invalid loop!");
        return;
    }

    // initialize
    outputs = chain_new(0);
    aura_output_collector_update();

    // subscribe for signals
    aura_event_signal_subscribe(SIGNAL_DISPLAY_DISCOVERED,
         aura_task_create(aura_outputs_on_display_discovered, this_loop));
}

//------------------------------------------------------------------------------

