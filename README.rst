.. image:: https://scan.coverity.com/projects/348/badge.svg
   :target: https://scan.coverity.com/projects/procenv
   :alt: Coverity Scan Build Status

.. image:: https://img.shields.io/badge/license-GPL-3.0.svg

.. image:: https://img.shields.io/badge/donate-flattr-red.svg
   :alt: Donate via flattr
   :target: https://flattr.com/profile/jamesodhunt

=======
procenv
=======

``procenv`` is a simple utility, licensed under the GPL, that essentially dumps
every conceivable aspect of a processes environment (*) that it can.

It unashamedly emulates a number of existing system utilities as it is
attempting to be all-encompassing: I wrote it with the aim of being able
to dump "everything" that a process may care about by simply running a
single program (by default). Also, the line of demarcation between
"process", "program" and  "system" is slightly blurry in some aspects.
For example ``sysconf(3)`` variables could arguably be considered system
attributes, but procenv shows these too since they are obviously meant
to be queryable by applications.

(*) - If you find anything missing, please either raise a bug via
      https://bugs.launchpad.net/procenv/+filebug or send a patch! :-)

References
----------

See http://ifdeflinux.blogspot.com/2012/10/procenv-and-process-environment.html


Author
------

``procenv`` was written by James Hunt <jamesodhunt@ubuntu.com>.
