AC_INIT([gaia], [1.0], [notishell@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_PROG_AR
AC_PROG_CC
AC_PROG_RANLIB
AC_CHECK_LIB(readline, readline, [], [])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
  Makefile
  src/Makefile
  src/gaia/Makefile
])
AC_OUTPUT
