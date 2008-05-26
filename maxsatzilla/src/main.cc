#include <iostream>
#include <cassert>
#include <cmath>
#include <unistd.h>
#include <sys/wait.h>
#include <utility>
#include <sstream>
#include <vector>
#include <fstream>

#include <cstdlib>

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
using std::vector;
using std::ifstream;
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
  msi.computeLocalSearchProperties(oss);
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

map<string, vector<string> > readSolversCfg(const string& path) {
  
  map<string, vector<string> > solversPath;

  ifstream file(path.c_str());
  
  if(!file.is_open()) {
    cerr << "Couldn't read solvers config file at " << path << "\n"
	 << "Exiting...\n";
    exit(EXIT_FAILURE);
  }

  string line;
  while(!getline(file, line).eof()) {
    // Parse the line we got.
    string::size_type found = line.find(' ');
    if(found == string::npos) {
      cerr << "Can't parse line in " << path << "\n"
	   << "Exiting...\n";
      exit(EXIT_FAILURE);
    }
    
    const string name = line.substr(0, found);
    string rest = line.substr(found+1);

    vector<string> argv;
    while(1) {
      string::size_type next = rest.find(' ');
      if(next == string::npos) {
	argv.push_back(rest);
	break;
      }

      argv.push_back(rest.substr(0, next));
      rest.erase(0, next+1);
    } 

    solversPath[name] = argv;
  }

  return solversPath;
}

int main(int argc, char *argv[]) {
  
  Arguments args;
  ArgumentsParser::parse(argc, argv, args); // Parse Arguments
  
  const string &instance = args.cnffile;

  ModelReader mreader(args.model);

  vector<string> solvers = mreader.getOutputLabels();
  map<string, vector<string> > solversPath= readSolversCfg(args.solverscfg);

  // Let's compute the features
  cout << "c Computing Features...";
  map<string, double> feats = getFeatures(instance);
  cout << " DONE\n";

  if(feats.size() == 0) {
    cerr << "c Error: Feature computation returned no features.\n";
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
  cout << "c Predicted Runtimes:\n";
  map<double, string> invPredRt;
  for(map<string, double>::const_iterator it = predRt.begin();
      it != predRt.end();
      it++) {
    cout << "c \t" << it->first << ": " << (mreader.getOutputStd() ? exp(it->second) : it->second) << "\n";
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

    char ** argv = NULL;
    const map<string, vector<string> >::const_iterator argvit = solversPath.find(it->second);
    if(argvit == solversPath.end()) {
      cerr << "No path set for " << it->second << "\n"
	   << "Exiting...\n";
      exit(EXIT_FAILURE);
    }

    pid_t pid;
    pid = fork();
    if(pid == 0) {
      const int argc = argvit->second.size() + 2;
      argv = new char* [argvit->second.size() + 2];
      for(int i = 0; i < argc-2; ++i)
	argv[i] = strdup((argvit->second)[i].c_str());
      argv[argc-2] = strdup(instance.c_str());
      argv[argc-1] = (char*) NULL;

      execv(argv[0], argv);
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
