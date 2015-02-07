all: build/aura

res/force:

clean:
	rm -rf doc build gen checks callgrind*

checks: checks/check-chain checks/check-store
check: checks
	@time (for c in checks/check*; do $$c; done)


build/aura: Makefile \
            build/config.o \
            build/global-objects.o \
            build/global-functions.o \
            build/utils-object.o \
            build/utils-chain.o \
            build/utils-branch.o \
            build/utils-store.o \
            build/utils-dbus.o \
            build/utils-keymap.o \
            build/utils-log.o \
            build/utils-environment.o \
            build/event-dispatcher.o \
            build/event-timer.o \
            build/event-signals.o \
            build/event-loop.o \
            build/event-task.o \
            build/event-factory.o \
            build/device-common.o \
            build/device-fb.o \
            build/device-drm.o \
            build/device-evdev.o \
            build/device-udev.o \
            build/output.o \
            build/output-collector.o \
            build/surface-data.o \
            build/surface-manager.o \
            build/keyboard-functions.o \
            build/keyboard-binding.o \
            build/keyboard-argmand.o \
            build/keyboard-bindings.o \
            build/keyboard-mode.o \
            build/exhibitor.o \
            build/exhibitor-display.o \
            build/exhibitor-compositor.o \
            build/exhibitor-frame.o \
            build/exhibitor-strategist.o \
            build/exhibitor-pointer.o \
            build/renderer-mmap.o \
            build/renderer-gl.o \
            build/wayland-surface.o \
            build/wayland-output.o \
            build/wayland-cache.o \
            build/wayland-state.o \
            build/wayland.o \
            build/wayland-protocol-compositor.o \
            build/wayland-protocol-surface.o \
            build/wayland-protocol-region.o \
            build/wayland-protocol-shell.o \
            build/wayland-protocol-shell-surface.o \
            build/wayland-protocol-xdg-shell.o \
            build/wayland-protocol-xdg-surface.o \
            build/xdg-shell-protocol.o \
            build/wayland-protocol-output.o \
            build/wayland-protocol-seat.o \
            build/wayland-protocol-pointer.o \
            build/wayland-protocol-keyboard.o \
            build/bind-egl-wayland.o \
            build/backend-gtk-res.o \
            build/backend-gtk-win.o \
            build/backend-gtk-app.o \
            build/backend-gtk.o \
            build/aura.o
	@mkdir -p build
	@echo "  LD   aura"
	@gcc -rdynamic -ldl -lrt -lpthread -lm -DDEBUG -g -O0 -o build/aura \
	       build/config.o build/global-objects.o build/global-functions.o build/utils-object.o build/utils-chain.o build/utils-branch.o build/utils-store.o build/utils-dbus.o build/utils-keymap.o build/utils-log.o build/utils-environment.o build/event-dispatcher.o build/event-timer.o build/event-signals.o build/event-loop.o build/event-task.o build/event-factory.o build/device-common.o build/device-fb.o build/device-drm.o build/device-evdev.o build/device-udev.o build/output.o build/output-collector.o build/surface-data.o build/surface-manager.o build/keyboard-functions.o build/keyboard-binding.o build/keyboard-argmand.o build/keyboard-bindings.o build/keyboard-mode.o build/exhibitor.o build/exhibitor-display.o build/exhibitor-compositor.o build/exhibitor-frame.o build/exhibitor-strategist.o build/exhibitor-pointer.o build/renderer-mmap.o build/renderer-gl.o build/wayland-surface.o build/wayland-output.o build/wayland-cache.o build/wayland-state.o build/wayland.o build/wayland-protocol-compositor.o build/wayland-protocol-surface.o build/wayland-protocol-region.o build/wayland-protocol-shell.o build/wayland-protocol-shell-surface.o build/wayland-protocol-xdg-shell.o build/wayland-protocol-xdg-surface.o build/xdg-shell-protocol.o build/wayland-protocol-output.o build/wayland-protocol-seat.o build/wayland-protocol-pointer.o build/wayland-protocol-keyboard.o build/bind-egl-wayland.o build/backend-gtk-res.o build/backend-gtk-win.o build/backend-gtk-app.o build/backend-gtk.o build/aura.o \
	       -ldbus-1 -lEGL -lgbm -lGL -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -ldrm -ludev -lwayland-server -lxkbcommon 

gen/xdg-shell-server-protocol.h: Makefile \
                                 res/xdg-shell.xml
	@mkdir -p gen
	@echo "  GEN  xdg-shell-server-protocol.h"
	@wayland-scanner server-header  < "res/xdg-shell.xml" > "gen/xdg-shell-server-protocol.h"

gen/xdg-shell-protocol.c: Makefile \
                          res/xdg-shell.xml
	@mkdir -p gen
	@echo "  GEN  xdg-shell-protocol.c"
	@wayland-scanner code  < "res/xdg-shell.xml" > "gen/xdg-shell-protocol.c"

gen/backend-gtk-res.c: Makefile \
                       res/aura.gresource.xml \
                       res/backend-gtk-main.ui \
                       res/backend-gtk-menu.ui \
                       res/backend-gtk-area.ui
	@mkdir -p gen
	@echo "  GEN  backend-gtk-res.c"
	@glib-compile-resources res/aura.gresource.xml --target=gen/backend-gtk-res.c --generate-source

