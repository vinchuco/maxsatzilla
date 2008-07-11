#include "forwardselection.hh"
#include "logmgm.hh"

#include <iostream>
#include <iterator>
#include <cassert>
#include <utility>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_multifit.h>

using std::cout;
using std::pair;
using std::make_pair;

ForwardSelection::ForwardSelection(const DataSet &ds) {

  /// \todo No error checking is being done on GSL output
  
  // Copying the output vector of interest
  ovec = gsl_vector_alloc(ds.getNRows());
  
  for(uint i = 0; i < ds.getNRows(); i++)
    gsl_vector_set(ovec, i, ds.getOutputValue(i));

  // Copying the matrix
  fmatrix = gsl_matrix_alloc(ds.getNRows(), ds.getNFeatures());

  for(uint r = 0; r < ds.getNRows(); r++)
    for(uint c = 0; c < ds.getNFeatures(); c++) 
      gsl_matrix_set(fmatrix, r, c, ds.getFeatureValue(r, c));

  // Compute initial variable index for forward selection
  initRegressor = 0;
  gsl_vector_const_view col0 = gsl_matrix_const_column(fmatrix, 0);
  double bestCorrelation = fabs(gsl_stats_correlation(ovec->data, 1, (&col0.vector)->data, 1, ovec->size));
  
  *(LogMgm::Instance()) << "corr(0)=" << bestCorrelation << " ";

  for(uint rindex = 1; rindex < fmatrix->size2; rindex++) {
    // Computes the correlation between column vindex and column output
    gsl_vector_const_view col = gsl_matrix_const_column(fmatrix, rindex);
    double posCurrentCorrelation = fabs(gsl_stats_correlation(ovec->data, 1, (&col.vector)->data, 1, ovec->size));
    
    *(LogMgm::Instance()) << "corr(" << rindex << ")=" << posCurrentCorrelation << " ";

    if(posCurrentCorrelation > bestCorrelation) {
      initRegressor = rindex;
      bestCorrelation = posCurrentCorrelation;
      *(LogMgm::Instance()) << "[B] ";
    }
  }
  
  *(LogMgm::Instance()) << "\nBest initial feature (highest correlation) is: " << initRegressor << "\n"
			<< "Its correlation is " << bestCorrelation << "\n\n";
  
  // Generates model for initial regressor
  double c0, c1, cov00, cov01, cov11, SSe;
  gsl_vector_const_view initRegressorCol = gsl_matrix_const_column(fmatrix, initRegressor);
  gsl_fit_linear((&initRegressorCol.vector)->data, 1, ovec->data, 1, ovec->size, 
		 &c0, &c1, &cov00, &cov01, &cov11, &SSe);

  *(LogMgm::Instance()) << "Best fit using initial regressor: Y = " << c0 << " + " << c1 << " X\n"
			<< "Covariance matrix: \n"
			<< "[ " << cov00 << ", " << cov01 << "\n"
			<< "  " << cov01 << ", " << cov11 << "]\n"
			<< "SSe = " << SSe << "\n\n";
  
  // Computing the regression sum of squares SSr of the model
  // SSr = SSt - SSe (SSe is sumsq given by regression function)
  // SSt is given by Sum (yi - mean(yi))^2
  SSt = 0.0;
  const double outmean = gsl_stats_mean(ovec->data, 1, ovec->size);
  for(uint oi = 0; oi < ovec->size; oi++)
    SSt += gsl_pow_2(gsl_vector_get(ovec, oi) - outmean);

  initSSr = SSt - SSe;
  
  *(LogMgm::Instance()) << "SSt = " << SSt << "\n"
			<< "SSr = " << initSSr << "\n\n";
}

ForwardSelection::~ForwardSelection() {

  // Deleting output vectors
  gsl_vector_free(ovec);
  
  // Deleting feature matrix
  gsl_matrix_free(fmatrix);
}

vector<uint> ForwardSelection::run(double fin) {

  // Current model SSr
  double modelSSr = initSSr;

  // Vector of regressors in model
  vector<bool> isInModel(fmatrix->size2, false);
  isInModel[initRegressor] = true;

  *(LogMgm::Instance()) << "Doing FS on " << isInModel.size() << " regressors.\n"
			<< "Forward Selection Variables: " << initRegressor << " ";

  while(1) { // This is broken later on...
    vector<pair<uint, double> > ssrvalues;
    bool foundNew = false; // Found regressor to add with fj > fin

    for(uint rindex = 0; rindex < isInModel.size(); rindex++) {
      if(isInModel[rindex]) // if current regressor is in model then we continue
	continue;

      double newModelSSr;
      const double fj = computeFtest(isInModel, modelSSr, rindex, &newModelSSr);

      if(fj > fin) {
	foundNew = true;
	ssrvalues.push_back(make_pair(rindex, newModelSSr));
      } 
    }

    if(foundNew) {
      assert(ssrvalues.size() > 0);
      // Find regressor with maximum SSr
      double maxSSr = ssrvalues[0].second;
      uint bestIndex = ssrvalues[0].first;
      for(uint i = 1; i < ssrvalues.size(); i++) {
	if(ssrvalues[i].second > maxSSr) {
	  maxSSr = ssrvalues[i].second;
	  bestIndex = ssrvalues[i].first;
	}
      }
      
      assert(!isInModel[bestIndex]);
      isInModel[bestIndex] = true;

      cout << bestIndex << " "; 
    } else {
      cout << "*** No more interesting regressors.\n";
      break;
    }

  }

  // Create result vector
  vector<uint> regs;
  for(uint i = 0; i < isInModel.size(); i++) 
    if(isInModel[i]) regs.push_back(i);

  *(LogMgm::Instance()) << "\n";

  return regs;
}

