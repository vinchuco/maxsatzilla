/* Based on maxsatz1.c, including look ahead further on unit clauses
 */
/*based on maxsatz5.c, moving lookahead before branching (instead of after
in maxsatz5.c
*/

/*based on maxsatz8.c, integrating resolution rule:

if x or y and x or �y are clauses, then remove these two binary clauses
and add the unit clause x

*/

/* based on maxsatz9.c, integrating advanced so-called star rule:

if x, y and �x or �y are clauses then remove these three clauses, increment
NB_EMPTY by 1 and add clause x or y.

*/

/* based on maxsatz10.c, generalize the rule of maxsatz10.c for all
linear implications to a contradiction
*/

/* based on maxsatz11.c
 */

/* based on maxsatz14.c. When UB-#NB_EMPTY=1, perform unit propagation
 */

/* based on maxsatz15.c. Copy active unitclauses in a special stack before
 look-ahead for computing LB
*/

/* based on maxsatz16.c. Two-level breadth-first search to propagate unit clauses
in lookahead for computing LB: pick an actual unit clause c, propagate all newly
generated unit clauses by c, then pick the next actual unit clause, do the same
thing.
An actual unit clause is an existing clause before look-ahead starts
*/

/* based on maxsatz17, remove rules 4, 5, 6 but keep rule 3
 */

/* Based on maxsatz17, 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/times.h>
#include <sys/types.h>
#include <limits.h>
#include <string.h>

typedef signed char my_type;
typedef unsigned char my_unsigned_type;

#define WORD_LENGTH 100 
#define TRUE 1
#define FALSE 0
#define NONE -1

#define WEIGHT 4
#define WEIGHT1 25
#define WEIGHT2 5
#define WEIGHT3 1
#define T 10

/* the tables of variables and clauses are statically allocated. Modify the 
   parameters tab_variable_size and tab_clause_size before compilation if 
   necessary */
#define tab_variable_size  10000
#define tab_clause_size 40000
#define tab_unitclause_size \
 ((tab_clause_size/4<2000) ? 2000 : tab_clause_size/4)
#define my_tab_variable_size \
 ((tab_variable_size/2<1000) ? 1000 : tab_variable_size/2)
#define my_tab_clause_size \
 ((tab_clause_size/2<2000) ? 2000 : tab_clause_size/2)
#define my_tab_unitclause_size \
 ((tab_unitclause_size/2<1000) ? 1000 : tab_unitclause_size/2)
#define tab_literal_size 2*tab_variable_size
#define double_tab_clause_size 2*tab_clause_size
#define positive(literal) literal<NB_VAR
#define negative(literal) literal>=NB_VAR
#define get_var_from_lit(literal) \
  ((literal<NB_VAR) ? literal : literal-NB_VAR)
#define complement(lit1, lit2) \
 ((lit1<lit2) ? lit2-lit1 == NB_VAR : lit1-lit2 == NB_VAR)

#define inverse_signe(signe) \
 (signe == POSITIVE) ? NEGATIVE : POSITIVE
#define unsat(val) (val==0)?"UNS":"SAT"
#define pop(stack) stack[--stack ## _fill_pointer]
#define push(item, stack) stack[stack ## _fill_pointer++] = item
#define satisfiable() CLAUSE_STACK_fill_pointer == NB_CLAUSE

#define NEGATIVE 0
#define POSITIVE 1
#define PASSIVE 0
#define ACTIVE 1

int *neg_in[tab_variable_size];
int *pos_in[tab_variable_size];
int neg_nb[tab_variable_size];
int pos_nb[tab_variable_size];
my_type var_current_value[tab_variable_size];
my_type var_rest_value[tab_variable_size];
my_type var_state[tab_variable_size];
my_type solution[tab_variable_size];

int saved_clause_stack[tab_variable_size];
int saved_reducedclause_stack[tab_variable_size];
int saved_unitclause_stack[tab_variable_size];
int saved_nb_empty[tab_variable_size];
my_unsigned_type nb_neg_clause_of_length1[tab_variable_size];
my_unsigned_type nb_pos_clause_of_length1[tab_variable_size];
my_unsigned_type nb_neg_clause_of_length2[tab_variable_size];
my_unsigned_type nb_neg_clause_of_length3[tab_variable_size];
my_unsigned_type nb_pos_clause_of_length2[tab_variable_size];
my_unsigned_type nb_pos_clause_of_length3[tab_variable_size];

float reduce_if_negative[tab_variable_size];
float reduce_if_positive[tab_variable_size];

int *sat[tab_clause_size];
int *var_sign[tab_clause_size];
my_type clause_state[tab_clause_size];
my_type clause_length[tab_clause_size];

int VARIABLE_STACK_fill_pointer = 0;
int CLAUSE_STACK_fill_pointer = 0;
int UNITCLAUSE_STACK_fill_pointer = 0;
int REDUCEDCLAUSE_STACK_fill_pointer = 0;


int VARIABLE_STACK[tab_variable_size];
int CLAUSE_STACK[tab_clause_size];
int UNITCLAUSE_STACK[tab_unitclause_size];
int REDUCEDCLAUSE_STACK[tab_clause_size*10];

int PREVIOUS_REDUCEDCLAUSE_STACK_fill_pointer = 0;

int NB_VAR;
int NB_CLAUSE;
int INIT_NB_CLAUSE;
int REAL_NB_CLAUSE;

long NB_UNIT=1, NB_MONO=0, NB_BRANCHE=0, NB_BACK = 0;
int NB_EMPTY=0, UB;

#define NO_CONFLICT -3
#define NO_REASON -3
int reason[tab_variable_size];
int REASON_STACK[tab_variable_size];
int REASON_STACK_fill_pointer=0;

int MY_UNITCLAUSE_STACK[tab_variable_size];
int MY_UNITCLAUSE_STACK_fill_pointer=0;
int CANDIDATE_LITERALS[2*tab_variable_size];
int CANDIDATE_LITERALS_fill_pointer=0;
int NEW_CLAUSES[tab_clause_size][7];
int NEW_CLAUSES_fill_pointer=0;
int lit_to_fix[tab_clause_size];
int *SAVED_CLAUSE_POSITIONS[tab_clause_size];
int SAVED_CLAUSES[tab_clause_size];
int SAVED_CLAUSES_fill_pointer=0;
int lit_involved_in_clause[2*tab_variable_size];
int INVOLVED_LIT_STACK[2*tab_variable_size];
int INVOLVED_LIT_STACK_fill_pointer=0;
int fixing_clause[2*tab_variable_size];
int saved_nb_clause[tab_variable_size];
int saved_saved_clauses[tab_variable_size];
int saved_new_clauses[tab_variable_size];

#include "input.c"

void remove_clauses(int var) {
  register int clause;
  register int *clauses;
  if (var_current_value[var] == POSITIVE) clauses = pos_in[var];
  else clauses = neg_in[var];
  for(clause=*clauses; clause!=NONE; clause=*(++clauses)) {
    if (clause_state[clause] == ACTIVE) {
      clause_state[clause] = PASSIVE;
      push(clause, CLAUSE_STACK);
    }
  }
}

int reduce_clauses(int var) {
  register int clause;
  register int *clauses;
  if (var_current_value[var] == POSITIVE) clauses = neg_in[var];
  else clauses = pos_in[var];
  for(clause=*clauses; clause!=NONE; clause=*(++clauses)) {
    if (clause_state[clause] == ACTIVE) {
      clause_length[clause]--;
      push(clause, REDUCEDCLAUSE_STACK);
      switch (clause_length[clause]) {
      case 0: NB_EMPTY++;
	if (UB<=NB_EMPTY) return NONE;
	break;
      case 1: 
	push(clause, UNITCLAUSE_STACK);
	break;
      }
    }
  }
  return TRUE;
}