gen/version.h: Makefile \
               res/force
	@mkdir -p gen
	@echo "  GEN  version.h"
	@desc=`git describe --always`; if ! grep -q "$$desc" gen/version.h 2> /dev/null; then echo -e "#ifndef AURA_VERSION\n#define AURA_VERSION \"$$desc\"\n#endif\n" > gen/version.h; fi

build/config.o: Makefile \
                src/config.c \
                src/config.h \
                src/utils-keymap.h \
                src/global-constants.h \
                src/global-types.h \
                src/configuration.h \
                src/global-functions.h \
                src/keyboard-functions.h \
                src/utils-chain.h \
                src/keyboard-binding.h \
                src/keyboard-bindings.h \
                src/utils-log.h
	@mkdir -p build
	@echo "  CC   config.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/config.o -Isrc -Igen \
	       -c src/config.c

build/global-objects.o: Makefile \
                        src/global-objects.c \
                        src/global-objects.h \
                        src/global-types.h \
                        src/utils-object.h \
                        src/global-constants.h \
                        src/utils-log.h
	@mkdir -p build
	@echo "  CC   global-objects.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/global-objects.o -Isrc -Igen \
	       -c src/global-objects.c

build/global-functions.o: Makefile \
                          src/global-functions.c \
                          src/global-functions.h \
                          src/utils-log.h \
                          src/global-constants.h \
                          src/global-types.h \
                          src/event-signals.h \
                          src/event-task.h \
                          src/utils-object.h \
                          src/exhibitor.h \
                          src/exhibitor-display.h \
                          src/exhibitor-compositor.h \
                          src/exhibitor-frame.h \
                          src/utils-branch.h \
                          src/utils-chain.h \
                          src/output.h \
                          src/renderer.h \
                          src/event-loop.h
	@mkdir -p build
	@echo "  CC   global-functions.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/global-functions.o -Isrc -Igen \
	       -c src/global-functions.c

build/utils-object.o: Makefile \
                      src/utils-object.c \
                      src/utils-object.h \
                      src/global-constants.h \
                      src/global-types.h
	@mkdir -p build
	@echo "  CC   utils-object.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/utils-object.o -Isrc -Igen \
	       -c src/utils-object.c

build/utils-chain.o: Makefile \
                     src/utils-chain.c \
                     src/utils-chain.h \
                     src/global-constants.h \
                     src/global-types.h
	@mkdir -p build
	@echo "  CC   utils-chain.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/utils-chain.o -Isrc -Igen \
	       -c src/utils-chain.c

build/utils-branch.o: Makefile \
                      src/utils-branch.c \
                      src/utils-branch.h \
                      src/utils-chain.h \
                      src/global-constants.h \
                      src/global-types.h \
                      src/utils-log.h
	@mkdir -p build
	@echo "  CC   utils-branch.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/utils-branch.o -Isrc -Igen \
	       -c src/utils-branch.c

build/utils-store.o: Makefile \
                     src/utils-store.c \
                     src/utils-store.h \
                     src/global-constants.h \
                     src/global-types.h \
                     src/utils-log.h
	@mkdir -p build
	@echo "  CC   utils-store.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/utils-store.o -Isrc -Igen \
	       -c src/utils-store.c

build/utils-dbus.o: Makefile \
                    src/utils-dbus.c \
                    src/utils-dbus.h \
                    src/utils-log.h \
                    src/global-constants.h \
                    src/global-types.h
	@mkdir -p build
	@echo "  CC   utils-dbus.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/utils-dbus.o -Isrc -Igen \
	       -c src/utils-dbus.c \
	       -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include 

build/utils-keymap.o: Makefile \
                      src/utils-keymap.c \
                      src/utils-keymap.h \
                      src/utils-log.h \
                      src/global-constants.h \
                      src/global-types.h \
                      src/utils-environment.h
	@mkdir -p build
	@echo "  CC   utils-keymap.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/utils-keymap.o -Isrc -Igen \
	       -c src/utils-keymap.c

build/utils-log.o: Makefile \
                   src/utils-log.c \
                   src/config.h \
                   src/utils-keymap.h \
                   src/global-constants.h \
                   src/global-types.h \
                   src/utils-log.h \
                   src/utils-environment.h \
                   gen/version.h
	@mkdir -p build
	@echo "  CC   utils-log.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/utils-log.o -Isrc -Igen \
	       -c src/utils-log.c

build/utils-environment.o: Makefile \
                           src/utils-environment.c \
                           src/utils-environment.h \
                           src/global-constants.h \
                           src/global-types.h \
                           src/utils-log.h
	@mkdir -p build
	@echo "  CC   utils-environment.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/utils-environment.o -Isrc -Igen \
	       -c src/utils-environment.c

build/event-dispatcher.o: Makefile \
                          src/event-dispatcher.c \
                          src/event-dispatcher.h \
                          src/utils-log.h \
                          src/global-constants.h \
                          src/global-types.h \
                          src/utils-chain.h
	@mkdir -p build
	@echo "  CC   event-dispatcher.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/event-dispatcher.o -Isrc -Igen \
	       -c src/event-dispatcher.c

