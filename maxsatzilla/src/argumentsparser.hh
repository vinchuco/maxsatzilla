#ifndef ARGUMENTSPARSER_HH
#define ARGUMENTSPARSER_HH

#include <string>
#include "config.h"
#include "arguments.hh"

extern "C" {
#include <argp.h>
}

using std::string;

namespace ArgumentsParser {
  
  /// This procedure parses the argc arguments in argv and sets appropriate flags in args.
  void parse(int argc, char *argv[], Arguments &args);

  /// Parses a single option
  error_t parseOption (int key, char *arg, struct argp_state *state);

}


#endif // ARGUMENTSPARSER_HH
