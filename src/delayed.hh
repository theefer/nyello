#ifndef __DELAYED_HH__
#define __DELAYED_HH__

#include <time.h>
#include <list>
#include <iostream>

using namespace std;

#include <xmmsclient/xmmsclient.h>

#include "asynchronizer.hh"


class DelayedCallback { };


class DelayedVoid {
public:
  DelayedVoid(xmmsc_result_t* res, const char* errmsg = NULL);
  ~DelayedVoid();

  void callback(xmmsc_result_t* res);
  void wait(Asynchronizer* async);

protected:
  const char* errmsg;

  bool ready;

  void unblock();

  //  static inline void setAsynchronizer(Asynchronizer* _async) { async = _async; }
  //  static Asynchronizer* async;
};
//FIXME: Compilation error for some reason
//Asynchronizer* DelayedVoid::async = NULL;


/**
 * Wrapper for delayed objects.
 *
 * This generic class is bound to a result pointer.  The product
 * generated from the result will be passed to each of the registered
 * receivers functions.
 *
 * DelayedCallback functions must be methods of a class inheriting from the
 * DelayedCallback class.
 *
 * This class allows abstraction from the xmmsc_result_t type, while
 * allowing underlining asynchroneous handling to be viewed as sync
 * (using wait and getProducts) or async (using only receiver
 * functions).  Abstraction is performed in a more object-oriented
 * way, focusing on data rather than on callback functions.
 */
template <class T>
class Delayed : public DelayedVoid {

  typedef void (DelayedCallback::*DelayedCallbackFnPtr)(T);

public:
  Delayed(xmmsc_result_t* res, const char* errmsg = NULL);
  ~Delayed();

  inline void addCallback(DelayedCallbackFnPtr fn) { receivers.push_back(fn); }

  inline T getProduct() { return product; }

protected:
  T product;

  list<DelayedCallbackFnPtr> receivers;
};



/*  == And here comes the implementation, in the header file due to
       boring GCC limitations...  == */


// Generic hack-function to use methods as callback functions
template <void (DelayedVoid::*func) (xmmsc_result_t*)>
void runDelayedMethod(xmmsc_result_t *res, void *del_ptr);


template <class T>
Delayed<T>::Delayed(xmmsc_result_t* res, const char* err) : DelayedVoid(res, err) {
}


template <class T>
Delayed<T>::~Delayed() {
  // FIXME: Delete receivers?
}


#endif  // __DELAYED_HH__
