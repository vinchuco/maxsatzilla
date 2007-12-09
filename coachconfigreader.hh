#ifndef COACHCONFIGREADER_HH
#define COACHCONFIGREADER_HH

#include "math/pputils.hh"

class CoachConfigReader {
public:
  CoachConfigReader(const string &);

  string getTrainingSetFilename()         const { return trainingSetFilename; }
  string getOutputModelFilename()         const { return outputModelFilename; }
  double getFSDelta()                     const { return fsDelta;             }
  uint   getFSInsts()                     const { return fsInsts;             }
  double getRRDelta()                     const { return rrDelta;             }
  vector<vector<uint> > getFEPartitions() const { return fePartitions;        }
  bool   featureStd()                     const { return featureStd;          }
  bool   outputStd ()                     const { return outputStd;           }

private:
  void eatSpaces(ifstream &);
  void eatLine(ifstream&);
  string getString(ifstream &);
  double getDouble(ifstream&);
  uint getUInt(ifstream&);
  vector<uint> getVectorUInt(ifstream&);

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
