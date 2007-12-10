#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <map>
#include <cassert>
#include <cstdlib>

#include "mszreader.hh"
#include "coachconfigreader.hh"

#include "math/dataset.hh"
#include "math/forwardselection.hh"
#include "math/ridgeregression.hh"

using std::vector;
using std::string;
using std::cerr;
using std::ofstream;
using std::ifstream;
using std::map;

using namespace iomsz;

#ifndef MODELHEADER
#define MODELHEADER "msz_model.hh"
#endif

template <class T>
void outputVectorComma(ofstream& file, const vector<T>& strs, bool strp = false) {
  for(size_t i = 0; i < strs.size(); i++) {
    if(strp) file << "\"";
    file << strs[i];
    if(strp) file << "\"";
    if(i != strs.size() - 1)
      file << ", ";
  }
}

void outputModelHeader(const map<string, pair<vector<double>, vector<string> > > &m) {

  const string path(MODELHEADER);

  ofstream file;
  file.open(path.c_str());

  // Generate Header Guard
  size_t slashpos = path.find_last_of("/");
  string guard(path, slashpos+1);
  for(size_t i = 0; i < guard.size(); i++) {
    if(isalpha(guard[i])) guard[i] = toupper(guard[i]); 
    else if(guard[i] == '.') guard[i] = '_';
  }

  file << "#ifndef " << guard << "\n"
       << "#define " << guard << "\n"
       << "\n";

  file << "enum Solver {";
  vector<string> solverNames;
  for(map<string, pair<vector<double>, vector<string> > >::const_iterator it = m.begin();
      it != m.end();
      it++) 
    solverNames.push_back(it->first);
  outputVectorComma(file, solverNames);
  file << ", ";
  file << " NUMSOLVERS};\n\n";

  file << "char *solverNames[] = {";
  outputVectorComma(file, solverNames, true);
  file << "};\n\n";

  file << "size_t nbFeatures[] = {";
  vector<size_t> nbFeatures;
  size_t maxFeatures = 0;
  for(map<string, pair<vector<double>, vector<string> > >::const_iterator it = m.begin();
      it != m.end();
      it++) {
    maxFeatures = (maxFeatures < it->second.first.size() ? it->second.first.size() : maxFeatures);
    nbFeatures.push_back(it->second.first.size());
  }
  outputVectorComma(file, nbFeatures);
  file << "};\n\n";

  file << "double weights[][" << maxFeatures << "] = {";
  size_t mindex = 0;
  for(map<string, pair<vector<double>, vector<string> > >::const_iterator it = m.begin();
      it != m.end();
      it++) {
    const bool last = (mindex++ == m.size()-1);
    const vector<double> &w = it->second.first;
    file << "{";
    outputVectorComma(file, w);
    // Add extra zeros
    const vector<size_t> zeros(w.size() - maxFeatures, 0);
    if(zeros.size() > 0) {
      file << ", ";
      outputVectorComma(file, zeros);
    }
    file << "}";
    if(!last) file << ", ";
  }
  file << "};\n\n";

  file << "char* features[][" << maxFeatures << "] = {";
  mindex = 0;
  for(map<string, pair<vector<double>, vector<string> > >::const_iterator it = m.begin();
      it != m.end();
      it++) {
    const bool last = (mindex++ == m.size()-1);
    const vector<string> &l = it->second.second;
    file << "{";
    outputVectorComma(file, l, true);
    const vector<size_t> zeros(l.size() - maxFeatures, 0);
    if(zeros.size() > 0) {
      file << ", ";
      outputVectorComma(file, zeros);
    }
    file << "}";
    if(!last) file << ", ";
  }
  file << "};\n\n";
  file << "#endif\n";

  file.close();

}


/**
 * Main function for MaxSATzilla training.
 * This function received 3 arguments:
 * - Forward Selection Threshold;
 * - Ridge Regression Scalar Delta;
 * - Name of the file with training set;
 * - Prefix for header output;
 */
int main(int argc, char *argv[]) {

  if (argc != 2) {
    cerr << "usage: coach <coachconfigfile>\n";
    exit(EXIT_FAILURE);
  }

  CoachConfigReader creader(argv[1]);

  unsigned int nbSolvers, nbFeatures, nbInstances, timeOut;
  string *solversNames;
  string *featuresNames;
  string *instancesNames;
  double **data;
  
  ifstream in(creader.getTrainingSetFilename().c_str());
  if (!in.is_open()) {
    cerr << "Error: Could not open file: "<< creader.getTrainingSetFilename() << "\n";
    exit(EXIT_FAILURE);
  }

  parse_DIMACS(in, nbSolvers, nbFeatures, nbInstances, timeOut, solversNames, featuresNames, instancesNames, data);

  try {
    MSZDataSet *ds = createDataSet(data, nbInstances, nbFeatures+nbSolvers, nbSolvers);
    
    vector<string> snames;
    for(size_t s = 0; s < nbSolvers; s++) snames.push_back(solversNames[s]);
    ds->printSolverStats(timeOut, snames);

    // Lets create the plot files
    vector<string> labels;
    for(size_t i = 0; i < nbSolvers; i++)
      labels.push_back(solversNames[i]);
    for(size_t i = 0; i < nbFeatures; i++)
      labels.push_back(featuresNames[i]);
    
    cerr << "Created labels for solvers and features of size : " << labels.size() << "\n";
    cerr << "features (" << nbFeatures << ") + solvers(" << nbSolvers << ") = " << labels.size() << "\n";
    ds->dumpPlotFiles(labels, "./coach");
    
    map<string, pair<vector<double>, vector<string> > > model;
    
    // Lets do a forward selection
    for(size_t s = 0; s < nbSolvers; s++) {
      MSZDataSet solverDS = *ds;
      solverDS.removeTimeouts(timeOut, s);
      solverDS.standardizeOutputs();
      solverDS.standardize();

      ForwardSelection fs(solverDS, s);
      vector<size_t> res = fs.run(creader.getFSDelta());
      solverDS.removeFeatures(res);
      
      RidgeRegression rr(solverDS);
      vector<double> w;
      w = rr.run(creader.getRRDelta(), s);

      vector<string> wlabels;
      wlabels.push_back("freeParameter");
      for(size_t i = 0; i < res.size(); i++)
	wlabels.push_back(featuresNames[res[i]]);

      assert(w.size() == wlabels.size());
      model[solversNames[s]] = make_pair(w, wlabels);
    }

    outputModelHeader(model);

    // Let's not forget to delete the dataset
    delete ds;
 
  } catch(std::bad_alloc) {
    cerr << "Coach: Bad memory allocation or not enough memory on the system\n."
	 << "Exiting peacefully...\n";
    exit(EXIT_FAILURE);
  }

  return 0;
}
