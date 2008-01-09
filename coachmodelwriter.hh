#ifndef COACHMODELWRITER_HH
#define COACHMODELWRITER_HH
 
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iterator>
#include <map>
#include <utility>

#include "reader.hh"

using std::pair;
using std::map;
using std::string;
using std::vector;
using std::ofstream;
using std::copy;
using std::cerr;

class CoachModelWriter {
public:
  CoachModelWriter(const string &);

  void writeSolverNames(const vector<string>&);
  void writeTrainingSetFilename(const string&);
  void writeWeight(const string&, const string&, double);
  void writeFreeWeight(const string&, double);
  void writeFeatureStd();
  void writeOutputStd();
  void writeFExpansion(const vector<uint>&);
  void writeFEOrder(uint);
  void writeStdFactors(const string&, const map<string, pair<double, double> >&);
  void endWrite();

private:
  ofstream file;
};

#endif // COACHMODELWRITER_HH
