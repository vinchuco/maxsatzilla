#include "ridgeregression.hh"

#include <gsl/gsl_linalg.h>

#include <iostream>
#include <fstream>
#include <cassert>

using std::ofstream;
using std::cerr;

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
  
#ifdef RRDEBUG
  cerr << "PHI (" << phi->size1 << ", " << phi->size2 << "):\n";
  gsl_matrix_fprintf(stderr, phi, "%f");
#endif

  // create phi transpose
  gsl_matrix *phit = gsl_matrix_alloc(phi->size2, phi->size1);
  gsl_matrix_transpose_memcpy(phit, phi);

#ifdef RRDEBUG
  cerr << "PHI^t (" << phit->size1 << ", " << phit->size2 << "):\n";
  gsl_matrix_fprintf(stderr, phit, "%f");
#endif

  // initialize delta matrix
  gsl_matrix *deltam = gsl_matrix_alloc(data.getNFeatures(), data.getNFeatures());
  gsl_matrix_set_identity(deltam);
  gsl_matrix_scale(deltam, delta);

#ifdef RRDEBUG
  cerr << "DELTAM (" << deltam->size1 << ", " << deltam->size2 << "):\n";
  gsl_matrix_fprintf(stderr, deltam, "%f");
#endif

  // initialize y vector as matrix
  gsl_matrix *y = gsl_matrix_alloc(data.getNRows(), 1);
  for(size_t r = 0; r < data.getNRows(); r++)
    gsl_matrix_set(y, r, 0, data.getOutputValue(r, out));

#ifdef RRDEBUG
  cerr << "Y (" << y->size1 << ", " << y->size2 << "):\n";
  gsl_matrix_fprintf(stderr, y, "%f");
#endif
  
  // Multiply phit by phi
  gsl_matrix *phit_phi = matrix_mult(phit, phi);
  
#ifdef RRDEBUG
  cerr << "PHI^t * PHI (" << phit_phi->size1 << ", " << phit_phi->size2 << "):\n";
  gsl_matrix_fprintf(stderr, phit_phi, "%f");
#endif

  // Create a matrix for deltam + phit_phi
  gsl_matrix *lf = gsl_matrix_alloc(deltam->size1, deltam->size2);
  gsl_matrix_memcpy(lf, deltam);
  gsl_matrix_add(lf, phit_phi);

#ifdef RRDEBUG
  cerr << "DELTAM + PHI^t * PHI (" << lf->size1 << ", " << lf->size2 << "):\n";
  gsl_matrix_fprintf(stderr, lf, "%f");
#endif

  // Invert lf to lfm1
  assert(lf->size1 == lf->size2);

  gsl_matrix *lflu = gsl_matrix_alloc(lf->size1, lf->size2);
  gsl_matrix_memcpy(lflu, lf);
  int signum;
  gsl_permutation *p = gsl_permutation_alloc(lflu->size1);

  gsl_linalg_LU_decomp(lflu, p, &signum);

  gsl_matrix *lfm1 = gsl_matrix_alloc(lflu->size1, lflu->size2);
  gsl_linalg_LU_invert(lflu, p, lfm1);

#ifdef RRDEBUG
  cerr << "(DELTAM + PHI^t * PHI)^-1 (" << lfm1->size1 << ", " << lfm1->size2 << "):\n";
  gsl_matrix_fprintf(stderr, lfm1, "%f");
#endif

  // multiply with phit and then by y
  gsl_matrix *lfm1_phit = matrix_mult(lfm1, phit);

#ifdef RRDEBUG
  cerr << "(DELTAM + PHI^t * PHI)^-1 * PHIt (" << lfm1_phit->size1 << ", " << lfm1_phit->size2 << "):\n";
  gsl_matrix_fprintf(stderr, lfm1_phit, "%f");
#endif

  gsl_matrix *final = matrix_mult(lfm1_phit, y);

#ifdef RRDEBUG
  cerr << "(DELTAM + PHI^t * PHI)^-1 * PHIt * Y (" << final->size1 << ", " << final->size2 << "):\n";
  gsl_matrix_fprintf(stderr, final, "%f");
#endif

  // Free matrices
  gsl_matrix_free(phi);
  gsl_matrix_free(deltam);
  gsl_matrix_free(phit);
  gsl_matrix_free(y);
  gsl_matrix_free(phit_phi);
  gsl_matrix_free(lf);
  gsl_matrix_free(lfm1);
  gsl_matrix_free(lflu);
  gsl_matrix_free(lfm1_phit);
  gsl_permutation_free(p);

  // Let's output this final matrix which should be m x 1.
  assert(final->size1 == data.getNFeatures());
  assert(final->size2 == 1);
  ofstream file;
  file.open(path.c_str());

  // Generate Header Guard
  size_t slashpos = path.find_last_of("/");
  string guard(path, slashpos+1);
  for(size_t i = 0; i < guard.size(); i++) {
    if(isalpha(guard[i])) guard[i] = toupper(guard[i]); 
    else if(guard[i] == '.') guard[i] = '_';
  }

  file << "#ifndef " << guard << "\n"
       << "#define " << guard << "\n"
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
