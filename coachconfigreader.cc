#include "coachconfigreader.hh"

#include <iostream>
#include <iterator>

using std::cerr;

CoachConfigReader::CoachConfigReader(const string &configFile)
  : Reader(configFile) {
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
	fePartitions.push_back(getVector<uint>());
      else if(paramName == "partorder")
	fePartOrder = getUInt();
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
