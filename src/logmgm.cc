#include "logmgm.hh"

#include <iostream>
#include <sys/stat.h>

using std::cout;

LogMgm *LogMgm::Instance() {
  static LogMgm inst;
  return &inst;
}

LogMgm::LogMgm() : setupDone(false), streams(NUMCATS) {
  catStack.push(SYSTEM);
}

LogMgm::~LogMgm() {

  for(unsigned int i = 0; i < streams.size(); ++i) {
    streams[i]->close();
    delete streams[i];
  }
  
}

void LogMgm::setOutputPath(const string& path) {

  cout << "Log output set to: " << path << "\n";

  int err = mkdir(path.c_str(), S_IRWXU);

  if(err == 0) {
    // Let's open up all the streams
    streams[FORSEL] = new ofstream((path + "/forwardselection.log").c_str());
    streams[RIDREG] = new ofstream((path + "/ridgeregression.log").c_str());
    streams[SYSTEM] = new ofstream((path + "/system.log").c_str());
    streams[PARSER] = new ofstream((path + "/parsing.log").c_str());

    setupDone = true;
    logPath = path;
    return;
  } else {
    setupDone = false;
    perror("Couldn't create log directory.");
  }
}

void LogMgm::setCategory(LogCat cat) {
  catStack.push(cat);
}

void LogMgm::endCategory() {
  if(catStack.size() != 1)
    catStack.pop();
}



