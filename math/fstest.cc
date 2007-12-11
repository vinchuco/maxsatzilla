#include <iostream>
#include <vector>

#include "dataset.hh"
#include "forwardselection.hh"
#include "pputils.hh"

using std::vector;
using std::string;
using std::cerr;

int main(void) {

  double data[][38] = {{1.0, 3.3, 2.8, 3.1, 4.1, 9.8},
		       {1.0, 4.4, 4.9, 3.5, 3.9, 12.6},
		       {1.0, 3.9, 5.3, 4.8, 4.7, 11.9},
		       {1.0, 3.9, 2.6, 3.1, 3.6, 11.1},
		       {1.0, 5.6, 5.1, 5.5, 5.1, 13.3},
		       {1.0, 4.6, 4.7, 5.0, 4.1, 12.8},
		       {1.0, 4.8, 4.8, 4.8, 3.3, 12.8},
		       {1.0, 5.3, 4.5, 4.3, 5.2, 12.0},
		       {1.0, 4.3, 4.3, 3.9, 2.9, 13.6},
		       {1.0, 4.3, 3.9, 4.7, 3.9, 13.9},
		       {1.0, 5.1, 4.3, 4.5, 3.6, 14.4},
		       {0.5, 3.3, 5.4, 4.3, 3.6, 12.3},
		       {0.8, 5.9, 5.7, 7.0, 4.1, 16.1},
		       {0.7, 7.7, 6.6, 6.7, 3.7, 16.1},
		       {1.0, 7.1, 4.4, 5.8, 4.1, 15.5},
		       {0.9, 5.5, 5.6, 5.6, 4.4, 15.5},
		       {1.0, 6.3, 5.4, 4.8, 4.6, 13.8},
		       {1.0, 5.0, 5.5, 5.5, 4.1, 13.8},
		       {1.0, 4.6, 4.1, 4.3, 3.1, 11.3},
		       {0.9, 3.4, 5.0, 3.4, 3.4, 7.9},
		       {0.9, 6.4, 5.4, 6.6, 4.8, 15.1},
		       {1.0, 5.5, 5.3, 5.3, 3.8, 13.5},
		       {0.7, 4.7, 4.1, 5.0, 3.7, 10.8},
		       {0.7, 4.1, 4.0, 4.1, 4.0, 9.5},
		       {1.0, 6.0, 5.4, 5.7, 4.7, 12.7},
		       {1.0, 4.3, 4.6, 4.7, 4.9, 11.6},
		       {1.0, 3.9, 4.0, 5.1, 5.1, 11.7},
		       {1.0, 5.1, 4.9, 5.0, 5.1, 11.9},
		       {1.0, 3.9, 4.4, 5.0, 4.4, 10.8},
		       {1.0, 4.5, 3.7, 2.9, 3.9, 8.5},
		       {1.0, 5.2, 4.3, 5.0, 6.0, 10.7},
		       {0.8, 4.2, 3.8, 3.0, 4.7, 9.1},
		       {1.0, 3.3, 3.5, 4.3, 4.5, 12.1},
		       {1.0, 6.8, 5.0, 6.0, 5.2, 14.9},
		       {0.8, 5.0, 5.7, 5.5, 4.8, 13.5},
		       {0.8, 3.5, 4.7, 4.2, 3.3, 12.2},
		       {0.8, 4.3, 5.5, 3.5, 5.8, 10.3},
		       {0.8, 5.2, 4.8, 5.7, 3.5, 13.2}};
  const uint rows = 38;
  const uint cols = 6;

  double **stdformat = new double* [rows];
  for(uint i = 0; i < rows; i++)
    stdformat[i] = new double [cols];

  // fill values
  for(uint r = 0; r < rows; r++)
    stdformat[r][0] = data[r][4];

  for(uint r = 0; r < rows; r++)
    for(uint c = 1; c < cols; c++)
      stdformat[r][c] = data[r][c-1];

  // Call forward selection
  MSZDataSet *ds = createDataSet(stdformat, rows, cols, 1);

  vector<string> labels;
  labels.push_back("Quality");
  labels.push_back("Clarity");
  labels.push_back("Aroma");
  labels.push_back("Body");
  labels.push_back("Flavor");
  labels.push_back("Oakiness");
  ds->dumpPlotFiles(labels, "./fstest");
  

  // Lets do a forward selection
  ForwardSelection fs(*ds, 0);
  vector<uint> res = fs.run(0.15);

  // free
  delete ds;
}
