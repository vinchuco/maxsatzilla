#ifndef DATASET_HH
#define DATASET_HH

class MSZDataSet {
public:
  MSZDataSet(double**, int, int, int);
  ~MSZDataSet();

private:
  double **matrix;
  int nrows;
  int ncols;
  int outputs;
};

/// API entrace function to create dataset to be used with other
/// functions.
MSZDataSet *createDataSet(double**, int, int, int = 1);

#endif // DATASET_HH
