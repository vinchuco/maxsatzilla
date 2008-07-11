#ifndef MODELTESTING_HH
#define MODELTESTING_HH

#include "dataset.hh"
#include "svmmodel.hh"
#include "rrmodel.hh"

namespace ModelTesting {

  void test(const Model *, const DataSet &);
  void testRRModel(const RRModel *, const DataSet &);
  void testSVMModel(const SVMModel *, const DataSet &);

};
#endif // MODELTESTING_HH
