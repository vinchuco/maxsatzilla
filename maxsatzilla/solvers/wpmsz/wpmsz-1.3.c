#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <sys/times.h>
#include <sys/types.h>
#include <limits.h>

#include <unistd.h>

typedef signed char my_type;
typedef unsigned char my_unsigned_type;

typedef long long int lli_type;

// #define DEBUG

#define WORD_LENGTH 1024
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
#define tab_variable_size  30000
#define tab_clause_size 1000000
#define tab_unitclause_size ((tab_clause_size/4<2000) ? 2000 : tab_clause_size/4)
#define my_tab_variable_size ((tab_variable_size/2<1000) ? 1000 : tab_variable_size/2)
#define my_tab_clause_size ((tab_clause_size/2<2000) ? 2000 : tab_clause_size/2)
#define my_tab_unitclause_size ((tab_unitclause_size/2<1000) ? 1000 : tab_unitclause_size/2)
#define tab_literal_size 2*tab_variable_size
#define double_tab_clause_size 2*tab_clause_size
#define positive(literal) literal<NB_VAR
#define negative(literal) literal>=NB_VAR
#define get_var_from_lit(literal) \
  ((literal<NB_VAR) ? literal : literal-NB_VAR)
#define complement(lit1, lit2) \
 ((lit1<lit2) ? lit2-lit1 == NB_VAR : lit1-lit2 == NB_VAR)
#define get_lit(v, s) ((s == POSITIVE) ? v : v + NB_VAR)

#define tab_clause_size_XL (tab_clause_size * 10)

#define inverse_signe(signe) \
 (signe == POSITIVE) ? NEGATIVE : POSITIVE
#define unsat(val) (val==0)?"UNS":"SAT"
#define pop(stack) stack[--stack ## _fill_pointer]
#define push(item, stack) stack[stack ## _fill_pointer++] = item
#define top(stack) stack[stack ## _fill_pointer - 1]
//#define satisfiable() CLAUSE_STACK_fill_pointer == NB_CLAUSE
#define min(a, b)  (((a) < (b)) ? (a) : (b))

#define NEGATIVE 0
#define POSITIVE 1
#define PASSIVE 0
#define ACTIVE 1

int first_neg_in[tab_variable_size]; // Where the first negative literal is for [var]
int first_pos_in[tab_variable_size]; // Where the first positive literal is for [var]
int last_neg_in[tab_variable_size]; // Where the last negative literal is for [var]
int last_pos_in[tab_variable_size]; // Where the last positive literal is for [var]
int LIT_IN_STACK[tab_clause_size_XL]; // Where the literals are for all the variables (double list [pre,#c,nex])
int LIT_IN_STACK_fill_pointer;
int BASE_LIT_IN_STACK = tab_clause_size_XL / 2;
my_type var_current_value[tab_variable_size]; // Current assignment of variables
my_type var_rest_value[tab_variable_size]; // Restore vaule of variables
my_type var_state[tab_variable_size]; // Variable status

int saved_lit_in_stack[tab_variable_size];
int saved_clause_stack[tab_variable_size];
int saved_reducedclause_stack[tab_variable_size];
int saved_unitclause_stack[tab_variable_size];
lli_type saved_nb_empty[tab_variable_size];
lli_type nb_neg_clause_of_length1[tab_variable_size];
lli_type nb_pos_clause_of_length1[tab_variable_size];
lli_type nb_neg_clause_of_length2[tab_variable_size];
lli_type nb_neg_clause_of_length3[tab_variable_size];
lli_type nb_pos_clause_of_length2[tab_variable_size];
lli_type nb_pos_clause_of_length3[tab_variable_size];

float reduce_if_negative[tab_variable_size];
float reduce_if_positive[tab_variable_size];

int *sat[tab_clause_size]; // Clauses [clause][literal]
int *var_sign[tab_clause_size]; // Clauses [clause][var,sign]
lli_type clause_weight[tab_clause_size]; // Clause weights
lli_type ini_clause_weight[tab_clause_size]; // Initial clause weights
my_type clause_state[tab_clause_size]; // Clause status
int clause_length[tab_clause_size]; // Clause length

int VARIABLE_STACK_fill_pointer = 0;
int CLAUSE_STACK_fill_pointer = 0;
int UNITCLAUSE_STACK_fill_pointer = 0;
int REDUCEDCLAUSE_STACK_fill_pointer = 0;


int VARIABLE_STACK[tab_variable_size];
int CLAUSE_STACK[tab_clause_size];
int UNITCLAUSE_STACK[tab_unitclause_size];
int REDUCEDCLAUSE_STACK[tab_clause_size];

int PREVIOUS_REDUCEDCLAUSE_STACK_fill_pointer = 0;

lli_type HARD_WEIGHT = 0;
int NB_VAR;
int NB_CLAUSE;
int INIT_NB_CLAUSE;
int REAL_NB_CLAUSE;
#define INIT_BASE_NB_CLAUSE (tab_clause_size / 2)
int BASE_NB_CLAUSE = INIT_BASE_NB_CLAUSE;

lli_type NB_MONO=0, NB_BRANCHE=0, NB_BACK = 0;
lli_type NB_EMPTY=0, UB;
int instance_type;
int partial;

#define NO_CONFLICT -3
#define NO_REASON -3
int reason[tab_variable_size];
int REASON_STACK[tab_variable_size];
int REASON_STACK_fill_pointer=0;

int MY_UNITCLAUSE_STACK[tab_unitclause_size];
int MY_UNITCLAUSE_STACK_fill_pointer=0;
int CANDIDATE_LITERALS[2*tab_variable_size];
int CANDIDATE_LITERALS_fill_pointer=0;
int NEW_CLAUSES[tab_clause_size][7];
int NEW_CLAUSES_fill_pointer=0;
int lit_to_fix[tab_clause_size];
int SAVED_CLAUSE_POSITIONS[tab_clause_size];
int SAVED_CLAUSES[tab_clause_size];
int SAVED_CLAUSES_fill_pointer=0;
int lit_involved_in_clause[2*tab_variable_size];
int INVOLVED_LIT_STACK[2*tab_variable_size];
int INVOLVED_LIT_STACK_fill_pointer=0;
int fixing_clause[2*tab_variable_size];
int saved_nb_clause[tab_variable_size];
int saved_saved_clauses[tab_variable_size];
int saved_new_clauses[tab_variable_size];

int CLAUSES_TO_REMOVE[tab_clause_size];
int CLAUSES_TO_REMOVE_fill_pointer=0;
lli_type WEIGHTS_TO_REMOVE[tab_clause_size];
int WEIGHTS_TO_REMOVE_fill_pointer=0;
lli_type CLAUSES_WEIGHTS_TO_REMOVE[tab_clause_size];
int CLAUSES_WEIGHTS_TO_REMOVE_fill_pointer=0;

my_type var_best_value[tab_variable_size]; // Best assignment of variables
int SAVED_WEIGHTS_CLAUSE[tab_clause_size];
int SAVED_WEIGHTS_CLAUSE_fill_pointer = 0;
lli_type SAVED_WEIGHTS_WEIGHT[tab_clause_size];
int SAVED_WEIGHTS_WEIGHT_fill_pointer = 0;
int saved_weights_nb[tab_variable_size];

int MARK_STACK[tab_variable_size * 2];
int MARK_STACK_fill_pointer = 0;
int mark[tab_variable_size * 2];

int IG_STACK[tab_unitclause_size];
int IG_STACK_fill_pointer;
int POST_UIP_LITS[tab_variable_size];
int POST_UIP_LITS_fill_pointer;
int NEW_CLAUSE_LITS[tab_variable_size];
int NEW_CLAUSE_LITS_fill_pointer;
int unit_of_var[tab_variable_size];
#define max_var_learned (tab_variable_size / 10)
int undo_learned[tab_variable_size][max_var_learned];
int nb_undo_learned[tab_variable_size];
#define MAX_LEN_LEARNED 20 // MAX_LEN_LEARNED = num_lits * 2 (best performance 20)

void add_new_lit_in(int *pos, int clause) {
	#ifdef DEBUG
		if (LIT_IN_STACK_fill_pointer > tab_clause_size_XL - 5) {
			printf("DEBUG: LIT_IN_STACK.\n");
			exit(0);
		}
	#endif
	LIT_IN_STACK[*pos] = clause;
	push(*pos, LIT_IN_STACK);
	LIT_IN_STACK[*pos + 1] = LIT_IN_STACK_fill_pointer;
	*pos = LIT_IN_STACK_fill_pointer;
	push(NONE, LIT_IN_STACK);
	push(NONE, LIT_IN_STACK);
}

void add_new_hlit_in(int *pos, int clause) {
	#ifdef DEBUG
		if (BASE_LIT_IN_STACK < 5) {
			printf("DEBUG: LIT_IN_STACK.\n");
			exit(0);
		}
	#endif
	BASE_LIT_IN_STACK--;
	LIT_IN_STACK[BASE_LIT_IN_STACK] = *pos;
	BASE_LIT_IN_STACK--;
	LIT_IN_STACK[*pos - 1] = BASE_LIT_IN_STACK;
	LIT_IN_STACK[BASE_LIT_IN_STACK] = clause;
	*pos = BASE_LIT_IN_STACK;
	BASE_LIT_IN_STACK--;
	LIT_IN_STACK[BASE_LIT_IN_STACK] = NONE;
}

int get_next_clause(int *pos) {
	*pos = LIT_IN_STACK[*pos + 1];
	return LIT_IN_STACK[*pos];
}

int get_prev_clause(int *pos) {
	*pos = LIT_IN_STACK[*pos - 1];
	return LIT_IN_STACK[*pos];
}

// #include "input.c" //

/* test if the new clause is redundant or subsompted by another */
#define OLD_CLAUSE_REDUNDANT -77
#define NEW_CLAUSE_REDUNDANT -7

int smaller_than(int lit1, int lit2) {
  return ((lit1<NB_VAR) ? lit1 : lit1-NB_VAR) < 
    ((lit2<NB_VAR) ? lit2 : lit2-NB_VAR);
}

int redundant(int *new_clause, int *old_clause) {
	int lit1, lit2, old_clause_diff=0, new_clause_diff=0;
	
	lit1=*old_clause; lit2=*new_clause;
	while ((lit1 != NONE) && (lit2 != NONE)) {
		if (smaller_than(lit1, lit2)) {
			lit1=*(++old_clause); old_clause_diff++;
		} else if (smaller_than(lit2, lit1)) {
			lit2=*(++new_clause); new_clause_diff++;
		} else if (complement(lit1, lit2)) {
			return FALSE; /* old_clause_diff++; new_clause_diff++; j1++; j2++; */
		} else {
			lit1=*(++old_clause);  lit2=*(++new_clause);
		}
	}
	if ((lit1 == NONE) && (old_clause_diff == 0))
	/* la nouvelle clause est redondante ou subsumee */
		return NEW_CLAUSE_REDUNDANT;
	if ((lit2 == NONE) && (new_clause_diff == 0))
	/* la old clause est redondante ou subsumee */
		return OLD_CLAUSE_REDUNDANT;
	return FALSE;
}

void remove_passive_clauses() {
	int  clause, put_in, first=NONE;
	for (clause = BASE_NB_CLAUSE; clause < NB_CLAUSE; clause++) {
		if (clause_state[clause]==PASSIVE) {
			first=clause; break;
		}
	}
	if (first!=NONE) {
		put_in=first;
		for(clause=first+1; clause<NB_CLAUSE; clause++) {
			if (clause_state[clause]==ACTIVE) {
				sat[put_in]=sat[clause]; var_sign[put_in]=var_sign[clause];
				clause_state[put_in]=ACTIVE;
				clause_length[put_in]=clause_length[clause];
				clause_weight[put_in]=clause_weight[clause];
				put_in++;
			}
		}
		NB_CLAUSE=put_in;
	}
}

