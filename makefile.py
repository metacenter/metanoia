#!/usr/bin/python
# file: makefile.py
# vim: tabstop=4 expandtab colorcolumn=81 list

import sys
import make

m = make.Make()

m.set_source_directory('src')
m.set_build_directory('build')
m.set_resource_directory('res')
m.set_gen_directory('gen')

m.set_wflags(['-Wall'])
m.set_lflags(['-rdynamic', '-ldl', '-lrt', '-lpthread', '-lm'])

#-------------------------------------------------------------------------------

with_gtk_support = True;

#-------------------------------------------------------------------------------

aura = m.add_link_target(
        output='aura',
        include_in_all=True
    )

#-------------------------------------------------------------------------------
# RESOUCES

wsshg = make.Generator(
        command_body='wayland-scanner server-header',
        command_args=' < "{inputs}" > "{output}"',
        shortcut='WSSHG'
    )
m.add_generator(wsshg)

wscg = make.Generator(
        command_body='wayland-scanner code',
        command_args=' < "{inputs}" > "{output}"',
        shortcut='WSCG'
    )
m.add_generator(wscg)

glcr = make.Generator(
        command_body='glib-compile-resources',
        command_args='{inputs} --target={output} --generate-source',
        shortcut='GLCR'
    )
m.add_generator(glcr)

target_xdg_shell_protocol = m.add_generated_target(
        generator=wsshg,
        output='xdg-shell-server-protocol.h',
        inputs=['xdg-shell.xml'],
        input_dir=m.get_resource_directory()[0]
    )

target_xdg_shell_protocol_code = m.add_generated_target(
        generator=wscg,
        output='xdg-shell-protocol.c',
        inputs=['xdg-shell.xml'],
        input_dir=m.get_resource_directory()[0]
    )

target_gtk_resources = m.add_generated_target(
        generator=glcr,
        output='backend-gtk-res.c',
        inputs=['aura.gresource.xml'],
        includes=['backend-gtk-main.ui',
                  'backend-gtk-menu.ui',
                  'backend-gtk-area.ui'],
        input_dir=m.get_resource_directory()[0]
    )

#-------------------------------------------------------------------------------
# MAIN

t = m.add_compile_target(
        output='aura.o',
        inputs=['aura.c']
    )
aura.add_input(t)

#-------------------------------------------------------------------------------
# CONFIGURATION

