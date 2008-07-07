#ifndef FEATURELABEL_HH
#define FEATURELABEL_HH

#include <map>
#include <string>
#include <ostream>

#include "pputils.hh"

using std::string;
using std::map;

class FeatureLabel {
public:
  FeatureLabel();
  FeatureLabel(const string&);
  
  bool operator==(const FeatureLabel&) const;
  bool operator<(const FeatureLabel&) const;

  uint getMultiplicity(const string&) const;
  void insert(const string&, uint = 1);
  void append(const FeatureLabel&);
  uint size() const { return labels.size(); }
  bool empty() const { return size() == 0; }
  string toContString() const;
  string toString() const;

  // Const iterator for feature label
  typedef map<string, uint>::const_iterator const_iterator;
  const_iterator begin() const { return labels.begin(); }
  const_iterator end()   const { return labels.end();   }

private:
  map<string, uint> labels; // Map Label -> Multiplicity
  
  friend 
  std::ostream& operator<<(std::ostream& s, const FeatureLabel& v) {
    if(v.labels.size() == 0) {
      s << "(EMPTYLABEL)";
    }
    else {
      if(v.labels.size() != 1) 
	s << "(* ";
      for(map<string, uint>::const_iterator it = v.labels.begin();
	  it != v.labels.end();
	  it++) 
	s << it->first << "^" << it->second << " ";
      if(v.labels.size() != 1)
	s << ")";
    }
    
    return s;
  }
  
};

#endif // FEATURELABEL_HH
