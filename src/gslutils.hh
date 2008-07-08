#ifndef GSLUTILS_HH
#define GSLUTILS_HH

#ifdef OLDGSL

#include <stddef.h>

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

__BEGIN_DECLS

double gsl_stats_correlation (const double data1[], const size_t stride1,const double data2[], const size_t stride2, const size_t n);

__END_DECLS

#endif // OLDGSL

#endif // GSLUTILS_HH
