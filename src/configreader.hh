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

struct SVMParams {
  // Support Vector Machine Options
  pair<uint,bool> degree;                    ///< Degree of polynomio
  pair<double,bool> gamma;                   ///< Kernel multiplier
  pair<double,bool> coef0;                   ///< Kernel adder
  pair<double,bool> cost;                       ///< cost of constraint violation
  pair<double,bool> p;                       ///< epsilon in epsilon insensitive function
  pair<double,bool> nu;                      ///< nu parameter in nu-svm
  pair<double,bool> cacheSize;               ///< size of available cache in Mb
  pair<double,bool> eps;                ///< Stop Criterion
  pair<bool,bool> shrinking;                    ///< Shrinking flag
  pair<bool,bool> probability;               ///< Probability flag
  pair<SVMRegression::RegressionType,bool> regressionType; ///< Type of SVM Regression
  pair<SVMRegression::KernelType,bool> kernelType; ///< Type of SVM kernel to use
};

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

  SVMParams getSVMParams()                const { return svmParams; }

private:
  struct tolower_op : public std::unary_function<char, char> {
    char operator()(char x) { return tolower(x); }
  };

  void parseConfig();
  void initSVMParams(); 

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
  SVMParams svmParams;                ///< Set of defined SVM parameters
};

#endif // CONFIGREADER_HH
