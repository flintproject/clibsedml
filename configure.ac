#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([clibsedml], [0.1.3], [tabe@fixedpoint.jp])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Wno-extra-portability -Werror foreign])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([config])

# Checks for programs.
LT_PREREQ([2.2.6])
LT_INIT([win32-dll disable-static pic-only])
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

# Checks for libraries.
AM_PATH_XML2([2.7.3])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD

AC_CONFIG_FILES([
  Makefile
  clibsedml.pc
  clibsedml.spec
  src/Makefile
  test/Makefile
])
AC_OUTPUT
