/** 
 * @file mszreader.hh Defines functions for parsing an msz file
 *
 * @author Florian Letombe & Paulo Matos & Jordi Planes
 * @since November, 2007
 *
 * Pieces of code extracted from MiniSat 
 *
 * MiniSat -- Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 * OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef MSZREADER__H__
#define MSZREADER__H__

//#include <ctime>
//#include <cstring>
//#include <cassert>
//#include <stdint.h>
//#include <errno.h>

//#include <signal.h>
//#include <zlib.h>

//#include <string>

using namespace std;

namespace iomsz {

  /*************************************************************************************/
#if defined(__linux__)
  static inline int memReadStat(int field) {
    char    name[256];
    pid_t pid = getpid();
    sprintf(name, "/proc/%d/statm", pid);
    FILE*   in = fopen(name, "rb");
    if (in == NULL) return 0;
    int     value;
    for (; field >= 0; field--)
      fscanf(in, "%d", &value);
    fclose(in);
    return value;
  }
  static inline uint64_t memUsed() { return (uint64_t)memReadStat(0) * (uint64_t)getpagesize(); }


#elif defined(__FreeBSD__)
  static inline uint64_t memUsed(void) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return ru.ru_maxrss*1024;
  }

#else
  static inline uint64_t memUsed() { return 0; }
#endif

#if defined(__linux__)
#include <fpu_control.h>
#endif

  //=================================================================================================
  // DIMACS Parser:

