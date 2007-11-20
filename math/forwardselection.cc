#include "forwardselection.hh"

#include <iostream>
#include <gsl/gsl_statistics.h>

using std::cerr;

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

  // Compute initial variable index for forward selection
  initVar = 0;
  gsl_vector_const_view col0 = gsl_matrix_const_column(fmatrix, 0);
  double bestCorrelation = gsl_stats_correlation(ovec->data, 1, (&col0.vector)->data, 1, ovec->size);

  for(size_t vindex = 1; vindex < fmatrix->size2; vindex++) {
    // Computes the correlation between column vindex and column output
    gsl_vector_const_view col = gsl_matrix_const_column(fmatrix, vindex);
    double currentCorrelation = gsl_stats_correlation(ovec->data, 1, (&col.vector)->data, 1, ovec->size);

    if(currentCorrelation > bestCorrelation) {
      initVar = vindex;
      bestCorrelation = currentCorrelation;
    }
  }

#ifdef FSDEBUG
  cerr << "Best initial feature (highest correlation) is: " << initVar << "\n"
       << "Its correlation is " << bestCorrelation << "\n";
#endif // FSDEBUG
}

ForwardSelection::~ForwardSelection() {

  // Deleting output vectors
  gsl_vector_free(ovec);
  
  // Deleting feature matrix
  gsl_matrix_free(fmatrix);
}

vector<int> ForwardSelection::run(double fin) {

  vector<int> EMPTY;

  return EMPTY;

}
