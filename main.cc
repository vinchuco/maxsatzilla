#include <iostream>
#include <cassert>
#include <cmath>

#include "getfeatures_wrapper.hh"

#include "mszmodelreader.hh"

using std::cerr;
using std::cout;

/**
 * Welcome, this  is the main maxsatzilla source file.
 * After some weeks of programming from scripts, to coaching 
 * test files and mathematical functions we get to the main 
 * file of maxsatzilla. 
 *
 * Main purpose of maxsatzilla is just to read the models available,
 * compute the feature of the instance to solve,
 * run the models over the feature set,
 * and compute the expected runtime.
 */

int main(int argc, char *argv[]) {
  
  if(argc != 3) {
    cerr << "usage: maxsatzilla <modelfile> <instance.cnf>\n";
    exit(EXIT_FAILURE);
  }

  const char *instance = argv[2];

  MszModelReader mreader(argv[1]);

  vector<string> solvers = mreader.getSolvers();

  // Let's compute the features
  cout << "Computing Features...";
  map<string, double> feats = getFeatures(instance);
  cout << " DONE\n";

#ifndef NDEBUG
  for(std::map<string, double>::const_iterator it = feats.begin(); it != feats.end(); it++) 
    cout << it->first << " : " << it->second << "\n";
#endif // NDEBUG
  
  // Let's compute the model
  map<string, double> predRt;
  for(uint s = 0; s < solvers.size(); s++) {
    cout << "Computing runtime for " << solvers[s] << ": ";
    // Computing model for solver s.
    double runtime = mreader.getModelWeight(solvers[s]);
    cout << runtime << " ";
    for(map<string, double>::const_iterator it = feats.begin();
	it != feats.end();
	it++) { 
      runtime += it->second * mreader.getModelWeight(solvers[s], it->first);
      cout << "(+" << it->first << ") " << runtime << " ";
    }
    cout << "\n";
    predRt[solvers[s]] = runtime;
  }

  // Let's display the models, from best to worst.
  cout << "Predicted Runtimes:\n";
  for(map<string, double>::const_iterator it = predRt.begin();
      it != predRt.end();
      it++)
    cout << "\t" << it->first << ": " << (mreader.getOutputStd() ? exp(it->second) : it->second) << "\n";

  return 0;
}
