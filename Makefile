all: build/metanoia build/metanoiactl-gtk

res/force:

clean:
	rm -rf doc build gen checks callgrind*

checks: checks/check-pool checks/check-chain checks/check-list checks/check-branch checks/check-store checks/check-frame
check: checks
	@time (echo; for c in checks/check*; do $$c; done)
memcheck: checks
	@time (echo; for c in checks/check*; do valgrind --leak-check=full --show-leak-kinds=all --log-file=valgrind.log $$c -q; if ! cat valgrind.log | grep "All heap blocks were freed -- no leaks are possible"; then cat valgrind.log; fi; rm -f valgrind.log; echo; done)
cppcheck:
	cppcheck -q --enable=all --template="[{severity}] {file} ({line}): {id} - {message}" --suppress=incorrectStringBooleanError .


build/metanoia: Makefile \
                build/backend-offscreen.o \
                build/bind-egl-wayland.o \
                build/config.o \
                build/device-common.o \
                build/device-drm.o \
                build/device-evdev.o \
                build/device-fb.o \
                build/device-udev.o \
                build/event-dispatcher.o \
                build/event-factory.o \
                build/event-loop.o \
                build/event-signals.o \
                build/event-task.o \
                build/event-timer.o \
                build/exhibitor-compositor.o \
                build/exhibitor-display.o \
                build/exhibitor-frame.o \
                build/exhibitor-pointer.o \
                build/exhibitor-strategist.o \
                build/exhibitor.o \
                build/global-enums.o \
                build/global-functions.o \
                build/global-objects.o \
                build/global-types.o \
                build/keyboard-binding.o \
                build/keyboard-bindings.o \
                build/keyboard-functions.o \
                build/keyboard-mode.o \
                build/metanoia.o \
                build/output-collector.o \
                build/output.o \
                build/renderer-gl.o \
                build/renderer-mmap.o \
                build/renderer.o \
                build/surface-data.o \
                build/surface-manager.o \
                build/utils-branch.o \
                build/utils-chain.o \
                build/utils-dbus.o \
                build/utils-environment.o \
                build/utils-gl.o \
                build/utils-keymap.o \
                build/utils-list.o \
                build/utils-log.o \
                build/utils-object.o \
                build/utils-pool.o \
                build/utils-store.o \
                build/wayland-cache.o \
                build/wayland-output.o \
                build/wayland-protocol-compositor.o \
                build/wayland-protocol-keyboard.o \
                build/wayland-protocol-output.o \
                build/wayland-protocol-pointer.o \
                build/wayland-protocol-region.o \
                build/wayland-protocol-screenshooter.o \
                build/wayland-protocol-seat.o \
                build/wayland-protocol-shell-surface.o \
                build/wayland-protocol-shell.o \
                build/wayland-protocol-surface.o \
                build/wayland-protocol-xdg-shell.o \
                build/wayland-protocol-xdg-surface.o \
                build/wayland-region.o \
                build/wayland-state.o \
                build/wayland-surface.o \
                build/wayland.o \
                build/xdg-shell-protocol.o \
                gen/screenshooter-protocol.c
	@mkdir -p build
	@echo "  LD   metanoia"
	@gcc -rdynamic -ldl -lrt -lpthread -lm -DDEBUG -g -O0 -o build/metanoia \
	       build/config.o build/global-enums.o build/global-types.o build/global-objects.o build/global-functions.o build/utils-object.o build/utils-pool.o build/utils-chain.o build/utils-list.o build/utils-branch.o build/utils-store.o build/utils-dbus.o build/utils-gl.o build/utils-keymap.o build/utils-log.o build/utils-environment.o build/event-dispatcher.o build/event-timer.o build/event-signals.o build/event-loop.o build/event-task.o build/event-factory.o build/renderer.o build/renderer-mmap.o build/renderer-gl.o build/device-common.o build/device-fb.o build/device-drm.o build/device-evdev.o build/device-udev.o build/output.o build/output-collector.o build/surface-data.o build/surface-manager.o build/keyboard-functions.o build/keyboard-binding.o build/keyboard-bindings.o build/keyboard-mode.o build/exhibitor.o build/exhibitor-display.o build/exhibitor-compositor.o build/exhibitor-frame.o build/exhibitor-strategist.o build/exhibitor-pointer.o build/wayland-region.o build/wayland-surface.o build/wayland-output.o build/wayland-cache.o build/wayland-state.o build/wayland.o build/wayland-protocol-compositor.o build/wayland-protocol-surface.o build/wayland-protocol-region.o build/wayland-protocol-shell.o build/wayland-protocol-shell-surface.o build/wayland-protocol-xdg-shell.o build/wayland-protocol-xdg-surface.o build/xdg-shell-protocol.o build/wayland-protocol-output.o build/wayland-protocol-seat.o build/wayland-protocol-pointer.o build/wayland-protocol-keyboard.o build/wayland-protocol-screenshooter.o gen/screenshooter-protocol.c build/bind-egl-wayland.o build/backend-offscreen.o build/metanoia.o \
	       -ldbus-1 -lEGL -lgbm -lGL -ldrm -ludev -lwayland-server -lxkbcommon

build/metanoiactl-gtk: Makefile \
                       build/controller-defs.o \
                       build/controller-gtk-app.o \
                       build/controller-gtk-display.o \
                       build/controller-gtk-res.o \
                       build/controller-gtk-win.o \
                       build/controller-output.o \
                       build/controller-wayland.o \
                       build/metanoiactl-gtk.o \
                       build/screenshooter-protocol.o \
                       build/utils-environment.o \
                       build/utils-log.o
	@mkdir -p build
	@echo "  LD   metanoiactl-gtk"
	@gcc -rdynamic -ldl -lrt -lpthread -lm -DDEBUG -g -O0 -o build/metanoiactl-gtk \
	       build/controller-defs.o build/controller-output.o build/controller-gtk-res.o build/controller-gtk-display.o build/controller-gtk-win.o build/controller-gtk-app.o build/controller-wayland.o build/metanoiactl-gtk.o build/utils-log.o build/utils-environment.o build/screenshooter-protocol.o \
	       -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lwayland-client

gen/xdg-shell-server-protocol.h: Makefile \
                                 res/xdg-shell.xml
	@mkdir -p gen
	@echo "  GEN  xdg-shell-server-protocol.h"
	@wayland-scanner server-header "res/xdg-shell.xml" "gen/xdg-shell-server-protocol.h"

gen/xdg-shell-protocol.c: Makefile \
                          res/xdg-shell.xml
	@mkdir -p gen
	@echo "  GEN  xdg-shell-protocol.c"
	@wayland-scanner code "res/xdg-shell.xml" "gen/xdg-shell-protocol.c"

