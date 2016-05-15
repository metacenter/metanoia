default: version_info_file build/metanoia build/metanoiactl-gtk

all: build/metanoia build/metanoiactl-gtk checks/check-globals checks/check-pool checks/check-store checks/check-chain checks/check-list checks/check-branch checks/check-frame

checks: checks/check-globals checks/check-pool checks/check-store checks/check-chain checks/check-list checks/check-branch checks/check-frame

check: checks
	@./checks/check-globals
	@./checks/check-pool
	@./checks/check-store
	@./checks/check-chain
	@./checks/check-list
	@./checks/check-branch
	@./checks/check-frame

clean:
	rm -rf build checks gen install inter doc callgrind* *plist* .ninja*

force:

cppcheck: Makefile \
          checks
	@python3 -B ./share/build/make.py cppcheck

install: force
	@python3 -B ./share/build/make.py install

make: force
	@python3 -B ./share/build/make.py make

memcheck: Makefile \
          checks
	@python3 -B ./share/build/make.py memcheck

ninja: force
	@python3 -B ./share/build/make.py ninja

build/metanoia: Makefile \
                inter/config.o \
                inter/global-enums.o \
                inter/global-types.o \
                inter/global-objects.o \
                inter/global-functions.o \
                inter/utils-debug.o \
                inter/utils-object.o \
                inter/utils-pool.o \
                inter/utils-store.o \
                inter/utils-chain.o \
                inter/utils-list.o \
                inter/utils-branch.o \
                inter/utils-log.o \
                inter/utils-environment.o \
                inter/utils-dbus.o \
                inter/utils-gl.o \
                inter/utils-keymap.o \
                inter/utils-keyboard-state.o \
                inter/utils-image.o \
                inter/utils-time.o \
                inter/event-dispatcher.o \
                inter/event-factory.o \
                inter/event-loop.o \
                inter/event-signals.o \
                inter/event-task.o \
                inter/event-timer.o \
                inter/surface-coordinator.o \
                inter/surface-data.o \
                inter/renderer.o \
                inter/renderer-mmap.o \
                inter/renderer-gl.o \
                inter/device-common.o \
                inter/device-fb.o \
                inter/device-drm.o \
                inter/device-evdev.o \
                inter/device-udev.o \
                inter/output.o \
                inter/output-collector.o \
                inter/input-binding.o \
                inter/input-bindings.o \
                inter/input-context.o \
                inter/input-functions.o \
                inter/input-mode.o \
                inter/exhibitor-frame.o \
                inter/exhibitor-frame-internal.o \
                inter/exhibitor-strategist.o \
                inter/exhibitor-compositor.o \
                inter/exhibitor-display.o \
                inter/exhibitor-pointer.o \
                inter/exhibitor.o \
                inter/exhibitor-module.o \
                inter/wayland-region.o \
                inter/wayland-surface.o \
                inter/wayland-output.o \
                inter/wayland-cache.o \
                inter/wayland-state.o \
                inter/wayland.o \
                inter/xdg-shell-protocol.o \
                inter/screenshooter-protocol.o \
                inter/wayland-protocol-compositor.o \
                inter/wayland-protocol-data-device.o \
                inter/wayland-protocol-data-source.o \
                inter/wayland-protocol-device-manager.o \
                inter/wayland-protocol-keyboard.o \
                inter/wayland-protocol-output.o \
                inter/wayland-protocol-pointer.o \
                inter/wayland-protocol-region.o \
                inter/wayland-protocol-screenshooter.o \
                inter/wayland-protocol-seat.o \
                inter/wayland-protocol-shell-surface.o \
                inter/wayland-protocol-shell.o \
                inter/wayland-protocol-subcompositor.o \
                inter/wayland-protocol-subsurface.o \
                inter/wayland-protocol-surface.o \
                inter/wayland-protocol-xdg-shell.o \
                inter/wayland-protocol-xdg-surface.o \
                inter/backend-offscreen.o \
                inter/metanoia.o
	@mkdir -p build
	@echo "  LD   build/metanoia"
	@clang -DDEBUG -g -O0 -rdynamic -ldl -lrt -lpthread -lm -lEGL -lGL -ldbus-1 -ldrm -lgbm -ljpeg -ludev -lwayland-server -lxkbcommon -o build/metanoia \
	    inter/config.o \
	    inter/global-enums.o \
	    inter/global-types.o \
	    inter/global-objects.o \
	    inter/global-functions.o \
	    inter/utils-debug.o \
	    inter/utils-object.o \
	    inter/utils-pool.o \
	    inter/utils-store.o \
	    inter/utils-chain.o \
	    inter/utils-list.o \
	    inter/utils-branch.o \
	    inter/utils-log.o \
	    inter/utils-environment.o \
	    inter/utils-dbus.o \
	    inter/utils-gl.o \
	    inter/utils-keymap.o \
	    inter/utils-keyboard-state.o \
	    inter/utils-image.o \
	    inter/utils-time.o \
	    inter/event-dispatcher.o \
	    inter/event-factory.o \
	    inter/event-loop.o \
	    inter/event-signals.o \
	    inter/event-task.o \
	    inter/event-timer.o \
	    inter/surface-coordinator.o \
	    inter/surface-data.o \
	    inter/renderer.o \
	    inter/renderer-mmap.o \
	    inter/renderer-gl.o \
	    inter/device-common.o \
	    inter/device-fb.o \
	    inter/device-drm.o \
	    inter/device-evdev.o \
	    inter/device-udev.o \
	    inter/output.o \
	    inter/output-collector.o \
	    inter/input-binding.o \
	    inter/input-bindings.o \
	    inter/input-context.o \
	    inter/input-functions.o \
	    inter/input-mode.o \
	    inter/exhibitor-frame.o \
	    inter/exhibitor-frame-internal.o \
	    inter/exhibitor-strategist.o \
	    inter/exhibitor-compositor.o \
	    inter/exhibitor-display.o \
	    inter/exhibitor-pointer.o \
	    inter/exhibitor.o \
	    inter/exhibitor-module.o \
	    inter/wayland-region.o \
	    inter/wayland-surface.o \
	    inter/wayland-output.o \
	    inter/wayland-cache.o \
	    inter/wayland-state.o \
	    inter/wayland.o \
	    inter/xdg-shell-protocol.o \
	    inter/screenshooter-protocol.o \
	    inter/wayland-protocol-compositor.o \
	    inter/wayland-protocol-data-device.o \
	    inter/wayland-protocol-data-source.o \
	    inter/wayland-protocol-device-manager.o \
	    inter/wayland-protocol-keyboard.o \
	    inter/wayland-protocol-output.o \
	    inter/wayland-protocol-pointer.o \
	    inter/wayland-protocol-region.o \
	    inter/wayland-protocol-screenshooter.o \
	    inter/wayland-protocol-seat.o \
	    inter/wayland-protocol-shell-surface.o \
	    inter/wayland-protocol-shell.o \
	    inter/wayland-protocol-subcompositor.o \
	    inter/wayland-protocol-subsurface.o \
	    inter/wayland-protocol-surface.o \
	    inter/wayland-protocol-xdg-shell.o \
	    inter/wayland-protocol-xdg-surface.o \
	    inter/backend-offscreen.o \
	    inter/metanoia.o

build/metanoiactl-gtk: Makefile \
                       inter/utils-debug.o \
                       inter/utils-time.o \
                       inter/utils-log.o \
                       inter/utils-environment.o \
                       gen/screenshooter-protocol.c \
                       inter/controller-defs.o \
                       inter/controller-output.o \
                       inter/controller-gtk-res.o \
                       inter/controller-gtk-display.o \
                       inter/controller-gtk-win.o \
                       inter/controller-gtk-app.o \
                       inter/controller-wayland.o \
                       inter/metanoiactl-gtk.o
	@mkdir -p build
	@echo "  LD   build/metanoiactl-gtk"
	@clang -DDEBUG -g -O0 -rdynamic -ldl -lrt -lpthread -lm -latk-1.0 -lcairo -lcairo-gobject -lgdk-3 -lgdk_pixbuf-2.0 -lgio-2.0 -lglib-2.0 -lgobject-2.0 -lgtk-3 -lpango-1.0 -lpangocairo-1.0 -lwayland-client -o build/metanoiactl-gtk \
	    inter/utils-debug.o \
	    inter/utils-time.o \
	    inter/utils-log.o \
	    inter/utils-environment.o \
	    gen/screenshooter-protocol.c \
	    inter/controller-defs.o \
	    inter/controller-output.o \
	    inter/controller-gtk-res.o \
	    inter/controller-gtk-display.o \
	    inter/controller-gtk-win.o \
	    inter/controller-gtk-app.o \
	    inter/controller-wayland.o \
	    inter/metanoiactl-gtk.o

