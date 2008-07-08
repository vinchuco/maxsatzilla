#include "modeltesting.hh"

#include "logmgm.hh"

#include <iostream>

using std::cout;

void ModelTesting::test(const Model *m, const MSZDataSet &testDS) {

  *(LogMgm::Instance()) << "Testing for output: " << testDS.getOutputLabel() << "\n";
  *(LogMgm::Instance()) << "Testing " << testDS.getNRows() << " instances\n";

  vector<double> predRuntimes(testDS.getNRows(), m.getRegressor());
  
  for(uint i = 0; i < testDS.getNRows(); i++) 
    for(uint f = 0; f < testDS.getNCols(); f++)
      predRuntimes[i] += testDS.getFeatureValue(i, f) * m.getRegressor(testDS.getColLabel(f));
  
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