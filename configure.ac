#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([seq2gif], [0.10.3], [user@zuse.jp])
AC_CONFIG_SRCDIR([color.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign])

AC_ARG_WITH(fontdata,
            AC_HELP_STRING([--with-fontdata],
                           [Specified font data (unifont, milkjf or mplus(default: unifont)]),
                           [fontdata=${withval}],
                           [fontdata=mplus])
AC_MSG_NOTICE([font data ${fontdata} is selected])

AC_DEFINE(FONT_MPLUS, 1, [M+ Bitmap font])
AC_DEFINE(FONT_MILKJF, 2, [milkjf font])
AC_DEFINE(FONT_UNIFONT, 3, [GNU Unifont])

if test x$fontdata == xunifont; then
    AC_DEFINE(SELECTED_FONT, FONT_MPLUS, [GNU Unifont is selected.])
else
    if test x$fontdata == xmplus; then
        AC_DEFINE(SELECTED_FONT, FONT_MPLUS, [M+ font is selected.])
    else
        AC_DEFINE(SELECTED_FONT, FONT_MILKJF, [milkjf font is selected.])
    fi
fi

AC_ARG_ENABLE([gcov],
              [AS_HELP_STRING([--enable-gcov],
                              [Use gcov])],
              [],
              [enable_gcov=no])
AM_CONDITIONAL([COND_GCOV], [test x$enable_gcov != xno])

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
AC_CHECK_HEADERS([errno.h \
                  sys/types.h \
                  ctype.h \
                  fcntl.h \
                  limits.h \
                  stdlib.h \
                  string.h \
                  wchar.h \
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
