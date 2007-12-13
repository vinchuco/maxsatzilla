#include "model.hh"

#include "pputils.hh"

Model::Model() 
  : freeRegressor(0) { }

void Model::addRegressor(double val, string id) {
  map<string, double>::iterator it = regressorMap.find(id);
  
  if(it != regressorMap.end()) {
    MSZWarn("You're trying to add a regressor, however, there's already one set of this id: %s with value %f. Ignoring your request.", id.c_str(), it->second);
    return;
  }
  
  regressorMap[id] = val;
}

void Model::addRegressor(double val) {
  if(freeRegressor != 0) {
    MSZWarn("You're trying to add a regressor already set to %f. Delete it first if you really want to add it to the model. For now I'm ignoring your request.", freeRegressor);
    return;
  }
  freeRegressor = val;	    
}

void Model::remRegressor(const string& id) {
  map<string, double>::iterator it = regressorMap.find(id);

  if(it == regressorMap.end()) {
    MSZWarn("You're trying to remove a regressor with id %s that is not set. Ignoring your request.", id.c_str());
    return;
  }

  regressorMap.erase(it);
}

double Model::getRegressor(const string& id) const {
  map<string, double>::const_iterator it = regressorMap.find(id);

  if(it == regressorMap.end()) {
    MSZWarn("You're trying to get a regressor with id %s that is not set. Returning 0.0.", id.c_str());
    return 0.0;
  }

  return it->second;
}

double Model::computeModelOutput(const vector<pair<string, double> >& features) {
  double y = freeRegressor;
  for(vector<pair<string, double> >::const_iterator it = features.begin();
      it != features.end();
      it++) {
    map<string, double>::const_iterator mit = regressorMap.find(it->first);
    if(mit != regressorMap.end())
      y += mit->second * it->second;
  }
  return y;
}

