#include "coachconfigreader.hh"

CoachConfigReader::CoachConfigReader(const string &configFile) {

  ifstream file(configFile); 
  if(!file.is_open()) {
    cout << "Unable to open config file.\n";
    exit(EXIT_FAILURE);
  }

  parseConfig(file);
  file.close();

}

CoachConfigReader::parseConfig(ifstream &file) {

  // For each loop we read a line in the file.
  while(file.good()) {
    eatSpaces(file);
    
    // If it is a comment we read everything up to the end of file.
    char c;
    c = file.get();
    if(c == 'c') {
      eatLine(file);
    }
    else if(c == 'p') {
      // Read everything up to the first non-space
      eatSpaces();

      // Read string up to the next space
      string paramName = getString(file);

      // Read everything up to the first non-space
      eatSpaces(file);

      if(paramName == "training") 
	trainingSetFilename = getString(file);
      else if(paramName == "model")
	outputModelFilename = getString(file);
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
	exit(FAILURE);
      }
    } else { // error
      cerr << "Error during parsing of config.\nExpected comment of parameter declaration, got character: " << c << "\n";
      exit(FAILURE);
    }
  }

}

void CoachConfigReader::eatSpaces(ifstream &file) {
  while(isspace(file.peek())) file.get();
}

void CoachConfigReader::eatLine(ifstream& file) {
  while(file.peek() != '\n') file.get();
  eatSpaces(file);
}

string CoachConfigReader::getString(ifstream &file) {
  string str;
  while(!isspace(file.peek())) str += ((char)(file.get()));
  return str;
}

double CoachConfigReader::getDouble(ifstream& file) {
  double num;
  file >> num;
  return num;
}

uint CoachConfigReader::getUInt(ifstream& file) {
  uint val;
  file >> val;
  return val;
}

vector<uint> CoachConfigReader::getVectorUInt(ifstream& file) {
  vector<uint> vec;
  while(file.peek() != '\n') {
    uint val;
    file >> val;
    vec.push_back(val);
  }
  return vec;
}
