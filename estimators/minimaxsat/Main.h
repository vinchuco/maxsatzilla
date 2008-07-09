#ifndef Main_h
#define Main_h

#include "Int.h"

//=================================================================================================


enum SolverT  { st_MiniSat, st_SatELite };
enum ConvertT { ct_Sorters, ct_Adders, ct_BDDs, ct_Mixed, ct_Undef };
enum Command  { cmd_Minimize, cmd_FirstSolution, cmd_AllSolutions };
enum FileT {ft_Pseudo,ft_Cnf,ft_Wcnf};

// -- output options:
extern bool     opt_satlive;
extern bool     opt_ansi;
extern char*    opt_cnf;
extern char*    opt_wcnf;
extern char*    opt_wcnf2;
extern int      opt_verbosity;
extern bool     opt_try;
extern int	opt_lc;
extern int	opt_heur;
extern int	opt_trans;
extern bool     opt_one_branch_only;

// -- solver options:
extern FileT    opt_file_type;
extern SolverT  opt_solver;
extern ConvertT opt_convert;
extern ConvertT opt_convert_goal;
extern bool     opt_convert_weak;
extern double   opt_bdd_thres;
extern double   opt_sort_thres;
extern double   opt_goal_bias;
extern Int      opt_goal;
extern Int      opt_upper;
extern Command  opt_command;
extern bool     opt_branch_pbvars;
extern int      opt_polarity_sug;
extern int      opt_sat;
extern int      opt_size;
extern int      opt_learn;
extern int      opt_prob;
extern int      opt_ls;

// -- files:
extern char*    opt_input;
extern char*    opt_result;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void reportf(const char* format, ...);      // 'printf()' replacer -- will put "c " first at each line if 'opt_satlive' is TRUE.
extern char* toString(Int num);
extern int   toint   (Int num);


//=================================================================================================
#endif
