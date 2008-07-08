#include "interface.hh"

Interface *Interface::Instance() {
  static Interface inst;
  return &inst;
}

void Interface::setStream(ostream& os) {
  oss = &os;
}
