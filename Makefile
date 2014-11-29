CC=gcc
WSSHG=wayland-scanner server-header
WSCG=wayland-scanner code

WFLAGS=-Wall
LFLAGS=-rdynamic -ldl -lrt -lpthread -lm

srcdir=src
builddir=build
gendir=gen

all: $(builddir)/aura

clean:
	rm -r $(builddir) $(gendir)


$(builddir)/aura: Makefile \
                  $(builddir)/aura.o \
                  $(builddir)/config.o \
                  $(builddir)/configuration-functions.o \
                  $(builddir)/utils-chain.o \
                  $(builddir)/utils-store.o \
                  $(builddir)/utils-dbus.o \
                  $(builddir)/utils-keymap.o \
                  $(builddir)/utils-log.o \
                  $(builddir)/utils-environment.o \
                  $(builddir)/event-dispatcher.o \
                  $(builddir)/event-timer.o \
                  $(builddir)/event-signals.o \
                  $(builddir)/event-loop.o \
                  $(builddir)/event-task.o \
                  $(builddir)/event-task-factory.o \
                  $(builddir)/device-common.o \
                  $(builddir)/device-fb.o \
                  $(builddir)/device-drm.o \
                  $(builddir)/device-evdev.o \
                  $(builddir)/device-udev.o \
                  $(builddir)/surface-aggregator.o \
                  $(builddir)/surface-compositor.o \
                  $(builddir)/surface-manager.o \
                  $(builddir)/keyboard-bindings.o \
                  $(builddir)/renderer-mmap.o \
                  $(builddir)/renderer-gl.o \
                  $(builddir)/wayland.o \
                  $(builddir)/wayland-common.o \
                  $(builddir)/wayland-protocol-compositor.o \
                  $(builddir)/wayland-protocol-surface.o \
                  $(builddir)/wayland-protocol-region.o \
                  $(builddir)/wayland-protocol-shell.o \
                  $(builddir)/wayland-protocol-shell-surface.o \
                  $(builddir)/wayland-protocol-xdg-shell.o \
                  $(builddir)/xdg-shell-protocol.o \
                  $(builddir)/wayland-protocol-output.o \
                  $(builddir)/wayland-protocol-seat.o \
                  $(builddir)/wayland-protocol-keyboard.o \
                  $(builddir)/bind-egl-wayland.o
	@mkdir -p $(builddir)
	@echo "  LD  aura"
	@$(CC) $(LFLAGS) -o $(builddir)/aura \
	       $(builddir)/aura.o $(builddir)/config.o $(builddir)/configuration-functions.o $(builddir)/utils-chain.o $(builddir)/utils-store.o $(builddir)/utils-dbus.o $(builddir)/utils-keymap.o $(builddir)/utils-log.o $(builddir)/utils-environment.o $(builddir)/event-dispatcher.o $(builddir)/event-timer.o $(builddir)/event-signals.o $(builddir)/event-loop.o $(builddir)/event-task.o $(builddir)/event-task-factory.o $(builddir)/device-common.o $(builddir)/device-fb.o $(builddir)/device-drm.o $(builddir)/device-evdev.o $(builddir)/device-udev.o $(builddir)/surface-aggregator.o $(builddir)/surface-compositor.o $(builddir)/surface-manager.o $(builddir)/keyboard-bindings.o $(builddir)/renderer-mmap.o $(builddir)/renderer-gl.o $(builddir)/wayland.o $(builddir)/wayland-common.o $(builddir)/wayland-protocol-compositor.o $(builddir)/wayland-protocol-surface.o $(builddir)/wayland-protocol-region.o $(builddir)/wayland-protocol-shell.o $(builddir)/wayland-protocol-shell-surface.o $(builddir)/wayland-protocol-xdg-shell.o $(builddir)/xdg-shell-protocol.o $(builddir)/wayland-protocol-output.o $(builddir)/wayland-protocol-seat.o $(builddir)/wayland-protocol-keyboard.o $(builddir)/bind-egl-wayland.o \
	       `pkg-config --libs egl libdrm wayland-server gl xkbcommon dbus-1 gbm libudev`

$(gendir)/xdg-shell-server-protocol.h: Makefile \
                                       protocol/xdg-shell.xml
	@mkdir -p $(gendir)
	@echo "  GEN xdg-shell-server-protocol.h"
	@$(WSSHG)  < "protocol/xdg-shell.xml" > "$(gendir)/xdg-shell-server-protocol.h"

