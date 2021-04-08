[![GPLv3 license](https://img.shields.io/badge/License-GPLv3-blue.svg)](http://perso.crans.org/besson/LICENSE.html)
[![Build Status](https://travis-ci.org/jamesodhunt/procenv.svg?branch=master)](https://travis-ci.org/jamesodhunt/procenv)
[![Coverity](https://img.shields.io/coverity/scan/jamesodhunt-procenv)](https://scan.coverity.com/projects/jamesodhunt-procenv)
[![Flattr](https://img.shields.io/badge/donate-flattr-blue.svg)](https://flattr.com/profile/jamesodhunt)
[![PayPal](https://img.shields.io/badge/paypal-donate-blue.svg)](https://www.paypal.me/jamesodhunt)

# `procenv`

[![demo](https://asciinema.org/a/118278.svg)](https://asciinema.org/a/118278?autoplay=1)

## Overview

`procenv` is a simple command-line utility, written in C and licensed
under the GPL, that dumps all attributes of the environment in which it
runs, in well-structured plain ASCII, JSON (YAML), XML or CSV.

> **Note:**
>
> If you find anything missing from the `procenv` output, please either raise
> an issue or send a patch :)

It is useful as a test tool, to understand what environment a process
runs in and for system comparison.

`procenv` runs on the following operating systems:

- Android
- FreeBSD
- GNU Hurd
- GNU Linux
- Minix 3
- NetBSD
- OpenBSD
- OSX

It unashamedly emulates a number of existing system utilities as it is
attempting to be all-encompassing: I wrote it with the aim of being able to
dump "everything" that a process may care about by simply running a single
program (by default). Also, the line of demarcation between "process",
"program" and  "system" is slightly blurry in some aspects.  For example
`sysconf(3)` variables could arguably be considered system attributes, but
`procenv` shows these too since they are obviously meant to be queryable by
applications.

## Results

`procenv` is extremely useful for learning about the environment
software builds in. Often, such systems disallow login, but do allow
access to log files. Handily, when you build `procenv`, it runs a battery of
tests and *also runs itself*. This means that the build environment gets
captured in the build logs themselves.

Select a link below and drill down to the build log to see the `procenv`
output:

### Semaphore-CI

- https://semaphoreci.com/jamesodhunt/procenv
  (Click "Passed", "Job #", then "`make check`" to see output).

### Travis-CI

- https://travis-ci.org/jamesodhunt/procenv

### Open Build Service (OBS)

- https://build.opensuse.org/package/show/home:jamesodhunt:procenv/procenv

If you distro does not yet provide a `procenv` package, binary
packages for RHEL, Fedora, CentOS, SLES, and Arch Linux are available
from here:

- https://software.opensuse.org/download.html?project=home%3Ajamesodhunt%3Aprocenv&package=procenv

Click on your icon for your distro and follow the instructions.

Note that these packages are "bleeding edge" (generated directly from the GitHub repository).

### Debian

#### `buildd` environment

- https://buildd.debian.org/status/package.php?p=procenv&suite=sid

#### Debian `autopkgtest` (DEP-8) environment

- https://ci.debian.net/packages/p/procenv/

### Fedora

- https://src.fedoraproject.org/rpms/procenv/

### Gentoo

- https://packages.gentoo.org/packages/sys-process/procenv

### Ubuntu

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
