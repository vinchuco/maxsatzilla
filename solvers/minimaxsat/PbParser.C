#include "PbParser.h"
#include "File.h"
#include <zlib.h>

#define CHUNK_LIMIT 1048576



//=================================================================================================
// Parser buffers (streams):


class StreamBuffer {
    gzFile  in;
    char    buf[CHUNK_LIMIT];
    int     pos;
    int     size;

    void assureLookahead() {
        if (pos >= size) {
	    line++;
            pos  = 0;
            size = gzread(in, buf, sizeof(buf)); } }

public:
    int     line;
    StreamBuffer(gzFile i) : in(i), pos(0), size(0) { line=1;
        assureLookahead(); }
    ~StreamBuffer() {}

    int  operator *  () { return (pos >= size) ? EOF : buf[pos]; }
    void operator ++ () { pos++; assureLookahead();}
};




//=================================================================================================
// PB Parser:


/*
The 'B' (parser Buffer) parameter should implement:
    
    operator *      -- Peek at current token. Should return a character 0-255 or 'EOF'.
    operator ++     -- Advance to next token.
    line            -- Public member variable.

The 'S' (Solver) parameter should implement:

    void allocConstrs(int n_vars, int n_constrs)
        -- Called before any of the below methods. Sets the size of the problem.

    int  getVar(cchar* name)
        -- Called during parsing to convert string names to indices. Ownership of 'name' 
        remains with caller (the string should be copied).

    void addGoal(const vec<Lit>& ps, const vec<Int>& Cs)
        -- Called before any constraint is adde to establish goal function:
                "minimize( Cs[0]*ps[0] + ... + Cs[n-1]*ps[n-1] )"

    bool addConstr(const vec<Lit>& ps, const vec<Int>& Cs, Int rhs, int ineq)
        -- Called when a constraint has been parsed. Constraint is of type:
                "Cs[0]*ps[0] + ... + Cs[n-1]*ps[n-1] >= rhs" ('rhs'=right-hand side). 
        'ineq' determines the inequality used: -2 for <, -1 for <=, 0 for ==, 1 for >=, 2 for >. 
        Should return TRUE if successful, FALSE if conflict detected.
*/


template<class B>
static void skipWhitespace(B& in) {     // not including newline
    while (*in == ' ' || *in == '\t')
        ++in; }

template<class B>
static void skipLine(B& in) {
    for (;;){
        if (*in == EOF) return;
        if (*in == '\n') { ++in; return; }
        ++in; } }

template<class B>
static void skipComments(B& in) {      // skip comment and empty lines (assuming we are at beginning of line)
    while (*in == '*' || *in == '\n') skipLine(in); }

template<class B>
static bool skipEndOfLine(B& in) {     // skip newline AND trailing comment/empty lines
    if (*in == '\n') ++in;
    else             return false;
    skipComments(in);
    return true; }

template<class B>
static bool skipText(B& in, cchar* text) {
    while (*text != 0){
        if (*in != *text) return false;
        ++in, ++text; }
    return true; }

template<class B>
static Int parseInt(B& in) {
    Int     val(0);
    bool    neg = false;
    skipWhitespace(in);
    if      (*in == '-') neg = true, ++in;
    else if (*in == '+') ++in;
    skipWhitespace(in);     // BE NICE: allow "- 3" and "+  4" etc.
    if (*in < '0' || *in > '9')
        throw nsprintf("Expected digit, not: %c", *in);
    while (*in >= '0' && *in <= '9'){
      #ifdef NO_GMP
        val *= 2;
        //if (val < 0 || val > Int(9223372036854775807LL >> 20)) throw xstrdup("Integer overflow. Use BigNum-version.");      // (20 extra bits should be enough...)
        val *= 5;
      #else
        val *= 10;
      #endif
        val += (*in - '0');
        ++in; }
    return neg ? -val : val; }

template<class B>
static char* parseIdent(B& in, vec<char>& tmp) {   // 'tmp' is cleared, then filled with the parsed string. '(char*)tmp' is returned for convenience.
    skipWhitespace(in);
    if ((*in < 'a' || *in > 'z') && (*in < 'A' || *in > 'Z') && *in != '_') throw nsprintf("Expected start of identifier, not: %c ",*in);
    tmp.clear();
    tmp.push(*in);
    ++in;
    while ((*in >= 'a' && *in <= 'z') || (*in >= 'A' && *in <= 'Z') || (*in >= '0' && *in <= '9') || *in == '_')
        tmp.push(*in),
        ++in;
    tmp.push(0);
    return (char*)tmp; }


