#include "modeltesting.hh"

#include "logmgm.hh"

#include <iostream>

#include <cstdlib>

using std::cout;
using std::cerr;

void ModelTesting::test(const Model *m, const DataSet &testDS) {
  
  const LearningAlgType lat = m->getLearningAlgType();
  
  if(lat == RR)
    testRRModel(static_cast<const RRModel *>(m), testDS);
  else if(lat == SVM)
    testSVMModel(static_cast<const SVMModel *>(m), testDS);
  else {
    cerr << "test: unexpected model type.";
    exit(EXIT_FAILURE);
  }
}

void ModelTesting::testRRModel(const RRModel *m, const DataSet &testDS) {

  *(LogMgm::Instance()) << "Testing for output: " << testDS.getOutputLabel() << "\n";
  *(LogMgm::Instance()) << "Testing " << testDS.getNRows() << " instances\n";

  vector<double> predRuntimes(testDS.getNRows(), m->getRegressor());
  
  for(uint i = 0; i < testDS.getNRows(); i++) 
    for(uint f = 0; f < testDS.getNCols(); f++)
      predRuntimes[i] += testDS.getFeatureValue(i, f) * m->getRegressor(testDS.getColLabel(f));
  
  vector<double> realRuntimes(testDS.getNRows(), 0);
  vector<double> diffRuntimes(testDS.getNRows(), 0);

  for(uint i = 0; i < testDS.getNRows(); i++) {
    realRuntimes[i] = testDS.getOutputValue(i);
    diffRuntimes[i] = predRuntimes[i] - realRuntimes[i];
  }

  // Outputting info
  for(uint i = 0; i < testDS.getNRows(); i++) 
    *(LogMgm::Instance()) << "Real: " << realRuntimes[i] 
			  << "\tPredicted: " << predRuntimes[i] 
			  << "\tError: " << diffRuntimes[i] << "\n";
  
  double sse = 0.0;
  for(uint i = 0; i < testDS.getNRows(); i++) 
    sse += diffRuntimes[i] * diffRuntimes[i];

  *(LogMgm::Instance()) << "Squared Sum of Errors for the model on the test data: " << sse << "\n";
}

void ModelTesting::testSVMModel(const SVMModel *m, const DataSet &testDS) {
  
  *(LogMgm::Instance()) << "Testing for output: " << testDS.getOutputLabel() << "\n";
  *(LogMgm::Instance()) << "Testing " << testDS.getNRows() << " instances\n";

  const uint numTestInstances = testDS.getNRows();
  const uint numFeatures = testDS.getNCols();
  vector<double> predRuntimes(numTestInstances, 0.0);
  struct svm_model *smodel = m->getSVMModelStruct();

  // Allocating array of svm_nodes for testing
  struct svm_node *x = new struct svm_node [numFeatures+1];
  for(uint f = 0; f < numFeatures; ++f)
    x[f].index = f;
  x[numFeatures].index = -1;
  
  for(uint i = 0; i < numTestInstances; ++i) {
    // Building svm_node for current instance
    for(uint f = 0; f < numFeatures; ++f)
      x[f].value = testDS.getFeatureValue(i, f);

    // Predict times
    predRuntimes[i] = svm_predict(smodel, x);
  }
  
  // Free array of svm_nodes
  delete[](x);

  vector<double> realRuntimes(testDS.getNRows(), 0);
  vector<double> diffRuntimes(testDS.getNRows(), 0);

  for(uint i = 0; i < numTestInstances; i++) {
    realRuntimes[i] = testDS.getOutputValue(i);
    diffRuntimes[i] = predRuntimes[i] - realRuntimes[i];
  }

  // Outputting info
  for(uint i = 0; i < numTestInstances; i++) 
    *(LogMgm::Instance()) << "Real: " << realRuntimes[i] 
			  << "\tPredicted: " << predRuntimes[i] 
			  << "\tError: " << diffRuntimes[i] << "\n";

  double sse = 0.0;
  for(uint i = 0; i < numTestInstances; i++) 
    sse += diffRuntimes[i] * diffRuntimes[i];

  *(LogMgm::Instance()) << "Squared Sum of Errors for the model on the test data: " << sse << "\n";
}
