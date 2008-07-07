#include "modelreader.hh"
#include "rrmodel.hh"
#include "svmmodel.hh"

#include <iostream>
#include <iterator>

#include <cstdlib>

using std::cerr;
using std::make_pair;

ModelReader::ModelReader(const string &configFile)
  : Reader(configFile), featureStd(false), outputStd(false) {
  parseConfig();
  file.close();
}

void ModelReader::parseConfig() {
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
      else if(paramName == "la") {
	string solverName = getString();
	string laStr = getString();
	las[solverName] = RR;
	if(laStr == "RR") 
	  models[solverName] = new RRModel();
	else if(laStr == "SVM") 
	  models[solverName] = new SVMModel();
	else 
	  cerr << "Warning: Can't recognize Learning Algorithm (la) option in model file. Ignoring.";
      }
      else if(paramName == "solvers") {
	outLabels = getVector<string>();
	const vector<string>::const_iterator end = outLabels.end();
	for(vector<string>::const_iterator it = outLabels.begin();
	    it != end;
	    ++it)
	  models[*it] = 0;
      }
      else if(paramName == "weight") {
	const string solverName = getString();
	const FeatureLabel featureName = parseFeatureLabel();
	const double value = getDouble();
	models[solverName].addRegressor(value, featureName);
#ifndef NDEBUG
	cout << "Weight " << solverName << " " << featureName << " " << value << "\n";
#endif
      }
      else if(paramName == "freeweight") {
	const string solverName = getString();
	const double value = getDouble();
	models[solverName].addRegressor(value);
#ifndef NDEBUG
	cout << "FreeWeight " << solverName << " " << value << "\n";
#endif
      }
      else if(paramName == "stdfactor") {
	const string solverName = getString();
	const FeatureLabel featureName = parseFeatureLabel();
	const double mean = getDouble();
	const double sdv = getDouble();
	factors[solverName].push_back(make_triple(featureName, mean, sdv));
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

Model* ModelReader::getModel(const string& sname) const {

  map<string, Model*>::const_iterator it = models.find(sname);

  if(it == models.end()) {
    cerr << "ModelReader::getModel: can't find model for solver " << sname << "\n";
    exit(EXIT_FAILURE);
  }

  return it->second;
}

pair<double, double> ModelReader::getStdFactors(const string& sname, const FeatureLabel& feature) const {
  const map<string, vector<Triple<FeatureLabel, double, double> > >::const_iterator it = factors.find(sname);

  if(it == factors.end()) {
    cerr << "Can't find STD factors of feature " << feature << " for solver " << sname << "\n";
    cerr << "Exiting...\n";
    exit(EXIT_FAILURE);
  }
  
  const vector<Triple<FeatureLabel, double, double> > &v = it->second;
  const vector<Triple<FeatureLabel, double, double> >::const_iterator end = v.end();
  
  for(vector<Triple<FeatureLabel, double, double> >::const_iterator it = v.begin();
      it != end;
      ++it) {
    if(it->first == feature)
      return make_pair(it->second, it->third);
  }

  /// \todo Solve this problem of feature computation. What to do if we are asked for features which do not exist?
  /// \todo Current solution doesn't seem good.
  //cerr << "Cannot find standardization factors in model for the requested feature: " << feature << "\n";
  //cerr << "Exiting...\n";
  //exit(EXIT_FAILURE);

  return make_pair(0.0, 0.0);
}

FeatureLabel ModelReader::parseFeatureLabel() {
  const uint nbSubLabels = getUInt();
  FeatureLabel fl;

  for(uint i = 0; i < nbSubLabels; ++i) {
    const string name = getString();
    const uint mult = getUInt();
    fl.insert(name, mult);
  }
  
  return fl;
}

/// @returns the learning algorithm used to generate a model for the given solver name.
/// @returns It returns NUM_LA if no solver with that name is found.
LearningAlg getLearningAlg(const string& sname) const {
  
  const map<string, LearningAlg>::const_iterator it = las.find(sname);
  if(it == las.end())
    return NUM_LA;
  else
    return *it;

}
