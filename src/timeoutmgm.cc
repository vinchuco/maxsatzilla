#include "timeoutmgm.hh"

#include <limits>

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


void TimeoutMgm::predictTimeouts(MSZDataSet &data, uint timeout, double maxerr) {

  // We use multi-parameter least squares fitting from GSL
  uint iter = 0;
  double err = numeric_limits<double>::max();

  const uint nrows = data.getNRows();
  const uint nfeatures = data.getNFeatures();
  vector<uint> timeoutIdx; // Indexes of rows with timeouts
  for(uint r = 0; r < nrows; ++r)
    if(data.getOutputValue(r) == timeout)
      timeoutIdx.push_back(r);

  cout << "Timeout prediction ";
  while(err > maxerr) { // This is where the magic happens    
    cout << ".";
    iter++;

    //**********************************
    // Step 1: Run a fit on current data
    gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(nrows, nfeatures+1);
    gsl_matrix *matrix = gsl_matrix_alloc(nrows, nfeatures+1);
    
    // Set first column to ones
    gsl_vector *vec1 = gsl_vector_alloc(nrows);
    gsl_vector_set_all(vec1, 1.0);
    gsl_matrix_set_col(matrix, 0, vec1);
    gsl_vector_free(vec1);

    // Set rest of matrix
    for(uint r = 0; r < nrows; ++r)
      if(uint c = 0; c < nfeatures; ++c)
	gsl_matrix_set(matrix, r, c+1, data.getFeatureValue(r, c));

    gsl_vector *y = gsl_vector_alloc(nrows);
    for(uint r = 0; r < nrows; ++r)
      gsl_vector_set(y, r, data.getOutputValue(r));

    gsl_vector *betas = gsl_vector_alloc(nfeatures+1);
    gsl_matrix *cov = gsl_matrix_alloc(nfeatures + 1, nfeatures + 1);
    double chisq;

    int fit = gsl_multifit_linear(matrix, y, betas, cov, &chisq, work);
    
    //******************************************************************
    // Step 2.1: Predict values for timedout instances with previous fit
    
    gsl_multifit_linear_free(work);
    gsl_matrix_free(matrix);
    gsl_matrix_free(cov);
    gsl_vector_free(y);
    gsl_vector_free(betas);

    // Step 2.3: Compute error


    // Step 2.2: Update values


  }
  cout << iter << "\n";
}