void remove_passive_vars_in_clause(int clause) {
	int *vars_signs, *vars_signs1, var, var1, first=NONE;
	
	vars_signs=var_sign[clause];
	for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
		if (var_state[var]!=ACTIVE) {
			first=var; break;
		}
	}
	if (first!=NONE) {
		for(vars_signs1=vars_signs+2, var1=*vars_signs1; var1!=NONE; var1=*(vars_signs1+=2)) {
			if (var_state[var1]==ACTIVE) {
				*vars_signs=var1; *(vars_signs+1) = *(vars_signs1+1);
				vars_signs+=2;
			}
		}
		*vars_signs=NONE;
	}
}

int clean_structure() {
	int clause, var, *vars_signs;
	
	remove_passive_clauses();
	if (NB_CLAUSE == BASE_NB_CLAUSE)
		return FALSE;
	for (clause = BASE_NB_CLAUSE; clause < NB_CLAUSE; clause++)
		remove_passive_vars_in_clause(clause);
	LIT_IN_STACK_fill_pointer = BASE_LIT_IN_STACK;
	for (var = 0; var < NB_VAR; var++) {
		push(NONE, LIT_IN_STACK);
		first_neg_in[var] = LIT_IN_STACK_fill_pointer;
		last_neg_in[var] = LIT_IN_STACK_fill_pointer;
		push(NONE, LIT_IN_STACK);
		push(NONE, LIT_IN_STACK);
		push(NONE, LIT_IN_STACK);
		first_pos_in[var] = LIT_IN_STACK_fill_pointer;
		last_pos_in[var] = LIT_IN_STACK_fill_pointer;
		push(NONE, LIT_IN_STACK);
		push(NONE, LIT_IN_STACK);
	}
	for (clause = BASE_NB_CLAUSE; clause < NB_CLAUSE; clause++) {
		vars_signs=var_sign[clause];
		for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
			if (*(vars_signs+1)==POSITIVE)
				add_new_lit_in(&last_pos_in[var], clause);
			else
				add_new_lit_in(&last_neg_in[var], clause);
		}
	}
	return TRUE;
}

void lire_clauses(FILE *fp_in, int instance_type) {
	int i, j, jj, ii, length, tautologie, lits[10000], lit, lit1;
	lli_type weight;
	
	partial = 0;
	if (HARD_WEIGHT > 0) // For partial
		partial = 1;
	for (i = BASE_NB_CLAUSE; i < NB_CLAUSE; i++) {
		length=0;
		if (instance_type != 0)
			fscanf(fp_in, "%lli", &weight);
		else
			weight = 1;
		fscanf(fp_in, "%d", &lits[length]);
		while (lits[length] != 0) {
			length++;
			fscanf(fp_in, "%d", &lits[length]);
		}
		tautologie = FALSE;
		/* test if some literals are redundant and sort the clause */
		for (ii=0; ii<length-1; ii++) {
			lit = lits[ii];
			for (jj=ii+1; jj<length; jj++) {
				if (abs(lit)>abs(lits[jj])) { // swap
					lit1=lits[jj]; lits[jj]=lit; lit=lit1;
				} else if (lit == lits[jj]) { // x v x = x
					lits[jj] = lits[length-1]; 
					jj--; length--; lits[length] = 0;
					printf("literal %d is redundant in clause %d \n", lit, i+1);
				} else if (abs(lit) == abs(lits[jj])) { // x v -x = T
					tautologie = TRUE; break;
				}
			}
			if (tautologie == TRUE) break;
			else lits[ii] = lit;
		}
		if (tautologie == FALSE) {
			sat[i]= (int *)malloc((length+1) * sizeof(int));
			for (j=0; j<length; j++) {
				if (lits[j] < 0) 
					sat[i][j] = abs(lits[j]) - 1 + NB_VAR ;
				else 
					sat[i][j] = lits[j]-1;
			}
			sat[i][length]=NONE;
			clause_length[i]=length;
			clause_weight[i] = weight;
			if (partial == 0)
				HARD_WEIGHT += weight;
			clause_state[i] = ACTIVE;
		} else {
			i--;
			NB_CLAUSE--;
		}
	}
}

void build_structure() {
	int i, *lits1, length, clause, *vars_signs, lit;
	
	for(clause = BASE_NB_CLAUSE; clause < NB_CLAUSE; clause++) { // Build [clause][var, sign] structure
		length = clause_length[clause];
		var_sign[clause] = (int *)malloc((2*length+1)*sizeof(int));
		lits1 = sat[clause]; vars_signs = var_sign[clause];
		for(lit=*lits1; lit!=NONE; lit=*(++lits1),(vars_signs+=2)) {
			if (negative(lit)) {
				*(vars_signs+1)= NEGATIVE;
				*vars_signs = get_var_from_lit(lit);
			} else {
				*(vars_signs+1)=POSITIVE;
				*vars_signs = lit;
			}
		}
		*vars_signs = NONE;
	}
	LIT_IN_STACK_fill_pointer = BASE_LIT_IN_STACK;
	for (i=0; i<NB_VAR; i++) { 
		push(NONE, LIT_IN_STACK);
		first_neg_in[i] = LIT_IN_STACK_fill_pointer;
		last_neg_in[i] = LIT_IN_STACK_fill_pointer;
		push(NONE, LIT_IN_STACK);
		push(NONE, LIT_IN_STACK);
		push(NONE, LIT_IN_STACK);
		first_pos_in[i] = LIT_IN_STACK_fill_pointer;
		last_pos_in[i] = LIT_IN_STACK_fill_pointer;
		push(NONE, LIT_IN_STACK);
		push(NONE, LIT_IN_STACK);
		var_state[i] = ACTIVE;
	}
	for (i = BASE_NB_CLAUSE; i < NB_CLAUSE; i++) { // Build pos/neg_in structure
		lits1 = sat[i];
		for(lit=*lits1; lit!=NONE; lit=*(++lits1)) {
			if (positive(lit)) 
				add_new_lit_in(&last_pos_in[lit], clause);
			else
				add_new_lit_in(&last_neg_in[get_var_from_lit(lit)], clause);
		}
	}
}

void eliminate_redundance() {
	//int *lits, i, lit, *clauses, res, clause;
	int i;
	
	for (i = BASE_NB_CLAUSE; i < NB_CLAUSE; i++) {
		if (clause_state[i]==ACTIVE) {
			if (clause_length[i]==1)
				push(i, UNITCLAUSE_STACK);
/*
			lits = sat[i];
			for(lit=*lits; lit!=NONE; lit=*(++lits)) {
				if (positive(lit))
					clauses=pos_in[lit];
				else clauses=neg_in[lit-NB_VAR];
					for(clause=*clauses; clause!=NONE; clause=*(++clauses)) {
						if ((clause<i) && (clause_state[clause]==ACTIVE)) {
							res=redundant(sat[i], sat[clause]);
							if (res==NEW_CLAUSE_REDUNDANT) {
								clause_state[i]=PASSIVE;
								break;
							} else if (res==OLD_CLAUSE_REDUNDANT)
								clause_state[clause]=PASSIVE;
						}
					}
				if (res==NEW_CLAUSE_REDUNDANT)
					break;
			}
*/
		}
	}
}

int build_simple_sat_instance(char *input_file) {
	FILE* fp_in=fopen(input_file, "r");
	char ch, word2[WORD_LENGTH];
	int i;
	char pLine[WORD_LENGTH];
	//int i, j, length, NB_CLAUSE1, res, ii, jj, tautologie, lit1, lits[1000], *lits1, lit, var, *pos_nb, *neg_nb;
	//int clause,*vars_signs,cpt;
	if (fp_in == NULL) {
		return FALSE;
	}
	
	fscanf(fp_in, "%c", &ch);
	while (ch!='p') {
		while (ch!='\n') fscanf(fp_in, "%c", &ch);  
			fscanf(fp_in, "%c", &ch);
	}
	i = 0;
	while (ch != '\n') {
		pLine[i] = ch;
		i++;
		fscanf(fp_in, "%c", &ch);
	}
	sscanf(pLine, "p %s %d %d %lli", word2, &NB_VAR, &NB_CLAUSE, &HARD_WEIGHT);
	printf("c Instance info: p %s %d %d %lli\n", word2, NB_VAR, NB_CLAUSE, HARD_WEIGHT);
	if (NB_VAR > tab_variable_size || NB_CLAUSE > tab_clause_size - INIT_BASE_NB_CLAUSE) {
		printf("ERROR: Out of memory.\n");
		exit(0);
	}
	NB_CLAUSE = NB_CLAUSE + BASE_NB_CLAUSE;
	INIT_NB_CLAUSE = NB_CLAUSE;
	if (strcmp(word2, "cnf") == 0)
		instance_type = 0; // cnf
	else {
		instance_type = 1; // wcnf
	}
	lire_clauses(fp_in, instance_type);
	fclose(fp_in);
	build_structure();
	eliminate_redundance();
	if (clean_structure()==FALSE)
		return FALSE;
	return TRUE;
}

// end of input.c

void print_clause(int clause) {
	int *vars_signs, var;
	
	printf("(%i, %i, %i) [%lli] ", clause, clause_length[clause], clause_state[clause], clause_weight[clause]);
	vars_signs = var_sign[clause];
	for(var = *vars_signs; var != NONE; var = *(vars_signs += 2)) {
		if (*(vars_signs + 1) == NEGATIVE)
			printf("-");
		printf("%i ", var + 1);
	}
	printf("0");
}

void remove_clauses(int var) {
	register int clause;
	int p_clause;
	
	if (var_current_value[var] == POSITIVE)
		p_clause = first_pos_in[var];
	else
		p_clause = first_neg_in[var];
	for (clause = LIT_IN_STACK[p_clause]; clause != NONE; clause = get_next_clause(&p_clause)) {
		if (clause_state[clause] == ACTIVE) {
			clause_state[clause] = PASSIVE;
			push(clause, CLAUSE_STACK);
		}
	}
}

int reduce_clauses(int var) {
	register int clause;
	int p_clause;

	if (var_current_value[var] == POSITIVE)
		p_clause = first_neg_in[var];
	else
		p_clause = first_pos_in[var];
	for (clause = LIT_IN_STACK[p_clause]; clause != NONE; clause = get_next_clause(&p_clause)) {
		if (clause_state[clause] == ACTIVE) {
			clause_length[clause]--;
			push(clause, REDUCEDCLAUSE_STACK);
			switch (clause_length[clause]) {
			case 0:
				NB_EMPTY += clause_weight[clause];
				if (UB<=NB_EMPTY) { /// Test if this check is needed
					push(clause, IG_STACK);
					push(var, MARK_STACK);
					mark[var] = MARK_STACK_fill_pointer;
					unit_of_var[var] = clause;
					return NONE;
				}
				break;
			case 1:
				push(clause, UNITCLAUSE_STACK);
				#ifdef DEBUG
					if (UNITCLAUSE_STACK_fill_pointer > tab_unitclause_size - 5) {
						printf("DEBUG: UNITCLAUSE_STACK.\n");
						exit(0);
					}
				#endif
				break;
			}
		}
	}
	return TRUE;
}

int my_reduce_clauses(int var) {
	register int clause;
	int p_clause;
	
	if (var_current_value[var] == POSITIVE)
		p_clause = first_neg_in[var];
	else
		p_clause = first_pos_in[var];
	for (clause = LIT_IN_STACK[p_clause]; clause != NONE; clause = get_next_clause(&p_clause)) {
		if (clause_state[clause] == ACTIVE) {
			clause_length[clause]--;
			push(clause, REDUCEDCLAUSE_STACK);
			switch (clause_length[clause]) {
			case 0:
				return clause;
			case 1:
				push(clause, MY_UNITCLAUSE_STACK);
				#ifdef DEBUG
					if (MY_UNITCLAUSE_STACK_fill_pointer > tab_unitclause_size - 5) {
						printf("DEBUG: MY_UNITCLAUSE_STACK.\n");
						exit(0);
					}
				#endif
				break;
			}
		}
	}
	return NO_CONFLICT;
}

