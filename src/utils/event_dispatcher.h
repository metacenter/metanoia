// file: event_dispatcher.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EVENT_DISPATCHER_H__
#define __AURA_EVENT_DISPATCHER_H__

#include <sys/epoll.h>

typedef struct AuraEventDispatcherPriv AuraEventDispatcher;

struct AuraEventData;
typedef void (*AuraEventHandler) (void*, struct epoll_event);
typedef struct {
    AuraEventHandler handler;
    int fd;
    uint32_t flags;
} AuraEventData;

AuraEventDispatcher* aura_event_dispatcher_new(void);
void aura_event_dispatcher_free(AuraEventDispatcher* self);

int aura_event_dispatcher_add_event_source(AuraEventDispatcher* self,
                                           AuraEventData* data);

void aura_event_dispatcher_start(AuraEventDispatcher* self);
void aura_event_dispatcher_stop(AuraEventDispatcher* self);

#endif // __AURA_EVENT_DISPATCHER_H__

