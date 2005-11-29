#include "delayed.hh"


DelayedVoid::DelayedVoid(xmmsc_result_t* res, const char* err) : AbstractDelayed<void>(res, err) {
  pmaker = new VoidProduct();
}

void
DelayedVoid::createProduct() {
  pmaker->create();
}

void
DelayedVoid::runCallbacks(xmmsc_result_t* res) {
}

