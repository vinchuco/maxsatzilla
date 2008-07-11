#ifndef LEARNINGALGORITHM_HH
#define LEARNINGALGORITHM_HH

#include "dataset.hh"
#include "model.hh"

class LearningAlgorithm {
public:
  LearningAlgorithm(const DataSet &);

  virtual Model *run() = 0;

protected:
  const DataSet &data; ///< Reference to data is kept.
};

#endif // LEARNINGALGORITHM_HH
