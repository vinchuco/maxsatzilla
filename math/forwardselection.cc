#include "forwardselection.hh"

#include <iostream>
#include <iterator>
#include <cassert>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_multifit.h>

#ifdef OLDGSL
#include "gslutils.hh"
#endif // OLDGSL

using std::cerr;
using std::cout;

ForwardSelection::ForwardSelection(const MSZDataSet &ds, size_t output) {

  /// \todo No error checking is being done on GSL output
  
  // Copying the output vector of interest
  ovec = gsl_vector_alloc(ds.getNRows());
  
  for(size_t i = 0; i < ds.getNRows(); i++)
    gsl_vector_set(ovec, i, ds.getOutputValue(i, output));

  // Copying the matrix
  fmatrix = gsl_matrix_alloc(ds.getNRows(), ds.getNFeatures());

  for(size_t r = 0; r < ds.getNRows(); r++)
    for(size_t c = 0; c < ds.getNFeatures(); c++) 
      gsl_matrix_set(fmatrix, r, c, ds.getFeatureValue(r, c));

  // Printing Matris
  gsl_matrix_fprintf(stderr, fmatrix, "%f");

  // Compute initial variable index for forward selection
  initRegressor = 0;
  gsl_vector_const_view col0 = gsl_matrix_const_column(fmatrix, 0);
  double bestCorrelation = fabs(gsl_stats_correlation(ovec->data, 1, (&col0.vector)->data, 1, ovec->size));
  
#ifdef FSDEBUG
    cerr << "corr(0)=" << bestCorrelation << " ";
#endif // FSDEBUG

  for(size_t rindex = 1; rindex < fmatrix->size2; rindex++) {
    // Computes the correlation between column vindex and column output
    gsl_vector_const_view col = gsl_matrix_const_column(fmatrix, rindex);
    double posCurrentCorrelation = fabs(gsl_stats_correlation(ovec->data, 1, (&col.vector)->data, 1, ovec->size));
    
#ifdef FSDEBUG
    cerr << "corr(" << rindex << ")=" << posCurrentCorrelation << " ";
#endif // FSDEBUG

    if(posCurrentCorrelation > bestCorrelation) {
      initRegressor = rindex;
      bestCorrelation = posCurrentCorrelation;
#ifdef FSDEBUG
      cerr << "[B] ";
#endif // FSDEBUG
    }
  }
  
#ifdef FSDEBUG
  cerr << "Best initial feature (highest correlation) is: " << initRegressor << "\n"
       << "Its correlation is " << bestCorrelation << "\n\n";
#endif // FSDEBUG
  
  // Generates model for initial regressor
  double c0, c1, cov00, cov01, cov11, SSe;
  gsl_vector_const_view initRegressorCol = gsl_matrix_const_column(fmatrix, initRegressor);
  gsl_fit_linear((&initRegressorCol.vector)->data, 1, ovec->data, 1, ovec->size, 
		 &c0, &c1, &cov00, &cov01, &cov11, &SSe);

#ifdef FSDEBUG
  cerr << "Best fit using initial regressor: Y = " << c0 << " + " << c1 << " X\n"
       << "Covariance matrix: \n"
       << "[ " << cov00 << ", " << cov01 << "\n"
       << "  " << cov01 << ", " << cov11 << "]\n"
       << "SSe = " << SSe << "\n\n";
#endif // FSDEBUG

  // Computing the regression sum of squares SSr of the model
  // SSr = SSt - SSe (SSe is sumsq given by regression function)
  // SSt is given by Sum (yi - mean(yi))^2
  SSt = 0.0;
  const double outmean = gsl_stats_mean(ovec->data, 1, ovec->size);
  for(size_t oi = 0; oi < ovec->size; oi++)
    SSt += gsl_pow_2(gsl_vector_get(ovec, oi) - outmean);

  initSSr = SSt - SSe;
  
#ifdef FSDEBUG
  cerr << "SSt = " << SSt << "\n"
       << "SSr = " << initSSr << "\n\n";
#endif // FSDEBUG
}

ForwardSelection::~ForwardSelection() {

  // Deleting output vectors
  gsl_vector_free(ovec);
  
  // Deleting feature matrix
  gsl_matrix_free(fmatrix);
}

