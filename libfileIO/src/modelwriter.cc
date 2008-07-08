#include "modelwriter.hh"

#include <sstream>
#include <cstdlib>

#include <svm.h>

ModelWriter::ModelWriter(const string &path) 
  : file(path.c_str()) {

  if(!file.is_open()) {
    cerr << "Unable to open config file.\n";
    exit(EXIT_FAILURE);
  }
  
}

void ModelWriter::endWrite() {
  file.close();
}

void ModelWriter::writeSolverNames(const vector<string>& names) {
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

void ModelWriter::writeTrainingSetFilename(const string& tname) {
  file << "p training " << tname << "\n";
}

void ModelWriter::writeWeight(const string& sname, const FeatureLabel &feature, double w) {
  file << "p weight " << sname << " " 
       << feature.size() << " ";

  const FeatureLabel::const_iterator end = feature.end();
  for(FeatureLabel::const_iterator it = feature.begin();
      it != end;
      ++it) 
    file << it->first << " " << it->second << " ";

  file << w << "\n";
}

void ModelWriter::writeFreeWeight(const string& sname, double w) {
  file << "p freeweight " << sname << " " << w << "\n";
}

void ModelWriter::writeFeatureStd() {
  file << "p feastd\n";
}

void ModelWriter::writeOutputStd() {
  file << "p outstd\n";
}

void ModelWriter::writeFExpansion(const vector<uint>& part) {
  file << "p part ";
  for(uint i = 0; i < part.size(); i++) {
    file << part[i];
    if(i != part.size() - 1)
      file << " ";
    else
      file << "\n";
  }
}

void ModelWriter::writeStdFactors(const string &sname, const vector<Triple<FeatureLabel, double, double> >& factors) {

  for(vector<Triple<FeatureLabel, double, double> >::const_iterator it = factors.begin();
      it != factors.end();
      it++) {
    file << "p stdfactor " << sname << " " << it->first.size() << " ";
    for(FeatureLabel::const_iterator lbs = it->first.begin();
	lbs != it->first.end();
	lbs++)
      file << lbs->first << " " << lbs->second << " ";

    file << it ->second << " " << it ->third << "\n";
  }
}

void ModelWriter::writeFEOrder(uint feorder) {
  file << "p feorder " << feorder << "\n";
}

void ModelWriter::writeLearningAlgType(LearningAlgType la) {
  string laStr;
  
  switch(la) {
  case RR:
    laStr = "RR";
    break;
  case SVM:
    laStr = "SVM";
    break;
  default:
    cerr << "Trying to write unknown Learning Algorithm descriptor\n";
    break;
  }

  file << "p la " << laStr << "\n";
}

void ModelWriter::writeModelFilename(struct svm_model *model) {

  // Lets create a random name for the model.
  std::stringstream out;
  out << rand();
  string filename("svm_model_");
  filename += out.str();

  svm_save_model(filename.c_str(), model);
  file << "p svm_model_file " << filename << "\n";

}
