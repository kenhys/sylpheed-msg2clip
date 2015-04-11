#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([msg2clip], [0.2.0], [kenhys@gmail.com])
AC_CONFIG_SRCDIR([src/msg2clip.c])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign])

AC_DEFINE([SYLPF_ID], [PACKAGE_NAME], [""])
AC_DEFINE([SYLPF_OPTION], [msg2clip_option], [""])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

m4_include(macros/sylplugin_factory.m4)

# Checks for libraries.
AM_PATH_GLIB_2_0(2.8.0,, AC_MSG_ERROR(Test for GLib failed. See the 'INSTALL' for help.))
AM_PATH_GTK_2_0(2.8.0,, AC_MSG_ERROR(Test for Gtk failed. See the 'INSTALL' for help.))
GETTEXT_PACKAGE=msg2clip
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Define text domain.])
AM_GLIB_GNU_GETTEXT

PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([GTK], [gtk+-2.0])
PKG_CHECK_MODULES([SYLPF], [sylplugin-factory])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
