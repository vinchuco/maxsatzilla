/****************************************************************************************[Solver.C]
MiniSat -- Copyright (c) 2003-2005, Niklas Een, Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#include "Solver.hh"
#include "Sort.hh"
#include <cmath>


//=================================================================================================
// Helper functions:


bool removeWatch(vec<Clause*>& ws, Clause* elem)    // Pre-condition: 'elem' must exists in 'ws' OR 'ws' must be empty.
{
    if (ws.size() == 0) return false;     // (skip lists that are already cleared)
    int j = 0;
    for (; ws[j] != elem  ; j++) assert(j < ws.size());
    for (; j < ws.size()-1; j++) ws[j] = ws[j+1];
    ws.pop();
    return true;
}


//=================================================================================================
// Operations on clauses:



/*----------------------------------------------------------------------------*\
 * Purpose: New function for creating a clause (jpms)
\*----------------------------------------------------------------------------*/

ClauseRef Solver::add_clause(vector<int>& ps_)
{
  //PRINT_ELEMENTS(ps_, "Adding clause: ");
  // if (!ok) return -1;  // jpms:20060122 -> Always add clause

  vec<Lit>    qs;
  assert(decisionLevel() == 0);
  // ps_.copyTo(qs);             // Make a copy of the input vector.
  vector<int>::iterator pos;
  for(pos = ps_.begin(); pos != ps_.end(); ++pos) {
    int elit = *pos;
    //Var v = (elit < 0) ? -elit : elit;
    Var v = ((elit < 0) ? -elit : elit)-1;// In minisat, *must* decrement var id
    assert((int)v>=0);
    //assert(v <= num_vars());

    while (v >= num_vars()) {
      //assert(0);
      new_var();
    }
    bool s = (elit < 0) ? true : false;
    //bool s = (elit < 0) ? false : true;
    Lit ilit(v, s);
    qs.push(ilit);
  }
  // Remove duplicates:
  sortUnique(qs);

  /* jpms:20060120 -> Currently ***NO*** simplifications
  // Check if clause is satisfied:
  for (int i = 0; i < qs.size()-1; i++){
    if (qs[i] == ~qs[i+1])
      return -1; }
  for (int i = 0; i < qs.size(); i++){
    if (value(qs[i]) == l_True)
      return -1; }
  
  // Remove false literals:
  int     i, j;
  for (i = j = 0; i < qs.size(); i++)
    if (value(qs[i]) != l_False)
      qs[j++] = qs[i];

  qs.shrink(i - j);
  */
  return static_cast<ClauseRef>(int_add_clause(qs, false));
}

// Access a clause...
StdClause& Solver::get_clause(ClauseRef cl)
{
  clref.set_clause(cl);
  return clref;
}


/*----------------------------------------------------------------------------*\
 * Purpose: Internal method for adding a clause.
\*----------------------------------------------------------------------------*/
Clause* Solver::int_add_clause(vec<Lit>& ps, bool learnt)
{
  Clause* c = NULL;
  if (ps.size() == 0){
    DBG(cout<<"Adding the empty clause..."<<endl;);
    ok = false;
  }
  else if (ps.size() == 1){
    c   = Clause_new(learnt, ps);
    if (learnt) {
      DBG(cout<<"Adding learnt unit clause"<<endl;);
      learnt_units++; learnts.push(c);
    }
    else { ++unit_cls; clauses.push(c); }
    // NOTE: If enqueue takes place at root level, the assignment will be lost in incremental use (it doesn't seem to hurt much though).
    if (!enqueue(ps[0], c)) {
      DBG(cout<<"Failed adding unit clause..."<<endl;);
      st_confl = c;   // Conflict at startup time
      ok = false;
    }
  }else{

    if (learnt){
      DBG(cout<<"Adding learnt clause "<<nLearnts()+1<<": ( ";
	  cout<<toStd(ps[0])<<" "<<toStd(ps[1])<<" ";);

      /* Sorting is slower.... jpms:20060307
      dlevel_gt cmp_gt(level); double mseed = 1923576;
      sort((Lit*)ps+1, ps.size()-1, cmp_gt, mseed);
      */
      // Allocate clause:
      c = Clause_new(learnt, ps);

      /* */
      // Put the second watch on the literal with highest decision level:
      int     max_i = 1;
      int     max   = level[var(ps[1])];
      for (int i = 2; i < ps.size(); i++) {
	if (level[var(ps[i])] > max) {
	  max   = level[var(ps[i])],
	    max_i = i;
	}
	DBG(cout<<toStd(ps[i])<<" ");
      }
      DBG(cout<<")"<<endl;);
      (*c)[1]     = ps[max_i];
      (*c)[max_i] = ps[1];
      /* */

      // Bump, enqueue, store clause:
      claBumpActivity(c);         // (newly learnt clauses should be considered active)
      check(enqueue((*c)[0], c));
      learnts.push(c);
      stats.learnts_literals += c->size();
    }else{
      // Allocate clause:
      c = Clause_new(learnt, ps);
      // Store clause:
      clauses.push(c);
      stats.clauses_literals += c->size();
    }
    // Watch clause:
    watches[index(~(*c)[0])].push(c);
    watches[index(~(*c)[1])].push(c);
    // Update stats
    if (ps.size() == 2)      { stats.clauses_binary++; }
    else if (ps.size() == 3) { stats.clauses_ternary++; }
    else                     { stats.clauses_large++; }

  }
  if(learnt) {  // If learnt, and logging, concluding tracing learnt clause
    if (plogger != NULL) { plogger->end_cl_trace(c); }
  }
  return c;    // return clause ref
}