int my_reduce_clauses(int var) {
  register int clause;
  register int *clauses;
  if (var_current_value[var] == POSITIVE) clauses = neg_in[var];
  else clauses = pos_in[var];
  for(clause=*clauses; clause!=NONE; clause=*(++clauses)) {
    if (clause_state[clause] == ACTIVE) {
      clause_length[clause]--;
      push(clause, REDUCEDCLAUSE_STACK);
      switch (clause_length[clause]) {
      case 0: return clause;
      case 1: 
	push(clause, MY_UNITCLAUSE_STACK);
	break;
      }
    }
  }
  return NO_CONFLICT;
}

int my_reduce_clauses_for_fl(int var) {
  register int clause;
  register int *clauses;
  if (var_current_value[var] == POSITIVE) clauses = neg_in[var];
  else clauses = pos_in[var];
  for(clause=*clauses; clause!=NONE; clause=*(++clauses)) {
    if (clause_state[clause] == ACTIVE) {
      clause_length[clause]--;
      push(clause, REDUCEDCLAUSE_STACK);
      switch (clause_length[clause]) {
      case 0: return clause;
      case 1: 
	push(clause, UNITCLAUSE_STACK);
	break;
      }
    }
  }
  return NO_CONFLICT;
}

void print_values(int nb_var) {
  FILE* fp_out;
  int i;
  fp_out = fopen("satx.sol", "w");
  for (i=0; i<nb_var; i++) {
    if (var_current_value[i] == 1) 
      fprintf(fp_out, "%d ", i+1);
    else
      fprintf(fp_out, "%d ", 0-i-1);
  }
  fprintf(fp_out, "\n");
  fclose(fp_out);			
} 

int backtracking() {
  int var, index, *position, saved;
      
  NB_BACK++;

  do {
    var = pop(VARIABLE_STACK);
    if (var_rest_value[var] == NONE) 
      var_state[var] = ACTIVE;
    else {
      for (index = saved_clause_stack[var]; 
	   index < CLAUSE_STACK_fill_pointer;
	   index++)
	clause_state[CLAUSE_STACK[index]] = ACTIVE;
      CLAUSE_STACK_fill_pointer = saved_clause_stack[var];

      for (index = saved_reducedclause_stack[var];
	   index < REDUCEDCLAUSE_STACK_fill_pointer;
	   index++) {	
	//clause = REDUCEDCLAUSE_STACK[index];
	clause_length[REDUCEDCLAUSE_STACK[index]]++;
      }
      REDUCEDCLAUSE_STACK_fill_pointer = saved_reducedclause_stack[var];
      UNITCLAUSE_STACK_fill_pointer=saved_unitclause_stack[var];
      NB_EMPTY=saved_nb_empty[var];
      NB_CLAUSE=saved_nb_clause[var];
      NEW_CLAUSES_fill_pointer=saved_new_clauses[var];
      
      saved=saved_saved_clauses[var];
      for (index = SAVED_CLAUSES_fill_pointer-1 ;
	   index >= saved;
	   index--) 
	*SAVED_CLAUSE_POSITIONS[index]=SAVED_CLAUSES[index];
      SAVED_CLAUSES_fill_pointer=saved;

      if (NB_EMPTY<UB) {
	var_current_value[var] = var_rest_value[var];
	var_rest_value[var] = NONE;
	push(var, VARIABLE_STACK);
	if (reduce_clauses(var)==NONE)
	  return NONE;
	remove_clauses(var);
	return TRUE;
      }
      else  var_state[var] = ACTIVE;
    }
  } while (VARIABLE_STACK_fill_pointer > 0);
  return FALSE;
}

int verify_solution() {
  int i, nb=0, var, *vars_signs, clause_truth,cpt;

  for (i=0; i<REAL_NB_CLAUSE; i++) {
    clause_truth = FALSE;
    vars_signs = var_sign[i];
    for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2))
      if (*(vars_signs+1) == var_current_value[var] ) {
	clause_truth = TRUE;
	break;
      }
    if (clause_truth == FALSE) nb++;
  }
  for (i=0; i<NB_VAR; i++)
    solution[i] = var_current_value[i];
  return nb;
}

void reset_context(int saved_clause_stack_fill_pointer, 
		    int saved_reducedclause_stack_fill_pointer,
		    int saved_unitclause_stack_fill_pointer,
		    int saved_variable_stack_fill_pointer) {
  int index, var, clause;
  for (index = saved_clause_stack_fill_pointer; 
       index < CLAUSE_STACK_fill_pointer;
       index++)
    clause_state[CLAUSE_STACK[index]] = ACTIVE;
  CLAUSE_STACK_fill_pointer = saved_clause_stack_fill_pointer;

  for (index = saved_reducedclause_stack_fill_pointer;
       index < REDUCEDCLAUSE_STACK_fill_pointer;
       index++) {	
    clause = REDUCEDCLAUSE_STACK[index];
    clause_length[REDUCEDCLAUSE_STACK[index]]++;
  }
  REDUCEDCLAUSE_STACK_fill_pointer = saved_reducedclause_stack_fill_pointer;

  for(index=saved_variable_stack_fill_pointer;
      index<VARIABLE_STACK_fill_pointer;
      index++) {
    var=VARIABLE_STACK[index];
    reason[var]=NO_REASON;
    var_state[var]=ACTIVE;
  }
  VARIABLE_STACK_fill_pointer=saved_variable_stack_fill_pointer;

  UNITCLAUSE_STACK_fill_pointer=saved_unitclause_stack_fill_pointer;
}

int replace_clause(int newclause, int clause_to_replace, int *clauses) {
  int clause, flag=FALSE;
  
  for(clause=*clauses; clause!=NONE; clause=*(++clauses)) {
    if (clause==clause_to_replace) {
      *clauses=newclause;
      SAVED_CLAUSE_POSITIONS[SAVED_CLAUSES_fill_pointer]=clauses;
      push(clause_to_replace, SAVED_CLAUSES);
      flag=TRUE;
      break;
    }
  }
  if (flag==FALSE)
    printf("problem..."), exit(0);
  return flag;
}

void create_binaryclause(int var1, int sign1, int var2, int sign2, 
			 int clause1, int clause2) {
  int clause, *vars_signs, flag=FALSE, *clauses1, *clauses2;
  if (sign1==POSITIVE) clauses1=pos_in[var1]; else clauses1=neg_in[var1];
  if (sign2==POSITIVE) clauses2=pos_in[var2]; else clauses2=neg_in[var2];
  vars_signs=NEW_CLAUSES[NEW_CLAUSES_fill_pointer++];
  if (var1<var2) {
    vars_signs[0]=var1; vars_signs[1]=sign1;
    vars_signs[2]=var2; vars_signs[3]=sign2;
  }
  else {
    vars_signs[0]=var2; vars_signs[1]=sign2;
    vars_signs[2]=var1; vars_signs[3]=sign1;
  }
  vars_signs[4]=NONE;
  var_sign[NB_CLAUSE]=vars_signs;
  clause_state[NB_CLAUSE]=ACTIVE;
  clause_length[NB_CLAUSE]=2;
  // if (NB_CLAUSE==305)
  // printf("aaa...");
  replace_clause(NB_CLAUSE, clause1, clauses1);
  replace_clause(NB_CLAUSE, clause2, clauses2);
  NB_CLAUSE++;
}

int verify_binary_clauses(int *varssigns, int var1, int sign1, int var2, int sign2) {
  int nb=0;

  if (var1==*varssigns) {
    if ((*(varssigns+1)!=1-sign1) || (var2!=*(varssigns+2)) ||
	(*(varssigns+3)!=1-sign2)) {
      printf("problem.."), exit(0);
      return FALSE;
    }
  }
  else {
    if ((var2 != *varssigns) || (*(varssigns+1)!=1-sign2) || (var1!=*(varssigns+2)) ||
	(*(varssigns+3)!=1-sign1)) {
      printf("problem.."), exit(0);
      return FALSE;
    }
  }
  return TRUE;
}