template<class B, class S>
void parseExpr(B& in, S& solver, vec<Lit>& out_ps, vec<Int>& out_Cs, vec<char>& tmp)
    // NOTE! only uses "getVar()" method of solver; doesn't add anything.
    // 'tmp' is a tempory, passed to avoid frequent memory reallocation.
{
    bool empty = true;
    for(;;){
        skipWhitespace(in);
        if ((*in < '0' || *in > '9') && *in != '+' && *in != '-') break;
        out_Cs.push(parseInt(in));
        skipWhitespace(in);
        //if (*in != '*') throw xstrdup("Missing '*' after coefficient.");
        //++in;
        out_ps.push(Lit(solver.getVar(parseIdent(in, tmp))));
        empty = false;
    }
    if (empty) throw xstrdup("Empty expression.");
}


template<class B, class S>
void parseSize(B& in, S& solver)
{
    int n_vars, n_constrs;

    if (*in != '*') return;
    ++in;
    skipWhitespace(in);

    if (!skipText(in, "#variable=")) goto Abort;
    n_vars = toint(parseInt(in));

    skipWhitespace(in);
    if (!skipText(in, "#constraint=")) goto Abort;
    n_constrs = toint(parseInt(in));

    solver.allocConstrs(n_vars, n_constrs);

  Abort:
    skipLine(in);
    skipComments(in);
}

template<class B, class S>
void parseGoal(B& in, S& solver)
{
    skipWhitespace(in);
    if (!skipText(in, "min:")) return;      // No goal specified. If file is syntactically correct, no characters will have been consumed (expecting integer).

    vec<Lit> ps; vec<Int> Cs; vec<char> tmp;
    skipWhitespace(in);
    if (*in == ';'){
        ++in;
        skipLine(in);
    }else{
        parseExpr(in, solver, ps, Cs, tmp);
        skipWhitespace(in);
        if (!skipText(in, ";")) throw xstrdup("Expecting ';' after goal function.");
    }
    skipEndOfLine(in);

    solver.addGoal(ps, Cs);
}

template<class B>
int parseInequality(B& in)
{
    int ineq;
    skipWhitespace(in);
    if (*in == '<'){
        ++in;
        if (*in == '=') ineq = -1, ++in;
        else            ineq = -2;
    }else if (*in == '>'){
        ++in;
        if (*in == '=') ineq = +1, ++in;
        else            ineq = +2;
    }else{
        if (*in == '='){
            ++in;
            if (*in == '=') ++in;
            ineq = 0;
        }else
            throw nsprintf("Expected inequality, not: %c", *in);
    }
    return ineq;
}

template<class B, class S>
bool parseConstrs(B& in, S& solver)
{
    vec<Lit> ps; vec<Int> Cs; vec<char> tmp;
    int     ineq;
    Int     rhs;
    while (*in != EOF){
        parseExpr(in, solver, ps, Cs, tmp);
        ineq = parseInequality(in);
        rhs  = parseInt(in);

        skipWhitespace(in);
        if (!skipText(in, ";")) throw xstrdup("Expecting ';' after constraint.");
        skipEndOfLine(in);

        if (!solver.addConstr(ps, Cs, rhs, ineq))
            return false;
        ps.clear();
        Cs.clear();
    }
    return true;
}


//=================================================================================================
// Main parser functions:


template<class B, class S>
static bool parse_PB(B& in, S& solver, bool abort_on_error)
{
    try{
        parseSize(in, solver);
        parseGoal(in, solver);
        return parseConstrs(in, solver);
    }catch (cchar* msg){
        if (abort_on_error){
            reportf("PARSE ERROR! [line %d] %s\n", in.line, msg);
            xfree(msg);
            if (opt_satlive && !opt_try)
                printf("s UNKNOWN\n");
            exit(opt_try ? 5 : 0);
        }else
            throw msg;
    }

}

// PB parser functions: Returns TRUE if successful, FALSE if conflict detected during parsing.
// If 'abort_on_error' is false, a 'cchar*' error message may be thrown.
//

void parse_PB_file(cchar* filename, PbSolver& solver, bool abort_on_error) 
{
    gzFile buf = gzopen(filename, "rb");
    if (buf == NULL)
	fprintf(stderr, "ERROR! Could not open file: %s\n",filename),
	exit(1);
    StreamBuffer in(buf);
    parse_PB(in, solver, abort_on_error);
    gzclose(buf);
}

//=================================================================================================
// DIMACS Parser:


template<class B>
static void skipWhitespaceCnf(B& in) {
    while ((*in >= 9 && *in <= 13) || *in == 32)
        ++in; }

template<class B>
static void skipLineCnf(B& in) {
    for (;;){
        if (*in == EOF) return;
        if (*in == '\n') { ++in; return; }
        ++in; } }

