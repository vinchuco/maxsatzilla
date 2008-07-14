#include "configreader.hh"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <cstdlib>

using std::cerr;

ConfigReader::ConfigReader(const string &configFile)
  : Reader(configFile), percentTest(0), fsopt(NONE), featureStd(false), outputStd(false), fePartOrder(0), la(NUM_LA), handleTimeout(false), timeoutError(0.0) {
  initSVMParams();
  parseConfig();
  file.close();
}

void ConfigReader::initSVMParams() {
  
  svmParams.degree = make_pair(0, false);
  svmParams.gamma = make_pair(0, false);
  svmParams.coef0 = make_pair(0, false);
  svmParams.cost = make_pair(0, false);
  svmParams.p = make_pair(0, false);
  svmParams.nu = make_pair(0, false);
  svmParams.cacheSize = make_pair(0, false);
  svmParams.eps = make_pair(0, false);
  svmParams.shrinking = make_pair(false, false);
  svmParams.probability = make_pair(false, false);
  svmParams.regressionType = make_pair(0, false);
  svmParams.kernelType = make_pair(0, false);

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
      else if(paramName == "rr_factor")
	rrDelta = getDouble();
      else if(paramName == "svm_degree")
	svmParams.degree = make_pair(getUInt(), true);
      else if(paramName == "svm_gamma")
	svmParams.gamma = make_pair(getDouble(), true);
      else if(paramName == "svm_coef0")
	svmParams.coef0 = make_pair(getDouble(), true);
      else if(paramName == "svm_c")
	svmParams.cost = make_pair(getDouble(), true);
      else if(paramName == "svm_p")
	svmParams.p = make_pair(getDouble(), true);
      else if(paramName == "svm_nu")
	svmParams.nu = make_pair(getDouble(), true);
      else if(paramName == "svm_cache_size")
	svmParams.cacheSize = make_pair(getDouble(), true);
      else if(paramName == "svm_eps")
	svmParams.eps = make_pair(getDouble(), true);
      else if(paramName == "svm_shrinking")
	svmParams.shrinking = make_pair(true, true);
      else if(paramName == "svm_probability")
	svmParams.probability = make_pair(true, true);
      else if(paramName == "svm_regression_type") {
	string type = getString();
	std::transform(type.begin(), type.end(), type.begin(), tolower_op());
	if(type == "nu")
	  svmParams.regressionType = make_pair(SVMRegression::NU_R, true);
	else if(type == "epsilon")
	  svmParams.regressionType = make_pair(SVMRegression::EPSILON_R, true);
	else {
	  MSZError("In your coach config file you have not set the svm regression type to a known value.");
	}
      }
      else if(paramName == "svm_kernel_type") {
	string kernel = getString();
	std::transform(kernel.begin(), kernel.end(), kernel.begin(), tolower_op());
	if(kernel == "linear")
	  svmParams.kernelType = make_pair(SVMRegression::LINEAR_K, true);
	else if(kernel == "poly")
	  svmParams.kernelType = make_pair(SVMRegression::POLY_K, true);
	else if(kernel == "rbf")
	  svmParams.kernelType = make_pair(SVMRegression::RBF_K, true);
	else if(kernel == "sigmoid")
	  svmParams.kernelType = make_pair(SVMRegression::SIGMOID_K, true);
	else {
	  MSZError("In your coach config file you have not set the svm kernel type to a known value.");
	}
      }
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
      else if(paramName == "timeouthandle") {
	handleTimeout = true;
	timeoutError = getDouble();
      }
      else if(paramName == "learning")  {
	string lAlg = getString();
	std::transform(lAlg.begin(), lAlg.end(), lAlg.begin(), tolower_op());
	if(lAlg == "rr")
	  la = RR;
	else if(lAlg == "svm")
	  la = SVM;
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
