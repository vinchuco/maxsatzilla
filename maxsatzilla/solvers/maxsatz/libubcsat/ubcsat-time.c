/*

      ##  ##  #####    #####   $$$$$   $$$$   $$$$$$    
      ##  ##  ##  ##  ##      $$      $$  $$    $$      
      ##  ##  #####   ##       $$$$   $$$$$$    $$      
      ##  ##  ##  ##  ##          $$  $$  $$    $$      
       ####   #####    #####  $$$$$   $$  $$    $$      
  ======================================================
  SLS SAT Solver from The University of British Columbia
  ======================================================
  ...Developed by Dave Tompkins (davet [@] cs.ubc.ca)...
  ------------------------------------------------------
  .......consult legal.txt for legal information........
  ......consult revisions.txt for revision history......
  ------------------------------------------------------
  .....e-mail ubcsat-help [@] cs.ubc.ca for support.....
  ------------------------------------------------------

*/

#include "ubcsat.h"

double fTotalTime;
double fLastTimeStamp;

#ifdef WIN32

struct _timeb tstruct;

#else

struct timeval tv;
struct timezone tzp;
struct tms prog_tms;

#endif

#ifdef WIN32

double CurTime() {
  _ftime( &tstruct );
  return((double) tstruct.time + ((double)tstruct.millitm)/1000.0);
}

void InitSeed() {
  _ftime( &tstruct );
  iSeed = (( tstruct.time & 0x001FFFFF ) * 1000) + tstruct.millitm;
}

double TimeElapsed()
{
  double answer;
  double fNewTimeStamp;

  _ftime( &tstruct );
  fNewTimeStamp = CurTime();
  answer = fNewTimeStamp - fLastTimeStamp;
  fLastTimeStamp = fNewTimeStamp;
  return answer;
}


#else

void InitSeed() {
  
  gettimeofday(&tv,&tzp);
  iSeed = (( tv.tv_sec & 0x000007FF ) * 1000000) + tv.tv_usec;

}

double TimeElapsed() {

  double answer;

  times(&prog_tms);
  answer = (((double)prog_tms.tms_utime-fLastTimeStamp)/((double)sysconf(_SC_CLK_TCK)));
  fLastTimeStamp = (double)prog_tms.tms_utime;
  
  return answer;
}

double CurTime() {

  double answer;

  times(&prog_tms);
  answer = (((double)prog_tms.tms_utime-fLastTimeStamp)/((double)sysconf(_SC_CLK_TCK)));
  
  return answer;
}

#endif

void StartClock() {
  TimeElapsed();
}

void StopClock() {
  fTotalTime = TimeElapsed();
}

