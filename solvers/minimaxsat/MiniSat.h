/**************************************************************************************************

Solver.h -- (C) Niklas Een, Niklas S�ensson, 2005

A simple Chaff-like SAT-solver with support for incremental SAT and Pseudo-boolean constraints.

**************************************************************************************************/

#ifndef MiniSat_h
#define MiniSat_h

#include "SolverTypes.h"
#include "VarOrder.h"

namespace MiniSat {

//=================================================================================================
// Clause -- a simple class for representing a clause

class Clause {
    // FEDE_NEW
    Int weight;
    int64 mark;
    bool hard;
    bool used;
    bool virt;
    uint    size_learnt;
    Lit     data[0];

public:

    // NOTE: Cannot be used by normal 'new' operator!
    Clause(bool learnt, const vec<Lit>& ps) {
	used=false;weight=1;hard=true; size_learnt = (ps.size() << 1) | (int)learnt;
        for (int i = 0; i < ps.size(); i++) data[i] = ps[i];
        if (learnt) activity() = 0; virt=false; mark=0;}
	
    // NOTE: Cannot be used by normal 'new' operator!
    Clause(bool learnt, const vec<Lit>& ps,Int w,bool h) {
        used=false;weight=w;hard=h; size_learnt = (ps.size() << 1) | (int)learnt;
        for (int i = 0; i < ps.size(); i++) data[i] = ps[i];
        if (learnt) activity() = 0; virt=false; mark=0;}
	
    void setNewClause(bool learnt, const vec<Lit>& ps,Int w,bool h) {
        used=false;weight=w;hard=h; size_learnt = (ps.size() << 1) | (int)learnt;
        for (int i = 0; i < ps.size(); i++) data[i] = ps[i];
        if (learnt) activity() = 0; virt=false; mark=0;}


    int     size        (void)      const { return size_learnt >> 1; }
    bool    learnt      (void)      const { return size_learnt & 1; }
    Lit     operator [] (int index) const { return data[index]; }
    Lit&    operator [] (int index)       { return data[index]; }
    float&  activity    (void)      const { return *((float*)&data[size()]); }
    Int     getWeight   (void)            { return weight;}
    void    setWeight   (Int c)           {weight=c;}
    bool    isHard      (void)      { return hard;}
    void    setHard     (bool b)          {hard=b;}
    bool    isUsed      (void)            {return used;}
    void    setUsed     (bool b)          {used=b;}
    int64   getMark	(void)		  {return mark;}
    void    setMark	(int64 m)	  {mark=m;}
    bool    isVirtual   (void)            {return virt;}
    void    setVirtual  (bool b)          {virt=b;}

    
};


//=================================================================================================
// LitClauseUnion -- a simple union type:


class LitClauseUnion {
    void* data;
    LitClauseUnion(void* d) : data(d) {}
public:
    friend LitClauseUnion makeLit    (Lit l)      { return LitClauseUnion((void*)(( ((intp)index(l))<<1) + 1)); }
    friend LitClauseUnion makeClause (Clause* c)  { assert(((intp)c & 1) == 0); return LitClauseUnion((void*)c); }

    bool        isLit             (void)     const { return ((intp)data & 1) == 1; }
    bool        isNull            (void)     const { return data == NULL; }
    Lit         getLit            (void)     const { return toLit((int)(((intp)data)>>1)); }
    Clause*     getClause         (void)     const { return (Clause*)data; }
    bool        operator==(LitClauseUnion c) const { return data == c.data; }
    bool        operator!=(LitClauseUnion c) const { return data != c.data; }
};

//=================================================================================================
// decAssignment -- datastructure to perform chronological backtracking


class decAssignment
{
/* Structure used to perform chronological backtracking */
public:
	int lit;
	int values;
};

//=================================================================================================
// Some minor classes to restore information


class restoreNC
{
public:
	int lit;
	Int cost;
	restoreNC(void){cost=0;}
	void setValues(int l,Int c) {lit=l; cost=c;}
};


class restoreClause
{
public:
	Int cost;
	Clause *clause;
	restoreClause(void){clause=NULL;}
	void setValues(Clause *cla,Int c) {clause=cla;cost=c;}
};

class restoreAssignment
{
	// Structure used to undo changes in order to recover past states.
public:
	Int prevLB;
	vec<restoreNC> restNC; // To restore NC table changes
	vec<restoreClause> restClaCost; // To restore changed weights on clauses
	vec<Clause*> restCla; // To reactivate disactivated clauses.
	vec<Clause*> learnCla;
	int newCompClauses;
	
	restoreAssignment(void){
		newCompClauses=0;
	}
    
