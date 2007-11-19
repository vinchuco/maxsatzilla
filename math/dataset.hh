#ifndef DATASET_HH
#define DATASET_HH

#include <string>
#include <vector>

using std::string;
using std::vector;

class MSZDataSet {
public:
  MSZDataSet(double**, size_t, size_t, size_t);
  ~MSZDataSet();

  /// Given a vector of labels for matrix columns and a prefix for file names, 
  /// dumps all the files, in a readable format for GNUPlot to use.
  void dumpPlotFiles(const vector<string> &, const string &);
  void dumpPlotFiles(char **, size_t, char *);

  /// Accessors
  double getFeatureValue(size_t, size_t); 
  double getOutputValue(size_t, size_t);

  inline size_t getNRows()     const { return nrows;           }
  inline size_t getNCols()     const { return ncols;           }
  inline size_t getNFeatures() const { return ncols - outputs; }
  inline size_t getNOutputs()  const { return outputs;         }

private:
  double **matrix;
  size_t nrows;
  size_t ncols;
  size_t outputs;
};

/// API entrace function to create dataset to be used with other
/// functions. This function NEVER returns a 0 pointer.
MSZDataSet *createDataSet(double**, size_t, size_t, size_t = 1);

#endif // DATASET_HH
