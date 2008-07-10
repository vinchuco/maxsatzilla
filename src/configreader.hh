#ifndef CONFIGREADER_HH
#define CONFIGREADER_HH

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <functional>

#include "pputils.hh"
#include "reader.hh"
#include "learningalgtype.hh"
#include "svmregression.hh"

using std::string;
using std::vector;
using std::ifstream;
using std::cout;

class ConfigReader : public Reader {
public:
  ConfigReader(const string &);
  virtual ~ConfigReader() {}

  enum fsOption {NONE=0, DELTA, INSTS};   ///< Which kind of option does the user provide.

  string getTrainingSetFilename()         const { return trainingSetFilename;      }
  string getOutputModelFilename()         const { return outputModelFilename;      }
  double getFSDelta()                     const { return fsDelta;                  }
  uint   getFSInsts()                     const { return fsInsts;                  }
  fsOption getFSOption()                  const { return fsopt;                    }
  double getRRDelta()                     const { return rrDelta;                  }
  bool   getFExpansion()                  const { return fePartOrder != 0;         }
  bool completeFE()                       const { return fePartitions.size() == 0; }
  vector<vector<uint> > getFEPartitions() const { return fePartitions;             }
  bool   getFeatureStd()                  const { return featureStd;               }
  bool   getOutputStd ()                  const { return outputStd;                }
  uint   getFEPartOrder()                 const { return fePartOrder;              }
  uint   getPercentTest()                 const { return percentTest;              }
  LearningAlgType getLearningAlg()        const { return la;                       }
  double getTimeoutError()                const { return timeoutError;             }
  bool   getHandleTimeouts()              const { return handleTimeout;            }
  
private:
  struct tolower_op : public std::unary_function<char, char> {
    char operator()(char x) { return tolower(x); }
  };

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
  LearningAlgType la;                     ///< Learning algorithm selected
  bool handleTimeout;                 ///< If true coach will handle timeouts
  double timeoutError;                ///< If handleTimeout is set this will be the error allows for timeout fitting.
  
  // Support Vector Machine Options
  uint svmDegree;                    ///< Degree of polynomio
  double svmGamma;                   ///< Kernel multiplier
  double svmCoef0;                   ///< Kernel adder
  double svmC;                       ///< cost of constraint violation
  double svmP;                       ///< epsilon in epsilon insensitive function
  double svmNu;                      ///< nu parameter in nu-svm
  double svmCacheSize;               ///< size of available cache in Mb
  double svmStopCrit;                ///< Stop Criterion
  uint svmShrink;                    ///< Shrinking flag
  uint svmProbability;               ///< Probability flag
  SVMRegression::RegressionType svmRegressionType; ///< Type of SVM Regression
  SVMRegression::KernelType svmKernelType; ///< Type of SVM kernel to use
};

#endif // CONFIGREADER_HH