build/event-timer.o: Makefile \
                     src/event-timer.c \
                     src/event-timer.h \
                     src/utils-log.h \
                     src/global-constants.h \
                     src/global-types.h
	@mkdir -p build
	@echo "  CC   event-timer.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/event-timer.o -Isrc -Igen \
	       -c src/event-timer.c

build/event-signals.o: Makefile \
                       src/event-signals.c \
                       src/event-signals.h \
                       src/event-task.h \
                       src/utils-object.h \
                       src/global-constants.h \
                       src/global-types.h \
                       src/event-loop.h \
                       src/utils-log.h \
                       src/utils-chain.h \
                       src/global-objects.h
	@mkdir -p build
	@echo "  CC   event-signals.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/event-signals.o -Isrc -Igen \
	       -c src/event-signals.c

build/event-loop.o: Makefile \
                    src/event-loop.c \
                    src/event-loop.h \
                    src/event-task.h \
                    src/utils-object.h \
                    src/global-constants.h \
                    src/global-types.h \
                    src/utils-log.h \
                    src/utils-chain.h \
                    src/utils-environment.h
	@mkdir -p build
	@echo "  CC   event-loop.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/event-loop.o -Isrc -Igen \
	       -c src/event-loop.c

build/event-task.o: Makefile \
                    src/event-task.c \
                    src/event-task.h \
                    src/utils-object.h \
                    src/global-constants.h \
                    src/global-types.h \
                    src/utils-log.h
	@mkdir -p build
	@echo "  CC   event-task.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/event-task.o -Isrc -Igen \
	       -c src/event-task.c

build/event-factory.o: Makefile \
                       src/event-factory.c \
                       src/event-factory.h \
                       src/event-task.h \
                       src/utils-object.h \
                       src/global-constants.h \
                       src/global-types.h \
                       src/event-dispatcher.h \
                       src/device-evdev.h \
                       src/device-udev.h \
                       src/backend-gtk.h \
                       src/event-loop.h \
                       src/output.h \
                       src/renderer.h \
                       src/utils-chain.h \
                       src/output-collector.h \
                       src/exhibitor.h \
                       src/exhibitor-display.h \
                       src/exhibitor-compositor.h \
                       src/exhibitor-frame.h \
                       src/utils-branch.h \
                       src/wayland.h \
                       src/utils-environment.h
	@mkdir -p build
	@echo "  CC   event-factory.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/event-factory.o -Isrc -Igen \
	       -c src/event-factory.c

build/device-common.o: Makefile \
                       src/device-common.c \
                       src/device-common.h \
                       src/utils-dbus.h \
                       src/utils-log.h \
                       src/global-constants.h \
                       src/global-types.h
	@mkdir -p build
	@echo "  CC   device-common.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/device-common.o -Isrc -Igen \
	       -c src/device-common.c

build/device-fb.o: Makefile \
                   src/device-fb.c \
                   src/device-fb.h \
                   src/output.h \
                   src/renderer.h \
                   src/utils-chain.h \
                   src/global-constants.h \
                   src/global-types.h \
                   src/utils-object.h \
                   src/device-common.h \
                   src/renderer-mmap.h \
                   src/utils-log.h
	@mkdir -p build
	@echo "  CC   device-fb.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/device-fb.o -Isrc -Igen \
	       -c src/device-fb.c

build/device-drm.o: Makefile \
                    src/device-drm.c \
                    src/device-drm.h \
                    src/output.h \
                    src/renderer.h \
                    src/utils-chain.h \
                    src/global-constants.h \
                    src/global-types.h \
                    src/utils-object.h \
                    src/utils-log.h \
                    src/renderer-mmap.h \
                    src/renderer-gl.h \
                    src/utils-dbus.h
	@mkdir -p build
	@echo "  CC   device-drm.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/device-drm.o -Isrc -Igen \
	       -c src/device-drm.c \
	       -I/usr/include/libdrm 

build/device-evdev.o: Makefile \
                      src/device-evdev.c \
                      src/device-evdev.h \
                      src/event-dispatcher.h \
                      src/device-common.h \
                      src/utils-dbus.h \
                      src/utils-log.h \
                      src/global-constants.h \
                      src/global-types.h \
                      src/global-objects.h \
                      src/utils-object.h \
                      src/event-signals.h \
                      src/event-task.h \
                      src/keyboard-bindings.h \
                      src/keyboard-binding.h \
                      src/utils-chain.h
	@mkdir -p build
	@echo "  CC   device-evdev.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/device-evdev.o -Isrc -Igen \
	       -c src/device-evdev.c

build/device-udev.o: Makefile \
                     src/device-udev.c \
                     src/device-udev.h \
                     src/event-dispatcher.h \
                     src/utils-log.h \
                     src/global-constants.h \
                     src/global-types.h \
                     src/event-signals.h \
                     src/event-task.h \
                     src/utils-object.h
	@mkdir -p build
	@echo "  CC   device-udev.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/device-udev.o -Isrc -Igen \
	       -c src/device-udev.c

