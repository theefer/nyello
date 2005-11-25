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
  // FIXME: Delete receivers?
}

void
DelayedVoid::callback(xmmsc_result_t* res) {
  if(xmmsc_result_iserror(res)) {
    // FIXME: Show error?
    if(errmsg != NULL) {
      cerr << errmsg << xmmsc_result_get_error(res) << endl;
    }
  }
  else {
//    T product(res);

//       // FIXME: We need an *object* to call the method from...
//       typename list<ReceiveFnPtr>::iterator receive;
//       for(receive = receivers.begin(); receive != receivers.end(); ++receive) {
//         (*receive)(product);
//       }
  }

  xmmsc_result_unref(res);
  
  // Unblock waiting
  unblock();
}


void
DelayedVoid::wait(Asynchronizer* async) {
  // FIXME: Should be able to inherit async from DelayedVoid

  // Pass IPC traffic until unblocked
  while(!ready) {
    async->waitForData(Asynchronizer::WAIT_XMMSIPC);
  }
}


void
DelayedVoid::unblock() {
  ready = true;
}
