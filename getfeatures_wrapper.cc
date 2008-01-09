#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cassert>
#include <iostream>

#include "getfeatures_wrapper.hh"

using std::ifstream;
using std::cerr;

#define GETFEATURE_PATH "./getfeaturestofile.sh"
#define TMPFEATURESFILE "/tmp/features.tmp"

namespace WrapperUtils {

  void runExec(const string &inst) {
    execl(GETFEATURE_PATH, "getfeaturestofile.sh", inst.c_str(), TMPFEATURESFILE);
  }
  
  map<string, double> parseFeaturesFromFile() {
    map<string, double> features;
    ifstream ffile (TMPFEATURESFILE);
    string line;

    if(ffile.is_open()) {
      while (!ffile.eof()) {
	getline (ffile,line);
	
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
      ffile.close();
    } else {
      cerr << "Couldn't open file for reading : " << TMPFEATURESFILE << "\n";
      exit(EXIT_FAILURE);
    }


    return features;
  }

};

map<string, double> getFeatures(const string &inst) {

  pid_t pid;
  pid = fork();

  if(pid == 0) {
    WrapperUtils::runExec(inst);
    exit(EXIT_SUCCESS);
  }
  else {
    // In Parent process, we need to wait for the child to finish
    int status;
    wait(&status);
  }

  return WrapperUtils::parseFeaturesFromFile();
}
