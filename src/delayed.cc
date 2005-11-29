#include "delayed.hh"


// Generic hack-function to use methods as callback functions
template <void (DelayedVoid::*func) (xmmsc_result_t*)>
void runDelayedMethod(xmmsc_result_t *res, void *del_ptr) {
  DelayedVoid* d = (DelayedVoid*)del_ptr;
  (d->*func)(res);
}


DelayedVoid::DelayedVoid(xmmsc_result_t* res, const char* err) : errmsg(err) {
  ready = false;

  xmmsc_result_notifier_set(res, &runDelayedMethod<&DelayedVoid::callback>, this);
  xmmsc_result_unref(res);
}

DelayedVoid::~DelayedVoid() {
  // FIXME: Delete callbacks?
}

void
DelayedVoid::callback(xmmsc_result_t* res) {
  try {
    runHooks(res);
    runCallbacks(res);
  }
  catch(...) {
    // FIXME: What do we do? throw further?
  }

  unblock();
}

void
DelayedVoid::runHooks(xmmsc_result_t* res) {
  if(xmmsc_result_iserror(res)) {
    // FIXME: Show error?
    if(errmsg != NULL) {
      cerr << errmsg << xmmsc_result_get_error(res) << endl;
    }

    // FIXME: Throw error?
  }
}

void
DelayedVoid::runCallbacks(xmmsc_result_t* res) {
  // FIXME: Do NOT count on overloading of this to assume this is not done in Delayed<T> !
  xmmsc_result_unref(res);
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
