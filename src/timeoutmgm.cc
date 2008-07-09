#include "timeoutmgm.hh"

#include "logmgm.hh"

#include <iostream>
#include <limits>
#include <vector>

#include <gsl/gsl_math.h>
#include <gsl/gsl_multifit.h>

using std::vector;
using std::cout;

/* 
 * In general terms the prediction works as follows:
 * Iterative Methods: 
 *
 ** Iteration 0: 
 *** Step 1:
 *** obtain an initial least squares fit of the data treating 
 *** timeouts as if the instances has finished in the timeout time.
 **
 *** Step 2:
 *** use the fit to estimate values for the timedout instances.
 **
 *
 ** Iteration n+1:
 *** Step 1:
 *** Use the values obtained from fit in iteration n to do a new fitting.
 **
 *** Step 2:
 *** use new fit to update values for timedout instances.
 **
 *
 * Repeat iterations until the values for the timedout instances converge.
 */

void TimeoutMgm::predictTimeouts(double **fmatrix, uint nrows, uint ncols, double *ovec, uint timeout, double maxerr) {

  // We use multi-parameter least squares fitting from GSL
  uint iter = 0;
  double err = std::numeric_limits<double>::max();

  vector<uint> timeoutIdx; // Indexes of rows with timeouts
  for(uint r = 0; r < nrows; ++r)
    if(ovec[r] == timeout)
      timeoutIdx.push_back(r);

  *(LogMgm::Instance()) << "Predicting Timeouts\n"
			<< "Dataset has " << timeoutIdx.size() << " / " << nrows << " timeouts\n";

  gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(nrows, ncols+1);
  gsl_vector *vec1 = gsl_vector_alloc(nrows);
  gsl_vector_set_all(vec1, 1.0);

  gsl_matrix *matrix = gsl_matrix_alloc(nrows, ncols+1);
    
  // Set first column to ones
  gsl_matrix_set_col(matrix, 0, vec1);
  gsl_vector_free(vec1);

  // Set rest of matrix
  for(uint r = 0; r < nrows; ++r)
    for(uint c = 0; c < ncols; ++c)
      gsl_matrix_set(matrix, r, c+1, fmatrix[r][c]);

  gsl_vector *y = gsl_vector_alloc(nrows);
  gsl_vector *betas = gsl_vector_alloc(ncols+1);
  gsl_matrix *cov = gsl_matrix_alloc(ncols + 1, ncols + 1);

  *(LogMgm::Instance()) << "Timeout prediction ";
  while(err > maxerr) { // This is where the magic happens    
    *(LogMgm::Instance()) << ".";
    iter++;

    if(iter % 100 == 0)
      *(LogMgm::Instance()) << err << "\n";

    //**********************************
    // Step 1: Run a fit on current data
    for(uint r = 0; r < nrows; ++r)
      gsl_vector_set(y, r, ovec[r]);

    double chisq;
    gsl_multifit_linear(matrix, y, betas, cov, &chisq, work);
    
    //******************************************************************
    // Step 2.1: Predict values for timedout instances with previous fit
    
    vector<double> predictions(timeoutIdx.size(), 0.0);

    for(uint idx = 0; idx < timeoutIdx.size(); ++idx) {
      predictions[idx] += gsl_vector_get(betas, 0);
      for(uint f = 1; f < ncols + 1; ++f)
	predictions[idx] += gsl_vector_get(betas, f) * fmatrix[timeoutIdx[idx]][f-1];
    }

    // Step 2.3: Compute error
    err = 0.0;
    for(uint i = 0; i < predictions.size(); ++i) 
      err += fabs(predictions[i] - ovec[timeoutIdx[i]]);
    err /= predictions.size();

    // Step 2.2: Update values
    for(uint idx = 0; idx < timeoutIdx.size(); ++idx) 
      ovec[timeoutIdx[idx]] =  predictions[idx];
  }
  *(LogMgm::Instance()) << iter << "(error: " << err << ")\n";

  gsl_multifit_linear_free(work);
  gsl_matrix_free(matrix);
  gsl_vector_free(y);
  gsl_matrix_free(cov);
  gsl_vector_free(betas);

  uint fails = 0;
  for(uint idx = 0; idx < timeoutIdx.size(); ++idx) {
    if(ovec[timeoutIdx[idx]] < timeout) {
      *(LogMgm::Instance()) << "After prediction " << timeoutIdx[idx] << " became " << ovec[timeoutIdx[idx]] << " (timeout: " << timeout << ")\n";
      fails++;
    }
  }
  *(LogMgm::Instance()) << "Prediction predicted " << fails << " timedout instances as non-timeouts.\n";
}
