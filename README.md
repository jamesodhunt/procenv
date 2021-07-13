[![GPLv3 license](https://img.shields.io/badge/License-GPLv3-blue.svg)](http://perso.crans.org/besson/LICENSE.html)
[![Build Status](https://travis-ci.org/jamesodhunt/procenv.svg?branch=master)](https://travis-ci.org/jamesodhunt/procenv)
[![Coverity](https://img.shields.io/coverity/scan/jamesodhunt-procenv)](https://scan.coverity.com/projects/jamesodhunt-procenv)
[![Flattr](https://img.shields.io/badge/donate-flattr-blue.svg)](https://flattr.com/profile/jamesodhunt)
[![PayPal](https://img.shields.io/badge/paypal-donate-blue.svg)](https://www.paypal.me/jamesodhunt)

# `procenv`

## Demo

[![demo](https://asciinema.org/a/118278.svg)](https://asciinema.org/a/118278?autoplay=1)

## Overview

`procenv` is a simple command-line utility, written in C and licensed
under the GPL, that dumps all attributes of the environment in which it
runs, in well-structured plain ASCII, JSON (YAML), XML or CSV.

> **Note:**
>
> If you find anything missing from the `procenv` output, please either raise
> an issue or send a patch :)

## Why?

When you first read what this tool does, you tend to think, _"Huh?"_,
or _"Why would anyone want a tool like that?"_ or _"How is that useful?"_

Here are a few examples to help explain. `procenv` can help...

- To learn about the Unix/Linux environment

  For example, compare these output files:

  ```bash
  $ procenv > /tmp/1
  $ sudo procenv > /tmp/2
  $ nohup procenv >/tmp/3 &
  ```

- To debug weird CI failures

  Even scratched your head wondering why your code works perfectly on your
  local machine, but explodes in flames in your CI system?

  Maybe you've had to hack your build to run a bunch of shell commands like
  the following to help your debug wtf is going wrong?:

  ```bash
  echo "FIXME: environment variables"
  env

  echo "FIXME: mounts"
  mount

  echo "FIXME: time"
  date

  echo "FIXME: umask"
  umask
  ```

  Look familiar? There is a simpler way: just run `procenv` in your CI, run
  it locally and `diff(1)` the output!

- To compare build environments

  When you run `make check` as part of the `procenv` build, the `procenv`
  binary runs. This is useful because if you can access the build logs for a
  particular build system, you can see the environment _of_ that build system
  by looking at the procenv logs.

  > **Note:** See the [results](#results) section for links to some common
  > build environments.

- To compare systems

   More generally, just run procenv on two systems and `diff(1)` the output to
   see what's different (different O/S versions, different distro versions,
   different Unix variants).

- To learn about the packaging environment:

  These commands show interesting details of compiler and libc used to build
  the package:

  ```bash
  $ procenv --compiler
  $ procenv --libc
  ```

- To see what changes for a process on each run

  Every new process creates gets a new process ID. But what else changes?

  ```bash
  $ diff <(procenv) <(procenv)
  ```

### Party trick

Since `procenv` can re-exec itself, you can do sneaky things like run
`procenv` _before_ the init daemon runs on your system! But wait, init systems
like `systemd` run as PID 1 I hear you cry! Correct, but using a command like
the following, you can run procenv as PID 1, which will dump it's output and
then launch `systemd` as the _same_ PID and boot the system as normal!

```grub
init=/usr/bin/procenv PROCENV_FILE=/dev/ttyS0 PROCENV_EXEC="/sbin/init --foo-bar --baz"
```

> **Note:** Further details about this and more examples are listed in the
> manual page, `procenv(1)`.

## Platforms

`procenv` runs on the following operating systems:

- Android
- *BSD
- [FreeBSD](#freebsd)
- GNU Hurd
- GNU Linux
- macOS
- [Minix 3](README-BUILD.md)

It unashamedly emulates a number of existing system utilities as it is
attempting to be all-encompassing: I wrote it with the aim of being able to
dump "everything" that a process may care about by simply running a single
program (by default). Also, the line of demarcation between "process",
"program" and  "system" is slightly blurry in some aspects.  For example
`sysconf(3)` variables could arguably be considered system attributes, but
`procenv` shows these too since they are obviously meant to be queryable by
applications.

## Install

### Snap

To install the [snap package](https://snapcraft.io/procenv):

```bash
$ sudo snap install procenv
```

> **Note:** Alternatively, you can [build your own snap](#build-snap-package).

### CentOS

- Enable EPEL repository.
- Install:

  ```bash
  $ sudo dnf -y install procenv
  ```

### Debian and Ubuntu

```bash
$ sudo apt -y install procenv
```

### Fedora

```bash
$ sudo dnf -y install procenv
```

### FreeBSD

To install the binary package:

```bash
$ sudo pkg -y install procenv
```

To install the port:

```bash
$ cd /usr/ports/sysutils/procenv
$ sudo make install clean
```

> **Note:** See also the [build document](README-BUILD.md).

### Gentoo

```bash
$ sudo emerge sys-process/procenv
```

### macOS

Install via [MacPorts](https://ports.macports.org/port/procenv/summary):

```bash
$ sudo port install procenv
```

### SUSE

```bash
$ sudo zypper install -y procenv
```

## Build

## Build snap package

```bash
$ git clone https://github.com/jamesodhunt/procenv
$ cd procenv
$ snapcraft
```

## Build from source

1. Install dependencies

   | Platform | Usage | Required? | Dependency | Rationale |
   |-|-|-|-|-|
   | common | build | yes | GCC or Clang compiler | For building the code |
   | common | build | yes | GNU Autoconf  | For configuring the source package |
   | common | build | yes | GNU Automake | For generating makefiles |
   | common | build | yes | GNU Make | For building the code |
   | common | build | yes | `pkgconf` / `pkg-config` | For configuring build dependencies |
   | common | test | optional | Check | For running unit tests |
   | common | test | optional | Expat | For validating XML output |
   | common | test | optional | GNU Groff | For checking man page documentation |
   | Linux | build | optional | `libapparmor` development package | For capabilities details |
   | Linux | build | optional | `libcap` development package | For capabilities details |
   | Linux | build | optional | `libnuma` development package | For NUMA memory details |
   | Linux | build | optional | `libselinux` development package | For SELinux details |
   | BSD | build | optional | `libsysinfo` package or port | For general memory details |

1. Checkout the source code:

   ```bash
   $ git clone https://github.com/jamesodhunt/procenv
   $ cd procenv
   ```

1. Configure and build:

   ```bash
   $ autoreconf -fi && ./configure
   $ make && make check && sudo make install
   ```

   > **Note:**
   >
   > For BSD systems, replace `make` with `gmake` above to ensure you run using
   > GNU Make (BSD make will hang at the test stage!)

## Results

`procenv` is extremely useful for learning about the environment
software builds in. Often, such systems disallow login, but do allow
access to log files. Handily, when you build `procenv`, it runs a battery of
tests and *also runs itself*. This means that the build environment gets
captured in the build logs themselves.

Select a link below and drill down to the build log to see the `procenv`
output:

### Debian build environment

#### `buildd` environment

- https://buildd.debian.org/status/package.php?p=procenv&suite=sid

#### Debian `autopkgtest` (DEP-8) environment

- https://ci.debian.net/packages/p/procenv/

### Fedora build environment

- https://src.fedoraproject.org/rpms/procenv/

### Gentoo build environment

- https://packages.gentoo.org/packages/sys-process/procenv

### MacPorts build environment

- https://ports.macports.org/port/procenv/

### Open Build Service (OBS) build environment

- https://build.opensuse.org/package/show/home:jamesodhunt:procenv/procenv

If you distro does not yet provide a `procenv` package, binary
packages for RHEL, Fedora, CentOS, SLES, and Arch Linux are available
from here:

- https://software.opensuse.org/download.html?project=home%3Ajamesodhunt%3Aprocenv&package=procenv

Click on your icon for your distro and follow the instructions.

Note that these packages are "bleeding edge" (generated directly from the GitHub repository).

### Semaphore-CI build environment

- https://semaphoreci.com/jamesodhunt/procenv
  (Click "Passed", "Job #", then "`make check`" to see output).

### Travis-CI build environment

- https://travis-ci.org/jamesodhunt/procenv

### Ubuntu build environment

- https://launchpad.net/ubuntu/+source/procenv

## Porting

Can you help port `procenv` to other platforms (AIX, HP-UX, Solaris, ...), or
can you give me access to new platforms? If so, please get in contact or take
a look at the [porting document](src/platform).

## References

See http://ifdeflinux.blogspot.com/2012/10/procenv-and-process-environment.html

## Author

`procenv` was written by James Hunt <jamesodhunt@gmail.com>.

## Home Page

- https://github.com/jamesodhunt/procenv
