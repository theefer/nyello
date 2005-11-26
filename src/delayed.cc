#include "delayed.hh"


StringMatcherProduct::StringMatcherProduct(char* _str) : str(_str) {
}

bool
StringMatcherProduct::create(xmmsc_result_t* res) {
  char* entry_name;
  bool found = false;

  while(xmmsc_result_list_valid(res) && !found) {
    xmmsc_result_get_string(res, &entry_name);
    if(strcmp(str, entry_name) == 0) {
      found = true;
    }
    xmmsc_result_list_next(res);
  }

  return found;
}




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
  try {
    runHooks(res);
    runCallbacks(res);
  }
  catch(...) {
    // FIXME: What do we do? throw further?
  }

  xmmsc_result_unref(res);
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
}


DelayedVoid*
DelayedVoid::wait() {
  // FIXME: Should be able to inherit async from DelayedVoid

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
