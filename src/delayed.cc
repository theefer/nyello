#include "delayed.hh"


// Generic hack-function to use methods as callback functions
template <void (DelayedVoid::*func) (xmmsc_result_t*)>
void runDelayedMethod(xmmsc_result_t *res, void *del_ptr) {
  DelayedVoid* d = (DelayedVoid*)del_ptr;
  (d->*func)(res);
}


DelayedVoid::DelayedVoid(xmmsc_result_t* res, const char* err) : errmsg(err) {
  ready = false;
  pmaker = new VoidProduct();

  xmmsc_result_notifier_set(res, &runDelayedMethod<&DelayedVoid::callback>, this);
  xmmsc_result_unref(res);
}

DelayedVoid::~DelayedVoid() {
  delete pmaker;

  // FIXME: Delete callbacks?
}

void
DelayedVoid::callback(xmmsc_result_t* res) {
  try {
    pmaker->setResult(res);
    pmaker->checkErrors(errmsg);
    createProduct();
    runCallbacks(res);
  }
  catch(...) {
    // FIXME: What do we do? throw further?
  }

  unblock();
}

void
DelayedVoid::createProduct() {
  pmaker->create();
}

void
DelayedVoid::runCallbacks(xmmsc_result_t* res) {
}


DelayedVoid*
DelayedVoid::wait() {
  // Pass IPC traffic until unblocked
  while(!ready) {
    async->waitForData(Asynchronizer::WAIT_XMMSIPC);
  }

  return this;
}


void
DelayedVoid::unblock() {
  ready = true;
}