#define CHUNK_LIMIT 1048576
#define reportf(format, args...) ( fflush(stdout), fprintf(stderr, format, ## args), fflush(stderr) )
  /*
  class StreamBuffer {
    istream&  in;
    char    buf[CHUNK_LIMIT];
    int     pos;
    int     size;

  public:
  StreamBuffer(istream& i) : in(i), pos(0), size(0) { }

    int  operator *  () { return (pos >= size) ? EOF : buf[pos];}
    void operator ++ () { pos++; }
  };
  */
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  class StreamBuffer {
    istream&  in;
    char    buf[CHUNK_LIMIT];
    int     pos;
    int     size;
    
    void assureLookahead() {
      if (pos >= size) {
       pos  = 0;
       size = in.readsome( buf, sizeof(buf));
      }
    }
    
  public:
    StreamBuffer(istream& i) : in(i), pos(0), size(0) { assureLookahead(); }
    
    int  operator *  () { return (pos >= size) ? EOF : buf[pos];}
    void operator ++ () { pos++; assureLookahead(); }
  };
  
  template<class B>
    static void skipWhitespace(B& in) {
    while ((*in >= 9 && *in <= 13) || *in == ' ')
      ++in;
  }

  template<class B>
    static void skipLine(B& in) {
    for (;;){
      if (*in == EOF || *in == '\0') return;
      if (*in == '\n') { ++in; return; }
      ++in;
    }
  }

  template<class B>
    static int parseInt(B& in) {
    int     val = 0;
    bool    neg = false;
    skipWhitespace(in);
    if      (*in == '-') neg = true, ++in;
    else if (*in == '+') ++in;
    if (*in < '0' || *in > '9') reportf("PARSE ERROR! (parseInt) Unexpected char: %c\n", *in), exit(3);
    while (*in >= '0' && *in <= '9')
      val = val*10 + (*in - '0'),
	++in;
    return neg ? -val : val;
  }

  template<class B>
  static int parseDecimal(B& in, int& nbDec=0) {
    int     val = 0;
    if (*in < '0' || *in > '9') reportf("PARSE ERROR! (parseDecimal) Unexpected char: %c\n", *in), exit(3);
    while (*in >= '0' && *in <= '9')
      val = val*10 + (*in - '0'),
	++in, ++nbDec;
    return val;
  }

  template<class B>
    static double parseDouble(B& in) {
    int     valReal, nbDec=0;
    double valDecimal;
    valReal=parseInt(in);

    if (*in == '.') {
      ++in;
      valDecimal=(double)parseDecimal(in,nbDec);
    }
    else return (double)valReal;

    for (;nbDec!=0; nbDec--)
      valDecimal/=10;
    return valReal+valDecimal;
  }

  template<class B>
    static void readWord(B& in, string& word) {
    skipWhitespace(in);
    for (; (*in < 9 || *in > 13) && *in != ' ';) {
      word+=*in;
      ++in;
    }
  }

  template<class B>
    static bool match(B& in, char* str) {
    for (; *str != 0; ++str, ++in)
      if (*str != *in)
	return false;
    return true;
  }

  template<class B>
     static void parse_MSZ_parameters(B& in,
				      unsigned int& nbSolvers,
				      unsigned int& nbFeatures,
				      unsigned int& nbInstances,
				      unsigned int& timeOut, 			       
				      string*& solversNames,
				      string*& featuresNames,
				      string*& instancesNames,
				      double**& matrix) {
    ++in;++in;
    if ( match(in, "msz") ) {
      nbSolvers = parseInt(in);
      solversNames=new string [nbSolvers];
      nbFeatures = parseInt(in);
      featuresNames=new string [nbFeatures];
      nbInstances = parseInt(in);
      instancesNames=new string [nbInstances];
      
      const int nbCases = nbSolvers+nbFeatures;
      matrix=new double* [nbInstances];
      for (unsigned int i=0; i<nbInstances; i++)
	matrix[i]=new double [nbCases];
      timeOut = parseInt(in);
      reportf("c Number of solvers:    %2d\n", nbSolvers);
      reportf("c Number of features:   %2d\n", nbFeatures);
      reportf("c Number of instances:  %2d\n", nbInstances);
      reportf("c Timeout:              %2d\n", timeOut);
    } else if ( match(in, "slv") ) {
      reportf("c Solvers:");
      for (unsigned int i=0; i<nbSolvers; i++) {
	readWord(in,solversNames[i]);
	reportf(" %s", solversNames[i].c_str());
      }
      reportf("\n");
    }
    else if ( match(in, "ftr") ) {
      reportf("c Features:");
      for (unsigned int i=0; i<nbFeatures; i++) {
	readWord(in,featuresNames[i]);
	reportf(" %s", featuresNames[i].c_str());
      }
      reportf("\n");
    }
    else reportf("PARSE ERROR! (parse_MSZ) Unexpected params chars: %c %c %c\n", *in, *in, *in), exit(3);
  }

  template<class B>
    static void parse_MSZ_main(B& in,
			       unsigned int& nbSolvers,
			       unsigned int& nbFeatures,
			       unsigned int& nbInstances,
			       unsigned int& timeOut,
			       string*& solversNames,
			       string*& featuresNames,
			       string*& instancesNames,
			       double**& matrix) {
    unsigned int inst = 0;
    for(;;){
      skipWhitespace(in);
      if (*in == EOF)
	break;
      else if (*in == 'p'){
	parse_MSZ_parameters( in, nbSolvers, nbFeatures, nbInstances, timeOut, solversNames, featuresNames, instancesNames, matrix );
      }
      else if (*in == 'c') 
	skipLine(in); 
      else if (*in == 'i') {
	++in;
	readWord(in,instancesNames[inst]);
	reportf("c Instance: %s\n", instancesNames[inst].c_str());
      } else {
	printf("Current character : %c\n", *in );
	reportf("c matrix:\n");
	for (unsigned int i=0; i<nbSolvers; i++) {
	  matrix[inst][i]=parseDouble(in);
	  reportf(" %f", matrix[inst][i]);
	  printf("Iteration %d\n", i);
	}
	reportf("\n");
	skipWhitespace(in);
	for (unsigned int i=nbSolvers; i<nbSolvers+nbFeatures; i++) {
	  matrix[inst][i]=parseDouble(in);
	  reportf(" %f", matrix[inst][i]);
	  printf("Iteration %d\n", i);
	}
	reportf("\n");
	inst++;
      }
    }
  }

  // Inserts problem into solver.
  //
  static void parse_DIMACS(istream& input_stream,
			   unsigned int& nbSolvers,
			   unsigned int& nbFeatures,
			   unsigned int& nbInstances,
			   unsigned int& timeOut,
			   string*& solversNames,
			   string*& featuresNames,
			   string*& instancesNames,
			   double**& matrix) {
    StreamBuffer in(input_stream);
    parse_MSZ_main(in, nbSolvers, nbFeatures, nbInstances, timeOut, solversNames, featuresNames, instancesNames, matrix);
  }

} // end namespace iomsz

#endif // MSZREADER__H__
