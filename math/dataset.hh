#ifndef DATASET_HH
#define DATASET_HH

#include <string>
#include <vector>
#include <utility>

#include "pputils.hh"

using std::string;
using std::vector;
using std::pair;

class MSZDataSet {
public:
  MSZDataSet(const double* const*, uint, uint, const string*, const double*, const string&);
  MSZDataSet(const MSZDataSet&);
  ~MSZDataSet();

  /// Given a vector of labels for matrix columns and a prefix for file names, 
  /// dumps all the files, in a readable format for GNUPlot to use.
  void dumpPlotFiles(const string &) const;

  /// Outputs information regarding the number of timeouts
  /// segmentation faults, etc. for each solver.
  /// Shouldn't be run AFTER output standardization.
  void printSolverStats(uint);

  /// Accessors
  double getFeatureValue(uint, uint) const; 
  double getOutputValue(uint)        const;

  uint getNRows()     const { return nrows;           }
  uint getNCols()     const { return ncols;           }
  uint getNFeatures() const { return ncols;           }
  uint getNRFeatures()const { return rfeatures;       }
  string getColLabel(uint i) const { return labels[i]; }
  string getOutputLabel() const { return vecLabel;      }

  // Dataset transformations
  /// Implement basis function expansions of any given order throughout
  /// any set of partitions provided. If no partition is provided it'll perform
  /// expansion throughtout all set.
  void expand(uint);
  void expand(uint, const vector<vector<uint> > &);
  
  /// Implements standardization of the feature set
  void standardize();

  /// Implements standardization of the outputs
  void standardizeOutput();

  /// Removes from current dataset the features in the vector
  void removeFeatures(const vector<uint> &);

  /// Removes the instances with timeouts in the given solver
  void removeTimeouts(uint);

  /// Prints raw matrix, no strings attached. Just for debugging purposes.
  void printRawMatrix(); 

private:
  double *getMColumn(uint c)           const { return matrix[c];    }
  double  getMValue(uint r, uint c)    const { return matrix[c][r]; }
  double *getVector()                  const { return ovec;         }
  double  getVValue(uint i)            const { return ovec[i];      }

  void    setMValue(uint r, uint c, double v){ matrix[c][r] = v;    }
  void    setMColumn(uint c, double *col)    { matrix[c] = col;     }
  void    setVector(double *vec)             { ovec = vec;          }
  void    setVValue(uint i, double v)        { ovec[i] = v;         }

  void   expandOnPartition(uint, const vector<uint> &);
  double computeCrossProduct(uint, uint *, uint, const vector<uint> &);
  
  double **matrix;       ///< Matrix of doubles allocated as an array of columns
  double * ovec;         ///< Vector of outputs
  uint nrows;            ///< Number of rows
  uint rfeatures;        ///< Number of raw features
  uint ncols;            ///< Number of columns (outputs + features)
  vector<string> labels; ///< Labels for each column of matrix
  string vecLabel;       ///< Label for the vector column
  bool stdDone;          ///< Flag for feature standardization
  bool oStdDone;         ///< Flag for output standardization
};

/// API entrace function to create dataset to be used with other
/// functions. This function NEVER returns a 0 pointer.
MSZDataSet *createDataSet(const double* const*, 
			  uint, 
			  uint, 
			  const string*, 
			  const double*, 
			  const string&);

pair<MSZDataSet *, MSZDataSet *> createDataSets(double**, 
						uint, 
						uint, 
						const string*, 
						const double*, 
						const string&,
						uint,
						uint);

#endif // DATASET_HH
