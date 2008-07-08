#ifndef GSLADDON_HH
#define GSLADDON_HH

#include <string>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

using std::string;

namespace GslAddon {

  gsl_matrix *matrix_mult(const gsl_matrix *, const gsl_matrix *);
  string matrix_toString(const gsl_matrix*, const string& = "");
  string vector_toString(const gsl_vector*, const string& = "");
  
};

#endif // GSLADDON_HH
