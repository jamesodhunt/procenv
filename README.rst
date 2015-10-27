.. image:: https://img.shields.io/badge/license-GPL-3.0.svg

.. image:: https://travis-ci.org/jamesodhunt/procenv.svg?branch=master
   :target: https://travis-ci.org/jamesodhunt/procenv

.. image:: https://scan.coverity.com/projects/jamesodhunt-procenv/badge.svg
   :target: https://scan.coverity.com/projects/jamesodhunt-procenv
   :alt: Coverity Scan Build Status

.. image:: https://img.shields.io/badge/donate-flattr-red.svg
   :alt: Donate via flattr
   :target: https://flattr.com/profile/jamesodhunt

=======
procenv
=======

.. contents::
.. sectnum::

Overview
--------

``procenv`` is a simple command-line utility, written in C and licensed
under the GPL, that dumps all attributes of the environment [#]_ in which
it runs, in well-structured plain ASCII, JSON (YAML), XML or CSV.

It is useful as a test tool, to understand what environment a process
runs in and for system comparison.

``procenv`` runs on Linux, FreeBSD, kFreeBSD, GNU Hurd and Android. It
unashamedly emulates a number of existing system utilities as it is
attempting to be all-encompassing: I wrote it with the aim of being able
to dump "everything" that a process may care about by simply running a
single program (by default). Also, the line of demarcation between
"process", "program" and  "system" is slightly blurry in some aspects.
For example ``sysconf(3)`` variables could arguably be considered system
attributes, but procenv shows these too since they are obviously meant
to be queryable by applications.

.. [#] If you find anything missing, please either raise a bug or send a
       patch :)

Results
-------

``procenv`` is extremely useful for learning about the environment
software builds in. Often, such systems disallow login, but do allow
access to log files. Handily, ``procenv`` runs a battery of tests on
build including running itself so that the build environment gets
captured in the builds logs themselves. Select a link below and drill
down to the build log to see the ``procenv`` output:

Travis-CI
~~~~~~~~~

* https://travis-ci.org/jamesodhunt/procenv

Open Build Service (OBS)
~~~~~~~~~~~~~~~~~~~~~~~~

Provides binary packages and build logs for RHEL, Fedora, Centos, SLES,
and Arch Linux.

* https://build.opensuse.org/package/show/home:jamesodhunt:procenv/procenv

Ubuntu
~~~~~~

build environment
.................

Click the "twisty" triangle to the left of the release number under a
release name, then click one of the build links (like "``amd64``":

* https://launchpad.net/ubuntu/+source/procenv

PPA environment
...............

https://code.launchpad.net/~jamesodhunt/+recipe/procenv-daily

Ubuntu Autopkgtest (DEP-8) environment
......................................

* https://jenkins.qa.ubuntu.com/view/Wily/view/AutoPkgTest/job/wily-adt-procenv/
* https://jenkins.qa.ubuntu.com/view/Vivid/view/AutoPkgTest/job/vivid-adt-procenv/
* https://jenkins.qa.ubuntu.com/view/Utopic/view/AutoPkgTest/job/utopic-adt-procenv/
* https://jenkins.qa.ubuntu.com/view/Trusty/view/AutoPkgTest/job/trusty-adt-procenv/

The ``sbuild`` tool environment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``procenv`` gets run from within sbuild via a DEP-8 test:

* https://jenkins.qa.ubuntu.com/view/Wily/view/AutoPkgTest/job/wily-adt-sbuild/

The ``pbuilder`` tool environment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``procenv`` gets run from within pbuilder via a DEP-8 test:

* https://jenkins.qa.ubuntu.com/view/Wily/view/AutoPkgTest/job/wily-adt-pbuilder/

Debian
~~~~~~

buildd (and ports buildd) environment
.....................................

* https://buildd.debian.org/status/package.php?p=procenv&suite=sid

Debian autopkgtest (DEP-8) environment
......................................

* http://ci.debian.net/#package/procenv

FreeBSD
~~~~~~~

Logs used to be available, but sadly it appears pointyhat.freebsd.com has
been taken offline (any replacement?)

Porting
-------

Can you help port ``procenv`` to other platforms (AIX, HP-UX, Solaris,
...)? If so, please get in contact.

References
----------

See http://ifdeflinux.blogspot.com/2012/10/procenv-and-process-environment.html


Author
------

``procenv`` was written by James Hunt <jamesodhunt@ubuntu.com>.
