// file: output-collector.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "output-collector.h"
#include "event-signals.h"
#include "utils-log.h"
#include "device-drm.h"
#include "device-fb.h"
#include "backend-gtk.h"
#include "config.h"

// FIXME: tmp
// TODO: support for many outputs
AuraOutput* output = 0;
int num = 0;

//------------------------------------------------------------------------------

void aura_outputs_update()
{
    int num = 0;

    LOG_INFO1("Updating outputs");

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
        return;
    }

    Link* link;
    for (link = actual_outputs->first; link; link = link->next) {
        AuraOutput* output = (AuraOutput*) link->data;
        AuraRenderer* renderer =
                      output->initialize(output, output->width, output->height);

        if (renderer) {
            aura_event_signal_emit(SIGNAL_DISPLAY_FOUND, renderer);
        } else {
            LOG_WARN1("Invalid renderer!");
        }
    }
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

    aura_outputs_update();

    aura_event_signal_subscribe(SIGNAL_DISPLAY_DISCOVERED,
         aura_task_create(on_display_discovered, this_loop));
}

//------------------------------------------------------------------------------

