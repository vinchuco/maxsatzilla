#include <iostream>
#include <cassert>
#include <cmath>
#include <unistd.h>
#include <sys/wait.h>

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

  if(feats.size() == 0) {
    cout << "Error: Feature computation returned no features.\n";
    exit(EXIT_FAILURE);
  }

#ifndef NDEBUG
  for(std::map<string, double>::const_iterator it = feats.begin(); it != feats.end(); it++) 
    cout << it->first << " : " << it->second << "\n";
#endif // NDEBUG
  
  // Let's compute the model
  map<string, double> predRt;
  for(uint s = 0; s < solvers.size(); s++) {
    cout << "Computing runtime for " << solvers[s] << ":\n";
    // Computing model for solver s.
    double runtime = mreader.getModelWeight(solvers[s]);
    cout << runtime << "\n";
    for(map<string, double>::const_iterator it = feats.begin();
	it != feats.end();
	it++) { 
      double w = mreader.getModelWeight(solvers[s], it->first);
      if(w != 0) {
	runtime += it->second * w;
	cout << "(+" << it->first << "[" << it->second << "] * " << w << ") " << runtime << "\n";
      }
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
  
  // Forking to run the best solver
  for(map<string, double>::const_iterator it = predRt.begin();
      it != predRt.end();
      it++) {
    cout << "** Runnning " << it->first << "\n";

    const string filename_prefix = "./";
    const string filename = filename_prefix + it->first;

    pid_t pid;
    pid = fork();
    if(pid == 0) {
      execl(filename.c_str(), it->first.c_str(), instance, "1000", "1000000");
      exit(EXIT_SUCCESS);
    } else {
      int status;
      wait(&status);
    
      if(WIFEXITED(status)) 
	cout << "** Process returned normally ";
      else
	cout << "** Process returned abnormally ";

      cout << " with exit code " << WEXITSTATUS(status) << "\n";

      if(WIFSIGNALED(status))
	cout << "** Process returned by signal " << WTERMSIG(status) << "\n";
      
      if(WIFEXITED(status)) 
	break;
    }
  }

  return 0;
}
