// file: event-dispatcher.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EVENT_DISPATCHER_H__
#define __AURA_EVENT_DISPATCHER_H__

#include <sys/epoll.h>

typedef struct AuraEventData AuraEventData;
typedef struct AuraEventDispatcherPriv AuraEventDispatcher;

typedef void (*AuraEventHandler) (AuraEventData*, struct epoll_event*);

struct AuraEventData {
    int fd;
    AuraEventHandler handler;
    uint32_t flags;
    void* data;
};

AuraEventData* aura_event_data_create(int fd,
                                      AuraEventHandler handler,
                                      uint32_t flags,
                                      void* data);

AuraEventDispatcher* aura_event_dispatcher_new(void);
void aura_event_dispatcher_free(AuraEventDispatcher* self);

int aura_event_dispatcher_is_running(AuraEventDispatcher* self);

int aura_event_dispatcher_initialize(AuraEventDispatcher* self);

int aura_event_dispatcher_add_event_source(AuraEventDispatcher* self,
                                           AuraEventData* data);

void aura_event_dispatcher_start(AuraEventDispatcher* self);
void aura_event_dispatcher_stop(AuraEventDispatcher* self);

void aura_event_dispatcher_default_signal_handler(AuraEventData* data,
                                                  struct epoll_event* event);

#endif // __AURA_EVENT_DISPATCHER_H__

