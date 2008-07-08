
#include <iostream>
#include "MaxSatInstance.hh"

#include <cstdlib>

using namespace std;

int main( int argc, char* argv[] ) {
  if ( argc <= 1 ) {
    printf("usage: %s instance-file\n", argv[0] );
    exit(EXIT_FAILURE);
  }

  MaxSatInstance msi( argv[1] );
  msi.computeLocalSearchProperties(cout);
  msi.printInfo(cout);

  return 0;
}
