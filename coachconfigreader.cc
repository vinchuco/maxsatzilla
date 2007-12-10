#include "coachconfigreader.hh"

#include <iostream>
#include <iterator>

using std::cerr;

CoachConfigReader::CoachConfigReader(const string &configFile)
  : file(configFile.c_str()) {

  cout << "Parsing " << configFile << "\n";
  
  if(!file.is_open()) {
    cout << "Unable to open config file.\n";
    exit(EXIT_FAILURE);
  }

  parseConfig();
  file.close();

}

void CoachConfigReader::parseConfig() {

  // For each loop we read a line in the file.
  while(!file.eof()) {
    eatSpaces();
    
    // If it is a comment we read everything up to the end of file.
    char c;
    c = file.get();
    if(c == 'c') {
      eatLine();
    }
    else if(c == 'p') {
      // Read everything up to the first non-space
      eatSpaces();

      // Read string up to the next space
      string paramName = getString();

      // Read everything up to the first non-space
      eatSpaces();

      if(paramName == "training") 
	trainingSetFilename = getString();
      else if(paramName == "model")
	outputModelFilename = getString();
      else if(paramName == "fsdelta")
	fsDelta = getDouble();
      else if(paramName == "fsinst")
	fsInsts = getUInt();
      else if(paramName == "rrdelta")
	rrDelta = getDouble();
      else if(paramName == "part")
	fePartitions.push_back(getVectorUInt());
      else if(paramName == "outstd")
	outputStd = true;
      else if(paramName == "feastd")
	featureStd = true;
      else { // error
	cerr << "Error during parsing of config.\nExpecting one of params: training, model, fsdelta, fsinst, rrdelta, part, outstd, feastd. Got: " << paramName << "\n";
	exit(EXIT_FAILURE);
      }
    } else { // error
      cerr << "Error during parsing of config.\nExpected comment of parameter declaration, got character: " << c << "\n";
      exit(EXIT_FAILURE);
    }
  }

}

void CoachConfigReader::eatSpaces() {
  while(isspace(file.peek())) file.get();
}

void CoachConfigReader::eatLine() {
  while(file.peek() != '\n') file.get();
  eatSpaces();
}

string CoachConfigReader::getString() {
  string str;
  while(!isspace(file.peek())) {
    char c = file.get();
    str += c;
  }
  return str;
}

double CoachConfigReader::getDouble() {
  double num;
  file >> num;
  return num;
}

uint CoachConfigReader::getUInt() {
  uint val;
  file >> val;
  return val;
}

vector<uint> CoachConfigReader::getVectorUInt() {
  vector<uint> vec;
  while(file.peek() != '\n') {
    uint val;
    file >> val;
    vec.push_back(val);
  }
  return vec;
}