int my_reduce_clauses_for_fl(int var) {
	register int clause;
	int p_clause;
	
	if (var_current_value[var] == POSITIVE)
		p_clause = first_neg_in[var];
	else
		p_clause = first_pos_in[var];
	for (clause = LIT_IN_STACK[p_clause]; clause != NONE; clause = get_next_clause(&p_clause)) {
		if (clause_state[clause] == ACTIVE) {
			clause_length[clause]--;
			push(clause, REDUCEDCLAUSE_STACK);
			switch (clause_length[clause]) {
			case 0:
				return clause;
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
	int var, index,clause, saved;
	int *vars_signs, var_s;
	
	NB_BACK++;
	
	while (VARIABLE_STACK_fill_pointer > 0) {
		var = pop(VARIABLE_STACK);
		
		if (nb_undo_learned[var] > 0) {
			for (index = 0; index < nb_undo_learned[var]; index++) {
				clause = undo_learned[var][index];
				clause_length[clause]++;
			}
			nb_undo_learned[var] = 0;
		}
		if (var_rest_value[var] == NONE)
			var_state[var] = ACTIVE;
		else {
			for (index = saved_clause_stack[var]; index < CLAUSE_STACK_fill_pointer; index++)
				clause_state[CLAUSE_STACK[index]] = ACTIVE;
			CLAUSE_STACK_fill_pointer = saved_clause_stack[var];
			for (index = saved_reducedclause_stack[var]; index < REDUCEDCLAUSE_STACK_fill_pointer; index++) {
				//clause = REDUCEDCLAUSE_STACK[index];
				clause_length[REDUCEDCLAUSE_STACK[index]]++;
			}
			REDUCEDCLAUSE_STACK_fill_pointer = saved_reducedclause_stack[var];
			UNITCLAUSE_STACK_fill_pointer=saved_unitclause_stack[var];
			NB_EMPTY=saved_nb_empty[var];
			NB_CLAUSE=saved_nb_clause[var];
			NEW_CLAUSES_fill_pointer=saved_new_clauses[var];
			saved=saved_saved_clauses[var];
			for (index = SAVED_CLAUSES_fill_pointer-1; index >= saved; index--)
				LIT_IN_STACK[SAVED_CLAUSE_POSITIONS[index]]=SAVED_CLAUSES[index];
			SAVED_CLAUSES_fill_pointer=saved;
			saved = saved_weights_nb[var];
			for (index = SAVED_WEIGHTS_CLAUSE_fill_pointer - 1; index >= saved; index--)
				clause_weight[SAVED_WEIGHTS_CLAUSE[index]] = SAVED_WEIGHTS_WEIGHT[index];
			SAVED_WEIGHTS_CLAUSE_fill_pointer = saved;
			SAVED_WEIGHTS_WEIGHT_fill_pointer = saved;
			saved = saved_lit_in_stack[var];
			for (index = LIT_IN_STACK_fill_pointer - 2; index > saved; index -= 3) {
				vars_signs = var_sign[LIT_IN_STACK[LIT_IN_STACK[index - 1]]];
				for(var_s = *vars_signs; var_s != NONE; var_s = *(vars_signs += 2)) {
					if (last_pos_in[var_s] == index) {
						last_pos_in[var_s] = LIT_IN_STACK[index - 1];
						break;
					}
					if (last_neg_in[var_s] == index) {
						last_neg_in[var_s] = LIT_IN_STACK[index - 1];
						break;
					}
				}
				LIT_IN_STACK[LIT_IN_STACK[index - 1] + 1] = NONE;
				LIT_IN_STACK[LIT_IN_STACK[index - 1]] = NONE;
			}
			LIT_IN_STACK_fill_pointer = saved;
			if (NB_EMPTY<UB) {
				var_current_value[var] = var_rest_value[var];
				var_rest_value[var] = NONE;
				push(var, VARIABLE_STACK);
				if (reduce_clauses(var)==NONE)
					return NONE;
				remove_clauses(var);
				return TRUE;
			} else
				var_state[var] = ACTIVE;
		}
	}
	return FALSE;
}

int verify_solution() {
	int i, var, *vars_signs, clause_truth;
	lli_type nb = 0;
	
	for (i = INIT_BASE_NB_CLAUSE; i < REAL_NB_CLAUSE; i++) {
		clause_truth = FALSE;
		vars_signs = var_sign[i];
		for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2))
			if (*(vars_signs+1) == var_current_value[var] ) {
				clause_truth = TRUE;
				break;
			}
		if (clause_truth == FALSE) {
			nb += ini_clause_weight[i];
		}
	}
	return nb;
}

void reset_context(int saved_clause_stack_fill_pointer, int saved_reducedclause_stack_fill_pointer, int saved_unitclause_stack_fill_pointer, int saved_variable_stack_fill_pointer) {
	int index, var, clause;
	for (index = saved_clause_stack_fill_pointer; index < CLAUSE_STACK_fill_pointer; index++)
		clause_state[CLAUSE_STACK[index]] = ACTIVE;
	CLAUSE_STACK_fill_pointer = saved_clause_stack_fill_pointer;
	
	for (index = saved_reducedclause_stack_fill_pointer; index < REDUCEDCLAUSE_STACK_fill_pointer; index++) {
		clause = REDUCEDCLAUSE_STACK[index];
		clause_length[REDUCEDCLAUSE_STACK[index]]++;
	}
	REDUCEDCLAUSE_STACK_fill_pointer = saved_reducedclause_stack_fill_pointer;
	
	for(index=saved_variable_stack_fill_pointer; index<VARIABLE_STACK_fill_pointer; index++) {
		var=VARIABLE_STACK[index];
		reason[var]=NO_REASON;
		var_state[var]=ACTIVE;
	}
	VARIABLE_STACK_fill_pointer=saved_variable_stack_fill_pointer;
	UNITCLAUSE_STACK_fill_pointer=saved_unitclause_stack_fill_pointer;
}

// int replace_clause(int newclause, int clause_to_replace, int *clauses) {
//   int clause, flag=FALSE;
//   
//   for(clause=*clauses; clause!=NONE; clause=*(++clauses)) {
//     if (clause==clause_to_replace) {
//       *clauses=newclause;
//       SAVED_CLAUSE_POSITIONS[SAVED_CLAUSES_fill_pointer]=clauses;
//       push(clause_to_replace, SAVED_CLAUSES);
//       flag=TRUE;
//       break;
//     }
//   }
//   if (flag==FALSE)
//     printf("problem...");
//   return flag;
// }

void create_binaryclause(int var1, int sign1, int var2, int sign2, lli_type min_weight) {
	int *vars_signs;
	
	vars_signs=NEW_CLAUSES[NEW_CLAUSES_fill_pointer++];
	if (var1<var2) {
		vars_signs[0]=var1;
		vars_signs[1]=sign1;
		vars_signs[2]=var2;
		vars_signs[3]=sign2;
	} else {
		vars_signs[0]=var2;
		vars_signs[1]=sign2;
		vars_signs[2]=var1;
		vars_signs[3]=sign1;
	}
	vars_signs[4]=NONE;
	var_sign[NB_CLAUSE]=vars_signs;
	clause_state[NB_CLAUSE]=ACTIVE;
	clause_length[NB_CLAUSE]=2;
	clause_weight[NB_CLAUSE] = min_weight;
	if (sign1==POSITIVE)
		add_new_lit_in(&last_pos_in[var1], NB_CLAUSE);
	else
		add_new_lit_in(&last_neg_in[var1], NB_CLAUSE);
	if (sign2==POSITIVE)
		add_new_lit_in(&last_pos_in[var2], NB_CLAUSE);
	else
		add_new_lit_in(&last_neg_in[var2], NB_CLAUSE);
	NB_CLAUSE++;
	#ifdef DEBUG
		if (NB_CLAUSE > tab_clause_size - 5) {
			printf("DEBUG: NB_CLAUSE.\n");
			exit(0);
		}
	#endif
}

int verify_binary_clauses(int *varssigns, int var1, int sign1, int var2, int sign2) {
  //int nb=0;

  if (var1==*varssigns) {
    if ((*(varssigns+1)!=1-sign1) || (var2!=*(varssigns+2)) ||
	(*(varssigns+3)!=1-sign2)) {
      printf("VBC problem..");
      return FALSE;
    }
  }
  else {
    if ((var2 != *varssigns) || (*(varssigns+1)!=1-sign2) || (var1!=*(varssigns+2)) ||
	(*(varssigns+3)!=1-sign1)) {
      printf("VBC problem..");
      return FALSE;
    }
  }
  return TRUE;
}

// int create_clause_from_conflict_clauses(int clause1, int clause2, int clause3) {
//   //int var3, sign3, var2, sign2,*clauses2, *clauses3, *vars_signs, varssigns[4], i=0, var;
//   int var3, sign3, var2, sign2, *vars_signs, varssigns[4], i=0, var;
// 
//   if ((clause_state[clause1]==ACTIVE) && (clause_length[clause1]==2) &&
//       (clause_state[clause2]==ACTIVE) && (clause_length[clause2]==1) &&
//       (clause_state[clause3]==ACTIVE) && (clause_length[clause3]==1)) {
//     vars_signs = var_sign[clause1];
//     for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
//       if (var_state[var]==ACTIVE) {
// 	varssigns[i++]=var; varssigns[i++]=*(vars_signs+1);
//       }
//     }
//     /*
//     vars_signs = var_sign[clause2];
//     for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
//       if (var_state[var]==ACTIVE) {
// 	var2=var; sign2=*(vars_signs+1);
//       }
//     }
//     vars_signs = var_sign[clause3];
//     for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
//       if (var_state[var]==ACTIVE) {
// 	var3=var; sign3=*(vars_signs+1);
//       }
//     }
//     verify_binary_clauses(varssigns, var2, sign2, var3, sign3);
//     */
//     var2=varssigns[0]; sign2=1-varssigns[1];
//     var3=varssigns[2]; sign3=1-varssigns[3];
//     create_binaryclause(var2, sign2, var3, sign3, clause2, clause3);
//     push(clause1, CLAUSES_TO_REMOVE);
//     push(clause2, CLAUSES_TO_REMOVE);
//     push(clause3, CLAUSES_TO_REMOVE);
//     return TRUE;
//   }
//   else {
//     return FALSE;
//   }
// }

int LINEAR_REASON_STACK1[tab_clause_size];
int LINEAR_REASON_STACK1_fill_pointer=0;
int LINEAR_REASON_STACK2[tab_clause_size];
int LINEAR_REASON_STACK2_fill_pointer=0;
int clause_involved[tab_clause_size];

int search_linear_reason1(int var) {
	int *vars_signs, clause, fixed_var, index_var, new_fixed_var;
	
	for(fixed_var=var; fixed_var!=NONE; fixed_var=new_fixed_var) {
		clause=reason[fixed_var];
		vars_signs = var_sign[clause];
		new_fixed_var=NONE;
		push(clause, LINEAR_REASON_STACK1);
		clause_involved[clause]=TRUE;
		for(index_var=*vars_signs; index_var!=NONE; index_var=*(vars_signs+=2)) {
			if ((index_var!=fixed_var) && (reason[index_var]!=NO_REASON)) {
				if (new_fixed_var==NONE)
					new_fixed_var=index_var;
				else {
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

#define SIMPLE_NON_LINEAR_CASE 2
#define SIMPLE_RS1_NON_LINEAR_CASE 3
#define SIMPLE_RS2_NON_LINEAR_CASE 4
#define SIMPLE_CUB_NON_LINEAR_CASE 5
#define SIMPLE_RS1_3_NON_LINEAR_CASE 6
#define SIMPLE_RS2_3_NON_LINEAR_CASE 7

int search_linear_reason2(int var) {
	int *vars_signs, clause, fixed_var, index_var, new_fixed_var;
	
	for(fixed_var=var; fixed_var!=NONE; fixed_var=new_fixed_var) {
		clause=reason[fixed_var];
		if (clause_involved[clause]==TRUE) {
			if (LINEAR_REASON_STACK2_fill_pointer == 2 && LINEAR_REASON_STACK1_fill_pointer > 2 && LINEAR_REASON_STACK1[ 2 ] == clause)
				return SIMPLE_NON_LINEAR_CASE;
			else if (LINEAR_REASON_STACK2_fill_pointer == 2 && LINEAR_REASON_STACK1_fill_pointer > 3 && LINEAR_REASON_STACK1[ 3 ] == clause)
				return SIMPLE_RS2_NON_LINEAR_CASE;
			else if (LINEAR_REASON_STACK2_fill_pointer == 3 && LINEAR_REASON_STACK1_fill_pointer > 2 && LINEAR_REASON_STACK1[ 2 ] == clause)
				return SIMPLE_RS1_NON_LINEAR_CASE;
			else if (LINEAR_REASON_STACK2_fill_pointer == 3 && LINEAR_REASON_STACK1_fill_pointer > 3 && LINEAR_REASON_STACK1[ 3 ] == clause)
				return SIMPLE_CUB_NON_LINEAR_CASE;
			else if (LINEAR_REASON_STACK2_fill_pointer == 2 && LINEAR_REASON_STACK1_fill_pointer > 4 && LINEAR_REASON_STACK1[ 4 ] == clause)
				return SIMPLE_RS2_3_NON_LINEAR_CASE;
			else if (LINEAR_REASON_STACK2_fill_pointer == 4 && LINEAR_REASON_STACK1_fill_pointer > 2 && LINEAR_REASON_STACK1[ 2 ] == clause)
				return SIMPLE_RS1_3_NON_LINEAR_CASE;
			else
				return FALSE;
		} else
			push(clause, LINEAR_REASON_STACK2);
		vars_signs = var_sign[clause];
		new_fixed_var=NONE;
		for(index_var=*vars_signs; index_var!=NONE; index_var=*(vars_signs+=2)) {
			if ((index_var!=fixed_var) && (reason[index_var]!=NO_REASON)) {
				if (new_fixed_var==NONE)
					new_fixed_var=index_var;
				else
					return FALSE;
			}
		}
	}
	return TRUE;
}

// clause1 is l1->l2, clause is l2->l3, clause3 is ((not l3) or (not l4))
// i.e., the reason of l2 is clause1, the reason of l3 is clause
int check_reason(int *varssigns, int clause, int clause1, int clause2) {
	//int var, *vars_signs, var1, var2, flag;
	int var, *vars_signs, flag;
	
	if ((reason[varssigns[0]]!=clause1) || (reason[varssigns[2]]!=clause))
		return FALSE;
	vars_signs = var_sign[clause2];
	flag=FALSE;
	for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
		if ((varssigns[2]==var) && (reason[var]!=NO_REASON) && (*(vars_signs+1) != var_current_value[var])) {
			flag=TRUE;
		}
	}
	return flag;
}

int create_complementary_binclause(int clause, int clause1, int clause2, lli_type min_weight) {
	int var, *vars_signs, i=0, varssigns[4], sign;
	vars_signs = var_sign[clause];
	for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
		if (reason[var]!=NO_REASON) {
			varssigns[i++]=var; varssigns[i++]=*(vars_signs+1); 
		}
	}
	if (reason[varssigns[2]]==clause1) {
		var=varssigns[2];
		sign=varssigns[3];
		varssigns[2]=varssigns[0];
		varssigns[3]=varssigns[1];
		varssigns[0]=var;
		varssigns[1]=sign;
	}
	#ifdef DEBUG
		if ((i!=4) || (check_reason(varssigns, clause, clause1, clause2)==FALSE))
			printf("CCB problem...");
	#endif
	create_binaryclause(varssigns[0], 1-varssigns[1], varssigns[2], 1-varssigns[3], min_weight);
	return TRUE;
}

int get_satisfied_literal(int clause) {
	int var, *vars_signs;
	
	vars_signs = var_sign[clause];
	for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
		if (*(vars_signs+1) == var_current_value[var])
			return var;
	}
	printf("ERROR: Satisfied literal not found.\n");
	return NONE;
}

void create_ternary_clauses(int var1, int sign1, int var2, int sign2, int var3, int sign3, lli_type min_weight) {
	int *vars_signs;
	
	vars_signs=NEW_CLAUSES[NEW_CLAUSES_fill_pointer++];
	vars_signs[0]=var1;
	vars_signs[1]=sign1;
	vars_signs[2]=var2;
	vars_signs[3]=sign2;
	vars_signs[4]=var3;
	vars_signs[5]=sign3;
	vars_signs[6]=NONE;
	var_sign[NB_CLAUSE] = vars_signs;
	clause_state[NB_CLAUSE] = ACTIVE;
	clause_length[NB_CLAUSE] = 3;
	clause_weight[NB_CLAUSE] = min_weight;
	if (sign1==POSITIVE)
		add_new_lit_in(&last_pos_in[var1], NB_CLAUSE);
	else
		add_new_lit_in(&last_neg_in[var1], NB_CLAUSE);
	if (sign2==POSITIVE)
		add_new_lit_in(&last_pos_in[var2], NB_CLAUSE);
	else
		add_new_lit_in(&last_neg_in[var2], NB_CLAUSE);
	if (sign3==POSITIVE)
		add_new_lit_in(&last_pos_in[var3], NB_CLAUSE);
	else
		add_new_lit_in(&last_neg_in[var3], NB_CLAUSE);
	NB_CLAUSE++;
	#ifdef DEBUG
		if (NB_CLAUSE > tab_clause_size - 5) {
			printf("DEBUG: NB_CLAUSE.\n");
			exit(0);
		}
	#endif
}

int non_linear_conflict(int empty_clause, int var1, int sign1, int var2, int sign2, lli_type min_weight) {
	int var, sign, j;
	// driving unit clause is LINEAR_REASON_STACK1[2] (propagate
	// it resulting the empty_clause by simple non-linear derivation
	// var1, sign1, var2, and sign2 are the two literals of empty_clause
	
	var = get_satisfied_literal(LINEAR_REASON_STACK1[2]);
	sign = var_current_value[var];
	for(j = 2; j < LINEAR_REASON_STACK1_fill_pointer - 1; j++) {
		create_complementary_binclause(LINEAR_REASON_STACK1[j], LINEAR_REASON_STACK1[j+1], LINEAR_REASON_STACK1[j-1], min_weight);
	}
	create_ternary_clauses(var, sign, var1, sign1, var2, sign2, min_weight);
	create_ternary_clauses(var, 1-sign, var1, 1-sign1, var2, 1-sign2, min_weight);
	return TRUE;
}

int non_linear_conflict_rs1(int empty_clause, int var1, int sign1, int var2, int sign2, lli_type min_weight) {
	int var, sign, j;
	int svar, ssign;
	
	var = get_satisfied_literal(LINEAR_REASON_STACK1[2]);
	sign = var_current_value[var];
	for(j = 2; j < LINEAR_REASON_STACK1_fill_pointer - 1; j++) {
		create_complementary_binclause(LINEAR_REASON_STACK1[j], LINEAR_REASON_STACK1[j+1], LINEAR_REASON_STACK1[j-1], min_weight);
	}
	svar = get_satisfied_literal(LINEAR_REASON_STACK2[2]);
	ssign = var_current_value[svar];
	create_ternary_clauses(var, sign, svar, 1-ssign, var2, 1-sign2, min_weight);
	create_ternary_clauses(var, 1-sign, svar, ssign, var2, sign2, min_weight);
	create_ternary_clauses(var, sign, var1, sign1, var2, sign2, min_weight);
	create_ternary_clauses(var, 1-sign, var1, 1-sign1, var2, 1-sign2, min_weight);
	return TRUE;
}

int non_linear_conflict_rs2(int empty_clause, int var1, int sign1, int var2, int sign2, lli_type min_weight) {
	int var, sign, j;
	int svar, ssign;
	
	var = get_satisfied_literal(LINEAR_REASON_STACK1[3]);
	sign = var_current_value[var];
	for(j = 3; j < LINEAR_REASON_STACK1_fill_pointer - 1; j++) {
		create_complementary_binclause(LINEAR_REASON_STACK1[j], LINEAR_REASON_STACK1[j+1], LINEAR_REASON_STACK1[j-1], min_weight);
	}
	svar = get_satisfied_literal(LINEAR_REASON_STACK1[2]);
	ssign = var_current_value[svar];
	create_ternary_clauses(var, sign, svar, 1-ssign, var1, 1-sign1, min_weight);
	create_ternary_clauses(var, 1-sign, svar, ssign, var1, sign1, min_weight);
	create_ternary_clauses(var, sign, var1, sign1, var2, sign2, min_weight);
	create_ternary_clauses(var, 1-sign, var1, 1-sign1, var2, 1-sign2, min_weight);
	return TRUE;
}

int non_linear_conflict_cub(int empty_clause, int var1, int sign1, int var2, int sign2, lli_type min_weight) {
	int var, sign, j;
	int s1var, s1sign;
	int s2var, s2sign;
	
	var = get_satisfied_literal(LINEAR_REASON_STACK1[3]);
	sign = var_current_value[var];
	for(j = 3; j < LINEAR_REASON_STACK1_fill_pointer - 1; j++) {
		create_complementary_binclause(LINEAR_REASON_STACK1[j], LINEAR_REASON_STACK1[j+1], LINEAR_REASON_STACK1[j-1], min_weight);
	}
	s1var = get_satisfied_literal(LINEAR_REASON_STACK1[2]);
	s1sign = var_current_value[s1var];
	s2var = get_satisfied_literal(LINEAR_REASON_STACK2[2]);
	s2sign = var_current_value[s2var];
	create_ternary_clauses(var, sign, s1var, 1-s1sign, var1, 1-sign1, min_weight);
	create_ternary_clauses(var, 1-sign, s1var, s1sign, var1, sign1, min_weight);
	create_ternary_clauses(var, sign, s2var, 1-s2sign, var2, 1-sign2, min_weight);
	create_ternary_clauses(var, 1-sign, s2var, s2sign, var2, sign2, min_weight);
	create_ternary_clauses(var, sign, var1, sign1, var2, sign2, min_weight);
	create_ternary_clauses(var, 1-sign, var1, 1-sign1, var2, 1-sign2, min_weight);
	return TRUE;
}

int non_linear_conflict_rs1_3(int empty_clause, int var1, int sign1, int var2, int sign2, lli_type min_weight) {
	int var, sign, j;
	int svar, ssign;
	int s3var, s3sign;
	
	var = get_satisfied_literal(LINEAR_REASON_STACK1[2]);
	sign = var_current_value[var];
	for(j = 2; j < LINEAR_REASON_STACK1_fill_pointer - 1; j++) {
		create_complementary_binclause(LINEAR_REASON_STACK1[j], LINEAR_REASON_STACK1[j+1], LINEAR_REASON_STACK1[j-1], min_weight);
	}
	svar = get_satisfied_literal(LINEAR_REASON_STACK2[2]);
	ssign = var_current_value[svar];
	s3var = get_satisfied_literal(LINEAR_REASON_STACK2[3]);
	s3sign = var_current_value[s3var];
	create_ternary_clauses(var, sign, s3var, 1-s3sign, svar, ssign, min_weight);
	create_ternary_clauses(var, 1-sign, s3var, s3sign, svar, 1-ssign, min_weight);
	create_ternary_clauses(var, sign, svar, 1-ssign, var2, 1-sign2, min_weight);
	create_ternary_clauses(var, 1-sign, svar, ssign, var2, sign2, min_weight);
	create_ternary_clauses(var, sign, var1, sign1, var2, sign2, min_weight);
	create_ternary_clauses(var, 1-sign, var1, 1-sign1, var2, 1-sign2, min_weight);
	return TRUE;
}

int non_linear_conflict_rs2_3(int empty_clause, int var1, int sign1, int var2, int sign2, lli_type min_weight) {
	int var, sign, j;
	int svar, ssign;
	int s3var, s3sign;
	
	var = get_satisfied_literal(LINEAR_REASON_STACK1[4]);
	sign = var_current_value[var];
	for(j = 4; j < LINEAR_REASON_STACK1_fill_pointer - 1; j++) {
		create_complementary_binclause(LINEAR_REASON_STACK1[j], LINEAR_REASON_STACK1[j+1], LINEAR_REASON_STACK1[j-1], min_weight);
	}
	svar = get_satisfied_literal(LINEAR_REASON_STACK1[2]);
	ssign = var_current_value[svar];
	s3var = get_satisfied_literal(LINEAR_REASON_STACK1[3]);
	s3sign = var_current_value[s3var];
	create_ternary_clauses(var, sign, s3var, 1-s3sign, svar, ssign, min_weight);
	create_ternary_clauses(var, 1-sign, s3var, s3sign, svar, 1-ssign, min_weight);
	create_ternary_clauses(var, sign, svar, 1-ssign, var1, 1-sign1, min_weight);
	create_ternary_clauses(var, 1-sign, svar, ssign, var1, sign1, min_weight);
	create_ternary_clauses(var, sign, var1, sign1, var2, sign2, min_weight);
	create_ternary_clauses(var, 1-sign, var1, 1-sign1, var2, 1-sign2, min_weight);
	return TRUE;
}

int linear_conflict(int clause, lli_type min_weight) {
	int var, *vars_signs, i=0, varssigns[6], j=0, res;
	
	vars_signs = var_sign[clause];
	for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
		if (reason[var]!=NO_REASON) {
			varssigns[i++]=var;
			varssigns[i++]=*(vars_signs+1);
			if (i>4)
				return FALSE;
		}
	}
	if (i>4)
		return FALSE;
	if (i==0) {
		printf("ERROR: Conflict without reason.\n");
		return FALSE;
	} else {
		for(j=0; j<LINEAR_REASON_STACK1_fill_pointer; j++)
			clause_involved[LINEAR_REASON_STACK1[j]]=NONE;
		LINEAR_REASON_STACK1_fill_pointer=1; LINEAR_REASON_STACK2_fill_pointer=1;
		LINEAR_REASON_STACK1[0]=clause; LINEAR_REASON_STACK2[0]=clause;
		if (search_linear_reason1(varssigns[0])==FALSE)
			return FALSE;
		else {
			if (i == 4) {
				res = search_linear_reason2(varssigns[2]);
				if (res == FALSE)
					return FALSE;
				else if (res == SIMPLE_NON_LINEAR_CASE)
					return non_linear_conflict(clause, varssigns[0], varssigns[1], varssigns[2], varssigns[3], min_weight);
				else if (res == SIMPLE_RS1_NON_LINEAR_CASE)
					return non_linear_conflict_rs1(clause, varssigns[0], varssigns[1], varssigns[2], varssigns[3], min_weight);
				else if (res == SIMPLE_RS2_NON_LINEAR_CASE)
					return non_linear_conflict_rs2(clause, varssigns[0], varssigns[1], varssigns[2], varssigns[3], min_weight);
				else if (res == SIMPLE_CUB_NON_LINEAR_CASE)
					return non_linear_conflict_cub(clause, varssigns[0], varssigns[1], varssigns[2], varssigns[3], min_weight);
				else if (res == SIMPLE_RS1_3_NON_LINEAR_CASE)
					return non_linear_conflict_rs1_3(clause, varssigns[0], varssigns[1], varssigns[2], varssigns[3], min_weight);
				else if (res == SIMPLE_RS2_3_NON_LINEAR_CASE)
					return non_linear_conflict_rs2_3(clause, varssigns[0], varssigns[1], varssigns[2], varssigns[3], min_weight);
				create_binaryclause(varssigns[0], 1-varssigns[1], varssigns[2], 1-varssigns[3], min_weight);
				for(j = 1; j < LINEAR_REASON_STACK2_fill_pointer - 1; j++) {
					create_complementary_binclause(LINEAR_REASON_STACK2[j], LINEAR_REASON_STACK2[j+1], LINEAR_REASON_STACK2[j-1], min_weight);
				}
			}
			for(j = 1; j < LINEAR_REASON_STACK1_fill_pointer - 1; j++) {
				create_complementary_binclause(LINEAR_REASON_STACK1[j], LINEAR_REASON_STACK1[j+1], LINEAR_REASON_STACK1[j-1], min_weight);
			}
		}
		return TRUE;
	}
}

void remove_linear_reasons(lli_type min_weight) {
	int i, clause;
	for(i = 0; i < LINEAR_REASON_STACK1_fill_pointer; i++) {
		clause = LINEAR_REASON_STACK1[i];
		if (clause_weight[clause] == min_weight) {
			clause_state[clause]=PASSIVE;
			push(clause, CLAUSE_STACK);
			push(clause, CLAUSES_WEIGHTS_TO_REMOVE);
			push(min_weight, WEIGHTS_TO_REMOVE);
			#ifdef DEBUG
				if (CLAUSES_WEIGHTS_TO_REMOVE_fill_pointer > tab_clause_size - 5) {
					printf("DEBUG: CLAUSES_WEIGHTS_TO_REMOVE.\n");
					exit(0);
				}
			#endif
		} else if (clause_weight[clause] > min_weight) {
			if (clause_weight[clause] < UB) {
				push(clause, SAVED_WEIGHTS_CLAUSE);
				#ifdef DEBUG
					if (SAVED_WEIGHTS_CLAUSE_fill_pointer > tab_clause_size - 5) {
						printf("DEBUG: SAVED_WEIGHTS_CLAUSE.\n");
						exit(0);
					}
				#endif
				push(clause_weight[clause], SAVED_WEIGHTS_WEIGHT);
				clause_weight[clause] -= min_weight;
				push(clause, CLAUSES_WEIGHTS_TO_REMOVE);
				push(min_weight, WEIGHTS_TO_REMOVE);
				#ifdef DEBUG
					if (CLAUSES_WEIGHTS_TO_REMOVE_fill_pointer > tab_clause_size - 5) {
						printf("DEBUG: CLAUSES_WEIGHTS_TO_REMOVE.\n");
						exit(0);
					}
				#endif
			}
		} else
			printf("ERROR: Negative weight.\n");
	}
	for(i = 1; i < LINEAR_REASON_STACK2_fill_pointer; i++) {
		clause=LINEAR_REASON_STACK2[i];
		if (clause_weight[clause] == min_weight) {
			clause_state[clause]=PASSIVE;
			push(clause, CLAUSE_STACK);
			push(clause, CLAUSES_WEIGHTS_TO_REMOVE);
			push(min_weight, WEIGHTS_TO_REMOVE);
			#ifdef DEBUG
				if (CLAUSES_WEIGHTS_TO_REMOVE_fill_pointer > tab_clause_size - 5) {
					printf("DEBUG: CLAUSES_WEIGHTS_TO_REMOVE.\n");
					exit(0);
				}
			#endif
		} else if (clause_weight[clause] > min_weight) {
			if (clause_weight[clause] < UB) {
				push(clause, SAVED_WEIGHTS_CLAUSE);
				#ifdef DEBUG
					if (SAVED_WEIGHTS_CLAUSE_fill_pointer > tab_clause_size - 5) {
						printf("DEBUG: SAVED_WEIGHTS_CLAUSE.\n");
						exit(0);
					}
				#endif
				push(clause_weight[clause], SAVED_WEIGHTS_WEIGHT);
				clause_weight[clause] -= min_weight;
				push(clause, CLAUSES_WEIGHTS_TO_REMOVE);
				push(min_weight, WEIGHTS_TO_REMOVE);
				#ifdef DEBUG
					if (CLAUSES_WEIGHTS_TO_REMOVE_fill_pointer > tab_clause_size - 5) {
						printf("DEBUG: CLAUSES_WEIGHTS_TO_REMOVE.\n");
						exit(0);
					}
				#endif
			}
		} else
			printf("ERROR: Negative weight.\n");
	}
}

int there_is_unit_clause( int var_to_check ) {
	int unitclause_position, unitclause, var, *vars_signs;
	
	for( unitclause_position = 0; unitclause_position < UNITCLAUSE_STACK_fill_pointer; unitclause_position++) {
		unitclause = UNITCLAUSE_STACK[ unitclause_position ];
		if ((clause_state[unitclause] == ACTIVE)  && (clause_length[unitclause] > 0)) {
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

void remove_clauses_for_fl(int var) {
	register int clause;
	int p_clause;
	
	if (var_current_value[var] == POSITIVE)
		p_clause = first_pos_in[var];
	else
		p_clause = first_neg_in[var];
	for (clause = LIT_IN_STACK[p_clause]; clause != NONE; clause = get_next_clause(&p_clause)) {
		if (clause_state[clause] == ACTIVE) {
			clause_state[clause] = PASSIVE;
			push(clause, CLAUSE_STACK);
		}
	}
}

int my_unitclause_process(int, lli_type*);

int assign_and_unitclause_process( int var, int value, int starting_point) {
	int clause;
	lli_type min_weight;
	
	var_current_value[var] = value;
	var_rest_value[var] = NONE;
	var_state[var] = PASSIVE;
	push(var, VARIABLE_STACK);
	if ((clause = my_reduce_clauses_for_fl(var)) == NO_CONFLICT) {
		remove_clauses_for_fl(var);
		return my_unitclause_process(starting_point, &min_weight);
	} else {
		return clause;
	}
}

int store_reason_clauses( int clause, int starting ) {
	int *vars_signs, var, i;
	
	push(clause, REASON_STACK);
	for(i = starting; i < REASON_STACK_fill_pointer; i++) {
		clause = REASON_STACK[i];
		vars_signs = var_sign[clause];
		for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
			if (reason[var]!=NO_REASON) {
				push(reason[var], REASON_STACK);
				reason[var] = NO_REASON;
			}
		}
	}
	return i;
}

void remove_reason_clauses(lli_type *min_weight) {
	int i, clause;
	
	for(i = 0; i < REASON_STACK_fill_pointer; i++) {
		clause = REASON_STACK[i];
		if (clause_weight[clause] < *min_weight)
			*min_weight = clause_weight[clause];
	}
	for(i = 0; i < REASON_STACK_fill_pointer; i++) {
		clause = REASON_STACK[i];
		if (clause_weight[clause] <= *min_weight) {
			clause_state[clause] = PASSIVE;
			push(clause, CLAUSE_STACK);
		} else if (clause_weight[clause] > *min_weight) {
			if (clause_weight[clause] < UB) {
				push(clause, SAVED_WEIGHTS_CLAUSE);
				#ifdef DEBUG
					if (SAVED_WEIGHTS_CLAUSE_fill_pointer > tab_clause_size - 5) {
						printf("DEBUG: SAVED_WEIGHTS_CLAUSE.\n");
						exit(0);
					}
				#endif
				push(clause_weight[clause], SAVED_WEIGHTS_WEIGHT);
				clause_weight[clause] -= *min_weight;
			}
		}
	}
	REASON_STACK_fill_pointer = 0;
}

lli_type simple_get_pos_clause_nb(int);
lli_type simple_get_neg_clause_nb(int);

int failed_literal( int conflict ) {
	int clause, var, la = 0;
	int saved_clause_stack_fill_pointer, saved_reducedclause_stack_fill_pointer, saved_unitclause_stack_fill_pointer, saved_variable_stack_fill_pointer, my_saved_clause_stack_fill_pointer, saved_reason_stack_fill_pointer;
	lli_type min_weight = HARD_WEIGHT;
	
	saved_clause_stack_fill_pointer = CLAUSE_STACK_fill_pointer;
	saved_reducedclause_stack_fill_pointer = REDUCEDCLAUSE_STACK_fill_pointer;
	saved_unitclause_stack_fill_pointer = UNITCLAUSE_STACK_fill_pointer;
	saved_variable_stack_fill_pointer = VARIABLE_STACK_fill_pointer;
	my_saved_clause_stack_fill_pointer = CLAUSE_STACK_fill_pointer;
	
	for( var=0; var < NB_VAR && la + conflict + NB_EMPTY < UB; var++ ) {
		if ( var_state[ var ] == ACTIVE && !there_is_unit_clause( var )) {
			simple_get_pos_clause_nb(var);
			simple_get_neg_clause_nb(var);
			if (nb_neg_clause_of_length2[ var ] > 1 &&  nb_pos_clause_of_length2[ var ] > 1 ) {
				if ((clause = assign_and_unitclause_process(var, FALSE, saved_unitclause_stack_fill_pointer))!=NO_CONFLICT) {
					saved_reason_stack_fill_pointer = store_reason_clauses( clause, 0 );
					reset_context(my_saved_clause_stack_fill_pointer, saved_reducedclause_stack_fill_pointer, saved_unitclause_stack_fill_pointer, saved_variable_stack_fill_pointer);
					my_saved_clause_stack_fill_pointer = CLAUSE_STACK_fill_pointer;
					if ((clause=assign_and_unitclause_process(var, TRUE, saved_unitclause_stack_fill_pointer))>=0) {
						store_reason_clauses( clause, saved_reason_stack_fill_pointer);
						reset_context(my_saved_clause_stack_fill_pointer, saved_reducedclause_stack_fill_pointer, saved_unitclause_stack_fill_pointer, saved_variable_stack_fill_pointer);
						remove_reason_clauses(&min_weight);
						my_saved_clause_stack_fill_pointer = CLAUSE_STACK_fill_pointer;
						la += min_weight;
					} else {
						REASON_STACK_fill_pointer = 0;
						reset_context(my_saved_clause_stack_fill_pointer, saved_reducedclause_stack_fill_pointer, saved_unitclause_stack_fill_pointer, saved_variable_stack_fill_pointer);
					}
				} else {
					reset_context(my_saved_clause_stack_fill_pointer, saved_reducedclause_stack_fill_pointer, saved_unitclause_stack_fill_pointer, saved_variable_stack_fill_pointer);
				}
			}
		}
	}
	reset_context(saved_clause_stack_fill_pointer, saved_reducedclause_stack_fill_pointer, saved_unitclause_stack_fill_pointer, saved_variable_stack_fill_pointer);
	return la;
}

int lookahead() {
	int saved_clause_stack_fill_pointer, saved_reducedclause_stack_fill_pointer,
		saved_unitclause_stack_fill_pointer, saved_variable_stack_fill_pointer,
		my_saved_clause_stack_fill_pointer,
		clause, var, *vars_signs, i;
	lli_type conflict = 0, min_weight;
	int saved_clause_weight_stack_fill_pointer;
	int index, found;
	
	CLAUSES_TO_REMOVE_fill_pointer=0;
	WEIGHTS_TO_REMOVE_fill_pointer = 0;
	CLAUSES_WEIGHTS_TO_REMOVE_fill_pointer = 0;
	saved_clause_stack_fill_pointer= CLAUSE_STACK_fill_pointer;
	saved_reducedclause_stack_fill_pointer = REDUCEDCLAUSE_STACK_fill_pointer;
	saved_unitclause_stack_fill_pointer = UNITCLAUSE_STACK_fill_pointer;
	saved_variable_stack_fill_pointer=VARIABLE_STACK_fill_pointer;
	my_saved_clause_stack_fill_pointer= CLAUSE_STACK_fill_pointer;
	saved_clause_weight_stack_fill_pointer = SAVED_WEIGHTS_CLAUSE_fill_pointer;
	while ((clause=my_unitclause_process(0, &min_weight))!=NO_CONFLICT) {
		min_weight = min(clause_weight[clause], min_weight);
		conflict += min_weight;
		if (conflict+NB_EMPTY>=UB) {
			break;
		}
		if (linear_conflict(clause, min_weight) == TRUE) {
			conflict -= min_weight;
			NB_EMPTY += min_weight;;
			reset_context(my_saved_clause_stack_fill_pointer, saved_reducedclause_stack_fill_pointer, saved_unitclause_stack_fill_pointer, saved_variable_stack_fill_pointer);
			remove_linear_reasons(min_weight);
			my_saved_clause_stack_fill_pointer = CLAUSE_STACK_fill_pointer;
		} else {
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
			reset_context(my_saved_clause_stack_fill_pointer, saved_reducedclause_stack_fill_pointer, saved_unitclause_stack_fill_pointer, saved_variable_stack_fill_pointer);
			for(i=0; i<REASON_STACK_fill_pointer; i++) {
				clause=REASON_STACK[i];
				if (clause_weight[clause] == min_weight) {
					clause_state[clause]=PASSIVE;
					push(clause, CLAUSE_STACK);
				} else {
					if (clause_weight[clause] < UB) {
						/* this is important for wms */
						found = FALSE;
						for (index = saved_clause_weight_stack_fill_pointer; index < SAVED_WEIGHTS_CLAUSE_fill_pointer; index++) {
							if (clause == SAVED_WEIGHTS_CLAUSE[index]) {
								found = TRUE;
								break;
							}
						}
						if (!found) {
							push(clause, SAVED_WEIGHTS_CLAUSE);
							#ifdef DEBUG
								if (SAVED_WEIGHTS_CLAUSE_fill_pointer > tab_clause_size - 5) {
									printf("DEBUG: SAVED_WEIGHTS_CLAUSE.\n");
									exit(0);
								}
							#endif
							push(clause_weight[clause], SAVED_WEIGHTS_WEIGHT);
						}
						clause_weight[clause] -= min_weight;
					}
				}
			}
			REASON_STACK_fill_pointer=0;
			my_saved_clause_stack_fill_pointer=CLAUSE_STACK_fill_pointer;
		}
	}
	
	if (conflict + NB_EMPTY < UB) {
		reset_context(my_saved_clause_stack_fill_pointer, saved_reducedclause_stack_fill_pointer, saved_unitclause_stack_fill_pointer, saved_variable_stack_fill_pointer);
		conflict += failed_literal( conflict );
	}
	
	reset_context(saved_clause_stack_fill_pointer, saved_reducedclause_stack_fill_pointer, saved_unitclause_stack_fill_pointer, saved_variable_stack_fill_pointer);
	for (i = SAVED_WEIGHTS_CLAUSE_fill_pointer - 1; i >= saved_clause_weight_stack_fill_pointer; i--)
		clause_weight[SAVED_WEIGHTS_CLAUSE[i]] = SAVED_WEIGHTS_WEIGHT[i];
	SAVED_WEIGHTS_CLAUSE_fill_pointer = saved_clause_weight_stack_fill_pointer;
	SAVED_WEIGHTS_WEIGHT_fill_pointer = saved_clause_weight_stack_fill_pointer;
	
	if (conflict+NB_EMPTY>=UB)
		return NONE;
	
	for (i = 0; i < WEIGHTS_TO_REMOVE_fill_pointer; i++) {
		clause = CLAUSES_WEIGHTS_TO_REMOVE[i];
		if (clause_weight[clause] == WEIGHTS_TO_REMOVE[i]) {
			push(clause, CLAUSE_STACK);
			clause_state[clause]=PASSIVE;
		} else {
			push(clause, SAVED_WEIGHTS_CLAUSE);
			#ifdef DEBUG
				if (SAVED_WEIGHTS_CLAUSE_fill_pointer > tab_clause_size - 5) {
					printf("DEBUG: SAVED_WEIGHTS_CLAUSE.\n");
					exit(0);
				}
			#endif
			push(clause_weight[clause], SAVED_WEIGHTS_WEIGHT);
			clause_weight[clause] -= WEIGHTS_TO_REMOVE[i];
		}
	}
	WEIGHTS_TO_REMOVE_fill_pointer = 0;
	CLAUSES_WEIGHTS_TO_REMOVE_fill_pointer = 0;
	
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
			} else
				return clause;
		}
	}
	return NO_CONFLICT;
}
  
int my_unitclause_process(int starting_point, lli_type *min_weight) {
	int unitclause, unitclause_position,clause, my_unitclause_position, my_unitclause;
	
	*min_weight = HARD_WEIGHT;
	for (unitclause_position = starting_point; unitclause_position < UNITCLAUSE_STACK_fill_pointer; unitclause_position++) {
		unitclause = UNITCLAUSE_STACK[unitclause_position];
		if ((clause_state[unitclause] == ACTIVE)  && (clause_length[unitclause]>0) /*&& unitclause > INIT_BASE_NB_CLAUSE*/) {
			MY_UNITCLAUSE_STACK_fill_pointer = 0;
			if (clause_weight[unitclause] < *min_weight)
				*min_weight = clause_weight[unitclause];
			if ((clause=satisfy_unitclause(unitclause)) != NO_CONFLICT) {
				return clause;
			} else {
				for (my_unitclause_position = 0; my_unitclause_position < MY_UNITCLAUSE_STACK_fill_pointer; my_unitclause_position++) {
					my_unitclause = MY_UNITCLAUSE_STACK[my_unitclause_position];
					if ((clause_state[my_unitclause] == ACTIVE) && (clause_length[my_unitclause]>0) /*&& my_unitclause > INIT_BASE_NB_CLAUSE*/) {
						if (clause_weight[my_unitclause] < *min_weight)
							*min_weight = clause_weight[my_unitclause];
						if ((clause=satisfy_unitclause(my_unitclause)) != NO_CONFLICT) {
							return clause;
						}
					}
				}
			}
		}
	}
	return NO_CONFLICT;
}

int get_complement(int lit) {
	if (positive(lit))
		return lit+NB_VAR;
	else
		return lit-NB_VAR;
}

void create_unitclause(int lit, int subsumedclause, int p_clause, lli_type weight) {
	int clause, *vars_signs, flag=FALSE;
	
	vars_signs=NEW_CLAUSES[NEW_CLAUSES_fill_pointer++];
	if (lit<NB_VAR) {
		vars_signs[0]=lit;
		vars_signs[1]=POSITIVE;
	} else {
		vars_signs[0]=lit-NB_VAR;
		vars_signs[1]=NEGATIVE;
	}
	vars_signs[2]=NONE;
	var_sign[NB_CLAUSE]=vars_signs;
	clause_state[NB_CLAUSE]=ACTIVE;
	clause_length[NB_CLAUSE] = 1;
	clause_weight[NB_CLAUSE] = weight;
	push(NB_CLAUSE, UNITCLAUSE_STACK);
	
	for (clause = LIT_IN_STACK[p_clause]; clause != NONE; clause = get_next_clause(&p_clause)) {
		if (clause==subsumedclause) {
			LIT_IN_STACK[p_clause]=NB_CLAUSE;
			SAVED_CLAUSE_POSITIONS[SAVED_CLAUSES_fill_pointer]=p_clause;
			push(subsumedclause, SAVED_CLAUSES);
			flag=TRUE;
			break;
		}
	}
	if (flag==FALSE)
		printf("ERROR: subsumed clause not found.\n");
	NB_CLAUSE++;
	#ifdef DEBUG
		if (NB_CLAUSE > tab_clause_size - 5) {
			printf("DEBUG: NB_CLAUSE.\n");
			exit(0);
		}
	#endif
}

void verify_resolvent(int lit, int clause1, int clause2) {
	int *vars_signs1, *vars_signs2, lit1=NONE, lit2=NONE, temp, flag=FALSE, var, nb=0;
	
	if ((clause_state[clause1]!=ACTIVE) || (clause_state[clause2]!=ACTIVE))
		printf("erreur ");
	if ((clause_length[clause1]!=2) || (clause_length[clause2]!=2))
		printf("erreur ");
	vars_signs1=var_sign[clause1];
	vars_signs2=var_sign[clause2];
	for(var=*vars_signs1; var!=NONE; var=*(vars_signs1+=2)) {
		if (var_state[var] == ACTIVE ) {
			nb++;
			if (*(vars_signs1+1)==POSITIVE)
				temp=var;
			else temp=var+NB_VAR;
				if (temp==lit)
					flag=TRUE;
				else
					lit1=temp;
		}
	}
	if ((nb!=2) || (flag==FALSE))
		printf("erreur ");
	nb=0; flag=FALSE;
	for(var=*vars_signs2; var!=NONE; var=*(vars_signs2+=2)) {
		if (var_state[var] == ACTIVE ) {
			nb++;
			if (*(vars_signs2+1)==POSITIVE)
				temp=var;
			else
				temp=var+NB_VAR;
			if (temp==lit)
				flag=TRUE;
			else
				lit2=temp;
		}
	}
	if ((nb!=2) || (flag==FALSE))
		printf("erreur ");
	if (!complement(lit1, lit2))
		printf("erreur ");
}

lli_type simple_get_neg_clause_nb(int var) {
	lli_type neg_clause2_nb = 0;
	int p_clause, clause;
	
	p_clause = first_neg_in[var];
	
	for(clause = LIT_IN_STACK[p_clause]; clause != NONE; clause = get_next_clause(&p_clause))
		if ((clause_state[clause] == ACTIVE) && (clause_length[clause]==2))
			neg_clause2_nb += clause_weight[clause];
	nb_neg_clause_of_length2[var] = neg_clause2_nb;
	return neg_clause2_nb;
}

lli_type simple_get_pos_clause_nb(int var) {
	lli_type pos_clause2_nb = 0;
	int p_clause, clause;
	
	p_clause = first_pos_in[var];
	
	for(clause = LIT_IN_STACK[p_clause]; clause != NONE; clause = get_next_clause(&p_clause))
		if ((clause_state[clause] == ACTIVE) && (clause_length[clause]==2))
			pos_clause2_nb += clause_weight[clause];
	nb_pos_clause_of_length2[var] = pos_clause2_nb;
	return pos_clause2_nb;
}

int get_neg_clause_nb(int var) {
	lli_type neg_clause1_nb=0,neg_clause3_nb = 0, neg_clause2_nb = 0;
	int p_clause, clause;
	
	p_clause = first_neg_in[var];
	MY_UNITCLAUSE_STACK_fill_pointer=0;
	
	for (clause = LIT_IN_STACK[p_clause]; clause != NONE; clause = get_next_clause(&p_clause)) {
		if ((clause_state[clause] == ACTIVE) && (clause_length[clause]>0)) {
			switch(clause_length[clause]) {
			case 1:
				neg_clause1_nb += clause_weight[clause];
				push(clause, MY_UNITCLAUSE_STACK);
				break;
			case 2:
				neg_clause2_nb += clause_weight[clause];
				break;
			default:
				neg_clause3_nb += clause_weight[clause];
				break;
			}
		}
	}
	nb_neg_clause_of_length1[var] = neg_clause1_nb;
	nb_neg_clause_of_length2[var] = neg_clause2_nb;
	nb_neg_clause_of_length3[var] = neg_clause3_nb;
	return neg_clause1_nb+neg_clause2_nb + neg_clause3_nb;
}

#define OTHER_LIT_FIXED 1
#define THIS_LIT_FIXED 2

int get_pos_clause_nb(int var) {
	lli_type pos_clause1_nb=0, pos_clause3_nb = 0, pos_clause2_nb = 0;
	int p_clause, clause, clause1;
	int flag;
	
	p_clause = first_pos_in[var];
	for (clause = LIT_IN_STACK[p_clause]; clause != NONE; clause = get_next_clause(&p_clause)) {
		if ((clause_state[clause] == ACTIVE) && (clause_length[clause]>0)) {
			switch(clause_length[clause]) {
			case 1:
				if (MY_UNITCLAUSE_STACK_fill_pointer>0) {
					clause1=pop(MY_UNITCLAUSE_STACK);
					flag = 0;
					while (clause_weight[clause] > clause_weight[clause1]) {
						clause_state[clause1] = PASSIVE;
						push(clause1, CLAUSE_STACK);
						nb_neg_clause_of_length1[var] -= clause_weight[clause1];
						NB_EMPTY += clause_weight[clause1];
						if (clause_weight[clause] < UB) {
// 							found = FALSE;
// 							for (index = saved_weights_nb[top(SAVED_WEIGHTS_CLAUSE)]; index < SAVED_WEIGHTS_CLAUSE_fill_pointer; index++) {
// 								if (clause == SAVED_WEIGHTS_CLAUSE[index]) {
// 									found = TRUE;
// 									break;
// 								}
// 							}
//							if (!found) {
								push(clause, SAVED_WEIGHTS_CLAUSE);
								#ifdef DEBUG
									if (SAVED_WEIGHTS_CLAUSE_fill_pointer > tab_clause_size - 5) {
										printf("DEBUG: SAVED_WEIGHTS_CLAUSE.\n");
										exit(0);
									}
								#endif
								push(clause_weight[clause], SAVED_WEIGHTS_WEIGHT);
//							}
							clause_weight[clause] -= clause_weight[clause1];
						}
						if (MY_UNITCLAUSE_STACK_fill_pointer>0)
							clause1=pop(MY_UNITCLAUSE_STACK);
						else {
							flag = 1;
							break;
						}
					}
					if (flag == 0) {
						clause_state[clause] = PASSIVE;
						push(clause, CLAUSE_STACK);
						nb_neg_clause_of_length1[var] -= clause_weight[clause];
						NB_EMPTY += clause_weight[clause];
						if (clause_weight[clause] < clause_weight[clause1]) {
							if (clause_weight[clause] < UB) {
// 								found = FALSE;
// 								for (index = saved_weights_nb[top(SAVED_WEIGHTS_CLAUSE)]; index < SAVED_WEIGHTS_CLAUSE_fill_pointer; index++) {
// 									if (clause1 == SAVED_WEIGHTS_CLAUSE[index]) {
// 										found = TRUE;
// 										break;
// 									}
// 								}
// 								if (!found) {
									push(clause1, SAVED_WEIGHTS_CLAUSE);
									#ifdef DEBUG
										if (SAVED_WEIGHTS_CLAUSE_fill_pointer > tab_clause_size - 5) {
											printf("DEBUG: SAVED_WEIGHTS_CLAUSE.\n");
											exit(0);
										}
									#endif
									push(clause_weight[clause1], SAVED_WEIGHTS_WEIGHT);
//								}
								clause_weight[clause1] -= clause_weight[clause];
							}
						} else {
							clause_state[clause1]=PASSIVE;
							push(clause1, CLAUSE_STACK);
						}
					} else
						pos_clause1_nb += clause_weight[clause];
				} else
					pos_clause1_nb += clause_weight[clause];
				break;
			case 2:
				pos_clause2_nb += clause_weight[clause];
				break;
			default:
				pos_clause3_nb += clause_weight[clause];
				break;
			}
		}
	}
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
		printf("ERROR: Assigning passive variable.\n");
	var_state[var] = PASSIVE;
	push(var, VARIABLE_STACK);
	var_current_value[var] = current_value;
	var_rest_value[var] = rest_value;
	if (reduce_clauses(var)==NONE)
		return NONE;
	remove_clauses(var);
	return TRUE;
}

void add_to_learned_clause(int clause, int unit_var) {
	int var, *vars_signs;
	int i, j;
	
	vars_signs = var_sign[clause];
	for (var = *vars_signs; var != NONE; var = *(vars_signs += 2)) {
		if (var != unit_var) {
			i = 0;
			if (mark[var] > 0) {
				while (i < POST_UIP_LITS_fill_pointer &&  mark[var] < mark[POST_UIP_LITS[i]])
					i += 2;
				if (i == POST_UIP_LITS_fill_pointer || POST_UIP_LITS[i] != var) {
					for (j = POST_UIP_LITS_fill_pointer - 2; j >= i; j -= 2) {
						POST_UIP_LITS[j + 2] = POST_UIP_LITS[j];
						POST_UIP_LITS[j + 3] = POST_UIP_LITS[j + 1];
					}
					POST_UIP_LITS[i] = var;
					POST_UIP_LITS[i + 1] = *(vars_signs + 1);
					POST_UIP_LITS_fill_pointer += 2;
				}
			} else {
				while (i < NEW_CLAUSE_LITS_fill_pointer && NEW_CLAUSE_LITS[i] != var)
					i += 2;
				if (i == NEW_CLAUSE_LITS_fill_pointer) {
					push(var, NEW_CLAUSE_LITS);
					push(*(vars_signs + 1), NEW_CLAUSE_LITS);
				}
			}
		}
	}
}

void add_clause_to_DB() {
	int var, *vars, *vars_signs;
	int sen;
	
	if (NEW_CLAUSE_LITS_fill_pointer > MAX_LEN_LEARNED)
		return;
	vars = NEW_CLAUSE_LITS;
	for (var = *vars; var != NONE; var = *(vars += 2)) {
		if (nb_undo_learned[var] >= max_var_learned)
			return;
	}
	BASE_NB_CLAUSE--;
	var_sign[BASE_NB_CLAUSE] = (int *) malloc ((NEW_CLAUSE_LITS_fill_pointer + 1) * sizeof(int));
	vars_signs = var_sign[BASE_NB_CLAUSE];
	vars = NEW_CLAUSE_LITS;
	for (var = *vars; var != NONE; var = *(vars += 2), vars_signs += 2) {
		sen = *(vars + 1);
		*vars_signs = var;
		*(vars_signs + 1) = sen;
		if (sen == POSITIVE) {
			add_new_hlit_in(&first_pos_in[var], BASE_NB_CLAUSE);
		} else {
			add_new_hlit_in(&first_neg_in[var], BASE_NB_CLAUSE);
		}
		undo_learned[var][nb_undo_learned[var]++] = BASE_NB_CLAUSE;
		#ifdef DEBUG
			if (nb_undo_learned[var] > max_var_learned - 5) {
				printf("DEBUG: nb_undo_learned[var].\n");
				exit(0);
			}
		#endif
	}
	*vars_signs = NONE;
	
	//NB_EMPTY += HARD_WEIGHT;
	clause_length[BASE_NB_CLAUSE] = 0;
	clause_weight[BASE_NB_CLAUSE] = HARD_WEIGHT;
	clause_state[BASE_NB_CLAUSE] = ACTIVE;
}

void hard_learning() {
	int litNum;
	
	POST_UIP_LITS_fill_pointer = 0;
	NEW_CLAUSE_LITS_fill_pointer = 0;
	add_to_learned_clause(pop(IG_STACK), top(MARK_STACK));
	add_to_learned_clause(pop(IG_STACK), top(MARK_STACK));
	litNum = 0;
	while (POST_UIP_LITS_fill_pointer - litNum > 2) {
		add_to_learned_clause(unit_of_var[POST_UIP_LITS[litNum]], POST_UIP_LITS[litNum]);
		litNum += 2;
	}
	if (POST_UIP_LITS_fill_pointer > 0) { /// fa falta?
		push(POST_UIP_LITS[litNum], NEW_CLAUSE_LITS);
		push(POST_UIP_LITS[litNum + 1], NEW_CLAUSE_LITS);
	}
	push(NONE, NEW_CLAUSE_LITS);
	add_clause_to_DB();
}

int unitclause_process() {
	int unitclause, var, *vars_signs, unitclause_position,clause;
	
	IG_STACK_fill_pointer = 0;
	for (unitclause_position = 0; unitclause_position < UNITCLAUSE_STACK_fill_pointer; unitclause_position++) {
		unitclause = UNITCLAUSE_STACK[unitclause_position];
		if ((clause_state[unitclause] == ACTIVE)  && (clause_length[unitclause]>0) && clause_weight[unitclause] >= UB) {
			vars_signs = var_sign[unitclause];
			for(var=*vars_signs; var!=NONE; var=*(vars_signs+=2)) {
				if (var_state[var] == ACTIVE ) {
					var_current_value[var] = *(vars_signs+1);
					var_rest_value[var] = NONE;
					var_state[var] = PASSIVE;
					push(var, VARIABLE_STACK);
					push(unitclause, IG_STACK);
					push(var, MARK_STACK);
					mark[var] = MARK_STACK_fill_pointer;
					unit_of_var[var] = unitclause;
					if ((clause=reduce_clauses(var)) !=NONE) {
						remove_clauses(var);
						break;
					} else {
						if (partial == 1 && clause_weight[top(IG_STACK)] >= UB) {
							hard_learning();
						}
						while (MARK_STACK_fill_pointer > 0)
							mark[pop(MARK_STACK)] = NONE;
						return NONE;
					}
				}
			}
		}
	}
	while (MARK_STACK_fill_pointer > 0)
		mark[pop(MARK_STACK)] = NONE;
	return TRUE;
}

void search_unit_from_binary() {
	int p_clause, p_clause_list, clause;
	int var, *vars_signs;
	int lit;
	int search_var;
	int c1, c2;
	
	for (search_var = 0; search_var < NB_VAR; search_var++) {
		if (var_state[search_var] == ACTIVE) {
			p_clause = first_neg_in[search_var];
			for (clause = LIT_IN_STACK[p_clause]; clause != NONE; clause = get_next_clause(&p_clause)) {
				if (clause_state[clause] == ACTIVE && clause_length[clause] == 2) {
					vars_signs = var_sign[clause];
					for(var = *vars_signs; var != NONE; var = *(vars_signs += 2)) {
						if (var_state[var] == ACTIVE && var != search_var) {
							lit = get_lit(var, *(vars_signs + 1));
							mark[lit] = clause;
							push(lit, MARK_STACK);
							break;
						}
					}
				}
			}
			p_clause = first_pos_in[search_var];
			for (clause = LIT_IN_STACK[p_clause]; clause != NONE; clause = get_next_clause(&p_clause)) {
				if (clause_state[clause] == ACTIVE && clause_length[clause] == 2) {
					vars_signs = var_sign[clause];
					for(var = *vars_signs; var != NONE; var = *(vars_signs += 2)) {
						if (var_state[var] == ACTIVE && var != search_var) {
							lit = get_lit(var, *(vars_signs + 1));
							if (mark[lit] != NONE) {
								if (lit < NB_VAR)
									p_clause_list = first_pos_in[var];
								else
									p_clause_list = first_neg_in[var];
								if (clause_weight[mark[lit]] > clause_weight[clause]) {
									c1 = clause;
									c2 = mark[lit];
								} else {
									c1 = mark[lit];
									c2 = clause;
									mark[lit] = NONE;
								}
								push(c1, CLAUSE_STACK);
								clause_state[c1]=PASSIVE;
								if (clause_weight[c1] == clause_weight[c2]) {
									push(c2, CLAUSE_STACK);
									clause_state[c2]=PASSIVE;
								} else {
									if (clause_weight[c2] < UB) {
										push(c2, SAVED_WEIGHTS_CLAUSE);
										#ifdef DEBUG
											if (SAVED_WEIGHTS_CLAUSE_fill_pointer > tab_clause_size - 5) {
												printf("DEBUG: SAVED_WEIGHTS_CLAUSE.\n");
												exit(0);
											}
										#endif
										push(clause_weight[c2], SAVED_WEIGHTS_WEIGHT);
										clause_weight[c2] -= clause_weight[c1];
									}
								}
								create_unitclause(lit, c1, p_clause_list, clause_weight[c1]);
								break;
							}
						}
					}
				}
			}
			while (MARK_STACK_fill_pointer > 0)
				mark[pop(MARK_STACK)] = NONE;
		}
	}
}

int choose_and_instantiate_variable() {
	int var, chosen_var=NONE,cont=0;
	float poid, max_poid = -1.0;
	NB_BRANCHE++;
	
	if (lookahead()==NONE)
		return NONE;
	
	// if (UB-NB_EMPTY==1)
	if (unitclause_process() == NONE)
		return NONE;
	
	search_unit_from_binary();
	
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
			} else if (get_pos_clause_nb(var) == 0) {
				NB_MONO++;
				var_current_value[var] = FALSE;
				var_rest_value[var] = NONE;
				var_state[var] = PASSIVE;
				push(var, VARIABLE_STACK);
				remove_clauses(var);
			} else if (nb_neg_clause_of_length1[var]+NB_EMPTY>=UB) {
				if (assign_value(var, FALSE, NONE)==NONE)
					return NONE;
			} else if (nb_pos_clause_of_length1[var]+NB_EMPTY>=UB) {
				if (assign_value(var, TRUE, NONE)==NONE)
					return NONE;
			} else if (nb_neg_clause_of_length1[var]>=nb_pos_clause_of_length1[var]+nb_pos_clause_of_length2[var]+nb_pos_clause_of_length3[var]) {
				if (assign_value(var, FALSE, NONE)==NONE)
					return NONE;
			} else if (nb_pos_clause_of_length1[var]>=nb_neg_clause_of_length1[var]+nb_neg_clause_of_length2[var]+nb_neg_clause_of_length3[var]) {
				if (assign_value(var, TRUE, NONE)==NONE)
					return NONE;
			} else {
				if (nb_neg_clause_of_length1[var]>nb_pos_clause_of_length1[var]) {
					cont+=nb_pos_clause_of_length1[var];
				} else {
					cont+=nb_neg_clause_of_length1[var];
				}
			}
		}
	}
	
	if (cont + NB_EMPTY>=UB)
		return NONE;
		
	for (var = 0; var < NB_VAR; var++) {
		if (var_state[var] == ACTIVE) {
			reduce_if_positive[var] = nb_neg_clause_of_length1[var] * 2 + nb_neg_clause_of_length2[var] * 4 + nb_neg_clause_of_length3[var];
			reduce_if_negative[var] = nb_pos_clause_of_length1[var] * 2 + nb_pos_clause_of_length2[var] * 4 + nb_pos_clause_of_length3[var];
			poid = reduce_if_positive[var] * reduce_if_negative[var] * 2 + reduce_if_positive[var] + reduce_if_negative[var];
			if (poid > max_poid) {
				chosen_var = var;
				max_poid = poid;
			}
		}
	}
	
	if (chosen_var == NONE) return FALSE;
	
	saved_clause_stack[chosen_var] = CLAUSE_STACK_fill_pointer;
	saved_reducedclause_stack[chosen_var] = REDUCEDCLAUSE_STACK_fill_pointer;
	saved_unitclause_stack[chosen_var] = UNITCLAUSE_STACK_fill_pointer;
	saved_nb_empty[chosen_var]=NB_EMPTY;
	saved_nb_clause[chosen_var]=NB_CLAUSE;
	saved_saved_clauses[chosen_var]=SAVED_CLAUSES_fill_pointer;
	saved_new_clauses[chosen_var]=NEW_CLAUSES_fill_pointer;
	saved_weights_nb[chosen_var] = SAVED_WEIGHTS_CLAUSE_fill_pointer;
	saved_lit_in_stack[chosen_var] = LIT_IN_STACK_fill_pointer;
	if (reduce_if_positive[chosen_var]<reduce_if_negative[chosen_var])
		return assign_value(chosen_var, TRUE, FALSE);
	else
		return assign_value(chosen_var, FALSE, TRUE);
}

