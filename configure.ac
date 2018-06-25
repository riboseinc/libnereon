
AC_PREREQ([2.69])
AC_INIT([libnereon], [0.9.0.7], [https://www.ribose.com])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/nereon.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_PROG_LIBTOOL
AC_PROG_INSTALL

# Checks for header files.
AC_CHECK_HEADERS_ONCE([fcntl.h unistd.h])
AC_TYPE_OFF_T
AC_FUNC_MMAP
AC_CHECK_HEADERS_ONCE([fcntl.h])
AC_CHECK_HEADERS_ONCE([sys/types.h])
AC_CHECK_HEADERS_ONCE([sys/stat.h])
AC_CHECK_HEADERS_ONCE([sys/param.h])
AC_CHECK_HEADERS_ONCE([sys/mman.h])
AC_CHECK_HEADERS_ONCE([stdlib.h])
AC_CHECK_HEADERS_ONCE([stddef.h])
AC_CHECK_HEADERS_ONCE([stdarg.h])
AC_CHECK_HEADERS_ONCE([stdbool.h])
AC_CHECK_HEADERS_ONCE([stdint.h])
AC_CHECK_HEADERS_ONCE([string.h])
AC_CHECK_HEADERS_ONCE([strings.h])
AC_CHECK_HEADERS_ONCE([unistd.h])
AC_CHECK_HEADERS_ONCE([ctype.h])
AC_CHECK_HEADERS_ONCE([errno.h])
AC_CHECK_HEADERS_ONCE([limits.h])
AC_CHECK_HEADERS_ONCE([libgen.h])
AC_CHECK_HEADERS_ONCE([stdio.h])
AC_CHECK_HEADERS_ONCE([float.h])
AC_CHECK_HEADERS_ONCE([math.h])
AC_CHECK_HEADERS_ONCE([endian.h sys/endian.h machine/endian.h])

AC_CHECK_HEADERS_ONCE([ucl.h])

AC_ARG_WITH([ucl],
    [AS_HELP_STRING([--with-ucl=DIR],
        [root of libucl directory])],
    [LIBUCL_CFLAGS="-I$withval/include"],
    [LIBUCL_LDADD="-lucl"],
    [LIBUCL_LDFLAGS="-L$withval/lib"])
AC_SUBST([LIBUCL_CFLAGS])
AC_SUBST([LIBUCL_LDFLAGS])
AC_SUBST([LIBUCL_LDADD])

AC_CHECK_HEADERS([ucl.h], [],
[
	AC_MSG_ERROR([Could not find libucl.h. Please specify libucl installation directory])
])

AC_CHECK_LIB([ucl], [ucl_object_new], [],
[
	AC_MSG_ERROR([Could not find libucl library. Please specify libucl installation directory])
])

AC_ARG_ENABLE(debug,
     AS_HELP_STRING(--enable-debug, turn on debugging (disabled by default)),
     [], [enable_debug=no]
)
if test "x$enable_debug" = "xyes" ; then
    CFLAGS="$CFLAGS -g3 -O0 -DDEBUG"
fi

AC_CONFIG_FILES([Makefile
                src/Makefile
                tests/Makefile
                tools/Makefile
                ])

AC_OUTPUT
