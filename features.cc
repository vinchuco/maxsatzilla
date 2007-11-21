
#include <iostream>
#include "MaxSatInstance.hh"

using namespace std;

int main( int argc, char* argv[] ) {
  if ( argc <= 1 ) exit(-1);

  MaxSatInstance msi( argv[1] );
  msi.computeLocalSearchProperties();
  msi.printInfo( cout );

  return 0;
}
