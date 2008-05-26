#include "reader.hh"

#include <iostream>

using std::cerr;

Reader::Reader(const string& path) 
  :  file(path.c_str()) {

#ifndef NDEBUG
  cout << "Parsing " << path << "\n";
#endif 

  if(!file.is_open()) {
    cerr << "Unable to open config file.\n";
    exit(EXIT_FAILURE);
  }
}


void Reader::eatSpaces() {
  while(isspace(file.peek())) file.get();
}

void Reader::eatLine() {
  while(file.peek() != '\n') file.get();
  eatSpaces();
}

string Reader::getString() {
  eatSpaces();
  string str;
  while(!isspace(file.peek())) {
    char c = file.get();
    str += c;
  }
  return str;
}

double Reader::getDouble() {
  double num;
  file >> num;
  return num;
}

uint Reader::getUInt() {
  uint val;
  file >> val;
  return val;
}

