#include "mszmodelreader.hh"

#include <iostream>
#include <iterator>

using std::cerr;

MszModelReader::MszModelReader(const string &configFile)
  : Reader(configFile) {
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
      }
      else if(paramName == "freeweight") {
	const string solverName = getString();
	const double value = getDouble();
	freeWeights[solverName] = value;
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
