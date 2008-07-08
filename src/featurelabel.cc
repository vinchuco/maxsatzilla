#include "featurelabel.hh"

#include <sstream>

using std::stringstream;

FeatureLabel::FeatureLabel() {}

FeatureLabel::FeatureLabel(const string& l) {
  labels[l] = 1;
}

bool FeatureLabel::operator==(const FeatureLabel& fl) const {
  return labels == fl.labels;
}

bool FeatureLabel::operator<(const FeatureLabel& fl) const {
  return labels < fl.labels;
}

uint FeatureLabel::getMultiplicity(const string& l) const {
  
  map<string, uint>::const_iterator f = labels.find(l);
  if(f == labels.end())
    return 0;

  return f->second;    
}

void FeatureLabel::insert(const string& l, uint m) {
  labels[l] += m;
}

void FeatureLabel::append(const FeatureLabel& fl) {

  const FeatureLabel::const_iterator end = fl.end();
  for(FeatureLabel::const_iterator it = fl.begin();
      it != end;
      ++it) 
    insert(it->first, it->second);

}

/** 
 * Returns a string representation of the current feature label.
 * @returns a string with the label.
 */
string FeatureLabel::toString() const {
  stringstream oss;
  oss << *this;
  return oss.str();
}

/**
 * Returns a continuous label (without spaces) for the current feature label.
 * @returns a string with the label.
 */
string FeatureLabel::toContString() const {
  string s = toString();
  for(uint i = 0; i < s.size(); ++i)
    if(s[i] == ' ') s[i] = '_';

  return s;
}