build/output.o: Makefile \
                src/output.c \
                src/output.h \
                src/renderer.h \
                src/utils-chain.h \
                src/global-constants.h \
                src/global-types.h \
                src/utils-object.h
	@mkdir -p build
	@echo "  CC   output.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/output.o -Isrc -Igen \
	       -c src/output.c

build/output-collector.o: Makefile \
                          src/output-collector.c \
                          src/output-collector.h \
                          src/event-loop.h \
                          src/event-task.h \
                          src/utils-object.h \
                          src/global-constants.h \
                          src/global-types.h \
                          src/event-signals.h \
                          src/utils-log.h \
                          src/device-drm.h \
                          src/output.h \
                          src/renderer.h \
                          src/utils-chain.h \
                          src/device-fb.h \
                          src/backend-gtk.h \
                          src/config.h \
                          src/utils-keymap.h
	@mkdir -p build
	@echo "  CC   output-collector.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/output-collector.o -Isrc -Igen \
	       -c src/output-collector.c

build/surface-data.o: Makefile \
                      src/surface-data.c \
                      src/surface-data.h \
                      src/global-types.h \
                      src/utils-store.h \
                      src/global-constants.h \
                      src/exhibitor-compositor.h \
                      src/exhibitor-frame.h \
                      src/utils-branch.h \
                      src/utils-chain.h \
                      src/utils-log.h
	@mkdir -p build
	@echo "  CC   surface-data.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/surface-data.o -Isrc -Igen \
	       -c src/surface-data.c

build/surface-manager.o: Makefile \
                         src/surface-manager.c \
                         src/surface-manager.h \
                         src/event-loop.h \
                         src/event-task.h \
                         src/utils-object.h \
                         src/global-constants.h \
                         src/global-types.h \
                         src/surface-data.h \
                         src/utils-store.h \
                         src/exhibitor-compositor.h \
                         src/exhibitor-frame.h \
                         src/utils-branch.h \
                         src/utils-chain.h \
                         src/utils-log.h \
                         src/event-timer.h \
                         src/event-signals.h
	@mkdir -p build
	@echo "  CC   surface-manager.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/surface-manager.o -Isrc -Igen \
	       -c src/surface-manager.c

build/keyboard-functions.o: Makefile \
                            src/keyboard-functions.c \
                            src/keyboard-functions.h \
                            src/utils-chain.h \
                            src/global-constants.h \
                            src/global-types.h \
                            src/keyboard-argmand.h \
                            src/utils-log.h \
                            src/exhibitor.h \
                            src/exhibitor-display.h \
                            src/exhibitor-compositor.h \
                            src/exhibitor-frame.h \
                            src/utils-branch.h \
                            src/output.h \
                            src/renderer.h \
                            src/utils-object.h \
                            src/event-loop.h \
                            src/event-task.h
	@mkdir -p build
	@echo "  CC   keyboard-functions.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/keyboard-functions.o -Isrc -Igen \
	       -c src/keyboard-functions.c

build/keyboard-binding.o: Makefile \
                          src/keyboard-binding.c \
                          src/keyboard-binding.h \
                          src/utils-chain.h \
                          src/global-constants.h \
                          src/global-types.h \
                          src/utils-log.h
	@mkdir -p build
	@echo "  CC   keyboard-binding.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/keyboard-binding.o -Isrc -Igen \
	       -c src/keyboard-binding.c

build/keyboard-argmand.o: Makefile \
                          src/keyboard-argmand.c \
                          src/keyboard-argmand.h \
                          src/global-constants.h \
                          src/global-types.h \
                          src/utils-log.h
	@mkdir -p build
	@echo "  CC   keyboard-argmand.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/keyboard-argmand.o -Isrc -Igen \
	       -c src/keyboard-argmand.c

build/keyboard-bindings.o: Makefile \
                           src/keyboard-bindings.c \
                           src/keyboard-bindings.h \
                           src/keyboard-binding.h \
                           src/utils-chain.h \
                           src/global-constants.h \
                           src/global-types.h \
                           src/keyboard-mode.h \
                           src/keyboard-argmand.h \
                           src/utils-log.h
	@mkdir -p build
	@echo "  CC   keyboard-bindings.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/keyboard-bindings.o -Isrc -Igen \
	       -c src/keyboard-bindings.c

build/keyboard-mode.o: Makefile \
                       src/keyboard-mode.c \
                       src/keyboard-mode.h \
                       src/keyboard-binding.h \
                       src/utils-chain.h \
                       src/global-constants.h \
                       src/global-types.h \
                       src/utils-log.h
	@mkdir -p build
	@echo "  CC   keyboard-mode.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/keyboard-mode.o -Isrc -Igen \
	       -c src/keyboard-mode.c

build/exhibitor.o: Makefile \
                   src/exhibitor.c \
                   src/exhibitor.h \
                   src/exhibitor-display.h \
                   src/exhibitor-compositor.h \
                   src/exhibitor-frame.h \
                   src/utils-branch.h \
                   src/utils-chain.h \
                   src/global-constants.h \
                   src/global-types.h \
                   src/output.h \
                   src/renderer.h \
                   src/utils-object.h \
                   src/event-loop.h \
                   src/event-task.h \
                   src/exhibitor-strategist.h \
                   src/exhibitor-pointer.h \
                   src/utils-log.h \
                   src/event-signals.h \
                   src/global-objects.h \
                   src/surface-manager.h \
                   src/surface-data.h \
                   src/utils-store.h
	@mkdir -p build
	@echo "  CC   exhibitor.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/exhibitor.o -Isrc -Igen \
	       -c src/exhibitor.c

