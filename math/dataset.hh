#ifndef DATASET_HH
#define DATASET_HH

#include <string>
#include <vector>

using std::string;
using std::vector;

class MSZDataSet {
public:
  MSZDataSet(double**, int, int, int);
  ~MSZDataSet();

  /// Given a vector of labels for matrix columns and a prefix for file names, 
  /// dumps all the files, in a readable format for GNUPlot to use.
  void dumpPlotFiles(const vector<string> &, const string &);
  void dumpPlotFiles(char **, int, char *);

private:
  double **matrix;
  int nrows;
  int ncols;
  int outputs;
};

/// API entrace function to create dataset to be used with other
/// functions. This function NEVER returns a 0 pointer.
MSZDataSet *createDataSet(double**, int, int, int = 1);

#endif // DATASET_HH
