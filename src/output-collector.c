// file: output-collector.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "output-collector.h"
#include "event-signals.h"
#include "utils-log.h"
#include "device-drm.h"
#include "device-fb.h"
#include "backend-gtk.h"
#include "config.h"

Chain* outputs = 0;

//------------------------------------------------------------------------------

Chain* aura_outputs_fetch_actual_outputs()
{
    int num = 0;
    Chain* actual_outputs = chain_new(0);

    if (!aura_settings().run_in_test_window) {
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

void aura_outputs_notify_outputs_found(Chain* found_outputs) {
    Link* link;
    for (link = found_outputs->first; link; link = link->next) {
        AuraOutput* output = (AuraOutput*) link->data;
        if (!output || !output->unique_name) {
            LOG_WARN1("Invalid output found!");
            continue;
        }

        LOG_INFO1("Initializing output '%s'", output->unique_name);
        output->renderer =
                      output->initialize(output, output->width, output->height);

        chain_append(outputs, output);

        if (output->renderer) {
            aura_event_signal_emit(SIGNAL_DISPLAY_FOUND, output);
        } else {
            LOG_WARN1("Invalid renderer!");
        }
    }
}

//------------------------------------------------------------------------------

void aura_outputs_notify_outputs_lost(Chain* lost_outputs) {
    Link* link;
    for (link = lost_outputs->first; link; link = link->next) {
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

void aura_outputs_update()
{
    LOG_INFO1("Updating outputs");

    Chain* actual_outputs = aura_outputs_fetch_actual_outputs();

    Chain* found_outputs = chain_subtract(actual_outputs, outputs,
                                         (AuraCompareFunc) aura_output_compare);

    Chain* lost_outputs = chain_subtract(outputs, actual_outputs,
                                         (AuraCompareFunc) aura_output_compare);

    aura_outputs_notify_outputs_found(found_outputs);
    aura_outputs_notify_outputs_lost(lost_outputs);

    chain_free(found_outputs);
    chain_free(lost_outputs);
    chain_free(actual_outputs);
}

//------------------------------------------------------------------------------

static void on_display_discovered(void* data)
{
    aura_outputs_update();
}

//------------------------------------------------------------------------------

void aura_output_collector_initialize(AuraLoop* this_loop)
{
    if (this_loop == 0) {
        LOG_ERROR("Invalid loop!");
        return;
    }

    outputs = chain_new(0);
    aura_outputs_update();

    aura_event_signal_subscribe(SIGNAL_DISPLAY_DISCOVERED,
         aura_task_create(on_display_discovered, this_loop));
}

//------------------------------------------------------------------------------