build/exhibitor-display.o: Makefile \
                           src/exhibitor-display.c \
                           src/exhibitor-display.h \
                           src/exhibitor-compositor.h \
                           src/exhibitor-frame.h \
                           src/utils-branch.h \
                           src/utils-chain.h \
                           src/global-constants.h \
                           src/global-types.h \
                           src/output.h \
                           src/renderer.h \
                           src/utils-object.h \
                           src/exhibitor-pointer.h \
                           src/event-loop.h \
                           src/event-task.h \
                           src/utils-log.h \
                           src/utils-environment.h \
                           src/event-timer.h \
                           src/event-signals.h \
                           src/surface-manager.h \
                           src/surface-data.h \
                           src/utils-store.h
	@mkdir -p build
	@echo "  CC   exhibitor-display.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/exhibitor-display.o -Isrc -Igen \
	       -c src/exhibitor-display.c

build/exhibitor-compositor.o: Makefile \
                              src/exhibitor-compositor.c \
                              src/exhibitor-compositor.h \
                              src/exhibitor-frame.h \
                              src/utils-branch.h \
                              src/utils-chain.h \
                              src/global-constants.h \
                              src/global-types.h \
                              src/surface-manager.h \
                              src/event-loop.h \
                              src/event-task.h \
                              src/utils-object.h \
                              src/surface-data.h \
                              src/utils-store.h \
                              src/utils-log.h
	@mkdir -p build
	@echo "  CC   exhibitor-compositor.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/exhibitor-compositor.o -Isrc -Igen \
	       -c src/exhibitor-compositor.c

build/exhibitor-frame.o: Makefile \
                         src/exhibitor-frame.c \
                         src/exhibitor-frame.h \
                         src/utils-branch.h \
                         src/utils-chain.h \
                         src/global-constants.h \
                         src/global-types.h \
                         src/utils-log.h \
                         src/surface-manager.h \
                         src/event-loop.h \
                         src/event-task.h \
                         src/utils-object.h \
                         src/surface-data.h \
                         src/utils-store.h \
                         src/exhibitor-compositor.h \
                         src/event-signals.h
	@mkdir -p build
	@echo "  CC   exhibitor-frame.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/exhibitor-frame.o -Isrc -Igen \
	       -c src/exhibitor-frame.c

build/exhibitor-strategist.o: Makefile \
                              src/exhibitor-strategist.c \
                              src/exhibitor-strategist.h \
                              src/exhibitor.h \
                              src/exhibitor-display.h \
                              src/exhibitor-compositor.h \
                              src/exhibitor-frame.h \
                              src/utils-branch.h \
                              src/utils-chain.h \
                              src/global-constants.h \
                              src/global-types.h \
                              src/output.h \
                              src/renderer.h \
                              src/utils-object.h \
                              src/event-loop.h \
                              src/event-task.h \
                              src/utils-log.h \
                              src/event-signals.h
	@mkdir -p build
	@echo "  CC   exhibitor-strategist.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/exhibitor-strategist.o -Isrc -Igen \
	       -c src/exhibitor-strategist.c

build/exhibitor-pointer.o: Makefile \
                           src/exhibitor-pointer.c \
                           src/exhibitor-pointer.h \
                           src/event-loop.h \
                           src/event-task.h \
                           src/utils-object.h \
                           src/global-constants.h \
                           src/global-types.h \
                           src/exhibitor.h \
                           src/exhibitor-display.h \
                           src/exhibitor-compositor.h \
                           src/exhibitor-frame.h \
                           src/utils-branch.h \
                           src/utils-chain.h \
                           src/output.h \
                           src/renderer.h \
                           src/utils-log.h \
                           src/global-objects.h \
                           src/event-signals.h
	@mkdir -p build
	@echo "  CC   exhibitor-pointer.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/exhibitor-pointer.o -Isrc -Igen \
	       -c src/exhibitor-pointer.c

build/renderer-mmap.o: Makefile \
                       src/renderer-mmap.c \
                       src/renderer-mmap.h \
                       src/output.h \
                       src/renderer.h \
                       src/utils-chain.h \
                       src/global-constants.h \
                       src/global-types.h \
                       src/utils-object.h \
                       src/surface-manager.h \
                       src/event-loop.h \
                       src/event-task.h \
                       src/surface-data.h \
                       src/utils-store.h \
                       src/exhibitor-compositor.h \
                       src/exhibitor-frame.h \
                       src/utils-branch.h \
                       src/utils-log.h
	@mkdir -p build
	@echo "  CC   renderer-mmap.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/renderer-mmap.o -Isrc -Igen \
	       -c src/renderer-mmap.c

