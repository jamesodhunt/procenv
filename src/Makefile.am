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
	    -e 's|[@]package_url[@]|$(PACKAGE_URL)|g' \
	    $< > $@
	chmod +x $@

if HAVE_CHECK
TESTS += check_pr_list

check_PROGRAMS = check_pr_list
check_pr_list_SOURCES = tests/check_pr_list.c
check_pr_list_CFLAGS = @CHECK_CFLAGS@ -I$(top_srcdir)/src
check_pr_list_LDADD = @CHECK_LIBS@ pr_list.o

endif

# Run built binary to ensure we can display all values
TESTS += procenv

TESTS += check_all_args

CLEANFILES += check_all_args

endif

EXTRA_DIST = \
    tests/show_compiler_details \
    tests/check_all_args.in \
	tests/check_pr_list.c