gen/screenshooter-server-protocol.h: Makefile \
                                     res/screenshooter.xml
	@mkdir -p gen
	@echo "  GEN  screenshooter-server-protocol.h"
	@wayland-scanner server-header "res/screenshooter.xml" "gen/screenshooter-server-protocol.h"

gen/screenshooter-client-protocol.h: Makefile \
                                     res/screenshooter.xml
	@mkdir -p gen
	@echo "  GEN  screenshooter-client-protocol.h"
	@wayland-scanner client-header "res/screenshooter.xml" "gen/screenshooter-client-protocol.h"

gen/screenshooter-protocol.c: Makefile \
                              res/screenshooter.xml
	@mkdir -p gen
	@echo "  GEN  screenshooter-protocol.c"
	@wayland-scanner code "res/screenshooter.xml" "gen/screenshooter-protocol.c"

gen/backend-gtk-res.c: Makefile \
                       res/backend-gtk-area.ui \
                       res/backend-gtk-main.ui \
                       res/backend-gtk-menu.ui \
                       res/metanoia.gresource.xml
	@mkdir -p gen
	@echo "  GEN  backend-gtk-res.c"
	@glib-compile-resources res/metanoia.gresource.xml --target=gen/backend-gtk-res.c --generate-source

gen/controller-gtk-res.c: Makefile \
                          res/metanoiactl-gtk-main.ui \
                          res/metanoiactl.gresource.xml
	@mkdir -p gen
	@echo "  GEN  controller-gtk-res.c"
	@glib-compile-resources res/metanoiactl.gresource.xml --target=gen/controller-gtk-res.c --generate-source

gen/version.h: Makefile \
               res/force
	@mkdir -p gen
	@echo "  GEN  version.h"
	@desc=`git describe --always`; if ! grep -q "$$desc" gen/version.h 2> /dev/null; then echo -e "#ifndef NOIA_VERSION\n#define NOIA_VERSION \"$$desc\"\n#endif\n" > gen/version.h; fi

build/screenshooter-protocol.o: Makefile \
                                gen/screenshooter-protocol.c
	@mkdir -p build
	@echo "  CC   screenshooter-protocol.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/screenshooter-protocol.o -Isrc -Igen \
	       -c gen/screenshooter-protocol.c

build/config.o: Makefile \
                src/config.c \
                src/config.h \
                src/configuration.h \
                src/global-constants.h \
                src/global-enums.h \
                src/global-functions.h \
                src/global-macros.h \
                src/global-types.h \
                src/keyboard-binding.h \
                src/keyboard-bindings.h \
                src/keyboard-functions.h \
                src/utils-keymap.h \
                src/utils-log.h \
                src/utils-pool.h
	@mkdir -p build
	@echo "  CC   config.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/config.o -Isrc -Igen \
	       -c src/config.c

build/global-enums.o: Makefile \
                      src/global-enums.c \
                      src/global-enums.h
	@mkdir -p build
	@echo "  CC   global-enums.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/global-enums.o -Isrc -Igen \
	       -c src/global-enums.c

build/global-types.o: Makefile \
                      src/global-enums.h \
                      src/global-macros.h \
                      src/global-types.c \
                      src/global-types.h
	@mkdir -p build
	@echo "  CC   global-types.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/global-types.o -Isrc -Igen \
	       -c src/global-types.c

build/global-objects.o: Makefile \
                        src/global-constants.h \
                        src/global-enums.h \
                        src/global-objects.c \
                        src/global-objects.h \
                        src/global-types.h \
                        src/utils-log.h \
                        src/utils-object.h
	@mkdir -p build
	@echo "  CC   global-objects.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/global-objects.o -Isrc -Igen \
	       -c src/global-objects.c

build/global-functions.o: Makefile \
                          src/event-loop.h \
                          src/event-signals.h \
                          src/event-task.h \
                          src/exhibitor-compositor.h \
                          src/exhibitor-display.h \
                          src/exhibitor-frame.h \
                          src/exhibitor.h \
                          src/global-constants.h \
                          src/global-enums.h \
                          src/global-functions.c \
                          src/global-functions.h \
                          src/global-macros.h \
                          src/global-types.h \
                          src/output.h \
                          src/renderer.h \
                          src/utils-branch.h \
                          src/utils-chain.h \
                          src/utils-list.h \
                          src/utils-log.h \
                          src/utils-object.h \
                          src/utils-pool.h
	@mkdir -p build
	@echo "  CC   global-functions.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/global-functions.o -Isrc -Igen \
	       -c src/global-functions.c

build/utils-object.o: Makefile \
                      src/global-constants.h \
                      src/global-enums.h \
                      src/global-types.h \
                      src/utils-object.c \
                      src/utils-object.h
	@mkdir -p build
	@echo "  CC   utils-object.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/utils-object.o -Isrc -Igen \
	       -c src/utils-object.c

build/utils-pool.o: Makefile \
                    src/global-enums.h \
                    src/global-macros.h \
                    src/global-types.h \
                    src/utils-pool.c \
                    src/utils-pool.h
	@mkdir -p build
	@echo "  CC   utils-pool.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/utils-pool.o -Isrc -Igen \
	       -c src/utils-pool.c

build/utils-chain.o: Makefile \
                     src/global-constants.h \
                     src/global-enums.h \
                     src/global-types.h \
                     src/utils-chain.c \
                     src/utils-chain.h
	@mkdir -p build
	@echo "  CC   utils-chain.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/utils-chain.o -Isrc -Igen \
	       -c src/utils-chain.c

build/utils-list.o: Makefile \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-macros.h \
                    src/global-types.h \
                    src/utils-chain.h \
                    src/utils-list.c \
                    src/utils-list.h
	@mkdir -p build
	@echo "  CC   utils-list.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/utils-list.o -Isrc -Igen \
	       -c src/utils-list.c

build/utils-branch.o: Makefile \
                      src/global-constants.h \
                      src/global-enums.h \
                      src/global-types.h \
                      src/utils-branch.c \
                      src/utils-branch.h \
                      src/utils-chain.h \
                      src/utils-log.h
	@mkdir -p build
	@echo "  CC   utils-branch.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/utils-branch.o -Isrc -Igen \
	       -c src/utils-branch.c

build/utils-store.o: Makefile \
                     src/global-constants.h \
                     src/global-enums.h \
                     src/global-macros.h \
                     src/global-types.h \
                     src/utils-log.h \
                     src/utils-store.c \
                     src/utils-store.h
	@mkdir -p build
	@echo "  CC   utils-store.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/utils-store.o -Isrc -Igen \
	       -c src/utils-store.c

