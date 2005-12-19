#include "delayed.hh"


DelayedVoid::DelayedVoid(xmmsc_result_t* res, string err)
  : AbstractDelayed<void>(res, err) {
  pmaker = new VoidProduct();
}

DelayedVoid::DelayedVoid(xmmsc_result_t* res, const char* err)
  : AbstractDelayed<void>(res, err) {
  pmaker = new VoidProduct();
}

void
DelayedVoid::createProduct() {
  pmaker->create();
}

void
DelayedVoid::runCallbacks() {
  while(receivers.size() > 0) {
    receivers.front()->run();
    delete receivers.front();
    receivers.pop_front();
  }
}

