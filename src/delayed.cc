#include "delayed.hh"


Delayed<void>::Delayed(xmmsc_result_t* res, string err)
  : AbstractDelayed<void>(res, err) {
  pmaker = new VoidProduct();
}

Delayed<void>::Delayed(xmmsc_result_t* res, const char* err)
  : AbstractDelayed<void>(res, err) {
  pmaker = new VoidProduct();
}

void
Delayed<void>::createProduct() {
  pmaker->create();
}

void
Delayed<void>::runCallbacks() {
  while(receivers.size() > 0) {
    receivers.front()->run();
    delete receivers.front();
    receivers.pop_front();
  }
}
