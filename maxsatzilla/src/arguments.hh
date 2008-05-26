#ifndef ARGUMENTS_HH
#define ARGUMENTS_HH

#include <string>

using std::string;

struct Arguments {
  string solverscfg;///< Solver config file
  string model;     ///< File containing model to evaluate
  string cnffile;   ///< CNF file to solve
  bool pretend;     ///< If this boolean is set, no solving will actually be done
};

#endif // ARGUMENTS_HH
