Initial Blog annoucement:

  http://ifdeflinux.blogspot.com/2012/10/procenv-and-process-environment.html

Homepage:

  https://launchpad.net/procenv

This is a simple utility, licensed under the GPL, that essentially dumps
every conceivable aspect of a processes environment (*) that it can
(without needing super-user privs). It unashamedly emulates a number of
existing system utilities as it is attempting to be all-encompassing: I
wrote it with the aim of being able to dump "everything" that a process
may care about by simply running a single program (by default). Also,
the line of demarcation between "process", "program" and  "system" is
slightly blurry in some aspects. For example sysconf(3) variables could
arguably be considered system attributes, but procenv shows these too
since they are obviously meant to be querable by applications.


(*) - If you find anything missing, please either raise a bug via
      https://bugs.launchpad.net/procenv/+filebug or send a patch! :-)
