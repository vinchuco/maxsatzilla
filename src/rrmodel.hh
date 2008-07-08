#ifndef RRMODEL_HH
#define RRMODEL_HH

#include <string>
#include <utility>
#include <map>
#include <set>

#include "pputils.hh"
#include "featurelabel.hh"
#include "learningalgtype.hh"
#include "model.hh"

using std::map;
using std::set;
using std::string;
using std::pair;
using std::make_pair;

/** The model class is a container
 *  for the regression output.
 *  It contains values for the regression coefficients
 *  which represents an hiper-plane in a high-dimensional space.
 *  The model is represented by:
 *  y(x1,...,xn) = b_0 + Sum_i=1^n b_i x_i
 *  where b_i are the coefficients of the model, or regressors (the important stuff) 
 *  and x_i are the variables of the model, the variables with which the model was trained.
 */
class RRModel : public Model {
public:
  RRModel();
  ~RRModel();

  // Const Iterator for the regressors
  typedef map<FeatureLabel, double>::const_iterator const_iterator;
  typedef map<FeatureLabel, double>::iterator iterator;
  const_iterator begin() const { return regressorMap.begin(); }
  iterator begin()             { return regressorMap.begin(); }
  const_iterator end()   const { return regressorMap.end();   }
  iterator end()               { return regressorMap.end();   }

  void addRegressor(double, const FeatureLabel&);
  void addRegressor(double);
  void remRegressor(const FeatureLabel&);
  double getRegressor(const FeatureLabel&) const;

  double getRegressor() const { return freeRegressor;           }  
  void   delRegressor()       { freeRegressor = 0.0;            }
  uint   nbRegressors() const { return regressorMap.size() + 1; }
  
  double computeModelOutput(const map<string, double>&) const;
  set<string> computeRawLabels() const;
  
protected:
  double freeRegressor;
  map<FeatureLabel, double> regressorMap; ///< Map where regressor values are kept by id, which is a string.
};

#endif // RRMODEL_HH
