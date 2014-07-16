// file: surface-aggregator.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface.h"
#include "surface-priv.h"

#include "utils/log.h"
#include "global-constants.h"

#include <malloc.h>
#include <search.h>
#include <stdlib.h>
#include <string.h>

static void* root = NULL;

//------------------------------------------------------------------------------

static int compare(const void* surface1, const void* surface2)
{
    SurfaceId id1 = ((SurfaceData*) surface1)->id;
    SurfaceId id2 = ((SurfaceData*) surface2)->id;

    if (id1 < id2) return -1;
    if (id1 > id2) return  1;
    return 0;
}

//------------------------------------------------------------------------------

SurfaceId aura_surface_create(void)
{
    // Create new surface
    SurfaceData* data = malloc(sizeof(SurfaceData));
    if (data == NULL) {
        LOG_ERROR("Could not create new surface!");
        free(data);
        return scInvalidSurfaceId;
    }
    memset(data, 0, sizeof(SurfaceData));

    // Generate (unused) id
    do {
        data->id = (SurfaceId) (rand() & 0xFF);
    } while (tfind((void *) data, &root, compare) != NULL);

    // Store surface
    if (tsearch((void *) data, &root, compare) == NULL) {
        LOG_ERROR("Could not store new surface!");
        free(data);
        return scInvalidSurfaceId;
    }

    // Inform surface manager // FIXME: inform on different event (shell?)
    aura_surface_manage(data->id);

    return data->id;
}

//------------------------------------------------------------------------------

SurfaceData* aura_surface_get(SurfaceId id)
{
    SurfaceData data;
    data.id = id;

    return tsearch((void *) &data, &root, compare);
}

//------------------------------------------------------------------------------

