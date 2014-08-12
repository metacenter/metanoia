CC=gcc
WFLAGS=-Wall
LFLAGS=-rdynamic -ldl -lrt -lpthread -lm

srcdir=src
builddir=build

all: $(builddir)/aura

clear:
	rm -r $(builddir)


$(builddir)/aura: Makefile \
                  $(builddir)/aura.o \
                  $(builddir)/configuration-functions.o \
                  $(builddir)/utils-log.o \
                  $(builddir)/utils-dbus.o \
                  $(builddir)/utils-chain.o \
                  $(builddir)/loop.o \
                  $(builddir)/event_dispatcher.o \
                  $(builddir)/task.o \
                  $(builddir)/task_factory.o \
                  $(builddir)/evdev.o \
                  $(builddir)/udev.o \
                  $(builddir)/drm.o \
                  $(builddir)/shared.o \
                  $(builddir)/devfb.o \
                  $(builddir)/surface-aggregator.o \
                  $(builddir)/surface-compositor.o \
                  $(builddir)/surface-manager.o \
                  $(builddir)/keyboard-bindings.o \
                  $(builddir)/renderer-mmap.o \
                  $(builddir)/renderer-gl.o \
                  $(builddir)/wayland.o \
                  $(builddir)/wayland-compositor.o \
                  $(builddir)/wayland-surface.o \
                  $(builddir)/wayland-output.o \
                  $(builddir)/wayland-shell.o \
                  $(builddir)/wayland-xdg_shell.o \
                  $(builddir)/xdg-shell-protocol.o \
                  $(builddir)/wayland-shell-surface.o \
                  $(builddir)/bind-egl-wayland.o
	@mkdir -p $(builddir)
	@echo "  LD  aura"
	@$(CC) $(LFLAGS) -o $(builddir)/aura \
	       $(builddir)/aura.o $(builddir)/configuration-functions.o $(builddir)/utils-log.o $(builddir)/utils-dbus.o $(builddir)/utils-chain.o $(builddir)/loop.o $(builddir)/event_dispatcher.o $(builddir)/task.o $(builddir)/task_factory.o $(builddir)/evdev.o $(builddir)/udev.o $(builddir)/drm.o $(builddir)/shared.o $(builddir)/devfb.o $(builddir)/surface-aggregator.o $(builddir)/surface-compositor.o $(builddir)/surface-manager.o $(builddir)/keyboard-bindings.o $(builddir)/renderer-mmap.o $(builddir)/renderer-gl.o $(builddir)/wayland.o $(builddir)/wayland-compositor.o $(builddir)/wayland-surface.o $(builddir)/wayland-output.o $(builddir)/wayland-shell.o $(builddir)/wayland-xdg_shell.o $(builddir)/xdg-shell-protocol.o $(builddir)/wayland-shell-surface.o $(builddir)/bind-egl-wayland.o \
	       `pkg-config --libs dbus-1 libudev libdrm gbm wayland-server egl gl`

$(builddir)/aura.o: Makefile \
                    $(srcdir)/aura.c
	@mkdir -p $(builddir)
	@echo "  CC  aura.o"
	@$(CC) $(WFLAGS) -o $(builddir)/aura.o -I$(srcdir) \
	       -c $(srcdir)/aura.c

$(builddir)/configuration-functions.o: Makefile \
                                       $(srcdir)/configuration-functions.c \
                                       $(srcdir)/configuration-functions.h
	@mkdir -p $(builddir)
	@echo "  CC  configuration-functions.o"
	@$(CC) $(WFLAGS) -o $(builddir)/configuration-functions.o -I$(srcdir) \
	       -c $(srcdir)/configuration-functions.c

$(builddir)/utils-chain.o: Makefile \
                           $(srcdir)/utils-chain.c \
                           $(srcdir)/utils-chain.h
	@mkdir -p $(builddir)
	@echo "  CC  utils-chain.o"
	@$(CC) $(WFLAGS) -o $(builddir)/utils-chain.o -I$(srcdir) \
	       -c $(srcdir)/utils-chain.c

$(builddir)/utils-dbus.o: Makefile \
                          $(srcdir)/utils-dbus.c \
                          $(srcdir)/utils-dbus.h
	@mkdir -p $(builddir)
	@echo "  CC  utils-dbus.o"
	@$(CC) $(WFLAGS) -o $(builddir)/utils-dbus.o -I$(srcdir) \
	       -c $(srcdir)/utils-dbus.c \
	       `pkg-config --cflags dbus-1`

