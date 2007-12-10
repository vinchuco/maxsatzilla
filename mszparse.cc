#include <iostream>
#include <fstream>

#include "mszreader.hh"

using namespace std;
using namespace iomsz;

int main( int argc, char* argv[] ) {
  if ( argc <= 1 ) exit(-1);

  unsigned int nbSolvers, nbFeatures, nbInstances, timeOut;
  string* solversNames, *featuresNames, *instancesNames;
  double** matrix;

  char * inputFileName = argv[1];
  ifstream in(inputFileName);
  if (in==NULL) {
    cerr<<"Error: Could not open file: "
	<<(inputFileName==NULL? "<stdin>": inputFileName)
	<<endl;
    exit(1);
  }

  parse_DIMACS(in, nbSolvers, nbFeatures, nbInstances, timeOut, solversNames, featuresNames, instancesNames, matrix);

  return 0;
}
