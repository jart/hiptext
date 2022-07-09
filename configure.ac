# -*- Autoconf -*-

AC_PREREQ(2.57)
AC_INIT(hiptext, 0.2, jtunney@gmail.com)

AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(config)
AM_INIT_AUTOMAKE(foreign)
AC_CONFIG_SRCDIR(src/hiptext.cc)
ifdef(AM_SILENT_RULES, [AM_SILENT_RULES(yes)])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
PKG_PROG_PKG_CONFIG(0.22)

AC_LANG(C++)
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX(11)
AC_PROG_RANLIB
AX_PTHREAD

AC_CHECK_LIB(jpeg, jpeg_set_defaults, [], [
  AC_MSG_ERROR([error: libjpeg is required])
])

PKG_CHECK_MODULES(LIBAVCODEC, libavcodec)
PKG_CHECK_MODULES(LIBAVFORMAT, libavformat)
PKG_CHECK_MODULES(LIBAVUTIL, libavutil)
PKG_CHECK_MODULES(LIBGLOG, libglog)
PKG_CHECK_MODULES(LIBPNG, libpng)
PKG_CHECK_MODULES(LIBSWSCALE, libswscale)

LIBGFLAGS_CFLAGS=""
AC_CHECK_HEADER([gflags/gflags.h], [
  AC_CHECK_LIB(gflags_nothreads, main, [
    LIBGFLAGS_LIBS="-lgflags_nothreads"
  ], [
    AC_CHECK_LIB(gflags, main, [
      LIBGFLAGS_LIBS="-lgflags"
    ], [
      AC_MSG_ERROR([error: libgflags_nothreads or libgflags are required])
    ])
  ])
], [
  AC_MSG_ERROR([error: libgflags headers are required])
])
AC_SUBST(LIBGFLAGS_CFLAGS)
AC_SUBST(LIBGFLAGS_LIBS)

AC_CHECK_PROGS(FREETYPE, freetype-config)
if test -n "$FREETYPE"; then
  LIBFREETYPE_CFLAGS="`$FREETYPE --cflags`"
  LIBFREETYPE_LIBS="`$FREETYPE --libs`"
else
  AC_MSG_WARN([freetype-config not found; guessing flags])
  LIBFREETYPE_CFLAGS="-I/usr/include/freetype2"
  LIBFREETYPE_LIBS="-lfreetype"
fi
AC_SUBST(LIBFREETYPE_CFLAGS)
AC_SUBST(LIBFREETYPE_LIBS)

AC_CHECK_PROGS(RAGEL, ragel)

AC_CONFIG_FILES(Makefile)
AC_OUTPUT