build/utils-dbus.o: Makefile \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-types.h \
                    src/utils-dbus.c \
                    src/utils-dbus.h \
                    src/utils-log.h
	@mkdir -p build
	@echo "  CC   utils-dbus.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/utils-dbus.o -Isrc -Igen \
	       -c src/utils-dbus.c \
	       -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include

build/utils-gl.o: Makefile \
                  src/global-constants.h \
                  src/global-enums.h \
                  src/global-types.h \
                  src/utils-gl.c \
                  src/utils-gl.h \
                  src/utils-log.h
	@mkdir -p build
	@echo "  CC   utils-gl.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/utils-gl.o -Isrc -Igen \
	       -c src/utils-gl.c \
	       -I/usr/include/libdrm

build/utils-keymap.o: Makefile \
                      src/global-constants.h \
                      src/global-enums.h \
                      src/global-types.h \
                      src/utils-environment.h \
                      src/utils-keymap.c \
                      src/utils-keymap.h \
                      src/utils-log.h
	@mkdir -p build
	@echo "  CC   utils-keymap.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/utils-keymap.o -Isrc -Igen \
	       -c src/utils-keymap.c

build/utils-log.o: Makefile \
                   gen/version.h \
                   src/global-constants.h \
                   src/global-enums.h \
                   src/global-types.h \
                   src/utils-environment.h \
                   src/utils-log.c \
                   src/utils-log.h
	@mkdir -p build
	@echo "  CC   utils-log.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/utils-log.o -Isrc -Igen \
	       -c src/utils-log.c

build/utils-environment.o: Makefile \
                           src/global-constants.h \
                           src/global-enums.h \
                           src/global-macros.h \
                           src/global-types.h \
                           src/utils-environment.c \
                           src/utils-environment.h \
                           src/utils-log.h
	@mkdir -p build
	@echo "  CC   utils-environment.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/utils-environment.o -Isrc -Igen \
	       -c src/utils-environment.c

build/event-dispatcher.o: Makefile \
                          src/event-dispatcher.c \
                          src/event-dispatcher.h \
                          src/global-constants.h \
                          src/global-enums.h \
                          src/global-macros.h \
                          src/global-types.h \
                          src/utils-chain.h \
                          src/utils-list.h \
                          src/utils-log.h
	@mkdir -p build
	@echo "  CC   event-dispatcher.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/event-dispatcher.o -Isrc -Igen \
	       -c src/event-dispatcher.c

build/event-timer.o: Makefile \
                     src/event-timer.c \
                     src/event-timer.h \
                     src/global-constants.h \
                     src/global-enums.h \
                     src/global-types.h \
                     src/utils-log.h
	@mkdir -p build
	@echo "  CC   event-timer.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/event-timer.o -Isrc -Igen \
	       -c src/event-timer.c

build/event-signals.o: Makefile \
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
                       src/utils-list.h \
                       src/utils-log.h \
                       src/utils-object.h
	@mkdir -p build
	@echo "  CC   event-signals.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/event-signals.o -Isrc -Igen \
	       -c src/event-signals.c

build/event-loop.o: Makefile \
                    src/event-loop.c \
                    src/event-loop.h \
                    src/event-task.h \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-macros.h \
                    src/global-types.h \
                    src/utils-chain.h \
                    src/utils-environment.h \
                    src/utils-list.h \
                    src/utils-log.h \
                    src/utils-object.h
	@mkdir -p build
	@echo "  CC   event-loop.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/event-loop.o -Isrc -Igen \
	       -c src/event-loop.c

build/event-task.o: Makefile \
                    src/event-task.c \
                    src/event-task.h \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-types.h \
                    src/utils-log.h \
                    src/utils-object.h
	@mkdir -p build
	@echo "  CC   event-task.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/event-task.o -Isrc -Igen \
	       -c src/event-task.c

build/event-factory.o: Makefile \
                       src/backend-offscreen.h \
                       src/device-evdev.h \
                       src/device-udev.h \
                       src/event-dispatcher.h \
                       src/event-factory.c \
                       src/event-factory.h \
                       src/event-loop.h \
                       src/event-task.h \
                       src/exhibitor-compositor.h \
                       src/exhibitor-display.h \
                       src/exhibitor-frame.h \
                       src/exhibitor.h \
                       src/global-constants.h \
                       src/global-enums.h \
                       src/global-macros.h \
                       src/global-types.h \
                       src/output-collector.h \
                       src/output.h \
                       src/renderer.h \
                       src/utils-branch.h \
                       src/utils-chain.h \
                       src/utils-environment.h \
                       src/utils-list.h \
                       src/utils-object.h \
                       src/utils-pool.h \
                       src/wayland.h
	@mkdir -p build
	@echo "  CC   event-factory.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/event-factory.o -Isrc -Igen \
	       -c src/event-factory.c

build/renderer.o: Makefile \
                  src/global-constants.h \
                  src/global-enums.h \
                  src/global-types.h \
                  src/renderer.c \
                  src/renderer.h \
                  src/utils-pool.h
	@mkdir -p build
	@echo "  CC   renderer.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/renderer.o -Isrc -Igen \
	       -c src/renderer.c

build/renderer-mmap.o: Makefile \
                       src/exhibitor-compositor.h \
                       src/exhibitor-frame.h \
                       src/global-constants.h \
                       src/global-enums.h \
                       src/global-macros.h \
                       src/global-types.h \
                       src/output.h \
                       src/renderer-mmap.c \
                       src/renderer-mmap.h \
                       src/renderer.h \
                       src/surface-data.h \
                       src/surface-manager.h \
                       src/utils-branch.h \
                       src/utils-chain.h \
                       src/utils-list.h \
                       src/utils-log.h \
                       src/utils-object.h \
                       src/utils-pool.h \
                       src/utils-store.h
	@mkdir -p build
	@echo "  CC   renderer-mmap.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/renderer-mmap.o -Isrc -Igen \
	       -c src/renderer-mmap.c

build/renderer-gl.o: Makefile \
                     src/bind-egl-wayland.h \
                     src/exhibitor-compositor.h \
                     src/exhibitor-frame.h \
                     src/global-constants.h \
                     src/global-enums.h \
                     src/global-macros.h \
                     src/global-types.h \
                     src/renderer-gl.c \
                     src/renderer-gl.h \
                     src/renderer.h \
                     src/surface-data.h \
                     src/surface-manager.h \
                     src/utils-branch.h \
                     src/utils-chain.h \
                     src/utils-gl.h \
                     src/utils-list.h \
                     src/utils-log.h \
                     src/utils-pool.h \
                     src/utils-store.h
	@mkdir -p build
	@echo "  CC   renderer-gl.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/renderer-gl.o -Isrc -Igen \
	       -c src/renderer-gl.c \
	       -I/usr/include/libdrm

