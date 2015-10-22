AUTOMAKE_OPTIONS = subdir-objects

AM_CFLAGS = \
	-pedantic \
	-std=gnu99 \
	-Wall -Wunused \
	-fstack-protector \
	-Wformat

bin_PROGRAMS = procenv
procenv_SOURCES = \
	procenv.c procenv.h \
	pr_list.c pr_list.h

procenv_LDADD = -lrt
procenv_CPPFLAGS =

if HAVE_SELINUX
    procenv_CPPFLAGS += -DHAVE_SELINUX
endif
if HAVE_APPARMOR
    procenv_CPPFLAGS += -DHAVE_APPARMOR
endif

if ENABLE_TESTS

TESTS =
CLEANFILES =

check_all_args: tests/check_all_args.in
	sed -e 's|[@]builddir[@]|$(top_builddir)/$(subdir)|g' \
	    -e 's|[@]man_path[@]|$(top_srcdir)/man/procenv.1|g' \
	    $< > $@
	chmod +x $@

TESTS += check_pr_list

# Run built binary to ensure we can display all values
TESTS += procenv

TESTS += check_all_args

check_PROGRAMS = check_pr_list

check_pr_list_SOURCES = tests/check_pr_list.c

check_pr_list_CFLAGS = @CHECK_CFLAGS@ -I$(top_srcdir)/src
check_pr_list_LDADD = @CHECK_LIBS@ pr_list.o

CLEANFILES += check_all_args

endif

EXTRA_DIST = \
    tests/show_compiler_details \
    tests/check_all_args.in
