CC=gcc
WFLAGS=-Wall
LFLAGS=-rdynamic -ldl -lrt -lpthread -lm

build=build


all: ${build}/aura

${build}:
	mkdir -p ${build}

${build}/aura: ${build}/aura.o ${build}/log.o ${build}/dbus.o ${build}/loop.o ${build}/event_dispatcher.o ${build}/chain.o ${build}/task.o ${build}/task_factory.o ${build}/evdev.o ${build}/udev.o ${build}/drm.o ${build}/shared.o ${build}/devfb.o ${build}/wayland.o ${build}/wayland-compositor.o ${build}/wayland-surface.o ${build}/wayland-output.o ${build}/wayland-shell.o ${build}/wayland-xdg_shell.o ${build}/xdg-shell-protocol.o ${build}/wayland-shell-surface.o ${build}/surface-aggregator.o ${build}/surface-compositor.o ${build}/surface-manager.o ${build}/keyboard-bindings.o ${build}/configuration-functions.o ${build}/renderer-mmap.o ${build}/renderer-gl.o
	gcc ${LFLAGS} ${build}/aura.o ${build}/log.o ${build}/dbus.o ${build}/loop.o ${build}/event_dispatcher.o \
		${build}/keyboard-bindings.o \
		${build}/configuration-functions.o \
		${build}/chain.o ${build}/task.o ${build}/task_factory.o ${build}/evdev.o ${build}/udev.o ${build}/drm.o \
		${build}/shared.o ${build}/devfb.o \
		${build}/wayland.o ${build}/wayland-compositor.o ${build}/wayland-surface.o ${build}/wayland-output.o \
		${build}/wayland-shell.o ${build}/wayland-xdg_shell.o ${build}/xdg-shell-protocol.o ${build}/wayland-shell-surface.o \
		${build}/renderer-mmap.o ${build}/renderer-gl.o \
		${build}/surface-aggregator.o ${build}/surface-compositor.o ${build}/surface-manager.o \
		-o ${build}/aura `pkg-config --libs dbus-1` `pkg-config --libs libudev` `pkg-config --libs libdrm` `pkg-config --libs wayland-server` `pkg-config --libs gbm` `pkg-config --libs egl` `pkg-config --libs gl`


${build}/aura.o: ${build} src/aura.c force
	gcc ${WFLAGS} -c src/aura.c -o ${build}/aura.o -Isrc

${build}/configuration-functions.o: ${build} src/configuration-functions.c src/configuration-functions.h force
	${CC} ${WFLAGS} -c src/configuration-functions.c -o ${build}/configuration-functions.o -Isrc


${build}/chain.o: ${build} src/utils/chain.c src/utils/chain.h force
	gcc ${WFLAGS} -c src/utils/chain.c -o ${build}/chain.o -Isrc

${build}/dbus.o: ${build} src/utils/dbus.c src/utils/dbus.h force
	gcc ${WFLAGS} -c src/utils/dbus.c -o ${build}/dbus.o `pkg-config --cflags dbus-1` -Isrc

${build}/event_dispatcher.o: ${build} src/utils/event_dispatcher.c src/utils/event_dispatcher.h force
	gcc ${WFLAGS} -c src/utils/event_dispatcher.c -o ${build}/event_dispatcher.o -fworking-directory -Isrc

${build}/log.o: ${build} src/utils/log.c src/utils/log.h force
	gcc ${WFLAGS} -c src/utils/log.c -o ${build}/log.o -Isrc

${build}/loop.o: ${build} src/utils/loop.c src/utils/loop.h force
	gcc ${WFLAGS} -c src/utils/loop.c -o ${build}/loop.o -Isrc

${build}/task.o: ${build} src/utils/task.c src/utils/task.h force
	gcc ${WFLAGS} -c src/utils/task.c -o ${build}/task.o -Isrc

${build}/task_factory.o: ${build} src/utils/task_factory.c src/utils/task_factory.h force
	gcc ${WFLAGS} -c src/utils/task_factory.c -o ${build}/task_factory.o -Isrc


${build}/keyboard-bindings.o: ${build} src/keyboard-bindings.c src/keyboard-bindings.h force
	${CC} ${WFLAGS} -c src/keyboard-bindings.c -o ${build}/keyboard-bindings.o -Isrc