$(builddir)/utils-log.o: Makefile \
                         $(srcdir)/utils-log.c \
                         $(srcdir)/utils-log.h
	@mkdir -p $(builddir)
	@echo "  CC  utils-log.o"
	@$(CC) $(WFLAGS) -o $(builddir)/utils-log.o -I$(srcdir) \
	       -c $(srcdir)/utils-log.c

$(builddir)/event_dispatcher.o: Makefile \
                                $(srcdir)/utils/event_dispatcher.c \
                                $(srcdir)/utils/event_dispatcher.h
	@mkdir -p $(builddir)
	@echo "  CC  event_dispatcher.o"
	@$(CC) $(WFLAGS) -o $(builddir)/event_dispatcher.o -I$(srcdir) \
	       -c $(srcdir)/utils/event_dispatcher.c

$(builddir)/loop.o: Makefile \
                    $(srcdir)/utils/loop.c \
                    $(srcdir)/utils/loop.h
	@mkdir -p $(builddir)
	@echo "  CC  loop.o"
	@$(CC) $(WFLAGS) -o $(builddir)/loop.o -I$(srcdir) \
	       -c $(srcdir)/utils/loop.c

$(builddir)/task.o: Makefile \
                    $(srcdir)/utils/task.c \
                    $(srcdir)/utils/task.h
	@mkdir -p $(builddir)
	@echo "  CC  task.o"
	@$(CC) $(WFLAGS) -o $(builddir)/task.o -I$(srcdir) \
	       -c $(srcdir)/utils/task.c

$(builddir)/task_factory.o: Makefile \
                            $(srcdir)/utils/task_factory.c \
                            $(srcdir)/utils/task_factory.h
	@mkdir -p $(builddir)
	@echo "  CC  task_factory.o"
	@$(CC) $(WFLAGS) -o $(builddir)/task_factory.o -I$(srcdir) \
	       -c $(srcdir)/utils/task_factory.c

$(builddir)/keyboard-bindings.o: Makefile \
                                 $(srcdir)/keyboard-bindings.c \
                                 $(srcdir)/keyboard-bindings.h
	@mkdir -p $(builddir)
	@echo "  CC  keyboard-bindings.o"
	@$(CC) $(WFLAGS) -o $(builddir)/keyboard-bindings.o -I$(srcdir) \
	       -c $(srcdir)/keyboard-bindings.c

$(builddir)/shared.o: Makefile \
                      $(srcdir)/devices/shared.c \
                      $(srcdir)/devices/shared.h
	@mkdir -p $(builddir)
	@echo "  CC  shared.o"
	@$(CC) $(WFLAGS) -o $(builddir)/shared.o -I$(srcdir) \
	       -c $(srcdir)/devices/shared.c

$(builddir)/devfb.o: Makefile \
                     $(srcdir)/devices/devfb.c \
                     $(srcdir)/devices/devfb.h
	@mkdir -p $(builddir)
	@echo "  CC  devfb.o"
	@$(CC) $(WFLAGS) -o $(builddir)/devfb.o -I$(srcdir) \
	       -c $(srcdir)/devices/devfb.c

$(builddir)/evdev.o: Makefile \
                     $(srcdir)/devices/evdev.c \
                     $(srcdir)/devices/evdev.h
	@mkdir -p $(builddir)
	@echo "  CC  evdev.o"
	@$(CC) $(WFLAGS) -o $(builddir)/evdev.o -I$(srcdir) \
	       -c $(srcdir)/devices/evdev.c

$(builddir)/udev.o: Makefile \
                    $(srcdir)/devices/udev.c \
                    $(srcdir)/devices/udev.h
	@mkdir -p $(builddir)
	@echo "  CC  udev.o"
	@$(CC) $(WFLAGS) -o $(builddir)/udev.o -I$(srcdir) \
	       -c $(srcdir)/devices/udev.c

$(builddir)/drm.o: Makefile \
                   $(srcdir)/devices/drm.c \
                   $(srcdir)/devices/drm.h
	@mkdir -p $(builddir)
	@echo "  CC  drm.o"
	@$(CC) $(WFLAGS) -o $(builddir)/drm.o -I$(srcdir) \
	       -c $(srcdir)/devices/drm.c \
	       `pkg-config --cflags libdrm egl`

$(builddir)/surface-aggregator.o: Makefile \
                                  $(srcdir)/surface-aggregator.c \
                                  $(srcdir)/surface-aggregator.h
	@mkdir -p $(builddir)
	@echo "  CC  surface-aggregator.o"
	@$(CC) $(WFLAGS) -o $(builddir)/surface-aggregator.o -I$(srcdir) \
	       -c $(srcdir)/surface-aggregator.c

