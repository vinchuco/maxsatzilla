#ifndef MSZMODEL_HH
#define MSZMODEL_HH

#include <vector>
#include <string>
#include <utility>
#include <map>

using std::map;
using std::vector;
using std::string;
using std::pair;

/** The model class is a container
 *  for the regression output.
 *  It contains values for the regression coefficients
 *  which represents an hiper-plane in a high-dimensional space.
 *  The model is represented by:
 *  y(x1,...,xn) = b_0 + Sum_i=1^n b_i x_i
 *  where b_i are the coefficients of the model, or regressors (the important stuff) 
 *  and x_i are the variables of the model, the variables with which the model was trained.
 */
class MSZModel {
public:
  MSZModel();

  void addRegressor(double, string);
  void addRegressor(double);
  void remRegressor(const string&);
  double getRegressor(const string&) const;

  double getRegressor() const { return freeRegressor;           }  
  void   delRegressor()       { freeRegressor = 0.0;            }
  uint   nbRegressors() const { return regressorMap.size() + 1; }

  double computeModelOutput(const vector<pair<string, double> >&);
  
protected:
  double freeRegressor;
  map<string, double> regressorMap; ///< Map where regressor values are kept by id, which is a string.
};

#endif // MSZMODEL_HH
