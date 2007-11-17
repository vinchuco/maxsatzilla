#include <iostream>

#include "dataset.hh"

static double data[][] = 
  {{900.0,   1.0,   100.0, 100.0,   120.3},
   {9000.0,  10.0,  150.0, 1000.0,  99.5},
   {90000.0, 100.0, 200.0, 10000.0, 950.2}};

static char* labels[] = {"Solver1", "Solver2", "Vars", "Clauses", "SomethingElse"};

int main(void) {

  MSZDataSet *ds = createDataSet(data, 3, 5, 2);
  
  ds->dumpPlotFiles(labels, "./driver_");

  return;
}