build/renderer-gl.o: Makefile \
                     src/renderer-gl.c \
                     src/renderer-gl.h \
                     src/renderer.h \
                     src/utils-chain.h \
                     src/global-constants.h \
                     src/global-types.h \
                     src/surface-manager.h \
                     src/event-loop.h \
                     src/event-task.h \
                     src/utils-object.h \
                     src/surface-data.h \
                     src/utils-store.h \
                     src/exhibitor-compositor.h \
                     src/exhibitor-frame.h \
                     src/utils-branch.h \
                     src/utils-log.h \
                     src/bind-egl-wayland.h
	@mkdir -p build
	@echo "  CC   renderer-gl.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/renderer-gl.o -Isrc -Igen \
	       -c src/renderer-gl.c \
	       -I/usr/include/libdrm 

build/wayland-surface.o: Makefile \
                         src/wayland-surface.c \
                         src/wayland-surface.h \
                         src/global-types.h \
                         src/wayland-types.h \
                         src/utils-log.h \
                         src/global-constants.h
	@mkdir -p build
	@echo "  CC   wayland-surface.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-surface.o -Isrc -Igen \
	       -c src/wayland-surface.c

build/wayland-output.o: Makefile \
                        src/wayland-output.c \
                        src/wayland-output.h \
                        src/utils-store.h \
                        src/global-constants.h \
                        src/global-types.h \
                        src/output.h \
                        src/renderer.h \
                        src/utils-chain.h \
                        src/utils-object.h \
                        src/utils-log.h
	@mkdir -p build
	@echo "  CC   wayland-output.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-output.o -Isrc -Igen \
	       -c src/wayland-output.c

build/wayland-cache.o: Makefile \
                       src/wayland-cache.c \
                       src/wayland-cache.h \
                       src/utils-store.h \
                       src/global-constants.h \
                       src/global-types.h \
                       src/wayland-surface.h \
                       src/wayland-types.h \
                       src/utils-chain.h \
                       src/utils-log.h
	@mkdir -p build
	@echo "  CC   wayland-cache.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-cache.o -Isrc -Igen \
	       -c src/wayland-cache.c

build/wayland-state.o: Makefile \
                       src/wayland-state.c \
                       src/wayland-state.h \
                       src/utils-store.h \
                       src/global-constants.h \
                       src/global-types.h \
                       src/output.h \
                       src/renderer.h \
                       src/utils-chain.h \
                       src/utils-object.h \
                       src/wayland-surface.h \
                       src/wayland-types.h \
                       src/wayland-output.h \
                       src/wayland-cache.h \
                       src/wayland-protocol-output.h \
                       src/surface-manager.h \
                       src/event-loop.h \
                       src/event-task.h \
                       src/surface-data.h \
                       src/exhibitor-compositor.h \
                       src/exhibitor-frame.h \
                       src/utils-branch.h \
                       src/utils-log.h \
                       gen/xdg-shell-server-protocol.h
	@mkdir -p build
	@echo "  CC   wayland-state.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-state.o -Isrc -Igen \
	       -c src/wayland-state.c

build/wayland.o: Makefile \
                 src/wayland.c \
                 src/wayland.h \
                 src/event-loop.h \
                 src/event-task.h \
                 src/utils-object.h \
                 src/global-constants.h \
                 src/global-types.h \
                 src/wayland-protocol-compositor.h \
                 src/wayland-protocol-shell.h \
                 src/wayland-protocol-seat.h \
                 src/wayland-protocol-xdg-shell.h \
                 src/wayland-protocol-output.h \
                 src/wayland-cache.h \
                 src/utils-store.h \
                 src/wayland-surface.h \
                 src/wayland-types.h \
                 src/utils-chain.h \
                 src/wayland-state.h \
                 src/output.h \
                 src/renderer.h \
                 src/utils-log.h \
                 src/utils-environment.h \
                 src/global-objects.h \
                 src/event-signals.h \
                 gen/xdg-shell-server-protocol.h
	@mkdir -p build
	@echo "  CC   wayland.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland.o -Isrc -Igen \
	       -c src/wayland.c

build/wayland-protocol-compositor.o: Makefile \
                                     src/wayland-protocol-compositor.c \
                                     src/wayland-protocol-compositor.h \
                                     src/wayland-protocol-surface.h \
                                     src/wayland-protocol-region.h \
                                     src/wayland-state.h \
                                     src/utils-store.h \
                                     src/global-constants.h \
                                     src/global-types.h \
                                     src/output.h \
                                     src/renderer.h \
                                     src/utils-chain.h \
                                     src/utils-object.h \
                                     src/wayland-surface.h \
                                     src/wayland-types.h \
                                     src/surface-manager.h \
                                     src/event-loop.h \
                                     src/event-task.h \
                                     src/surface-data.h \
                                     src/exhibitor-compositor.h \
                                     src/exhibitor-frame.h \
                                     src/utils-branch.h \
                                     src/utils-log.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-compositor.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-protocol-compositor.o -Isrc -Igen \
	       -c src/wayland-protocol-compositor.c

