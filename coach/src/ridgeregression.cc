#include "ridgeregression.hh"

#include "gsladdon.hh"
#include "logmgm.hh"

#include <gsl/gsl_linalg.h>

#include <iostream>
#include <cassert>

using std::cerr;

RidgeRegression::RidgeRegression(const MSZDataSet &data) :
  LearningAlgorithm(data), delta(1.0) { }

RidgeRegression::~RidgeRegression() { }

Model RidgeRegression::run() {

  const uint nbParams = data.getNFeatures()+1;

  // Create PHI matrix
  gsl_matrix *phi = gsl_matrix_alloc(data.getNRows(), nbParams);

  // populate phi
  for(uint r = 0; r < phi->size1; r++) {
    for(uint c = 0; c < phi->size2; c++) {
      if(c == 0)
	gsl_matrix_set(phi, r, c, 1.0);
      else
	gsl_matrix_set(phi, r, c, data.getFeatureValue(r, c-1));
    }
  }
  
  *(LogMgm::Instance()) << GslAddon::matrix_toString(phi, "Phi");

  // create phi transpose
  gsl_matrix *phit = gsl_matrix_alloc(phi->size2, phi->size1);
  gsl_matrix_transpose_memcpy(phit, phi);

  *(LogMgm::Instance()) << GslAddon::matrix_toString(phit, "Phi^t");

  // initialize delta matrix
  gsl_matrix *deltam = gsl_matrix_alloc(nbParams, nbParams);
  gsl_matrix_set_identity(deltam);
  gsl_matrix_scale(deltam, delta);

  *(LogMgm::Instance()) << GslAddon::matrix_toString(deltam, "DeltaM");

  // initialize y vector as matrix
  gsl_matrix *y = gsl_matrix_alloc(data.getNRows(), 1);
  for(uint r = 0; r < data.getNRows(); r++)
    gsl_matrix_set(y, r, 0, data.getOutputValue(r));

  *(LogMgm::Instance()) << GslAddon::matrix_toString(y, "Y");
  
  // Multiply phit by phi
  gsl_matrix *phit_phi = GslAddon::matrix_mult(phit, phi);

  *(LogMgm::Instance()) << GslAddon::matrix_toString(phit_phi, "Phi^t * Phi");

  // Create a matrix for deltam + phit_phi
  gsl_matrix *lf = gsl_matrix_alloc(deltam->size1, deltam->size2);
  gsl_matrix_memcpy(lf, deltam);
  gsl_matrix_add(lf, phit_phi);

  *(LogMgm::Instance()) << GslAddon::matrix_toString(lf, "DeltaM + Phi^t * Phi");

  // Invert lf to lfm1
  assert(lf->size1 == lf->size2);

  gsl_matrix *lflu = gsl_matrix_alloc(lf->size1, lf->size2);
  gsl_matrix_memcpy(lflu, lf);
  int signum;
  gsl_permutation *p = gsl_permutation_alloc(lflu->size1);

  gsl_linalg_LU_decomp(lflu, p, &signum);

  gsl_matrix *lfm1 = gsl_matrix_alloc(lflu->size1, lflu->size2);
  gsl_linalg_LU_invert(lflu, p, lfm1);

  *(LogMgm::Instance()) << GslAddon::matrix_toString(lfm1, "(DeltaM + Phi^t * Phi)^-1");

  // multiply with phit and then by y
  gsl_matrix *lfm1_phit = GslAddon::matrix_mult(lfm1, phit);

  *(LogMgm::Instance()) << GslAddon::matrix_toString(lfm1_phit, "(DeltaM + Phi^t * Phi)^-1 * Phi^t");

  gsl_matrix *final = GslAddon::matrix_mult(lfm1_phit, y);

  *(LogMgm::Instance()) << GslAddon::matrix_toString(final, "(DeltaM + Phi^t * Phi)^-1 * Phi^t * Y");

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
  assert(final->size1 == nbParams);
  assert(final->size2 == 1);

  Model m;
  m.addRegressor(gsl_matrix_get(final, 0, 0));
  for(uint i = 1; i < final->size1; i++) 
    m.addRegressor(gsl_matrix_get(final, i, 0), data.getColLabel(i-1));

  gsl_matrix_free(final);
  return m;
}
