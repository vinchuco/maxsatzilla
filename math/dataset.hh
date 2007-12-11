#ifndef DATASET_HH
#define DATASET_HH

#include <string>
#include <vector>

#include "pputils.hh"

using std::string;
using std::vector;

class MSZDataSet {
public:
  MSZDataSet(double**, uint, uint, uint);
  MSZDataSet(const MSZDataSet&);
  ~MSZDataSet();

  /// Given a vector of labels for matrix columns and a prefix for file names, 
  /// dumps all the files, in a readable format for GNUPlot to use.
  void dumpPlotFiles(const vector<string> &, const string &) const;
  void dumpPlotFiles(char **, uint, char *) const ;

  /// Outputs information regarding the number of timeouts
  /// segmentation faults, etc. for each solver.
  /// Shouldn't be run AFTER output standardization.
  void printSolverStats(uint, const vector<string> &);

  /// Accessors
  double getFeatureValue(uint, uint) const; 
  double getOutputValue(uint, uint)  const;

  inline uint getNRows()     const { return nrows;           }
  inline uint getNCols()     const { return ncols;           }
  inline uint getNFeatures() const { return ncols - outputs; }
  inline uint getNRFeatures()const { return rfeatures;       }
  inline uint getNOutputs()  const { return outputs;         }

  // Dataset transformations
  /// Implement basis function expansions of any given order throughout
  /// any set of partitions provided. If no partition is provided it'll perform
  /// expansion throughtout all set.
  void expand(uint);
  void expand(uint, const vector<vector<uint> > &);
  
  /// Implements standardization of the feature set
  void standardize();

  /// Implements standardization of the outputs
  void standardizeOutputs();

  /// Removes from current dataset the features in the vector
  void removeFeatures(const vector<uint> &);

  /// Removes the instances with timeouts in the given solver
  void removeTimeouts(uint, uint);

  /// Prints raw matrix, no strings attached. Just for debugging purposes.
  void printRawMatrix(); 

private:
  double *getMColumn(uint c)           const { return matrix[c];    }
  double getMValue(uint r, uint c)   const { return matrix[c][r]; }
  void setMValue(uint r, uint c, double v) { matrix[c][r] = v;    }
  void setMColumn(uint c, double *col)       { matrix[c] = col;     }

  void expandOnPartition(uint, const vector<uint> &);
  double computeCrossProduct(uint, uint *, uint, const vector<uint> &);
  
  double **matrix;   ///< Matrix of doubles allocated as an array of columns
  uint nrows;     ///< Number of rows
  uint rfeatures; ///< Number of raw features
  uint ncols;     ///< Number of columns (outputs + features)
  uint outputs;   ///< Number of outputs
  bool stdDone;     ///< Flag for feature standardization
  bool oStdDone;    ///< Flag for output standardization
};

/// API entrace function to create dataset to be used with other
/// functions. This function NEVER returns a 0 pointer.
MSZDataSet *createDataSet(double**, uint, uint, uint = 1);

#endif // DATASET_HH
