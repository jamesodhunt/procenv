#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(procenv, 0.6, [james.hunt@ubuntu.com])
AC_COPYRIGHT([Copyright (C) 2012 James Hunt <james.hunt@ubuntu.com>])
AC_CONFIG_SRCDIR([src/procenv.c])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config])

AC_GNU_SOURCE

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O

PKG_PROG_PKG_CONFIG

# Checks for libraries.

# Checks for header files.
# this header is not available on older distributions (such as Ubuntu
# Lucid)
AC_CHECK_HEADERS([linux/securebits.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_UID_T
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([clock_gettime getcwd localtime_r strcasecmp strchr strstr])

# BSD process inspection library
AC_SEARCH_LIBS([kvm_openfiles], [kvm])

AC_SEARCH_LIBS([getpidcon], [selinux], [HAVE_SELINUX=true])
AM_CONDITIONAL([HAVE_SELINUX], [test x$HAVE_SELINUX = xtrue])

AC_SEARCH_LIBS([aa_gettaskcon], [apparmor], [HAVE_APPARMOR=true])
AM_CONDITIONAL([HAVE_APPARMOR], [test x$HAVE_APPARMOR = xtrue])

# Other checks

AM_INIT_AUTOMAKE
AC_OUTPUT(Makefile src/Makefile)
