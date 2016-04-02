Contributing
============

You probably have read [installation.md](./installation.md) and know how to build,
run and configure Metanoia. Have suggestions? Want to help? Here's how to:

 * **bug reports**

   All users are encouraged to fill bug reports for everything: bugs, features,
   suggestions, request for more documentation, changes in documentation,
   enhancements... whatever that should be done.

   Bugtracker can be found on [github](https://github.com/metacenter/metanoia/issues).

 * **idea selling**

   Any idea for improvements, feature proposals are very welcome. Metanoia is in
   very early stage of development, so ground breaking ideas are still relatively
   easy to implement. On the other hand feature you need may be already planned. On
   github you can brainstorm the ideas and vote for issues you are interested in to
   indicate they should be implemented with higher priority.

 * **artworks**

 * **coding**

   C (gnu11 standard) was chosen as base language. (C++ was good alternative as
   C is not best language for creating business logic but everything around
   metanoia uses C so there was no point in making wrappers for whole world).

   There is plan to be able to write configuration and plugins in Python or Lua.

   Source code is available on [github](https://github.com/metacenter/metanoia)
   and can be fetched using command
   ```
   git clone git@github.com:metacenter/metanoia.git
   ```

   If you need quick overview of architecture see [concepts.md](./concepts.md).

Later parts of this file focus on tips for developers.

Directories
-----------

Here is description of directories in repo:

 * **src** - all hand-written applications source code

 * **test** - source code of unit tests

 * **res** - all source non-code (xml, ui, gtk resources) used during
   compilation

 * **info** - hand-written documentation

 * **share** - other non-binary non-code files

These directories are created during build:

 * **build** - all target binaries (programs, libraries)

 * **inter** - intermediate binaries

 * **gen** - generated source code (version file, wayland protocols, etc.)

 * **check** - unit test binaries

 * **doc** - doxygen documentation

 * **install** - default install destination

Run unit tests
--------------

Running unit test is as simple as

```
make check
```

If you want unit test run under Valgrind there is

```
make memcheck
```

Generate doxygen documentation
------------------------------

Doxyfile is provided in top directory. Command

```
doxygen
```

will produce documentation for all sources and unit tests in `doc/` directory.

Code conventions
----------------

Using code conventions is important because it homogenizes source code making it
more predictable for the reader and therefore easier to understand.

Here are some conventions used in metanoia:

 * keep line width up to 80,
 * keep file length up to 999 lines,
 * use
   - `lower_snake_case` for function, variable and member names,
   - `UpperCamelCase` for data types and structure names,
   - `lowerCamelCase` for global variables and constants,
   - `UPPER_SNAKE_CASE` for macro names,
 * use four spaces for indentation (tabs make no sense with line width limit),
 * do not use block comments (these are reserved for debugging),
 * divide functions with horizontal rule (this greatly increases readability),
 * local headers always included before global,
 * even if code is clear use comments to separate code blocks visually,
 * document everything with doxygen (use "@"; place comments in headers),

Git workflow and versioning
---------------------------

Metanoia is hosted on GitHub and uses GitHub flow (pull requests) but patches
sent by mail will also be accepted.

Active development takes place on subbranches, on `master` land only complete
features. Every release has version and codename. Version is used to tag commit.
Code name is used for branch. If bugs were found fixes are make on this branch,
then are merged to master (never other way).

Version string consists of three numbers: EPOCH.MAJOR.MINOR
 * MINOR - bug-fixed release
 * MAJOR - normal release (codenamed)
 * EPOCH - version containing revolutionary changes opening new chapter in
   history

Make sure every commit compiles - this may make life easier when searching for
commit that introduced bug.

Future
------

List of features planed for nearest releases can be found on
[wiki TODO page](https://github.com/metacenter/metanoia/wiki/TODO).

