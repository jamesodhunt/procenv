AM_CFLAGS = \
	-pedantic \
	-std=gnu99 \
	-Wall -Wunused -O2 \
	-fstack-protector \
	-Wformat -Werror=format-security

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
CLEANFILES =

check_all_args: tests/check_all_args.in
	sed -e 's|[@]abs_builddir[@]|$(abs_builddir)|g' \
	    $< > $@
	chmod +x $@

# Dump preprocessor/compiler/linker details as debug aid should the
# subsequent test fail.
TESTS += tests/show_compiler_details

# Run built binary to ensure we can display all values
TESTS += procenv

TESTS += check_all_args

CLEANFILES += check_all_args

EXTRA_DIST = \
    tests/show_compiler_details \
    tests/check_all_args.in