build/device-common.o: Makefile \
                       src/device-common.c \
                       src/device-common.h \
                       src/global-constants.h \
                       src/global-enums.h \
                       src/global-types.h \
                       src/utils-dbus.h \
                       src/utils-log.h
	@mkdir -p build
	@echo "  CC   device-common.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/device-common.o -Isrc -Igen \
	       -c src/device-common.c

build/device-fb.o: Makefile \
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
                   src/utils-chain.h \
                   src/utils-list.h \
                   src/utils-log.h \
                   src/utils-object.h \
                   src/utils-pool.h
	@mkdir -p build
	@echo "  CC   device-fb.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/device-fb.o -Isrc -Igen \
	       -c src/device-fb.c

build/device-drm.o: Makefile \
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
                    src/utils-chain.h \
                    src/utils-gl.h \
                    src/utils-keymap.h \
                    src/utils-list.h \
                    src/utils-log.h \
                    src/utils-object.h \
                    src/utils-pool.h
	@mkdir -p build
	@echo "  CC   device-drm.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/device-drm.o -Isrc -Igen \
	       -c src/device-drm.c \
	       -I/usr/include/libdrm

build/device-evdev.o: Makefile \
                      src/device-common.h \
                      src/device-evdev.c \
                      src/device-evdev.h \
                      src/event-dispatcher.h \
                      src/event-signals.h \
                      src/event-task.h \
                      src/global-constants.h \
                      src/global-enums.h \
                      src/global-objects.h \
                      src/global-types.h \
                      src/keyboard-binding.h \
                      src/keyboard-bindings.h \
                      src/utils-dbus.h \
                      src/utils-log.h \
                      src/utils-object.h \
                      src/utils-pool.h
	@mkdir -p build
	@echo "  CC   device-evdev.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/device-evdev.o -Isrc -Igen \
	       -c src/device-evdev.c

build/device-udev.o: Makefile \
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
	@mkdir -p build
	@echo "  CC   device-udev.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/device-udev.o -Isrc -Igen \
	       -c src/device-udev.c

build/output.o: Makefile \
                src/global-constants.h \
                src/global-enums.h \
                src/global-macros.h \
                src/global-types.h \
                src/output.c \
                src/output.h \
                src/renderer.h \
                src/utils-object.h \
                src/utils-pool.h
	@mkdir -p build
	@echo "  CC   output.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/output.o -Isrc -Igen \
	       -c src/output.c

build/output-collector.o: Makefile \
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
                          src/utils-chain.h \
                          src/utils-keymap.h \
                          src/utils-list.h \
                          src/utils-log.h \
                          src/utils-object.h \
                          src/utils-pool.h
	@mkdir -p build
	@echo "  CC   output-collector.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/output-collector.o -Isrc -Igen \
	       -c src/output-collector.c

build/surface-data.o: Makefile \
                      src/exhibitor-compositor.h \
                      src/exhibitor-frame.h \
                      src/global-constants.h \
                      src/global-enums.h \
                      src/global-macros.h \
                      src/global-types.h \
                      src/surface-data.c \
                      src/surface-data.h \
                      src/utils-branch.h \
                      src/utils-chain.h \
                      src/utils-list.h \
                      src/utils-log.h \
                      src/utils-pool.h \
                      src/utils-store.h
	@mkdir -p build
	@echo "  CC   surface-data.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/surface-data.o -Isrc -Igen \
	       -c src/surface-data.c

build/surface-manager.o: Makefile \
                         src/event-signals.h \
                         src/event-task.h \
                         src/event-timer.h \
                         src/exhibitor-compositor.h \
                         src/exhibitor-frame.h \
                         src/global-constants.h \
                         src/global-enums.h \
                         src/global-macros.h \
                         src/global-types.h \
                         src/surface-data.h \
                         src/surface-manager.c \
                         src/surface-manager.h \
                         src/utils-branch.h \
                         src/utils-chain.h \
                         src/utils-list.h \
                         src/utils-log.h \
                         src/utils-object.h \
                         src/utils-pool.h \
                         src/utils-store.h
	@mkdir -p build
	@echo "  CC   surface-manager.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/surface-manager.o -Isrc -Igen \
	       -c src/surface-manager.c

build/keyboard-functions.o: Makefile \
                            src/event-loop.h \
                            src/event-task.h \
                            src/exhibitor-compositor.h \
                            src/exhibitor-display.h \
                            src/exhibitor-frame.h \
                            src/exhibitor.h \
                            src/global-constants.h \
                            src/global-enums.h \
                            src/global-macros.h \
                            src/global-types.h \
                            src/keyboard-functions.c \
                            src/keyboard-functions.h \
                            src/output.h \
                            src/renderer.h \
                            src/utils-branch.h \
                            src/utils-chain.h \
                            src/utils-list.h \
                            src/utils-log.h \
                            src/utils-object.h \
                            src/utils-pool.h
	@mkdir -p build
	@echo "  CC   keyboard-functions.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/keyboard-functions.o -Isrc -Igen \
	       -c src/keyboard-functions.c

build/keyboard-binding.o: Makefile \
                          src/global-constants.h \
                          src/global-enums.h \
                          src/global-types.h \
                          src/keyboard-binding.c \
                          src/keyboard-binding.h \
                          src/utils-log.h \
                          src/utils-pool.h
	@mkdir -p build
	@echo "  CC   keyboard-binding.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/keyboard-binding.o -Isrc -Igen \
	       -c src/keyboard-binding.c

build/keyboard-bindings.o: Makefile \
                           src/global-constants.h \
                           src/global-enums.h \
                           src/global-macros.h \
                           src/global-types.h \
                           src/keyboard-binding.h \
                           src/keyboard-bindings.c \
                           src/keyboard-bindings.h \
                           src/keyboard-mode.h \
                           src/utils-chain.h \
                           src/utils-list.h \
                           src/utils-log.h \
                           src/utils-pool.h
	@mkdir -p build
	@echo "  CC   keyboard-bindings.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/keyboard-bindings.o -Isrc -Igen \
	       -c src/keyboard-bindings.c

