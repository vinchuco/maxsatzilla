#ifndef COACHMODELWRITER_HH
#define COACHMODELWRITER_HH
 
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iterator>

#include "reader.hh"

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
  void endWrite();

private:
  ofstream file;
};

#endif // COACHMODELWRITER_HH
