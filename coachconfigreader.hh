#ifndef COACHCONFIGREADER_HH
#define COACHCONFIGREADER_HH

#include "math/pputils.hh"

class CoachConfigReader {
public:
  CoachConfigReader(const string &);

  string getTrainingSetFilename()         const { return trainingSetFilename; }
  string getOutputModelFilename()         const { return outputModelFilename; }
  double getFSDelta()                     const { return fsDelta;             }
  double getRRDelta()                     const { return rrDelta;             }
  vector<vector<uint> > getFEPartitions() const { return fePartitions;        }
  bool   featureStd()                     const { return featureStd;          }
  bool   outputStd ()                     const { return outputStd;           }

private:
  string trainingSetFilename;
  string outputModelFilename;
  double fsDelta;
  double rrDelta;
  vector<vector<uint> > fePartitions;
  bool featureStd;
  bool outputStd;
};

#endif // COACHCONFIGREADER_HH
