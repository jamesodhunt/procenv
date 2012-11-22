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

TESTS =

# Dump preprocessor/compiler/linker details as debug aid should the
# subsequent test fail.
TESTS += tests/show_compiler_details

# Run built binary to ensure we can display all values
TESTS += procenv

EXTRA_DIST = tests/show_compiler_details