int CLAUSES_TO_REMOVE[tab_clause_size];
int CLAUSES_TO_REMOVE_fill_pointer=0;

int create_clause_from_conflict_clauses(int clause1, int clause2, int clause3) {
  int var3, sign3, var2, sign2,*clauses2, *clauses3, *vars_signs, 
    varssigns[4], i=0, var;

  if ((clause_state[clause1]==ACTIVE) && (clause_length[clause1]==2) &&
      (clause_state[clause2]==ACTIVE) && (clause_length[clause2]==1) &&
      (clause_state[clause3]==ACTIVE) && (clause_length[clause3]==1)) {
    vars_signs = var_sign[clause1];
    for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
      if (var_state[var]==ACTIVE) {
	varssigns[i++]=var; varssigns[i++]=*(vars_signs+1);
      }
    }
    /*
    vars_signs = var_sign[clause2];
    for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
      if (var_state[var]==ACTIVE) {
	var2=var; sign2=*(vars_signs+1);
      }
    }
    vars_signs = var_sign[clause3];
    for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
      if (var_state[var]==ACTIVE) {
	var3=var; sign3=*(vars_signs+1);
      }
    }
    verify_binary_clauses(varssigns, var2, sign2, var3, sign3);
    */
    var2=varssigns[0]; sign2=1-varssigns[1];
    var3=varssigns[2]; sign3=1-varssigns[3];
    create_binaryclause(var2, sign2, var3, sign3, clause2, clause3);
    push(clause1, CLAUSES_TO_REMOVE);
    push(clause2, CLAUSES_TO_REMOVE);
    push(clause3, CLAUSES_TO_REMOVE);
    return TRUE;
  }
  else {
    return FALSE;
  }
}

int LINEAR_REASON_STACK1[tab_clause_size];
int LINEAR_REASON_STACK1_fill_pointer=0;
int LINEAR_REASON_STACK2[tab_clause_size];
int LINEAR_REASON_STACK2_fill_pointer=0;
int clause_involved[tab_clause_size];

int search_linear_reason1(int var) {
  int *vars_signs, clause, fixed_var, index_var, new_fixed_var;

  for(fixed_var=var; fixed_var!=NONE; fixed_var=new_fixed_var) {
    clause=reason[fixed_var];
    vars_signs = var_sign[clause]; new_fixed_var=NONE;
    push(clause, LINEAR_REASON_STACK1);
    clause_involved[clause]=TRUE;
    for(index_var=*vars_signs; index_var!=NONE; index_var=*(vars_signs+=2)) {
      if ((index_var!=fixed_var) && (reason[index_var]!=NO_REASON)) {
	if (new_fixed_var==NONE)
	  new_fixed_var=index_var;
	else return FALSE;
      }
    }
  }
  return TRUE;
}

#define SIMPLE_NON_LINEAR_CASE 2

int search_linear_reason2(int var) {
  int *vars_signs, clause, fixed_var, index_var, new_fixed_var;

  for(fixed_var=var; fixed_var!=NONE; fixed_var=new_fixed_var) {
    clause=reason[fixed_var];
    if (clause_involved[clause]==TRUE) {
      if ( LINEAR_REASON_STACK2_fill_pointer == 2 &&
	   LINEAR_REASON_STACK1_fill_pointer > 2 &&
	   LINEAR_REASON_STACK1[ 2 ] == clause ) 
	return SIMPLE_NON_LINEAR_CASE;
      else
	return FALSE;
    }
    else 
      push(clause, LINEAR_REASON_STACK2);
    vars_signs = var_sign[clause]; new_fixed_var=NONE;
    for(index_var=*vars_signs; index_var!=NONE; index_var=*(vars_signs+=2)) {
      if ((index_var!=fixed_var) && (reason[index_var]!=NO_REASON)) {
	if (new_fixed_var==NONE)
	  new_fixed_var=index_var;
	else return FALSE;
      }
    }
  }
  return TRUE;
}

// clause1 is l1->l2, clause is l2->l3, clause3 is ((not l3) or (not l4))
// i.e., the reason of l2 is clause1, the reason of l3 is clause
int check_reason(int *varssigns, int clause, int clause1, int clause2) {
  int var, *vars_signs, var1, var2, flag;

  if ((reason[varssigns[0]]!=clause1) || (reason[varssigns[2]]!=clause)) 
    return FALSE;
  vars_signs = var_sign[clause2]; flag=FALSE;
  for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
    if ((varssigns[2]==var) && (reason[var]!=NO_REASON) && 
	(*(vars_signs+1) != var_current_value[var])) {
      flag=TRUE;
    }
  }
  return flag;
}

int create_complementary_binclause(int clause, int clause1, int clause2) {
  int var, *vars_signs, i=0, varssigns[4], sign, j=0;
  vars_signs = var_sign[clause];
  for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
    if (reason[var]!=NO_REASON) {
      varssigns[i++]=var; varssigns[i++]=*(vars_signs+1); 
    }
  }
  if (reason[varssigns[2]]==clause1) {
    var=varssigns[2]; sign=varssigns[3];
    varssigns[2]=varssigns[0]; varssigns[3]=varssigns[1];
    varssigns[0]=var; varssigns[1]=sign;
  }
  if ((i!=4) || (check_reason(varssigns, clause, clause1, clause2)==FALSE))
    printf("problem..."), exit(0);
  create_binaryclause(varssigns[0], 1-varssigns[1],
		      varssigns[2], 1-varssigns[3], clause1, clause2);
  return TRUE;
}

int get_satisfied_literal(int clause) {
  int var, *vars_signs;
  vars_signs = var_sign[clause];
  for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
    if (*(vars_signs+1) == var_current_value[var])
      return var;
  }
  printf("erreur"), exit(0);
  return NONE;
}

void create_ternary_clauses(int var1, int sign1, int var2, int sign2, 
			    int var3, int sign3, int clause1, 
			    int clause2, int clause3) {
  int clause, *vars_signs, flag=FALSE, *clauses1, *clauses2, *clauses3;
  if (sign1==POSITIVE) clauses1=pos_in[var1]; else clauses1=neg_in[var1];
  if (sign2==POSITIVE) clauses2=pos_in[var2]; else clauses2=neg_in[var2];
  if (sign3==POSITIVE) clauses3=pos_in[var3]; else clauses3=neg_in[var3];
  vars_signs=NEW_CLAUSES[NEW_CLAUSES_fill_pointer++];
  vars_signs[0]=var1; vars_signs[1]=sign1;
  vars_signs[2]=var2; vars_signs[3]=sign2;
  vars_signs[4]=var3; vars_signs[5]=sign3;
  vars_signs[6]=NONE;
  var_sign[NB_CLAUSE]=vars_signs;
  clause_state[NB_CLAUSE]=ACTIVE;
  clause_length[NB_CLAUSE]=3;
  // if (NB_CLAUSE==305)
  // printf("aaa...");
  replace_clause(NB_CLAUSE, clause1, clauses1);
  replace_clause(NB_CLAUSE, clause2, clauses2);
  replace_clause(NB_CLAUSE, clause3, clauses3);
  NB_CLAUSE++;
}  