checks/check-globals: Makefile \
                      inter/test-globals.o \
                      inter/global-types.o
	@mkdir -p checks
	@echo "  LD   checks/check-globals"
	@clang -DDEBUG -g -O0 -rdynamic -ldl -lrt -lpthread -lm -o checks/check-globals \
	    inter/test-globals.o \
	    inter/global-types.o

checks/check-pool: Makefile \
                   inter/test-pool.o \
                   inter/utils-debug.o \
                   inter/utils-pool.o
	@mkdir -p checks
	@echo "  LD   checks/check-pool"
	@clang -DDEBUG -g -O0 -rdynamic -ldl -lrt -lpthread -lm -o checks/check-pool \
	    inter/test-pool.o \
	    inter/utils-debug.o \
	    inter/utils-pool.o

checks/check-store: Makefile \
                    inter/utils-debug.o \
                    inter/test-store.o \
                    inter/utils-store.o
	@mkdir -p checks
	@echo "  LD   checks/check-store"
	@clang -DDEBUG -g -O0 -rdynamic -ldl -lrt -lpthread -lm -o checks/check-store \
	    inter/utils-debug.o \
	    inter/test-store.o \
	    inter/utils-store.o

checks/check-chain: Makefile \
                    inter/test-chain.o \
                    inter/utils-chain.o
	@mkdir -p checks
	@echo "  LD   checks/check-chain"
	@clang -DDEBUG -g -O0 -rdynamic -ldl -lrt -lpthread -lm -o checks/check-chain \
	    inter/test-chain.o \
	    inter/utils-chain.o

checks/check-list: Makefile \
                   inter/test-list.o \
                   inter/utils-debug.o \
                   inter/utils-chain.o \
                   inter/utils-list.o
	@mkdir -p checks
	@echo "  LD   checks/check-list"
	@clang -DDEBUG -g -O0 -rdynamic -ldl -lrt -lpthread -lm -o checks/check-list \
	    inter/test-list.o \
	    inter/utils-debug.o \
	    inter/utils-chain.o \
	    inter/utils-list.o

checks/check-branch: Makefile \
                     inter/test-branch.o \
                     inter/utils-chain.o \
                     inter/utils-branch.o
	@mkdir -p checks
	@echo "  LD   checks/check-branch"
	@clang -DDEBUG -g -O0 -rdynamic -ldl -lrt -lpthread -lm -o checks/check-branch \
	    inter/test-branch.o \
	    inter/utils-chain.o \
	    inter/utils-branch.o

checks/check-frame: Makefile \
                    inter/test-frame.o \
                    inter/mock-surface-coordinator.o \
                    inter/utils-pool.o \
                    inter/utils-debug.o \
                    inter/utils-store.o \
                    inter/utils-chain.o \
                    inter/utils-branch.o \
                    inter/global-enums.o \
                    inter/global-types.o \
                    inter/exhibitor-frame.o \
                    inter/exhibitor-frame-internal.o
	@mkdir -p checks
	@echo "  LD   checks/check-frame"
	@clang -DDEBUG -g -O0 -rdynamic -ldl -lrt -lpthread -lm -o checks/check-frame \
	    inter/test-frame.o \
	    inter/mock-surface-coordinator.o \
	    inter/utils-pool.o \
	    inter/utils-debug.o \
	    inter/utils-store.o \
	    inter/utils-chain.o \
	    inter/utils-branch.o \
	    inter/global-enums.o \
	    inter/global-types.o \
	    inter/exhibitor-frame.o \
	    inter/exhibitor-frame-internal.o

gen/xdg-shell-server-protocol.h: Makefile \
                                 res/xdg-shell.xml
	@mkdir -p gen
	@echo "  GEN  gen/xdg-shell-server-protocol.h"
	@wayland-scanner server-header res/xdg-shell.xml gen/xdg-shell-server-protocol.h

gen/screenshooter-server-protocol.h: Makefile \
                                     res/screenshooter.xml
	@mkdir -p gen
	@echo "  GEN  gen/screenshooter-server-protocol.h"
	@wayland-scanner server-header res/screenshooter.xml gen/screenshooter-server-protocol.h

gen/screenshooter-client-protocol.h: Makefile \
                                     res/screenshooter.xml
	@mkdir -p gen
	@echo "  GEN  gen/screenshooter-client-protocol.h"
	@wayland-scanner client-header res/screenshooter.xml gen/screenshooter-client-protocol.h

version_info_file: force
	@mkdir -p gen
	@echo "  GEN  version_info_file"
	@python3 -B ./share/build/make.py version

gen/xdg-shell-protocol.c: Makefile \
                          res/xdg-shell.xml
	@mkdir -p gen
	@echo "  GEN  gen/xdg-shell-protocol.c"
	@wayland-scanner code res/xdg-shell.xml gen/xdg-shell-protocol.c

gen/screenshooter-protocol.c: Makefile \
                              res/screenshooter.xml
	@mkdir -p gen
	@echo "  GEN  gen/screenshooter-protocol.c"
	@wayland-scanner code res/screenshooter.xml gen/screenshooter-protocol.c

gen/controller-gtk-res.c: Makefile \
                          res/metanoiactl-gtk-main.ui \
                          res/metanoiactl.gresource.xml
	@mkdir -p gen
	@echo "  GEN  gen/controller-gtk-res.c"
	@glib-compile-resources res/metanoiactl.gresource.xml --target=gen/controller-gtk-res.c --generate-source

inter/global-types.o: Makefile \
                      src/global-enums.h \
                      src/global-macros.h \
                      src/global-types.c \
                      src/global-types.h \
                      src/utils-debug.h
	@mkdir -p inter
	@echo "  CC   inter/global-types.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/global-types.o \
	    src/global-types.c

inter/utils-debug.o: Makefile \
                     src/global-enums.h \
                     src/global-types.h \
                     src/utils-debug.c \
                     src/utils-debug.h
	@mkdir -p inter
	@echo "  CC   inter/utils-debug.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/utils-debug.o \
	    src/utils-debug.c

inter/utils-pool.o: Makefile \
                    src/global-enums.h \
                    src/global-macros.h \
                    src/global-types.h \
                    src/utils-debug.h \
                    src/utils-pool.c \
                    src/utils-pool.h
	@mkdir -p inter
	@echo "  CC   inter/utils-pool.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/utils-pool.o \
	    src/utils-pool.c

inter/utils-store.o: Makefile \
                     src/global-constants.h \
                     src/global-enums.h \
                     src/global-macros.h \
                     src/global-types.h \
                     src/utils-debug.h \
                     src/utils-store.c \
                     src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/utils-store.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/utils-store.o \
	    src/utils-store.c

inter/utils-chain.o: Makefile \
                     src/global-constants.h \
                     src/global-enums.h \
                     src/global-types.h \
                     src/utils-chain.c \
                     src/utils-chain.h
	@mkdir -p inter
	@echo "  CC   inter/utils-chain.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/utils-chain.o \
	    src/utils-chain.c

inter/utils-list.o: Makefile \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-macros.h \
                    src/global-types.h \
                    src/utils-chain.h \
                    src/utils-debug.h \
                    src/utils-list.c \
                    src/utils-list.h
	@mkdir -p inter
	@echo "  CC   inter/utils-list.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/utils-list.o \
	    src/utils-list.c

inter/utils-branch.o: Makefile \
                      src/global-constants.h \
                      src/global-enums.h \
                      src/global-types.h \
                      src/utils-branch.c \
                      src/utils-branch.h \
                      src/utils-chain.h \
                      src/utils-log.h
	@mkdir -p inter
	@echo "  CC   inter/utils-branch.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/utils-branch.o \
	    src/utils-branch.c

inter/global-enums.o: Makefile \
                      src/global-enums.c \
                      src/global-enums.h
	@mkdir -p inter
	@echo "  CC   inter/global-enums.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/global-enums.o \
	    src/global-enums.c

inter/exhibitor-frame.o: Makefile \
                         src/exhibitor-frame-internal.h \
                         src/exhibitor-frame.c \
                         src/exhibitor-frame.h \
                         src/global-constants.h \
                         src/global-enums.h \
                         src/global-macros.h \
                         src/global-types.h \
                         src/surface-coordinator.h \
                         src/surface-data.h \
                         src/utils-branch.h \
                         src/utils-chain.h \
                         src/utils-debug.h \
                         src/utils-pool.h \
                         src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/exhibitor-frame.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/exhibitor-frame.o \
	    src/exhibitor-frame.c

