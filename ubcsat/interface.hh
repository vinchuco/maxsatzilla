#ifndef INTERFACE_HH
#define INTERFACE_HH

#include <iostream>

using std::ostream;

class Interface {
public:
  static Interface *Instance();

  void setStream(ostream&);

private:
  ostream *oss;

  template<class T> friend 
  Interface& operator<<(Interface &i, const T& t) {
    *(i.oss) << t;
    return i;
  }
};

#endif // INTERFACE_HH
