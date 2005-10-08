#ifndef __DELAYED_HH__
#define __DELAYED_HH__

#include <time.h>
#include <list>
#include <iostream>

using namespace std;

#include <xmmsclient/xmmsclient.h>

class Handler { };
class Receiver { };


/**
 * Wrapper for delayed objects.
 *
 * This generic class is bound to a result pointer.  All registered
 * handlers functions will be passed the result when it is ready,
 * returning a object of type T.  Each of these objects will be passed
 * to each of the registered receivers functions.
 *
 * Handler functions must be methods of a class inheriting from the
 * Handler class.
 * Receiver functions must be methods of a class inheriting from the
 * Receiver class.
 *
 * This class allows abstraction from the xmmsc_result_t type, while
 * allowing underlining asynchroneous handling to be viewed as sync
 * (using wait and getProducts) or async (using only receiver
 * functions).  Abstraction is performed in a more object-oriented
 * way, focusing on data rather than on callback functions.
 */
template <class T>
class Delayed {

  typedef T (Handler::*HandleFnPtr)(xmmsc_result_t* res);
  typedef void (Receiver::*ReceiveFnPtr)(T);

public:
  Delayed(xmmsc_result_t* res, const char* errmsg = NULL);
  ~Delayed();

  void callback(xmmsc_result_t* res);
  void wait();

  inline void addHandler(HandleFnPtr fn)   { handlers.push_back(fn); }
  inline void addReceiver(ReceiveFnPtr fn) { receivers.push_back(fn); }

  T getLastProduct();
  list<T> getProducts();

protected:
  const char* errmsg;

  bool ready;

  list<HandleFnPtr>  handlers;
  list<ReceiveFnPtr> receivers;

  list<T> products;

  void unblock();
};



/*  == And here comes the implementation, in the header file due to
       boring GCC limitations...  == */


// Generic hack-function to use methods as callback functions
template <class T, void (Delayed<T>::*func) (xmmsc_result_t*)>
void runDelayedMethod(xmmsc_result_t *res, void *del_ptr) {
  Delayed<T>* d = (Delayed<T>*)del_ptr;
  (d->*func)(res);
}

template <class T>
Delayed<T>::Delayed(xmmsc_result_t* res, const char* err) : errmsg(err) {
  ready = false;

  xmmsc_result_notifier_set(res, &runDelayedMethod<T, &Delayed::callback>, this);
  xmmsc_result_unref(res);
}


template <class T>
Delayed<T>::~Delayed() {
  // FIXME: unref?
  // FIXME: Delete handlers and receivers?
}


template <class T>
void
Delayed<T>::callback(xmmsc_result_t* res) {
  if(xmmsc_result_iserror(res)) {
    // FIXME: Show error?
    if(errmsg != NULL) {
      cerr << errmsg << xmmsc_result_get_error(res) << endl;
    }
  }
  else {
    typename list<ReceiveFnPtr>::iterator receive;
    typename list<HandleFnPtr>::iterator  handle;
    for(handle = handlers.begin(); handle != handlers.end(); ++handle) {
// FIXME: We need an *object* to call the method from...
//       T product = (*handle)(res);
//       for(receive = receivers.begin(); receive != receivers.end(); ++receive) {
//         (*receive)(product);
//       }
//       products.push_back(product);
    }
  }

  xmmsc_result_unref(res);
  
  // Unblock waiting
  unblock();
}


template <class T>
T
Delayed<T>::getLastProduct() {
  return products.back();
}


template <class T>
list<T>
Delayed<T>::getProducts() {
  return products;
}


template <class T>
void
Delayed<T>::wait() {
  struct timespec tv;
  tv.tv_sec  = 0;
  tv.tv_nsec = 100000000; // 100ms

  // FIXME: How to *wait* properly?
  while(!ready) {
    nanosleep(&tv, NULL);
    cout << "not ready" << endl;
  }
}


template <class T>
void
Delayed<T>::unblock() {
  // FIXME: How to *unblock* waiters?
  ready = true;
  cout << "unblock" << endl;
}


#endif  // __DELAYED_HH__
