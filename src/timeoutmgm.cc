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

  uint nrows = data.getNRows();
  vector<uint> timeoutIdx; // Indexes of rows with timeouts
  for(uint r = 0; r < nrows; ++r)
    if(data.getOutputValue(r) == timeout)
      timeoutIdx.push_back(r);

  cout << "Timeout prediction ";
  while(err > maxerr) { // This is where the magic happens    
    cout << ".";
    iter++;

    // Step 1: Run a fit on current data
    
    

    // Step 2.1: Predict values for timedout instances with previous fit


    // Step 2.3: Compute error


    // Step 2.2: Update values


  }
  cout << iter << "\n";
}
