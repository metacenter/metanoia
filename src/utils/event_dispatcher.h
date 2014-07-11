// file: event_dispatcher.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EVENT_DISPATCHER_H__
#define __AURA_EVENT_DISPATCHER_H__

#include <sys/epoll.h>
#include <sys/signal.h>

typedef struct AuraEventDispatcherPriv AuraEventDispatcher;

struct AuraEventData;

typedef void (*AuraEventHandler) (void* data, struct epoll_event);
typedef void (*AuraTimerHandler) (union sigval data);

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

int aura_event_dispatcher_timer_run(AuraEventDispatcher* self,
                                    AuraTimerHandler timer_handler,
                                    int miliseconds);

#endif // __AURA_EVENT_DISPATCHER_H__