int non_linear_conflict(int empty_clause, int var1, 
			int sign1, int var2, int sign2) {
  int var, sign, j;
  // driving unit clause is LINEAR_REASON_STACK1[2] (propagate
  // it resulting the empty_clause by simple non-linear derivation
  // var1, sign1, var2, and sign2 are the two literals of empty_clause
  var=get_satisfied_literal(LINEAR_REASON_STACK1[2]);
  sign=var_current_value[var];
  for(j=2; j<LINEAR_REASON_STACK1_fill_pointer-1; j++) {
    create_complementary_binclause(LINEAR_REASON_STACK1[j],
				   LINEAR_REASON_STACK1[j+1],
				   LINEAR_REASON_STACK1[j-1]);
    push(LINEAR_REASON_STACK1[j], CLAUSES_TO_REMOVE);
  }
  push(LINEAR_REASON_STACK1[j], CLAUSES_TO_REMOVE);
  create_ternary_clauses(var, sign, var1, sign1, var2, sign2,
			 LINEAR_REASON_STACK1[2],
			 empty_clause, empty_clause);
  create_ternary_clauses(var, 1-sign, var1, 1-sign1, var2, 1-sign2,
			 LINEAR_REASON_STACK2[1],
			 LINEAR_REASON_STACK1[1],
			 LINEAR_REASON_STACK2[1]);
  push(empty_clause, CLAUSES_TO_REMOVE);
  push( LINEAR_REASON_STACK1[1], CLAUSES_TO_REMOVE);
  push( LINEAR_REASON_STACK2[1], CLAUSES_TO_REMOVE);
  return TRUE;
}

	
int linear_conflict(int clause) {
  int var, *vars_signs, i=0, varssigns[6], j=0, res;
  vars_signs = var_sign[clause];
  for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
    if (reason[var]!=NO_REASON) {
      varssigns[i++]=var; varssigns[i++]=*(vars_signs+1); 
      if (i>4)
	return FALSE;
    }
  }
  if (i>4) return FALSE;
  if (i==0)
    printf("bizzar...\n"), exit(0);
  else {
    for(j=0; j<LINEAR_REASON_STACK1_fill_pointer; j++) 
      clause_involved[LINEAR_REASON_STACK1[j]]=NONE;
    LINEAR_REASON_STACK1_fill_pointer=1; LINEAR_REASON_STACK2_fill_pointer=1;
    LINEAR_REASON_STACK1[0]=clause; LINEAR_REASON_STACK2[0]=clause;
    if (search_linear_reason1(varssigns[0])==FALSE)
      return FALSE;
    else {
      if (i==4) {
	res=search_linear_reason2(varssigns[2]);
	if (res==FALSE)
	  return FALSE;
	else if (res==SIMPLE_NON_LINEAR_CASE) {
	  // printf("zskjehrz  \n");
	  return non_linear_conflict(clause, varssigns[0], varssigns[1], 
				     varssigns[2], varssigns[3]);
	}
	create_binaryclause(varssigns[0], 1-varssigns[1], 
			    varssigns[2], 1-varssigns[3], 
			    LINEAR_REASON_STACK1[1], LINEAR_REASON_STACK2[1]);
	for(j=1; j<LINEAR_REASON_STACK2_fill_pointer-1; j++) {
	  create_complementary_binclause(LINEAR_REASON_STACK2[j],
					 LINEAR_REASON_STACK2[j+1],
					 LINEAR_REASON_STACK2[j-1]);
	  push(LINEAR_REASON_STACK2[j], CLAUSES_TO_REMOVE);
	}
	push(LINEAR_REASON_STACK2[j], CLAUSES_TO_REMOVE);
      }
      push(clause, CLAUSES_TO_REMOVE);
      for(j=1; j<LINEAR_REASON_STACK1_fill_pointer-1; j++) {
	create_complementary_binclause(LINEAR_REASON_STACK1[j],
				       LINEAR_REASON_STACK1[j+1],
				       LINEAR_REASON_STACK1[j-1]);
	push(LINEAR_REASON_STACK1[j], CLAUSES_TO_REMOVE);
      }
      push(LINEAR_REASON_STACK1[j], CLAUSES_TO_REMOVE);
    }
    return TRUE;
  }
}

void remove_linear_reasons() {
  int i, clause;
  for(i=0; i<LINEAR_REASON_STACK1_fill_pointer; i++) {
    clause=LINEAR_REASON_STACK1[i];
    clause_state[clause]=PASSIVE;
    push(clause, CLAUSE_STACK);
  }
  for(i=1; i<LINEAR_REASON_STACK2_fill_pointer; i++) {
    clause=LINEAR_REASON_STACK2[i];
    clause_state[clause]=PASSIVE;
    push(clause, CLAUSE_STACK);
  }
}      

int there_is_unit_clause( int var_to_check ) {
  int unitclause_position, unitclause, var, *vars_signs;

  for( unitclause_position = 0;
       unitclause_position < UNITCLAUSE_STACK_fill_pointer;
       unitclause_position++) {
    unitclause = UNITCLAUSE_STACK[ unitclause_position ];
    if ((clause_state[unitclause] == ACTIVE)  && (clause_length[unitclause]>0))
      {
        vars_signs = var_sign[unitclause];
        for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
          if ( var == var_to_check && var_state[var] == ACTIVE ) {
            return TRUE;
          }
        }
      }
  }
  return FALSE;
}

int assign_and_unitclause_process( int var, int value, int starting_point ) {
  int clause;
  var_current_value[var] = value;
  var_rest_value[var] = NONE;
  var_state[var] = PASSIVE;
  push(var, VARIABLE_STACK);
  if ((clause=my_reduce_clauses_for_fl(var))==NO_CONFLICT) {
    remove_clauses(var);
    return my_unitclause_process( starting_point );
  }
  else {
    return clause;
  }
}

int store_reason_clauses( int clause, int starting ) {
  int *vars_signs, var, i;
  push(clause, REASON_STACK);
  for(i=starting; i<REASON_STACK_fill_pointer; i++) {
    clause=REASON_STACK[i];
    vars_signs = var_sign[clause];
    for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
      if (reason[var]!=NO_REASON) {
        push(reason[var], REASON_STACK);
        reason[var]=NO_REASON;
      }
    }
  }
  return i;
}

void remove_reason_clauses() {
  int i, clause;
  for(i=0; i<REASON_STACK_fill_pointer; i++) {
    clause=REASON_STACK[i];
    clause_state[clause]=PASSIVE;
    push(clause, CLAUSE_STACK);
  }
  REASON_STACK_fill_pointer=0;
}

int failed_literal( int conflict ) {
  int clause, var, la = 0;
  int saved_clause_stack_fill_pointer, saved_reducedclause_stack_fill_pointer,
    saved_unitclause_stack_fill_pointer, saved_variable_stack_fill_pointer,
    my_saved_clause_stack_fill_pointer, saved_reason_stack_fill_pointer;

  saved_clause_stack_fill_pointer= CLAUSE_STACK_fill_pointer;
  saved_reducedclause_stack_fill_pointer = REDUCEDCLAUSE_STACK_fill_pointer;
  saved_unitclause_stack_fill_pointer = UNITCLAUSE_STACK_fill_pointer;
  saved_variable_stack_fill_pointer=VARIABLE_STACK_fill_pointer;
  my_saved_clause_stack_fill_pointer= CLAUSE_STACK_fill_pointer;

  for( var=0; var < NB_VAR && la+conflict+NB_EMPTY<UB; var++ ) {
    if ( var_state[ var ] == ACTIVE &&
         !there_is_unit_clause( var )) {
      simple_get_pos_clause_nb(var); simple_get_neg_clause_nb(var);
      if (nb_neg_clause_of_length2[ var ] > 1 &&  nb_pos_clause_of_length2[ var ] > 1 ) {
      //do {
        if ((clause=assign_and_unitclause_process(var, FALSE, saved_unitclause_stack_fill_pointer))!=NO_CONFLICT) {
	  //  printf("One conflict found\n");
          saved_reason_stack_fill_pointer = store_reason_clauses( clause, 0 );
          reset_context(my_saved_clause_stack_fill_pointer,
                        saved_reducedclause_stack_fill_pointer,
                        saved_unitclause_stack_fill_pointer,
                        saved_variable_stack_fill_pointer);
          //remove_reason_clauses();
          //saved_reason_stack_fill_pointer = 0;
          my_saved_clause_stack_fill_pointer=CLAUSE_STACK_fill_pointer;
          if ((clause=assign_and_unitclause_process(var, TRUE, saved_unitclause_stack_fill_pointer))>=0) {
            la++;
            store_reason_clauses( clause, saved_reason_stack_fill_pointer );
            reset_context(my_saved_clause_stack_fill_pointer,
                          saved_reducedclause_stack_fill_pointer,
                          saved_unitclause_stack_fill_pointer,
                          saved_variable_stack_fill_pointer);
            remove_reason_clauses();
            my_saved_clause_stack_fill_pointer=CLAUSE_STACK_fill_pointer;
         } else {
            REASON_STACK_fill_pointer = 0;
            reset_context(my_saved_clause_stack_fill_pointer,
                          saved_reducedclause_stack_fill_pointer,
                          saved_unitclause_stack_fill_pointer,
                          saved_variable_stack_fill_pointer);
          }
        } else {
          reset_context(my_saved_clause_stack_fill_pointer,
                        saved_reducedclause_stack_fill_pointer,
                        saved_unitclause_stack_fill_pointer,
                        saved_variable_stack_fill_pointer);
        }
        //!!There could be more conflicts than just one
        //} while( clause != NO_CONFLICT );
      }
    }
  }
  //if ( satisfiable() ) printf("Empty set\n");
  reset_context(saved_clause_stack_fill_pointer,
                saved_reducedclause_stack_fill_pointer,
                saved_unitclause_stack_fill_pointer,
                saved_variable_stack_fill_pointer);
  return la;
}

