#ifndef __DELAYED_HH__
#define __DELAYED_HH__

#include <list>
#include <iostream>

using namespace std;

#include <xmmsclient/xmmsclient.h>

#include "productmaker.hh"
#include "asynchronizer.hh"


class DelayedCallback { };


class DelayedVoid {

public:
  static inline void setAsynchronizer(Asynchronizer* _async) { async = _async; }

  DelayedVoid(xmmsc_result_t* res, const char* errmsg = NULL);
  ~DelayedVoid();

  void callback(xmmsc_result_t* res);

  DelayedVoid* wait();

protected:
  static Asynchronizer* async;

  ProductMaker<void>* pmaker;
  const char* errmsg;

  bool ready;

  virtual void createProduct();
  virtual void runCallbacks(xmmsc_result_t* res);

  void unblock();
};


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
  Delayed(xmmsc_result_t* res, ProductMaker<T>* pmaker, const char* errmsg = NULL);
  ~Delayed();

  inline void addCallback(DelayedCallbackFnPtr fn) { receivers.push_back(fn); }

  T getProduct();

protected:
  T product;

  ProductMaker<T>* pmaker;
  list<DelayedCallbackFnPtr> receivers;

  virtual void createProduct();
  virtual void runCallbacks(xmmsc_result_t* res);
};



/*  == And here comes the implementation, in the header file due to
       boring GCC limitations...  == */


// Generic hack-function to use methods as callback functions
template <void (DelayedVoid::*func) (xmmsc_result_t*)>
void runDelayedMethod(xmmsc_result_t *res, void *del_ptr);


template <class T>
Delayed<T>::Delayed(xmmsc_result_t* res, const char* err) : DelayedVoid(res, err) {
  // FIXME: actually working?
  ObjectProduct<T> oprod;
  pmaker = &oprod;
}

template <class T>
Delayed<T>::Delayed(xmmsc_result_t* res, ProductMaker<T>* _pmaker, const char* err)
  : DelayedVoid(res, err), pmaker(_pmaker) {
}

template <class T>
Delayed<T>::~Delayed() {
  delete pmaker;
}


template <class T>
void
Delayed<T>::createProduct() {
  product = pmaker->create();
}

template <class T>
void
Delayed<T>::runCallbacks(xmmsc_result_t* res) {
  // FIXME: We need an *object* to call the method from...
//   typename list<DelayedCallbackFnPtr>::iterator receive;
//   for(receive = receivers.begin(); receive != receivers.end(); ++receive) {
//     (*receive)(product);
//   }
}

template <class T>
T
Delayed<T>::getProduct() {
  wait();
  return product;
}

#endif  // __DELAYED_HH__