build/wayland-protocol-surface.o: Makefile \
                                  src/wayland-protocol-surface.c \
                                  src/wayland-protocol-surface.h \
                                  src/wayland-state.h \
                                  src/utils-store.h \
                                  src/global-constants.h \
                                  src/global-types.h \
                                  src/output.h \
                                  src/renderer.h \
                                  src/utils-chain.h \
                                  src/utils-object.h \
                                  src/wayland-surface.h \
                                  src/wayland-types.h \
                                  src/surface-manager.h \
                                  src/event-loop.h \
                                  src/event-task.h \
                                  src/surface-data.h \
                                  src/exhibitor-compositor.h \
                                  src/exhibitor-frame.h \
                                  src/utils-branch.h \
                                  src/utils-log.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-surface.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-protocol-surface.o -Isrc -Igen \
	       -c src/wayland-protocol-surface.c

build/wayland-protocol-region.o: Makefile \
                                 src/wayland-protocol-region.c \
                                 src/wayland-protocol-region.h \
                                 src/utils-log.h \
                                 src/global-constants.h \
                                 src/global-types.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-region.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-protocol-region.o -Isrc -Igen \
	       -c src/wayland-protocol-region.c

build/wayland-protocol-shell.o: Makefile \
                                src/wayland-protocol-shell.c \
                                src/wayland-protocol-shell.h \
                                src/wayland-protocol-shell-surface.h \
                                src/wayland-cache.h \
                                src/utils-store.h \
                                src/global-constants.h \
                                src/global-types.h \
                                src/wayland-surface.h \
                                src/wayland-types.h \
                                src/utils-chain.h \
                                src/utils-log.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-shell.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-protocol-shell.o -Isrc -Igen \
	       -c src/wayland-protocol-shell.c

build/wayland-protocol-shell-surface.o: Makefile \
                                        src/wayland-protocol-shell-surface.c \
                                        src/wayland-protocol-shell-surface.c \
                                        src/wayland-protocol-shell-surface.h \
                                        src/utils-log.h \
                                        src/global-constants.h \
                                        src/global-types.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-shell-surface.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-protocol-shell-surface.o -Isrc -Igen \
	       -c src/wayland-protocol-shell-surface.c

build/wayland-protocol-xdg-shell.o: Makefile \
                                    src/wayland-protocol-xdg-shell.c \
                                    src/wayland-protocol-xdg-shell.h \
                                    src/wayland-protocol-xdg-surface.h \
                                    src/wayland-cache.h \
                                    src/utils-store.h \
                                    src/global-constants.h \
                                    src/global-types.h \
                                    src/wayland-surface.h \
                                    src/wayland-types.h \
                                    src/utils-chain.h \
                                    src/utils-log.h \
                                    gen/xdg-shell-server-protocol.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-xdg-shell.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-protocol-xdg-shell.o -Isrc -Igen \
	       -c src/wayland-protocol-xdg-shell.c

build/wayland-protocol-xdg-surface.o: Makefile \
                                      src/wayland-protocol-xdg-surface.c \
                                      src/wayland-protocol-xdg-surface.h \
                                      src/utils-log.h \
                                      src/global-constants.h \
                                      src/global-types.h \
                                      src/surface-manager.h \
                                      src/event-loop.h \
                                      src/event-task.h \
                                      src/utils-object.h \
                                      src/surface-data.h \
                                      src/utils-store.h \
                                      src/exhibitor-compositor.h \
                                      src/exhibitor-frame.h \
                                      src/utils-branch.h \
                                      src/utils-chain.h \
                                      gen/xdg-shell-server-protocol.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-xdg-surface.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-protocol-xdg-surface.o -Isrc -Igen \
	       -c src/wayland-protocol-xdg-surface.c

build/xdg-shell-protocol.o: Makefile \
                            gen/xdg-shell-protocol.c
	@mkdir -p build
	@echo "  CC   xdg-shell-protocol.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/xdg-shell-protocol.o -Isrc -Igen \
	       -c gen/xdg-shell-protocol.c

build/wayland-protocol-output.o: Makefile \
                                 src/wayland-protocol-output.c \
                                 src/wayland-protocol-output.h \
                                 src/utils-log.h \
                                 src/global-constants.h \
                                 src/global-types.h \
                                 src/output.h \
                                 src/renderer.h \
                                 src/utils-chain.h \
                                 src/utils-object.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-output.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-protocol-output.o -Isrc -Igen \
	       -c src/wayland-protocol-output.c

build/wayland-protocol-seat.o: Makefile \
                               src/wayland-protocol-seat.c \
                               src/wayland-protocol-seat.h \
                               src/wayland-protocol-pointer.h \
                               src/wayland-protocol-keyboard.h \
                               src/wayland-state.h \
                               src/utils-store.h \
                               src/global-constants.h \
                               src/global-types.h \
                               src/output.h \
                               src/renderer.h \
                               src/utils-chain.h \
                               src/utils-object.h \
                               src/wayland-surface.h \
                               src/wayland-types.h \
                               src/utils-log.h \
                               src/config.h \
                               src/utils-keymap.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-seat.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-protocol-seat.o -Isrc -Igen \
	       -c src/wayland-protocol-seat.c

build/wayland-protocol-pointer.o: Makefile \
                                  src/wayland-protocol-pointer.c \
                                  src/wayland-protocol-keyboard.h \
                                  src/utils-log.h \
                                  src/global-constants.h \
                                  src/global-types.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-pointer.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-protocol-pointer.o -Isrc -Igen \
	       -c src/wayland-protocol-pointer.c

