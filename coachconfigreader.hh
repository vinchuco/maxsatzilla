#ifndef COACHCONFIGREADER_HH
#define COACHCONFIGREADER_HH

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "math/pputils.hh"
#include "reader.hh"

using std::string;
using std::vector;
using std::ifstream;
using std::cout;

class CoachConfigReader : public Reader {
public:
  CoachConfigReader(const string &);

  enum fsOption {NONE=0, DELTA, INSTS};   ///< Which kind of option does the user provide.

  string getTrainingSetFilename()         const { return trainingSetFilename; }
  string getOutputModelFilename()         const { return outputModelFilename; }
  double getFSDelta()                     const { return fsDelta;             }
  uint   getFSInsts()                     const { return fsInsts;             }
  fsOption getFSOption()                  const { return fsopt;               }
  double getRRDelta()                     const { return rrDelta;             }
  vector<vector<uint> > getFEPartitions() const { return fePartitions;        }
  bool   getFeatureStd()                  const { return featureStd;          }
  bool   getOutputStd ()                  const { return outputStd;           }
  uint   getFEPartOrder()                 const { return fePartOrder;         }
  uint   getPercentTest()                 const { return percentTest;         }
  
private:
  void parseConfig();
  
  string trainingSetFilename;         ///< Filename that will contain the training set to read from.
  string outputModelFilename;         ///< Filename that will contain the model to output.
  uint percentTest;                   ///< Percentage of usable instances to be used for testing.
  union {
    double fsDelta;                     ///< Forward Selection threshold (alt. to fsInsts).
    uint fsInsts;                       ///< Number of features to pick from Forward Selection (alt. to fsDelta).
  };
  fsOption fsopt;                     ///< Which option has the user provided for FS
  double rrDelta;                     ///< Value to decrease ridge regression result values.
  vector<vector<uint> > fePartitions; ///< Partitions for function expansion.
  bool featureStd;                    ///< Flag which determines if standardization of features should be performed.
  bool outputStd;                     ///< Flag which determines if standardization of outputs should be performed.
  uint fePartOrder;                   ///< Order of the function basis expansion.
};

#endif // COACHCONFIGREADER_HH
