#include <iostream>

#include "getfeatures_wrapper.hh"
#include "msz_model.hh"

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
  
  if(argc != 2) {
    cerr << "usage: maxsatzilla <instance.cnf>\n";
    exit(EXIT_FAILURE);
  }

  const char *instance = argv[1];

  // Let's compute the features
  cout << "Computing Features...";
  map<string, double> feats = getFeatures(instance);

  cout << " DONE\n";
#ifndef NDEBUG
  for(std::map<string, double>::const_iterator it = feats.begin(); it != feats.end(); it++) 
    cout << it->first << " : " << it->second << "\n";
#endif NDEBUG
  
  // Let's compute the model
    

  // Let's display the models, from best to worst.


  return 0;
}