// Disposes a clauses and removes it from watcher lists. NOTE! Low-level; does NOT change the 'clauses' and 'learnts' vector.
//
void Solver::remove(Clause* c, bool just_dealloc)
{
  exit(9);
    if (!just_dealloc){
      removeWatch(watches[index(~(*c)[0])], c),
	removeWatch(watches[index(~(*c)[1])], c);
    }

    if (c->learnt()) stats.learnts_literals -= c->size();
    else             stats.clauses_literals -= c->size();

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
Var Solver::newVar() {
    int     index;
    index = nVars();
    watches     .push();          // (list for positive literal)
    watches     .push();          // (list for negative literal)
    reason      .push(NULL);
    assigns     .push(toInt(l_Undef));
    level       .push(-1);
    activity    .push(0);
    order       .newVar();
    analyze_seen.push(0);
    return index; }


// Returns FALSE if immediate conflict.
bool Solver::assume(Lit p) {
    trail_lim.push(trail.size());
    return enqueue(p); }


// Revert to the state at given level.
void Solver::cancelUntil(int level) {
    if (decisionLevel() > level){
        for (int c = trail.size()-1; c >= trail_lim[level]; c--){
            Var     x  = var(trail[c]);
            assigns[x] = toInt(l_Undef);
            reason [x] = NULL;
            order.undo(x); }
        trail.shrink(trail.size() - trail_lim[level]);
        trail_lim.shrink(trail_lim.size() - level);
        qhead = trail.size(); } }


//=================================================================================================
// Major methods:


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
void Solver::analyze(Clause* confl, vec<Lit>& out_learnt, int& out_btlevel)
{
    vec<char>&     seen  = analyze_seen;
    int            pathC = 0;
    Lit            p     = lit_Undef;

    // jpms: if using a proof logger, then activate logging for new clause
    if (plogger != NULL) { plogger->begin_cl_trace(); }

    // Generate conflict clause:
    //
    out_learnt.push();      // (leave room for the asserting literal)
    out_btlevel = 0;
    int index = trail.size()-1;
    do{
        assert(confl != NULL);          // (otherwise should be UIP)
        Clause& c = *confl;

	if (plogger != NULL) {
	  NDBG(cout << "Adding dep...\n";);
	  assert(p==lit_Undef || toStd(p)!=0);
	  plogger->add_dep_pair(&c, (p==lit_Undef)?0:toStd(p));
	}

        if (c.learnt())
            claBumpActivity(&c);

        for (int j = (p == lit_Undef) ? 0 : 1; j < c.size(); j++){
            Lit q = c[j];
            if (!seen[var(q)]) {
	      if (level[var(q)] >= 0){   // 20060129:jpms -> must include lev 0
                varBumpActivity(q);
                seen[var(q)] = 1;
                if (level[var(q)] == decisionLevel())
                    pathC++;
                else{
                    out_learnt.push(q);
                    out_btlevel = maxT(out_btlevel, level[var(q)]);

                }
	      }
	    }
        }

        // Select next clause to look at:
        while (!seen[var(trail[index--])]);
        p     = trail[index+1];
        confl = reason[var(p)];
        seen[var(p)] = 0;
        pathC--;

    }while (pathC > 0);
    out_learnt[0] = ~p;

    /* 20060131:jpms
      NOTE: Either use the function call below or the simplifications
            afterwards. Variable analyze_clear *must* be properly updated.
    */
    out_learnt.copyTo(analyze_toclear);   // Comment if below *not* commented

    /*   20060129:jpms -> no simplifications 
    int     i, j;
    if (expensive_ccmin){
        // Simplify conflict clause (a lot):
        //
        uint    min_level = 0;
        for (i = 1; i < out_learnt.size(); i++)
            min_level |= 1 << (level[var(out_learnt[i])] & 31);         // (maintain an abstraction of levels involved in conflict)

        out_learnt.copyTo(analyze_toclear);
        for (i = j = 1; i < out_learnt.size(); i++)
            if (reason[var(out_learnt[i])] == NULL || !analyze_removable(out_learnt[i], min_level))
                out_learnt[j++] = out_learnt[i];
    }else{
        // Simplify conflict clause (a little):
        //
        out_learnt.copyTo(analyze_toclear);
        for (i = j = 1; i < out_learnt.size(); i++){
            Clause* r = reason[var(out_learnt[i])];
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
    }
    */

    stats.max_literals += out_learnt.size();
    /*   20060129:jpms -> no simplifications 
      out_learnt.shrink(i - j);
    */
    stats.tot_literals += out_learnt.size();

    for (int j = 0; j < analyze_toclear.size(); j++) seen[var(analyze_toclear[j])] = 0;    // ('seen[]' is now cleared)
}


// Check if 'p' can be removed. 'min_level' is used to abort early if visiting literals at a level that cannot be removed.
//
bool Solver::analyze_removable(Lit p, uint min_level)
{
    assert(reason[var(p)] != NULL);
    analyze_stack.clear(); analyze_stack.push(p);
    int top = analyze_toclear.size();
    while (analyze_stack.size() > 0){
        assert(reason[var(analyze_stack.last())] != NULL);
        Clause* r = reason[var(analyze_stack.last())]; analyze_stack.pop();
        Clause& c = *r;
        for (int i = 1; i < c.size(); i++){
            Lit p = c[i];
            if (!analyze_seen[var(p)] && level[var(p)] != 0){
                if (reason[var(p)] != NULL && ((1 << (level[var(p)] & 31)) & min_level) != 0){
                    analyze_seen[var(p)] = 1;
                    analyze_stack.push(p);
                    analyze_toclear.push(p);
                }else{
                    for (int j = top; j < analyze_toclear.size(); j++)
                        analyze_seen[var(analyze_toclear[j])] = 0;
                    analyze_toclear.shrink(analyze_toclear.size() - top);
                    return false;
                }
            }
        }
    }

    return true;
}


/*_________________________________________________________________________________________________
|
|  analyzeFinal : (confl : Clause*) (skip_first : bool)  ->  [void]
|  
|  Description:
|    Specialized analysis procedure to express the final conflict in terms of assumptions.
|    'root_level' is allowed to point beyond end of trace (useful if called after conflict while
|    making assumptions). If 'skip_first' is TRUE, the first literal of 'confl' is  ignored (needed
|    if conflict arose before search even started).
|________________________________________________________________________________________________@*/
void Solver::analyzeFinal(Clause* confl, bool skip_first)
{
  DBG(cout << "Entering analyze final. Should add a clause...\n";);
    // -- NOTE! This code is relatively untested. Please report bugs!
    conflict.clear();
    /* 20051026:jpms -> *must* trace dependencies at decision level 0
    if (root_level == 0) { assert(0); return; }
    */

    if (plogger != NULL) { plogger->begin_cl_trace(); }

    if (plogger != NULL) {
      NDBG(cout << "Adding dep...\n";);
      plogger->add_dep_pair(confl, 0);
    }

    vec<char>&     seen  = analyze_seen;
    for (int i = skip_first ? 1 : 0; i < confl->size(); i++){
        Var     x = var((*confl)[i]);
        if (level[x] >= 0)    // 20051026:jpms -> working also @dlevel 0
            seen[x] = 1;
    }

    int     start = (root_level >= trail_lim.size()) ? trail.size()-1 : trail_lim[root_level];
    int lbound = (trail_lim.size() > 0) ? trail_lim[0] : 0;
    for (int i = start; i >= lbound; i--){
        Var     x = var(trail[i]);
	DBG(cout << "Tracing var : "<<x+1<<" in analyzeFinal"<<endl;);
        if (seen[x]){
	  DBG(cout << x+1 << " seen in earlier tracing" <<endl;);
            Clause* r = reason[x];

	    if (plogger != NULL) {
	      NDBG(cout << "Adding dep...\n";);
	      if (r!=NULL) {
		assert(toStd(trail[i])!=0);
		plogger->add_dep_pair(r, toStd(trail[i]));
	      }
	      DBG(else{cout<<"NO reason for "<<x<<endl;});
	    }

            if (r == NULL){
	        // assert(level[x] > 0);    // 20051026:jpms -> working also @dlevel 0
                conflict.push(~trail[i]);
            }else{
                    Clause& c = *r;
                    for (int j = 1; j < c.size(); j++)
                        if (level[var(c[j])] >= 0)    // 20051026:jpms -> working also @dlevel 0
                            seen[var(c[j])] = 1;
	    }
            seen[x] = 0;
        }
    }
    if (plogger != NULL) { plogger->end_cl_trace(empty_cl); }
}


/*_________________________________________________________________________________________________
|
|  enqueue : (p : Lit) (from : Clause*)  ->  [bool]
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
/* *****
// jpms:20060220 -> Currently inlined
bool Solver::enqueue(Lit p, Clause* from)
{
    if (value(p) != l_Undef)
        return value(p) != l_False;
    else{
      DBG(cout<<"Assigning "<<var(p)+1<<" @ "<<decisionLevel()<<endl;);

        assigns[var(p)] = toInt(lbool(!sign(p)));
        level  [var(p)] = decisionLevel();
        reason [var(p)] = from;
        trail.push(p);
        return true;
    }
}
***** */

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
Clause* Solver::propagate()
{
    Clause* confl = NULL;
    while (qhead < trail.size()){
        stats.propagations++;
        simpDB_props--;

        Lit            p   = trail[qhead++];     // 'p' is enqueued fact to propagate.
	//cout << "Assigning: " << toStd(p) << endl;

        vec<Clause*>&  ws  = watches[index(p)];
        Clause**       i,** j, **end;

        for (i = j = (Clause**)ws, end = i + ws.size();  i != end;){
                Clause& c = **i; i++;
		/* ***** */
		if (c.size() == 2) {    // jpms:20060219
		  *j++ = &c;
		  Lit false_lit = ~p;
		  if (c[0] == false_lit)
                    c[0] = c[1], c[1] = false_lit;
		  if (value(c[0]) != l_True &&
		      !enqueue(c[0], &c)) {
		    if (decisionLevel() == 0)
		      ok = false;
		    confl = &c;
		    qhead = trail.size();
		    // Copy the remaining watches:
		    while (i < end)
		      *j++ = *i++;
		  }
		  continue;
		}
		/* *** */
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
                }else{
                    // Look for new watch:
		    int clsz = c.size();
                    for (int k = 2; k < clsz; k++)
                        if (value(c[k]) != l_False){
                            c[1] = c[k]; c[k] = false_lit;
                            watches[index(~c[1])].push(&c);
                            goto FoundWatch; }

                    // Did not find watch -- clause is unit under assignment:
                    *j++ = &c;
                    if (!enqueue(first, &c)){
                        if (decisionLevel() == 0)
                            ok = false;
                        confl = &c;
                        qhead = trail.size();
                        // Copy the remaining watches:
                        while (i < end)
                            *j++ = *i++;
                    }
                  FoundWatch:;
                }
	}
        ws.shrink(i - j);
    }
    return confl;
}