int lookahead() {
  int saved_clause_stack_fill_pointer, saved_reducedclause_stack_fill_pointer,
    saved_unitclause_stack_fill_pointer, saved_variable_stack_fill_pointer,
    my_saved_clause_stack_fill_pointer,
    clause, conflict=0, var, *vars_signs, i, unitclause;

  // if (NB_BACK==160)
  //  printf("sqhvdzhj");

  CLAUSES_TO_REMOVE_fill_pointer=0;
  saved_clause_stack_fill_pointer= CLAUSE_STACK_fill_pointer;
  saved_reducedclause_stack_fill_pointer = REDUCEDCLAUSE_STACK_fill_pointer;
  saved_unitclause_stack_fill_pointer = UNITCLAUSE_STACK_fill_pointer;
  saved_variable_stack_fill_pointer=VARIABLE_STACK_fill_pointer;
  my_saved_clause_stack_fill_pointer= CLAUSE_STACK_fill_pointer;
  while ((clause=my_unitclause_process(0))!=NO_CONFLICT) {
    conflict++;
    if (conflict+NB_EMPTY>=UB) break;
    
    if (linear_conflict(clause)==TRUE) {
      conflict--; NB_EMPTY++;
      reset_context(my_saved_clause_stack_fill_pointer, 
		    saved_reducedclause_stack_fill_pointer,
		    saved_unitclause_stack_fill_pointer,
		    saved_variable_stack_fill_pointer);
      remove_linear_reasons();
      my_saved_clause_stack_fill_pointer=CLAUSE_STACK_fill_pointer;
    }
    else 
    {
      push(clause, REASON_STACK);
      for(i=0; i<REASON_STACK_fill_pointer; i++) {
	clause=REASON_STACK[i]; vars_signs = var_sign[clause];
	for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
	  if (reason[var]!=NO_REASON) {
	    push(reason[var], REASON_STACK);
	    reason[var]=NO_REASON;
	  }
	}
      }
      reset_context(my_saved_clause_stack_fill_pointer, 
		    saved_reducedclause_stack_fill_pointer,
		    saved_unitclause_stack_fill_pointer,
		    saved_variable_stack_fill_pointer);
      for(i=0; i<REASON_STACK_fill_pointer; i++) {
	clause=REASON_STACK[i];
	clause_state[clause]=PASSIVE; push(clause, CLAUSE_STACK);
      }
      REASON_STACK_fill_pointer=0;
      my_saved_clause_stack_fill_pointer=CLAUSE_STACK_fill_pointer;
    }
  }
  if ( conflict+NB_EMPTY < UB ) {
    reset_context(my_saved_clause_stack_fill_pointer, 
		  saved_reducedclause_stack_fill_pointer,
		  saved_unitclause_stack_fill_pointer,
		  saved_variable_stack_fill_pointer); 
    conflict += failed_literal( conflict );
  }
    
  reset_context(saved_clause_stack_fill_pointer, 
		saved_reducedclause_stack_fill_pointer,
		saved_unitclause_stack_fill_pointer,
		saved_variable_stack_fill_pointer);
  if (conflict+NB_EMPTY>=UB) 
    return NONE;
  for (i=0; i<CLAUSES_TO_REMOVE_fill_pointer; i++) {
    clause=CLAUSES_TO_REMOVE[i];
    push(clause, CLAUSE_STACK); clause_state[clause]=PASSIVE;
  }
  CLAUSES_TO_REMOVE_fill_pointer=0;
  return conflict;
}

int satisfy_unitclause(int unitclause) {
  int *vars_signs, var, clause;

  vars_signs = var_sign[unitclause];
  for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
    if (var_state[var] == ACTIVE ){
      var_current_value[var] = *(vars_signs+1);
      var_rest_value[var] = NONE;
      reason[var]=unitclause;
      var_state[var] = PASSIVE;
      push(var, VARIABLE_STACK);
      if ((clause=my_reduce_clauses(var))==NO_CONFLICT) {
	remove_clauses(var);
	return NO_CONFLICT;
      }
      else 
	return clause;
    }
  }
  return NO_CONFLICT;
}
  
int my_unitclause_process(int starting_point) {
  int unitclause, var, *vars_signs, unitclause_position,clause,
    my_unitclause_position, my_unitclause;

  for (unitclause_position = starting_point; 
       unitclause_position < UNITCLAUSE_STACK_fill_pointer;
       unitclause_position++) {
    unitclause = UNITCLAUSE_STACK[unitclause_position];
    if ((clause_state[unitclause] == ACTIVE)  && (clause_length[unitclause]>0)) {
      MY_UNITCLAUSE_STACK_fill_pointer=0;
      if ((clause=satisfy_unitclause(unitclause)) != NO_CONFLICT)
	return clause;
      else {
	for (my_unitclause_position = 0; 
	     my_unitclause_position < MY_UNITCLAUSE_STACK_fill_pointer;
	     my_unitclause_position++) {
	  my_unitclause = MY_UNITCLAUSE_STACK[my_unitclause_position];
	  if ((clause_state[my_unitclause] == ACTIVE)  
	      && (clause_length[my_unitclause]>0)) {
	    if ((clause=satisfy_unitclause(my_unitclause)) != NO_CONFLICT)
	      return clause;
	  }     
	}
      }
    }
  }
  return NO_CONFLICT;
}

int get_complement(int lit) {
  if (positive(lit)) return lit+NB_VAR;
  else return lit-NB_VAR;
}

void create_unitclause(int lit, int subsumedclause, int *clauses) {
  int clause, *vars_signs, flag=FALSE;

  vars_signs=NEW_CLAUSES[NEW_CLAUSES_fill_pointer++];
  if (lit<NB_VAR) {
    vars_signs[0]=lit;
    vars_signs[1]=POSITIVE;
  }
  else {
    vars_signs[0]=lit-NB_VAR;
    vars_signs[1]=NEGATIVE;
  }
  vars_signs[2]=NONE;
  var_sign[NB_CLAUSE]=vars_signs;
  clause_state[NB_CLAUSE]=ACTIVE;
  clause_length[NB_CLAUSE]=1;
  push(NB_CLAUSE, UNITCLAUSE_STACK);

  for(clause=*clauses; clause!=NONE; clause=*(++clauses)) {
    if (clause==subsumedclause) {
      *clauses=NB_CLAUSE;
      SAVED_CLAUSE_POSITIONS[SAVED_CLAUSES_fill_pointer]=clauses;
      push(subsumedclause, SAVED_CLAUSES);
      flag=TRUE;
      break;
    }
  }
  if (flag==FALSE)
    printf("erreur "), exit(0);
  NB_CLAUSE++;
}