template<class B>
static Int parseIntCnf(B& in) {
    Int     val = 0;
    bool    neg = false;
    skipWhitespaceCnf(in);
    if      (*in == '-') neg = true, ++in;
    else if (*in == '+') ++in;
    if (*in < '0' || *in > '9') fprintf(stderr, "PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
    while (*in >= '0' && *in <= '9')
        val = val*10 + (*in - '0'),
        ++in;
    return neg ? -val : val; }

template<class B, class mS>
static Int readClauseCnf(B& in,  mS& S, vec<Lit>& lits,bool wcnf) {
    int     parsed_lit, var;
    Int weight=1;
    if(wcnf) weight = parseIntCnf(in);
    lits.clear();
    for (;;){
        parsed_lit = toint(parseIntCnf(in));
        if (parsed_lit == 0) break;
        var = abs(parsed_lit)-1;
        while (var >= S.nVars()) S.newVar();
        lits.push( (parsed_lit > 0) ? Lit(var) : ~Lit(var) );
    }
    return weight;
}

template<class B, class mS>
static void parse_DIMACS_main(B& in, mS& S) {
    vec<Lit> lits;
    Int w;
    Int max=-1;
    bool wcnf=false;
    for (;;){
        skipWhitespaceCnf(in);
        if (*in == EOF)
            break;
        else if (*in == 'c') skipLineCnf(in);
	else if(*in == 'p')
	{
		++in;
		skipWhitespaceCnf(in);
		if(*in== 'w') wcnf=true;
		if (wcnf) {reportf("Parsing WCNF file...\n"); ++in; ++in; ++in; ++in;}
		else { reportf("Parsing CNF file...\n"); ++in; ++in; ++in;}
		//if (wcnf) {reportf("Parsing WCNF file...\n"); }
		//else { reportf("Parsing CNF file...\n");}

		parseIntCnf(in); // Number of variables
		parseIntCnf(in); // Number of clauses
		while(not (*in >= '0' and *in <= '9') and not(*in == '\n')){
			++in; 
		} 
		
		S.setTop(-1);
		
		if (*in >= '0' and *in <= '9') max=parseIntCnf(in);
		if(max!=-1) { 
			reportf("UB %d \n",toint(max)); 
			S.setTop(max);
		}
		skipLineCnf(in);
	}
        else
	{
            w=readClauseCnf(in, S, lits,wcnf);
	    if(opt_sat==1) // Satisfiability
		S.addWeightedClause(lits,w,true);
	    else  {// Max-SAT 
	    	if(lits.size()>1)
		{
			if(max >-1 and w>=max) S.addWeightedClause(lits,max,true);
			else S.addWeightedClause(lits,w,false);
		}
		else {
			if(max >-1 and w>=max) S.addWeightedClause(lits,max,true);
			else S.addUnitSoftClauses(lits[0],w);
		}
	    }
	}

		
    }
}

// Inserts problem into solver.
    
void parse_WCNF_file(cchar* filename, PbSolver& solver, bool abort_on_error)
{
    
    gzFile buf = gzopen(filename, "rb");
    if (buf == NULL)
	fprintf(stderr, "ERROR! Could not open file: %s\n",filename),
	exit(1);
    StreamBuffer in(buf);
    parse_DIMACS_main(in, solver.getSolver());
    gzclose(buf);

}

//=================================================================================================



//=================================================================================================
// Debug:


#if 0
#include "Debug.h"
#include "Map.h"
#define Solver DummySolver

struct DummySolver {
    Map<cchar*, int> name2index;
    vec<cchar*>      index2name;

    int getVar(cchar* name) {
        int ret;
        if (!name2index.peek(name, ret)){
            index2name.push(xstrdup(name));
            ret = name2index.set(index2name.last(), index2name.size()-1); }
        return ret; }

    void alloc(int n_vars, int n_constrs) {
        printf("alloc(%d, %d)\n", n_vars, n_constrs); }
    void addGoal(vec<Lit>& ps, vec<Int>& Cs) {
        printf("MIN: "); dump(ps, Cs); printf("\n"); }
    bool addConstr(vec<Lit>& ps, vec<Int>& Cs, Int rhs, int ineq) {
        static cchar* ineq_name[5] = { "<", "<=" ,"==", ">=", ">" };
        printf("CONSTR: "); dump(ps, Cs); printf(" %s ", ineq_name[ineq+2]); dump(rhs); printf("\n");
        return true; }
};

void test(void)
{
    DummySolver     solver;
    debug_names = &solver.index2name;
    parseFile_PB("test.pb", solver, true);
}
#endif
