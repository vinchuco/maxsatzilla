#ifndef MODELTESTING_HH
#define MODELTESTING_HH

#include "dataset.hh"
#include "svmmodel.hh"
#include "rrmodel.hh"

namespace ModelTesting {

  void test(const Model *, const MSZDataSet &);
  void testRRModel(const RRModel *, const MSZDataSet &);
  void testSVMModel(const SVMModel *, const MSZDataSet &);

};
#endif // MODELTESTING_HH
