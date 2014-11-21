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
                  $(builddir)/utils-chain.o \
                  $(builddir)/utils-dbus.o \
                  $(builddir)/utils-log.o \
                  $(builddir)/event-dispatcher.o \
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
                  $(builddir)/wayland-compositor.o \
                  $(builddir)/wayland-surface.o \
                  $(builddir)/wayland-shell.o \
                  $(builddir)/wayland-shell-surface.o \
                  $(builddir)/wayland-xdg_shell.o \
                  $(builddir)/xdg-shell-protocol.o \
                  $(builddir)/wayland-output.o \
                  $(builddir)/bind-egl-wayland.o
	@mkdir -p $(builddir)
	@echo "  LD  aura"
	@$(CC) $(LFLAGS) -o $(builddir)/aura \
	       $(builddir)/aura.o $(builddir)/configuration-functions.o $(builddir)/utils-chain.o $(builddir)/utils-dbus.o $(builddir)/utils-log.o $(builddir)/event-dispatcher.o $(builddir)/event-loop.o $(builddir)/event-task.o $(builddir)/event-task-factory.o $(builddir)/device-common.o $(builddir)/device-fb.o $(builddir)/device-drm.o $(builddir)/device-evdev.o $(builddir)/device-udev.o $(builddir)/surface-aggregator.o $(builddir)/surface-compositor.o $(builddir)/surface-manager.o $(builddir)/keyboard-bindings.o $(builddir)/renderer-mmap.o $(builddir)/renderer-gl.o $(builddir)/wayland.o $(builddir)/wayland-compositor.o $(builddir)/wayland-surface.o $(builddir)/wayland-shell.o $(builddir)/wayland-shell-surface.o $(builddir)/wayland-xdg_shell.o $(builddir)/xdg-shell-protocol.o $(builddir)/wayland-output.o $(builddir)/bind-egl-wayland.o \
	       `pkg-config --libs wayland-server libdrm dbus-1 gl libudev egl gbm`

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

$(builddir)/event-dispatcher.o: Makefile \
                                $(srcdir)/event-dispatcher.c \
                                $(srcdir)/event-dispatcher.h
	@mkdir -p $(builddir)
	@echo "  CC  event-dispatcher.o"
	@$(CC) $(WFLAGS) -o $(builddir)/event-dispatcher.o -I$(srcdir) \
	       -c $(srcdir)/event-dispatcher.c

$(builddir)/event-loop.o: Makefile \
                          $(srcdir)/event-loop.c \
                          $(srcdir)/event-loop.h
	@mkdir -p $(builddir)
	@echo "  CC  event-loop.o"
	@$(CC) $(WFLAGS) -o $(builddir)/event-loop.o -I$(srcdir) \
	       -c $(srcdir)/event-loop.c

$(builddir)/event-task.o: Makefile \
                          $(srcdir)/event-task.c \
                          $(srcdir)/event-task.h
	@mkdir -p $(builddir)
	@echo "  CC  event-task.o"
	@$(CC) $(WFLAGS) -o $(builddir)/event-task.o -I$(srcdir) \
	       -c $(srcdir)/event-task.c

$(builddir)/event-task-factory.o: Makefile \
                                  $(srcdir)/event-task-factory.c \
                                  $(srcdir)/event-task-factory.h
	@mkdir -p $(builddir)
	@echo "  CC  event-task-factory.o"
	@$(CC) $(WFLAGS) -o $(builddir)/event-task-factory.o -I$(srcdir) \
	       -c $(srcdir)/event-task-factory.c

$(builddir)/device-common.o: Makefile \
                             $(srcdir)/device-common.c \
                             $(srcdir)/device-common.h
	@mkdir -p $(builddir)
	@echo "  CC  device-common.o"
	@$(CC) $(WFLAGS) -o $(builddir)/device-common.o -I$(srcdir) \
	       -c $(srcdir)/device-common.c

$(builddir)/device-fb.o: Makefile \
                         $(srcdir)/device-fb.c \
                         $(srcdir)/device-fb.h
	@mkdir -p $(builddir)
	@echo "  CC  device-fb.o"
	@$(CC) $(WFLAGS) -o $(builddir)/device-fb.o -I$(srcdir) \
	       -c $(srcdir)/device-fb.c

$(builddir)/device-drm.o: Makefile \
                          $(srcdir)/device-drm.c \
                          $(srcdir)/device-drm.h
	@mkdir -p $(builddir)
	@echo "  CC  device-drm.o"
	@$(CC) $(WFLAGS) -o $(builddir)/device-drm.o -I$(srcdir) \
	       -c $(srcdir)/device-drm.c \
	       `pkg-config --cflags egl libdrm`

$(builddir)/device-evdev.o: Makefile \
                            $(srcdir)/device-evdev.c \
                            $(srcdir)/device-evdev.h
	@mkdir -p $(builddir)
	@echo "  CC  device-evdev.o"
	@$(CC) $(WFLAGS) -o $(builddir)/device-evdev.o -I$(srcdir) \
	       -c $(srcdir)/device-evdev.c

$(builddir)/device-udev.o: Makefile \
                           $(srcdir)/device-udev.c \
                           $(srcdir)/device-udev.h
	@mkdir -p $(builddir)
	@echo "  CC  device-udev.o"
	@$(CC) $(WFLAGS) -o $(builddir)/device-udev.o -I$(srcdir) \
	       -c $(srcdir)/device-udev.c

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

$(builddir)/keyboard-bindings.o: Makefile \
                                 $(srcdir)/keyboard-bindings.c \
                                 $(srcdir)/keyboard-bindings.h
	@mkdir -p $(builddir)
	@echo "  CC  keyboard-bindings.o"
	@$(CC) $(WFLAGS) -o $(builddir)/keyboard-bindings.o -I$(srcdir) \
	       -c $(srcdir)/keyboard-bindings.c

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