build/wayland-protocol-keyboard.o: Makefile \
                                   src/wayland-protocol-keyboard.c \
                                   src/wayland-protocol-keyboard.h \
                                   src/utils-log.h \
                                   src/global-constants.h \
                                   src/global-types.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-keyboard.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/wayland-protocol-keyboard.o -Isrc -Igen \
	       -c src/wayland-protocol-keyboard.c

build/bind-egl-wayland.o: Makefile \
                          src/bind-egl-wayland.c \
                          src/renderer-gl.h \
                          src/renderer.h \
                          src/utils-chain.h \
                          src/global-constants.h \
                          src/global-types.h \
                          src/wayland.h \
                          src/event-loop.h \
                          src/event-task.h \
                          src/utils-object.h \
                          src/utils-log.h
	@mkdir -p build
	@echo "  CC   bind-egl-wayland.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/bind-egl-wayland.o -Isrc -Igen \
	       -c src/bind-egl-wayland.c

build/backend-gtk-res.o: Makefile \
                         gen/backend-gtk-res.c
	@mkdir -p build
	@echo "  CC   backend-gtk-res.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/backend-gtk-res.o -Isrc -Igen \
	       -c gen/backend-gtk-res.c \
	       -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/harfbuzz -I/usr/include/libdrm -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include 

build/backend-gtk-win.o: Makefile \
                         src/backend-gtk-win.c \
                         src/backend-gtk-app.h \
                         src/global-types.h \
                         src/backend-gtk-win.h \
                         src/utils-log.h \
                         src/global-constants.h \
                         src/event-signals.h \
                         src/event-task.h \
                         src/utils-object.h \
                         gen/version.h
	@mkdir -p build
	@echo "  CC   backend-gtk-win.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/backend-gtk-win.o -Isrc -Igen \
	       -c src/backend-gtk-win.c \
	       -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/harfbuzz -I/usr/include/libdrm -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include 

build/backend-gtk-app.o: Makefile \
                         src/backend-gtk-app.c \
                         src/backend-gtk-app.h \
                         src/global-types.h \
                         src/backend-gtk-win.h \
                         src/utils-log.h \
                         src/global-constants.h \
                         src/event-signals.h \
                         src/event-task.h \
                         src/utils-object.h
	@mkdir -p build
	@echo "  CC   backend-gtk-app.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/backend-gtk-app.o -Isrc -Igen \
	       -c src/backend-gtk-app.c \
	       -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/harfbuzz -I/usr/include/libdrm -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include 

build/backend-gtk.o: Makefile \
                     src/backend-gtk.c \
                     src/backend-gtk.h \
                     src/event-loop.h \
                     src/event-task.h \
                     src/utils-object.h \
                     src/global-constants.h \
                     src/global-types.h \
                     src/output.h \
                     src/renderer.h \
                     src/utils-chain.h \
                     src/backend-gtk-app.h \
                     src/utils-log.h \
                     src/utils-environment.h \
                     src/global-functions.h \
                     src/renderer-mmap.h
	@mkdir -p build
	@echo "  CC   backend-gtk.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/backend-gtk.o -Isrc -Igen \
	       -c src/backend-gtk.c \
	       -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/harfbuzz -I/usr/include/libdrm -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include 

build/aura.o: Makefile \
              src/aura.c \
              src/utils-log.h \
              src/global-constants.h \
              src/global-types.h \
              src/utils-dbus.h \
              src/utils-environment.h \
              src/event-dispatcher.h \
              src/event-loop.h \
              src/event-task.h \
              src/utils-object.h \
              src/event-signals.h \
              src/event-factory.h \
              src/surface-manager.h \
              src/surface-data.h \
              src/utils-store.h \
              src/exhibitor-compositor.h \
              src/exhibitor-frame.h \
              src/utils-branch.h \
              src/utils-chain.h \
              src/config.h \
              src/utils-keymap.h
	@mkdir -p build
	@echo "  CC   aura.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o build/aura.o -Isrc -Igen \
	       -c src/aura.c

checks/check-chain: Makefile \
                    tests/test-chain.c \
                    src/utils-chain.c \
                    tests/../src/utils-chain.h \
                    tests/../src/global-constants.h \
                    tests/../src/global-types.h \
                    tests/tests-suit.h \
                    src/utils-chain.h \
                    src/global-constants.h \
                    src/global-types.h
	@mkdir -p checks
	@echo "  CC   check-chain"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o checks/check-chain -Isrc -Igen \
	      tests/test-chain.c src/utils-chain.c

checks/check-store: Makefile \
                    tests/test-store.c \
                    src/utils-store.c \
                    src/utils-chain.c \
                    tests/../src/utils-store.h \
                    tests/../src/global-constants.h \
                    tests/../src/global-types.h \
                    tests/tests-suit.h \
                    src/utils-store.h \
                    src/global-constants.h \
                    src/global-types.h \
                    src/utils-log.h \
                    src/utils-chain.h \
                    src/global-constants.h \
                    src/global-types.h
	@mkdir -p checks
	@echo "  CC   check-store"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -DDEBUG -g -O0 -o checks/check-store -Isrc -Igen \
	      tests/test-store.c src/utils-store.c src/utils-chain.c

