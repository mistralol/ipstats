AC_INIT(ipstats, 1.0.0, james@stev.org)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_MACRO_DIR([m4])

LT_INIT
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

AX_CXX_COMPILE_STDCXX_11(noext, optional)

AX_COMPILER_FLAGS([WARN_CFLAGS],[WARN_LDFLAGS])
AC_SUBST([AM_CXXFLAGS], $WARN_CXXFLAGS)

dnl give error and exit if we don't have pkgconfig
AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, [ ], [
  AC_MSG_ERROR([You need to have pkg-config installed!])
])

AC_CHECK_LIB([pcap], [pcap_open_live], [], [
	AC_MSG_ERROR([You need to have libpcap installed])
])

AC_CHECK_LIB([ncurses], [initscr], [], [
	AC_MSG_ERROR([You need to have libncurses installed])
])

AC_CHECK_LIB([boost_thread], [_init], [], [
	AC_MSG_ERROR([You need to have boost_thread installed])
])

AC_CHECK_LIB([boost_system], [_init], [], [
	AC_MSG_ERROR([You need to have boost_system installed])
])

AC_CHECK_LIB([pthread], [pthread_mutex_init], [], [
	AC_MSG_ERROR([You need to have pthread installed])
])

AC_OUTPUT(Makefile src/Makefile)