inter/exhibitor-frame-internal.o: Makefile \
                                  src/exhibitor-frame-internal.c \
                                  src/exhibitor-frame-internal.h \
                                  src/exhibitor-frame.h \
                                  src/global-constants.h \
                                  src/global-enums.h \
                                  src/global-macros.h \
                                  src/global-types.h \
                                  src/surface-coordinator.h \
                                  src/surface-data.h \
                                  src/utils-branch.h \
                                  src/utils-chain.h \
                                  src/utils-debug.h \
                                  src/utils-pool.h \
                                  src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/exhibitor-frame-internal.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/exhibitor-frame-internal.o \
	    src/exhibitor-frame-internal.c

inter/config.o: Makefile \
                src/config.c \
                src/config.h \
                src/configuration.h \
                src/global-constants.h \
                src/global-enums.h \
                src/global-functions.h \
                src/global-macros.h \
                src/global-objects.h \
                src/global-types.h \
                src/input-binding.h \
                src/input-bindings.h \
                src/input-context.h \
                src/input-functions.h \
                src/input-mode.h \
                src/utils-chain.h \
                src/utils-debug.h \
                src/utils-keymap.h \
                src/utils-list.h \
                src/utils-log.h \
                src/utils-object.h
	@mkdir -p inter
	@echo "  CC   inter/config.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/config.o \
	    src/config.c

inter/global-objects.o: Makefile \
                        src/global-constants.h \
                        src/global-enums.h \
                        src/global-macros.h \
                        src/global-objects.c \
                        src/global-objects.h \
                        src/global-types.h \
                        src/utils-debug.h \
                        src/utils-log.h \
                        src/utils-object.h
	@mkdir -p inter
	@echo "  CC   inter/global-objects.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/global-objects.o \
	    src/global-objects.c

inter/global-functions.o: Makefile \
                          src/event-loop.h \
                          src/event-signals.h \
                          src/event-task.h \
                          src/exhibitor-module.h \
                          src/global-constants.h \
                          src/global-enums.h \
                          src/global-functions.c \
                          src/global-functions.h \
                          src/global-types.h \
                          src/surface-coordinator.h \
                          src/surface-data.h \
                          src/utils-log.h \
                          src/utils-object.h \
                          src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/global-functions.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/global-functions.o \
	    src/global-functions.c

inter/utils-object.o: Makefile \
                      src/global-constants.h \
                      src/global-enums.h \
                      src/global-types.h \
                      src/utils-object.c \
                      src/utils-object.h
	@mkdir -p inter
	@echo "  CC   inter/utils-object.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/utils-object.o \
	    src/utils-object.c

inter/utils-log.o: Makefile \
                   gen/version.h \
                   src/global-constants.h \
                   src/global-enums.h \
                   src/global-macros.h \
                   src/global-types.h \
                   src/utils-debug.h \
                   src/utils-environment.h \
                   src/utils-log.c \
                   src/utils-log.h \
                   src/utils-time.h
	@mkdir -p inter
	@echo "  CC   inter/utils-log.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/utils-log.o \
	    src/utils-log.c

inter/utils-environment.o: Makefile \
                           src/global-constants.h \
                           src/global-enums.h \
                           src/global-macros.h \
                           src/global-types.h \
                           src/utils-debug.h \
                           src/utils-environment.c \
                           src/utils-environment.h \
                           src/utils-log.h
	@mkdir -p inter
	@echo "  CC   inter/utils-environment.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/utils-environment.o \
	    src/utils-environment.c

inter/utils-dbus.o: Makefile \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-macros.h \
                    src/global-types.h \
                    src/utils-dbus.c \
                    src/utils-dbus.h \
                    src/utils-debug.h \
                    src/utils-log.h
	@mkdir -p inter
	@echo "  CC   inter/utils-dbus.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -o inter/utils-dbus.o \
	    src/utils-dbus.c

inter/utils-gl.o: Makefile \
                  src/global-constants.h \
                  src/global-enums.h \
                  src/global-macros.h \
                  src/global-types.h \
                  src/utils-debug.h \
                  src/utils-gl.c \
                  src/utils-gl.h \
                  src/utils-log.h
	@mkdir -p inter
	@echo "  CC   inter/utils-gl.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -I/usr/include/libdrm -o inter/utils-gl.o \
	    src/utils-gl.c

inter/utils-keymap.o: Makefile \
                      src/global-constants.h \
                      src/global-enums.h \
                      src/global-macros.h \
                      src/global-types.h \
                      src/utils-debug.h \
                      src/utils-environment.h \
                      src/utils-keymap.c \
                      src/utils-keymap.h \
                      src/utils-log.h
	@mkdir -p inter
	@echo "  CC   inter/utils-keymap.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/utils-keymap.o \
	    src/utils-keymap.c

inter/utils-keyboard-state.o: Makefile \
                              src/global-enums.h \
                              src/global-macros.h \
                              src/global-types.h \
                              src/utils-debug.h \
                              src/utils-keyboard-state.c \
                              src/utils-keyboard-state.h
	@mkdir -p inter
	@echo "  CC   inter/utils-keyboard-state.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/utils-keyboard-state.o \
	    src/utils-keyboard-state.c

inter/utils-image.o: Makefile \
                     src/global-constants.h \
                     src/global-enums.h \
                     src/global-macros.h \
                     src/global-types.h \
                     src/utils-debug.h \
                     src/utils-image.c \
                     src/utils-image.h \
                     src/utils-log.h
	@mkdir -p inter
	@echo "  CC   inter/utils-image.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/utils-image.o \
	    src/utils-image.c

inter/utils-time.o: Makefile \
                    src/global-enums.h \
                    src/global-types.h \
                    src/utils-time.c \
                    src/utils-time.h
	@mkdir -p inter
	@echo "  CC   inter/utils-time.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/utils-time.o \
	    src/utils-time.c

inter/event-dispatcher.o: Makefile \
                          src/event-dispatcher.c \
                          src/event-dispatcher.h \
                          src/global-constants.h \
                          src/global-enums.h \
                          src/global-macros.h \
                          src/global-types.h \
                          src/utils-chain.h \
                          src/utils-debug.h \
                          src/utils-list.h \
                          src/utils-log.h
	@mkdir -p inter
	@echo "  CC   inter/event-dispatcher.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/event-dispatcher.o \
	    src/event-dispatcher.c

inter/event-factory.o: Makefile \
                       src/backend-offscreen.h \
                       src/device-evdev.h \
                       src/device-udev.h \
                       src/event-dispatcher.h \
                       src/event-factory.c \
                       src/event-factory.h \
                       src/event-loop.h \
                       src/event-task.h \
                       src/exhibitor-module.h \
                       src/global-constants.h \
                       src/global-enums.h \
                       src/global-macros.h \
                       src/global-types.h \
                       src/output-collector.h \
                       src/output.h \
                       src/renderer.h \
                       src/surface-coordinator.h \
                       src/surface-data.h \
                       src/utils-chain.h \
                       src/utils-debug.h \
                       src/utils-environment.h \
                       src/utils-list.h \
                       src/utils-object.h \
                       src/utils-pool.h \
                       src/utils-store.h \
                       src/wayland.h
	@mkdir -p inter
	@echo "  CC   inter/event-factory.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/event-factory.o \
	    src/event-factory.c

inter/event-loop.o: Makefile \
                    src/event-loop.c \
                    src/event-loop.h \
                    src/event-task.h \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-macros.h \
                    src/global-types.h \
                    src/utils-chain.h \
                    src/utils-debug.h \
                    src/utils-environment.h \
                    src/utils-list.h \
                    src/utils-log.h \
                    src/utils-object.h
	@mkdir -p inter
	@echo "  CC   inter/event-loop.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/event-loop.o \
	    src/event-loop.c

inter/event-signals.o: Makefile \
                       src/event-loop.h \
                       src/event-signals.c \
                       src/event-signals.h \
                       src/event-task.h \
                       src/global-constants.h \
                       src/global-enums.h \
                       src/global-macros.h \
                       src/global-objects.h \
                       src/global-types.h \
                       src/utils-chain.h \
                       src/utils-debug.h \
                       src/utils-list.h \
                       src/utils-log.h \
                       src/utils-object.h
	@mkdir -p inter
	@echo "  CC   inter/event-signals.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/event-signals.o \
	    src/event-signals.c

inter/event-task.o: Makefile \
                    src/event-task.c \
                    src/event-task.h \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-types.h \
                    src/utils-log.h \
                    src/utils-object.h
	@mkdir -p inter
	@echo "  CC   inter/event-task.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/event-task.o \
	    src/event-task.c

