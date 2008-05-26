#include <iostream>
#include <string>
#include <set>
#include <iterator>
#include <fstream>

#include <gsl/gsl_rng.h>

#include "pputils.hh"
#include "modelreader.hh"

using namespace std;

#define COEF_SCALE 20.0

int main(int argc, char *argv[]) {

  if(argc != 5) {
    cerr << "Expecting exactly 4 arguments. Given " << argc - 1 << "\n";
    cerr << "Usage: coachtester <model> <output> <instances> <error>";
    exit(EXIT_FAILURE);
  }

  const string modelFile = string(argv[1]);
  const string output = string(argv[2]);
  const uint nbInstances = atoi(argv[3]);
  const double error = atof(argv[4]);
  
  gsl_rng_env_setup();
  gsl_rng * r = gsl_rng_alloc (gsl_rng_default);
  ModelReader mreader(modelFile);

  // Need to gather all the features involved in the computation of the model.
  // So, a set of strings should be created such that the set of features
  // is created by looping the solver models and for each solver model, by looping 
  // through the feature labels.
  set<string> labels;
  
  const vector<string>& outlabels = mreader.getOutputLabels();
  const vector<string>::const_iterator outLabelsEnd = outlabels.end();
  for(vector<string>::const_iterator outl = outlabels.begin();
      outl != outLabelsEnd;
      ++outl) {
    // Model for current solver
    const Model& model = mreader.getModel(*outl);
    const set<string> rlabels = model.computeRawLabels();
    labels.insert(rlabels.begin(), rlabels.end());
  }

  cout << "Generating data for labels: ";
  std::copy(labels.begin(), labels.end(), std::ostream_iterator<string>(cout, " "));
  cout << std::endl;

  ofstream file(output.c_str());
  
  file.precision(9);
  file.setf(ios::fixed, ios::floatfield);

  file << "p msz " << outlabels.size() 
       << " " << labels.size() 
       << " " << nbInstances 
       << " " << 100000 << "\n";

  file << "p slv ";
  std::copy(outlabels.begin(), outlabels.end(), std::ostream_iterator<string>(file, " "));
  file << "\n";

  file << "p ftr ";
  std::copy(labels.begin(), labels.end(), std::ostream_iterator<string>(file, " "));
  file << "\n";

  for(uint i = 0; i < nbInstances; ++i) {
    
    cout << "Generating coefficients for instance " << i << "\n";
    
    // We need to compute a number for each feature
    map<string, double> values;

    for(set<string>::const_iterator ls = labels.begin();
	ls != labels.end();
	++ls) {
      const double rv = gsl_rng_uniform (r) * COEF_SCALE;
      values[*ls] = rv;
      cout << "* " << *ls << " := " << rv << "\n";
    }
    
    cout << "Computing outputs for instance " << i << "\n";

    // Compute the output of each model
    map<string, double> outputs;
    for(vector<string>::const_iterator outl = outlabels.begin();
	outl != outlabels.end();
	++outl) {
      const double res = mreader.getModel(*outl).computeModelOutput(values);
      outputs[*outl] = res;
      cout << "** " << *outl << " := " << res << "\n";
    }

    // Now we need to output this information into a file
    file << "i inst" << i + 1 << "\n";
    
    for(vector<string>::const_iterator outl = outlabels.begin();
	outl != outlabels.end();
	++outl) 
      file << outputs[*outl] << " ";
    file << "\n";

    for(set<string>::const_iterator ls = labels.begin();
	ls != labels.end();
	++ls)
      file << values[*ls] << " ";
    file << "\n";
  }

  file.close();
  gsl_rng_free(r);

  return 0;
}