int verify_resolvent(int lit, int clause1, int clause2) {
  int *vars_signs1, *vars_signs2, lit1, lit2, temp, flag=FALSE, var, nb=0;

  if ((clause_state[clause1]!=ACTIVE) || (clause_state[clause2]!=ACTIVE))
      printf("erreur "), exit(0);
  if ((clause_length[clause1]!=2) || (clause_length[clause2]!=2))
    printf("erreur "), exit(0);
  vars_signs1=var_sign[clause1];
  vars_signs2=var_sign[clause2];
  for(var=*vars_signs1; var!=NONE; var=*(vars_signs1+=2)) {
    if (var_state[var] == ACTIVE ) {
      nb++;
      if (*(vars_signs1+1)==POSITIVE) 
	temp=var;
      else temp=var+NB_VAR;
      if (temp==lit) flag=TRUE;
      else {
	lit1=temp;
      }
    }
  }
  if ((nb!=2) || (flag==FALSE))
    printf("erreur "), exit(0);
  nb=0; flag=FALSE;
  for(var=*vars_signs2; var!=NONE; var=*(vars_signs2+=2)) {
    if (var_state[var] == ACTIVE ) {
      nb++;
      if (*(vars_signs2+1)==POSITIVE) 
	temp=var;
      else temp=var+NB_VAR;
      if (temp==lit) flag=TRUE;
      else {
	lit2=temp;
      }
    }
  }
  if ((nb!=2) || (flag==FALSE))
    printf("erreur "), exit(0);
  if (!complement(lit1, lit2))
    printf("erreur "), exit(0);
}

int searching_two_clauses_to_fix_neglit(int clause, int lit) {
  int lit1, clause1, var1, opp_lit1;
  if (lit_to_fix[clause]==NONE) {
    lit_to_fix[clause]=lit;
  }
  else {
    lit1=lit_to_fix[clause];
    var1=get_var_from_lit(lit1);
    //  if (var_state[var1]!=ACTIVE)
    //    printf("erreur2  ");
    opp_lit1=get_complement(lit1);
    clause1=fixing_clause[opp_lit1];
    if ((clause1!= NONE) && (clause_state[clause1]==ACTIVE)) {
      fixing_clause[opp_lit1]=NONE;
      lit_involved_in_clause[opp_lit1]=NONE;
      // verify_resolvent(lit, clause1, clause);
      push(clause1, CLAUSE_STACK);
      clause_state[clause1]=PASSIVE;
      push(clause, CLAUSE_STACK);
      clause_state[clause]=PASSIVE;
      create_unitclause(lit, clause1, neg_in[lit-NB_VAR]);
      var1=get_var_from_lit(lit1);
      nb_neg_clause_of_length2[var1]--;
      nb_pos_clause_of_length2[var1]--;
      return TRUE;
    }
    else {
      fixing_clause[lit1]=clause;
      push(lit1, CANDIDATE_LITERALS);
      lit_involved_in_clause[lit1]=clause;
      push(lit1, INVOLVED_LIT_STACK);
    }
  }
  return FALSE;
}

int simple_get_neg_clause_nb(int var) {
  my_type neg_clause1_nb=0,neg_clause3_nb = 0, neg_clause2_nb = 0;
  int *clauses, clause, i;
  clauses = neg_in[var]; MY_UNITCLAUSE_STACK_fill_pointer=0;

  for(clause=*clauses; clause!=NONE; clause=*(++clauses))
    if ((clause_state[clause] == ACTIVE) && (clause_length[clause]==2))
      neg_clause2_nb++;
    nb_neg_clause_of_length2[var] = neg_clause2_nb;
    return neg_clause2_nb;
}

int simple_get_pos_clause_nb(int var) {
  my_type pos_clause1_nb=0,pos_clause3_nb = 0, pos_clause2_nb = 0;
  int *clauses, clause, i;
  clauses = pos_in[var]; MY_UNITCLAUSE_STACK_fill_pointer=0;

  for(clause=*clauses; clause!=NONE; clause=*(++clauses))
    if ((clause_state[clause] == ACTIVE) && (clause_length[clause]==2))
      pos_clause2_nb++;
    nb_pos_clause_of_length2[var] = pos_clause2_nb;
    return pos_clause2_nb;
}

int get_neg_clause_nb(int var) {
  my_type neg_clause1_nb=0,neg_clause3_nb = 0, neg_clause2_nb = 0;
  int *clauses, clause, i;
  clauses = neg_in[var]; MY_UNITCLAUSE_STACK_fill_pointer=0;

  for(clause=*clauses; clause!=NONE; clause=*(++clauses)) {
    if ((clause_state[clause] == ACTIVE) && (clause_length[clause]>0)) {
      switch(clause_length[clause]) {
      case 1: neg_clause1_nb++; 
	push(clause, MY_UNITCLAUSE_STACK); break;
      case 2: neg_clause2_nb++; 
	if (searching_two_clauses_to_fix_neglit(clause, var+NB_VAR)==TRUE) {
	  neg_clause2_nb-=2; neg_clause1_nb++; 
	}
	break;
      default: neg_clause3_nb++; break;
      }
    }
  }
  for(i=0; i<CANDIDATE_LITERALS_fill_pointer; i++) 
    fixing_clause[CANDIDATE_LITERALS[i]]=NONE;
  CANDIDATE_LITERALS_fill_pointer=0;
  nb_neg_clause_of_length1[var] = neg_clause1_nb;
  nb_neg_clause_of_length2[var] = neg_clause2_nb;
  nb_neg_clause_of_length3[var] = neg_clause3_nb;
  return neg_clause1_nb+neg_clause2_nb + neg_clause3_nb;
}

#define OTHER_LIT_FIXED 1
#define THIS_LIT_FIXED 2

int searching_two_clauses_to_fix_poslit(int clause, int lit) {
  int lit1, clause1, var1, opp_lit1;
  if (lit_to_fix[clause]==NONE) {
    lit_to_fix[clause]=lit;
  }
  else {
    lit1=lit_to_fix[clause];
    var1=get_var_from_lit(lit1);
    //   if (var_state[var1]!=ACTIVE)
    //   printf("erreur2  ");
    clause1=lit_involved_in_clause[lit1];
    if ((clause1!=NONE) && (clause_state[clause1]==ACTIVE)) {
      //  verify_resolvent(lit1, clause1, clause);
      push(clause1, CLAUSE_STACK);
      clause_state[clause1]=PASSIVE;
      push(clause, CLAUSE_STACK);
      clause_state[clause]=PASSIVE;
      if (lit1<NB_VAR) {
	create_unitclause(lit1, clause1, pos_in[lit1]);
	nb_pos_clause_of_length2[lit1]-=2;
	nb_pos_clause_of_length1[lit1]++;
      }
      else {
	create_unitclause(lit1, clause1, neg_in[lit1-NB_VAR]);
	nb_neg_clause_of_length2[lit1-NB_VAR]-=2;
	nb_neg_clause_of_length1[lit1-NB_VAR]++;
      }
      return OTHER_LIT_FIXED;
    }
    else {
      opp_lit1=get_complement(lit1);
      clause1=fixing_clause[opp_lit1];
      if ((clause1!= NONE) && (clause_state[clause1]==ACTIVE)) {
	fixing_clause[opp_lit1]=NONE;
	//	verify_resolvent(lit, clause1, clause);
	push(clause1, CLAUSE_STACK);
	clause_state[clause1]=PASSIVE;
	push(clause, CLAUSE_STACK);
	clause_state[clause]=PASSIVE;
	create_unitclause(lit, clause1, pos_in[lit]);
	var1=get_var_from_lit(lit1);
	nb_neg_clause_of_length2[var1]--;
	nb_pos_clause_of_length2[var1]--;
	return THIS_LIT_FIXED;
      }
      else {
	fixing_clause[lit1]=clause;
	push(lit1, CANDIDATE_LITERALS);
      }
    }
  }
  return FALSE;
}

