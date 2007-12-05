#include <iostream>
#include <vector>

#include <zlib.h>

#include "mszreader.hh"

#include "math/dataset.hh"
#include "math/forwardselection.hh"
#include "math/ridgeregression.hh"

using std::vector;
using std::string;
using std::cerr;

using namespace iomsz;

/**
 * Main function for MaxSATzilla training.
 * This function received 3 arguments:
 * - Forward Selection Threshold;
 * - Ridge Regression Scalar Delta;
 * - Name of the file with training set;
 * - Prefix for header output;
 */
int main(int argc, char *argv[]) {

  if (argc <= 1 || argc >= 6) {
    cerr << "usage: coach <fsthreshold> <delta> <trainingset> <headerprefix>\n";
    exit(EXIT_FAILURE);
  }

  unsigned int nbSolvers, nbFeatures, nbInstances, timeOut;
  string *solversNames;
  string *featuresNames;
  string *instancesNames;
  double **data;
  
  double threshold = atof(argv[1]);
  double delta = atof(argv[2]);
  char *inputFileName = argv[3];
  char *headerPrefix = argv[4];
  
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

  try {
    MSZDataSet *ds = createDataSet(data, nbInstances, nbFeatures+nbSolvers, nbSolvers);
    
    vector<string> snames;
    for(size_t s = 0; s < nbSolvers; s++) snames.push_back(solversNames[s]);
    ds->printSolverStats(timeOut, snames);
    
    // Lets create the plot files
    vector<string> labels;
    for(size_t i = 0; i < nbSolvers; i++)
      labels.push_back(solversNames[i]);
    for(size_t i = 0; i < nbFeatures; i++)
      labels.push_back(featuresNames[i]);
    
    cerr << "Created labels for solvers and features of size : " << labels.size() << "\n";
    cerr << "features (" << nbFeatures << ") + solvers(" << nbSolvers << ") = " << labels.size() << "\n";
    ds->dumpPlotFiles(labels, "./coach");
    
    // Let's apply dataset transformations
    ds->standardize();
    ds->standardizeOutputs();
    //ds->expand(2); // always calls standardize() if you didn't before
    
    // Lets do a forward selection
    for(size_t s = 0; s < nbSolvers; s++) {
      ForwardSelection fs(*ds, s);
      vector<size_t> res = fs.run(threshold);
      
      MSZDataSet solverDS = *ds;
      solverDS.removeFeatures(res);
      
      RidgeRegression rr(solverDS);
      rr.run(delta, s, string(headerPrefix)+snames[s]);
    }

    // Let's not forget to delete the dataset
    delete ds;
 
  } catch(std::bad_alloc) {
    cerr << "Coach: Bad memory allocation or not enough memory on the system\n."
	 << "Exiting peacefully...\n";
    exit(EXIT_FAILURE);
  }

  return 0;
}
