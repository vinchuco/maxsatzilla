#ifndef TIMEOUTMGM_HH
#define TIMEOUTMGM_HH

#include "pputils.hh"

/*
 * This function implements the management of timeouts as
 * censored data as described in the paper
 * "A simple method for regression analysis with censored data"
 * - Schmee and Hahn
 * Technometrics 1979
 */
namespace TimeoutMgm {
  void predictTimeouts(double **, uint, uint, double *, uint, double);
};

#endif // TIMEOUTMGM_HH
