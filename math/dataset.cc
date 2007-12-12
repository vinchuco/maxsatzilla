#include <iostream>
#include <fstream>

#include <iostream>
#include <iterator>
#include <cstdlib>
#include <cassert>
#include <cmath>

#include <gsl/gsl_math.h>
#include <gsl/gsl_combination.h>

#include "dataset.hh"

using std::cout;
using std::cerr;
using std::ofstream;

MSZDataSet::MSZDataSet(const double* const* m, uint nrows, uint ncols, const string* mlabels, const double* ov, const string& olabel)
  : nrows(nrows), rfeatures(ncols), ncols(ncols), stdDone(false), oStdDone(false) {

  // Recomputing matrix to be made of columns instead of lines
  matrix = new double* [ncols];
  for(uint c = 0; c < ncols; c++) {
    // Copy label also
    labels.push_back(mlabels[c]);
    // Create column
    setMColumn(c, new double [nrows]);
  }
  
  // Copy the elements
  for(uint r = 0; r < nrows; r++) 
    for(uint c = 0; c < ncols; c++) 
      setMValue(r, c, m[r][c]);  

  // Create the output vector
  ovec = new double [nrows];

  // Copy label
  vecLabel = olabel;
  for(uint i = 0; i < nrows; i++) {
    // Handle value
    double val = ov[i];
 
    if(val < 0.01) {
      MSZWarn("Output %u of %s is less than 0.01. Resizing to 0.01.", i, olabel.c_str());
      val = 0.01;
    }   
    setVValue(i, val);
  }
}
  
MSZDataSet::MSZDataSet(const MSZDataSet& ds) 
  : nrows(ds.nrows), rfeatures(ds.rfeatures), ncols(ds.ncols), stdDone(ds.stdDone), oStdDone(ds.oStdDone) {

  // Now we just need to copy the matrix.
  matrix = new double* [ncols];
  for(uint c = 0; c < ncols; c++)
    setMColumn(c, new double [nrows]);

  for(uint r = 0; r < nrows; r++)
    for(uint c = 0; c < ncols; c++)
      setMValue(r, c, ds.getMValue(r, c));

  // Copy the output vector
  ovec = new double [ncols];
  for(uint i = 0; i < ncols; i++)
    setVValue(i, ds.getVValue(i));

  // Copy the labels
  labels = ds.labels;
  vecLabel = ds.vecLabel;
  
}

MSZDataSet::~MSZDataSet() {

  // Please God, do not let this Seg Fault!

  // Clean up matrix by deleting each line.
  for(uint c = 0; c < ncols; c++) 
    delete[] getMColumn(c);
	 
  delete[] matrix;
  delete[] ovec;
}

void MSZDataSet::printRawMatrix() {

  cerr << "Printing all matrix [" << nrows << ", " << ncols << "]\n";
  for(uint r = 0; r < nrows; r++) {
    cerr << "[Row " << r << "] ";
    for(uint c = 0; c < ncols; c++) {
      cerr << getMValue(r, c) << " ";
    }
    cerr << "\n";
  }

}

void MSZDataSet::dumpPlotFiles(const string &prefix) const {
  // Dump the files for plotting features against output
  for(uint f = 0; f < ncols; f++) {
    ofstream file;
    file.open((prefix + "@" + vecLabel + "@" + labels[f] + ".dat").c_str());
    
    file << "# This file is generated by MatSATZilla\n"
	 << "# It should be used with gnuplot to plot\n"
	 << "# the feature " << labels[f] << " against\n"
	 << "# the output " << vecLabel << "\n"
	 << "#\n"
      // Outputting timestamp could be nice.
      //<< "# Timestamp: " << 
	 << "#\n"
	 << "# " << labels[f] << "\t\t" << vecLabel << "\n";
    
    // For each instance
    for(uint i = 0; i < nrows; i++) 
      file << getMValue(i, f) << "\t\t" << getVValue(i) << "\n";
    
    file.close();
  }
}

void MSZDataSet::printSolverStats(uint timeout) {

  uint nbTimeouts = 0;

  for(uint i = 0; i < nrows; i++) {
    if(getVValue(i) == timeout)
      nbTimeouts++;
  }

  cout << "Solver: " << vecLabel << "\n"
       << "\t\tTimeouts: " << nbTimeouts << " (" << (((double)(nbTimeouts)) / nrows)*100.0 << ")\n"
       << "\t\tUsable Instances: " << nrows - nbTimeouts << " (" << (((double)(nrows - nbTimeouts)) / nrows)*100.0 << ")\n";

}

double MSZDataSet::getOutputValue(uint row) const {
  assert(row < nrows);
  return getVValue(row);
}