${build}/shared.o: ${build} src/devices/shared.c src/devices/shared.h force
	gcc ${WFLAGS} -c src/devices/shared.c -o ${build}/shared.o -Isrc

${build}/devfb.o: ${build} src/devices/devfb.c src/devices/devfb.h force
	gcc ${WFLAGS} -c src/devices/devfb.c -o ${build}/devfb.o -Isrc

${build}/evdev.o: ${build} src/devices/evdev.c src/devices/evdev.h force
	gcc ${WFLAGS} -c src/devices/evdev.c -o ${build}/evdev.o -Isrc

${build}/udev.o: ${build} src/devices/udev.c src/devices/udev.h force
	gcc ${WFLAGS} -c src/devices/udev.c -o ${build}/udev.o -Isrc

${build}/drm.o: ${build} src/devices/drm.c src/devices/drm.h force
	gcc ${WFLAGS} -c src/devices/drm.c -o ${build}/drm.o -Isrc `pkg-config --cflags libdrm` `pkg-config --cflags egl`


${build}/wayland.o: ${build} src/frontends/wayland.c src/frontends/wayland.h force
	gcc ${WFLAGS} -c src/frontends/wayland.c -o ${build}/wayland.o -Isrc

${build}/wayland-compositor.o: ${build} src/frontends/wayland-compositor.c src/frontends/wayland-compositor.h force
	gcc ${WFLAGS} -c src/frontends/wayland-compositor.c -o ${build}/wayland-compositor.o -Isrc

${build}/wayland-surface.o: ${build} src/frontends/wayland-surface.c src/frontends/wayland-surface.h force
	gcc ${WFLAGS} -c src/frontends/wayland-surface.c -o ${build}/wayland-surface.o -Isrc

${build}/wayland-shell.o: ${build} src/frontends/wayland-shell.c src/frontends/wayland-shell.h force
	gcc ${WFLAGS} -c src/frontends/wayland-shell.c -o ${build}/wayland-shell.o -Isrc

${build}/wayland-shell-surface.o: ${build} src/frontends/wayland-shell-surface.c src/frontends/wayland-shell-surface.h force
	gcc ${WFLAGS} -c src/frontends/wayland-shell-surface.c -o ${build}/wayland-shell-surface.o -Isrc

${build}/wayland-xdg_shell.o: ${build} src/frontends/wayland-xdg_shell.c src/frontends/wayland-xdg_shell.h force
	gcc ${WFLAGS} -c src/frontends/wayland-xdg_shell.c -o ${build}/wayland-xdg_shell.o -Isrc

${build}/xdg-shell-protocol.o: ${build} src/frontends/xdg-shell-protocol.c force
	gcc ${WFLAGS} -c src/frontends/xdg-shell-protocol.c -o ${build}/xdg-shell-protocol.o -Isrc

${build}/wayland-output.o: ${build} src/frontends/wayland-output.c src/frontends/wayland-output.h force
	gcc ${WFLAGS} -c src/frontends/wayland-output.c -o ${build}/wayland-output.o -Isrc


${build}/renderer-mmap.o: ${build} src/renderer-mmap.c src/renderer-mmap.h force
	${CC} ${WFLAGS} -c src/renderer-mmap.c -o ${build}/renderer-mmap.o -Isrc

${build}/renderer-gl.o: ${build} src/renderer-gl.c src/renderer-gl.h force
	${CC} ${WFLAGS} -c src/renderer-gl.c -o ${build}/renderer-gl.o -Isrc


${build}/surface-aggregator.o: ${build} src/surface-aggregator.c src/surface-aggregator.h force
	${CC} ${WFLAGS} -c src/surface-aggregator.c -o ${build}/surface-aggregator.o -Isrc

${build}/surface-compositor.o: ${build} src/surface-compositor.c src/surface-compositor.h force
	${CC} ${WFLAGS} -c src/surface-compositor.c -o ${build}/surface-compositor.o -Isrc

${build}/surface-manager.o: ${build} src/surface-manager.c src/surface-manager.h force
	${CC} ${WFLAGS} -c src/surface-manager.c -o ${build}/surface-manager.o -Isrc

clear: force
	rm -r ${build}

force:

