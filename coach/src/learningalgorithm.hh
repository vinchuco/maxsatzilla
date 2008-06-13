#ifndef LEARNINGALGORITHM_HH
#define LEARNINGALGORITHM_HH

class LearningAlgorithm {
public:
  LearningAlgorithm(const MSZDataSet &);

  virtual Model run() = 0;

private:
  const MSZDataSet &data; ///< Reference to data is kept.
};

#endif // LEARNINGALGORITHM_HH
