#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <map>
#include <cassert>
#include <cstdlib>
#include <cmath>

#include "mszreader.hpp"
#include "configreader.hh"
#include "modelwriter.hh"
#include "modeltesting.hh"
#include "triple.hpp"
#include "dataset.hh"
#include "logmgm.hh"
#include "forwardselection.hh"
#include "learningalgorithm.hh"
#include "ridgeregression.hh"
#include "svmregression.hh"

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

  if (argc != 3) {
    cerr << "usage: coach <coachconfigfile> <logdir>\n";
    exit(EXIT_FAILURE);
  }

  ConfigReader creader(argv[1]);
  LogMgm *lm = LogMgm::Instance();
  lm->setOutputPath(argv[2]);

  unsigned int nbSolvers, nbFeatures, nbInstances, timeOut;
  string *solversNames;
  string *featuresNames;
  string *instancesNames;
  double **data;
  
  ifstream in(creader.getTrainingSetFilename().c_str());
  if (!in.is_open()) {
    cerr << "Error: Could not open file: " << creader.getTrainingSetFilename() << "\n";
    exit(EXIT_FAILURE);
  }

  parse_DIMACS(in, nbSolvers, nbFeatures, nbInstances, timeOut, solversNames, featuresNames, instancesNames, data);
  in.close();

  // Let's compute the number of training and testing instances
  const uint percentTest = creader.getPercentTest();

  try {
    ModelWriter mwriter(creader.getOutputModelFilename());
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

    // Output CSV files
    for(uint s = 0; s < nbSolvers; ++s) {
      dss[s].first->toCsv(LogMgm::Instance()->getLogPath() + "/" + solversNames[s] + "_train.csv");
      if(dss[s].second != 0)
	dss[s].second->toCsv(LogMgm::Instance()->getLogPath() + "/" + solversNames[s] + "_test.csv");
    }

    // Removing harmful features from dataset
    for(uint s = 0; s < nbSolvers; ++s) {
      dss[s].first->removeHarmfulFeatures();
      if(dss[s].second != 0)
	dss[s].second->removeHarmfulFeatures();
    }

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
      dss[s].first->dumpPlotFiles(LogMgm::Instance()->getLogPath() + "/coachTraining_org");
      if(percentTest > 0)
	dss[s].second->dumpPlotFiles(LogMgm::Instance()->getLogPath() + "/coachTest_org");
      
      if(percentTest == 0)
	dss[s].first->removeTimeouts(timeOut); // Make sure timeout removal happens BEFORE standardization of outputs
      
      if(creader.getOutputStd()) {
	dss[s].first->standardizeOutput();
	dss[s].second->standardizeOutput();
      }

      if(creader.getFeatureStd()) {
	vector<Triple<FeatureLabel, double, double> > factors = dss[s].first->standardize();
	dss[s].second->standardize(factors);

	// Output standardization factors
	mwriter.writeStdFactors(solversNames[s], factors);
      }

      // If there was any kind of standardization let's output plot files again.
      if(creader.getFeatureStd() || creader.getOutputStd()) {
	dss[s].first->dumpPlotFiles(LogMgm::Instance()->getLogPath() + "/coachTraining_std");
	if(percentTest > 0)
	  dss[s].second->dumpPlotFiles(LogMgm::Instance()->getLogPath() + "/coachTest_std");
      }

    }

    // Write standardization options to file
    if(creader.getFeatureStd()) mwriter.writeFeatureStd();
    if(creader.getOutputStd()) mwriter.writeOutputStd();

    for(uint s = 0; s < nbSolvers; s++) {

      // Performing Forward Selection
      lm->setCategory(LogMgm::FORSEL);
      if(creader.getFSOption() != ConfigReader::NONE) {
	ForwardSelection fs(*(dss[s].first));
	vector<uint> res;
	if(creader.getFSOption() == ConfigReader::DELTA) 
	  res = fs.run(creader.getFSDelta());
	else if(creader.getFSOption() == ConfigReader::INSTS)
	  res = fs.runForBest(creader.getFSInsts());
	else {
	  cerr << "Coach: Got an unexpected FS options: " << creader.getFSOption() << "\n";
	  exit(EXIT_FAILURE);
	}
	dss[s].first->removeFeatures(res);
	dss[s].second->removeFeatures(res);
      }
      lm->endCategory();

      // Performing function expansion
      if(creader.getLearningAlg() == RR && creader.getFExpansion()) { // Let's do function expansion if learning alg is ridgeregression
	if(creader.completeFE()) { // Expand on the complete set of variables
	  dss[s].first->expand(creader.getFEPartOrder());
	  dss[s].second->expand(creader.getFEPartOrder());
	}
	else {
	  dss[s].first->expand(creader.getFEPartOrder(), creader.getFEPartitions());
	  dss[s].second->expand(creader.getFEPartOrder(), creader.getFEPartitions());
	}

	dss[s].first->toCsv(LogMgm::Instance()->getLogPath() + "/" + solversNames[s] + "-fe_train.csv");
	if(dss[s].second != 0)
	  dss[s].second->toCsv(LogMgm::Instance()->getLogPath() + "/" + solversNames[s] + "-fe_test.csv");
	
	// If function expansion was performed, we should perform a second 
	// forward selection
	lm->setCategory(LogMgm::FORSEL);
	if(creader.getFSOption() != ConfigReader::NONE) {
	  ForwardSelection fs(*(dss[s].first));
	  vector<uint> res;
	  if(creader.getFSOption() == ConfigReader::DELTA) 
	    res = fs.run(creader.getFSDelta());
	  else if(creader.getFSOption() == ConfigReader::INSTS)
	    res = fs.runForBest(creader.getFSInsts());
	  else {
	    cerr << "Coach: Got an unexpected FS options: " << creader.getFSOption() << "\n";
	    exit(EXIT_FAILURE);
	  }
	  dss[s].first->removeFeatures(res);
	  dss[s].second->removeFeatures(res);
	}
	lm->endCategory();
      }

      // Computing the model with ridge regression
      lm->setCategory(LogMgm::RIDREG);
      LearningAlgorithm *la = 0;
      switch(creader.getLearningAlg()) {
      case RR: {
	RidgeRegression * rr = new RidgeRegression(*(dss[s].first));
	rr->setDelta(creader.getRRDelta());
	la = rr;
	break;
      }
      case SVM: {
	SVMRegression *svm = new SVMRegression(*(dss[s].first));
	la = svm;
	break;
      }
      case NN: {
	//NNRegression * nn = NNRegression(*(dss[s].first));
	//la = nn;
	exit(EXIT_FAILURE);
	break;
      }
      default: 
	cerr << "No known learning method defined.\n";
	exit(EXIT_FAILURE);
	break;
      }
      Model m = la->run();
      lm->endCategory();

      // Outputting the model to file
      mwriter.writeFreeWeight(solversNames[s], m.getRegressor());
      for(Model::const_iterator it = m.begin();
	  it != m.end();
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