void dpl() {
	lli_type nb;
	int i;
	
	do {
		if (VARIABLE_STACK_fill_pointer==NB_VAR) {
			UB = NB_EMPTY;
			nb = verify_solution();
			if (nb != NB_EMPTY)
				printf("ERROR: Solution verification fails, real_empty = %lli, NB_EMPTY = %lli.\n", nb, NB_EMPTY);
			printf("o %lli\n", UB);
			for (i = 0; i < NB_VAR; i++)
				var_best_value[i] = var_current_value[i];
			while (backtracking()==NONE);
			if (VARIABLE_STACK_fill_pointer==0)
				break;
		}
		//if (UB-NB_EMPTY==1)
		if (unitclause_process() ==NONE)
			while (backtracking()==NONE);
		if (choose_and_instantiate_variable()==NONE)
			while (backtracking()==NONE);
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
		saved_weights_nb[var] = 0;
		saved_lit_in_stack[var] = 0;
		mark[var] = NONE;
		mark[var + NB_VAR] = NONE;
		nb_undo_learned[var] = 0;
	}
	for (clause = 0; clause < NB_CLAUSE; clause++) {
		lit_to_fix[clause]=NONE;
		clause_involved[clause]=NONE;
		ini_clause_weight[clause] = clause_weight[clause];
	}
}

