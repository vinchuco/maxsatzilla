AC_PREREQ(2.59)
AC_INIT([Furius], [3.0], [paulo@matos-sorge.com jordi.planes@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_LANG(C++)

# Checks for programs.
AC_PROG_YACC
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LEX
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# TODO: Check for presence of libsvm through svm.h
# Checks for libraries.
# FIXME: Replace `main' with a function in `-lblas':
#AC_CHECK_LIB([blas], [main])
# FIXME: Replace `main' with a function in `-lcblas':
#AC_CHECK_LIB([cblas], [main])
# FIXME: Replace `main' with a function in `-lgmp':
#AC_CHECK_LIB([gmp], [main])
# FIXME: Replace `main' with a function in `-lgsl':
#AC_CHECK_LIB([gsl], [main])
# FIXME: Replace `main' with a function in `-lgslcblas':
#AC_CHECK_LIB([gslcblas], [main])
# FIXME: Replace `main' with a function in `-lm':
#AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lz':
#AC_CHECK_LIB([z], [main])

AC_SEARCH_LIBS([svm_train], [svm], [], [
  AC_MSG_ERROR([unable to find svm_train in libsvm, libsvm required])
])
AC_SEARCH_LIBS([gsl_vector_alloc], [gsl], [], [
  AC_MSG_ERROR([unable to find gsl_vector_alloc in libgsl, libgsl required])
], [-lblas])

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h limits.h stdint.h stdlib.h string.h sys/time.h sys/timeb.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([alarm floor getpagesize gettimeofday memset pow select sqrt strchr strstr strtol])

AC_CONFIG_FILES([Makefile
		 src/Makefile
                 ubcsat/Makefile])
AC_OUTPUT
