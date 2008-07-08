#include <unistd.h>
#include "MiniSat.h"
#include "Sort.h"
#include <cmath>

namespace MiniSat {
//=================================================================================================
// Debug:


// For derivation output (verbosity level 2)
#define L_IND    "%-*d"
#define L_ind    decisionLevel()*3+3,decisionLevel()
#define L_LIT    "%sx%d"
#define L_lit(p) sign(p)?"~":"", var(p)

// Just like 'assert()' but expression will be evaluated in the release version as well.
inline void check(bool expr) { assert(expr); }


//=================================================================================================
// Helpers:


void removeWatch(vec<Clause *>& ws, Clause * elem)
{
    int j = 0;
    for (; ws[j] != elem  ; j++) assert(j < ws.size());
    for (; j < ws.size()-1; j++) ws[j] = ws[j+1];
    ws.pop_();
}


//=================================================================================================
// Operations on clauses:

// Returns FALSE if top-level conflict detected (must be handled); TRUE otherwise.
// 'out_clause' may be set to NULL if clause is already satisfied by the top-level assignment.
//

bool Solver::clauseCreation(const vec<Lit>& ps_, bool learnt, Clause*& out_clause,Int w,bool h,bool tl)
{
    vec<Lit>    qs;
    if (&out_clause != NULL) out_clause = NULL;
    
    if (!learnt){
        assert(decisionLevel() == 0);
        ps_.copyTo(qs);             // Make a copy of the input vector.

        // Remove false literals:
        for (int i = 0; i < qs.size();){
            if (value(qs[i]) != l_Undef){
                if (value(qs[i]) == l_True)
                    return true;    // Clause always true -- don't add anything.
                else
                    qs[i] = qs.last(),
                    qs.pop();
            }else
                i++;
        }

        // Remove duplicates:
        sortUnique(qs);
        for (int i = 0; i < qs.size()-1; i++){
            if (qs[i] == ~qs[i+1])
                return true;        // Clause always true -- don't add anything.
        }
    }
    const vec<Lit>& ps = learnt ? ps_ : qs;     // 'ps' is now the (possibly) reduced vector of literals.

    if (ps.size() == 0)
        return false;
    else if (ps.size() == 1 and h) // It is a unit HARD clause
        return enqueue(ps[0]);
    else{
        // Allocate clause:
        assert(sizeof(Lit)   == sizeof(unsigned));
        assert(sizeof(float) == sizeof(unsigned));
        void*   mem = xmalloc<char>(sizeof(Clause) + sizeof(unsigned)*(ps.size() + (int)learnt));
        Clause* c   = new (mem) Clause(learnt,ps,w,h);

        // For learnt clauses only:
        if (learnt){
            // Put the second watch on the literal with highest decision level:
            int     max_i = 1;
            int     max   = level[var(ps[1])];
            for (int i = 2; i < ps.size(); i++)
                if (level[var(ps[i])] > max)
                    max   = level[var(ps[i])],
                    max_i = i;
            (*c)[1]     = ps[max_i];
            (*c)[max_i] = ps[1];

            // Bumping:
            claBumpActivity(c); // (newly learnt clauses should be considered active)
            for (int i = 0; i < ps.size(); i++) {
			if(opt_heur==2) varBumpActivity(ps[i]);
		}

            stats.learnts++;
            stats.learnts_literals += c->size();
        }else{
            stats.clauses++;
            stats.clauses_literals += c->size();
        }

        // Store clause:
	
	
 if(ps.size() > 1) // Else, it is a soft unary clause!!
 {
     if(tl) {
     watches[index(~(*c)[0])].push_();
     watches[index(~(*c)[0])][watches[index(~(*c)[0])].size()-1]=c;
     watches[index(~(*c)[1])].push_();
     watches[index(~(*c)[1])][watches[index(~(*c)[1])].size()-1]=c;
     }
 }

        if (&out_clause != NULL) out_clause = c;

        return true;
    }
     
}

bool Solver::newClause(const vec<Lit>& ps_, bool learnt, Clause*& out_clause,bool tl)
{
 return clauseCreation(ps_,learnt, out_clause,Int(1),true,tl);
}

bool Solver::newClause(const vec<Lit>& ps_, bool learnt, Clause*& out_clause,Int w,bool h,bool tl)
{
 return clauseCreation(ps_,learnt, out_clause,w,h,tl);
}

void Solver::remove(Clause* c, bool just_dealloc)
{
    if (!just_dealloc)
    {
            removeWatch(watches[index(~(*c)[0])], c);
            removeWatch(watches[index(~(*c)[1])], c);
    }
    if (c->learnt()){
        stats.learnts--;
        stats.learnts_literals -= c->size();
    }else{
        stats.clauses--;
        stats.clauses_literals -= c->size();
    }

    xfree(c);
}


// Can assume everything has been propagated! (esp. the first two literals are != l_False, unless
// the clause is binary and satisfied, in which case the first literal is true)
// Returns True if clause is satisfied (will be removed), False otherwise.
//
bool Solver::simplify(Clause* c) const
{
    assert(decisionLevel() == 0);
    for (int i = 0; i < c->size(); i++){
        if (value((*c)[i]) == l_True)
            return true;
    }
    return false;
}



//=================================================================================================
// Minor methods:


// Creates a new SAT variable in the solver. If 'decision_var' is cleared, variable will not be
// used as a decision variable (NOTE! This has effects on the meaning of a SATISFIABLE result).
//
Var Solver::newVar(bool dvar)
{
    int     index;
    index = nVars();
    watches .push();          // (list for positive literal)
    watches .push();          // (list for negative literal)
    reason  .push(NULL);
    assigns .push(toInt(l_Undef));
    level   .push(-1);
    level_up.push(0);
    activity.push(0);
    polarity_sug.push(toInt(l_Undef));
    order   .newVar(dvar);
    //orderS  .newVar(dvar);
    analyze_seen.push(0);
    trail.capacity(index+1);
    
    return index;
}



// Returns FALSE if immediate conflict.
bool Solver::assume(Lit p) {
    //if (verbosity >= 2)
    //printf(L_IND"assume("L_LIT") %g\n", L_ind, L_lit(p), activity[var(p)]);

    if(not allHard)
    {
 trail_dec.push_();
 trail_dec[trail_dec.size()-1].lit=index(p);
 trail_dec[trail_dec.size()-1].values=1;
    }

    trail_lim.push(trail.size());
    return enqueue(p); }


// Revert to the state at given level.
void Solver::cancelUntil(int level) {
    if (decisionLevel() > level){
        for (int c = trail.size()-1; c >= trail_lim[level]; c--){
            Var     x  = var(trail[c]);
            assigns[x] = toInt(l_Undef);
     
     if(not allHard) restoreAssumption(index(trail[c]));

            reason [x] = NULL;    
            order.undo(x);
	}
	trail.shrink_(trail.size() - trail_lim[level]);
	trail_lim.shrink(trail_lim.size() - level);
	if(not allHard) trail_dec.shrink_(trail_dec.size() - level);
	

        qhead = trail.size(); 
	
	} }


// Record a clause and drive backtracking. 'clause[0]' must contain the asserting literal.
//
void Solver::record(const vec<Lit>& clause)
{
    assert(clause.size() != 0);
    Clause* c;
    check(newClause(clause, true, c,true));
    assert(ok);
    check(enqueue(clause[0], c));
    if (c != NULL) learnts.push(c);
}


//=================================================================================================
// Major methods:


#define ANALYZE_LIT(p) \
            if (!seen[var(q)] && level[var(q)] > 0){                \
                if(opt_heur==2) varBumpActivity(q);                 \
                seen[var(q)] = 1;                                   \
                if (level[var(q)] == decisionLevel())               \
                    pathC++;                                        \
                else{                                               \
                    out_learnt.push(q);                             \
                    out_btlevel = max(out_btlevel, level[var(q)]);  \
                }                                                   \
            }
/*_________________________________________________________________________________________________
|
|  analyze : (confl : Clause*) (out_learnt : vec<Lit>&) (out_btlevel : int&)  ->  [void]
|  
|  Description:
|    Analyze conflict and produce a reason clause.
|  
|    Pre-conditions:
|      * 'out_learnt' is assumed to be cleared.
|      * Current decision level must be greater than root level.
|  
|    Post-conditions:
|      * 'out_learnt[0]' is the asserting literal at level 'out_btlevel'.
|  
|  Effect:
|    Will undo part of the trail, upto but not beyond the assumption of the current decision level.
|________________________________________________________________________________________________@*/
void Solver::analyze(Clause* _confl, vec<Lit>& out_learnt, int& out_btlevel)
{
    Clause * confl = _confl;
    vec<char>&     seen  = analyze_seen;
    int            pathC = 0;
    Lit            p     = lit_Undef;

    // Generate conflict clause:
    //
    out_learnt.push();      // (leave room for the asserting literal)
    out_btlevel = 0;
    int ind = trail.size()-1;
    do{
        assert(confl != NULL);          // (otherwise should be UIP)

            Clause& c = *confl;
     
            if (c.learnt())
                claBumpActivity(&c);

            for (int j = p == lit_Undef ? 0 : 1; j < c.size(); j++){
                Lit q = c[j];
                ANALYZE_LIT(p);
            }

        // Select next clause to look at:
        while (!seen[var(trail[ind--])]);
        p     = trail[ind+1];
        confl = reason[var(p)];
        seen[var(p)] = 0;
        pathC--;

    }while (pathC > 0);
    out_learnt[0] = ~p;

    /*if (opt_branch_pbvars){ ... <code here Niklas> }*/

  #if 1
    // Simplify conflict clause (a bit more):
    //
    int i,j;
    unsigned int minl = 0;
    for (i = 1; i < out_learnt.size(); i++)
        minl |= 1 << (level[var(out_learnt[i])] & 31);

    out_learnt.copyTo(toclear);
    for (i = j = 1; i < out_learnt.size(); i++)
        if (reason[var(out_learnt[i])] == NULL || !removable(out_learnt[i],minl))
            out_learnt[j++] = out_learnt[i];
  #else
    // Simplify conflict clause (a little):
    //
    int i,j;
    out_learnt.copyTo(toclear);
    for (i = j = 1; i < out_learnt.size(); i++){
        Clause * r = reason[var(out_learnt[i])];
        if (r == NULL)
            out_learnt[j++] = out_learnt[i];
	else{
            Clause& c = *r;
            for (int k = 1; k < c.size(); k++)
                if (!seen[var(c[k])] && level[var(c[k])] != 0){
                    out_learnt[j++] = out_learnt[i];
                    break; }
        }
    }
  #endif

    stats.max_literals += out_learnt.size();
    out_learnt.shrink(i - j);
    stats.tot_literals += out_learnt.size();

    for (int j = 0; j < toclear.size(); j++) seen[var(toclear[j])] = 0;    // ('seen[]' is now cleared)

    //printf(L_IND"Learnt {", L_ind);
    //for (int i = 0; i < out_learnt.size(); i++) printf(" "L_LIT, L_lit(out_learnt[i]));
    //printf(" } at level %d\n", out_btlevel);
}

/* Document me!
 */
#define REMOVABLE_LIT(p) \
            if (!analyze_seen[var(p)] && level[var(p)] != 0)                                             \
                if (reason[var(p)] != NULL && ((1 << (level[var(p)] & 31)) &  minl) != 0){   \
                    analyze_seen[var(p)] = 1;                                                            \
                    stack.push(p);                                                                       \
                    toclear.push(p);                                                                     \
                }else{                                                                                   \
                    for (int j = top; j < toclear.size(); j++)                                           \
                        analyze_seen[var(toclear[j])] = 0;                                               \
                    toclear.shrink_(toclear.size() - top);                                               \
                    return false;                                                                        \
                }

bool Solver::removable(Lit l, unsigned int minl)
{
    assert(reason[var(l)] != NULL);
    stack.clear_(); stack.push(l);
    int top = toclear.size();
    while (stack.size() > 0){
        assert(reason[var(stack.last())] != NULL);
        Clause * r = reason[var(stack.last())]; stack.pop();
            Clause& c = *r;
            for (int i = 1; i < c.size(); i++)
                REMOVABLE_LIT(c[i]);
    }

    return true;
}


/*_________________________________________________________________________________________________
|
|  enqueue : (p : Lit) (from : Clause *)  ->  [bool]
|  
|  Description:
|    Puts a new fact on the propagation queue as well as immediately updating the variable's value.
|    Should a conflict arise, FALSE is returned.
|  
|  Input:
|    p    - The fact to enqueue
|    from - [Optional] Fact propagated from this (currently) unit clause. Stored in 'reason[]'.
|           Default value is NULL (no reason).
|  
|  Output:
|    TRUE if fact was enqueued without conflict, FALSE otherwise.
|________________________________________________________________________________________________@*/
bool Solver::enqueue(Lit p, Clause * from,int l_up)
{
    if (value(p) != l_Undef){
        return value(p) != l_False;
    }else{
        //printf(L_IND"bind("L_LIT")\n", L_ind, L_lit(p));
        // New fact -- store it.
        assigns[var(p)] = toInt(lbool(!sign(p)));
        level  [var(p)] = decisionLevel();
        reason [var(p)] = from;
 level_up[var(p)] = l_up;
        trail.push_(p);
 if(allHard)
 {
  reasons.insertInfo(index(p),from);
 }
        return true;
    }
}

/*_________________________________________________________________________________________________
|
|  propagate : [void]  ->  [Clause*]
|  
|  Description:
|    Propagates all enqueued facts. If a conflict arises, the conflicting clause is returned,
|    otherwise NULL.
|  
|    Post-conditions:
|      * the propagation queue is empty, even if there was a conflict.
|________________________________________________________________________________________________@*/
Clause* Solver::propagate(void)
{
    
    Clause* confl = NULL;
    while (qhead < trail.size()){
        //stats.propagations++;
        Lit                   p  = trail[qhead++];     // 'p' is enqueued fact to propagate.
        vec<Clause *>&  ws = watches[index(p)];
        Clause       **i  = (Clause **)ws, **j = i, **end = i + ws.size();
 array_rest[index(p)].prevLB=LB;
 LB+=NC[index(~p)];

        for (;;) {
        next:

            if (i == end) break;
     
     else{
                Clause& c = **i; i++;
  if(not c.isUsed())
  {
  
	// Make sure the false literal is data[1]:
	Lit false_lit = ~p;
	if (c[0] == false_lit)
	c[0] = c[1], c[1] = false_lit;

	assert(c[1] == false_lit);

	// If 0th watch is true, then clause is already satisfied.
	Lit   first = c[0];
	lbool val   = value(first);
	if (val == l_True){
		*j++ = &c;
		goto next;
	}
	else{
		// Look for new watch:
		for (int k = 2; k < c.size(); k++)
		if (value(c[k]) != l_False){
		c[1] = c[k]; c[k] = false_lit;
		watches[index(~c[1])].push_();
		watches[index(~c[1])][watches[index(~c[1])].size()-1]=&c;
		goto next; }
		
		// Clause is unit under assignment:
		*j++ = &c;
		if(c.isHard()) { // if is hard, 
		
			if (!enqueue(first, &c)){
				confl = &c;
				qhead = trail.size();
				// Copy the remaining watches:
				while (i < end)
				*j++ = *i++;
			}
		}
		else {   // SOFT CLAUSE
			if(value(first)!=l_Undef) {
				// ESTO NO PASA NUNCA (EN TEORIA :p )
				if(value(first)==l_False) LB+=c.getWeight();
				
				c.setUsed(true);
				array_rest[index(p)].pushCla(&c);
			}
			else createUnaryClauseAndNC(p,first,&c);
		}
		//else goto next;
	}
  
  }
  else *j++ = &c;
            }
        }
        //stats.inspects += j - (LitClauseUnion*)ws;
        ws.shrink_(i - j);
    }
    //reportf("END %d \n",decisionLevel());
       
    return confl;
}

/*_________________________________________________________________________________________________
|
|  reduceDB : ()  ->  [void]
|  
|  Description:
|    Remove half of the learnt clauses, minus the clauses locked by the current assignment. Locked
|    clauses are clauses that are reason to a some assignment.
|________________________________________________________________________________________________@*/
struct reduceDB_lt { bool operator () (Clause* x, Clause* y) { return x->size() > 2 && (y->size() == 2 || x->activity() < y->activity()); } };
void Solver::reduceDB(void)
{
    int     i, j;
    double  extra_lim = cla_inc / learnts.size();    // Remove any clause below this activity

    sort(learnts, reduceDB_lt());
    for (i = j = 0; i < learnts.size() / 2; i++){
        if (learnts[i]->size() > 2 && !locked(learnts[i]))
            remove(learnts[i]);
        else
            learnts[j++] = learnts[i];
    }
    for (; i < learnts.size(); i++){
        if (learnts[i]->size() > 2 && !locked(learnts[i]) && learnts[i]->activity() < extra_lim)
            remove(learnts[i]);
        else
            learnts[j++] = learnts[i];
    }
    learnts.shrink_(i - j);
}


/*_________________________________________________________________________________________________
|
|  simplifyDB : [void]  ->  [bool]
|  
|  Description:
|    Simplify all constraints according to the current top-level assigment (redundant constraints
|    may be removed altogether).
|________________________________________________________________________________________________@*/
void Solver::simplifyDB(void)
{
    if (!ok) return;    // GUARD (public method)
    assert(decisionLevel() == 0);

    if (propagate() != NULL){
        ok = false;
        return; }
    if (nAssigns() == last_simplify)
        return;

    last_simplify = nAssigns();
    for (int type = 0; type < 2; type++){
        vec<Clause*>& cs = type ? learnts : clauses;

        int     j = 0;
        for (int i = 0; i < cs.size(); i++){
            if (simplify(cs[i]))
                remove(cs[i]);
            else
                cs[j++] = cs[i];
        }
        cs.shrink_(cs.size()-j);
    }
}

int Solver::undoChronological(void)
{
	int level;
	bool cont=true;
	bool first=true;
	
	// undo all decisions and unit propagations until we find a variable such that only one value has been assumed
	
	level = decisionLevel()-1;
	
	
	// MEGA CAMBIO: Posible problema cuando level==-1
	if(level==-1) return level;
	
	
	do {	
		for (int c = trail.size()-1; c >= trail_lim[level]; c--){
			Var     x  = var(trail[c]);
			assigns[x] = toInt(l_Undef);
			reason [x] = NULL;
			restoreAssumption(index(trail[c]));
			order.undo(x);
		}
		
		trail.shrink_(trail.size() - trail_lim[level]);
		trail_lim.shrink(1);
		qhead = trail.size();
		
		if((trail_dec.size()-1)<0 or trail_dec[trail_dec.size()-1].values==1) {
			cont=false;
		}
		else {
			if(first) {
				lecoutre=trail_dec[trail_dec.size()-1].lit;
				first=false;
			}
			trail_dec.shrink_(1);
   			level--;
   
  		}
	
  
 	}
 	while(cont && level>-1);
 	
 	return level;
}


/*_________________________________________________________________________________________________
|
|  search : (nof_conflicts : int) (nof_learnts : int) (params : const SearchParams&)  ->  [lbool]
|  
|  Description:
|    Search for a model the specified number of conflicts, keeping the number of learnt clauses
|    below the provided limit. NOTE! Use negative value for 'nof_conflicts' or 'nof_learnts' to
|    indicate infinity.
|  
|  Output:
|    'l_True' if a partial assigment that is consistent with respect to the clauseset is found. If
|    all variables are decision variables, this means that the clause set is satisfiable. 'l_False'
|    if the clause set is unsatisfiable. 'l_Undef' if the bound on number of conflicts is reached.
|________________________________________________________________________________________________@*/
lbool Solver::search(int nof_conflicts, int nof_learnts, const SearchParams& params)
{
    //int sug=-1;
    // search_state = 1 CONFLICT DIRECTED BACKTRACKING (as old minisat), 2=CHRONOLOGICAL BACKTRACKING
    if (!ok) return l_False;    // GUARD (public method)
    assert(root_level == decisionLevel());

    stats.starts++;
    int     conflictC = 0;
    var_decay = 1 / params.var_decay;
    cla_decay = 1 / params.clause_decay;
    model.clear();

    search_state = 1;
    
    for (;;){
 if(search_state == 2)
 { 
  search_state=1;
  int res=undoChronological();
  if(res<root_level)
  {
                 //return l_False;
   restoreAssumption(indexRest());
   return l_True;
  }
  
  
	// Assume 
	Lit next;
	stats.decisions++;
	next=~Lit::toLit(trail_dec[trail_dec.size()-1].lit);
	trail_dec[trail_dec.size()-1].lit=index(next);
	trail_dec[trail_dec.size()-1].values=0;
	trail_lim.push(trail.size());
	check(enqueue(next));
	}
	
        else 
	{
	Clause* confl=propagate();
	
	if(confl !=NULL and not learn) {
		// To update the branching heuristic!
		vec<Lit>    learnt_clause;
        	int         backtrack_level;
		analyze(confl, learnt_clause, backtrack_level);
		search_state=2; 
		}
		
	if(LB>=UB) search_state=2;
	
	if(confl==NULL)
	{
		Lit pick = lit_Undef;
		if(not order.empty() and opt_sat==0) sug=pruneValues();
		else sug=-1;
		
		if(opt_lc==1 and sug==-1)
		{
			Int lb=applyLC();
			if(lb>=UB) {
				// MEGA CAMBIO
				//sug=-1;
				search_state=2;
			}
		}
	}
	
        if (confl != NULL and search_state==1){
            // CONFLICT

            // EXPERIMENTAL
		trail_copy.clear();
		stats.conflicts++; conflictC++;
		vec<Lit>    learnt_clause;
		int         backtrack_level;
		if (decisionLevel() == root_level) {
			restoreAssumption(indexRest());
			return l_False;
		}
	
		analyze(confl, learnt_clause, backtrack_level);
		if(opt_learn==0) search_state=2; 
		else {
			assert(backtrack_level < decisionLevel());
			cancelUntil(max(backtrack_level, root_level));
			record(learnt_clause);
			varDecayActivity(); 
			claDecayActivity();
		}		
		
        }
	else if(sug!=-1 and search_state==1){
		search_state=1;
		// Assume 
		Lit next=Lit::toLit(sug);
		
		stats.decisions++;
		trail_dec.push_();
		trail_dec[trail_dec.size()-1].lit=index(next);
		trail_dec[trail_dec.size()-1].values=0;
		trail_lim.push(trail.size());
		check(enqueue(next));
	}
	else if(search_state==1) {
            
	    if (decisionLevel() == 0)
                // Simplify the set of problem clauses:
                simplifyDB(), assert(ok);

            if (nof_learnts >= 0 && learnts.size()-nAssigns() >= nof_learnts)
                // Reduce the set of learnt clauses:
                reduceDB();

            // New variable decision:
            stats.decisions++;

	    	//calculateStaticOrdering();
		Lit pick = lit_Undef;
	
		Var next = order.select(params.random_var_freq);

		if (next == var_Undef)
		{
			//previous=LB;
			
			resultado=true;
			UB=LB;
			model.growTo(nVars());
			
			for (int i = 0; i < nVars(); i++) {
				model[i] = value(i);
			}

			printf("o %d \n",toint(UB));
			fflush(stdout);
	
			search_state=2;
		
			if(UB==0)
			{    
				cancelUntil(root_level);
				return l_True;
			}
		}
  else
  {
  
  if(opt_sat==0 and bNC[next])
  {
   if(NC[index(Lit(next))]>=NC[index(~Lit(next))]) check(assume(Lit(next)));
   else check(assume(~Lit(next)));
  }
  else
  {
  if (polarity_sug[next] == toInt(l_False))
  {
   check(assume(Lit(next)));
  }
  else if (polarity_sug[next] == toInt(l_True))
  {
   check(assume(Lit(next)));
  }
  else
  {
   check(assume(Lit(next)));  // Arbitrarly default to negative polarity...
  }
  }
  }
 }
 
 }
    } // end for
}



// Return search-space coverage. Not extremely reliable.
//
double Solver::progressEstimate(void)
{
    double  progress = 0;
    double  F = 1.0 / nVars();
    for (int i = 0; i < nVars(); i++)
        if (value(i) != l_Undef)
            progress += pow(F, level[i]);
    return progress / nVars();
}


// Divide all variable activities by 1e100.
//
void Solver::varRescaleActivity(void)
{
    for (int i = 0; i < nVars(); i++)
        activity[i] *= 1e-100;
    var_inc *= 1e-100;
}


// Divide all constraint activities by 1e100.
//
void Solver::claRescaleActivity(void)
{
    for (int i = 0; i < learnts.size(); i++)
        learnts[i]->activity() *= 1e-20;
    cla_inc *= 1e-20;
}


void Solver::showClauses()
{
    for (int i = 0; i < clauses.size(); i++)
    {
            if(clauses[i]->isHard())
	    {
	    	reportf(" %d HARD %d %d\n",i,clauses[i]->size(),toint(clauses[i]->getWeight()));
	    }
	    else
	    {
	    	reportf(" %d SOFT %d %d\n",i,clauses[i]->size(),toint(clauses[i]->getWeight()));
	    }

    }
}

/*_________________________________________________________________________________________________
|
|  solve : (assumps : const vec<Lit>&)  ->  [bool]
|  
|  Description:
|    Top-level solve. If using assumptions (non-empty 'assumps' vector), you must call
|    'simplifyDB()' first to see that no top-level conflict is present (which would put the solver
|    in an undefined state).
|________________________________________________________________________________________________@*/

bool Solver::solve(const vec<Lit>& assumps)
{
    createMaxSATStructures();
    //simplifyDB();
    if (opt_wcnf != NULL) {
    	exportClauses2(opt_wcnf);
    	return false;
    }
    if(!ok) {
    	reportf("RES: UNSAT %g %d\n",cpuTime(),stats.decisions);
    }
    if (!ok) return false;
    
    
    SearchParams    params(0.95, 0.999, 0.02);
    double  nof_conflicts = 100;
    double  nof_learnts   = nClauses() / 3;
    //double  nof_learnts   = 4000;
    lbool   status        = l_Undef;

    //double random_seed=91648253;
    //Var next = irand(random_seed,2);
    
    for (int i = 0; i < assumps.size(); i++)
      if (!assume(assumps[i]) || propagate() != NULL){
	cancelUntil(0);
	/*
	  if(opt_sat==0) reportf("RES: %d %g %d\n",toint(UB),cpuTime(),stats.decisions);
	  else reportf("RES: U %g %d\n",cpuTime(),stats.decisions);
	*/
	
	reportf("RES: UNSAT %g %d\n",cpuTime(),stats.decisions);
	return false; }
    root_level = decisionLevel();
    
    
    while (status == l_Undef){
      learn=true;
      /*
      if(opt_ls==1) {
	localSearch( 2, 10, 12345 );
	if(topUB==-1) resultado=true; // UB es una solucion
	if(topUB!=-1 and UB < topUB) resultado=true; // UB es una solucion
      }
      */
      
      if(topUB!=-1 and topUB < UB) UB = topUB;
      
      if(opt_upper>-1) {
	UB = opt_upper;
	printf("o %d \n",toint(UB));
	fflush(stdout);
	resultado = true;
	model.growTo(nVars());
	for(int j=0;j<nVars();j++)
	  {
	    model[j]=l_True;
	  }
      }
      
      DoHardening();
      status = search((int)nof_conflicts, (int)nof_learnts, params);
      nof_conflicts *= 1.5;
      nof_learnts   *= 1.1;
    }
    
    //showNCTable();
    if (verbosity >= 1)
        reportf("==============================================================================\n");

    cancelUntil(0);
    /*
    if(opt_sat==0) reportf("RES: %d %g %d\n",toint(UB),cpuTime(),stats.decisions);
    else {
	if(toint(UB)==0) reportf("RES: S %g %d\n",cpuTime(),stats.decisions);
	else reportf("RES: U %g %d\n",cpuTime(),stats.decisions);
    }
    */

    //if(status == l_True)
    if(resultado)
    {     
	reportf("RES: %d %g %d\n",toint(UB),cpuTime(),stats.decisions);
	printf("s OPTIMUM FOUND\n");
	printf("v ");
	for (int i = 0; i < nVars(); i++) {
		if(model[i]==l_True) printf("%d ",i+1);
		else printf("-%d ",i+1);
	}
	printf("\n");
	fflush(stdout);
    }
    else reportf("RES: UNSAT %g %d\n",cpuTime(),stats.decisions);
        
    return status == l_True;
}


//=================================================================================================
// Debug:

void Solver::countClauses(int & n_vars, int & n_clauses)
{
    assert(decisionLevel() == 0);
    n_vars = -1, n_clauses = 0;
    
    for (int i = 0; i < assigns.size(); i++)
        //if (value(i) != l_Undef && level[i] == 0 && reason[i].isNull())
 if (value(i) != l_Undef && level[i] == 0)
            n_vars = i+1, n_clauses++;
    for (int i = 0; i < clauses.size(); i++){
        Clause& c = *clauses[i];
        for (int j = 0; j < c.size(); j++){
            if (var(c[j])+1 > n_vars)
                n_vars = var(c[j])+1; }
        n_clauses++;
    }
}

void Solver::exportClauses2(cchar* filename){
 int cont=0;
 Int maxw=0;
     for(int i=0;i<nVars();i++) {
  if(NC[index(Lit(i))]>0) cont++;
  if(NC[index(~Lit(i))]>0) cont++;
  maxw+=NC[index(Lit(i))] + NC[index(~Lit(i))];
 }
 for (int i = 0; i < clauses.size(); i++){
  Clause& c = *clauses[i];
  if(not c.isUsed()) cont++;
  if(not c.isHard()) maxw+=c.getWeight();
 }
 for (int i = 0; i < comps.size(); i++){
  Clause& c = *comps[i];
  if(not c.isUsed()) {
   if(not c.isHard()) maxw+=c.getWeight();
   cont++;
  }
 }
 if(LB>=0) cont+=2;
 FILE*   out = fopen(filename, "wb"); assert(out != NULL);
 
 reportf("Exporting WCNF to: \b%s\b\n", filename);
 //fprintf(out,"c PSEUDOBOOLEAN %d \n",toint(UB));
 fprintf(out,"p wcnf %d %d %d\n",nVars(),cont,toint(maxw));

     for(int i=0;i<nVars();i++) {
  if(NC[index(Lit(i))]>0) fprintf(out,"%d %d 0\n",toint(NC[index(Lit(i))]),i+1);
  if(NC[index(~Lit(i))]>0) fprintf(out,"%d -%d 0\n",toint(NC[index(~Lit(i))]),i+1); 
 }
 for (int i = 0; i < clauses.size(); i++){
  Clause& c = *clauses[i];
  if(not c.isUsed()) {
   if(c.isHard()) fprintf(out,"%d ",toint(maxw));
   else fprintf(out,"%d ",toint(c.getWeight()));
   for (int j = 0; j < c.size(); j++)
    fprintf(out,"%s%d ", sign(c[j])?"-":"", var(c[j])+1);
   fprintf(out,"0\n");
  }
 }
 for (int i = 0; i < comps.size(); i++){
  Clause& c = *comps[i];
  if(not c.isUsed()) {
   if(c.isHard()) fprintf(out,"%d ",150);
   else fprintf(out,"%d ",toint(c.getWeight()));
   for (int j = 0; j < c.size(); j++)
    fprintf(out,"%s%d ", sign(c[j])?"-":"", var(c[j])+1);
   fprintf(out,"0\n");
  }
 }
 if(LB>0) {
  fprintf(out,"%d 1 0\n",toint(LB));
  fprintf(out,"%d -1 0\n",toint(LB));
 }
 fclose(out);

}

void Solver::exportClauses(cchar* filename)
{
    assert(decisionLevel() == 0);
    FILE*   out = fopen(filename, "wb"); assert(out != NULL);

    // HACK: Find biggest variable index used and count the number of clauses:
    int     n_vars = -1, n_clauses = 0;
    for (int i = 0; i < assigns.size(); i++)
        //if (value(i) != l_Undef && level[i] == 0 && reason[i].isNull())
	if (value(i) != l_Undef && level[i] == 0)
            n_vars = i+1, n_clauses++;
    for (int i = 0; i < clauses.size(); i++){
        Clause& c = *clauses[i];
        for (int j = 0; j < c.size(); j++){
            if (var(c[j])+1 > n_vars)
                n_vars = var(c[j])+1; }
        n_clauses++;
    }
    fprintf(out, "p cnf %d %d\n", n_vars, n_clauses);

    // Export CNF:
    for (int i = 0; i < assigns.size(); i++)
        //if (value(i) != l_Undef && level[i] == 0 && reason[i].isNull())
 if (value(i) != l_Undef && level[i] == 0)
            fprintf(out, "%d 0\n", (value(i) == l_True) ? i+1 : -(i+1));

    for (int i = 0; i < clauses.size(); i++){
        Clause& c = *clauses[i];
        for (int j = 0; j < c.size(); j++)
            fprintf(out, "%s%d ", sign(c[j])?"-":"", var(c[j])+1);
        fprintf(out, "0\n");
    }
    fclose(out);
}

void Solver::addUnitSoftClauses(Lit p,Int w)
{
 //reportf("%d %d\n",var(p),toint(w));
 tmp_unit_soft.push();
 tmp_unit_soft[tmp_unit_soft.size()-1].lit=index(p);
 tmp_unit_soft[tmp_unit_soft.size()-1].cost=w;  
 
}


void Solver::unitSoftClauses(vec<Lit>& ps,vec<Int>& Cs)
{
    
  for (int i = 0; i < ps.size(); i++)
  {
	tmp_unit_soft.push();
	int v=var(ps[i]);
	if(toint(Cs[i])>0)
	{
		tmp_unit_soft[tmp_unit_soft.size()-1].lit=index(~Lit(v));
		tmp_unit_soft[tmp_unit_soft.size()-1].cost=Cs[i];
	}
	else if(toint(Cs[i])<0)
	{
		tmp_unit_soft[tmp_unit_soft.size()-1].lit=index(Lit(v));
		tmp_unit_soft[tmp_unit_soft.size()-1].cost=-Cs[i];
	}
  }
  
}

// FEDE_NEW
void Solver::exportWcnfClauses(FILE*   out,int hard)
{
    // Export WCNF:
    for (int i = 0; i < assigns.size(); i++)
        //if (value(i) != l_Undef && level[i] == 0 && reason[i].isNull())
 if (value(i) != l_Undef && level[i] == 0)
            fprintf(out, "%d %d 0\n",hard, (value(i) == l_True) ? i+1 : -(i+1));

    for (int i = 0; i < clauses.size(); i++){
        Clause& c = *clauses[i];
 fprintf(out, "%d ",hard);
        for (int j = 0; j < c.size(); j++)
            fprintf(out, "%s%d ", sign(c[j])?"-":"", var(c[j])+1);
        fprintf(out, "0\n");
    }
}

/************************************** MAX_SAT FUNCTIONS *************************************/
// It applies propagation considering all clauses hard.
Clause* Solver::propagateMaxSAT(int limit_up)
{
    Clause* confl = NULL;
    bool final=false;
    int qhead2=trail.size();
    int qheadMax=trail.size();
    while (not final){
    //while (qhead < trail.size()){
        stats.propagations++;
 Lit p;
 if(qhead2<trail.size()) { 
  p  = trail[qhead2++];     // 'p' is enqueued fact to propagate.
 }
 else {
  if(qhead<qheadMax) {
   p  = trail[qhead++];
  }
  else return confl;
 }
        //Lit                   p  = trail[qhead++];     // 'p' is enqueued fact to propagate.
 if(level_up[var(p)]==limit_up) {
  return confl; // We have reached the limit
 }
 
        vec<Clause *>&  ws = watches[index(p)];
        Clause       **i  = (Clause **)ws, **j = i, **end = i + ws.size();

        for (;;) {
        next:
     
            if (i == end) break;
     
     else{
                Clause& c = **i; i++;
  
  if(not c.isUsed() and c.getMark()!=currentMark)
  {
                // Make sure the false literal is data[1]:
                Lit false_lit = ~p;
                if (c[0] == false_lit)
                    c[0] = c[1], c[1] = false_lit;

                assert(c[1] == false_lit);

                // If 0th watch is true, then clause is already satisfied.
                Lit   first = c[0];
                lbool val   = value(first);
                if (val == l_True){
                    *j++ = &c;
                    goto next;
                }else{
                    // Look for new watch:
                    for (int k = 2; k < c.size(); k++)
                        if (value(c[k]) != l_False){
                            c[1] = c[k]; c[k] = false_lit;
       watches[index(~c[1])].push_();
       watches[index(~c[1])][watches[index(~c[1])].size()-1]=&c;
                            goto next; }
                    // Clause is unit under assignment:
      *j++ = &c;
       if (!enqueue(first, &c,level_up[var(p)]+1)){
    confl = &c;
    qhead = trail.size();
    final=true;
    //reportf("CONFLICT %d\n",var(first));
    //reportf("Conflict %d\n",var(p));
    // Copy the remaining watches:
    while (i < end)
    *j++ = *i++;
      }
                }
  
  }
  else *j++ = &c;
            }
        }
        ws.shrink_(i - j);
    }
    return confl;
}

void Solver::paintClause(Clause *c1)
{
  // COMMENTED BY POCM BECAUSE IS NOT GOOD CAST
  // printf("    CLAUSE %X : ",(int)c1);
 if(c1->isUsed())printf(" used=t ");
 else printf(" used=f ");
 for(int i=0;i<c1->size();i++) {
  if(value(var((*c1)[i])) == l_Undef) {
  if(index((*c1)[i])%2==0)
   printf("%d ",var((*c1)[i]));
  else
   printf("-%d ",var((*c1)[i]));
  }
 }
 printf(" S:%d ",c1->size());
 printf(" W:%d ",toint(c1->getWeight()));
 printf(" H:%d ",toint(c1->isHard()));
 printf(" L:%d ",toint(c1->learnt()));
 if(c1->isVirtual()) printf(" W2: %d",toint(NC[index((*c1)[0])]));
 printf(" V:%d \n",toint(c1->isVirtual()));
 fflush(stdout);
}


void Solver::modifyUnitCost(Clause *c,Int cost)
{
 if(c->isVirtual()){
  if(not trans_now) {
   changeUnaryCost((*c)[0],cost);
   reasons.restNC.push_();
   reasons.restNC[reasons.restNC.size()-1].setValues(index((*c)[0]),cost);
  }
  else {
   changeUnaryCost((*c)[0],cost);
   int ind=indexRest();
   array_rest[ind].pushNC(index((*c)[0]),cost);
  }
 }
}

Clause * Solver::addSoftClause(vec<Lit> & ps,Int w,Lit p, bool hard=false)
{
 Clause *c;
 check(newClause(ps, false, c,w,hard,true));

 array_rest[index(p)].newCompClauses++;
 comps.push_();
 comps[comps.size()-1]=c;
 return c;
 
}

void Solver::generateComp(Lit & clash,vec<Lit> & cA, vec<Lit> & cB)
{
 int i,j;
 bool create=false;
 Lit p=Lit::toLit(indexRest());
 
 
 for(i=0;i<cB.size();i++)
 { 
  psAux.clear_();
  psAux.push_();
  psAux[psAux.size()-1]=clash;
  
  for(j=0;j<cA.size();j++) {
   psAux.push_(); psAux[psAux.size()-1]=cA[j];
  }

  for(j=0;j<i;j++)
  {
   psAux.push_(); psAux[psAux.size()-1]=cB[j];
  }
  psAux.push_(); psAux[psAux.size()-1]=~cB[i];
  
  create=true;
  
  sortUnique(psAux);
  for (j = 0; j < psAux.size()-1 and create; j++){
  if (psAux[j] == ~psAux[j+1])
   create=false;
  }
  
   
  if(create) {
   if(psAux.size() > maxArity) maxArity=psAux.size();
   addSoftClause(psAux,minAux,p);
  }

 }
}

void Solver::compClauses(Clause*& c1,Clause*& c2,Lit & clash)
{
 Lit p=Lit::toLit(indexRest());
 
 if(not c1->isHard()) {
  if(not c1->isVirtual() and c1->getWeight()==minAux) {
   c1->setUsed(true);
   array_rest[index(p)].pushCla(c1);
  }
  else if(not c1->isVirtual()){
   array_rest[index(p)].pushClaCost(c1,minAux);
   c1->setWeight(c1->getWeight()-minAux);
  }
 }
 if(not c2->isHard()) {
  if(not c2->isVirtual() and  c2->getWeight()==minAux) {
   c2->setUsed(true);
   array_rest[index(p)].pushCla(c2);
  }
  else if(not c2->isVirtual()){
   array_rest[index(p)].pushClaCost(c2,minAux);
   c2->setWeight(c2->getWeight()-minAux);
  }
 }

 if(not c1->isHard()) generateComp(clash,psAuxA,psAuxB);
 if(not c2->isHard()) {
  clash=~clash;
  generateComp(clash,psAuxB,psAuxA);
 }
}

// It applies resolution between c1 and c2 and returns the resolvent in c1
// "usedLit" table is used to discard existing literals
void Solver::resolution(Clause*& c1,Clause*& c2,Lit & clash,bool pre=false)
{
 psAux.clear_();
 psAuxA.clear_();
 psAuxB.clear_();
 
 auxMark++;
 
 int cont=0;
 for(int i=0;i<c1->size();i++)
 {
  if(value((*c1)[i]) == l_Undef and (*c1)[i]!=clash)
  {
   if(usedLit[index((*c1)[i])]!=auxMark){
    cont++;
    psAux.push_();
    psAux[psAux.size()-1]=(*c1)[i];
    usedLit[index((*c1)[i])]=auxMark;
   }
   psAuxA.push_();
   psAuxA[psAuxA.size()-1]=(*c1)[i];
  }
 }
 
 clash=~clash;
 
 for(int i=0;i<c2->size();i++)
 {
  if(value((*c2)[i]) == l_Undef and (*c2)[i]!=clash)
  {
   if(usedLit[index((*c2)[i])]!=auxMark){
    cont++;
    psAux.push_();
    psAux[psAux.size()-1]=(*c2)[i];
    usedLit[index((*c2)[i])]=auxMark;
   }
   psAuxB.push_();
   psAuxB[psAuxB.size()-1]=(*c2)[i];
  }
 }
 clash=~clash;
 
 if(cont==0) {
  c1=NULL;
 }
 else
 {
  Clause *&newCla=newClaAux;
  if(not pre and not trans_now) {
   // It is not the preprocessing and it does not transform the problem
   if(not c1->isVirtual() and not c1->isHard())  {
    c1->setMark(currentMark);
   }
   if(not c2->isVirtual() and not c2->isHard()) {
    c2->setMark(currentMark);
   }
  }

  if(pre or not trans_now)
  {
   // It is the preprocessing or it does not transform the problem
   newCla->setNewClause(false,psAux,minAux,true);
   c1=newCla;
  }
  else if(not pre and trans_now)
  {
   // It is not the preprocessing, and it transforms the problem.
   if(psAux.size()>1)
   {
    if(c1->isHard() and c2->isHard())
     newCla->setNewClause(false,psAux,UB,true);
    else
     newCla->setNewClause(false,psAux,minAux,false);
    
    compClauses(c1,c2,clash);
    c1=newCla;
   }
   else
   {
    if(c1->isHard() and c2->isHard()) {
     newCla->setNewClause(false,psAux,UB,true);
     sug=index((*newCla)[0]);
    }
    else
     newCla->setNewClause(false,psAux,minAux,false);
    compClauses(c1,c2,clash);
    c1=newCla;
   }
  }
 }
}

// This function returns true if c1 and c2 share a clashing variable, otherwise ir returns false.
// It saves the clashing variable in "clash"

bool Solver::clashingClauses(Clause *c1,Clause *c2,Lit & clash)
{
 
 for(int i=0;i<c1->size();i++) 
 {
  if(value((*c1)[i]) == l_Undef)
  {
   for(int j=0;j<c2->size();j++)
   {
    if(value((*c2)[j]) == l_Undef and (*c1)[i] == ~(*c2)[j]) {
     clash=(*c1)[i];
     return true;
    }
   }
  }
 }
 return false;
}

// This procedure traverses all the clauses involved in the last inconsistency, and it calculates 
// the minimum weight to propagate.  It will be used also to decide wheter it transforms the problem or not
void Solver::preDetectInconsistency(Clause*& a,bool singleton=false)
{
 
 bool end=false;
 bool clash=false;
 int pos;
 int max=0;
 Lit x;
 if(opt_trans==1 or opt_trans==2) trans_now=true;
 else trans_now=false;

 pos=reasons.size-1;
 if(a->isVirtual()) minAux=NC[index((*a)[0])];
 else minAux=getClauseWeight(a);
 
 while(not end)
 {
  Clause *& b=reasons.r_trail[pos].reason;
  // Do the two clauses clash?
  clash=clashingClauses(a,b,x);
  
  if(clash)
  {
   reasons.r_trail[pos].clash=index(x);
   if(b->isVirtual()) minAux=minimumWeight(minAux,NC[index((*b)[0])]);
   else minAux=minimumWeight(minAux,getClauseWeight(b));
   
   resolution(a,b,x,true);
   if(a!=NULL and a->size()>max) max=a->size();
   
   if(not singleton) 
   {  if(a==NULL) end=true;}
   else
   {  if(a->size()==1) { 
     end=true;
      }
   }
  }
  else reasons.r_trail[pos].clash=-1;
  pos--;
 }
 if(singleton and opt_trans>0) trans_now=true;
 else {
  
  if(opt_trans==1) trans_now=true;
  else if(opt_trans==2 and max<=opt_size) trans_now=true;
  else trans_now=false;
 }
}


void Solver::detectInconsistency(Clause*& a,bool singleton=false)
{
 
 bool end=false;
 int clash;
 int pos;
 Lit x;
 Int lb=LB;
 Clause *f=a;
 pos=reasons.size-1;
 
 preDetectInconsistency(a,singleton);
 a=f;
 
 if(not singleton) modifyUnitCost(a,-minAux);

 while(not end)
 {
  Clause *& b=reasons.r_trail[pos].reason;
  
  
  // Do the two clauses clash?
  clash=reasons.r_trail[pos].clash;
  
  if(clash!=-1)
  {
   x=Lit::toLit(clash);
   if(not singleton) modifyUnitCost(b,-minAux);
   resolution(a,b,x);
   
   if(not singleton) 
   {  if(a==NULL) end=true;}
   else
   {  if(a->size()==1) { 
     if(not trans_now){
      
      changeUnaryCost((*a)[0],minAux);
      reasons.restNC.push_();
      reasons.restNC[reasons.restNC.size()-1].setValues(index((*a)[0]),minAux);
      end=true;
     }
     else
     {
      changeUnaryCost((*a)[0],minAux);
      array_rest[indexRest()].pushNC(index((*a)[0]),minAux);
      
      Lit p=Lit::toLit(indexRest());
      executeNC((*a)[0],p);
      end=true;
     }
      }
   }
  }
  pos--;
 }
}

Int Solver::computeLB(int lev)
{
 Int tot=0;
 bool end=false;
 allHard=true;
 bool sense=true;

 while (not end)
 {
  int rest_level=max(root_level,decisionLevel());

  if(sense) {
  for(int i=0;i<nVars();i++)
  {
   
   Lit p=Lit(i,false);
   if(value(p) == l_Undef){
    if(bNC[var(p)])
    {
     if(NC[index(p)]>0)
     {
      trail_lim.push(trail.size());
      check(enqueue(p, NCLits[index(p)],1));
     }
     else if(NC[index(~p)]>0)
     {
      trail_lim.push(trail.size());
      check(enqueue(~p, NCLits[index(~p)],1));
     }
    }
   }
  }
  }
  else {
  for(int i=nVars()-1;i>0;i--)
  {
   
   Lit p=Lit(i,false);
   if(value(p) == l_Undef){
    if(bNC[var(p)])
    {
     if(NC[index(p)]>0)
     {
      trail_lim.push(trail.size());
      check(enqueue(p, NCLits[index(p)],1));
     }
     else if(NC[index(~p)]>0)
     {
      trail_lim.push(trail.size());
      check(enqueue(~p, NCLits[index(~p)],1));
     }
    }
   }
  }
  }
  sense=not sense;
  //reportf("END\n");

  Clause *confl=propagateMaxSAT(lev);
  cancelUntil(rest_level);
  
  if(confl!=NULL) {
   detectInconsistency(confl);
   if(not trans_now) tot+=minAux;
   else LB+=minAux;
   if(sug!=-1) end=true;
  }
  else end=true;
  reasons.clean();
 }
 
 allHard=false;
 
 return tot;
}

void Solver::applyAC(int lev)
{
 Clause *confl=NULL;
 bool rep=false;
 allHard=true;
 // MEGA CAMBIO
 for(int i=0;i<nVars() and sug==-1;i++)
 {
  Lit p=Lit(i,false);
  if(value(p) == l_Undef)
  {
   int rest_level=max(root_level,decisionLevel());
   trail_lim.push(trail.size());
   check(enqueue(p, NCLits[index(p)],1));
   confl=NULL;
   confl=propagateMaxSAT(lev);
   cancelUntil(rest_level);
   if(confl!=NULL) {
    cont_ac++;
    detectInconsistency(confl,true);
    rep=true;
   }
   reasons.clean();
   
   trail_lim.push(trail.size());
   check(enqueue(~p, NCLits[index(~p)],1));
   confl=NULL;
   confl=propagateMaxSAT(lev);
   cancelUntil(rest_level);
   if(confl!=NULL) {
    cont_ac++;
    detectInconsistency(confl,true);
    rep=true;
   }
   reasons.clean();
   if(rep) {rep=false; i--;}
  }
 }
 allHard=false;
}

Int Solver::applyLC()
{
 Int tot;
 tot=0;
 
 currentMark++;
 
 	if(decisionLevel()==0) {
		
		
		//if(opt_heur==1)
		{
			if(opt_prob==1 || opt_prob==2) {
				applyAC(2);
				applyAC(3);
			}
		}
		tot+=computeLB(-1);
		calculateStaticOrdering();
		
		
		
		
	}
	else {
		if(opt_prob==2) {
			applyAC(2);
			applyAC(3);
		}

		tot+=computeLB(-1);
	}
 
	for(int i=reasons.restNC.size()-1;i>=0;i--) {
	changeUnaryCost(Lit::toLit(reasons.restNC[i].lit),-reasons.restNC[i].cost);
	reasons.restNC.pop_();
 }
 /*
 for(int i=reasons.restMarkCla.size()-1;i>=0;i--) {
  Clause *c = reasons.restMarkCla[i].clause;
  c->setWeight(c->getWeight()+reasons.restMarkCla[i].cost);
  reasons.restMarkCla.pop_();
 }
 */
 
 return tot+LB;

}

/************************ TRANSFORMATION AND RESTORATION FUNCTIONS *************************+*/

void Solver::createMaxSATStructures(void)
{
 resultado=false;
 lecoutre=-1;
 maxArity=0;
 currentMark=0;
 auxMark=0;
 cont_ac=0;
 allHard=false;
 
 LB=0;
 if (opt_goal != Int_MAX) UB=opt_goal;
 else UB=INT_MAX;

 NC = new Int[nVars()*2];
 array_heur = new double[nVars()];
 bNC = new bool[nVars()*2];
 usedLit = new int64[nVars()*2];
 reasons.createStack(nVars()*2);
 for(int i=0;i<nVars()*2;i++) { NC[i]=0;usedLit[i]=0;}
 for(int i=0;i<tmp_unit_soft.size();i++) { 
  NC[tmp_unit_soft[i].lit]+=tmp_unit_soft[i].cost;
 }
 maxPseudo=0;
 for(int i=0;i<nVars();i++)
 {
  if(NC[index(Lit(i))]>0 or NC[index(~Lit(i))]>0) 
  { bNC[i]=true; maxPseudo = maxPseudo + NC[index(Lit(i))] + NC[index(~Lit(i))];}
  else { bNC[i]=false; }
 }
 
 array_rest= new restoreAssignment[nVars()*2+1];
 
 NCLits = new Clause*[nVars()*2];
 int vVirtual;
 
 //if(tmp_unit_soft.size()==0) vVirtual=nVars();
 //else vVirtual=tmp_unit_soft.size();
 vVirtual=nVars();
 
 for(int i=0;i<vVirtual;i++) {
  Clause* c=NULL;
  vec<Lit>    tmp_unit_clause;
  
  tmp_unit_clause.push(Lit(i));
  newClause(tmp_unit_clause, false, c,0,false,true);
  if(c!=NULL)
  {
   c->setVirtual(true);
   NCLits[index(Lit(i))]=c;
  }
  
  tmp_unit_clause.pop();
  tmp_unit_clause.push(~Lit(i));
  c=NULL;
  newClause(tmp_unit_clause, false, c,0,false,true);
  if(c!=NULL)
  {
   c->setVirtual(true);
   NCLits[index(~Lit(i))]=c;
  }
  tmp_unit_clause.pop();
 }
 
 psAux.clear_();
 //for(int i=0;i<nVars()*2;i++) psAux.push(Lit::toLit(i));
 for(int i=0;i<nVars()*2;i++) { psAux.push_(); psAux[psAux.size()-1]=Lit::toLit(i); }
 void*   mem = xmalloc<char>(sizeof(Clause) + sizeof(unsigned)*(psAux.size() + (int)true));
 newClaAux = new (mem) Clause(false,psAux,0,true);
}


void Solver::changeUnaryCost(Lit l,Int c)
{
 NC[index(l)]+=c;
 if(NC[index(l)]>0 or NC[index(~l)]>0) bNC[var(l)]=true;
 else bNC[var(l)]=false;
}

void Solver::executeNC(Lit & first,Lit & p)
{
 if(NC[index(first)]>0 and NC[index(~first)]>0)
 {
  // Node Consistency
  Int min;
  if(NC[index(Lit(first))]>NC[index(~Lit(first))]) min=NC[index(~Lit(first))];
  else min=NC[index(Lit(first))];
  changeUnaryCost(Lit(first),-min);
  changeUnaryCost(~Lit(first),-min);
  
  LB+= min;
  array_rest[indexRest()].pushNC(index(Lit(first)),-min);
  array_rest[indexRest()].pushNC(index(~Lit(first)),-min);
 }
}

int Solver::pruneValues()
{
 for(int i=0;i<nVars();i++)
 {
  if(value(Lit(i)) == l_Undef and bNC[i]){
   if(NC[index(Lit(i))]+LB>=UB) {
    return index(Lit(i));
   }
   else if(NC[index(~Lit(i))]+LB>=UB) {
    return index(~Lit(i));
   }
  }
 }
 return -1;
}


void Solver::createUnaryClauseAndNC(Lit & p, Lit & first, Clause* c)
{
 c->setUsed(true);
 changeUnaryCost(Lit(first),c->getWeight());
 array_rest[index(p)].pushNC(index(first),c->getWeight());
 array_rest[index(p)].pushCla(c);
 executeNC(first,p);
}

void Solver::removeSoft(Clause* c)
{
    removeWatch(watches[index(~(*c)[0])], c);
    removeWatch(watches[index(~(*c)[1])], c);

    xfree(c);
}


void Solver::restoreAssumption(int lit)
{
 int i;
 LB=array_rest[lit].prevLB;
 for(i=array_rest[lit].restNC.size()-1;i>=0;i--)
 {
  changeUnaryCost(Lit::toLit(array_rest[lit].restNC[i].lit),-array_rest[lit].restNC[i].cost);
  array_rest[lit].restNC.pop_();
 }
 
 for(i=array_rest[lit].restCla.size()-1;i>=0;i--)
 {
  array_rest[lit].restCla[i]->setUsed(false);
  array_rest[lit].restCla.pop_();
	}
	
	for(i=array_rest[lit].restClaCost.size()-1;i>=0;i--) {
		Clause *c = array_rest[lit].restClaCost[i].clause;
		c->setWeight(c->getWeight()+array_rest[lit].restClaCost[i].cost);
		array_rest[lit].restClaCost.pop_();
	}
	
	if(array_rest[lit].newCompClauses>0)
	{
		for(i=(array_rest[lit].newCompClauses-1);i>=0;i--) {
			Clause *c= comps[comps.size()-1];
			removeSoft(c);
			comps[comps.size()-1]=NULL;
			comps.pop();
		}
		array_rest[lit].newCompClauses=0;
	}
	
	if(array_rest[lit].learnCla.size()>0) {
		for(int i=array_rest[lit].learnCla.size()-1;i>=0;i--) {
			
			Clause*c=array_rest[lit].learnCla[i];
			int pos=0;
			int cont=0;
			
			for(int j=0;j<c->size() and pos<2;j++) {
				if(value(var((*c)[j]))==l_Undef) {
					Lit paux;
					paux=(*c)[j];
					(*c)[j]=(*c)[pos];
     (*c)[pos]=paux;
     pos++;
     cont++;
    }
   }
   if(pos==2) {
    watches[index(~(*c)[0])].push_();
    watches[index(~(*c)[0])][watches[index(~(*c)[0])].size()-1]=c;
    watches[index(~(*c)[1])].push_();
    watches[index(~(*c)[1])][watches[index(~(*c)[1])].size()-1]=c;
    learnts.push(c);
   }   
   array_rest[lit].learnCla.pop_();
  }
 }
}

Int Solver::minimumWeight(Int a,Int b){
 if(a<b) return a;
 else return b;
}

Int Solver::getClauseWeight(Clause *c){
 if(c->isHard()) return UB;
 else if(c->isVirtual()) return NC[index((*c)[0])];
 else return c->getWeight();
}

int Solver::indexRest(){
 if(decisionLevel()>0) return index(trail[trail.size()-1]);
 else return nVars()*2;
}

void Solver::calculateStaticOrdering()
{
 if(opt_heur==1)
 {
 for(int i=0;i<nVars();i++)
 {
  Lit p=Lit(i,false);
  activity[var(p)]=0;
  if(NC[index(p)]>0)
   activity[var(p)] += toint(NC[index(p)]);
  if(NC[index(~p)]>0)
   activity[var(p)] += toint(NC[index(~p)]);
 }
 
 for (int i = 0; i < clauses.size(); i++){
  Clause& c = *clauses[i];
  
  if(not c.isUsed())
  {
  int cont=0;
  bool sat=false;
  for (int j = 0; j < c.size(); j++){ 
   if(value(c[j])==l_Undef) cont++;
   if(value(c[j])==l_True) sat=true;}
  if(not sat)
  {
  for (int j = 0; j < c.size(); j++){
   Lit p=c[j];
   
   if(c.isHard() and maxPseudo>0) activity[var(p)] += toint(maxPseudo)/pow(2,cont);
   else activity[var(p)] += toint(c.getWeight())/pow(2,cont);
  }
  }
  }
 }
 for (int i = 0; i < comps.size(); i++){
  Clause& c = *comps[i];
  if(not c.isUsed())
  {
  int cont=0;
  bool sat=false;
  for (int j = 0; j < c.size(); j++){ 
   if(value(c[j])==l_Undef) cont++;
   if(value(c[j])==l_True) sat=true;}
  if(not sat) 
  {
  for (int j = 0; j < c.size(); j++){
   Lit p=c[j];
   if(c.isHard() and maxPseudo>0) activity[var(p)] += toint(maxPseudo)/pow(2,cont);
   else activity[var(p)] += toint(c.getWeight())/pow(2,cont);
  }
  }
  }
 }
 
 for(int i=0;i<nVars();i++)
 {
  order.update(i);
 }
 }
}

int Solver::calculateJeroslow()
{
 if(opt_heur==2)
 {
  //reportf("HELLO\n");
  for(int i=0;i<nVars();i++)
  {
   Lit p=Lit(i,false);
   array_heur[i]=0;
   if(NC[index(p)]>0)
    array_heur[i] += toint(NC[index(p)]);
   if(NC[index(~p)]>0)
    array_heur[i] += toint(NC[index(~p)]);
  }
  
  for (int i = 0; i < clauses.size(); i++){
   Clause& c = *clauses[i];
   
   if(not c.isUsed())
   {
   int cont=0;
   bool sat=false;
   for (int j = 0; j < c.size(); j++){ 
    if(value(c[j])==l_Undef) cont++;
    if(value(c[j])==l_True) sat=true;}
   if(not sat)
   {
   for (int j = 0; j < c.size(); j++){
    Lit p=c[j];
    
    if(c.isHard() and maxPseudo>0) array_heur[var(p)] += toint(maxPseudo)/pow(2,cont);
    else array_heur[var(p)] += toint(c.getWeight())/pow(2,cont);
   }
   }
   }
  }
  for (int i = 0; i < comps.size(); i++){
   Clause& c = *comps[i];
   if(not c.isUsed())
   {
			int cont=0;
			bool sat=false;
			for (int j = 0; j < c.size(); j++){ 
				if(value(c[j])==l_Undef) cont++;
				if(value(c[j])==l_True) sat=true;}
			if(not sat) 
			{
			for (int j = 0; j < c.size(); j++){
				Lit p=c[j];
				if(c.isHard() and maxPseudo>0) array_heur[var(p)] += toint(maxPseudo)/pow(2,cont);
				else array_heur[var(p)] += toint(c.getWeight())/pow(2,cont);
			}
			}
			}
		}
		int bestv=-1;
		double bestc=-1.0;
		for(int i=0;i<nVars();i++)
		{
			if(value(i)==l_Undef){
				if(array_heur[i]>=bestc) {
					bestv=i;
					bestc=array_heur[i];
				}
			}
		}
		//reportf("BYE 1 %d\n",bestv);
		return bestv;
	}
	//reportf("BYE 2 \n");
	return -1;
}

void Solver::localSearch( int iTimeOut, int iNumRuns, int iSeed ){
  if(opt_file_type == ft_Pseudo) {
    setTop(-1);  
    return;
  }

  printf("local search probe...\n");

  char sTimeout[64];
  char sRuns[64];

  sprintf(sTimeout, "%d", iTimeOut );
  sprintf(sRuns, "%d", iNumRuns );

  char* vlineFilename = new char[512];
  sprintf(vlineFilename, "%s", P_tmpdir);
  strcat(vlineFilename, "/XXXXXX");
  vlineFilename = mktemp(vlineFilename);

  char strseed[64];
  sprintf(strseed, "%d", iSeed );

  char* argv[] = {"ubcsat", 
		  "-alg", NULL, "-noimprove", NULL,  "-i", opt_input, 
		  "-runs", sRuns, "-timeout", sTimeout,
		  "-r", "stats", vlineFilename, "best",
		  "-seed", strseed,
		  "-satzilla",
		  NULL
  };
  
  int argc = 17;
  
  // -- do saps
  argv[2]="saps";
  argv[4]="0.1";

  if( opt_file_type != ft_Cnf ) { argv[argc] = "-w"; argc++; }
  
  //if ( ubcsat::main(argc, argv) == 10 ) printf("Instance satisfiable\n");

  FILE *ft;
#define MAX_CARS 1000
  float best_solution;
  char line[ MAX_CARS ];
  
  ft = fopen(vlineFilename, "r");
   while( fgets( line, MAX_CARS, ft ) != NULL ) {     
     sscanf( line, "BestSolution_Mean = %f\n", &best_solution );
   }
  printf("o %d \n", (int)best_solution );
  fflush(stdout);    
  fclose(ft);	
  
  delete[] vlineFilename;
}

void Solver::DoHardening() {

    int *taux = new int[nVars()*2];
    bool heuristic=false;
    int hard=0;
    
    for (int i = 0; i < nVars()*2; i++) taux[i]=0;
    
    for (int i = 0; i < clauses.size(); i++)
    {
            if(not clauses[i]->isHard())
	    {
	    	if(clauses[i]->getWeight()>=UB) 
		{
			Clause& c = *clauses[i];
			c.setHard(true);
		}
	    }
	
	Clause& c = *clauses[i];
	if(c.isHard()) hard++;
	if(c.isHard() and not heuristic)
	{
		for (int j = 0; j < c.size() and not heuristic; j++){
			Lit p=c[j];
			taux[index(p)]++;
			if(taux[index(p)]>0 and taux[index(~p)]>0) heuristic=true;
			
		}
	}

    }
    delete taux;
    // MEGA CAMBIO (Heuristica mixta)
    if(opt_heur==3) // AUTO
    {
	if (heuristic and hard > ((2*clauses.size())/3) ) opt_heur=2; // VSIDS
	else opt_heur=1; // JEROSLOW
	//if(heuristic) opt_prob=0; // NO PROBING IN PRESENCE OF HARD CLAUSES
    }
    
    if(opt_heur==0) {
    	reportf("HEURISTIC: NONE \n");
	for(int i=0;i<nVars();i++)
	{
		Lit p=Lit(i,false);
		array_heur[i]=0;
		order.update(i);
	}
    }
    if(opt_heur==1) reportf("HEURISTIC: JEROSLOW LIKE \n");
    if(opt_heur==2) reportf("HEURISTIC: VSIDS LIKE \n");


	
}




}// end namespace MiniSat