void ubcsat(char file[]) {
	int i;
	char str[WORD_LENGTH];
	char fout[WORD_LENGTH];
	char strLS[tab_variable_size + WORD_LENGTH];
	double optimumLS;
	char valuesLS[tab_variable_size + WORD_LENGTH];
	FILE *ls;

	ls = fopen("wpmsz-ubcsat", "r");
	if (ls == (FILE*) NULL) {
		printf("WARNING: Ubcsat not found.\n");
		return;
	}
	sprintf(fout, "/tmp/wpmsz-ubcsat-%i.out", getpid());
	if (instance_type == 0) {
		sprintf(str, "wpmsz-ubcsat -alg irots -seed 0 -runs 10 -cutoff %i -solve -r best -inst %s | grep \"Run ID\" -A1 | tail -n1 > %s", NB_VAR * 100, file, fout);
	} else {
		sprintf(str, "wpmsz-ubcsat -alg irots -w -seed 0 -runs 10 -cutoff %i -solve -r best -inst %s | grep \"Run ID\" -A1 | tail -n1 > %s", NB_VAR * 100, file, fout);
	}
	system(str);
	ls = fopen(fout, "r");
	if (ls == (FILE*) NULL) {
		printf("WARNING: Ubcsat output file not found.\n");
		return;
	}
	fgets(strLS, tab_variable_size + WORD_LENGTH, ls);
	fclose(ls);
	remove(fout);
	sscanf(strLS, "%i %i %lf %s", &i, &i, &optimumLS, valuesLS);
	// printf("%lf\n%lli\n%s\n", optimumLS, (long long int) optimumLS, valuesLS);
	UB = min(UB, (long long int) optimumLS);
	i = 0;
	while (valuesLS[i] != '\0') {
		if (valuesLS[i] == '1')
			var_best_value[i] = TRUE;
		else
			var_best_value[i] = FALSE;
		i++;
	}
	if (i != NB_VAR) {
		printf("WARNING: Ubcsat problem.\n");
	}
}

