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

  cerr << "\n\n\nPOCM part... from now, all problems are MINE! :)\n";

  cerr << "Read file: " << inputFileName << "\n"
       << "Number of Solvers: " << nbSolvers << "\n"
       << "Number of Features: " << nbFeatures << "\n"
       << "Number of Instances: " << nbInstances << "\n"
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
  cerr << "\n";

  MSZDataSet *ds = createDataSet(data, nbInstances, nbFeatures+nbSolvers, nbSolvers);

  // Lets create the plot files
  vector<string> labels;
  for(size_t i = 0; i < nbSolvers; i++)
    labels.push_back(solversNames[i]);
  for(size_t i = 0; i < nbFeatures; i++)
    labels.push_back(featuresNames[i]);

  cerr << "Created labels for solvers and features of size : " << labels.size() << "\n";
  cerr << "features (" << nbFeatures << ") + solvers(" << nbSolvers << ") = " << labels.size() << "\n";
  ds->dumpPlotFiles(labels, "./driver");

  // Let's apply dataset transformations
  ds->standardize();
  ds->standardizeOutputs();
  ds->expand(1); // always calls standardize() if you didn't before

  // Lets do a forward selection
  ForwardSelection fs(*ds, 1);
  vector<size_t> res = fs.run(0.15);
  
  ds->removeFeatures(res);

  RidgeRegression rr(*ds);
  rr.run(0.1, 1, "./msuncore-model.hh");

  // Let's not forget to delete the dataset
  delete ds;
  
  return 0;
}
