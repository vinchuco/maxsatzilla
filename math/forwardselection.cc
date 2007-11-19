#include "forwardselection.hh"

ForwardSelection::ForwardSelection(const MSZDataSet &ds, size_t output) {

  /// \todo No error checking is being done on GSL output
  
  // Copying the output vector of interest
  ovec = gsl_vector_alloc(ds.getNRows());
  
  for(size_t i = 0; i < ds.getNRows(); i++)
    gsl_vector_set(ovec, i, ds.getOutputValue(i, output));

  // Copying the matrix
  fmatrix = gsl_matrix_alloc(ds.getNRows(), ds.getNFeatures());

  for(size_t r = 0; r < ds.getNRows(); r++)
    for(size_t c = 0; c < ds.getNFeatures; c++) 
      gsl_matrix_set(fmatrix, r, c, ds.getFeatureValue(r, c));

  // Compute initial variable index for forward selection
  size_t bestVar = 0;
  gsl_vector_const_view col0 = gsl_matrix_const_column(fmatrix, 0);
  double bestCorrelation = gsl_stats_correlation(ovec, 1, &col0.vector);

}

ForwardSelection::~ForwardSelection() {

  // Deleting output vectors
  gsl_vector_free(ovec);
  
  // Deleting feature matrix
  gsl_matrix_free(fmatrix);
}

double ForwardSelection::run(double fin) {

  

}