	void pushNC(int lit,Int cost) {
		restNC.push_();
		restNC[restNC.size()-1].setValues(lit,cost);
		}
	void pushCla(Clause *c) {
		restCla.push_();
		restCla[restCla.size()-1]=c;
	}
	void pushClaCost(Clause *c,Int co) {
		restClaCost.push_();
		restClaCost[restClaCost.size()-1].setValues(c,co);

	}

	void restoreThis(Int * NC)
	{
		for(int i=restNC.size()-1;i>=0;i--)
		{
			NC[restNC[i].lit]-=restNC[i].cost;
			restNC.pop();
		}
	}
	void pushLearnCla(Clause *c) {
		learnCla.push_();
		learnCla[learnCla.size()-1]=c;
	}
};

class reasoningInfo
{
	public:
	int lit; 
	Clause *reason;
	int clash;
};
class restoreReasoningStack
{
	// Structure needed to do the transformations or to mark clauses as used.
	public:
	int size;
	vec<restoreNC> restNC;
	reasoningInfo *r_trail;
	vec<restoreClause> restMarkCla;

	void createStack(int nlits) { r_trail = new reasoningInfo[nlits]; size=0; }
	~restoreReasoningStack(void){ delete[] r_trail; }
	
	void insertInfo(int lit,Clause *r,bool b=false) { 
	     int pos=size; size++; r_trail[pos].lit=lit;  r_trail[pos].reason=r; }

	void clean(void) {size=0;}
};



//=================================================================================================
// Solver -- the main class:


struct SolverStats : public BasicSolverStats {
    int64   clauses, clauses_literals, learnts, learnts_literals, max_literals, tot_literals;
    SolverStats(void) : clauses(0), clauses_literals(0), learnts(0), learnts_literals(0), max_literals(0), tot_literals(0) {}
};


struct SearchParams {
    double  var_decay, clause_decay, random_var_freq;    // (reasonable values are: 0.95, 0.999, 0.02)
    SearchParams(double v = 1, double c = 1, double r = 0) : var_decay(v), clause_decay(c), random_var_freq(r) { }
};


class Solver {
protected:
    vec<Clause*>        clauses;        // List of problem constraints.
    vec<Clause*>        learnts;        // List of learnt clauses.
    vec<Clause*>        comps;   // List of clashing clauses
    double              cla_inc;        // Amount to bump next clause with.
    double              cla_decay;      // INVERSE decay factor for clause activity: stores 1/decay.

    vec<double>         activity;       // A heuristic measurement of the activity of a variable.
    vec<char>           polarity;       // Polarity suggestion for branching -- 0=first assume positive polarity, 1=first assume negative polarity.
    double              var_inc;        // Amount to bump next variable with.
    double              var_decay;      // INVERSE decay factor for variable activity: stores 1/decay. Use negative value for static variable order.
    VarOrder            order;          // Keeps track of the decision variable order.

    vec<vec<Clause *> >
                        watches;        // 'watches[lit]' is a list of constraints watching 'lit' (will go there if literal becomes true).

public:
    bool                ok;             // If FALSE, the constraints are already unsatisfiable. No part of the solver state may be used!
    vec<int>            assigns;        // The current assignments (lbool:s stored as int:s for backward compatibility).
    vec<Lit>            trail;          // List of assignments made.
    vec<char>           polarity_sug;   // Suggestion (from user of Solver) for initial polarity to branch on. An 'lbool' coded as a 'char'.
    
    
    // NOTE! MaxSAT structures
    Int 		*NC;		// Array to compute Node Consistency. If w(x)>1 and w(x)>1, then LB=LB + min(w(x),w(x)).
    int64 		*usedLit;
    bool 		*bNC;
    Clause **	NCLits;			// Array of literals. They will be used to apply UP in order to detect inconsistencies.

    Int LB;				// An explicit lower bound of the current assignment.
    Int UB;				// An explicit upper bound of the best solution found so far.
    Int topUB;				// Upper bound of Hard Clauses
    vec<decAssignment>  trail_dec;	// The information necessary for chronological backtracking.
    bool		learn;		// Apply or not apply learning
    int 		search_state;	// Used in the search function to decide what to apply: assume a literal, chronological backtracking, backjumping.	
    double 		random_seed;
    restoreAssignment   *array_rest;	// Array to restore all transformations done by an assignment.
    vec<restoreNC>      tmp_unit_soft;
    int			cont_ac;
    bool		allHard;
    restoreReasoningStack reasons;
    vec<int>		prunedValues;
    int 		lecoutre;
    bool		trans_now;
    
