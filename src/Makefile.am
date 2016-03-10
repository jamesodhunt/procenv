AUTOMAKE_OPTIONS = subdir-objects

AM_CFLAGS = \
	-pedantic \
	-std=gnu99 \
	-Wall -Wunused \
	-fstack-protector \
	-Wformat


# XXX: magic option that will remove all unused symbols
# (defined by platform-generic.c).
AM_CFLAGS += -fdata-sections -ffunction-sections

# keep it tight
AM_CFLAGS += -Werror

bin_PROGRAMS = procenv

procenv_SOURCES = \
	procenv.c procenv.h \
	pr_list.c pr_list.h \
	pstring.c pstring.h \
	string-util.c string-util.h \
	output.c output.h \
	util.c util.h \
	types.h \
	messages.h \
	platform.h platform-headers.h \
    platform/platform-generic.c platform/platform-generic.h

# XXX: unused symbol removal magic - part 2
procenv_LDFLAGS = -Wl,--gc-sections

procenv_LDADD = -lrt
procenv_CPPFLAGS =
procenv_CPPFLAGS += -I $(srcdir) -I $(srcdir)/platform

if PROCENV_PLATFORM_LINUX
procenv_SOURCES += platform/linux/platform.c platform/linux/platform-linux.h
procenv_CPPFLAGS += -I $(srcdir)/platform/linux -DPROCENV_PLATFORM_LINUX
endif

if PROCENV_PLATFORM_HURD
procenv_SOURCES += platform/hurd/platform.c platform/hurd/platform-hurd.h
procenv_CPPFLAGS += -I $(srcdir)/platform/hurd -DPROCENV_PLATFORM_HURD
endif

if PROCENV_PLATFORM_FREEBSD
procenv_SOURCES += platform/freebsd/platform.c platform/freebsd/platform-freebsd.h
procenv_CPPFLAGS += -I $(srcdir)/platform/freebsd -DPROCENV_PLATFORM_FREEBSD
endif

if PROCENV_PLATFORM_GENERIC
procenv_SOURCES += platform/unknown/platform.c platform/unknown/platform-unknown.h
procenv_CPPFLAGS += -I $(srcdir)/platform/unknown -DPROCENV_PLATFORM_GENERIC
endif

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
	    -e 's|[@]package_url[@]|$(PACKAGE_URL)|g' \
	    -e 's|[@]procenv_platform[@]|$(procenv_platform)|g' \
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
