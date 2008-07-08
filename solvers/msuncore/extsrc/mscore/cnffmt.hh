/*----------------------------------------------------------------------------*\
 * File:        cnffmt.hh
 *
 * Description: Class definitions for CNF parser, based extensively on
 *              MiniSAT parser, but using the STL.
 *              NOTE: When linking, option -lz *must* be used
 *
 * Author:      jpms
 * 
 * Revision:    $Id: cnffmt.hh 73 2007-07-26 15:16:48Z jpms $.
 *
 *                                     Copyright (c) 2007, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#ifndef _CNFFMT_H
#define _CNFFMT_H 1

#include <ctime>
#include <unistd.h>
#include <signal.h>
#include <zlib.h>

#include <vector>

#include "globals.hh"
#include "std_clause.hh"
#include "cnf_store.hh"

using namespace std;


//=============================================================================
// DIMACS Parser: (This borrows **extensively** from the MiniSAT parse)

#define CHUNK_LIMIT 1048576

class StreamBuffer {
  gzFile  in;
  char    buf[CHUNK_LIMIT];
  int     pos;
  int     size;

  void assureLookahead() {
    if (pos >= size) {
      pos  = 0;
      size = gzread(in, buf, sizeof(buf)); } }

public:
  StreamBuffer(gzFile i) : in(i), pos(0), size(0) {
    assureLookahead(); }

  int  operator *  () { return (pos >= size) ? EOF : buf[pos]; }
  void operator ++ () { pos++; assureLookahead(); }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template<class B>
static void skipWhitespace(B& in) {
    while ((*in >= 9 && *in <= 13) || *in == 32)
        ++in; }

template<class B>
static void skipLine(B& in) {
    for (;;){
        if (*in == EOF) return;
        if (*in == '\n') { ++in; return; }
        ++in; } }

template<class B>
static int parseInt(B& in) {
    int     val = 0;
    bool    neg = false;
    skipWhitespace(in);
    if      (*in == '-') neg = true, ++in;
    else if (*in == '+') ++in;
    if (*in < '0' || *in > '9') fprintf(stderr, "PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
    while (*in >= '0' && *in <= '9')
        val = val*10 + (*in - '0'),
        ++in;
    return neg ? -val : val; }

template<class B>
static void readClause(B& in, CNFStore& S, vector<int>& lits) {
  int parsed_lit;
  lits.clear();
  for (;;){
    parsed_lit = parseInt(in);
    if (parsed_lit == 0) break;
    lits.push_back(parsed_lit);
  }
}

template<class B>
static void parse_DIMACS_main(B& in, CNFStore& S, ClIDMap* clidmap=NULL) {
  vector<int> lits;
  int clcnt = 0;
  for (;;){
    skipWhitespace(in);
    if (*in == EOF)
      break;
    else if (*in == 'c' || *in == 'p')
      skipLine(in);
    else {
      readClause(in, S, lits);
      ClauseRef clref = S.add_clause(lits);
      if (clidmap) { clidmap->insert(clref, ++clcnt); }
    }
  }
}

class CNFParser {
public:
  inline void load_cnf_file(gzFile input_stream, CNFStore& S) {
    StreamBuffer in(input_stream);
    parse_DIMACS_main(in, S); }

  inline void load_cnf_file(gzFile input_stream, CNFStore& S, ClIDMap& clidmap) {
    StreamBuffer in(input_stream);
    parse_DIMACS_main(in, S, &clidmap); }
};

//=============================================================================

#endif /* _CNFFMT_H */

/*----------------------------------------------------------------------------*/
