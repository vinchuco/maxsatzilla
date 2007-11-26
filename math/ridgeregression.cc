#include "ridgeregression.hh"

#include <fstream>
#include <cassert>

using std::ofstream;

RidgeRegression::RidgeRegression(const MSZDataSet &data) :
  data(data) { }

RidgeRegression::~RidgeRegression() { }

void RidgeRegression::run(double delta, size_t out, const string &path) {

  // Create PHI matrix
  gsl_matrix *phi = gsl_matrix_alloc(data.getNRows(), data.getNFeatures());
  
  // populate phi
  for(size_t r = 0; r < phi->size1; r++)
    for(size_t c = 0; c < phi->size2; c++)
      gsl_matrix_set(phi, r, c, data.getFeatureValue(r, c));
  
  // create phi transpose
  gsl_matrix *phit = gsl_matrix_alloc(phi->size2, phi->size1);
  gsl_matrix_transpose_memcpy(phit, phi);

  // initialize delta matrix
  gsl_matrix *deltam = gsl_matrix_alloc(data.getNFeatures(), data.getNFeatures());
  gsl_matrix_set_identity(deltam);
  gsl_matrix_scale(deltam, delta);

  // initialize y vector as matrix
  gsl_matrix *y = gsl_matrix_alloc(data.getNRows(), 1);
  for(size_t r = 0; r < data.getNRows(); r++)
    gsl_matrix_set(y, r, 0, data.getOutputValue(r, out));

  gsl_matrix *phit_phi = matrix_mult(phit, phi);
  
  gsl_matrix_add(deltam, phit_phi);

  gsl_matrix *lf = matrix_mult(deltam, phit);
  gsl_matrix *final = matrix_mult(lf, y);

  // Free matrices
  gsl_matrix_free(phi);
  gsl_matrix_free(deltam);
  gsl_matrix_free(phit);
  gsl_matrix_free(y);
  gsl_matrix_free(phit_phi);
  gsl_matrix_free(lf);

  // Let's output this final matrix which should be m x 1.
  assert(final->size1 == data.getNFeatures());
  assert(final->size2 == 1);
  ofstream file;
  file.open(path.c_str());

  file << "#ifndef MAXSATZILLA_MODEL_FILE_HH\n"
       << "#define MAXSATZILLA_MODEL_FILE_HH\n"
       << "static double w[] = {";
    
  for(size_t i = 0; i < final->size1; i++) {
    file << gsl_matrix_get(final, i, 0);

    if(i != final->size1 - 1)
      file << ", ";
  }

  file << "};\n"
       << "#endif";

  file.close();

  gsl_matrix_free(final);
}

gsl_matrix *RidgeRegression::matrix_mult(const gsl_matrix *m1, const gsl_matrix *m2) const {
  
  assert(m1->size2 == m2->size1);

  gsl_matrix *prod = gsl_matrix_alloc(m1->size1, m2->size2);
  
  for(size_t r = 0; r < m1->size1; r++) {
    gsl_vector_const_view rowm1 = gsl_matrix_const_row(m1, r);

    for(size_t c = 0; c < m2->size2; c++) {
      gsl_vector_const_view colm2 = gsl_matrix_const_column(m2, c);
      assert((&rowm1.vector)->size == (&colm2.vector)->size);
      double vprod = 0.0;
      for(size_t i = 0; i < (&rowm1.vector)->size; i++)
	vprod += gsl_vector_get((&rowm1.vector), i) * gsl_vector_get((&colm2.vector), i); 

      gsl_matrix_set(prod, r, c, vprod);
    }
  }

  return prod;
}
