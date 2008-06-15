#include "model.hh"

#include "pputils.hh"

#include <iostream>
#include <gsl/gsl_math.h>

#include <cstdlib>

using std::cerr;

RRModel::Model() 
  : freeRegressor(0) { }

void RRModel::addRegressor(double val, const FeatureLabel& id) {
  const_iterator it = regressorMap.find(id);
  
  if(it != end()) {
    MSZWarn("You're trying to add a regressor with value %f, however, there's already one set: %s with value %f. Ignoring your request.", val, id.toString().c_str(), it->second);
    return;
  }
  
  regressorMap[id] = val;
}

void RRModel::addRegressor(double val) {
  if(freeRegressor != 0) {
    MSZWarn("You're trying to set the free regressor already set to %f, to a new value %f. Delete it first if you really want to add it to the model. For now I'm ignoring your request.", val, freeRegressor);
    return;
  }
  freeRegressor = val;	    
}

void RRModel::remRegressor(const FeatureLabel& id) {
  map<FeatureLabel, double>::iterator it = regressorMap.find(id);

  if(it == regressorMap.end()) {
    MSZWarn("You're trying to remove a regressor with id %s that is not set. Ignoring your request.", id.toString().c_str());
    return;
  }

  regressorMap.erase(it);
}

double RRModel::getRegressor(const FeatureLabel& id) const {
  map<FeatureLabel, double>::const_iterator it = regressorMap.find(id);

  if(it == regressorMap.end()) {
    // MSZWarn("You're trying to get a regressor with id %s that is not set. Returning 0.0.", id.c_str());
    return 0.0;
  }

  return it->second;
}

double RRModel::computeModelOutput(const map<string, double>& features) const {
  
  double y = freeRegressor;
  const map<string, double>::const_iterator featuresEnd = features.end();
  const map<FeatureLabel, double>::const_iterator regressorMapEnd = regressorMap.end();

  for(map<FeatureLabel, double>::const_iterator it = regressorMap.begin();
      it != regressorMapEnd;
      ++it) {

    double value = 1.0;
    for(FeatureLabel::const_iterator f = it->first.begin();
	f != it->first.end();
	++f) {
      
      const map<string, double>::const_iterator fval = features.find(f->first);
      if(fval == featuresEnd) {
	cerr << "RRModel::computeModelOutput: During model computation, failed to find feature " << f->first << "\n"
	     << "Dumping list of features found:\n";
	for(map<string, double>::const_iterator featuresit = features.begin();
	    featuresit != features.end();
	    ++featuresit) 
	  cerr << featuresit->first << "\n";
	exit(EXIT_FAILURE);
      }
      
      value *= gsl_pow_int(fval->second, f->second);
    }
    y += value * it->second;
  }
  return y;
}

set<string> RRModel::computeRawLabels() const {

  set<string> rlabels;
  const_iterator endit = end();

  for(const_iterator it = begin(); it != endit; ++it)
    for(FeatureLabel::const_iterator f = it->first.begin();
	f != it->first.end();
	++f)
      rlabels.insert(f->first);

  return rlabels;
}