inter/event-timer.o: Makefile \
                     src/event-timer.c \
                     src/event-timer.h \
                     src/global-constants.h \
                     src/global-enums.h \
                     src/global-types.h \
                     src/utils-log.h
	@mkdir -p inter
	@echo "  CC   inter/event-timer.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/event-timer.o \
	    src/event-timer.c

inter/surface-coordinator.o: Makefile \
                             src/event-signals.h \
                             src/event-task.h \
                             src/global-constants.h \
                             src/global-enums.h \
                             src/global-macros.h \
                             src/global-types.h \
                             src/surface-coordinator.c \
                             src/surface-coordinator.h \
                             src/surface-data.h \
                             src/utils-debug.h \
                             src/utils-log.h \
                             src/utils-object.h \
                             src/utils-store.h \
                             src/utils-time.h
	@mkdir -p inter
	@echo "  CC   inter/surface-coordinator.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/surface-coordinator.o \
	    src/surface-coordinator.c

inter/surface-data.o: Makefile \
                      src/global-constants.h \
                      src/global-enums.h \
                      src/global-macros.h \
                      src/global-types.h \
                      src/surface-data.c \
                      src/surface-data.h \
                      src/utils-debug.h \
                      src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/surface-data.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/surface-data.o \
	    src/surface-data.c

inter/renderer.o: Makefile \
                  src/global-constants.h \
                  src/global-enums.h \
                  src/global-types.h \
                  src/renderer.c \
                  src/renderer.h \
                  src/surface-coordinator.h \
                  src/surface-data.h \
                  src/utils-pool.h \
                  src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/renderer.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/renderer.o \
	    src/renderer.c

inter/renderer-mmap.o: Makefile \
                       src/global-constants.h \
                       src/global-enums.h \
                       src/global-macros.h \
                       src/global-types.h \
                       src/output.h \
                       src/renderer-mmap.c \
                       src/renderer-mmap.h \
                       src/renderer.h \
                       src/surface-coordinator.h \
                       src/surface-data.h \
                       src/utils-debug.h \
                       src/utils-log.h \
                       src/utils-object.h \
                       src/utils-pool.h \
                       src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/renderer-mmap.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/renderer-mmap.o \
	    src/renderer-mmap.c

inter/renderer-gl.o: Makefile \
                     src/config.h \
                     src/global-constants.h \
                     src/global-enums.h \
                     src/global-macros.h \
                     src/global-types.h \
                     src/renderer-gl.c \
                     src/renderer-gl.h \
                     src/renderer.h \
                     src/surface-coordinator.h \
                     src/surface-data.h \
                     src/utils-chain.h \
                     src/utils-debug.h \
                     src/utils-gl.h \
                     src/utils-keymap.h \
                     src/utils-list.h \
                     src/utils-log.h \
                     src/utils-pool.h \
                     src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/renderer-gl.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -I/usr/include/libdrm -o inter/renderer-gl.o \
	    src/renderer-gl.c

inter/device-common.o: Makefile \
                       src/device-common.c \
                       src/device-common.h \
                       src/global-constants.h \
                       src/global-enums.h \
                       src/global-macros.h \
                       src/global-types.h \
                       src/utils-dbus.h \
                       src/utils-debug.h
	@mkdir -p inter
	@echo "  CC   inter/device-common.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/device-common.o \
	    src/device-common.c

inter/device-fb.o: Makefile \
                   src/device-common.h \
                   src/device-fb.c \
                   src/device-fb.h \
                   src/global-constants.h \
                   src/global-enums.h \
                   src/global-macros.h \
                   src/global-types.h \
                   src/output.h \
                   src/renderer-mmap.h \
                   src/renderer.h \
                   src/surface-coordinator.h \
                   src/surface-data.h \
                   src/utils-chain.h \
                   src/utils-debug.h \
                   src/utils-list.h \
                   src/utils-log.h \
                   src/utils-object.h \
                   src/utils-pool.h \
                   src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/device-fb.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/device-fb.o \
	    src/device-fb.c

inter/device-drm.o: Makefile \
                    src/config.h \
                    src/device-drm.c \
                    src/device-drm.h \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-macros.h \
                    src/global-types.h \
                    src/output.h \
                    src/renderer-gl.h \
                    src/renderer-mmap.h \
                    src/renderer.h \
                    src/surface-coordinator.h \
                    src/surface-data.h \
                    src/utils-chain.h \
                    src/utils-debug.h \
                    src/utils-gl.h \
                    src/utils-keymap.h \
                    src/utils-list.h \
                    src/utils-log.h \
                    src/utils-object.h \
                    src/utils-pool.h \
                    src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/device-drm.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -I/usr/include/libdrm -o inter/device-drm.o \
	    src/device-drm.c

inter/device-evdev.o: Makefile \
                      src/config.h \
                      src/device-common.h \
                      src/device-evdev.c \
                      src/device-evdev.h \
                      src/event-dispatcher.h \
                      src/event-signals.h \
                      src/event-task.h \
                      src/global-constants.h \
                      src/global-enums.h \
                      src/global-macros.h \
                      src/global-objects.h \
                      src/global-types.h \
                      src/input-binding.h \
                      src/input-bindings.h \
                      src/input-context.h \
                      src/utils-chain.h \
                      src/utils-dbus.h \
                      src/utils-debug.h \
                      src/utils-keymap.h \
                      src/utils-list.h \
                      src/utils-log.h \
                      src/utils-object.h
	@mkdir -p inter
	@echo "  CC   inter/device-evdev.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/device-evdev.o \
	    src/device-evdev.c

inter/device-udev.o: Makefile \
                     src/device-udev.c \
                     src/device-udev.h \
                     src/event-dispatcher.h \
                     src/event-signals.h \
                     src/event-task.h \
                     src/global-constants.h \
                     src/global-enums.h \
                     src/global-types.h \
                     src/utils-log.h \
                     src/utils-object.h
	@mkdir -p inter
	@echo "  CC   inter/device-udev.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/device-udev.o \
	    src/device-udev.c

inter/output.o: Makefile \
                src/global-constants.h \
                src/global-enums.h \
                src/global-macros.h \
                src/global-types.h \
                src/output.c \
                src/output.h \
                src/renderer.h \
                src/surface-coordinator.h \
                src/surface-data.h \
                src/utils-debug.h \
                src/utils-object.h \
                src/utils-pool.h \
                src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/output.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/output.o \
	    src/output.c

inter/output-collector.o: Makefile \
                          src/backend-offscreen.h \
                          src/config.h \
                          src/device-drm.h \
                          src/device-fb.h \
                          src/event-loop.h \
                          src/event-signals.h \
                          src/event-task.h \
                          src/global-constants.h \
                          src/global-enums.h \
                          src/global-macros.h \
                          src/global-types.h \
                          src/output-collector.c \
                          src/output-collector.h \
                          src/output.h \
                          src/renderer.h \
                          src/surface-coordinator.h \
                          src/surface-data.h \
                          src/utils-chain.h \
                          src/utils-debug.h \
                          src/utils-keymap.h \
                          src/utils-list.h \
                          src/utils-log.h \
                          src/utils-object.h \
                          src/utils-pool.h \
                          src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/output-collector.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/output-collector.o \
	    src/output-collector.c

inter/input-binding.o: Makefile \
                       src/global-constants.h \
                       src/global-enums.h \
                       src/global-macros.h \
                       src/global-objects.h \
                       src/global-types.h \
                       src/input-binding.c \
                       src/input-binding.h \
                       src/input-context.h \
                       src/utils-chain.h \
                       src/utils-debug.h \
                       src/utils-list.h \
                       src/utils-log.h \
                       src/utils-object.h
	@mkdir -p inter
	@echo "  CC   inter/input-binding.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/input-binding.o \
	    src/input-binding.c

inter/input-bindings.o: Makefile \
                        src/global-constants.h \
                        src/global-enums.h \
                        src/global-macros.h \
                        src/global-objects.h \
                        src/global-types.h \
                        src/input-binding.h \
                        src/input-bindings.c \
                        src/input-bindings.h \
                        src/input-context.h \
                        src/input-mode.h \
                        src/utils-chain.h \
                        src/utils-debug.h \
                        src/utils-list.h \
                        src/utils-log.h \
                        src/utils-object.h
	@mkdir -p inter
	@echo "  CC   inter/input-bindings.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/input-bindings.o \
	    src/input-bindings.c

inter/input-context.o: Makefile \
                       src/global-constants.h \
                       src/global-enums.h \
                       src/global-macros.h \
                       src/global-objects.h \
                       src/global-types.h \
                       src/input-context.c \
                       src/input-context.h \
                       src/utils-chain.h \
                       src/utils-debug.h \
                       src/utils-list.h \
                       src/utils-object.h
	@mkdir -p inter
	@echo "  CC   inter/input-context.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/input-context.o \
	    src/input-context.c

