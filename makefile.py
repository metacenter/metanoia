#!/usr/bin/python -B
# file: makefile.py
# vim: tabstop=4 expandtab colorcolumn=81 list

import sys
import make

m = make.Make()

m.set_source_directory('src')
m.set_build_directory('build')
m.set_resource_directory('res')
m.set_gen_directory('gen')

m.set_c_compiler('gcc')
m.set_oflags(['-O3']) # release
m.set_oflags(['-DDEBUG', '-g', '-O0']) # debug
m.set_cflags(['-std=gnu11', '-Wall', '-W', '-Wextra', '-Wpedantic', '-Werror'])
m.set_lflags(['-rdynamic', '-ldl', '-lrt', '-lpthread', '-lm'])

#-------------------------------------------------------------------------------

with_gtk_support = True

#-------------------------------------------------------------------------------

metanoia = m.add_link_target(
        output='metanoia',
        include_in_all=True,
    )

#-------------------------------------------------------------------------------
# GENERATORS

wsshg = make.Generator(
        command_body='wayland-scanner server-header',
        command_args=' < "{inputs}" > "{output}"',
    )
m.add_generator(wsshg)

wscg = make.Generator(
        command_body='wayland-scanner code',
        command_args=' < "{inputs}" > "{output}"',
    )
m.add_generator(wscg)

glcr = make.Generator(
        command_body='glib-compile-resources',
        command_args='{inputs} --target={output} --generate-source',
    )
m.add_generator(glcr)

vgen = make.Generator(
        command_body='desc=`git describe --always`;',
        command_args='if ! grep -q "$$desc" gen/version.h 2> /dev/null; '
                     'then echo -e "#ifndef NOIA_VERSION\\n'
                     '#define NOIA_VERSION \\"$$desc\\"\\n#endif\\n"'
                     ' > gen/version.h; fi'
    )

#-------------------------------------------------------------------------------
# RESOUCES

target_xdg_shell_protocol = m.add_generated_target(
        generator=wsshg,
        output='xdg-shell-server-protocol.h',
        inputs=['xdg-shell.xml'],
    )

target_xdg_shell_protocol_code = m.add_generated_target(
        generator=wscg,
        output='xdg-shell-protocol.c',
        inputs=['xdg-shell.xml'],
    )

target_gtk_resources = m.add_generated_target(
        generator=glcr,
        output='backend-gtk-res.c',
        inputs=['metanoia.gresource.xml'],
        deps=['backend-gtk-main.ui',
              'backend-gtk-menu.ui',
              'backend-gtk-area.ui'],
    )

version_file = m.add_generated_target(
        generator=vgen,
        output='version.h',
        deps=['force']
    )

#-------------------------------------------------------------------------------
# CONFIGURATION

t = m.add_compile_target(
        output='config.o',
        inputs=['config.c'],
    )
metanoia.add_input(t)

#-------------------------------------------------------------------------------
# GLOBALS