vector<uint> ForwardSelection::runForBest(uint nvars) {
  
  if(nvars == 0) {
    MSZError("You are asking for the best 0 variables of forward selection. Are you sane?");
  } else if(nvars == 1) {
    vector<uint> regs(1, initRegressor);
    return regs;
  } else if(nvars > fmatrix->size2) {
    MSZWarn("You are asking for the best %u variables of forward selection in a system with only %lu.", nvars, (unsigned long)(fmatrix->size2));
    vector<uint> regs(fmatrix->size2, 0);
    for(uint i = 0; i < regs.size(); i++)
      regs.push_back(i);
    return regs;
  }

  // It seems we want more than 1 var.
  uint nvarsInModel = 1;
  
  // Current model SSr
  double modelSSr = initSSr;

  // Vector of regressors in model
  vector<bool> isInModel(fmatrix->size2, false);
  isInModel[initRegressor] = true;

  *(LogMgm::Instance()) << "Doing FS on " << isInModel.size() << " regressors. Selecting best " << nvars << "\n"
			<< "Forward Selection Variables: " << initRegressor << " ";

  while(nvarsInModel < nvars) { // This is broken later on...
    vector<pair<uint, double> > ssrvalues;

    for(uint rindex = 0; rindex < isInModel.size(); rindex++) {
      if(isInModel[rindex]) // if current regressor is in model then we continue
	continue;

      double newModelSSr;
      computeFtest(isInModel, modelSSr, rindex, &newModelSSr);

      ssrvalues.push_back(make_pair(rindex, newModelSSr));
    }

    // Find regressor with maximum SSr
    assert(ssrvalues.size() > 0);
    double maxSSr = ssrvalues[0].second;
    uint bestIndex = ssrvalues[0].first;
    for(uint i = 1; i < ssrvalues.size(); i++) {
      if(ssrvalues[i].second > maxSSr) {
	maxSSr = ssrvalues[i].second;
	bestIndex = ssrvalues[i].first;
      }
    }
    
    assert(!isInModel[bestIndex]);
    isInModel[bestIndex] = true;
    nvarsInModel++;
    
    *(LogMgm::Instance()) << bestIndex << " "; 
  }

  // Create result vector
  vector<uint> regs;
  for(uint i = 0; i < isInModel.size(); i++) 
    if(isInModel[i]) regs.push_back(i);

  *(LogMgm::Instance()) << "\n";

  return regs;
}

/// This function computes the Ftest for adding the new regressor to the existent model.
/// Returns the result of the Ftest and as parameter returns the new regression sum of squares.
double ForwardSelection::computeFtest(const vector<bool> &model, double modelSSr, uint newRegressorIndex, double *newSSr) {


  *(LogMgm::Instance()) << "\n\n*******************************\n"
			<< "Given the model with vars: ";
  for(uint i = 0; i < model.size(); i++)
    if(model[i]) cout << i << " ";
  *(LogMgm::Instance()) << "\n" << "trying to add " << newRegressorIndex << "\n";

  uint numberRegressors = 0;
  for(uint i = 0; i < model.size(); i++)
    if(model[i]) numberRegressors++;
  numberRegressors++; // The new model will have all previous regressors plus another one.

  const uint numberParams = numberRegressors + 1;

  // Generate matrix of features for the new model
  // We are going to create this matrix by allocating it first and then
  // copying column by column using views. This seems to be the most efficient 
  // way to do it.
  // Note AGAIN the number of parameters (B) is the number of regressors + 1
  gsl_matrix *matrix = gsl_matrix_alloc(fmatrix->size1, numberParams);
  uint setColumn = 1; // Next column to set of matrix

  // Let's set the 0th column to 1.
  gsl_vector *vec1 = gsl_vector_alloc(ovec->size);
  gsl_vector_set_all(vec1, 1.0);
  gsl_matrix_set_col(matrix, 0, vec1);
  gsl_vector_free(vec1);
 
  // Set the rest of the matrix.
  for(uint i = 0; i < model.size(); i++) {
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

  *(LogMgm::Instance()) << "Multiple Regression finished, parameter vector is:\n[ ";
  for(uint i = 0; i < betas->size; i++)
    *(LogMgm::Instance()) << gsl_vector_get(betas, i) << " "; 
  *(LogMgm::Instance()) << "]\n"
			<< "SSe = " << SSe << "\n"
			<< "R-Sq = " << (double)(1.0-(SSe / SSt)) << "\n"
			<< "R-Sq(adj) = " << (double)(1.0 - ((SSe / (fmatrix->size1 - numberParams)) / (SSt / (fmatrix->size1 - 1)))) << "\n"
			<< "FTest value = " << ftest << "\n"
			<< "*******************************\n\n";
  
  gsl_matrix_free(matrix);
  gsl_matrix_free(cov);
  gsl_vector_free(betas);
  
  return ftest;
}