t = m.add_compile_target(
        output='config.o',
        inputs=['config.c'],
        includes=['config.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='global-functions.o',
        inputs=['global-functions.c'],
        includes=['global-functions.h']
    )
aura.add_input(t)

#-------------------------------------------------------------------------------
# UTILS

t = m.add_compile_target(
        output='utils-chain.o',
        inputs=['utils-chain.c'],
        includes=['utils-chain.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='utils-store.o',
        inputs=['utils-store.c'],
        includes=['utils-store.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='utils-dbus.o',
        inputs=['utils-dbus.c'],
        includes=['utils-dbus.h'],
        pkgs={'dbus-1'}
    )
aura.add_input(t)

t = m.add_compile_target(
        output='utils-keymap.o',
        inputs=['utils-keymap.c'],
        includes=['utils-keymap.h'],
        pkgs={'xkbcommon'}
    )
aura.add_input(t)

t = m.add_compile_target(
        output='utils-log.o',
        inputs=['utils-log.c'],
        includes=['utils-log.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='utils-environment.o',
        inputs=['utils-environment.c'],
        includes=['utils-environment.h']
    )
aura.add_input(t)

#-------------------------------------------------------------------------------
# EVENTS

t = m.add_compile_target(
        output='event-dispatcher.o',
        inputs=['event-dispatcher.c'],
        includes=['event-dispatcher.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='event-timer.o',
        inputs=['event-timer.c'],
        includes=['event-timer.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='event-signals.o',
        inputs=['event-signals.c'],
        includes=['event-signals.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='event-loop.o',
        inputs=['event-loop.c'],
        includes=['event-loop.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='event-task.o',
        inputs=['event-task.c'],
        includes=['event-task.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='event-factory.o',
        inputs=['event-factory.c'],
        includes=['event-factory.h']
    )
aura.add_input(t)

#-------------------------------------------------------------------------------
# DEVICES

t = m.add_compile_target(
        output='device-common.o',
        inputs=['device-common.c'],
        includes=['device-common.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='device-fb.o',
        inputs=['device-fb.c'],
        includes=['device-fb.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='device-drm.o',
        inputs=['device-drm.c'],
        includes=['device-drm.h'],
        pkgs={'libdrm', 'gbm', 'egl'}
    )
aura.add_input(t)

t = m.add_compile_target(
        output='device-evdev.o',
        inputs=['device-evdev.c'],
        includes=['device-evdev.h'],
        pkgs={'libudev'}
    )
aura.add_input(t)

t = m.add_compile_target(
        output='device-udev.o',
        inputs=['device-udev.c'],
        includes=['device-udev.h'],
        pkgs={'libudev'}
    )
aura.add_input(t)

#-------------------------------------------------------------------------------
# OUTPUT

t = m.add_compile_target(
        output='output.o',
        inputs=['output.c'],
        includes=['output.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='output-collector.o',
        inputs=['output-collector.c'],
        includes=['output-collector.h']
    )
aura.add_input(t)

#-------------------------------------------------------------------------------
#

t = m.add_compile_target(
        output='surface-data.o',
        inputs=['surface-data.c'],
        includes=['surface-data.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='surface-compositor.o',
        inputs=['surface-compositor.c'],
        includes=['surface-compositor.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='surface-manager.o',
        inputs=['surface-manager.c'],
        includes=['surface-manager.h']
    )
aura.add_input(t)

#-------------------------------------------------------------------------------
# KEYBOARD

t = m.add_compile_target(
        output='keyboard-bindings.o',
        inputs=['keyboard-bindings.c'],
        includes=['keyboard-bindings.h']
    )
aura.add_input(t)

#-------------------------------------------------------------------------------
#

t = m.add_compile_target(
        output='renderer-mmap.o',
        inputs=['renderer-mmap.c'],
        includes=['renderer-mmap.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='renderer-gl.o',
        inputs=['renderer-gl.c'],
        includes=['renderer-gl.h'],
        pkgs={'gl'}
    )
aura.add_input(t)

#-------------------------------------------------------------------------------
#

t = m.add_compile_target(
        output='wayland.o',
        inputs=['wayland.c'],
        includes=['wayland.h', target_xdg_shell_protocol],
        pkgs={'wayland-server'}
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-state.o',
        inputs=['wayland-state.c'],
        includes=['wayland-state.h'],
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-compositor.o',
        inputs=['wayland-protocol-compositor.c'],
        includes=['wayland-protocol-compositor.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-surface.o',
        inputs=['wayland-protocol-surface.c'],
        includes=['wayland-protocol-surface.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-region.o',
        inputs=['wayland-protocol-region.c'],
        includes=['wayland-protocol-region.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-shell.o',
        inputs=['wayland-protocol-shell.c'],
        includes=['wayland-protocol-shell.h', target_xdg_shell_protocol]
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-shell-surface.o',
        inputs=['wayland-protocol-shell-surface.c'],
        includes=['wayland-protocol-shell-surface.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-xdg-shell.o',
        inputs=['wayland-protocol-xdg-shell.c'],
        includes=['wayland-protocol-xdg-shell.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='xdg-shell-protocol.o',
        inputs=['xdg-shell-protocol.c'],
        input_dir=m.get_gen_directory()[0]
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-output.o',
        inputs=['wayland-protocol-output.c'],
        includes=['wayland-protocol-output.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-seat.o',
        inputs=['wayland-protocol-seat.c'],
        includes=['wayland-protocol-seat.h'],
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-keyboard.o',
        inputs=['wayland-protocol-keyboard.c'],
        includes=['wayland-protocol-keyboard.h']
    )
aura.add_input(t)

#-------------------------------------------------------------------------------
#

t = m.add_compile_target(
        output='bind-egl-wayland.o',
        inputs=['bind-egl-wayland.c'],
        includes=['bind-egl-wayland.h']
    )
aura.add_input(t)

#-------------------------------------------------------------------------------
# GTK BACKEND

if with_gtk_support:
    t = m.add_compile_target(
            output='backend-gtk.o',
            inputs=['backend-gtk.c'],
            includes=['backend-gtk.h'],
            pkgs={'gtk+-3.0'}
        )
    aura.add_input(t)

    t = m.add_compile_target(
            output='backend-gtk-app.o',
            inputs=['backend-gtk-app.c'],
            includes=['backend-gtk-app.h'],
            pkgs={'gtk+-3.0'}
        )
    aura.add_input(t)

    t = m.add_compile_target(
            output='backend-gtk-win.o',
            inputs=['backend-gtk-win.c'],
            includes=['backend-gtk-win.h'],
            pkgs={'gtk+-3.0'}
        )
    aura.add_input(t)

    t = m.add_compile_target(
            output='backend-gtk-res.o',
            inputs=['backend-gtk-res.c'],
            input_dir=m.get_gen_directory()[0],
            pkgs={'gtk+-3.0'}
        )
    aura.add_input(t)

else:
    t = m.add_compile_target(
            output='backend-gtk.o',
            inputs=['backend-gtk-dummy.c'],
            includes=['backend-gtk.h'],
        )
    aura.add_input(t)

#-------------------------------------------------------------------------------

if __name__ == '__main__':
    m.run(sys.argv[1:])

