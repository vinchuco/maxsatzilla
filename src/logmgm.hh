#ifndef LOGMGM_HH
#define LOGMGM_HH

#include <stack>
#include <string>
#include <vector>
#include <fstream>

#include <time.h>

using std::vector;
using std::string;
using std::stack;
using std::ofstream;

class LogMgm {
public:
  LogMgm();
  ~LogMgm();
  enum LogCat {FORSEL = 0, RIDREG, SYSTEM, PARSER, SVMREG, TOHAND, TESTNG, NUMCATS};

  static LogMgm *Instance();

  void setOutputPath(const string&);
  string getLogPath() const { return logPath; }
  void setCategory(LogCat);
  void endCategory();

private:
  bool setupDone;
  vector<ofstream*> streams;
  stack<LogCat> catStack;
  string logPath;

  template<class T> friend
  LogMgm& operator<<(LogMgm &lm, const T& t) {
    *(lm.streams[lm.catStack.top()]) << t;
    return lm;
  }

};



#endif // LOGMGM_HH
