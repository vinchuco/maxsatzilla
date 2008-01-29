#ifndef ARGUMENTS_HH
#define ARGUMENTS_HH

#include <string>

using std::string;

struct Arguments {
  string model; ///< File containing model to evaluate.
  bool pretend; ///< If this boolean is set, no solving will actually be done.
};

#endif // ARGUMENTS_HH