$(builddir)/surface-compositor.o: Makefile \
                                  $(srcdir)/surface-compositor.c \
                                  $(srcdir)/surface-compositor.h
	@mkdir -p $(builddir)
	@echo "  CC  surface-compositor.o"
	@$(CC) $(WFLAGS) -o $(builddir)/surface-compositor.o -I$(srcdir) \
	       -c $(srcdir)/surface-compositor.c

$(builddir)/surface-manager.o: Makefile \
                               $(srcdir)/surface-manager.c \
                               $(srcdir)/surface-manager.h
	@mkdir -p $(builddir)
	@echo "  CC  surface-manager.o"
	@$(CC) $(WFLAGS) -o $(builddir)/surface-manager.o -I$(srcdir) \
	       -c $(srcdir)/surface-manager.c

$(builddir)/renderer-mmap.o: Makefile \
                             $(srcdir)/renderer-mmap.c \
                             $(srcdir)/renderer-mmap.h
	@mkdir -p $(builddir)
	@echo "  CC  renderer-mmap.o"
	@$(CC) $(WFLAGS) -o $(builddir)/renderer-mmap.o -I$(srcdir) \
	       -c $(srcdir)/renderer-mmap.c

$(builddir)/renderer-gl.o: Makefile \
                           $(srcdir)/renderer-gl.c \
                           $(srcdir)/renderer-gl.h
	@mkdir -p $(builddir)
	@echo "  CC  renderer-gl.o"
	@$(CC) $(WFLAGS) -o $(builddir)/renderer-gl.o -I$(srcdir) \
	       -c $(srcdir)/renderer-gl.c

$(builddir)/wayland.o: Makefile \
                       $(srcdir)/frontends/wayland.c \
                       $(srcdir)/frontends/wayland.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland.o -I$(srcdir) \
	       -c $(srcdir)/frontends/wayland.c

$(builddir)/wayland-compositor.o: Makefile \
                                  $(srcdir)/frontends/wayland-compositor.c \
                                  $(srcdir)/frontends/wayland-compositor.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-compositor.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-compositor.o -I$(srcdir) \
	       -c $(srcdir)/frontends/wayland-compositor.c

$(builddir)/wayland-surface.o: Makefile \
                               $(srcdir)/frontends/wayland-surface.c \
                               $(srcdir)/frontends/wayland-surface.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-surface.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-surface.o -I$(srcdir) \
	       -c $(srcdir)/frontends/wayland-surface.c

$(builddir)/wayland-shell.o: Makefile \
                             $(srcdir)/frontends/wayland-shell.c \
                             $(srcdir)/frontends/wayland-shell.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-shell.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-shell.o -I$(srcdir) \
	       -c $(srcdir)/frontends/wayland-shell.c

$(builddir)/wayland-shell-surface.o: Makefile \
                                     $(srcdir)/frontends/wayland-shell-surface.c \
                                     $(srcdir)/frontends/wayland-shell-surface.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-shell-surface.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-shell-surface.o -I$(srcdir) \
	       -c $(srcdir)/frontends/wayland-shell-surface.c

$(builddir)/wayland-xdg_shell.o: Makefile \
                                 $(srcdir)/frontends/wayland-xdg_shell.c \
                                 $(srcdir)/frontends/wayland-xdg_shell.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-xdg_shell.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-xdg_shell.o -I$(srcdir) \
	       -c $(srcdir)/frontends/wayland-xdg_shell.c

$(builddir)/xdg-shell-protocol.o: Makefile \
                                  $(srcdir)/frontends/xdg-shell-protocol.c
	@mkdir -p $(builddir)
	@echo "  CC  xdg-shell-protocol.o"
	@$(CC) $(WFLAGS) -o $(builddir)/xdg-shell-protocol.o -I$(srcdir) \
	       -c $(srcdir)/frontends/xdg-shell-protocol.c

$(builddir)/wayland-output.o: Makefile \
                              $(srcdir)/frontends/wayland-output.c \
                              $(srcdir)/frontends/wayland-output.h
	@mkdir -p $(builddir)
	@echo "  CC  wayland-output.o"
	@$(CC) $(WFLAGS) -o $(builddir)/wayland-output.o -I$(srcdir) \
	       -c $(srcdir)/frontends/wayland-output.c

$(builddir)/bind-egl-wayland.o: Makefile \
                                $(srcdir)/bind-egl-wayland.c \
                                $(srcdir)/bind-egl-wayland.h
	@mkdir -p $(builddir)
	@echo "  CC  bind-egl-wayland.o"
	@$(CC) $(WFLAGS) -o $(builddir)/bind-egl-wayland.o -I$(srcdir) \
	       -c $(srcdir)/bind-egl-wayland.c

