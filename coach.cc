#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <map>
#include <cassert>
#include <cstdlib>

#include "mszreader.hh"
#include "coachconfigreader.hh"
#include "coachmodelwriter.hh"

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
  in.close();

  // Spliting data into solver data and feature data.
  

  try {
    CoachModelWriter mwriter(creader.getOutputModelFilename());
    mwriter.writeTrainingSetFilename(creader.getTrainingSetFilename());

    // Creating a vector of datasets, one for each solver.
    vector<MSZDataSet *> dss(nbSolvers, 0);
    for(uint s = 0; s < nbSolvers; s++)
      dss[s] = createDataSet(fdata, nbInstances, nbFeatures, featuresNames, sdata, solversNames[s]);
    
    vector<string> snames;
    for(uint s = 0; s < nbSolvers; s++) snames.push_back(solversNames[s]);
    ds->printSolverStats(timeOut, snames);
    mwriter.writeSolverNames(snames);

    // Lets create the plot files
    vector<string> labels;
    for(uint i = 0; i < nbSolvers; i++)
      labels.push_back(solversNames[i]);
    for(uint i = 0; i < nbFeatures; i++)
      labels.push_back(featuresNames[i]);
    
    cerr << "Created labels for solvers and features of size : " << labels.size() << "\n";
    cerr << "features (" << nbFeatures << ") + solvers(" << nbSolvers << ") = " << labels.size() << "\n";
    ds->dumpPlotFiles(labels, "./coach");
    
    if(creader.getOutputStd()) {
      ds->standardizeOutputs();
      mwriter.writeOutputStd();
    }
    if(creader.getFeatureStd()) {
      ds->standardize();
      mwriter.writeFeatureStd();
    }

    // Lets do a forward selection
    for(uint s = 0; s < nbSolvers; s++) {
      MSZDataSet solverDS(*ds, s);
      solverDS.removeTimeouts(timeOut, s);
      
      ForwardSelection fs(solverDS, 0);
      vector<uint> res = fs.run(creader.getFSDelta());
      solverDS.removeFeatures(res);
      
      RidgeRegression rr(solverDS);
      Model m = rr.run(creader.getRRDelta(), 0);
      
      mwriter.writeFreeWeight(solversNames[s], m.getRegressor());
      const map<string, double> &rmap = m.getAllRegressors();
      for(map<string, double>::const_iterator it = rmap.begin();
	  it != rmap.end();
	  it++)
	mwriter.writeWeight(solversNames[s], it->first, it->second);
    }

    mwriter.endWrite();

    // Let's not forget to delete the dataset
    delete ds;
    
  } catch(std::bad_alloc) {
    cerr << "Coach: Bad memory allocation or not enough memory on the system\n."
	 << "Exiting peacefully...\n";
    exit(EXIT_FAILURE);
  }

  delete[] solversNames;
  delete[] featuresNames;
  delete[] instancesNames;
  
  return 0;
}