build/keyboard-mode.o: Makefile \
                       src/global-constants.h \
                       src/global-enums.h \
                       src/global-types.h \
                       src/keyboard-binding.h \
                       src/keyboard-mode.c \
                       src/keyboard-mode.h \
                       src/utils-log.h \
                       src/utils-pool.h
	@mkdir -p build
	@echo "  CC   keyboard-mode.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/keyboard-mode.o -Isrc -Igen \
	       -c src/keyboard-mode.c

build/exhibitor.o: Makefile \
                   src/event-loop.h \
                   src/event-signals.h \
                   src/event-task.h \
                   src/exhibitor-compositor.h \
                   src/exhibitor-display.h \
                   src/exhibitor-frame.h \
                   src/exhibitor-pointer.h \
                   src/exhibitor-strategist.h \
                   src/exhibitor.c \
                   src/exhibitor.h \
                   src/global-constants.h \
                   src/global-enums.h \
                   src/global-functions.h \
                   src/global-macros.h \
                   src/global-objects.h \
                   src/global-types.h \
                   src/output.h \
                   src/renderer.h \
                   src/surface-data.h \
                   src/surface-manager.h \
                   src/utils-branch.h \
                   src/utils-chain.h \
                   src/utils-list.h \
                   src/utils-log.h \
                   src/utils-object.h \
                   src/utils-pool.h \
                   src/utils-store.h
	@mkdir -p build
	@echo "  CC   exhibitor.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/exhibitor.o -Isrc -Igen \
	       -c src/exhibitor.c

build/exhibitor-display.o: Makefile \
                           src/event-loop.h \
                           src/event-signals.h \
                           src/event-task.h \
                           src/event-timer.h \
                           src/exhibitor-compositor.h \
                           src/exhibitor-display.c \
                           src/exhibitor-display.h \
                           src/exhibitor-frame.h \
                           src/exhibitor-pointer.h \
                           src/global-constants.h \
                           src/global-enums.h \
                           src/global-macros.h \
                           src/global-types.h \
                           src/output.h \
                           src/renderer.h \
                           src/surface-data.h \
                           src/surface-manager.h \
                           src/utils-branch.h \
                           src/utils-chain.h \
                           src/utils-environment.h \
                           src/utils-list.h \
                           src/utils-log.h \
                           src/utils-object.h \
                           src/utils-pool.h \
                           src/utils-store.h
	@mkdir -p build
	@echo "  CC   exhibitor-display.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/exhibitor-display.o -Isrc -Igen \
	       -c src/exhibitor-display.c

build/exhibitor-compositor.o: Makefile \
                              src/event-loop.h \
                              src/event-task.h \
                              src/exhibitor-compositor.c \
                              src/exhibitor-compositor.h \
                              src/exhibitor-display.h \
                              src/exhibitor-frame.h \
                              src/exhibitor.h \
                              src/global-constants.h \
                              src/global-enums.h \
                              src/global-macros.h \
                              src/global-types.h \
                              src/output.h \
                              src/renderer.h \
                              src/surface-data.h \
                              src/surface-manager.h \
                              src/utils-branch.h \
                              src/utils-chain.h \
                              src/utils-list.h \
                              src/utils-log.h \
                              src/utils-object.h \
                              src/utils-pool.h \
                              src/utils-store.h
	@mkdir -p build
	@echo "  CC   exhibitor-compositor.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/exhibitor-compositor.o -Isrc -Igen \
	       -c src/exhibitor-compositor.c

build/exhibitor-frame.o: Makefile \
                         src/event-signals.h \
                         src/event-task.h \
                         src/exhibitor-compositor.h \
                         src/exhibitor-frame.c \
                         src/exhibitor-frame.h \
                         src/global-constants.h \
                         src/global-enums.h \
                         src/global-macros.h \
                         src/global-types.h \
                         src/surface-data.h \
                         src/surface-manager.h \
                         src/utils-branch.h \
                         src/utils-chain.h \
                         src/utils-list.h \
                         src/utils-log.h \
                         src/utils-object.h \
                         src/utils-pool.h \
                         src/utils-store.h
	@mkdir -p build
	@echo "  CC   exhibitor-frame.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/exhibitor-frame.o -Isrc -Igen \
	       -c src/exhibitor-frame.c

build/exhibitor-strategist.o: Makefile \
                              src/event-loop.h \
                              src/event-signals.h \
                              src/event-task.h \
                              src/exhibitor-compositor.h \
                              src/exhibitor-display.h \
                              src/exhibitor-frame.h \
                              src/exhibitor-strategist.c \
                              src/exhibitor-strategist.h \
                              src/exhibitor.h \
                              src/global-constants.h \
                              src/global-enums.h \
                              src/global-macros.h \
                              src/global-types.h \
                              src/output.h \
                              src/renderer.h \
                              src/utils-branch.h \
                              src/utils-chain.h \
                              src/utils-list.h \
                              src/utils-log.h \
                              src/utils-object.h \
                              src/utils-pool.h
	@mkdir -p build
	@echo "  CC   exhibitor-strategist.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/exhibitor-strategist.o -Isrc -Igen \
	       -c src/exhibitor-strategist.c

build/exhibitor-pointer.o: Makefile \
                           src/event-loop.h \
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
                           src/surface-data.h \
                           src/surface-manager.h \
                           src/utils-branch.h \
                           src/utils-chain.h \
                           src/utils-list.h \
                           src/utils-log.h \
                           src/utils-object.h \
                           src/utils-pool.h \
                           src/utils-store.h
	@mkdir -p build
	@echo "  CC   exhibitor-pointer.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/exhibitor-pointer.o -Isrc -Igen \
	       -c src/exhibitor-pointer.c

build/wayland-region.o: Makefile \
                        src/global-constants.h \
                        src/global-enums.h \
                        src/global-types.h \
                        src/utils-log.h \
                        src/wayland-region.c \
                        src/wayland-region.h
	@mkdir -p build
	@echo "  CC   wayland-region.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-region.o -Isrc -Igen \
	       -c src/wayland-region.c

build/wayland-surface.o: Makefile \
                         src/global-constants.h \
                         src/global-enums.h \
                         src/global-types.h \
                         src/utils-log.h \
                         src/wayland-surface.c \
                         src/wayland-surface.h \
                         src/wayland-types.h
	@mkdir -p build
	@echo "  CC   wayland-surface.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-surface.o -Isrc -Igen \
	       -c src/wayland-surface.c

build/wayland-output.o: Makefile \
                        src/global-constants.h \
                        src/global-enums.h \
                        src/global-types.h \
                        src/output.h \
                        src/renderer.h \
                        src/utils-log.h \
                        src/utils-object.h \
                        src/utils-pool.h \
                        src/utils-store.h \
                        src/wayland-output.c \
                        src/wayland-output.h
	@mkdir -p build
	@echo "  CC   wayland-output.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-output.o -Isrc -Igen \
	       -c src/wayland-output.c