inter/input-functions.o: Makefile \
                         src/config.h \
                         src/event-loop.h \
                         src/event-signals.h \
                         src/event-task.h \
                         src/exhibitor-module.h \
                         src/global-constants.h \
                         src/global-enums.h \
                         src/global-macros.h \
                         src/global-objects.h \
                         src/global-types.h \
                         src/input-binding.h \
                         src/input-bindings.h \
                         src/input-context.h \
                         src/input-functions.c \
                         src/input-functions.h \
                         src/input-mode.h \
                         src/surface-coordinator.h \
                         src/surface-data.h \
                         src/utils-chain.h \
                         src/utils-debug.h \
                         src/utils-keymap.h \
                         src/utils-list.h \
                         src/utils-log.h \
                         src/utils-object.h \
                         src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/input-functions.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/input-functions.o \
	    src/input-functions.c

inter/input-mode.o: Makefile \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-macros.h \
                    src/global-objects.h \
                    src/global-types.h \
                    src/input-binding.h \
                    src/input-context.h \
                    src/input-mode.c \
                    src/input-mode.h \
                    src/utils-chain.h \
                    src/utils-debug.h \
                    src/utils-list.h \
                    src/utils-log.h \
                    src/utils-object.h
	@mkdir -p inter
	@echo "  CC   inter/input-mode.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/input-mode.o \
	    src/input-mode.c

inter/exhibitor-strategist.o: Makefile \
                              src/event-signals.h \
                              src/event-task.h \
                              src/exhibitor-compositor.h \
                              src/exhibitor-frame.h \
                              src/exhibitor-strategist.c \
                              src/exhibitor-strategist.h \
                              src/exhibitor.h \
                              src/global-constants.h \
                              src/global-enums.h \
                              src/global-macros.h \
                              src/global-objects.h \
                              src/global-types.h \
                              src/output.h \
                              src/renderer.h \
                              src/surface-coordinator.h \
                              src/surface-data.h \
                              src/utils-branch.h \
                              src/utils-chain.h \
                              src/utils-debug.h \
                              src/utils-list.h \
                              src/utils-object.h \
                              src/utils-pool.h \
                              src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/exhibitor-strategist.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/exhibitor-strategist.o \
	    src/exhibitor-strategist.c

inter/exhibitor-compositor.o: Makefile \
                              src/config.h \
                              src/event-signals.h \
                              src/event-task.h \
                              src/exhibitor-compositor.c \
                              src/exhibitor-compositor.h \
                              src/exhibitor-frame.h \
                              src/exhibitor.h \
                              src/global-constants.h \
                              src/global-enums.h \
                              src/global-macros.h \
                              src/global-objects.h \
                              src/global-types.h \
                              src/output.h \
                              src/renderer.h \
                              src/surface-coordinator.h \
                              src/surface-data.h \
                              src/utils-branch.h \
                              src/utils-chain.h \
                              src/utils-debug.h \
                              src/utils-keymap.h \
                              src/utils-list.h \
                              src/utils-log.h \
                              src/utils-object.h \
                              src/utils-pool.h \
                              src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/exhibitor-compositor.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/exhibitor-compositor.o \
	    src/exhibitor-compositor.c

inter/exhibitor-display.o: Makefile \
                           src/config.h \
                           src/event-signals.h \
                           src/event-task.h \
                           src/event-timer.h \
                           src/exhibitor-compositor.h \
                           src/exhibitor-display.c \
                           src/exhibitor-display.h \
                           src/exhibitor-frame.h \
                           src/exhibitor-pointer.h \
                           src/exhibitor.h \
                           src/global-constants.h \
                           src/global-enums.h \
                           src/global-macros.h \
                           src/global-objects.h \
                           src/global-types.h \
                           src/output.h \
                           src/renderer.h \
                           src/surface-coordinator.h \
                           src/surface-data.h \
                           src/utils-branch.h \
                           src/utils-chain.h \
                           src/utils-debug.h \
                           src/utils-environment.h \
                           src/utils-image.h \
                           src/utils-keymap.h \
                           src/utils-list.h \
                           src/utils-log.h \
                           src/utils-object.h \
                           src/utils-pool.h \
                           src/utils-store.h \
                           src/utils-time.h
	@mkdir -p inter
	@echo "  CC   inter/exhibitor-display.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/exhibitor-display.o \
	    src/exhibitor-display.c

inter/exhibitor-pointer.o: Makefile \
                           src/event-signals.h \
                           src/event-task.h \
                           src/exhibitor-compositor.h \
                           src/exhibitor-display.h \
                           src/exhibitor-frame.h \
                           src/exhibitor-pointer.c \
                           src/exhibitor-pointer.h \
                           src/exhibitor.h \
                           src/global-constants.h \
                           src/global-enums.h \
                           src/global-macros.h \
                           src/global-objects.h \
                           src/global-types.h \
                           src/output.h \
                           src/renderer.h \
                           src/surface-coordinator.h \
                           src/surface-data.h \
                           src/utils-branch.h \
                           src/utils-chain.h \
                           src/utils-debug.h \
                           src/utils-list.h \
                           src/utils-log.h \
                           src/utils-object.h \
                           src/utils-pool.h \
                           src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/exhibitor-pointer.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/exhibitor-pointer.o \
	    src/exhibitor-pointer.c

inter/exhibitor.o: Makefile \
                   src/exhibitor-compositor.h \
                   src/exhibitor-display.h \
                   src/exhibitor-frame.h \
                   src/exhibitor-pointer.h \
                   src/exhibitor-strategist.h \
                   src/exhibitor.c \
                   src/exhibitor.h \
                   src/global-constants.h \
                   src/global-enums.h \
                   src/global-macros.h \
                   src/global-objects.h \
                   src/global-types.h \
                   src/output.h \
                   src/renderer.h \
                   src/surface-coordinator.h \
                   src/surface-data.h \
                   src/utils-branch.h \
                   src/utils-chain.h \
                   src/utils-debug.h \
                   src/utils-list.h \
                   src/utils-log.h \
                   src/utils-object.h \
                   src/utils-pool.h \
                   src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/exhibitor.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/exhibitor.o \
	    src/exhibitor.c

inter/exhibitor-module.o: Makefile \
                          src/event-loop.h \
                          src/event-signals.h \
                          src/event-task.h \
                          src/exhibitor-module.c \
                          src/exhibitor-module.h \
                          src/exhibitor.h \
                          src/global-constants.h \
                          src/global-enums.h \
                          src/global-macros.h \
                          src/global-objects.h \
                          src/global-types.h \
                          src/output.h \
                          src/renderer.h \
                          src/surface-coordinator.h \
                          src/surface-data.h \
                          src/utils-chain.h \
                          src/utils-debug.h \
                          src/utils-list.h \
                          src/utils-log.h \
                          src/utils-object.h \
                          src/utils-pool.h \
                          src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/exhibitor-module.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/exhibitor-module.o \
	    src/exhibitor-module.c

inter/wayland-region.o: Makefile \
                        src/global-constants.h \
                        src/global-enums.h \
                        src/global-types.h \
                        src/utils-log.h \
                        src/wayland-region.c \
                        src/wayland-region.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-region.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-region.o \
	    src/wayland-region.c

inter/wayland-surface.o: Makefile \
                         src/global-constants.h \
                         src/global-enums.h \
                         src/global-macros.h \
                         src/global-types.h \
                         src/utils-chain.h \
                         src/utils-debug.h \
                         src/utils-list.h \
                         src/utils-log.h \
                         src/wayland-surface.c \
                         src/wayland-surface.h \
                         src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-surface.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-surface.o \
	    src/wayland-surface.c

inter/wayland-output.o: Makefile \
                        src/global-constants.h \
                        src/global-enums.h \
                        src/global-types.h \
                        src/output.h \
                        src/renderer.h \
                        src/surface-coordinator.h \
                        src/surface-data.h \
                        src/utils-log.h \
                        src/utils-object.h \
                        src/utils-pool.h \
                        src/utils-store.h \
                        src/wayland-output.c \
                        src/wayland-output.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-output.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-output.o \
	    src/wayland-output.c

inter/wayland-cache.o: Makefile \
                       src/global-constants.h \
                       src/global-enums.h \
                       src/global-macros.h \
                       src/global-types.h \
                       src/utils-chain.h \
                       src/utils-debug.h \
                       src/utils-list.h \
                       src/utils-log.h \
                       src/utils-store.h \
                       src/wayland-cache.c \
                       src/wayland-cache.h \
                       src/wayland-region.h \
                       src/wayland-surface.h \
                       src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-cache.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-cache.o \
	    src/wayland-cache.c

