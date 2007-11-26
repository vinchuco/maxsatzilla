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
  void dumpPlotFiles(const vector<string> &, const string &) const;
  void dumpPlotFiles(char **, size_t, char *) const ;

  /// Accessors
  double getFeatureValue(size_t, size_t) const; 
  double getOutputValue(size_t, size_t)  const;

  inline size_t getNRows()     const { return nrows;           }
  inline size_t getNCols()     const { return ncols;           }
  inline size_t getNFeatures() const { return ncols - outputs; }
  inline size_t getNRFeatures()const { return rfeatures;       }
  inline size_t getNOutputs()  const { return outputs;         }

  // Dataset transformations
  /// Implement basis function expansions of any given order throughout
  /// any set of partitions provided. If no partition is provided it'll perform
  /// expansion throughtout all set.
  void expand(size_t);
  void expand(size_t, const vector<vector<size_t> > &);
  
  /// Implements standardization of the feature set
  void standardize();

  /// Implements standardization of the outputs
  void standardizeOutputs();

  /// Removes from current dataset the features in the vector
  void removeFeatures(const vector<size_t> &);

private:
  void expandOnPartition(size_t, const vector<size_t> &);
  double computeCrossProduct(size_t, size_t *, size_t, const vector<size_t> &);
  
  double **matrix;
  size_t nrows;
  size_t rfeatures; ///< Number of raw features
  size_t ncols;
  size_t outputs;
};

/// API entrace function to create dataset to be used with other
/// functions. This function NEVER returns a 0 pointer.
MSZDataSet *createDataSet(double**, size_t, size_t, size_t = 1);

#endif // DATASET_HH
