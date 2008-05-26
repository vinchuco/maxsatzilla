/*----------------------------------------------------------------------------*\
 * Version: $Id: macros.hh 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: Macro definitions
 *
 *                                    Copyright (c) 2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#ifndef _MACROS_HH_
#define _MACROS_HH_ 1


/*----------------------------------------------------------------------------*\
 * Utility macros (minof, maxof, ...)
\*----------------------------------------------------------------------------*/

#define minof(x,y) ((x)<(y))?(x):(y)
#define maxof(x,y) ((x)>(y))?(x):(y)


/*----------------------------------------------------------------------------*\
 * Macros for printing generic debug info anf for controlling debug actions
\*----------------------------------------------------------------------------*/

#ifndef DBGMACROS
#define DBGMACROS
#define NDBG(x)
#define NDBGPRT(x)
#ifdef FULLDEBUG
#define DBG(x) x
#define DBGPRT(x) std::cout << x << std::endl; std::cout.flush();
#else
#define DBG(x)
#define DBGPRT(x)
#endif
#ifndef NCHECK
#define CHK(x) x
#define CHKPRT(x) std::cout << x << std::endl; std::cout.flush();
#else
#define CHK(x)
#define CHKPRT(x)
#endif
#endif


/*----------------------------------------------------------------------------*\
 * A more versatile (and verbose) assert macro; also a way to use assert code
\*----------------------------------------------------------------------------*/

#ifndef NDEBUG
#define verbassert(cond, message) if(!(cond)) {cerr<<message<<endl; assert(0);}
#define ASSERTCODE(x) x
#else
#define verbassert(cond, message) 
#define ASSERTCODE(x)
#endif

#define CHKCODE(x)  ASSERTCODE(x)


/*----------------------------------------------------------------------------*\
 * Macros for memory management
\*----------------------------------------------------------------------------*/

#define NEW(t,p) ((p) = (t*)malloc((long)sizeof *(p)))
#define NEWV(t,n,p) ((p) = (t*)malloc((long)(n*sizeof(*(p)))))
#define DEL(p) free(p);


#endif /* _MACROS_HH_ */

/*----------------------------------------------------------------------------*/
