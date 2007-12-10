#ifndef COACHCONFIGREADER_HH
#define COACHCONFIGREADER_HH

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "math/pputils.hh"

using std::string;
using std::vector;
using std::ifstream;
using std::cout;

class CoachConfigReader {
public:
  CoachConfigReader(const string &);

  string getTrainingSetFilename()         const { return trainingSetFilename; }
  string getOutputModelFilename()         const { return outputModelFilename; }
  double getFSDelta()                     const { return fsDelta;             }
  uint   getFSInsts()                     const { return fsInsts;             }
  double getRRDelta()                     const { return rrDelta;             }
  vector<vector<uint> > getFEPartitions() const { return fePartitions;        }
  bool   getFeatureStd()                  const { return featureStd;          }
  bool   getOutputStd ()                  const { return outputStd;           }

private:
  void parseConfig();
  void eatSpaces();
  void eatLine();
  string getString();
  double getDouble();
  uint getUInt();
  vector<uint> getVectorUInt();
  
  ifstream file;                      ///< File to read the configuration
  string trainingSetFilename;         ///< Filename that will contain the training set to read from.
  string outputModelFilename;         ///< Filename that will contain the model to output.
  double fsDelta;                     ///< Forward Selection threshold (alt. to fsInsts).
  uint fsInsts;                       ///< Number of features to pick from Forward Selection (alt. to fsDelta).
  double rrDelta;                     ///< Value to decrease ridge regression result values.
  vector<vector<uint> > fePartitions; ///< Partitions for function expansion.
  bool featureStd;                    ///< Flag which determines if standardization of features should be performed.
  bool outputStd;                     ///< Flag which determines if standardization of outputs should be performed.
};

#endif // COACHCONFIGREADER_HH
