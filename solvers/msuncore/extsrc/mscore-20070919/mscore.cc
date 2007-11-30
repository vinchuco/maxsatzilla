/*----------------------------------------------------------------------------*\
 * File:        mscore.cc
 *
 * Description: Tool to extract unsat core from minisat (modified 1.14
 *              version, used in NOTOS in 2006).
 *
 * Author:      jpms
 * 
 * Revision:    $Id: mscore.cc 78 2007-07-27 17:22:20Z jpms $.
 *
 *                                     Copyright (c) 2007, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#include <string>

#include "globals.hh"
#include "logger.hh"
#include "std_clause.hh"
#include "cnf_store.hh"
#include "cnffmt.hh"
#include "ptrace.hh"
#include "Solver.hh"
#include "ucore.hh"
#include "mscore.hh"

using namespace std;

void print_header(char* wdir, char* msg);
void print_core(char* fname, Solver& ssolv, ClIDMap& cl2id, ClRefSet& clset);


/*----------------------------------------------------------------------------*\
 * Purpose: The Unsat Core extractor.
\*----------------------------------------------------------------------------*/

int main(int argc, char** argv)
{
  assert(argc>2);  // Must specify log dir
  print_header(argv[1], "Unsat core extractor");
  gzFile in = gzopen(argv[2], "rb");
  if (in == NULL) {cout<<"ERROR! Could not open file: "<<argv[2]<<"\n";exit(1);}
  CNFParser parser;
  Solver ssolv;
  ClIDMap cl2id;
  FILE* fout = (argc > 3) ? fopen(argv[3], "w") : NULL;

  parser.load_cnf_file(in, ssolv, cl2id);
  DBG(cout << "Map of Cls to IDs:\n" << cl2id << endl;);
  /* jpms:20070919 -> Must get core,
   *                  even if not okay (because of trivial unsat formula)
  if (!ssolv.okay()) { 
    CLOG(clog << "c Problem with CNF parsing. No core generated. ";);
    CLOG(clog << "Check the formula." << endl;);
    exit(1);
  }
  */
  ProofTrace pt;
  ssolv.register_proof_logger(&pt);
  bool status = ssolv.solve();

  if (fout != NULL){
    if (status){
      fprintf(fout, "SAT\n");
      for (int i = 0; i < ssolv.num_vars(); i++)
	if (ssolv.model[i] != l_Undef)
	  fprintf(fout, "%s%s%d",
		  (i==0)?"":" ", (ssolv.model[i]==l_True)?"":"-", i+1);
      fprintf(fout, " 0\n");
    } else
      fprintf(fout, "UNSAT\n");
    fclose(fout);
  }

  if (status == true) {
    CLOG(clog << "c Instance is SAT. No core generated." << endl;);
    exit(1);
  }
  assert(status == false);
  DBG(cout << "Proof Trace:\n" << pt;);

  UnsatCore uc;
  uc.compute_core(pt);
  int num_traced = pt.num_traced_clauses();
  int num_visited = pt.num_visited_clauses();
  //CLOG(clog << "c traced: " << num_traced << " vs. visited: " << num_visited << endl;);

  DBG(cout << "Unsat Core:\n" << uc;);

  print_core(argv[2], ssolv, cl2id, uc.cl_refs());
}

void print_core(char* fname, Solver& ssolv, ClIDMap& cl2id, ClRefSet& clset)
{
  //CLOG(clog << "c Input File: " << fname << endl;);
  //CLOG(clog << "c Unsat Core: " << endl;);
  //CLOG(clog << "c " << endl;);
  //CLOG(clog << "p cnf " << ssolv.num_vars() << " " << clset.size() << endl;);
  for(ClRefSet::iterator cpos=clset.begin(); cpos!=clset.end(); ++cpos) {
    ClauseRef clref = *cpos;
    assert(cl2id.find(clref) != cl2id.end());
    ClIDMap::iterator ipos = cl2id.find(clref);
    //CLOG(clog << "c ID: " << ipos->second << endl;);
    CLOG(clog << *clref;);
  }
}

void print_header(char *wdir, char *msg)
{
  string logfstr(wdir);
  logfstr.append("/");
  logfstr.append(log_file);
  const char *full_log_name = logfstr.c_str();
  FLOG(LogUtils::set_log_fname(full_log_name););
  //CLOG(clog<<"c *** "<<msg<<" ***"<<endl;);
  //CLOG(clog<<"c *** "<<toolname<<", version "<<release;);
  //CLOG(clog<<", distrib "<<dist_date;);
  //CLOG(clog<<", built "<<build_date<<" ***"<<endl;);
  //CLOG(clog<<"c *** "<<authordef<<" ***"<<endl;);
  //CLOG(clog<<"c "<<endl;);
}

/*----------------------------------------------------------------------------*/
