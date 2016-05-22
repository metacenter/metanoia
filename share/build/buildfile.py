#!/usr/bin/python3

import build
from build import GeneratedTarget  as Gen
from build import CompileTarget    as Com
from build import TestTarget       as Tst
from build import ExecutableTarget as Exe
from build import CheckTarget      as Chk
from build import PhonyTarget      as Pny

#-------------------------------------------------------------------------------
# CONFIGURATION

c = build.ConfigClangDebug()
c.add_cflags(['-Wpedantic', '-std=gnu11'])
c.set_lflags(['-rdynamic', '-ldl', '-lrt', '-lpthread', '-lm'])

p = build.Project(c)
p.add_clean_commands(['doc', 'callgrind*', '*plist*', '.ninja*'])

#-------------------------------------------------------------------------------
# APPLICATIONS

metanoia        = Exe(output='metanoia')
metanoiactl_gtk = Exe(output='metanoiactl-gtk')

#-------------------------------------------------------------------------------
# GENERATORS

wsshg = build.Generator('wsshg',
              ['wayland-scanner', 'server-header', '{inputs}', '{output}'])

wschg = build.Generator('wschg',
              ['wayland-scanner', 'client-header', '{inputs}', '{output}'])

wscg = build.Generator('wscg',
             ['wayland-scanner', 'code', '{inputs}', '{output}'])

glcr = build.Generator('glcr',
             ['glib-compile-resources', '{inputs}',
              '--target={output}', '--generate-source'])

vgen = build.Generator('vgen',
             ['python3', '-B', './share/build/make.py', 'version'])

#-------------------------------------------------------------------------------
# GENERATED FILES

target_version_info = Gen(vgen, output=None, alias='version_info_file')

target_xdg_shell_server = Gen(
        generator=wsshg,
        inputs=['xdg-shell.xml'],
        output='xdg-shell-server-protocol.h',
    )

target_xdg_shell_code = Gen(
        generator=wscg,
        inputs=['xdg-shell.xml'],
        output='xdg-shell-protocol.c',
    )

target_screenshooter_server = Gen(
        generator=wsshg,
        inputs=['screenshooter.xml'],
        output='screenshooter-server-protocol.h',
    )

target_screenshooter_client = Gen(
        generator=wschg,
        inputs=['screenshooter.xml'],
        output='screenshooter-client-protocol.h',
    )

target_screenshooter_code = Gen(
        generator=wscg,
        inputs=['screenshooter.xml'],
        output='screenshooter-protocol.c',
    )

target_gtk_resources = Gen(
        generator=glcr,
        inputs=['metanoiactl.gresource.xml'],
        output='controller-gtk-res.c',
        deps=['metanoiactl-gtk-main.ui'],
    )

p.add([target_xdg_shell_server,
       target_screenshooter_server,
       target_screenshooter_client])

#-------------------------------------------------------------------------------
# PHONY TARGETS

generated_files = [target_version_info,
                   target_xdg_shell_server,
                   target_xdg_shell_code,
                   target_screenshooter_server,
                   target_screenshooter_client,
                   target_screenshooter_code,
                   target_gtk_resources]

p.add(Pny(build.Generator('memcheck',
      ['python3', '-B', './share/build/make.py', 'memcheck']),
      deps=['checks']))

p.add(Pny(build.Generator('cppcheck',
      ['python3', '-B', './share/build/make.py', 'cppcheck']),
      deps=['checks']))

p.add(Pny(build.Generator('make',
      ['python3', '-B', './share/build/make.py', 'make']),
      deps=generated_files))

p.add(Pny(build.Generator('ninja',
      ['python3', '-B', './share/build/make.py', 'ninja']),
      deps=generated_files))

p.add(Pny(build.Generator('install',
      ['python3', '-B', './share/build/make.py', 'install'])))

#-------------------------------------------------------------------------------
# CONFIGURATION

metanoia.add([Com(['config.c'])])

#-------------------------------------------------------------------------------
# GLOBALS

target_global_enums     = Com(['global-enums.c'])
target_global_types     = Com(['global-types.c'])
target_global_objects   = Com(['global-objects.c'])

metanoia.add([target_global_enums,
              target_global_types,
              target_global_objects])

#-------------------------------------------------------------------------------
# UTILS

target_utils_debug          = Com(['utils-debug.c'])
target_utils_object         = Com(['utils-object.c'])
target_utils_pool           = Com(['utils-pool.c'])
target_utils_store          = Com(['utils-store.c'])
target_utils_chain          = Com(['utils-chain.c'])
target_utils_list           = Com(['utils-list.c'])
target_utils_branch         = Com(['utils-branch.c'])
target_utils_log            = Com(['utils-log.c'])
target_utils_environment    = Com(['utils-environment.c'])
target_utils_dbus           = Com(['utils-dbus.c'],          pkgs={'dbus-1'})
target_utils_gl             = Com(['utils-gl.c'],            pkgs={'gl', 'egl'})
target_utils_keymap         = Com(['utils-keymap.c'],        pkgs={'xkbcommon'})
target_utils_keyboard_state = Com(['utils-keyboard-state.c'],pkgs={'xkbcommon'})
target_utils_image          = Com(['utils-image.c'],         libs={'jpeg'})
target_utils_time           = Com(['utils-time.c'])

metanoia.add([target_utils_debug,
              target_utils_object,
              target_utils_pool,
              target_utils_store,
              target_utils_chain,
              target_utils_list,
              target_utils_branch,
              target_utils_log,
              target_utils_environment,
              target_utils_dbus,
              target_utils_gl,
              target_utils_keymap,
              target_utils_keyboard_state,
              target_utils_image,
              target_utils_time])

