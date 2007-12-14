#include "modeltesting.hh"

void ModelTesting::test(const MszModel &m, const MszDataSet &testDS) {

  cout << "Testing for output: " << testDS.getOutputLabel() << "\n";
  cout << "Testing " << testDS.getNRows() << "instances\n";
  
  for(uint i = 0; i < testDS.getNRows(); i++) {
    

  }

}