build/wayland-cache.o: Makefile \
                       src/global-constants.h \
                       src/global-enums.h \
                       src/global-macros.h \
                       src/global-types.h \
                       src/utils-chain.h \
                       src/utils-list.h \
                       src/utils-log.h \
                       src/utils-store.h \
                       src/wayland-cache.c \
                       src/wayland-cache.h \
                       src/wayland-region.h \
                       src/wayland-surface.h \
                       src/wayland-types.h
	@mkdir -p build
	@echo "  CC   wayland-cache.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-cache.o -Isrc -Igen \
	       -c src/wayland-cache.c

build/wayland-state.o: Makefile \
                       gen/xdg-shell-server-protocol.h \
                       src/exhibitor-compositor.h \
                       src/exhibitor-frame.h \
                       src/global-constants.h \
                       src/global-enums.h \
                       src/global-macros.h \
                       src/global-types.h \
                       src/output.h \
                       src/renderer.h \
                       src/surface-data.h \
                       src/surface-manager.h \
                       src/utils-branch.h \
                       src/utils-chain.h \
                       src/utils-list.h \
                       src/utils-log.h \
                       src/utils-object.h \
                       src/utils-pool.h \
                       src/utils-store.h \
                       src/wayland-cache.h \
                       src/wayland-output.h \
                       src/wayland-protocol-output.h \
                       src/wayland-region.h \
                       src/wayland-state.c \
                       src/wayland-state.h \
                       src/wayland-surface.h \
                       src/wayland-types.h
	@mkdir -p build
	@echo "  CC   wayland-state.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-state.o -Isrc -Igen \
	       -c src/wayland-state.c

build/wayland.o: Makefile \
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
                 src/utils-chain.h \
                 src/utils-environment.h \
                 src/utils-keymap.h \
                 src/utils-list.h \
                 src/utils-log.h \
                 src/utils-object.h \
                 src/utils-pool.h \
                 src/utils-store.h \
                 src/wayland-cache.h \
                 src/wayland-protocol-compositor.h \
                 src/wayland-protocol-output.h \
                 src/wayland-protocol-screenshooter.h \
                 src/wayland-protocol-seat.h \
                 src/wayland-protocol-shell.h \
                 src/wayland-protocol-xdg-shell.h \
                 src/wayland-region.h \
                 src/wayland-state.h \
                 src/wayland-surface.h \
                 src/wayland-types.h \
                 src/wayland.c \
                 src/wayland.h
	@mkdir -p build
	@echo "  CC   wayland.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland.o -Isrc -Igen \
	       -c src/wayland.c

build/wayland-protocol-compositor.o: Makefile \
                                     src/exhibitor-compositor.h \
                                     src/exhibitor-frame.h \
                                     src/global-constants.h \
                                     src/global-enums.h \
                                     src/global-macros.h \
                                     src/global-types.h \
                                     src/output.h \
                                     src/renderer.h \
                                     src/surface-data.h \
                                     src/surface-manager.h \
                                     src/utils-branch.h \
                                     src/utils-chain.h \
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
	@mkdir -p build
	@echo "  CC   wayland-protocol-compositor.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-protocol-compositor.o -Isrc -Igen \
	       -c src/wayland-protocol-compositor.c

build/wayland-protocol-surface.o: Makefile \
                                  src/exhibitor-compositor.h \
                                  src/exhibitor-frame.h \
                                  src/global-constants.h \
                                  src/global-enums.h \
                                  src/global-macros.h \
                                  src/global-types.h \
                                  src/output.h \
                                  src/renderer.h \
                                  src/surface-data.h \
                                  src/surface-manager.h \
                                  src/utils-branch.h \
                                  src/utils-chain.h \
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
	@mkdir -p build
	@echo "  CC   wayland-protocol-surface.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-protocol-surface.o -Isrc -Igen \
	       -c src/wayland-protocol-surface.c

build/wayland-protocol-region.o: Makefile \
                                 src/global-constants.h \
                                 src/global-enums.h \
                                 src/global-macros.h \
                                 src/global-types.h \
                                 src/utils-chain.h \
                                 src/utils-list.h \
                                 src/utils-log.h \
                                 src/utils-store.h \
                                 src/wayland-cache.h \
                                 src/wayland-protocol-region.c \
                                 src/wayland-protocol-region.h \
                                 src/wayland-region.h \
                                 src/wayland-surface.h \
                                 src/wayland-types.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-region.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-protocol-region.o -Isrc -Igen \
	       -c src/wayland-protocol-region.c

build/wayland-protocol-shell.o: Makefile \
                                src/exhibitor-compositor.h \
                                src/exhibitor-frame.h \
                                src/global-constants.h \
                                src/global-enums.h \
                                src/global-macros.h \
                                src/global-types.h \
                                src/surface-data.h \
                                src/surface-manager.h \
                                src/utils-branch.h \
                                src/utils-chain.h \
                                src/utils-list.h \
                                src/utils-log.h \
                                src/utils-pool.h \
                                src/utils-store.h \
                                src/wayland-cache.h \
                                src/wayland-protocol-shell-surface.h \
                                src/wayland-protocol-shell.c \
                                src/wayland-protocol-shell.h \
                                src/wayland-region.h \
                                src/wayland-surface.h \
                                src/wayland-types.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-shell.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-protocol-shell.o -Isrc -Igen \
	       -c src/wayland-protocol-shell.c

build/wayland-protocol-shell-surface.o: Makefile \
                                        src/global-constants.h \
                                        src/global-enums.h \
                                        src/global-macros.h \
                                        src/global-types.h \
                                        src/utils-log.h \
                                        src/wayland-protocol-shell-surface.c \
                                        src/wayland-protocol-shell-surface.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-shell-surface.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-protocol-shell-surface.o -Isrc -Igen \
	       -c src/wayland-protocol-shell-surface.c

build/wayland-protocol-xdg-shell.o: Makefile \
                                    gen/xdg-shell-server-protocol.h \
                                    src/exhibitor-compositor.h \
                                    src/exhibitor-frame.h \
                                    src/global-constants.h \
                                    src/global-enums.h \
                                    src/global-macros.h \
                                    src/global-types.h \
                                    src/surface-data.h \
                                    src/surface-manager.h \
                                    src/utils-branch.h \
                                    src/utils-chain.h \
                                    src/utils-list.h \
                                    src/utils-log.h \
                                    src/utils-pool.h \
                                    src/utils-store.h \
                                    src/wayland-cache.h \
                                    src/wayland-protocol-xdg-shell.c \
                                    src/wayland-protocol-xdg-surface.h \
                                    src/wayland-region.h \
                                    src/wayland-surface.h \
                                    src/wayland-types.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-xdg-shell.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-protocol-xdg-shell.o -Isrc -Igen \
	       -c src/wayland-protocol-xdg-shell.c

