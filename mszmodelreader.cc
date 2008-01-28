#include "mszmodelreader.hh"

#include <iostream>
#include <iterator>

using std::cerr;
using std::make_pair;

MszModelReader::MszModelReader(const string &configFile)
  : Reader(configFile), featureStd(false), outputStd(false) {
  parseConfig();
  file.close();
}

void MszModelReader::parseConfig() {
  // For each loop we read a line in the file.
  while(!file.eof()) {
    eatSpaces();
    
    // If it is a comment we read everything up to the end of file.
    char c;
    c = file.get();
    if(c == 'c') {
      eatLine();
    }
    else if(c == 'p') {
      // Read everything up to the first non-space
      eatSpaces();

      // Read string up to the next space
      string paramName = getString();
    
      // Read everything up to the first non-space
      eatSpaces();

      if(paramName == "training") 
	trainingSetFilename = getString();
      else if(paramName == "part")
	fePartitions.push_back(getVector<uint>());
      else if(paramName == "feorder")
	feOrder = getUInt();
      else if(paramName == "outstd")
	outputStd = true;
      else if(paramName == "feastd")
	featureStd = true;
      else if(paramName == "solvers")
	solvers = getVector<string>();
      else if(paramName == "weight") {
	const string solverName = getString();
	const string featureName = getString();
	const double value = getDouble();
	weights[solverName][featureName] = value;
	cout << "Weight " << solverName << " " << featureName << " " << value << "\n";
      }
      else if(paramName == "freeweight") {
	const string solverName = getString();
	const double value = getDouble();
	freeWeights[solverName] = value;
	cout << "FreeWeight " << solverName << " " << value << "\n";
      }
      else if(paramName == "stdfactor") {
	const string solverName = getString();
	const string featureName = getString();
	const double mean = getDouble();
	const double sdv = getDouble();
	factors[solverName][featureName] = make_pair(mean, sdv);
      }
      else { // error
	cerr << "Error during parsing of model.\nExpecting one of params: training, model, fsdelta, fsinst, rrdelta, part, outstd, feastd. Got: " << paramName << "\n";
	exit(EXIT_FAILURE);
      }
    } else { // error
      cerr << "Error during parsing of model.\nExpected comment or parameter declaration, got character: " << c << "\n";
      exit(EXIT_FAILURE);
    }
    eatSpaces();
  }
}

double MszModelReader::getModelWeight(const string& solver, const string& feature) {
  return weights[solver][feature];
}

double MszModelReader::getModelWeight(const string& solver) {
  return freeWeights[solver];
}

pair<double, double> MszModelReader::getStdFactors(const string& sname, const string& feature) {
  return factors[sname][feature];
}

FeatureLabel MszModelReader::parseFeatureLabel() {
  uint nbSubLabels = getInteger();
  FeatureLabel fl;

  for(uint i = 0; i < nbSubLabels; ++i) {
    const string name = getString();
    const uint mult = getInteger();
    fl.insert(name, mult);
  }
  
  return fl;
}