inter/wayland-state.o: Makefile \
                       gen/xdg-shell-server-protocol.h \
                       src/global-constants.h \
                       src/global-enums.h \
                       src/global-macros.h \
                       src/global-types.h \
                       src/output.h \
                       src/renderer.h \
                       src/surface-coordinator.h \
                       src/surface-data.h \
                       src/utils-chain.h \
                       src/utils-debug.h \
                       src/utils-keyboard-state.h \
                       src/utils-list.h \
                       src/utils-log.h \
                       src/utils-object.h \
                       src/utils-pool.h \
                       src/utils-store.h \
                       src/utils-time.h \
                       src/wayland-cache.h \
                       src/wayland-output.h \
                       src/wayland-protocol-output.h \
                       src/wayland-region.h \
                       src/wayland-state.c \
                       src/wayland-state.h \
                       src/wayland-surface.h \
                       src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-state.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-state.o \
	    src/wayland-state.c

inter/wayland.o: Makefile \
                 gen/screenshooter-server-protocol.h \
                 gen/xdg-shell-server-protocol.h \
                 src/config.h \
                 src/event-loop.h \
                 src/event-signals.h \
                 src/event-task.h \
                 src/global-constants.h \
                 src/global-enums.h \
                 src/global-macros.h \
                 src/global-objects.h \
                 src/global-types.h \
                 src/output.h \
                 src/renderer.h \
                 src/surface-coordinator.h \
                 src/surface-data.h \
                 src/utils-chain.h \
                 src/utils-debug.h \
                 src/utils-environment.h \
                 src/utils-keymap.h \
                 src/utils-list.h \
                 src/utils-log.h \
                 src/utils-object.h \
                 src/utils-pool.h \
                 src/utils-store.h \
                 src/wayland-cache.h \
                 src/wayland-protocol-compositor.h \
                 src/wayland-protocol-device-manager.h \
                 src/wayland-protocol-output.h \
                 src/wayland-protocol-screenshooter.h \
                 src/wayland-protocol-seat.h \
                 src/wayland-protocol-shell.h \
                 src/wayland-protocol-subcompositor.h \
                 src/wayland-protocol-xdg-shell.h \
                 src/wayland-region.h \
                 src/wayland-state.h \
                 src/wayland-surface.h \
                 src/wayland-types.h \
                 src/wayland.c \
                 src/wayland.h
	@mkdir -p inter
	@echo "  CC   inter/wayland.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland.o \
	    src/wayland.c

inter/xdg-shell-protocol.o: Makefile \
                            gen/xdg-shell-protocol.c
	@mkdir -p inter
	@echo "  CC   inter/xdg-shell-protocol.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/xdg-shell-protocol.o \
	    gen/xdg-shell-protocol.c

inter/screenshooter-protocol.o: Makefile \
                                gen/screenshooter-protocol.c
	@mkdir -p inter
	@echo "  CC   inter/screenshooter-protocol.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/screenshooter-protocol.o \
	    gen/screenshooter-protocol.c

inter/wayland-protocol-compositor.o: Makefile \
                                     src/global-constants.h \
                                     src/global-enums.h \
                                     src/global-macros.h \
                                     src/global-types.h \
                                     src/output.h \
                                     src/renderer.h \
                                     src/surface-coordinator.h \
                                     src/surface-data.h \
                                     src/utils-chain.h \
                                     src/utils-debug.h \
                                     src/utils-list.h \
                                     src/utils-log.h \
                                     src/utils-object.h \
                                     src/utils-pool.h \
                                     src/utils-store.h \
                                     src/wayland-cache.h \
                                     src/wayland-protocol-compositor.c \
                                     src/wayland-protocol-compositor.h \
                                     src/wayland-protocol-region.h \
                                     src/wayland-protocol-surface.h \
                                     src/wayland-region.h \
                                     src/wayland-state.h \
                                     src/wayland-surface.h \
                                     src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-compositor.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-compositor.o \
	    src/wayland-protocol-compositor.c

inter/wayland-protocol-data-device.o: Makefile \
                                      src/global-constants.h \
                                      src/global-enums.h \
                                      src/global-macros.h \
                                      src/global-types.h \
                                      src/utils-debug.h \
                                      src/utils-log.h \
                                      src/wayland-protocol-data-device.c \
                                      src/wayland-protocol-data-device.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-data-device.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-data-device.o \
	    src/wayland-protocol-data-device.c

inter/wayland-protocol-data-source.o: Makefile \
                                      src/global-constants.h \
                                      src/global-enums.h \
                                      src/global-macros.h \
                                      src/global-types.h \
                                      src/utils-debug.h \
                                      src/utils-log.h \
                                      src/wayland-protocol-data-source.c \
                                      src/wayland-protocol-data-source.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-data-source.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-data-source.o \
	    src/wayland-protocol-data-source.c

inter/wayland-protocol-device-manager.o: Makefile \
                                         src/global-constants.h \
                                         src/global-enums.h \
                                         src/global-macros.h \
                                         src/global-types.h \
                                         src/utils-chain.h \
                                         src/utils-debug.h \
                                         src/utils-list.h \
                                         src/utils-log.h \
                                         src/utils-store.h \
                                         src/wayland-cache.h \
                                         src/wayland-protocol-data-device.h \
                                         src/wayland-protocol-data-source.h \
                                         src/wayland-protocol-device-manager.c \
                                         src/wayland-protocol-device-manager.h \
                                         src/wayland-region.h \
                                         src/wayland-surface.h \
                                         src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-device-manager.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-device-manager.o \
	    src/wayland-protocol-device-manager.c

inter/wayland-protocol-keyboard.o: Makefile \
                                   src/config.h \
                                   src/global-constants.h \
                                   src/global-enums.h \
                                   src/global-macros.h \
                                   src/global-types.h \
                                   src/output.h \
                                   src/renderer.h \
                                   src/surface-coordinator.h \
                                   src/surface-data.h \
                                   src/utils-chain.h \
                                   src/utils-debug.h \
                                   src/utils-keymap.h \
                                   src/utils-list.h \
                                   src/utils-log.h \
                                   src/utils-object.h \
                                   src/utils-pool.h \
                                   src/utils-store.h \
                                   src/wayland-cache.h \
                                   src/wayland-protocol-keyboard.c \
                                   src/wayland-protocol-keyboard.h \
                                   src/wayland-region.h \
                                   src/wayland-state.h \
                                   src/wayland-surface.h \
                                   src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-keyboard.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-keyboard.o \
	    src/wayland-protocol-keyboard.c

inter/wayland-protocol-output.o: Makefile \
                                 src/global-constants.h \
                                 src/global-enums.h \
                                 src/global-macros.h \
                                 src/global-types.h \
                                 src/output.h \
                                 src/renderer.h \
                                 src/surface-coordinator.h \
                                 src/surface-data.h \
                                 src/utils-chain.h \
                                 src/utils-debug.h \
                                 src/utils-list.h \
                                 src/utils-log.h \
                                 src/utils-object.h \
                                 src/utils-pool.h \
                                 src/utils-store.h \
                                 src/wayland-cache.h \
                                 src/wayland-protocol-output.c \
                                 src/wayland-protocol-output.h \
                                 src/wayland-region.h \
                                 src/wayland-surface.h \
                                 src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-output.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-output.o \
	    src/wayland-protocol-output.c

inter/wayland-protocol-pointer.o: Makefile \
                                  src/global-constants.h \
                                  src/global-enums.h \
                                  src/global-macros.h \
                                  src/global-types.h \
                                  src/output.h \
                                  src/renderer.h \
                                  src/surface-coordinator.h \
                                  src/surface-data.h \
                                  src/utils-chain.h \
                                  src/utils-debug.h \
                                  src/utils-list.h \
                                  src/utils-log.h \
                                  src/utils-object.h \
                                  src/utils-pool.h \
                                  src/utils-store.h \
                                  src/wayland-cache.h \
                                  src/wayland-protocol-keyboard.h \
                                  src/wayland-protocol-pointer.c \
                                  src/wayland-region.h \
                                  src/wayland-state.h \
                                  src/wayland-surface.h \
                                  src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-pointer.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-pointer.o \
	    src/wayland-protocol-pointer.c

inter/wayland-protocol-region.o: Makefile \
                                 src/global-constants.h \
                                 src/global-enums.h \
                                 src/global-macros.h \
                                 src/global-types.h \
                                 src/utils-chain.h \
                                 src/utils-debug.h \
                                 src/utils-list.h \
                                 src/utils-log.h \
                                 src/utils-store.h \
                                 src/wayland-cache.h \
                                 src/wayland-protocol-region.c \
                                 src/wayland-protocol-region.h \
                                 src/wayland-region.h \
                                 src/wayland-surface.h \
                                 src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-region.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-region.o \
	    src/wayland-protocol-region.c

