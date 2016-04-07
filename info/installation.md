Installation
============

Build
-----

Metanoia uses its home-grown (python) script generating make files or ninja
files (could not find any sufficiently flexible and clean build system). Default
Makefile and build.ninja files are provided in git so compilation takes only:

```
make
```

or

```
ninja
```

Metanoia does not yet provide packages.

Dependences
-----------

Required runtime dependences:

 * libdbus
 * libdrm
 * libgbm
 * libgl, libegl
 * libjpeg
 * libudev
 * libwayland
 * libxkbcommon

Optional runtime dependences:

 * glib-2.0
 * gtk+-3.0

Buildtime dependences:

 * make
 * ninja
 * python3

Configuration
-------------

Configuration is wider topic described in [configuration.md](./configuration.md).

Run
---

To run metanoia switch to Linux console e.g. using ctrl+alt+F6
(make sure it's kernel console, not userspace console like kmscon).

For GPU acceleration metanoia requires drivers which support KMS (Kernel Mode
Setting). Without acceleration performance will be very poor.

In case of **any** problems please make bug report in
[bugtracker](https://github.com/metacenter/metanoia/issues).  Providing logs
will be helpful. They can be found in `$XDG_RUNTIME_DIR/noia-XXXXXX/log` where
`XXXXXX` is string randomly generated for each metanoia instance (you will
probably be able to figure out correct one by last access time).

For list of options and environment variables you can refer to [manual
page](./manual.adoc).

