#include "configreader.hh"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <cstdlib>

using std::cerr;

ConfigReader::ConfigReader(const string &configFile)
  : Reader(configFile), percentTest(0), fsopt(NONE), featureStd(false), outputStd(false), fePartOrder(0), la(NUM_LA) {
  parseConfig();
  file.close();
}

void ConfigReader::parseConfig() {

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
      else if(paramName == "fsdelta") {
	if(fsopt == NONE) {
	  fsDelta = getDouble();
	  fsopt = DELTA;
	} else {
	  MSZError("In your coach config file you have set fsinst to %u and you're now trying to set the delta. Only one is possible.", fsInsts);
	}
      }
      else if(paramName == "fsinst") {
	if(fsopt == NONE) {
	  fsInsts = getUInt();
	  fsopt = INSTS;
	} else {
	  MSZError("In your coach config file you have set delta to %f and you're now trying to set the instances. Only one is possible.", fsDelta);
	}
      }
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
      else if(paramName == "pertest")
	percentTest = getUInt();
      else if(paramName == "learning")  {
	string lAlg = getString();
	std::transform(lAlg.begin(), lAlg.end(), lAlg.begin(), tolower_op());
	if(lAlg == "rr")
	  la = RR;
	//else if(lAlg == "svm")
	//  la = SVM;
	//else if(lAlg == "nn")
	//  la = NN;
	else {
	  MSZError("In your coach config file you have not set the learning algorithm to a known value.");
	}
      }
      else { // error
	cerr << "Error during parsing of config.\nExpecting one of params: training, model, fsdelta, fsinst, rrdelta, part, outstd, feastd. Got: " << paramName << "\n";
	exit(EXIT_FAILURE);
      }
    } else if(c == EOF) 
      break;
    else { // error
      cerr << "Error during parsing of config.\nExpected comment of parameter declaration, got character: " << c << "\n";
      exit(EXIT_FAILURE);
    }
  }

}