$(gendir)/xdg-shell-protocol.c: Makefile \
                                protocol/xdg-shell.xml
	@mkdir -p $(gendir)
	@echo "  GEN xdg-shell-protocol.c"
	@$(WSCG)  < "protocol/xdg-shell.xml" > "$(gendir)/xdg-shell-protocol.c"

$(builddir)/aura.o: Makefile \
                    $(srcdir)/aura.c
	@mkdir -p $(builddir)
	@echo "  CC  aura.o"
	@$(CC) $(WFLAGS) -o $(builddir)/aura.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/aura.c

$(builddir)/config.o: Makefile \
                      $(srcdir)/config.c \
                      $(srcdir)/config.h
	@mkdir -p $(builddir)
	@echo "  CC  config.o"
	@$(CC) $(WFLAGS) -o $(builddir)/config.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/config.c

$(builddir)/configuration-functions.o: Makefile \
                                       $(srcdir)/configuration-functions.c \
                                       $(srcdir)/configuration-functions.h
	@mkdir -p $(builddir)
	@echo "  CC  configuration-functions.o"
	@$(CC) $(WFLAGS) -o $(builddir)/configuration-functions.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/configuration-functions.c

$(builddir)/utils-chain.o: Makefile \
                           $(srcdir)/utils-chain.c \
                           $(srcdir)/utils-chain.h
	@mkdir -p $(builddir)
	@echo "  CC  utils-chain.o"
	@$(CC) $(WFLAGS) -o $(builddir)/utils-chain.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/utils-chain.c

$(builddir)/utils-store.o: Makefile \
                           $(srcdir)/utils-store.c \
                           $(srcdir)/utils-store.h
	@mkdir -p $(builddir)
	@echo "  CC  utils-store.o"
	@$(CC) $(WFLAGS) -o $(builddir)/utils-store.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/utils-store.c

$(builddir)/utils-dbus.o: Makefile \
                          $(srcdir)/utils-dbus.c \
                          $(srcdir)/utils-dbus.h
	@mkdir -p $(builddir)
	@echo "  CC  utils-dbus.o"
	@$(CC) $(WFLAGS) -o $(builddir)/utils-dbus.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/utils-dbus.c \
	       `pkg-config --cflags dbus-1`

$(builddir)/utils-keymap.o: Makefile \
                            $(srcdir)/utils-keymap.c \
                            $(srcdir)/utils-keymap.h
	@mkdir -p $(builddir)
	@echo "  CC  utils-keymap.o"
	@$(CC) $(WFLAGS) -o $(builddir)/utils-keymap.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/utils-keymap.c \
	       `pkg-config --cflags xkbcommon`

$(builddir)/utils-log.o: Makefile \
                         $(srcdir)/utils-log.c \
                         $(srcdir)/utils-log.h
	@mkdir -p $(builddir)
	@echo "  CC  utils-log.o"
	@$(CC) $(WFLAGS) -o $(builddir)/utils-log.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/utils-log.c

$(builddir)/utils-environment.o: Makefile \
                                 $(srcdir)/utils-environment.c \
                                 $(srcdir)/utils-environment.h
	@mkdir -p $(builddir)
	@echo "  CC  utils-environment.o"
	@$(CC) $(WFLAGS) -o $(builddir)/utils-environment.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/utils-environment.c

$(builddir)/event-dispatcher.o: Makefile \
                                $(srcdir)/event-dispatcher.c \
                                $(srcdir)/event-dispatcher.h
	@mkdir -p $(builddir)
	@echo "  CC  event-dispatcher.o"
	@$(CC) $(WFLAGS) -o $(builddir)/event-dispatcher.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/event-dispatcher.c

$(builddir)/event-timer.o: Makefile \
                           $(srcdir)/event-timer.c \
                           $(srcdir)/event-timer.h
	@mkdir -p $(builddir)
	@echo "  CC  event-timer.o"
	@$(CC) $(WFLAGS) -o $(builddir)/event-timer.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/event-timer.c

$(builddir)/event-signals.o: Makefile \
                             $(srcdir)/event-signals.c \
                             $(srcdir)/event-signals.h
	@mkdir -p $(builddir)
	@echo "  CC  event-signals.o"
	@$(CC) $(WFLAGS) -o $(builddir)/event-signals.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/event-signals.c

