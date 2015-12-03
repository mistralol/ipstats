AC_INIT(ipstats, 1.0.0, james@stev.org)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_MACRO_DIR([m4])

LT_INIT
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_CXX

AX_CXX_COMPILE_STDCXX_11(noext, optional)

AC_PROG_LIBTOOL

dnl give error and exit if we don't have pkgconfig
AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, [ ], [
  AC_MSG_ERROR([You need to have pkg-config installed!])
])

AC_SUBST([AM_CFLAGS], ["-Wall -Werror"])

AC_OUTPUT(Makefile src/Makefile)


