/**
 **
 ** DO NOT CHANGE OR DELETE THIS FILE. 
 ** THIS FILE BELONGS TO THE ZILLA-COMMON PROJECT, BUT FOR PRACTICAL
 ** REASONS A COPY NEEDS TO LIVE IN THIS FOLDER.
 **
 **/

#include "reader.hh"

#include <iostream>

using std::cerr;

Reader::Reader(const string& path) 
  :  file(path.c_str()) {

  cout << "Parsing " << path << "\n";
  
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