int main(int argc, char *argv[]) {
	char saved_input_file[WORD_LENGTH];
	//int i,  var;
	int i;
	//long begintime, endtime, mess;
	//struct tms *a_tms;
	//FILE *fp_time;
	
	if (argc <= 1) {
		printf("Using format: %s input_instance [-l]\n\t-l: without local search.", argv[0]);
		return FALSE;
	}
	for (i=0; i<WORD_LENGTH; i++)
		saved_input_file[i]=argv[1][i];
	
	//a_tms = ( struct tms *) malloc( sizeof (struct tms));
	//mess=times(a_tms); begintime = a_tms->tms_utime;
	printf("c ----------------------------\n");
	printf("c - Weighted Partial MaxSATZ -\n");
	printf("c ----------------------------\n");
	#ifdef DEBUG
		printf("c DEBUG mode ON\n");
	#endif
	
	switch (build_simple_sat_instance(argv[1])) {
	case FALSE:
		printf("ERROR: Input file error\n");
		return FALSE;
	case TRUE:
		UB = HARD_WEIGHT;
		if (argc < 3 || strcmp(argv[2], "-l") != 0)
			ubcsat(argv[1]);
		else
			printf("c Without local search.\n");
		printf("o %lli\n", UB);
		if (UB != 0) {
			init();
			dpl();
		}
		break;
	case NONE:
		printf("An empty resolvant is found!\n"); break;
	}
	//mess=times(a_tms); endtime = a_tms->tms_utime;
	
	printf("c Learned clauses = %i\n", INIT_BASE_NB_CLAUSE - BASE_NB_CLAUSE);
	printf("c NB_MONO= %lli, NB_BRANCHE= %lli, NB_BACK= %lli \n", NB_MONO, NB_BRANCHE, NB_BACK);
	if (UB >= HARD_WEIGHT) {
		printf("s UNSATISFIABLE\n");
	} else {
		printf("s OPTIMUM FOUND\nc Optimal Solution = %lli\n", UB);
		printf("v");
		for (i = 0; i < NB_VAR; i++) {
			if (var_best_value[i] == FALSE)
				printf(" -%i", i + 1);
			else
				printf(" %i", i + 1);
		}
		printf(" 0\n");
	}
	
//printf ("Program terminated in %5.3f seconds.\n",
//	  ((double)(endtime-begintime)/CLK_TCK));

  //fp_time = fopen("timetable", "a");
  //fprintf(fp_time, "maxsatz14bis+fl %s %5.3f %ld %ld %d %d %d %d\n", 
//	  saved_input_file, ((double)(endtime-begintime)/CLK_TCK), 
	//  NB_BRANCHE, NB_BACK,  
	  //UB, NB_VAR, INIT_NB_CLAUSE, NB_CLAUSE-INIT_NB_CLAUSE);
  //printf("maxsatz14bis+fl %s %5.3f %ld %ld %d %d %d %d\n", 
//	 saved_input_file, ((double)(endtime-begintime)/CLK_TCK), 
//	 NB_BRANCHE, NB_BACK,
//	 UB, NB_VAR, INIT_NB_CLAUSE, NB_CLAUSE-INIT_NB_CLAUSE);
 // fclose(fp_time);
	return TRUE;
}
