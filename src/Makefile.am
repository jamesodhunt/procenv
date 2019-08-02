AUTOMAKE_OPTIONS = subdir-objects

AM_CFLAGS = \
	-pedantic \
	-std=gnu99 \
	-Wall -Wunused \
	-fstack-protector \
	-Wformat


# XXX: magic option that will remove all unused symbols
# (defined by platform-generic.c).
if PROCENV_PLATFORM_DARWIN
AM_CFLAGS += -flto
else
AM_CFLAGS += -fdata-sections -ffunction-sections
endif

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
if PROCENV_PLATFORM_DARWIN
procenv_LDFLAGS = -flto
else
procenv_LDFLAGS = -Wl,--gc-sections
endif

# should really do this in configure.ac
if PROCENV_PLATFORM_MINIX
procenv_LDADD = -lc
endif

procenv_CPPFLAGS =
procenv_CPPFLAGS += -I $(srcdir) -I $(srcdir)/platform

if PROCENV_PLATFORM_DARWIN
procenv_SOURCES += platform/darwin/platform.c platform/darwin/platform-darwin.h
procenv_CPPFLAGS += -I $(srcdir)/platform/darwin -D PROCENV_PLATFORM_DARWIN
endif

if PROCENV_PLATFORM_LINUX
procenv_SOURCES += platform/linux/platform.c platform/linux/platform-linux.h
procenv_CPPFLAGS += -I $(srcdir)/platform/linux -D PROCENV_PLATFORM_LINUX
endif

if PROCENV_PLATFORM_MINIX
procenv_SOURCES += platform/minix/platform.c platform/minix/platform-minix.h
procenv_CPPFLAGS += \
	-I $(srcdir)/platform/minix -D PROCENV_PLATFORM_MINIX
endif

if PROCENV_PLATFORM_HURD
procenv_SOURCES += platform/hurd/platform.c platform/hurd/platform-hurd.h
procenv_CPPFLAGS += -I $(srcdir)/platform/hurd -D PROCENV_PLATFORM_HURD
endif

if PROCENV_PLATFORM_FREEBSD
procenv_SOURCES += platform/freebsd/platform.c platform/freebsd/platform-freebsd.h
procenv_CPPFLAGS += -I $(srcdir)/platform/freebsd \
				-D PROCENV_PLATFORM_FREEBSD \
				-D PROCENV_PLATFORM_BSD
endif

if PROCENV_PLATFORM_NETBSD
procenv_SOURCES += platform/netbsd/platform.c platform/netbsd/platform-netbsd.h
procenv_CPPFLAGS += -I $(srcdir)/platform/netbsd \
				-D PROCENV_PLATFORM_NETBSD \
				-D PROCENV_PLATFORM_BSD
endif

if PROCENV_PLATFORM_OPENBSD
procenv_SOURCES += platform/openbsd/platform.c platform/openbsd/platform-openbsd.h
procenv_CPPFLAGS += -I $(srcdir)/platform/openbsd \
				-D PROCENV_PLATFORM_OPENBSD \
				-D PROCENV_PLATFORM_BSD
endif

if PROCENV_PLATFORM_GENERIC
procenv_SOURCES += platform/unknown/platform.c platform/unknown/platform-unknown.h
procenv_CPPFLAGS += -I $(srcdir)/platform/unknown -D PROCENV_PLATFORM_GENERIC
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

TESTS += tests/show_machine_details

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
check_pr_list_SOURCES = tests/check_pr_list.c pr_list.c
check_pr_list_CFLAGS = @CHECK_CFLAGS@ -I$(top_srcdir)/src
check_pr_list_LDADD = @CHECK_LIBS@

endif

TESTS += check_all_args

# Run built binary to ensure we can display all values
TESTS += procenv

CLEANFILES += check_all_args

endif

EXTRA_DIST = \
    tests/show_compiler_details \
    tests/show_machine_details \
    tests/check_all_args.in \
    tests/check_pr_list.c
