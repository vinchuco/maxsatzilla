#include "coachmodelwriter.hh"

CoachModelWriter::CoachModelWriter(const string &path) 
  : file(path.c_str()) {

  if(!file.is_open()) {
    cerr << "Unable to open config file.\n";
    exit(EXIT_FAILURE);
  }
  
}

void CoachModelWriter::endWrite() {
  file.close();
}

void CoachModelWriter::writeSolverNames(const vector<string>& names) {
  file << "p solvers ";
  /* copy(names.begin(), names.end(), std::ostream_iterator<string>(file, " "));
   * WHY WE DON'T USE COPY... 
   *
   * Copy inserts the separator element EVEN after the last element (which is
   * AFAICT highly stupid!). This means that after the last element a space comes up
   * and only then a new line. Reader::getVector<>() reads a vector and assumed that
   * right after the last element there are no spaces, but a newline.
   * While the parser (which is very simple) is not modified, we should not use copy.
   * Same store for writeFExpansion.
   */
  for(uint i = 0; i < names.size(); i++) {
    file << names[i];
    if(i != names.size() - 1)
      file << " ";
    else
      file << "\n";
  }
  file << "\n";
}

void CoachModelWriter::writeTrainingSetFilename(const string& tname) {
  file << "p training " << tname << "\n";
}

void CoachModelWriter::writeWeight(const string& sname, const string &feature, double w) {
  file << "p weight " << sname << " " << feature << " " << w << "\n";
}

void CoachModelWriter::writeFreeWeight(const string& sname, double w) {
  file << "p freeweight " << sname << " " << w << "\n";
}

void CoachModelWriter::writeFeatureStd() {
  file << "p feastd\n";
}

void CoachModelWriter::writeOutputStd() {
  file << "p outstd\n";
}

void CoachModelWriter::writeFExpansion(const vector<uint>& part) {
  file << "p part ";
  for(uint i = 0; i < part.size(); i++) {
    file << part[i];
    if(i != part.size() - 1)
      file << " ";
    else
      file << "\n";
  }
}

void CoachModelWriter::writeStdFactors(const string &sname, const map<string, pair<double, double> >& factors) {

  for(map<string, pair<double, double> >::const_iterator it = factors.begin();
      it != factors.end();
      it++) 
    file << "p stdfactor " << sname << " " << it->first << " " << it ->second.first << " " << it ->second.second << "\n";
}

void CoachModelWriter::writeFEOrder(uint feorder) {
  file << "p feorder " << feorder << "\n";
}
