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
AuraRenderer* rend = 0;
int num = 0;

//------------------------------------------------------------------------------

void aura_outputs_update()
{
    int result = -1;

    LOG_INFO1("Updating outputs");

    if (!aura_settings().run_in_test_window) {
        result = aura_drm_update_devices(&output, &num);
        if (result < 0) {
            result = aura_setup_framebuffer(&output, &num);
        }
    } else {
        result = aura_backend_gtk_get_outputs(&output, &num);
    }

    if (result < 0) {
        LOG_WARN1("No valid outputs!");
        return;
    }

    rend = output->initialize((struct AuraOutput*) output,
                               output->width, output->height);

    aura_event_signal_emit(SIGNAL_DISPLAY_FOUND, rend);
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

