#!/bin/sh

# make generated files
mkdir -p gen
wayland-scanner server-header res/xdg-shell.xml gen/xdg-shell-server-protocol.h
wayland-scanner server-header res/screenshooter.xml gen/screenshooter-server-protocol.h
wayland-scanner client-header res/screenshooter.xml gen/screenshooter-client-protocol.h
wayland-scanner code res/xdg-shell.xml gen/xdg-shell-protocol.c
wayland-scanner code res/screenshooter.xml gen/screenshooter-protocol.c
glib-compile-resources res/metanoiactl.gresource.xml --target=gen/controller-gtk-res.c --generate-source
python3 -B ./share/build/make.py version

# prepare build system
python3 -B ./share/build/make.py make
python3 -B ./share/build/make.py ninja

