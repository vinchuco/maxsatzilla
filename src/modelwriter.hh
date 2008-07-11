#ifndef MODELWRITER_HH
#define MODELWRITER_HH
 
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iterator>
#include <map>

#include "reader.hh"
#include "featurelabel.hh" 
#include "learningalgtype.hh"
#include "triple.hpp"

using std::pair;
using std::map;
using std::string;
using std::vector;
using std::ofstream;
using std::copy;
using std::cerr;

class ModelWriter {
public:
  ModelWriter(const string &);

  void writeSolverNames(const vector<string>&);
  void writeTrainingSetFilename(const string&);
  void writeWeight(const string&, const FeatureLabel&, double);
  void writeFreeWeight(const string&, double);
  void writeFeatureStd();
  void writeOutputStd();
  void writeFExpansion(const vector<uint>&);
  void writeFEOrder(uint);
  void writeStdFactors(const string&, const vector<Triple<FeatureLabel, double, double> >&);
  void writeLearningAlgType(LearningAlgType);
  void writeModelFilename(const string, struct svm_model *); ///< Writes the svm_model to a file and the filename of where the model is kept to the model file.
  void writeModelOrderedLabels(const string, const vector<FeatureLabel>&);
  void endWrite();

private:
  ofstream file;
};

#endif // MODELWRITER_HH
