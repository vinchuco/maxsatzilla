#include <iostream>
#include <vector>

#include "../mszparse.h"

#include "dataset.hh"
#include "forwardselection.hh"
#include "ridgeregression.hh"

using std::vector;

#define FIN 0.15

int main(void) {

  


  MSZDataSet *ds = createDataSet(data, 3, 5, 2);

  // Lets create the plot files
  ds->dumpPlotFiles(labels, 5, "./driver");

  // Lets do a forward selection
  ForwardSelection fs(*ds, 0);

  vector<size_t> res = fs.run(FIN);
  
  // Let's not forget to delete the dataset
  delete ds;
  
  return 0;
}