t = m.add_compile_target(
        output='global-objects.o',
        inputs=['global-objects.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='global-functions.o',
        inputs=['global-functions.c'],
    )
metanoia.add_input(t)

#-------------------------------------------------------------------------------
# UTILS

t = m.add_compile_target(
        output='utils-object.o',
        inputs=['utils-object.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='utils-chain.o',
        inputs=['utils-chain.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='utils-list.o',
        inputs=['utils-list.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='utils-branch.o',
        inputs=['utils-branch.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='utils-store.o',
        inputs=['utils-store.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='utils-dbus.o',
        inputs=['utils-dbus.c'],
        pkgs={'dbus-1'}
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='utils-keymap.o',
        inputs=['utils-keymap.c'],
        pkgs={'xkbcommon'}
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='utils-log.o',
        inputs=['utils-log.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='utils-environment.o',
        inputs=['utils-environment.c'],
    )
metanoia.add_input(t)

#-------------------------------------------------------------------------------
# EVENTS

t = m.add_compile_target(
        output='event-dispatcher.o',
        inputs=['event-dispatcher.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='event-timer.o',
        inputs=['event-timer.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='event-signals.o',
        inputs=['event-signals.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='event-loop.o',
        inputs=['event-loop.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='event-task.o',
        inputs=['event-task.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='event-factory.o',
        inputs=['event-factory.c'],
    )
metanoia.add_input(t)

#-------------------------------------------------------------------------------
# RENDERERS

t = m.add_compile_target(
        output='renderer.o',
        inputs=['renderer.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='renderer-mmap.o',
        inputs=['renderer-mmap.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='renderer-gl.o',
        inputs=['renderer-gl.c'],
        pkgs={'gl'}
    )
metanoia.add_input(t)

#-------------------------------------------------------------------------------
# DEVICES

t = m.add_compile_target(
        output='device-common.o',
        inputs=['device-common.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='device-fb.o',
        inputs=['device-fb.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='device-drm.o',
        inputs=['device-drm.c'],
        pkgs={'libdrm', 'gbm', 'egl'}
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='device-evdev.o',
        inputs=['device-evdev.c'],
        pkgs={'libudev'}
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='device-udev.o',
        inputs=['device-udev.c'],
        pkgs={'libudev'}
    )
metanoia.add_input(t)

#-------------------------------------------------------------------------------
# OUTPUT

t = m.add_compile_target(
        output='output.o',
        inputs=['output.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='output-collector.o',
        inputs=['output-collector.c'],
    )
metanoia.add_input(t)

#-------------------------------------------------------------------------------
#

t = m.add_compile_target(
        output='surface-data.o',
        inputs=['surface-data.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='surface-manager.o',
        inputs=['surface-manager.c'],
    )
metanoia.add_input(t)

#-------------------------------------------------------------------------------
# KEYBOARD

t = m.add_compile_target(
        output='keyboard-functions.o',
        inputs=['keyboard-functions.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='keyboard-binding.o',
        inputs=['keyboard-binding.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='keyboard-argmand.o',
        inputs=['keyboard-argmand.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='keyboard-bindings.o',
        inputs=['keyboard-bindings.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='keyboard-mode.o',
        inputs=['keyboard-mode.c'],
    )
metanoia.add_input(t)

#-------------------------------------------------------------------------------
# EXHIBITOR

t = m.add_compile_target(
        output='exhibitor.o',
        inputs=['exhibitor.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='exhibitor-display.o',
        inputs=['exhibitor-display.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='exhibitor-compositor.o',
        inputs=['exhibitor-compositor.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='exhibitor-frame.o',
        inputs=['exhibitor-frame.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='exhibitor-strategist.o',
        inputs=['exhibitor-strategist.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='exhibitor-pointer.o',
        inputs=['exhibitor-pointer.c'],
    )
metanoia.add_input(t)

#-------------------------------------------------------------------------------
# WAYLAND FRONTEND

t = m.add_compile_target(
        output='wayland-region.o',
        inputs=['wayland-region.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-surface.o',
        inputs=['wayland-surface.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-output.o',
        inputs=['wayland-output.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-cache.o',
        inputs=['wayland-cache.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-state.o',
        inputs=['wayland-state.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland.o',
        inputs=['wayland.c'],
        pkgs={'wayland-server'}
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-compositor.o',
        inputs=['wayland-protocol-compositor.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-surface.o',
        inputs=['wayland-protocol-surface.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-region.o',
        inputs=['wayland-protocol-region.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-shell.o',
        inputs=['wayland-protocol-shell.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-shell-surface.o',
        inputs=['wayland-protocol-shell-surface.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-xdg-shell.o',
        inputs=['wayland-protocol-xdg-shell.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-xdg-surface.o',
        inputs=['wayland-protocol-xdg-surface.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='xdg-shell-protocol.o',
        deps=['xdg-shell-protocol.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-output.o',
        inputs=['wayland-protocol-output.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-seat.o',
        inputs=['wayland-protocol-seat.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-pointer.o',
        inputs=['wayland-protocol-pointer.c'],
    )
metanoia.add_input(t)

t = m.add_compile_target(
        output='wayland-protocol-keyboard.o',
        inputs=['wayland-protocol-keyboard.c'],
    )
metanoia.add_input(t)

#-------------------------------------------------------------------------------
#

t = m.add_compile_target(
        output='bind-egl-wayland.o',
        inputs=['bind-egl-wayland.c'],
    )
metanoia.add_input(t)

#-------------------------------------------------------------------------------
# GTK BACKEND

if with_gtk_support:
    t = m.add_compile_target(
            output='backend-gtk-res.o',
            deps=['backend-gtk-res.c'],
            pkgs={'gtk+-3.0'}
        )
    metanoia.add_input(t)

    t = m.add_compile_target(
            output='backend-gtk-output.o',
            inputs=['backend-gtk-output.c'],
            pkgs={'gtk+-3.0'}
        )
    metanoia.add_input(t)

    t = m.add_compile_target(
            output='backend-gtk-group.o',
            inputs=['backend-gtk-group.c'],
            pkgs={'gtk+-3.0'}
        )
    metanoia.add_input(t)

    t = m.add_compile_target(
            output='backend-gtk-win.o',
            inputs=['backend-gtk-win.c'],
            pkgs={'gtk+-3.0'}
        )
    metanoia.add_input(t)

    t = m.add_compile_target(
            output='backend-gtk-app.o',
            inputs=['backend-gtk-app.c'],
            pkgs={'gtk+-3.0'}
        )
    metanoia.add_input(t)

    t = m.add_compile_target(
            output='backend-gtk.o',
            inputs=['backend-gtk.c'],
            pkgs={'gtk+-3.0'}
        )
    metanoia.add_input(t)

else:
    t = m.add_compile_target(
            output='backend-gtk-dummy.o',
            inputs=['backend-gtk-dummy.c'],
        )
    metanoia.add_input(t)

#-------------------------------------------------------------------------------
# MAIN

t = m.add_compile_target(
        output='metanoia.o',
        inputs=['metanoia.c']
    )
metanoia.add_input(t)

#-------------------------------------------------------------------------------
# TESTS

m.add_test(output='check-chain',
           inputs=['test-chain.c'],
           deps=['utils-chain.c'])

m.add_test(output='check-list',
           inputs=['test-list.c'],
           deps=['utils-chain.c', 'utils-list.c'])

m.add_test(output='check-branch',
           inputs=['test-branch.c'],
           deps=['utils-chain.c', 'utils-branch.c'])

m.add_test(output='check-store',
           inputs=['test-store.c'],
           deps=['utils-chain.c', 'utils-store.c'])

#-------------------------------------------------------------------------------

if __name__ == '__main__':
    m.run(sys.argv[1:])

