#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <map>
#include <cassert>
#include <cstdlib>
#include <cmath>

#include "mszreader.hh"
#include "coachconfigreader.hh"
#include "coachmodelwriter.hh"
#include "modeltesting.hh"

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
 * This function receives the path to the configuration file.
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

  // Let's compute the number of training and testing instances
  const uint percentTest = creader.getPercentTest();

  try {
    CoachModelWriter mwriter(creader.getOutputModelFilename());
    mwriter.writeTrainingSetFilename(creader.getTrainingSetFilename());

    // Creating a vector of pairs of datasets, one for each solver.
    // If there is a test set, we create the test set, otherwise we create a dataset and the test position is 0.
    vector<pair<MSZDataSet *, MSZDataSet *> > dss(nbSolvers, make_pair((MSZDataSet*)0, (MSZDataSet*)0));

    double **fdata = new double* [nbInstances];
    for(uint f = 0; f < nbInstances; f++)
      fdata[f] = new double [nbFeatures];
    
    for(uint r = 0; r < nbInstances; r++)
      for(uint c = 0; c < nbFeatures; c++)
	fdata[r][c] = data[r][c+nbSolvers];
    
    for(uint s = 0; s < nbSolvers; s++) {

      double *outputs = new double [nbInstances];
      for(uint i = 0; i < nbInstances; i++)
	outputs[i] = data[i][s];

      if(percentTest > 0)
	dss[s] = createDataSets(fdata, nbInstances, nbFeatures, featuresNames, outputs, solversNames[s], timeOut, percentTest);
      else
	dss[s] = std::make_pair(createDataSet(fdata, nbInstances, nbFeatures, featuresNames, outputs, solversNames[s]), (MSZDataSet*)0);

      delete[] outputs;
    }

    for(uint i = 0; i < nbInstances; i++)
      delete[] fdata[i];
    delete[] fdata;

    vector<string> snames;
    for(uint s = 0; s < nbSolvers; s++) snames.push_back(solversNames[s]);
    mwriter.writeSolverNames(snames);

    if(percentTest == 0) 
      for(uint s = 0; s < nbSolvers; s++)
	dss[s].first->printSolverStats(timeOut);

    // Lets create the plot files
    vector<string> labels;
    for(uint s = 0; s < nbSolvers; s++)  labels.push_back(solversNames[s]);
    for(uint f = 0; f < nbFeatures; f++) labels.push_back(featuresNames[f]);
    
    for(uint s = 0; s < nbSolvers; s++) {
      dss[s].first->dumpPlotFiles("./coachTraining");
      if(percentTest > 0)
	dss[s].second->dumpPlotFiles("./coachTest");
      
      if(percentTest == 0)
	dss[s].first->removeTimeouts(timeOut); // Make sure timeout removal happens BEFORE standardization of outputs
      
      if(creader.getOutputStd()) {
	dss[s].first->standardizeOutput();
	dss[s].second->standardizeOutput();
      }

      if(creader.getFeatureStd()) {
	map<string, pair<double, double> > factors = dss[s].first->standardize();
	dss[s].second->standardize(factors);

	// Output standardization factors
	mwriter.writeStdFactors(solversNames[s], factors);
      }
    }

    // Write standardization options to file
    if(creader.getFeatureStd()) mwriter.writeFeatureStd();
    if(creader.getOutputStd()) mwriter.writeOutputStd();

    for(uint s = 0; s < nbSolvers; s++) {

      // Performing Forward Selection
      if(creader.getFSOption() != CoachConfigReader::NONE) {
	ForwardSelection fs(*(dss[s].first));
	vector<uint> res;
	if(creader.getFSOption() == CoachConfigReader::DELTA) 
	  res = fs.run(creader.getFSDelta());
	else if(creader.getFSOption() == CoachConfigReader::INSTS)
	  res = fs.runForBest(creader.getFSInsts());
	else {
	  cerr << "Coach: Got an unexpected FS options: " << creader.getFSOption() << "\n";
	  exit(EXIT_FAILURE);
	}
	dss[s].first->removeFeatures(res);
      }

      // Computing the model with ridge regression
      RidgeRegression rr(*(dss[s].first));
      Model m = rr.run(creader.getRRDelta());

      // Outputting the model to file
      mwriter.writeFreeWeight(solversNames[s], m.getRegressor());
      const map<string, double> &rmap = m.getAllRegressors();
      for(map<string, double>::const_iterator it = rmap.begin();
	  it != rmap.end();
	  it++)
	mwriter.writeWeight(solversNames[s], it->first, it->second);

      // Testing model against a test dataset
      ModelTesting::test(m, *(dss[s].second));
    }

    mwriter.endWrite();

    // Let's not forget to delete the datasets
    for(uint s = 0; s < nbSolvers; s++) {
      delete dss[s].first;
      delete dss[s].second;
    }
    
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
