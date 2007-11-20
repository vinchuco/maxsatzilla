#include <iostream>
#include <vector>

#include "dataset.hh"
//#include "forwardselection.hh"

using std::vector;

#define FIN 0.15

static char* labels[5] = {"Solver1", "Solver2", "Vars", "Clauses", "SomethingElse"};

int main(void) {

  double **data = 0;
  data = new double*[3];
  for(int i = 0; i < 3; i++)
    data[i] = new double[5];

  data[0][0] = 900.0; data[0][1] = 1.0; data[0][2] = 100.0; data[0][3] = 100.0; data[0][4] = 120.3;
  data[1][0] = 9000.0; data[1][1] = 10.0; data[1][2] = 150.0; data[1][3] = 1000.0; data[1][4] = 99.5;
  data[2][0] = 90000.0; data[2][1] = 100.0; data[2][2] = 200.0; data[2][3] = 10000.0; data[2][4] = 950.2;

  MSZDataSet *ds = createDataSet(data, 3, 5, 2);

  // Lets create the plot files
  ds->dumpPlotFiles(labels, 5, "./driver");

  // Lets do a forward selection
  //ForwardSelection fs(*ds, 0);

  //vector<int> res = fs.run(FIN);
  
  // Let's not forget to delete the dataset
  delete ds;
  
  return 0;
}