inter/wayland-protocol-screenshooter.o: Makefile \
                                        gen/screenshooter-server-protocol.h \
                                        src/global-constants.h \
                                        src/global-enums.h \
                                        src/global-macros.h \
                                        src/global-types.h \
                                        src/output.h \
                                        src/renderer.h \
                                        src/surface-coordinator.h \
                                        src/surface-data.h \
                                        src/utils-chain.h \
                                        src/utils-debug.h \
                                        src/utils-list.h \
                                        src/utils-log.h \
                                        src/utils-object.h \
                                        src/utils-pool.h \
                                        src/utils-store.h \
                                        src/wayland-cache.h \
                                        src/wayland-protocol-screenshooter.c \
                                        src/wayland-protocol-screenshooter.h \
                                        src/wayland-region.h \
                                        src/wayland-surface.h \
                                        src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-screenshooter.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-screenshooter.o \
	    src/wayland-protocol-screenshooter.c

inter/wayland-protocol-seat.o: Makefile \
                               src/config.h \
                               src/global-constants.h \
                               src/global-enums.h \
                               src/global-macros.h \
                               src/global-types.h \
                               src/output.h \
                               src/renderer.h \
                               src/surface-coordinator.h \
                               src/surface-data.h \
                               src/utils-chain.h \
                               src/utils-debug.h \
                               src/utils-keymap.h \
                               src/utils-list.h \
                               src/utils-log.h \
                               src/utils-object.h \
                               src/utils-pool.h \
                               src/utils-store.h \
                               src/wayland-cache.h \
                               src/wayland-protocol-keyboard.h \
                               src/wayland-protocol-pointer.h \
                               src/wayland-protocol-seat.c \
                               src/wayland-protocol-seat.h \
                               src/wayland-region.h \
                               src/wayland-state.h \
                               src/wayland-surface.h \
                               src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-seat.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-seat.o \
	    src/wayland-protocol-seat.c

inter/wayland-protocol-shell-surface.o: Makefile \
                                        src/global-constants.h \
                                        src/global-enums.h \
                                        src/global-macros.h \
                                        src/global-types.h \
                                        src/output.h \
                                        src/renderer.h \
                                        src/surface-coordinator.h \
                                        src/surface-data.h \
                                        src/utils-chain.h \
                                        src/utils-debug.h \
                                        src/utils-list.h \
                                        src/utils-log.h \
                                        src/utils-object.h \
                                        src/utils-pool.h \
                                        src/utils-store.h \
                                        src/wayland-cache.h \
                                        src/wayland-protocol-shell-surface.c \
                                        src/wayland-protocol-shell-surface.h \
                                        src/wayland-region.h \
                                        src/wayland-state.h \
                                        src/wayland-surface.h \
                                        src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-shell-surface.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-shell-surface.o \
	    src/wayland-protocol-shell-surface.c

inter/wayland-protocol-shell.o: Makefile \
                                src/global-constants.h \
                                src/global-enums.h \
                                src/global-macros.h \
                                src/global-types.h \
                                src/utils-debug.h \
                                src/utils-log.h \
                                src/wayland-protocol-shell-surface.h \
                                src/wayland-protocol-shell.c \
                                src/wayland-protocol-shell.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-shell.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-shell.o \
	    src/wayland-protocol-shell.c

inter/wayland-protocol-subcompositor.o: Makefile \
                                        src/global-constants.h \
                                        src/global-enums.h \
                                        src/global-macros.h \
                                        src/global-types.h \
                                        src/utils-chain.h \
                                        src/utils-debug.h \
                                        src/utils-list.h \
                                        src/utils-log.h \
                                        src/utils-store.h \
                                        src/wayland-cache.h \
                                        src/wayland-protocol-subcompositor.c \
                                        src/wayland-protocol-subcompositor.h \
                                        src/wayland-protocol-subsurface.h \
                                        src/wayland-region.h \
                                        src/wayland-surface.h \
                                        src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-subcompositor.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-subcompositor.o \
	    src/wayland-protocol-subcompositor.c

inter/wayland-protocol-subsurface.o: Makefile \
                                     src/global-constants.h \
                                     src/global-enums.h \
                                     src/global-macros.h \
                                     src/global-types.h \
                                     src/utils-debug.h \
                                     src/utils-log.h \
                                     src/wayland-protocol-subsurface.c \
                                     src/wayland-protocol-subsurface.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-subsurface.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-subsurface.o \
	    src/wayland-protocol-subsurface.c

inter/wayland-protocol-surface.o: Makefile \
                                  src/global-constants.h \
                                  src/global-enums.h \
                                  src/global-macros.h \
                                  src/global-types.h \
                                  src/output.h \
                                  src/renderer.h \
                                  src/surface-coordinator.h \
                                  src/surface-data.h \
                                  src/utils-chain.h \
                                  src/utils-debug.h \
                                  src/utils-list.h \
                                  src/utils-log.h \
                                  src/utils-object.h \
                                  src/utils-pool.h \
                                  src/utils-store.h \
                                  src/wayland-cache.h \
                                  src/wayland-protocol-surface.c \
                                  src/wayland-protocol-surface.h \
                                  src/wayland-region.h \
                                  src/wayland-state.h \
                                  src/wayland-surface.h \
                                  src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-surface.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-surface.o \
	    src/wayland-protocol-surface.c

inter/wayland-protocol-xdg-shell.o: Makefile \
                                    gen/xdg-shell-server-protocol.h \
                                    src/global-constants.h \
                                    src/global-enums.h \
                                    src/global-macros.h \
                                    src/global-types.h \
                                    src/utils-chain.h \
                                    src/utils-debug.h \
                                    src/utils-list.h \
                                    src/utils-log.h \
                                    src/utils-store.h \
                                    src/wayland-cache.h \
                                    src/wayland-protocol-xdg-shell.c \
                                    src/wayland-protocol-xdg-shell.h \
                                    src/wayland-protocol-xdg-surface.h \
                                    src/wayland-region.h \
                                    src/wayland-surface.h \
                                    src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-xdg-shell.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-xdg-shell.o \
	    src/wayland-protocol-xdg-shell.c

inter/wayland-protocol-xdg-surface.o: Makefile \
                                      gen/xdg-shell-server-protocol.h \
                                      src/global-constants.h \
                                      src/global-enums.h \
                                      src/global-macros.h \
                                      src/global-types.h \
                                      src/output.h \
                                      src/renderer.h \
                                      src/surface-coordinator.h \
                                      src/surface-data.h \
                                      src/utils-chain.h \
                                      src/utils-debug.h \
                                      src/utils-list.h \
                                      src/utils-log.h \
                                      src/utils-object.h \
                                      src/utils-pool.h \
                                      src/utils-store.h \
                                      src/wayland-cache.h \
                                      src/wayland-protocol-xdg-surface.c \
                                      src/wayland-protocol-xdg-surface.h \
                                      src/wayland-region.h \
                                      src/wayland-state.h \
                                      src/wayland-surface.h \
                                      src/wayland-types.h
	@mkdir -p inter
	@echo "  CC   inter/wayland-protocol-xdg-surface.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/wayland-protocol-xdg-surface.o \
	    src/wayland-protocol-xdg-surface.c

inter/backend-offscreen.o: Makefile \
                           src/backend-offscreen.c \
                           src/backend-offscreen.h \
                           src/event-loop.h \
                           src/event-signals.h \
                           src/event-task.h \
                           src/global-constants.h \
                           src/global-enums.h \
                           src/global-macros.h \
                           src/global-types.h \
                           src/output.h \
                           src/renderer-gl.h \
                           src/renderer-mmap.h \
                           src/renderer.h \
                           src/surface-coordinator.h \
                           src/surface-data.h \
                           src/utils-chain.h \
                           src/utils-debug.h \
                           src/utils-gl.h \
                           src/utils-list.h \
                           src/utils-log.h \
                           src/utils-object.h \
                           src/utils-pool.h \
                           src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/backend-offscreen.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/backend-offscreen.o \
	    src/backend-offscreen.c