int get_pos_clause_nb(int var) {
  my_type pos_clause1_nb=0, pos_clause3_nb = 0, pos_clause2_nb = 0;
  int *clauses, clause, clause1, i;
  clauses = pos_in[var];
  for(clause=*clauses; clause!=NONE; clause=*(++clauses)) {
    if ((clause_state[clause] == ACTIVE) && (clause_length[clause]>0)) {
      switch(clause_length[clause]) {
      case 1:
	if (MY_UNITCLAUSE_STACK_fill_pointer>0) {
	  clause1=pop(MY_UNITCLAUSE_STACK);
	  clause_state[clause]=PASSIVE;
	  push(clause, CLAUSE_STACK);
	  clause_state[clause1]=PASSIVE;
	  push(clause1, CLAUSE_STACK);
	  nb_neg_clause_of_length1[var]--;
	  NB_EMPTY++;
	}
	else pos_clause1_nb++; 
	break;
      case 2: pos_clause2_nb++; 
	switch(searching_two_clauses_to_fix_poslit(clause, var)) {
	case OTHER_LIT_FIXED: nb_neg_clause_of_length2[var]--;
	  pos_clause2_nb--;
	  break;
	case THIS_LIT_FIXED: pos_clause2_nb-=2;
	  pos_clause1_nb++;
	  break;
	}
	break;
      default: pos_clause3_nb++; break;
      }
    }
  }
  for(i=0; i<CANDIDATE_LITERALS_fill_pointer; i++) 
    fixing_clause[CANDIDATE_LITERALS[i]]=NONE;
  CANDIDATE_LITERALS_fill_pointer=0;
  for(i=0; i<INVOLVED_LIT_STACK_fill_pointer; i++) 
    lit_involved_in_clause[INVOLVED_LIT_STACK[i]]=NONE;
  INVOLVED_LIT_STACK_fill_pointer=0;
  nb_pos_clause_of_length1[var] = pos_clause1_nb;
  nb_pos_clause_of_length2[var] = pos_clause2_nb;
  nb_pos_clause_of_length3[var] = pos_clause3_nb;
  return pos_clause1_nb+pos_clause2_nb + pos_clause3_nb;
}

int satisfy_literal(int lit) {
  int var;
  if (positive(lit)) {
    if (var_state[lit]==ACTIVE) {
      var_current_value[lit] = TRUE;
      if (reduce_clauses(lit)==FALSE) return NONE;
      var_rest_value[lit]=NONE;
      var_state[lit] = PASSIVE;
      push(lit, VARIABLE_STACK);
      remove_clauses(lit);
    }
    else
      if (var_current_value[lit]==FALSE) return NONE;
  } 
  else {
    var = get_var_from_lit(lit);
    if (var_state[var]==ACTIVE) {
      var_current_value[var] = FALSE;
      if (reduce_clauses(var)==FALSE) return NONE;
      var_rest_value[var]=NONE;
      var_state[var] = PASSIVE;
      push(var, VARIABLE_STACK);
      remove_clauses(var);
    }
    else
      if (var_current_value[var]==TRUE) return NONE;
  }
  return TRUE;
}

int assign_value(int var, int current_value, int rest_value) {
  if (var_state[var]==PASSIVE)
    printf("erreur1...\n"), exit(0);
  var_state[var] = PASSIVE;
  push(var, VARIABLE_STACK);
  var_current_value[var] = current_value;
  var_rest_value[var] = rest_value;
  if (reduce_clauses(var)==NONE) 
    return NONE;
  remove_clauses(var);
  return TRUE;
}

int unitclause_process() {
  int unitclause, var, *vars_signs, unitclause_position,clause;
  
  for (unitclause_position = 0; 
       unitclause_position < UNITCLAUSE_STACK_fill_pointer;
       unitclause_position++) {
    unitclause = UNITCLAUSE_STACK[unitclause_position];
    if ((clause_state[unitclause] == ACTIVE)  && (clause_length[unitclause]>0)) {
      vars_signs = var_sign[unitclause];
      for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
	if (var_state[var] == ACTIVE ){
	  var_current_value[var] = *(vars_signs+1);
	  var_rest_value[var] = NONE;
	  var_state[var] = PASSIVE;
	  push(var, VARIABLE_STACK);
	  if ((clause=reduce_clauses(var)) !=NONE) {
	    remove_clauses(var);
	    break;
	  }
	  else {
	    return NONE;
	  }
	}
      }
    }     
  }
  return TRUE;
}

int choose_and_instantiate_variable() {
  int var, nb=0, chosen_var=NONE,cont=0, cont1; 
  int  i;
  float posi, nega;
  int a,b,c,clause;
  float poid, max_poid = -1.0; 
  my_type pos2, neg2, flag=0;
  NB_BRANCHE++;

  // if (NB_BRANCHE==173)
  //   printf("zerza ");

  if (lookahead()==NONE)
    return NONE;

  if (UB-NB_EMPTY==1)
    if (unitclause_process() ==NONE)
      return NONE;

  for (clause=0; clause<NB_CLAUSE; clause++) 
    lit_to_fix[clause]=NONE;

  for (var = 0; var < NB_VAR; var++) {
    if (var_state[var] == ACTIVE) {
      reduce_if_negative[var]=0;
      reduce_if_positive[var]=0;
      if (get_neg_clause_nb(var) == 0) {
	NB_MONO++;
	var_current_value[var] = TRUE;
	var_rest_value[var] = NONE;
	var_state[var] = PASSIVE;
	push(var, VARIABLE_STACK);
	remove_clauses(var);
      }
      else if (get_pos_clause_nb(var) == 0) {
	NB_MONO++;
	var_current_value[var] = FALSE;
	var_rest_value[var] = NONE;
	var_state[var] = PASSIVE;
	push(var, VARIABLE_STACK);
	remove_clauses(var);
      }
      else if (nb_neg_clause_of_length1[var]+NB_EMPTY>=UB) {
	flag++;
	if (assign_value(var, FALSE, NONE)==NONE)
	  return NONE;
      }
      else if (nb_pos_clause_of_length1[var]+NB_EMPTY>=UB) {
	flag++;
	if (assign_value(var, TRUE, NONE)==NONE)
	  return NONE;
      }
      else if (nb_neg_clause_of_length1[var]>=
	       nb_pos_clause_of_length1[var]+
	       nb_pos_clause_of_length2[var]+ 
	       nb_pos_clause_of_length3[var]) {
	flag++;
	if (assign_value(var, FALSE, NONE)==NONE)
	  return NONE;
      }
      else if (nb_pos_clause_of_length1[var]>=
	       nb_neg_clause_of_length1[var]+
	       nb_neg_clause_of_length2[var]+ 
	       nb_neg_clause_of_length3[var]) {
	flag++;
	if (assign_value(var, TRUE, NONE)==NONE)
	  return NONE;
      }
      else {
	if (nb_neg_clause_of_length1[var]>nb_pos_clause_of_length1[var]) {
	  cont+=nb_pos_clause_of_length1[var];
	}
	else {
	  cont+=nb_neg_clause_of_length1[var];
	}
      }
    }
  }

  if (cont+NB_EMPTY>=UB)
    return NONE;
  for (var = 0; var < NB_VAR; var++) {
    if (var_state[var] == ACTIVE) {
      /*   
      if (nb_neg_clause_of_length1[var]>nb_pos_clause_of_length1[var])
	cont1=cont-nb_pos_clause_of_length1[var];
      else cont1=cont-nb_neg_clause_of_length1[var];
      if (nb_neg_clause_of_length1[var]+cont1+NB_EMPTY>=UB) {
	if (assign_value(var, FALSE, NONE)==NONE)
	  return NONE;
      }
      else if (nb_pos_clause_of_length1[var]+cont1+NB_EMPTY>=UB) {
	if (assign_value(var, TRUE, NONE)==NONE)
	  return NONE;
      }
      else {
      */
	reduce_if_positive[var]=nb_neg_clause_of_length1[var]*2+
	  nb_neg_clause_of_length2[var]*4+ 
	  nb_neg_clause_of_length3[var];
	reduce_if_negative[var]=nb_pos_clause_of_length1[var]*2+
	  nb_pos_clause_of_length2[var]*4+ 
	  nb_pos_clause_of_length3[var];
	poid=reduce_if_positive[var]*reduce_if_negative[var]*64+
	  reduce_if_positive[var]+reduce_if_negative[var];
	if (poid>max_poid) {
	  chosen_var=var;
	  max_poid=poid;
	}
	//	 }
    }
  }
  if (chosen_var == NONE) return FALSE;
  //      printf("%d \n",NB_BACK);
  //   printf("Chosen_va %d\n",chosen_var);
  saved_clause_stack[chosen_var] = CLAUSE_STACK_fill_pointer;
  saved_reducedclause_stack[chosen_var] = REDUCEDCLAUSE_STACK_fill_pointer;
  saved_unitclause_stack[chosen_var] = UNITCLAUSE_STACK_fill_pointer;
  saved_nb_empty[chosen_var]=NB_EMPTY;
  // return assign_value(chosen_var, TRUE, FALSE);
  saved_nb_clause[chosen_var]=NB_CLAUSE;
  saved_saved_clauses[chosen_var]=SAVED_CLAUSES_fill_pointer;
  saved_new_clauses[chosen_var]=NEW_CLAUSES_fill_pointer;
  if (reduce_if_positive[chosen_var]<reduce_if_negative[chosen_var])
    return assign_value(chosen_var, TRUE, FALSE);
  else return assign_value(chosen_var, FALSE, TRUE);

}

