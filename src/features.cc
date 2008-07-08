
#include <iostream>
#include "MaxSatInstance.hh"

using namespace std;

int main( int argc, char* argv[] ) {
  if ( argc <= 1 ) {
    printf("usage: %s instance-file\n", argv[0] );
    exit(0);
  }

  MaxSatInstance msi( argv[1] );
  msi.computeLocalSearchProperties(cout);
  msi.printInfo(cout);

  return 0;
}