inter/metanoia.o: Makefile \
                  src/config.h \
                  src/event-dispatcher.h \
                  src/event-factory.h \
                  src/event-loop.h \
                  src/event-signals.h \
                  src/event-task.h \
                  src/global-constants.h \
                  src/global-enums.h \
                  src/global-macros.h \
                  src/global-types.h \
                  src/metanoia.c \
                  src/surface-coordinator.h \
                  src/surface-data.h \
                  src/utils-chain.h \
                  src/utils-dbus.h \
                  src/utils-debug.h \
                  src/utils-environment.h \
                  src/utils-keymap.h \
                  src/utils-list.h \
                  src/utils-log.h \
                  src/utils-object.h \
                  src/utils-store.h
	@mkdir -p inter
	@echo "  CC   inter/metanoia.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -o inter/metanoia.o \
	    src/metanoia.c

inter/controller-defs.o: Makefile \
                         src/controller-defs.c \
                         src/controller-defs.h
	@mkdir -p inter
	@echo "  CC   inter/controller-defs.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -o inter/controller-defs.o \
	    src/controller-defs.c

inter/controller-output.o: Makefile \
                           src/controller-defs.h \
                           src/controller-output.c \
                           src/controller-output.h
	@mkdir -p inter
	@echo "  CC   inter/controller-output.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -o inter/controller-output.o \
	    src/controller-output.c

inter/controller-gtk-res.o: Makefile \
                            gen/controller-gtk-res.c
	@mkdir -p inter
	@echo "  CC   inter/controller-gtk-res.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -o inter/controller-gtk-res.o \
	    gen/controller-gtk-res.c

inter/controller-gtk-display.o: Makefile \
                                src/controller-defs.h \
                                src/controller-gtk-display.c \
                                src/controller-gtk-display.h \
                                src/controller-output.h \
                                src/global-constants.h \
                                src/global-enums.h \
                                src/global-types.h \
                                src/utils-log.h
	@mkdir -p inter
	@echo "  CC   inter/controller-gtk-display.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -I/usr/include/at-spi-2.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/dbus-1.0 -I/usr/include/freetype2 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/glib-2.0 -I/usr/include/gtk-3.0 -I/usr/include/harfbuzz -I/usr/include/libdrm -I/usr/include/libpng16 -I/usr/include/pango-1.0 -I/usr/include/pixman-1 -I/usr/lib/dbus-1.0/include -I/usr/lib/glib-2.0/include -pthread -o inter/controller-gtk-display.o \
	    src/controller-gtk-display.c

inter/controller-gtk-win.o: Makefile \
                            gen/version.h \
                            src/controller-gtk-app.h \
                            src/controller-gtk-display.h \
                            src/controller-gtk-win.c \
                            src/controller-gtk-win.h \
                            src/controller-output.h
	@mkdir -p inter
	@echo "  CC   inter/controller-gtk-win.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -I/usr/include/at-spi-2.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/dbus-1.0 -I/usr/include/freetype2 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/glib-2.0 -I/usr/include/gtk-3.0 -I/usr/include/harfbuzz -I/usr/include/libdrm -I/usr/include/libpng16 -I/usr/include/pango-1.0 -I/usr/include/pixman-1 -I/usr/lib/dbus-1.0/include -I/usr/lib/glib-2.0/include -pthread -o inter/controller-gtk-win.o \
	    src/controller-gtk-win.c

inter/controller-gtk-app.o: Makefile \
                            src/controller-bind.h \
                            src/controller-defs.h \
                            src/controller-gtk-app.c \
                            src/controller-gtk-app.h \
                            src/controller-gtk-win.h \
                            src/controller-output.h
	@mkdir -p inter
	@echo "  CC   inter/controller-gtk-app.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -I/usr/include/at-spi-2.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/dbus-1.0 -I/usr/include/freetype2 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/glib-2.0 -I/usr/include/gtk-3.0 -I/usr/include/harfbuzz -I/usr/include/libdrm -I/usr/include/libpng16 -I/usr/include/pango-1.0 -I/usr/include/pixman-1 -I/usr/lib/dbus-1.0/include -I/usr/lib/glib-2.0/include -pthread -o inter/controller-gtk-app.o \
	    src/controller-gtk-app.c

inter/controller-wayland.o: Makefile \
                            gen/screenshooter-client-protocol.h \
                            src/controller-bind.h \
                            src/controller-defs.h \
                            src/controller-output.h \
                            src/controller-wayland.c \
                            src/controller-wayland.h \
                            src/global-constants.h \
                            src/global-enums.h \
                            src/global-types.h \
                            src/utils-environment.h \
                            src/utils-log.h
	@mkdir -p inter
	@echo "  CC   inter/controller-wayland.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -o inter/controller-wayland.o \
	    src/controller-wayland.c

inter/metanoiactl-gtk.o: Makefile \
                         src/controller-gtk-app.h \
                         src/global-constants.h \
                         src/global-enums.h \
                         src/global-types.h \
                         src/metanoiactl-gtk.c \
                         src/utils-environment.h
	@mkdir -p inter
	@echo "  CC   inter/metanoiactl-gtk.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Igen -I/usr/include/at-spi-2.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/dbus-1.0 -I/usr/include/freetype2 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/glib-2.0 -I/usr/include/gtk-3.0 -I/usr/include/harfbuzz -I/usr/include/libdrm -I/usr/include/libpng16 -I/usr/include/pango-1.0 -I/usr/include/pixman-1 -I/usr/lib/dbus-1.0/include -I/usr/lib/glib-2.0/include -pthread -o inter/metanoiactl-gtk.o \
	    src/metanoiactl-gtk.c

inter/test-globals.o: Makefile \
                      src/global-enums.h \
                      src/global-types.h \
                      tests/test-globals.c \
                      tests/tests-suit.h
	@mkdir -p inter
	@echo "  CC   inter/test-globals.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Itests -o inter/test-globals.o \
	    tests/test-globals.c

inter/test-pool.o: Makefile \
                   src/utils-pool.h \
                   tests/test-pool.c \
                   tests/tests-suit.h
	@mkdir -p inter
	@echo "  CC   inter/test-pool.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Itests -o inter/test-pool.o \
	    tests/test-pool.c

inter/test-store.o: Makefile \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-types.h \
                    src/utils-store.h \
                    tests/test-store.c \
                    tests/tests-suit.h
	@mkdir -p inter
	@echo "  CC   inter/test-store.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Itests -o inter/test-store.o \
	    tests/test-store.c

inter/test-chain.o: Makefile \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-types.h \
                    src/utils-chain.h \
                    tests/test-chain.c \
                    tests/tests-suit.h
	@mkdir -p inter
	@echo "  CC   inter/test-chain.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Itests -o inter/test-chain.o \
	    tests/test-chain.c

inter/test-list.o: Makefile \
                   src/global-constants.h \
                   src/global-enums.h \
                   src/global-macros.h \
                   src/global-types.h \
                   src/utils-chain.h \
                   src/utils-debug.h \
                   src/utils-list.h \
                   tests/test-list.c \
                   tests/tests-suit.h
	@mkdir -p inter
	@echo "  CC   inter/test-list.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Itests -o inter/test-list.o \
	    tests/test-list.c

inter/test-branch.o: Makefile \
                     src/global-constants.h \
                     src/global-enums.h \
                     src/global-types.h \
                     src/utils-branch.h \
                     src/utils-chain.h \
                     tests/test-branch.c \
                     tests/tests-suit.h
	@mkdir -p inter
	@echo "  CC   inter/test-branch.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Itests -o inter/test-branch.o \
	    tests/test-branch.c

inter/test-frame.o: Makefile \
                    src/exhibitor-frame-internal.h \
                    src/exhibitor-frame.h \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-macros.h \
                    src/global-types.h \
                    src/surface-coordinator.h \
                    src/surface-data.h \
                    src/utils-branch.h \
                    src/utils-chain.h \
                    src/utils-debug.h \
                    src/utils-pool.h \
                    src/utils-store.h \
                    tests/mock-surface-coordinator.h \
                    tests/test-frame.c \
                    tests/tests-suit.h
	@mkdir -p inter
	@echo "  CC   inter/test-frame.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Itests -o inter/test-frame.o \
	    tests/test-frame.c

inter/mock-surface-coordinator.o: Makefile \
                                  src/global-constants.h \
                                  src/global-enums.h \
                                  src/global-macros.h \
                                  src/global-types.h \
                                  src/surface-coordinator.h \
                                  src/surface-data.h \
                                  src/utils-debug.h \
                                  src/utils-store.h \
                                  tests/mock-surface-coordinator.c \
                                  tests/mock-surface-coordinator.h
	@mkdir -p inter
	@echo "  CC   inter/mock-surface-coordinator.o"
	@clang -DDEBUG -g -O0 -Wall -W -Wextra -Werror -Wpedantic -std=gnu11 -c -Isrc -Itests -o inter/mock-surface-coordinator.o \
	    tests/mock-surface-coordinator.c