build/wayland-protocol-xdg-surface.o: Makefile \
                                      gen/xdg-shell-server-protocol.h \
                                      src/exhibitor-compositor.h \
                                      src/exhibitor-frame.h \
                                      src/global-constants.h \
                                      src/global-enums.h \
                                      src/global-macros.h \
                                      src/global-types.h \
                                      src/surface-data.h \
                                      src/surface-manager.h \
                                      src/utils-branch.h \
                                      src/utils-chain.h \
                                      src/utils-list.h \
                                      src/utils-log.h \
                                      src/utils-pool.h \
                                      src/utils-store.h \
                                      src/wayland-protocol-xdg-surface.c \
                                      src/wayland-protocol-xdg-surface.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-xdg-surface.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-protocol-xdg-surface.o -Isrc -Igen \
	       -c src/wayland-protocol-xdg-surface.c

build/xdg-shell-protocol.o: Makefile \
                            gen/xdg-shell-protocol.c
	@mkdir -p build
	@echo "  CC   xdg-shell-protocol.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/xdg-shell-protocol.o -Isrc -Igen \
	       -c gen/xdg-shell-protocol.c

build/wayland-protocol-output.o: Makefile \
                                 src/global-constants.h \
                                 src/global-enums.h \
                                 src/global-macros.h \
                                 src/global-types.h \
                                 src/output.h \
                                 src/renderer.h \
                                 src/utils-log.h \
                                 src/utils-object.h \
                                 src/utils-pool.h \
                                 src/wayland-protocol-output.c \
                                 src/wayland-protocol-output.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-output.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-protocol-output.o -Isrc -Igen \
	       -c src/wayland-protocol-output.c

build/wayland-protocol-seat.o: Makefile \
                               src/config.h \
                               src/global-constants.h \
                               src/global-enums.h \
                               src/global-macros.h \
                               src/global-types.h \
                               src/output.h \
                               src/renderer.h \
                               src/utils-chain.h \
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
	@mkdir -p build
	@echo "  CC   wayland-protocol-seat.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-protocol-seat.o -Isrc -Igen \
	       -c src/wayland-protocol-seat.c

build/wayland-protocol-pointer.o: Makefile \
                                  src/global-constants.h \
                                  src/global-enums.h \
                                  src/global-macros.h \
                                  src/global-types.h \
                                  src/output.h \
                                  src/renderer.h \
                                  src/utils-log.h \
                                  src/utils-object.h \
                                  src/utils-pool.h \
                                  src/utils-store.h \
                                  src/wayland-protocol-keyboard.h \
                                  src/wayland-protocol-pointer.c \
                                  src/wayland-state.h \
                                  src/wayland-surface.h \
                                  src/wayland-types.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-pointer.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-protocol-pointer.o -Isrc -Igen \
	       -c src/wayland-protocol-pointer.c

build/wayland-protocol-keyboard.o: Makefile \
                                   src/global-constants.h \
                                   src/global-enums.h \
                                   src/global-macros.h \
                                   src/global-types.h \
                                   src/utils-log.h \
                                   src/wayland-protocol-keyboard.c \
                                   src/wayland-protocol-keyboard.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-keyboard.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-protocol-keyboard.o -Isrc -Igen \
	       -c src/wayland-protocol-keyboard.c

build/wayland-protocol-screenshooter.o: Makefile \
                                        gen/screenshooter-server-protocol.h \
                                        src/global-constants.h \
                                        src/global-enums.h \
                                        src/global-macros.h \
                                        src/global-types.h \
                                        src/output.h \
                                        src/renderer.h \
                                        src/utils-log.h \
                                        src/utils-object.h \
                                        src/utils-pool.h \
                                        src/wayland-protocol-screenshooter.c \
                                        src/wayland-protocol-screenshooter.h
	@mkdir -p build
	@echo "  CC   wayland-protocol-screenshooter.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/wayland-protocol-screenshooter.o -Isrc -Igen \
	       -c src/wayland-protocol-screenshooter.c

build/bind-egl-wayland.o: Makefile \
                          src/bind-egl-wayland.c \
                          src/event-loop.h \
                          src/event-task.h \
                          src/global-constants.h \
                          src/global-enums.h \
                          src/global-types.h \
                          src/renderer-gl.h \
                          src/renderer.h \
                          src/utils-gl.h \
                          src/utils-log.h \
                          src/utils-object.h \
                          src/utils-pool.h \
                          src/wayland.h
	@mkdir -p build
	@echo "  CC   bind-egl-wayland.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/bind-egl-wayland.o -Isrc -Igen \
	       -c src/bind-egl-wayland.c

build/backend-offscreen.o: Makefile \
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
                           src/utils-chain.h \
                           src/utils-gl.h \
                           src/utils-list.h \
                           src/utils-log.h \
                           src/utils-object.h \
                           src/utils-pool.h
	@mkdir -p build
	@echo "  CC   backend-offscreen.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/backend-offscreen.o -Isrc -Igen \
	       -c src/backend-offscreen.c

build/metanoia.o: Makefile \
                  src/config.h \
                  src/event-dispatcher.h \
                  src/event-factory.h \
                  src/event-loop.h \
                  src/event-signals.h \
                  src/event-task.h \
                  src/exhibitor-compositor.h \
                  src/exhibitor-frame.h \
                  src/global-constants.h \
                  src/global-enums.h \
                  src/global-macros.h \
                  src/global-types.h \
                  src/metanoia.c \
                  src/surface-data.h \
                  src/surface-manager.h \
                  src/utils-branch.h \
                  src/utils-chain.h \
                  src/utils-dbus.h \
                  src/utils-environment.h \
                  src/utils-keymap.h \
                  src/utils-list.h \
                  src/utils-log.h \
                  src/utils-object.h \
                  src/utils-pool.h \
                  src/utils-store.h
	@mkdir -p build
	@echo "  CC   metanoia.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/metanoia.o -Isrc -Igen \
	       -c src/metanoia.c

build/controller-defs.o: Makefile \
                         src/controller-defs.c \
                         src/controller-defs.h
	@mkdir -p build
	@echo "  CC   controller-defs.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/controller-defs.o -Isrc -Igen \
	       -c src/controller-defs.c \
	       -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

