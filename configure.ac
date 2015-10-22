#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([procenv_version], 0.40)

AC_INIT(procenv, [procenv_version], [jamesodhunt@ubuntu.com])
AC_COPYRIGHT([Copyright (C) 2012-2015 James Hunt <jamesodhunt@ubuntu.com> and Kees Cook <kees@ubuntu.com>])
AC_CONFIG_SRCDIR([src/procenv.c])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config])

AC_SUBST(PROCENV_VERSION, procenv_version)

AC_GNU_SOURCE
AC_SYS_LARGEFILE

#---------------------------------------------------------------------
# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O

PKG_PROG_PKG_CONFIG

# Look for C unit test framework (http://check.sourceforge.net/).
PKG_CHECK_MODULES([CHECK], [check], [HAVE_CHECK=yes], [HAVE_CHECK=no])

#---------------------------------------------------------------------
# Checks for header files.
# this header is not available on older distributions (such as Ubuntu
# Lucid)
AC_CHECK_HEADERS([linux/securebits.h])
AC_CHECK_HEADERS(pthread.h,, [AC_MSG_ERROR([pthread.h required])])

#---------------------------------------------------------------------
# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([clock_gettime getcwd localtime_r strcasecmp strchr strstr sched_getcpu])

# BSD process inspection library
AC_SEARCH_LIBS([kvm_openfiles], [kvm],
               [HAVE_KVM=yes],
               [HAVE_KVM=no])

AC_SEARCH_LIBS([numa_available], [numa],
               [HAVE_NUMA=yes],
               [HAVE_NUMA=no])
AC_CHECK_HEADERS([numa.h])

# FreeBSD 9+ with appropriately configured kernel
# (enabled by default in FreeBSD 10)
AC_SEARCH_LIBS([cap_getmode], [c])
AC_CHECK_HEADERS([sys/capability.h])

AC_SEARCH_LIBS([cap_init], [cap],
               [HAVE_LIBCAP=yes],
               [HAVE_LIBCAP=no])

AC_SEARCH_LIBS([pthread_create], [pthread],
               [HAVE_PTHREAD=yes],
               [HAVE_PTHREAD=no])
AC_SEARCH_LIBS([getpidcon], [selinux],
               [HAVE_SELINUX=yes],
               [HAVE_SELINUX=no])
AM_CONDITIONAL([HAVE_SELINUX], [test x$HAVE_SELINUX = xtrue])
AC_CHECK_HEADERS([selinux/selinux.h])

AC_SEARCH_LIBS([aa_gettaskcon], [apparmor],
               [HAVE_APPARMOR=yes],
               [HAVE_APPARMOR=no])

AM_CONDITIONAL([HAVE_APPARMOR], [test x$HAVE_APPARMOR = xtrue])
AC_CHECK_HEADERS([sys/apparmor.h])

AC_ARG_ENABLE([tests],
	AS_HELP_STRING([--disable-tests],
		[Disable unit tests]),
		[enable_tests=no], [enable_tests=yes])

AM_CONDITIONAL([ENABLE_TESTS], test "$enable_tests" = yes)
AM_CONDITIONAL([HAVE_CHECK], test "$HAVE_CHECK" = yes)

#---------------------------------------------------------------------
# Other checks

# automake-1.13 defaults to running tests in parallel. As a consequence,
# it also disables verbose output meaning that procenv output is not
# visible in build logs. Therefore, force old behaviour by passing
# 'serial-tests', but only for version of automake >= 1.13 since older
# versions don't recognise that option.
AM_INIT_AUTOMAKE(m4_esyscmd([
	version=`automake --version|head -n 1|grep -o "[0-9][0-9]*\.[0-9][0-9]*"`
	major=`echo $version|cut -d\. -f1`
	minor=`echo $version|cut -d\. -f2`
	if [ "$major" = 1 -a "$minor" -ge 13 ]; then
		echo serial-tests
	elif [ "$major" -gt 1 ]; then
		echo serial-tests
	fi
]))

original_cflags="$CFLAGS"
CFLAGS=-Werror=format-security
AC_MSG_CHECKING([whether CC supports -Werror=format-security])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
    [AC_MSG_RESULT([yes])]
        [AM_CFLAGS=-Werror=format-security],
	    [AC_MSG_RESULT([no])])
CFLAGS="$original_cflags"
AC_SUBST([AM_CFLAGS])

AC_ARG_ENABLE([compiler-optimisations],
	AS_HELP_STRING([--disable-compiler-optimisations],
		       [Disable compiler optimisations]),
	[AS_IF([test "x$enable_compiler_optimisations" = "xno"],
	       [[CFLAGS=`echo "$CFLAGS" | sed -e "s/ -O[1-9s]*\b/ -O0/g"`
	         CXXFLAGS=`echo "$CXXFLAGS" | sed -e "s/ -O[1-9s]*\b/ -O0/g"`]])])

AC_ARG_ENABLE([linker-optimisations],
	AS_HELP_STRING([--disable-linker-optimisations],
		       [Disable linker optimisations]),
	[AS_IF([test "x$enable_linker_optimisations" = "xno"],
	       [LDFLAGS=`echo "$LDFLAGS" | sed -e "s/ -Wl,-O[0-9]*\b//g"`],
	       [LDFLAGS="$LDFLAGS -Wl,-O1"])])

AC_ARG_ENABLE([debug],
	AS_HELP_STRING([--enable-debug],
		       [enable developer build]),
	[AS_IF([test "x$enable_debug" = "xyes"],
	       [[CFLAGS=`echo "$CFLAGS -DDEBUG -pg"`
	         CXXFLAGS=`echo "$CXXFLAGS -DDEBUG -pg"`
             DEBUG_BUILD=yes]])], [DEBUG_BUILD=no])

AC_ARG_ENABLE([reproducible_build],
	AS_HELP_STRING([--enable-reproducible-build],
		[Disable display of build-time values that are guaranteed to differ between builds]),
		[reproducible_build=yes], [reproducible_build=no])

if test "$reproducible_build" = yes; then
	AC_DEFINE([PROCENV_REPRODUCIBLE_BUILD], [1], [Generate a reproducible build])
fi

#---------------------------------------------------------------------
# XXX: Dump details of the preprocess/compiler/linker *NOW* so that if
# procenv fails to build later, the build logs will show details of the
# environment that should aid remote debugging.
#
# This may be bad form, but trying to do this at the automake stage
# seems(?) to be impossible.
t=src/tests/show_compiler_details
msg=$(cat <<EOT
checking that test '$t' runs (output an aide to debugging should procenv fail to build)

EOT
)
AS_BOX($msg)
AS_IF([$t], [test_result=pass], [test_result=fail])
AS_BOX([showing result of test '$t': $test_result])

AC_OUTPUT(Makefile src/Makefile procenv.spec)
AC_MSG_RESULT([
    Configure settings for $PACKAGE_NAME version $VERSION

    Reproducible build          : ${reproducible_build}
    Debug build                 : ${DEBUG_BUILD}
    Check Unit Test Framework   : ${HAVE_CHECK}

    libapparmor                 : ${HAVE_APPARMOR}
    libselinux                  : ${HAVE_SELINUX}
    libcap                      : ${HAVE_LIBCAP}
    libnuma                     : ${HAVE_NUMA}
    libpthread                  : ${HAVE_PTHREAD}
    libkvm                      : ${HAVE_KVM}
              ])