$(builddir)/event-loop.o: Makefile \
                          $(srcdir)/event-loop.c \
                          $(srcdir)/event-loop.h
	@mkdir -p $(builddir)
	@echo "  CC  event-loop.o"
	@$(CC) $(WFLAGS) -o $(builddir)/event-loop.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/event-loop.c

$(builddir)/event-task.o: Makefile \
                          $(srcdir)/event-task.c \
                          $(srcdir)/event-task.h
	@mkdir -p $(builddir)
	@echo "  CC  event-task.o"
	@$(CC) $(WFLAGS) -o $(builddir)/event-task.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/event-task.c

$(builddir)/event-task-factory.o: Makefile \
                                  $(srcdir)/event-task-factory.c \
                                  $(srcdir)/event-task-factory.h
	@mkdir -p $(builddir)
	@echo "  CC  event-task-factory.o"
	@$(CC) $(WFLAGS) -o $(builddir)/event-task-factory.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/event-task-factory.c

$(builddir)/device-common.o: Makefile \
                             $(srcdir)/device-common.c \
                             $(srcdir)/device-common.h
	@mkdir -p $(builddir)
	@echo "  CC  device-common.o"
	@$(CC) $(WFLAGS) -o $(builddir)/device-common.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/device-common.c

$(builddir)/device-fb.o: Makefile \
                         $(srcdir)/device-fb.c \
                         $(srcdir)/device-fb.h
	@mkdir -p $(builddir)
	@echo "  CC  device-fb.o"
	@$(CC) $(WFLAGS) -o $(builddir)/device-fb.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/device-fb.c

$(builddir)/device-drm.o: Makefile \
                          $(srcdir)/device-drm.c \
                          $(srcdir)/device-drm.h
	@mkdir -p $(builddir)
	@echo "  CC  device-drm.o"
	@$(CC) $(WFLAGS) -o $(builddir)/device-drm.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/device-drm.c \
	       `pkg-config --cflags egl gbm libdrm`

$(builddir)/device-evdev.o: Makefile \
                            $(srcdir)/device-evdev.c \
                            $(srcdir)/device-evdev.h
	@mkdir -p $(builddir)
	@echo "  CC  device-evdev.o"
	@$(CC) $(WFLAGS) -o $(builddir)/device-evdev.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/device-evdev.c \
	       `pkg-config --cflags libudev`

$(builddir)/device-udev.o: Makefile \
                           $(srcdir)/device-udev.c \
                           $(srcdir)/device-udev.h
	@mkdir -p $(builddir)
	@echo "  CC  device-udev.o"
	@$(CC) $(WFLAGS) -o $(builddir)/device-udev.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/device-udev.c \
	       `pkg-config --cflags libudev`

$(builddir)/surface-aggregator.o: Makefile \
                                  $(srcdir)/surface-aggregator.c \
                                  $(srcdir)/surface-aggregator.h
	@mkdir -p $(builddir)
	@echo "  CC  surface-aggregator.o"
	@$(CC) $(WFLAGS) -o $(builddir)/surface-aggregator.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/surface-aggregator.c

$(builddir)/surface-compositor.o: Makefile \
                                  $(srcdir)/surface-compositor.c \
                                  $(srcdir)/surface-compositor.h
	@mkdir -p $(builddir)
	@echo "  CC  surface-compositor.o"
	@$(CC) $(WFLAGS) -o $(builddir)/surface-compositor.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/surface-compositor.c

$(builddir)/surface-manager.o: Makefile \
                               $(srcdir)/surface-manager.c \
                               $(srcdir)/surface-manager.h
	@mkdir -p $(builddir)
	@echo "  CC  surface-manager.o"
	@$(CC) $(WFLAGS) -o $(builddir)/surface-manager.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/surface-manager.c

$(builddir)/keyboard-bindings.o: Makefile \
                                 $(srcdir)/keyboard-bindings.c \
                                 $(srcdir)/keyboard-bindings.h
	@mkdir -p $(builddir)
	@echo "  CC  keyboard-bindings.o"
	@$(CC) $(WFLAGS) -o $(builddir)/keyboard-bindings.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/keyboard-bindings.c

$(builddir)/renderer-mmap.o: Makefile \
                             $(srcdir)/renderer-mmap.c \
                             $(srcdir)/renderer-mmap.h
	@mkdir -p $(builddir)
	@echo "  CC  renderer-mmap.o"
	@$(CC) $(WFLAGS) -o $(builddir)/renderer-mmap.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/renderer-mmap.c

