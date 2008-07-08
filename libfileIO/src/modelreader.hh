#ifndef MODELREADER_HH
#define MODELREADER_HH

#include <string>
#include <map>
#include <utility>

#include "reader.hh"
#include "featurelabel.hh"
#include "triple.hpp"
#include "learningalgtype.hh"

class Model;

using std::string;
using std::map; 
using std::pair;


class ModelReader : public Reader {
public:
  ModelReader(const string&);
  
  string getTrainingSetFilename()           const { return trainingSetFilename; }
  string getOutputModelFilename()           const { return outputModelFilename; }
  bool   getFeatureStd()                    const { return featureStd;          }
  bool   getOutputStd()                     const { return outputStd;           }
  vector<vector<uint> > getFEPartitions()   const { return fePartitions;        }
  uint   getFEOrder()                       const { return feOrder;             }
  Model *getModel(const string&)            const;
  pair<double, double> getStdFactors(const string&, const FeatureLabel&) const;
  vector<string> getOutputLabels()          const { return outLabels;           }
  LearningAlgType getLearningAlgType(const string&) const;

private:
  void parseConfig();
  FeatureLabel parseFeatureLabel();

  string trainingSetFilename;
  string outputModelFilename;
  bool featureStd;
  bool outputStd;
  uint feOrder;
  vector<string> outLabels;
  vector<vector<uint> > fePartitions;
  map<string, LearningAlgType> las;
  map<string, Model *> models;
  map<string, vector<Triple<FeatureLabel, double, double> > > factors; ///< Mapping solvers to maps of features to factors
};

#endif // MODELREADER_HH
