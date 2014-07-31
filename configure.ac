#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([seq2gif], [0.9.0], [user@zuse.jp])
AC_CONFIG_SRCDIR([color.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign])

AC_ARG_WITH(fontdata,
            AC_HELP_STRING([--with-fontdata],
                           [Specified font data (mplus or milkjf, default is mplus)]),
                           [fontdata=${withval}],
                           [fontdata=mplus])
AC_MSG_NOTICE([font data ${fontdata} is selected])

AC_DEFINE(FONT_MPLUS, 1, [m+ bitmap font])
AC_DEFINE(FONT_MILKJF, 2, [milkjf font])

if test x$fontdata == xmilkjf; then
    AC_DEFINE(SELECTED_FONT, FONT_MILKJF, [selected font data])
else
    AC_DEFINE(SELECTED_FONT, FONT_MPLUS, [selected font data])
fi

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADER([stdio.h], [],
                [AC_MSG_ERROR(stdio.h not found.)])
AC_CHECK_HEADER([stdint.h], [],
                [AC_MSG_ERROR(stdint.h not found.)])
AC_CHECK_HEADER([stdbool.h], [],
                [AC_MSG_ERROR(stdbool.h not found.)])
AC_CHECK_HEADER([math.h], [],
                [AC_MSG_ERROR(math.h not found.)])
AC_CHECK_HEADERS([sys/errno.h \
                  errno.h \
                  sys/ctype.h \
                  ctype.h \
                  fcntl.h \
                  limits.h \
                  stdlib.h \
                  string.h \
                  wchar.h \
                  sys/unistd.h \
                  unistd.h \
                  getopt.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_CHECK_FUNC([memset], [],
              [AC_MSG_ERROR(memset is not available.)])

AC_CHECK_FUNC([getopt_long],
              [AC_DEFINE(HAVE_GETOPT_LONG, 1, [whether getopt_long is avilable])],
              [AC_CHECK_FUNC([getopt],
                             [AC_DEFINE(HAVE_GETOPT, 1, [whether getopt is avilable])],
                             [AC_MSG_ERROR(getopt or getopt_long is not available.)])])

AC_CHECK_FUNCS([setmode _setmode memcpy memmove strchr strstr strtol calloc])

AC_DEFUN([LS_UPDATE_TIMESTAMP], [
          touch aclocal.m4 Makefile.in configure config.h.in
          ])
LS_UPDATE_TIMESTAMP

AC_CONFIG_FILES([Makefile package.json])
AC_OUTPUT