double MSZDataSet::getFeatureValue(uint row, uint col) const {
  assert(row < nrows);
  assert(col < ncols);
  return getMValue(row, col);
}

void MSZDataSet::standardize() {
  
  cout << "Standardizing features forall k . x_k = `q(x_k - x')/sdv_i(x_i)...\n";

  if(!stdDone) {
    for(uint c = 0; c < ncols; c++) {

      // Compute column mean and compute column variance.
      double mean = 0.0;
      for(uint r = 0; r < nrows; r++) 
	mean += getMValue(r, c);
      mean /= nrows;

      cout << "[Feature " << labels[c] << "] Centering = " << mean << "; ";

      // Compute column standard deviation. 
      double sdv = 0.0;
      for(uint r = 0; r < nrows; r++) 
	sdv += gsl_pow_2(getMValue(r, c) - mean);
      sdv /= nrows;
      sdv = sqrt(sdv);

      cout << "Scaling = " << sdv << "\n";

      for(uint r = 0; r < nrows; r++) 
	setMValue(r, c, (getMValue(r, c) - mean) / sdv);
	
    }
    
  }
  stdDone = true;

  cout << "DONE\n";

}

void MSZDataSet::removeFeatures(const vector<uint> &keepVec) {
  // We need to remove the feature indexes in vec from the current data.
  
  cout << "Keeping features in dataset: ";

  // Create the indices to keep
  // by: 
  // - incrementing all indices by output
  // - sorting
  // - adding output indices to the beginning of vector
  vector<uint> vec(keepVec);
  
  if(vec.size() == ncols) { 
    cout << "ALL\n";
    return;
  } else {
    copy(vec.begin(), vec.end(), std::ostream_iterator<uint>(cout, " "));
    cout << std::endl;
  }

  sort(vec.begin(), vec.end());        // sorts 

  // Count the number of raw features about to be removed
  uint rawToRemove = 0;
  for(vector<uint>::const_iterator it = vec.begin(); 
      it != vec.end();
      it++)
    if(*it < rfeatures) 
      rawToRemove++;

  rfeatures -= rawToRemove;
  const uint oldNCols = ncols;
  ncols = vec.size();
  
  // Now, we just need to delete the columns referenced in the vector
  // and resize the main column array (delete + new, no realloc!).
  double **newMatrix = new double* [ncols];
  // Note that the vector indices denote the destination column
  // in the new matrix and the vector contents the origin column on the
  // old matrix. This means that we need to copy the column v[i] of old matrix
  // to column i of the new matrix.
  for(uint c = 0; c < ncols; c++) {
    newMatrix[c] = getMColumn(vec[c]);
    setMColumn(vec[c], 0); // Those which are not 0 are the columns 
                           // that should be deleted afterwards
  }
  for(uint c = 0; c < oldNCols; c++)
    if(getMColumn(c) != 0) delete[] getMColumn(c);
  
  delete[] matrix;
  matrix = newMatrix;
}

void MSZDataSet::standardizeOutput() {

  cout << "Standardizing outputs... ";

  if(!stdDone) 
    for(uint i = 0; i < nrows; i++) 
      setVValue(i, log(getVValue(i)));

  oStdDone = true;

  cout <<  "DONE\n";

}

void MSZDataSet::expand(uint n) { 
  vector<uint> pvec(ncols); 
  for(uint i = 0; i < pvec.size(); i++)
    pvec[i] = i;

  expandOnPartition(n, pvec);
}

void MSZDataSet::expand(uint n, const vector<vector<uint> > &pvec) {
  for(uint i = 0; i < pvec.size(); i++)
    expandOnPartition(n, pvec[i]);
}

