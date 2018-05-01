#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([mirror-remote], [0.1], [nmanichon@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/mirror.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

AC_CHECK_PROG(DOXYGEN, doxygen, doxygen)
if test -z "$DOXYGEN";
then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

AC_CHECK_PROG(PDFLATEX, pdflatex, pdflatex)
if test -z "$PDFLATEX";
then AC_MSG_WARN([pdflatex not found - continuing without pdflatex support])
fi

AC_CHECK_PROG(DOT, dot, dot)
if test -z "$DOT";
then AC_MSG_WARN([dot not found - continuing without dot support])
fi
# Checks for libraries.
PKG_CHECK_MODULES([libgit2], [libgit2])
PKG_CHECK_MODULES([libmicrohttpd], [libmicrohttpd])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_CONDITIONAL([HAVE_PDFLATEX], [test -n "$PDFLATEX"])
AM_CONDITIONAL([HAVE_DOT], [test -n "$DOT"])
AM_COND_IF([HAVE_PDFLATEX], [AC_SUBST(GEN_HTML, NO) AC_SUBST(GEN_LATEX, YES)], [AC_SUBST(GEN_HTML, YES) AC_SUBST(GEN_LATEX, NO)])

AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/doxyconfig])])

AM_COND_IF([HAVE_DOT], [AC_SUBST(USE_DOT, YES)], [AC_SUBST(USE_DOT, NO)])

AC_OUTPUT(Makefile src/Makefile man/Makefile doc/Makefile)