/*_________________________________________________________________________________________________
|
|  reduceDB : ()  ->  [void]
|  
|  Description:
|    Remove half of the learnt clauses, minus the clauses locked by the current assignment. Locked
|    clauses are clauses that are reason to some assignment. Binary clauses are never removed.
|________________________________________________________________________________________________@*/
struct reduceDB_lt { bool operator () (Clause* x, Clause* y) { return x->size() > 2 && (y->size() == 2 || x->activity() < y->activity()); } };
void Solver::reduceDB()
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
    learnts.shrink(i - j);
}


/*_________________________________________________________________________________________________
|
|  simplifyDB : [void]  ->  [bool]
|  
|  Description:
|    Simplify the clause database according to the current top-level assigment. Currently, the only
|    thing done here is the removal of satisfied clauses, but more things can be put here.
|________________________________________________________________________________________________@*/
void Solver::simplifyDB()
{
  assert(0);    // 20051026:jpms -> *must* not execute this function

  if (!ok) return;    // GUARD (public method)
  assert(decisionLevel() == 0);

  if (propagate() != NULL){
    ok = false;
    return; }

    if (nAssigns() == simpDB_assigns || simpDB_props > 0)   // (nothing has changed or preformed a simplification too recently)
        return;

    // Clear watcher lists:
    for (int i = simpDB_assigns; i < nAssigns(); i++){
        Lit           p  = trail[i];
        watches[index( p)].clear(true);
        watches[index(~p)].clear(true);
    }

    // Remove satisfied clauses:
    for (int type = 0; type < 2; type++){
        vec<Clause*>& cs = type ? learnts : clauses;
        int     j  = 0;
        for (int i = 0; i < cs.size(); i++){
            if (!locked(cs[i]) && simplify(cs[i]))  // (the test for 'locked()' is currently superfluous, but without it the reason-graph is not correctly maintained for decision level 0)
                remove(cs[i]);
            else
                cs[j++] = cs[i];
        }
        cs.shrink(cs.size()-j);
    }

    simpDB_assigns = nAssigns();
    simpDB_props   = stats.clauses_literals + stats.learnts_literals;   // (shouldn't depend on 'stats' really, but it will do for now)
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
    if (!ok) return l_False;    // GUARD (public method)
    assert(root_level == decisionLevel());

    stats.starts++;
    int     conflictC = 0;
    var_decay = 1 / params.var_decay;
    cla_decay = 1 / params.clause_decay;
    model.clear();
    for (;;){
        Clause* confl = propagate();
        if (confl != NULL){
            // CONFLICT
            stats.conflicts++; conflictC++;
            vec<Lit>    learnt_clause;
            int         backtrack_level;
            if (decisionLevel() == root_level){
                // Contradiction found:
                analyzeFinal(confl);
                return l_False; }
            analyze(confl, learnt_clause, backtrack_level);
            cancelUntil(maxT(backtrack_level, root_level));
            //newClause(learnt_clause, true);
	    int_add_clause(learnt_clause, true);

            if (learnt_clause.size() == 1) level[var(learnt_clause[0])] = 0;    // (this is ugly (but needed for 'analyzeFinal()') -- in future versions, we will backtrack past the 'root_level' and redo the assumptions)
            varDecayActivity();
            claDecayActivity();

        }else{
            // NO CONFLICT

            if (nof_conflicts >= 0 && conflictC >= nof_conflicts){
                // Reached bound on number of conflicts:
                progress_estimate = progressEstimate();
                cancelUntil(root_level);
                return l_Undef; }

	    /* 20051026:jpms
            if (decisionLevel() == 0)
                // Simplify the set of problem clauses:
                simplifyDB(), assert(ok);
	    */

	    /* 200070726:jpms -> clauses cannot be deleted with proot tracing...
            if (nof_learnts >= 0 && learnts.size()-nAssigns() >= nof_learnts)
                // Reduce the set of learnt clauses:
                reduceDB();
	    */

            // New variable decision:
            stats.decisions++;
            Var next = order.select(params.random_var_freq);

            if (next == var_Undef){
                // Model found:
                model.growTo(nVars());
                for (int i = 0; i < nVars(); i++) model[i] = value(i);
                cancelUntil(root_level);
                return l_True;
            }

            check(assume(~Lit(next)));
        }
    }
}


