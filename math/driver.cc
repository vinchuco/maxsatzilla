#include <iostream>
#include <vector>

#include <zlib.h>

#include "../mszreader.hh"

#include "dataset.hh"
#include "forwardselection.hh"
#include "ridgeregression.hh"

using std::vector;
using std::string;
using std::cerr;

using namespace iomsz;

#define FIN 0.15

int main(int argc, char *argv[]) {

   if (argc <= 1) exit(EXIT_FAILURE);

  unsigned int nbSolvers, nbFeatures, nbInstances, timeOut;
  string *solversNames;
  string *featuresNames;
  string *instancesNames;
  double **data;
  
  char *inputFileName = argv[1];

  gzFile in=(inputFileName==NULL? gzdopen(0,"rb"): gzopen(inputFileName,"rb"));
  if (in==NULL) {
    cerr<<"Error: Could not open file: "
        <<(inputFileName==NULL? "<stdin>": inputFileName)
        <<endl;
    exit(1);
  }

  parse_DIMACS(in, nbSolvers, nbFeatures, nbInstances, timeOut, solversNames, featuresNames, instancesNames, data);

  cerr << "Read file: " << inputFileName << "\n"
       << "Number of Solvers: " << nbSolvers << "\n"
       << "Number of Features: " << nbFeatures << "\n"
       << "Timeout: " << timeOut << "\n";

  cerr << "Solver Names: ";
  for(size_t i = 0; i < nbSolvers; i++)
    cerr << solversNames[i] << " ";

  cerr << "\nFeature Names: ";
  for(size_t i = 0; i < nbFeatures; i++)
    cerr << featuresNames[i] << " ";
  
  cerr << "\nInstance Names: ";
  for(size_t i = 0; i < nbInstances; i++)
    cerr << instancesNames[i] << " ";

  MSZDataSet *ds = createDataSet(data, 3, 5, 2);

  // Lets create the plot files
  //ds->dumpPlotFiles(labels, 5, "./driver");

  // Lets do a forward selection
  ForwardSelection fs(*ds, 0);

  vector<size_t> res = fs.run(FIN);
  
  // Let's not forget to delete the dataset
  delete ds;
  
  return 0;
}