vector<size_t> ForwardSelection::run(double fin) {

  // Current model SSr
  double modelSSr = initSSr;

  // Vector of regressors in model
  vector<bool> isInModel(fmatrix->size2, false);
  isInModel[initRegressor] = true;

#ifdef FSDEBUG
  cerr << "Doing FS on " << isInModel.size() << " regressors.\n";
#endif //FSDEBUG

  while(1) { // This is broken later on...
    vector<double> ssrvalues(fmatrix->size2, 0.0);
    bool foundNew = false; // Found regressor to add with fj > fin

    for(size_t rindex = 0; rindex < isInModel.size(); rindex++) {
      if(isInModel[rindex]) // if current regressor is in model then we continue
	continue;

      double newModelSSr;
      const double fj = computeFtest(isInModel, modelSSr, rindex, &newModelSSr);

      if(fj > fin) {
#ifdef FSDEBUG
	cerr << "Regressor " << rindex << " might be added.\n";
#endif // FSDEBUG

	foundNew = true;
	ssrvalues[rindex] = newModelSSr;
      } 
#ifdef FSDEBUG
      else 
	cerr << "Regressor " << rindex << " will not be added. Ftest below threshold.\n";
#endif // FSDEBUG
    }

    if(foundNew) {
      // Find regressor with maximum SSr
      double maxSSr = 0.0;
      size_t bestIndex = 0;
      for(size_t i = 0; i < ssrvalues.size(); i++) {
	if(ssrvalues[i] > maxSSr) {
	  maxSSr = ssrvalues[i];
	  bestIndex = i;
	}
      }
      
      assert(!isInModel[bestIndex]);
      isInModel[bestIndex] = true;

#ifdef FSDEBUG
      cerr << "*** Adding " << bestIndex << " to the model.\n"; 
#endif //FSDEBUG
    } else {
      cerr << "*** No more interesting regressors.\n";
      break;
    }

  }

  // Create result vector
  vector<size_t> regs;
  for(size_t i = 0; i < isInModel.size(); i++) 
    if(isInModel[i]) regs.push_back(i);

  cout << "Forward Selection finished by choosing variables:\n";
  copy(regs.begin(), regs.end(), std::ostream_iterator<size_t>(cout, " "));
  cout << std::endl;

  return regs;
}

/// This function computes the Ftest for adding the new regressor to the existent model.
/// Returns the result of the Ftest and as parameter returns the new regression sum of squares.
double ForwardSelection::computeFtest(const vector<bool> &model, double modelSSr, size_t newRegressorIndex, double *newSSr) {

#ifdef FSDEBUG
  cerr << "\n\n*******************************\n"
       << "Given the model with vars: ";
  for(size_t i = 0; i < model.size(); i++)
    if(model[i]) cerr << i << " ";
  cerr << "\n" << "trying to add " << newRegressorIndex << "\n";
#endif // FSDEBUG

  size_t numberRegressors = 0;
  for(size_t i = 0; i < model.size(); i++)
    if(model[i]) numberRegressors++;
  numberRegressors++; // The new model will have all previous regressors plus another one.

  const size_t numberParams = numberRegressors + 1;

  // Generate matrix of features for the new model
  // We are going to create this matrix by allocating it first and then
  // copying column by column using views. This seems to be the most efficient 
  // way to do it.
  // Note AGAIN the number of parameters (B) is the number of regressors + 1
  gsl_matrix *matrix = gsl_matrix_alloc(fmatrix->size1, numberParams);
  size_t setColumn = 1; // Next column to set of matrix

  // Let's set the 0th column to 1.
  gsl_vector *vec1 = gsl_vector_alloc(ovec->size);
  gsl_vector_set_all(vec1, 1.0);
  gsl_matrix_set_col(matrix, 0, vec1);
  gsl_vector_free(vec1);
 
  // Set the rest of the matrix.
  for(size_t i = 0; i < model.size(); i++) {
    if(model[i] || i == newRegressorIndex) {
      gsl_vector_const_view col = gsl_matrix_const_column(fmatrix, i);
      gsl_matrix_set_col(matrix, setColumn, &col.vector);
      setColumn++;
    }
  }

  assert(setColumn == numberRegressors+1);

  // Now that matrix is set we can perform the regression
  // Note AGAIN the number of parameters (B) is the number of regressors + 1
  gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(ovec->size, numberParams);

  // Initialise parameter vector
  gsl_vector *betas = gsl_vector_alloc(numberParams);
  gsl_matrix *cov = gsl_matrix_alloc(numberParams, numberParams);
  double SSe;
  gsl_multifit_linear(matrix, ovec, betas, cov, &SSe, work);
  gsl_multifit_linear_free(work);
  
  // Now lets analyse the results.
  *newSSr = SSt - SSe;
  
  double ftest = (*newSSr - modelSSr) / (SSe / ovec->size);

#ifdef FSDEBUG
  cerr << "Multiple Regression finished, parameter vector is:\n[ ";
  for(size_t i = 0; i < betas->size; i++)
    cerr << gsl_vector_get(betas, i) << " "; 
  cerr << "]\n"
       << "SSe = " << SSe << "\n"
       << "R-Sq = " << (double)(1.0-(SSe / SSt)) << "\n"
       << "R-Sq(adj) = " << (double)(1.0 - ((SSe / (fmatrix->size1 - numberParams)) / (SSt / (fmatrix->size1 - 1)))) << "\n"
       << "FTest value = " << ftest << "\n"
       << "*******************************\n\n";
#endif // FSDEBUG
  
  gsl_matrix_free(matrix);
  gsl_matrix_free(cov);
  gsl_vector_free(betas);
  
  return ftest;
}
