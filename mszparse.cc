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

  delete[] solversNames;
  delete[] featuresNames;
  delete[] instancesNames;
  for(size_t i = 0; i < nbSolvers+nbInstances; i++)
    delete[] matrix[i];
  delete[] matrix;

  return 0;
}