// Return search-space coverage. Not extremely reliable.
//
double Solver::progressEstimate()
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
void Solver::varRescaleActivity()
{
    for (int i = 0; i < nVars(); i++)
        activity[i] *= 1e-100;
    var_inc *= 1e-100;
}


// Divide all constraint activities by 1e100.
//
void Solver::claRescaleActivity()
{
    for (int i = 0; i < learnts.size(); i++)
        learnts[i]->activity() *= 1e-20;
    cla_inc *= 1e-20;
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
  assert(ok);
  /* 20051026:jpms -> *must* not invoke simplifyDB
  simplifyDB();
  if (!ok) return false;
  */
  // 20051026:jpms -> Replacing simplifyDB with a single call to propagate
  root_level = 0;
  Clause* confl = propagate();
  if (confl == NULL && st_confl != NULL) { confl = st_confl; }
  assert(ok || confl!=NULL);
  
  if (confl != NULL) {
    ok = false;
    analyzeFinal(confl);
    return ok;
  }
  assert(ok == true);
  // jpms:20051026

    SearchParams    params(default_params);
    double  nof_conflicts = 100;
    double  nof_learnts   = nClauses() / 3;
    lbool   status        = l_Undef;

    // Perform assumptions:
    root_level = assumps.size();
    for (int i = 0; i < assumps.size(); i++){
        Lit p = assumps[i];
        assert(var(p) < nVars());
        if (!assume(p)){
            if (reason[var(p)] != NULL){
                analyzeFinal(reason[var(p)], true);
                conflict.push(~p);
            }else
                conflict.clear(),
                conflict.push(~p);
            cancelUntil(0);
            return false; }
        Clause* confl = propagate();
        if (confl != NULL){
            analyzeFinal(confl), assert(conflict.size() > 0);
            cancelUntil(0);
            return false; }
    }
    assert(root_level == decisionLevel());

    // Search:
    if (verbosity >= 1){
        reportf("==================================[MINISAT]===================================\n");
        reportf("| Conflicts |     ORIGINAL     |              LEARNT              | Progress |\n");
        reportf("|           | Clauses Literals |   Limit Clauses Literals  Lit/Cl |          |\n");
        reportf("==============================================================================\n");
    }

    while (status == l_Undef){
        if (verbosity >= 1){
            reportf("| %9d | %7d %8d | %7d %7d %8d %7.1f | %6.3f %% |\n", (int)stats.conflicts, nClauses(), (int)stats.clauses_literals, (int)nof_learnts, nLearnts(), (int)stats.learnts_literals, (double)stats.learnts_literals/nLearnts(), progress_estimate*100);
            fflush(stdout);
        }
        status = search((int)nof_conflicts, (int)nof_learnts, params);
        nof_conflicts *= 1.5;
        nof_learnts   *= 1.1;
    }
    if (verbosity >= 1)
        reportf("==============================================================================\n");

    cancelUntil(0);
    return status == l_True;
}


/*----------------------------------------------------------------------------*\
 * Purpose: Print stats
\*----------------------------------------------------------------------------*/

void Solver::print_stats(bool outcome, ostream& outs)
{
  outs << "--------------------------------------\n";

  outs << "SAT Solver outcome: "<<outcome<<"\n";
  outs << "SAT Solver stats:\n";
  outs << "  Variables     : " << num_vars() << endl;
  outs << "  Clauses       : " << num_total_cls() << endl;
  outs << "    Unit Cls    : " << (unit_cls+learnt_units) << endl;
  outs << "    Binary Cls  : " << stats.clauses_binary << endl;
  outs << "    Ternary Cls : " << stats.clauses_ternary << endl;
  outs << "    Large Cls   : " << stats.clauses_large << endl;
  outs << "  Decisions     : " << stats.decisions << endl;
  outs << "  Propagations  : " << stats.propagations << endl;
  outs << "  Conflicts     : " << stats.conflicts << endl;
  outs << "  Restarts      : " << stats.starts << endl;

  outs << "--------------------------------------\n";
}

