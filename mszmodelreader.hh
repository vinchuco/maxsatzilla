#ifndef MSZMODELREADER_HH
#define MSZMODELREADER_HH

#include <string>
#include <map>
#include <utility>

#include "reader.hh"

using std::string;
using std::map; 
using std::pair;

class MszModelReader : public Reader {
public:
  MszModelReader(const string&);
  
  string getTrainingSetFilename()         const { return trainingSetFilename; }
  string getOutputModelFilename()         const { return outputModelFilename; }
  bool   getFeatureStd()                  const { return featureStd;          }
  bool   getOutputStd()                   const { return outputStd;           }
  vector<vector<uint> > getFEPartitions() const { return fePartitions;        }
  uint   getFEOrder()                     const { return feOrder;             }
  double getModelWeight(const string&, const string&);
  double getModelWeight(const string&);
  pair<double, double> getStdFactors(const string&, const string&);
  vector<string> getSolvers()             const { return solvers;             }

private:
  void parseConfig();
  FeatureLabel parseFeatureLabel();

  string trainingSetFilename;
  string outputModelFilename;
  bool featureStd;
  bool outputStd;
  uint feOrder;
  vector<string> solvers;
  vector<vector<uint> > fePartitions;
  map<string, map<string, double> > weights; ///< Mapping from solver to map to features and coefficient values
  map<string, double> freeWeights; ///< Mapping solvers to free weights
  map<string, map<string, pair<double, double> > > factors; ///< Mapping solvers to maps of features to factors
};

#endif // MSZMODELREADER_HH