$(builddir)/renderer-gl.o: Makefile \
                           $(srcdir)/renderer-gl.c \
                           $(srcdir)/renderer-gl.h
	@mkdir -p $(builddir)
	@echo "  CC  renderer-gl.o"
	@$(CC) $(WFLAGS) -o $(builddir)/renderer-gl.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/renderer-gl.c \
	       `pkg-config --cflags gl`

$(builddir)/wayland.o: Makefile \
                       $(srcdir)/wayland.c \
                       $(srcdir)/wayland.h \
                       $(gendir)/xdg-shell-server-protocol.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/wayland.c \
	       `pkg-config --cflags wayland-server`

$(builddir)/wayland-common.o: Makefile \
                              $(srcdir)/wayland-common.c \
                              $(srcdir)/wayland-common.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-common.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-common.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/wayland-common.c

$(builddir)/wayland-protocol-compositor.o: Makefile \
                                           $(srcdir)/wayland-protocol-compositor.c \
                                           $(srcdir)/wayland-protocol-compositor.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-protocol-compositor.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-protocol-compositor.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/wayland-protocol-compositor.c

$(builddir)/wayland-protocol-surface.o: Makefile \
                                        $(srcdir)/wayland-protocol-surface.c \
                                        $(srcdir)/wayland-protocol-surface.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-protocol-surface.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-protocol-surface.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/wayland-protocol-surface.c

$(builddir)/wayland-protocol-region.o: Makefile \
                                       $(srcdir)/wayland-protocol-region.c \
                                       $(srcdir)/wayland-protocol-region.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-protocol-region.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-protocol-region.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/wayland-protocol-region.c

$(builddir)/wayland-protocol-shell.o: Makefile \
                                      $(srcdir)/wayland-protocol-shell.c \
                                      $(srcdir)/wayland-protocol-shell.h \
                                      $(gendir)/xdg-shell-server-protocol.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-protocol-shell.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-protocol-shell.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/wayland-protocol-shell.c

$(builddir)/wayland-protocol-shell-surface.o: Makefile \
                                              $(srcdir)/wayland-protocol-shell-surface.c \
                                              $(srcdir)/wayland-protocol-shell-surface.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-protocol-shell-surface.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-protocol-shell-surface.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/wayland-protocol-shell-surface.c

$(builddir)/wayland-protocol-xdg-shell.o: Makefile \
                                          $(srcdir)/wayland-protocol-xdg-shell.c \
                                          $(srcdir)/wayland-protocol-xdg-shell.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-protocol-xdg-shell.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-protocol-xdg-shell.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/wayland-protocol-xdg-shell.c

$(builddir)/xdg-shell-protocol.o: Makefile \
                                  $(gendir)/xdg-shell-protocol.c
	@mkdir -p $(builddir)
	@echo "  CC  xdg-shell-protocol.o"
	@$(CC) $(WFLAGS) -o $(builddir)/xdg-shell-protocol.o -I$(srcdir) -I$(gendir) \
	       -c $(gendir)/xdg-shell-protocol.c

$(builddir)/wayland-protocol-output.o: Makefile \
                                       $(srcdir)/wayland-protocol-output.c \
                                       $(srcdir)/wayland-protocol-output.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-protocol-output.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-protocol-output.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/wayland-protocol-output.c

$(builddir)/wayland-protocol-seat.o: Makefile \
                                     $(srcdir)/wayland-protocol-seat.c \
                                     $(srcdir)/wayland-protocol-seat.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-protocol-seat.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-protocol-seat.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/wayland-protocol-seat.c

$(builddir)/wayland-protocol-keyboard.o: Makefile \
                                         $(srcdir)/wayland-protocol-keyboard.c \
                                         $(srcdir)/wayland-protocol-keyboard.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-protocol-keyboard.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-protocol-keyboard.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/wayland-protocol-keyboard.c

$(builddir)/bind-egl-wayland.o: Makefile \
                                $(srcdir)/bind-egl-wayland.c \
                                $(srcdir)/bind-egl-wayland.h
	@mkdir -p $(builddir)
	@echo "  CC  bind-egl-wayland.o"
	@$(CC) $(WFLAGS) -o $(builddir)/bind-egl-wayland.o -I$(srcdir) -I$(gendir) \
	       -c $(srcdir)/bind-egl-wayland.c

