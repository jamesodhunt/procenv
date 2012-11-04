bin_PROGRAMS = procenv
procenv_SOURCES = procenv.c procenv.h
procenv_LDADD = -lrt
procenv_CPPFLAGS =

if HAVE_SELINUX
    procenv_CPPFLAGS += -DHAVE_SELINUX
endif
if HAVE_APPARMOR
    procenv_CPPFLAGS += -DHAVE_APPARMOR
endif

TESTS = procenv
