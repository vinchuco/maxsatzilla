#ifndef READER_HH
#define READER_HH

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "math/pputils.hh"
#include "reader.hh"

using std::string;
using std::vector;
using std::ifstream;
using std::cout;

class Reader { 
public:
  Reader(const string &);
  virtual ~Reader() { }
  
protected:
  virtual void parseConfig() = 0;
  void eatSpaces();
  void eatLine();
  string getString();
  double getDouble();
  uint getUInt();
  template <typename T>
  vector<T> getVector();

  ifstream file;                      ///< File to read the configuration
};

template <typename T>
vector<T> Reader::getVector() {
  vector<T> vec;
  while(file.peek() != '\n') {
    T val;
    file >> val;
    vec.push_back(val);
  }
  return vec;
}

#endif // READER_HH