int dpl() {
  int var, nb;
  do {
    if (VARIABLE_STACK_fill_pointer==NB_VAR) {
      UB=NB_EMPTY; 
      printf("o %d\n", UB );
      nb=verify_solution();
      if (nb!=NB_EMPTY)
      	printf("problem nb..."), exit(0);
      while (backtracking()==NONE);
      if (VARIABLE_STACK_fill_pointer==0)
	break;
    }
    if (UB-NB_EMPTY==1)
      if (unitclause_process() ==NONE)
	while (backtracking()==NONE);
    if (choose_and_instantiate_variable()==NONE)
      while (backtracking()==NONE);
    // else if (lookahead()==NONE) 
    //  while (backtracking()==NONE);
  } while (VARIABLE_STACK_fill_pointer > 0);
}

void init() {
  int var, clause;
  NB_EMPTY=0; REAL_NB_CLAUSE=NB_CLAUSE;
  UNITCLAUSE_STACK_fill_pointer=0;
  VARIABLE_STACK_fill_pointer=0;
  CLAUSE_STACK_fill_pointer = 0;
  REDUCEDCLAUSE_STACK_fill_pointer = 0;
  for (var=0; var<NB_VAR; var++) {
    reason[var]=NO_REASON;
    fixing_clause[var]=NONE;
    fixing_clause[var+NB_VAR]=NONE;
    lit_involved_in_clause[var]=NONE;
    lit_involved_in_clause[var+NB_VAR]=NONE;
  }
  for (clause=0; clause<NB_CLAUSE; clause++) {
    lit_to_fix[clause]=NONE;
    clause_involved[clause]=NONE;
  }
}
 
char* emptyFileName() {
  char *vlineFilename = malloc( sizeof(char) * 512 );  
  sprintf(vlineFilename, "%s", P_tmpdir);
  strcat(vlineFilename, "/XXXXXX");
  vlineFilename = mktemp(vlineFilename);
  return vlineFilename;
}

int localSearch( char* filename, int iTimeOut, int iNumRuns, int iSeed ){
  char sTimeout[64];
  char sRuns[64];
  char* vlineFilename = emptyFileName();
  char strseed[64];
  char* argv[] = {"ubcsat", 
		  "-alg", NULL, "-noimprove", NULL,  "-i", filename, 
		  "-runs", sRuns, "-timeout", sTimeout,
		  "-r", "stats", vlineFilename, "best",
		  //"-seed", strseed,
		  //"-satzilla",
		  NULL
  };
  FILE *ft;
  float best_solution;
  char line[ 1000 ];

  sprintf(sTimeout, "%d", iTimeOut );
  sprintf(sRuns, "%d", iNumRuns );
  sprintf(strseed, "%d", iSeed );
  
  // -- do saps
  argv[2]="saps";
  argv[4]="0.1";

  if ( ubcsat_main( 14, argv) == 10 ) printf("Instance satisfiable\n");
 
  ft = fopen(vlineFilename, "r");
   while( fgets( line, 1000, ft ) != NULL ) {     
     sscanf( line, "BestSolution_Mean = %f\n", &best_solution );
   }
  printf("o %d \n", (int)best_solution );
  fflush(stdout);    
  fclose(ft);
  return (int)best_solution;
}

main(int argc, char *argv[]) {
  char saved_input_file[WORD_LENGTH];
  int i,  var; 
  long begintime, endtime, mess;
  struct tms *a_tms;
  FILE *fp_time;

  if (argc<2) {
    printf("Using format: maxsatz input_instance [upper_bound]\n"), exit(0);
    return FALSE;
  }
  for (i=0; i<WORD_LENGTH; i++) saved_input_file[i]=argv[1][i];

  a_tms = ( struct tms *) malloc( sizeof (struct tms));
  mess=times(a_tms); begintime = a_tms->tms_utime;

  switch (build_simple_sat_instance(argv[1])) {
  case FALSE: printf("Input file error\n"), exit(0); return FALSE;
  case TRUE:
    if (argc>2) UB=atoi(argv[2]); else UB=localSearch(argv[1], 5, 100, 12345 );
    init();
    dpl();
    break;
  case NONE: printf("An empty resolvant is found!\n"), exit(0); break;
  }
  mess=times(a_tms); endtime = a_tms->tms_utime;

  printf("c Best Solution=%d\n", UB);
  printf("c NB_MONO= %ld, NB_UNIT= %ld, NB_BRANCHE= %ld, NB_BACK= %ld \n", 
	 NB_MONO, NB_UNIT, NB_BRANCHE, NB_BACK);
	        
  printf ("c Program terminated in %5.3f seconds.\n",
	  ((double)(endtime-begintime)/CLK_TCK));

  printf("o %d\n", UB );
  printf("s OPTIMUM FOUND\nv ");
  for (i=0; i<NB_VAR; i++) {
    if( solution[ i ] == 1 ) printf("%d ", i+1 );
    else printf("%d ", 0-i-1 );
  }
  printf("\n");


  /*
  fp_time = fopen("timetable", "a");
  fprintf(fp_time, "maxsatz14bis+fl %s %5.3f %ld %ld %d %d %d %d\n", 
	  saved_input_file, ((double)(endtime-begintime)/CLK_TCK), 
	  NB_BRANCHE, NB_BACK,  
	  UB, NB_VAR, INIT_NB_CLAUSE, NB_CLAUSE-INIT_NB_CLAUSE);
  printf("maxsatz14bis+fl %s %5.3f %ld %ld %d %d %d %d\n", 
	 saved_input_file, ((double)(endtime-begintime)/CLK_TCK), 
	 NB_BRANCHE, NB_BACK,
	 UB, NB_VAR, INIT_NB_CLAUSE, NB_CLAUSE-INIT_NB_CLAUSE);
  fclose(fp_time);
  */
  exit (10);
}
