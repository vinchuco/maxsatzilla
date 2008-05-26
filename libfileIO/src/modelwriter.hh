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
  void endWrite();

private:
  ofstream file;
};

#endif // MODELWRITER_HH