build/controller-output.o: Makefile \
                           src/controller-defs.h \
                           src/controller-output.c \
                           src/controller-output.h
	@mkdir -p build
	@echo "  CC   controller-output.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/controller-output.o -Isrc -Igen \
	       -c src/controller-output.c \
	       -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

build/controller-gtk-res.o: Makefile \
                            gen/controller-gtk-res.c
	@mkdir -p build
	@echo "  CC   controller-gtk-res.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/controller-gtk-res.o -Isrc -Igen \
	       -c gen/controller-gtk-res.c \
	       -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/harfbuzz -I/usr/include/libdrm -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

build/controller-gtk-display.o: Makefile \
                                src/controller-defs.h \
                                src/controller-gtk-display.c \
                                src/controller-gtk-display.h \
                                src/controller-output.h \
                                src/global-constants.h \
                                src/global-enums.h \
                                src/global-types.h \
                                src/utils-log.h
	@mkdir -p build
	@echo "  CC   controller-gtk-display.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/controller-gtk-display.o -Isrc -Igen \
	       -c src/controller-gtk-display.c \
	       -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/harfbuzz -I/usr/include/libdrm -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

build/controller-gtk-win.o: Makefile \
                            gen/version.h \
                            src/controller-gtk-app.h \
                            src/controller-gtk-display.h \
                            src/controller-gtk-win.c \
                            src/controller-gtk-win.h \
                            src/controller-output.h
	@mkdir -p build
	@echo "  CC   controller-gtk-win.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/controller-gtk-win.o -Isrc -Igen \
	       -c src/controller-gtk-win.c \
	       -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/harfbuzz -I/usr/include/libdrm -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

build/controller-gtk-app.o: Makefile \
                            src/controller-bind.h \
                            src/controller-defs.h \
                            src/controller-gtk-app.c \
                            src/controller-gtk-app.h \
                            src/controller-gtk-win.h \
                            src/controller-output.h
	@mkdir -p build
	@echo "  CC   controller-gtk-app.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/controller-gtk-app.o -Isrc -Igen \
	       -c src/controller-gtk-app.c \
	       -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/harfbuzz -I/usr/include/libdrm -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

build/controller-wayland.o: Makefile \
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
	@mkdir -p build
	@echo "  CC   controller-wayland.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/controller-wayland.o -Isrc -Igen \
	       -c src/controller-wayland.c \
	       -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

build/metanoiactl-gtk.o: Makefile \
                         src/controller-gtk-app.h \
                         src/global-constants.h \
                         src/global-enums.h \
                         src/global-types.h \
                         src/metanoiactl-gtk.c \
                         src/utils-environment.h
	@mkdir -p build
	@echo "  CC   metanoiactl-gtk.o"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o build/metanoiactl-gtk.o -Isrc -Igen \
	       -c src/metanoiactl-gtk.c \
	       -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/harfbuzz -I/usr/include/libdrm -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

checks/check-pool: Makefile \
                   src/global-enums.h \
                   src/global-macros.h \
                   src/global-types.h \
                   src/utils-pool.c \
                   src/utils-pool.h \
                   tests/test-pool.c \
                   tests/tests-suit.h
	@mkdir -p checks
	@echo "  CC   check-pool"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o checks/check-pool -Isrc -Igen \
	      tests/test-pool.c src/utils-pool.c

checks/check-chain: Makefile \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-types.h \
                    src/utils-chain.c \
                    src/utils-chain.h \
                    tests/test-chain.c \
                    tests/tests-suit.h
	@mkdir -p checks
	@echo "  CC   check-chain"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o checks/check-chain -Isrc -Igen \
	      tests/test-chain.c src/utils-chain.c

checks/check-list: Makefile \
                   src/global-constants.h \
                   src/global-enums.h \
                   src/global-macros.h \
                   src/global-types.h \
                   src/utils-chain.c \
                   src/utils-chain.h \
                   src/utils-list.c \
                   src/utils-list.h \
                   tests/test-list.c \
                   tests/tests-suit.h
	@mkdir -p checks
	@echo "  CC   check-list"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o checks/check-list -Isrc -Igen \
	      tests/test-list.c src/utils-chain.c src/utils-list.c

checks/check-branch: Makefile \
                     src/global-constants.h \
                     src/global-enums.h \
                     src/global-types.h \
                     src/utils-branch.c \
                     src/utils-branch.h \
                     src/utils-chain.c \
                     src/utils-chain.h \
                     src/utils-log.h \
                     tests/test-branch.c \
                     tests/tests-suit.h
	@mkdir -p checks
	@echo "  CC   check-branch"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o checks/check-branch -Isrc -Igen \
	      tests/test-branch.c src/utils-chain.c src/utils-branch.c

checks/check-store: Makefile \
                    src/global-constants.h \
                    src/global-enums.h \
                    src/global-macros.h \
                    src/global-types.h \
                    src/utils-chain.c \
                    src/utils-chain.h \
                    src/utils-log.h \
                    src/utils-store.c \
                    src/utils-store.h \
                    tests/test-store.c \
                    tests/tests-suit.h
	@mkdir -p checks
	@echo "  CC   check-store"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o checks/check-store -Isrc -Igen \
	      tests/test-store.c src/utils-chain.c src/utils-store.c

checks/check-frame: Makefile \
                    src/event-signals.h \
                    src/event-task.h \
                    src/exhibitor-compositor.h \
                    src/exhibitor-frame.c \
                    src/exhibitor-frame.h \
                    src/global-constants.h \
                    src/global-enums.c \
                    src/global-enums.h \
                    src/global-macros.h \
                    src/global-types.h \
                    src/surface-data.h \
                    src/surface-manager.h \
                    src/utils-branch.c \
                    src/utils-branch.h \
                    src/utils-chain.c \
                    src/utils-chain.h \
                    src/utils-list.h \
                    src/utils-log.h \
                    src/utils-object.h \
                    src/utils-pool.c \
                    src/utils-pool.h \
                    src/utils-store.c \
                    src/utils-store.h \
                    tests/fake-utils-log.c \
                    tests/mock-surface-manager.c \
                    tests/mock-surface-manager.h \
                    tests/test-frame.c \
                    tests/tests-suit.h
	@mkdir -p checks
	@echo "  CC   check-frame"
	@gcc -std=gnu11 -Wall -W -Wextra -Wpedantic -Werror -DDEBUG -g -O0 -o checks/check-frame -Isrc -Igen \
	      tests/test-frame.c tests/fake-utils-log.c tests/mock-surface-manager.c src/utils-chain.c src/utils-branch.c src/utils-store.c src/utils-pool.c src/global-enums.c src/exhibitor-frame.c

