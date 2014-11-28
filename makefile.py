#!/usr/bin/python
# file: makefile.py
# vim: tabstop=4 expandtab colorcolumn=81 list

import sys
import make

m = make.Make()

m.set_source_directory('src')
m.set_build_directory('build')
m.set_gen_directory('gen')

m.set_wflags(['-Wall'])
m.set_lflags(['-rdynamic', '-ldl', '-lrt', '-lpthread', '-lm'])

#-------------------------------------------------------------------------------

aura = m.add_link_target(
        output='aura',
        include_in_all=True
    )

#-------------------------------------------------------------------------------
# PROTOCOL

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

target_xdg_shell_protocol = m.add_generated_target(
        generator=wsshg,
        output='xdg-shell-server-protocol.h',
        inputs=['xdg-shell.xml'],
        input_dir='protocol'
    )

target_xdg_shell_protocol_code = m.add_generated_target(
        generator=wscg,
        output='xdg-shell-protocol.c',
        inputs=['xdg-shell.xml'],
        input_dir='protocol'
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
        output='configuration-functions.o',
        inputs=['configuration-functions.c'],
        includes=['configuration-functions.h']
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
        output='utils-log.o',
        inputs=['utils-log.c'],
        includes=['utils-log.h']
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
        output='event-task-factory.o',
        inputs=['event-task-factory.c'],
        includes=['event-task-factory.h']
    )
aura.add_input(t)

#-------------------------------------------------------------------------------
#

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
#

t = m.add_compile_target(
        output='surface-aggregator.o',
        inputs=['surface-aggregator.c'],
        includes=['surface-aggregator.h']
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
        output='wayland-compositor.o',
        inputs=['wayland-compositor.c'],
        includes=['wayland-compositor.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-surface.o',
        inputs=['wayland-surface.c'],
        includes=['wayland-surface.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-region.o',
        inputs=['wayland-region.c'],
        includes=['wayland-region.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-shell.o',
        inputs=['wayland-shell.c'],
        includes=['wayland-shell.h', target_xdg_shell_protocol]
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-shell-surface.o',
        inputs=['wayland-shell-surface.c'],
        includes=['wayland-shell-surface.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-xdg_shell.o',
        inputs=['wayland-xdg_shell.c'],
        includes=['wayland-xdg_shell.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='xdg-shell-protocol.o',
        inputs=['xdg-shell-protocol.c'],
        input_dir=m.get_gen_directory()[0]
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-output.o',
        inputs=['wayland-output.c'],
        includes=['wayland-output.h']
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-seat.o',
        inputs=['wayland-seat.c'],
        includes=['wayland-seat.h'],
        pkgs={'xkbcommon'}
    )
aura.add_input(t)

t = m.add_compile_target(
        output='wayland-keyboard.o',
        inputs=['wayland-keyboard.c'],
        includes=['wayland-keyboard.h']
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

if __name__ == '__main__':
    m.run(sys.argv[1:])