#-------------------------------------------------------------------------------
# EVENTS

metanoia.add(Com.from_matching('event-*.c', c))

#-------------------------------------------------------------------------------
# SURFACE

metanoia.add(Com.from_matching('surface-*.c', c))

#-------------------------------------------------------------------------------
# RENDERERS

metanoia.add([Com(['renderer.c']),
              Com(['renderer-mmap.c']),
              Com(['renderer-gl.c'], pkgs={'gl'})])

#-------------------------------------------------------------------------------
# DEVICES

metanoia.add([Com(['device-common.c']),
              Com(['device-fb.c']),
              Com(['device-drm.c'],   pkgs={'libdrm', 'gbm', 'egl'}),
              Com(['device-evdev.c'], pkgs={'libudev'}),
              Com(['device-udev.c'],  pkgs={'libudev'})])

#-------------------------------------------------------------------------------
# OUTPUT

metanoia.add([Com(['output.c']),
              Com(['output-collector.c'])])

#-------------------------------------------------------------------------------
# INPUT

metanoia.add(Com.from_matching('input-*.c', c))

#-------------------------------------------------------------------------------
# EXHIBITOR

target_exhibitor_frame          = Com(['exhibitor-frame.c'])
target_exhibitor_frame_internal = Com(['exhibitor-frame-internal.c'])

metanoia.add([target_exhibitor_frame,
              target_exhibitor_frame_internal,
              Com(['exhibitor-strategist.c']),
              Com(['exhibitor-compositor.c']),
              Com(['exhibitor-display.c']),
              Com(['exhibitor-pointer.c']),
              Com(['exhibitor.c']),
              Com(['exhibitor-module.c'])])

#-------------------------------------------------------------------------------
# WAYLAND FRONTEND

metanoia.add([Com(['wayland-region.c']),
              Com(['wayland-surface.c']),
              Com(['wayland-output.c']),
              Com(['wayland-cache.c']),
              Com(['wayland-state.c'], deps={target_xdg_shell_server}),
              Com(['wayland.c'],       deps={target_screenshooter_server},
                                       pkgs={'wayland-server'}),
              Com([target_xdg_shell_code]),
              Com([target_screenshooter_code])])

metanoia.add(Com.from_matching('wayland-protocol-*.c', c))

#-------------------------------------------------------------------------------
# OFFSCREEN BACKEND

metanoia.add([Com(['backend-offscreen.c'])])

#-------------------------------------------------------------------------------
# MAIN

metanoia.add([Com(['metanoia.c'])])

#-------------------------------------------------------------------------------
# METANOIACTL

metanoiactl_gtk.add([target_utils_debug,
                     target_utils_time,
                     target_utils_log,
                     target_utils_environment,
                     target_screenshooter_code])
metanoiactl_gtk \
    .add([Com(['controller-defs.c'],        pkgs={'glib-2.0'}),
          Com(['controller-output.c'],      pkgs={'glib-2.0'}),
          Com([target_gtk_resources],       pkgs={'glib-2.0'}),
          Com(['controller-gtk-display.c'], pkgs={'gtk+-3.0'}),
          Com(['controller-gtk-win.c'],     pkgs={'gtk+-3.0'}),
          Com(['controller-gtk-app.c'],     pkgs={'gtk+-3.0'}),
          Com(['controller-wayland.c'],     pkgs={'glib-2.0', 'wayland-client'},
                                            deps={target_screenshooter_client}),
          Com(['metanoiactl-gtk.c'],        pkgs={'gtk+-3.0'})])

#-------------------------------------------------------------------------------
# TESTS

target_test_globals = Tst(['test-globals.c'])
target_test_pool    = Tst(['test-pool.c'])
target_test_chain   = Tst(['test-chain.c'])
target_test_list    = Tst(['test-list.c'])
target_test_store   = Tst(['test-store.c'])
target_test_branch  = Tst(['test-branch.c'])
target_test_frame   = Tst(['test-frame.c'])

target_mock_surface_coordinator = Tst(['mock-surface-coordinator.c'])

p.add(Chk(output='check-globals',
          inputs=[target_test_globals, target_global_types]))

p.add(Chk(output='check-pool',
          inputs=[target_test_pool, target_utils_debug, target_utils_pool]))

p.add(Chk(output='check-store',
          inputs=[target_utils_debug, target_test_store, target_utils_store]))

p.add(Chk(output='check-chain',
          inputs=[target_test_chain, target_utils_chain]))

p.add(Chk(output='check-list',
          inputs=[target_test_list, target_utils_debug,
                  target_utils_chain, target_utils_list]))

p.add(Chk(output='check-branch',
          inputs=[target_test_branch, target_utils_chain, target_utils_branch]))

p.add(Chk(output='check-frame',
          inputs=[target_test_frame, target_mock_surface_coordinator,
                  target_utils_pool, target_utils_debug,
                  target_utils_store, target_utils_chain, target_utils_branch,
                  target_global_enums, target_global_types,
                  target_exhibitor_frame, target_exhibitor_frame_internal]))

#-------------------------------------------------------------------------------
# MAIN TARGETS

p.add(target_version_info, include_in_default=True)
p.add(metanoia,            include_in_default=True)
p.add(metanoiactl_gtk,     include_in_default=True)

#-------------------------------------------------------------------------------

