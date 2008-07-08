#include "argumentsparser.hh"

void ArgumentsParser::parse(int argc, char *argv[], Arguments &args) {
  /* Our argp parser. */
  /* Program documentation. */
  static char doc[] = 
    "MaxSatZilla is a portfolio solver able to solve MaxSat and Partial MaxSat Problems.";
  
  static char args_doc[] = "<MODEL_FILE> <CNF_FILE>";
  
  /* The options we understand. */
  static struct argp_option options[] = {
    /* Name    short  args   flags,  doc,                                   group*/
    {"pretend",     'p', 0,      0,  "Only output name of solver that would solve it, but do not execute it." },
    {"solverscfg",  's', "CFG_FILE",      0,  "Filename of the file with the solvers path. (default = solvers.cfg)"},
    { 0 }
  };

  static struct argp argp = { options, parseOption, args_doc, doc };
  
  argp_parse (&argp, argc, argv, 0, 0, &args);
}

error_t
ArgumentsParser::parseOption (int key, char *arg, struct argp_state *state) {
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  Arguments *arguments = static_cast<Arguments*>(state->input);

  switch (key) {
  case 'p':
    arguments->pretend = true;
    break;
  case 's':
    arguments->solverscfg = string(arg);
    break;
  case ARGP_KEY_ARG:
    if(arguments->model == "")
      arguments->model = string(arg);
    else if(arguments->cnffile == "")
      arguments->cnffile = string(arg);
    else 
      argp_usage(state);
    break;
  case ARGP_KEY_END:
    // There is at least one obligatory argument
    if (state->arg_num < 2) 
      argp_usage (state);
    break;
  case ARGP_KEY_INIT:
   // Initialize all arguments
    arguments->solverscfg = "solvers.cfg";
    arguments->pretend = false;
    arguments->model = "";
    arguments->cnffile = "";
    break;
    
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}
