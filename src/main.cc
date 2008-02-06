#include <iostream>
#include <cassert>
#include <cmath>
#include <unistd.h>
#include <sys/wait.h>
#include <utility>
#include <sstream>

#include "config.h"

#include "modelreader.hh"
#include "MaxSatInstance.hh"
#include "arguments.hh"
#include "argumentsparser.hh"

extern "C" {
#include <argp.h>
}
const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;

using std::cerr;
using std::cout;
using std::pair;
using std::make_pair;
using std::stringstream;

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

map<string, double> getFeatures(const string &inst) {

  stringstream oss;
  map<string, double> features;
 
  MaxSatInstance msi(inst.c_str());
  msi.computeLocalSearchProperties();
  msi.printInfo(oss);

  string line;

  while (!oss.eof()) {
    getline(oss,line);
    
    if(line.empty())
      continue;
    
    // split at :
    size_t possplit = line.find(":");
    string featurename(line, 0, possplit);
    const string featurevalue(line, possplit+1);
    
    // replace spaces and dots in feature name by _
    for(size_t i = 0; i < featurename.size(); i++)
      if(featurename[i] == ' ' || featurename[i] == '.') featurename[i] = '_';
    
    const double feat = atof(featurevalue.c_str());
    
    features[featurename] = feat;
  }
  return features;
}

int main(int argc, char *argv[]) {
  
  Arguments args;
  ArgumentsParser::parse(argc, argv, args); // Parse Arguments
  
  const string &instance = args.cnffile;

  ModelReader mreader(args.model);

  vector<string> solvers = mreader.getOutputLabels();

  // Let's compute the features
  cout << "Computing Features...";
  map<string, double> feats = getFeatures(instance);
  cout << " DONE\n";

  if(feats.size() == 0) {
    cerr << "Error: Feature computation returned no features.\n";
    exit(EXIT_FAILURE);
  }

#ifndef NDEBUG
  for(std::map<string, double>::const_iterator it = feats.begin(); it != feats.end(); it++) 
    cout << it->first << " : " << it->second << "\n";
#endif // NDEBUG

  // Let's compute the model
  map<string, double> predRt;
  for(uint s = 0; s < solvers.size(); s++) {

    // Standardizing computed features
    map<string, double> solverFeatures;
    if(mreader.getFeatureStd()) {
      for(std::map<string, double>::const_iterator it = feats.begin(); it != feats.end(); ++it) {
	pair<double, double> factors = mreader.getStdFactors(solvers[s], it->first);
	solverFeatures[it->first] = (it->second - factors.first) / factors.second; 
      }
    } else {
      solverFeatures = feats;
    }

#ifndef NDEBUG
    cout << "Computing runtime for " << solvers[s] << ":\n";
#endif // NDEBUG
    const Model &m = mreader.getModel(solvers[s]);
    const double runtime = m.computeModelOutput(solverFeatures);
    predRt[solvers[s]] = runtime;

#ifndef NDEBUG
    cout << " Model output for " << solvers[s] << " is " << runtime << "\n";
#endif // NDEBUG
  }

  // Let's display the models, from best to worst.
  cout << "Predicted Runtimes:\n";
  map<double, string> invPredRt;
  for(map<string, double>::const_iterator it = predRt.begin();
      it != predRt.end();
      it++) {
    cout << "\t" << it->first << ": " << (mreader.getOutputStd() ? exp(it->second) : it->second) << "\n";
    invPredRt[it->second] = it->first;
  }
  
  // Forking to run the best solver
  for(map<double, string>::const_iterator it = invPredRt.begin();
      it != invPredRt.end();
      it++) {
#ifndef NDEBUG
    cout << "** Runnning " << it->second << "\n";
#endif // NDEBUG

    // If we only pretend then return
    if(args.pretend)
      return 0;

    const string filename_prefix = "./";
    const string filename = filename_prefix + it->second;

    pid_t pid;
    pid = fork();
    if(pid == 0) {
      execl(filename.c_str(), it->second.c_str(), instance.c_str(), (char *)NULL);
      exit(EXIT_SUCCESS);
    } else {
      int status;
      wait(&status);
    
#ifndef NDEBUG
      if(WIFEXITED(status)) 
	cout << "** Process returned normally ";
      else
	cout << "** Process returned abnormally ";

      cout << " with exit code " << WEXITSTATUS(status) << "\n";

      if(WIFSIGNALED(status))
	cout << "** Process returned by signal " << WTERMSIG(status) << "\n";
#endif // NDEBUG

      if(WIFEXITED(status)) 
	break;
    }
  }

  return 0;
}