    // Auxiliary variables to speed up the code
    int64		currentMark;
    int64		auxMark;
    vec<Lit>		psAux;
    vec<Lit>		psAuxA;
    vec<Lit>		psAuxB;
    Clause* 		newClaAux;
    Int			minAux;
    int  		maxArity;
    Int 		maxPseudo;
    double   		*array_heur;
    
private:
    vec<int>            trail_lim;      // Separator indices for different decision levels in 'trail'.
    vec<Lit>            trail_copy;     // <<== EXPERIMENTAL
    vec<Clause *> reason;         // 'reason[var]' is the clause that implied the variables current value, or 'NULL' if none.
    vec<int>            level;          // 'level[var]' is the decision level at which assignment was made.
    vec<int>		level_up;	// The level of unit propagation.	
    int                 root_level;     // Level of first proper decision.
    int                 last_simplify;  // Number of top-level assignments at last 'simplifyDB()'.
    int                 qhead;          // head of queue (as index in trail)

    // Temporaries (to reduce allocation overhead):
    //
    vec<char>           analyze_seen;
    vec<Lit>            toclear;
    vec<Lit>            stack;
    Clause*             tmp_binary;
    int64 		backs;
    int64		level_backs;
    Int 		previous;
    int 		sug;
    bool 		resultado;

    // Main internal methods:
    //
    void        restoreAssumption(int lit);
    bool        assume       (Lit p);
    void        cancelUntil  (int level);
    void        record       (const vec<Lit>& clause);

    void        analyze      (Clause* confl, vec<Lit>& out_learnt, int& out_btlevel); // (bt = backtrack)
    bool        removable    (Lit l, uint minl);

public:
    bool        enqueue      (Lit fact, Clause * from = NULL,int l_up=1);
    bool        enqueueMaxsat(Lit fact, Clause * from = NULL,int l_up=1);
    void 	createMaxSATStructures	(void);

private:
    int 	indexRest(void);
    int		pruneValues();
    void	removeSoft(Clause* c);
    Clause*     propagate    (void);
    Clause*     propagateMaxSAT(int limit_up);
    Clause*	addSoftClause(vec<Lit> & ps,Int w,Lit p,bool hard);
    void 	generateComp(Lit & clash,vec<Lit> & cA, vec<Lit> & cB);
    Int 	getClauseWeight(Clause *c);
    Int 	minimumWeight(Int a,Int b);
    void	paintClause(Clause *c1);
    void	compClauses(Clause*& c1,Clause*& c2,Lit & clash);
    void 	changeUnaryCost(Lit l,Int c);
    void 	executeNC(Lit & p, Lit & first);
    void 	createUnaryClauseAndNC(Lit & p, Lit & first,Clause* c);
    void 	applyAC(int lev);
    Int		applyLC();
    void	modifyUnitCost(Clause *c,Int cost);
    void	resolution(Clause*& c1,Clause*& c2,Lit & clash,bool pre);
    bool 	clashingClauses(Clause *c1,Clause *c2,Lit & clash);
    void 	preDetectInconsistency(Clause*& a,bool singleton);
    void 	detectInconsistency(Clause*& confl,bool singleton);
    Int		computeLB(int lev);
    void        reduceDB     (void);
    Lit         pickBranchLit(const SearchParams& params);
    lbool       search       (int nof_conflicts, int nof_learnts, const SearchParams& params);
    double      progressEstimate(void);

    // Activity:
    //
    void    calculateStaticOrdering(void);
    void    localSearch(void);
    void    DoHardening(void);
    int     calculateJeroslow(void);

    void    varBumpActivity(Lit p) {
    	if(opt_heur==2) // VSIDS
	{
        if (var_decay < 0) return;     // (negative decay means static variable order -- don't bump)
        if ( (activity[var(p)] += var_inc) > 1e100 ) varRescaleActivity();
        order.update(var(p));
	}
	}
    void    varBumpActivity(Lit p,double fact) {
    	if(opt_heur==1) {
        //if (var_decay < 0) return;     // (negative decay means static variable order -- don't bump)
        //if ( (activity[var(p)] += var_inc*fact) > 1e100 ) varRescaleActivity();
	activity[var(p)] += fact;
        order.update(var(p)); 
	}
	}

    void    varDecayActivity(void) { if (var_decay >= 0) var_inc *= var_decay; }
    void    varRescaleActivity(void);
    void    claDecayActivity(void) { cla_inc *= cla_decay; }
    void    claRescaleActivity(void);

    // Operations on clauses:
    //
    bool    clauseCreation(const vec<Lit>& ps_, bool learnt, Clause*& out_clause,Int w,bool h,bool tl);
    bool    newClause(const vec<Lit>& ps, bool learnt, Clause*& out_clause,bool tl);
    bool    newClause(const vec<Lit>& ps, bool learnt, Clause*& out_clause,Int w,bool h,bool tl);
    void    claBumpActivity (Clause* c) { if ( (c->activity() += cla_inc) > 1e20 ) claRescaleActivity(); }
    void    remove(Clause* c, bool just_dealloc = false);
    bool    locked          (const Clause* c) const { Clause * r = reason[var((*c)[0])]; return r == c; }
    bool    simplify        (Clause* c) const;