void MSZDataSet::expandOnPartition(uint k, const vector<uint> &pvec) {
  
  // expansion is quadratic at the minimum and 
  // and be bigger than partition size.
  assert(k >= 2 && k <= pvec.size());

  // Standardize features
  standardize();

  // Output information
  cout << "Expanding feature set by order " << k << " on partition:\n";
  copy(pvec.begin(), pvec.end(), std::ostream_iterator<uint>(cout, " "));
  cout << "\n";

  // Reallocate the data.
  // If we have N features, then, to expand to a order-K polynomial
  // we are adding 
  // n + (n !) / (k! (n-k!))
  uint num = 1;
  for(uint i = 0; i < k; i++) num *= rfeatures - i; // for big k hell breaks loose
  uint den = 1;
  for(uint i = k; i > 0; i--) den *= i; 
  uint nNewF = rfeatures;
  nNewF += num / den;
  ncols += nNewF; // Update number of columns

  cout << "Expansion increases feature set by " << nNewF << " columns " << " summing a total of " << ncols << " columns.\n";
  
  // Reallocating main vector and copying all the others to their initial places.
  double **newMatrix = new double* [ncols];
  for(uint c = 0; c < ncols - nNewF; c++)
    newMatrix[c] = getMColumn(c);
  delete[] matrix;
  matrix = newMatrix;

  for(uint nc = ncols - nNewF; nc < ncols; nc++)
    setMColumn(nc, new double [nrows]);

  // Compute the cross products
  // To do this we compute all the indices combinations and use them
  // to compute the cross product on columns defined in pvec.
  uint currColumn = rfeatures;
  const uint n = pvec.size();
  gsl_combination * c;
  c = gsl_combination_calloc (n, k);
  uint *ind = new uint [k];
  
  // Compute cross product for initial configuration
  // Now using the very nice combination structure from GSL.
  // Up to rev. 121, we used custom combination generator.
  do {
    for(uint i = 0; i < k; i++)
      ind[i] = gsl_combination_get(c, i);

    // Compute cross product in current configuration
    for(uint r = 0; r < nrows; r++) {
      const double cprod = computeCrossProduct(r, ind, k, pvec);
      setMValue(r, currColumn, cprod);
      
      // Update labels
      assert(currColumn == labels.size());
      string newLabel = "CP(";
      for(uint i = 0; i < k; i++) {
	newLabel += labels[ind[i]];
	if(i != k-1) newLabel += ", ";
      }
      newLabel += ")";
      labels.push_back(newLabel);
    }
    currColumn++;
  } while(gsl_combination_next (c) == GSL_SUCCESS);
  gsl_combination_free (c);
  delete[] ind;

  // Compute Powers
  for(uint i = 0; i < pvec.size(); i++) {
    for(uint r = 0; r < nrows; r++) {
      // Compute powers for pvec[i] feature
      setMValue(r, currColumn, gsl_pow_int(getMValue(r, pvec[i]), k));

      // Set label
      assert(currColumn == labels.size());
      string newLabel = "POW(";
      newLabel += labels[pvec[i]];
      newLabel += ", ";
      newLabel += k;
      newLabel += ")";
      labels.push_back(newLabel);
    }
  }

}

double MSZDataSet::computeCrossProduct(uint r, uint *ind, uint k, const vector<uint> &vec) {
  double cp = 1.0; // Cross-product
  for(uint i = 0; i < k; i++)
    cp *= getMValue(r, vec[ind[i]]);
  return cp;
}

void MSZDataSet::removeTimeouts(uint timeout) {

  if(oStdDone) {
    cerr << "Output standardization already performed. Cannot remove timeouts.\n";
    return;
  }

  // Count and keep rows without timeouts
  vector<uint> keepRows;
  for(uint r = 0; r < nrows; r++) {
    if(getVValue(r) != timeout) 
      keepRows.push_back(r);
  }

  if(keepRows.size() == nrows) {
    cout << "There are no rows to remove.\n";
    return;
  }

  cout << "Removing " << nrows - keepRows.size() 
       << " (" <<  (double)(nrows - keepRows.size()) / nrows << "%) of dataset due to timeout.\n";

  // Allocating new matrix
  double **newMatrix = new double* [ncols];
  for(uint c = 0; c < ncols; c++)
    newMatrix[c] = new double [keepRows.size()];

  nrows = keepRows.size();

  for(uint c = 0; c < ncols; c++) {
    uint nr = 0;
    for(vector<uint>::const_iterator rptr = keepRows.begin();
	rptr != keepRows.end();
	rptr++) {
      assert(*rptr >= nr);

      newMatrix[c][nr++] = getMValue(*rptr, c);
    }
  }

  // delete old matrix
  for(uint c = 0; c < ncols; c++)
    delete[] matrix[c];
  delete[] matrix;

  matrix = newMatrix;

}

/////////////////////////////////////////
/////////////////////////////////////////
//
// API Entrace Function for Data Set creation
//
/////////////////////////////////////////
/////////////////////////////////////////

MSZDataSet *createDataSet(const double* const* matrix, 
			  uint nrows, 
			  uint ncols, 
			  const string* labels, 
			  const double* ovec, 
			  const string& ovecLabel) {
  assert(matrix != 0);
  
#ifndef NDEBUG
  for(uint r = 0; r < nrows; r++)
    assert(matrix[r] != 0);
#endif // NDEBUG

  // The call!
  MSZDataSet *ds = new MSZDataSet(matrix, nrows, ncols, labels, ovec, ovecLabel);
  
  return ds;
}
