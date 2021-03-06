/*----------------------------------------------------------------------------*\
 * Version: $Id: proof_logger.hh 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: Basic proof logging interface, guaranteed by SAT solvers
 *              used in NOTOS.
 *
 *                               Copyright (c) 2005-2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#ifndef _PROOF_LOGGER_HH_
#define _PROOF_LOGGER_HH_ 1

#include "std_clause.hh"


/*----------------------------------------------------------------------------*\
 * Class: ProofLogger
 *
 * Purpose: Proof logging functions, which enable recovering a resolution
 *          proof
\*----------------------------------------------------------------------------*/

class ProofLogger {

public:    // Functions that build the trace

  ProofLogger() { }                       // Default onstructor

  virtual ~ProofLogger() { }              // Virtual destructor

  virtual void begin_cl_trace() = 0;      // Start logging a learnt cl

  virtual void add_dep_pair(ClauseRef cl, int lit) = 0;  // Add another dep pair

  virtual void end_cl_trace(ClauseRef cl) = 0;    // Finish logging wrt cl cl

};

#endif /* _PROOF_LOGGER_HH_ */

/*----------------------------------------------------------------------------*/
