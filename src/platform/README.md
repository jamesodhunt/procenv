# `procenv` drivers

## Overview

`procenv` now supports platform drivers. These are simply translation
units that are compiled only for a particular platform. They were
introduced to combat the "`#ifdef` hell" of previous releases.

## Build Strategy

### `configure.ac`

Used to identify the platform the build is run on, exporting
`PROCENV_PLATFORM_<name>` and `$procenv_platform"` to `src/Makefile.am`.

### `src/Makefile.am`

- Defines `PROCENV_PLATFORM_<name>` as a symbol to the compiler.

- Adds platform driver (platform-specific) source files to the list of
  files to be built.

To simplify the include policy, all platform-specific headers are added to the
appropriate section in `platform-headers.h`.

Additionally, one other object get built on *all* platforms:

- `platform-generic`

  Provides generic Unix/Linux implementations of particular methods.

  Drivers are free to reference the functions from this translation unit.
  See below for details.

The build uses some magic options so that although all possible
implementations get built, *only those symbols actually referenced become part
of the final binary* (meaning you don't end up with lots of generic functions
linked into your binary if your driver doesn't use them :-)

## Implementing a new driver

- `configure.ac`:
   - Update the `$target_to_consider` case logic for your platform.
   - Add an `AM_CONDITIONAL(PROCENV_PLATFORM_*, ...)`.

- Create `src/platform/${platform}/platform.c` with an implementation of
  the global "`struct procenv_ops platform_ops`".

  For each `struct procenv_ops` function pointer, 

  - If it doesn't make sense to define a particular function for the
    platform, simply omit specifying a function pointer.

    (Note that you could set the function pointer to `NULL`, but that
    isn't necessary given that the `platform_ops` symbol is global, and
    thus effectively all pointers not specified are already implicitly set
    to `NULL`).

  - If one of the generic implementations is suitable, set the function
    pointer to the corresponding `*_generic()` function in
    `platform-generic.c`.

  - If neither of the above options is suitable, define a static
    implementation in your `platform.c` (with the function name
    `*_<platform>`) file and set the corresponding function pointer to
    that function.

- Create `src/platform/${platform}/platform-${platform}.h`.