    int     decisionLevel(void) const { return trail_lim.size(); }
    // MaxSAT
    int     undoChronological   (void);
    bool    applyLearning(void) { return (irand(random_seed,2)==1); };
    void    doNothing(int rep) {for(int j=0;j<rep;j++){for(int i=0;i<nVars()*2;i++){NC[i]=NC[i]+13;}}};
    void    doNothingB(int rep,Lit p) {for(int j=0;j<rep;j++) { 
     vec<Clause *>&  ws = watches[index(p)];
     int noth;
     for(int i=0;i<ws.size();i++) noth++;
    } }

    void doNothingC(int rep)
    {
    	
	int noth;
    	for(int i=0;i<rep;i++)
	{
		for(int j=0;j<nVars();j++)
		{
			vec<Clause *>&  ws=watches[index(Lit(j))];
			noth=0;
			for(int k=0;k<ws.size();k++) noth++;
			
			vec<Clause *>&  wsn=watches[index(~Lit(j))];
			noth=0;
			for(int k=0;k<wsn.size();k++) noth++;

			
			j=j+1;
		}
	}
    }

public:
    Solver(void) : cla_inc          (1)
                 , cla_decay        (1)
                 , var_inc          (1)
                 , var_decay        (1)
                 , order            (assigns, activity)
                 , ok               (true)
                 , last_simplify    (-1)
                 , qhead            (0)
                 , progress_estimate(0)
                 , verbosity(0)
                 {
                     vec<Lit> dummy(2,lit_Undef);
                     void*   mem = xmalloc<char>(sizeof(Clause) + sizeof(uint)*2);
                     tmp_binary  = new (mem) Clause(false,dummy);
		     random_seed=91648253; allHard=false;
                 }

   ~Solver(void) {
       for (int i = 0; i < learnts.size(); i++) remove(learnts[i], true);
       for (int i = 0; i < clauses.size(); i++) remove(clauses[i], true); 
       // MAXSAT
       delete[] NC;
       delete[] bNC;
       delete[] usedLit;
       delete[] array_rest;
       delete[] array_heur;
       delete newClaAux;
       }

       
    // Helpers: (semi-internal)
    //
    lbool   value(Var x) const { return toLbool(assigns[x]); }
    lbool   value(Lit p) const { return sign(p) ? ~toLbool(assigns[var(p)]) : toLbool(assigns[var(p)]); }

    int     nAssigns(void) { return trail.size(); }
    int     nClauses(void) { return clauses.size(); }
    int     nLearnts(void) { return learnts.size(); }
    void    unitSoftClauses	(vec<Lit>& ps,vec<Int>& Cs);
    void    addUnitSoftClauses(Lit p,Int w);

    // Statistics: (read-only member variable)
    //
    SolverStats stats;

    // Problem specification:
    //
    void    setWeight(Clause *c,int w,bool h);
    Var     newVar (bool decision_var = true);
    int     nVars  (void)  { return assigns.size(); }
    bool    addUnit(Lit p) { if (ok) ok = enqueue(p); return ok; }
    bool    addClause(const vec<Lit>& ps) { if (ok){ Clause* c; ok = newClause(ps, false, c,true); if (c != NULL) clauses.push(c); } return ok; }
    
    //bool    addWeightedClause(const vec<Lit>& ps,int w,int h) { if (ok){ Clause* c; ok = newClause(ps, false, c); setWeight(c,w,h); if (c != NULL) clauses.push(c); } return ok; }
    bool    addWeightedClause(const vec<Lit>& ps,Int w,bool h) { if (ok){ Clause* c; ok = newClause(ps, false, c,w,h,true); if (c != NULL) clauses.push(c); } return ok; }
    void    setTop(Int v) { topUB=v; }

    // -- debug:
    void    countClauses(int & n_vars, int & n_clauses);
    void    exportClauses(cchar* filename);
    void    exportClauses2(cchar* filename);
    void    exportWcnfClauses(FILE *out,int hard);

    // Solving:
    //
    bool    okay(void) { return ok; }
    void    simplifyDB(void);
    bool    solve(const vec<Lit>& assumps);
    bool    solve(void) { vec<Lit> tmp; return solve(tmp); }
    void    showClauses();
    

    double      progress_estimate;  // Set by 'search()'.
    vec<lbool>  model;              // If problem is solved, this vector contains the model (if any).
    int         verbosity;          // Verbosity level. 0=silent, 1=some progress report, 2=everything
};


//=================================================================================================
}
#endif
