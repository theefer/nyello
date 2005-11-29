#ifndef __DELAYED_HH__
#define __DELAYED_HH__

#include <list>
#include <iostream>

using namespace std;

#include <xmmsclient/xmmsclient.h>

#include "productmaker.hh"
#include "asynchronizer.hh"


class DelayedCallback { };


// FIXME: How do we declare a static var for a class that uses templates?
class FooDelayed {
public:
  static inline void setAsynchronizer(Asynchronizer* _async) { async = _async; }

protected:
  static Asynchronizer* async;
};


template <class T>
class AbstractDelayed : public FooDelayed {
public:
  //  static inline void setAsynchronizer(Asynchronizer* _async) { async = _async; }

  AbstractDelayed(xmmsc_result_t* res, const char* errmsg = NULL);
  ~AbstractDelayed();

  void callback(xmmsc_result_t* res);

  AbstractDelayed<T>* wait();

protected:
  //  static Asynchronizer* async;

  ProductMaker<T>* pmaker;
  const char* errmsg;

  bool ready;

  virtual void createProduct() = 0;
  virtual void runCallbacks(xmmsc_result_t* res) = 0;

  void unblock();
};



class DelayedVoid : public AbstractDelayed<void> {
public:
  DelayedVoid(xmmsc_result_t* res, const char* errmsg = NULL);

protected:
  virtual void createProduct();
  virtual void runCallbacks(xmmsc_result_t* res);
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
class Delayed : public AbstractDelayed<T> {

  typedef void (DelayedCallback::*DelayedCallbackFnPtr)(T);

public:
  Delayed(xmmsc_result_t* res, const char* errmsg = NULL);
  Delayed(xmmsc_result_t* res, ProductMaker<T>* pmaker, const char* errmsg = NULL);

  inline void addCallback(DelayedCallbackFnPtr fn) { receivers.push_back(fn); }

  T getProduct();

protected:
  T product;

  list<DelayedCallbackFnPtr> receivers;

  virtual void createProduct();
  virtual void runCallbacks(xmmsc_result_t* res);
};




/*  == And here comes the implementation, in the header file due to
       boring GCC limitations...  == */


// Generic hack-function to use methods as callback functions
template <class T, void (AbstractDelayed<T>::*func) (xmmsc_result_t*)>
void runDelayedMethod(xmmsc_result_t *res, void *del_ptr) {
  AbstractDelayed<T>* d = (AbstractDelayed<T>*)del_ptr;
  (d->*func)(res);
}



template <class T>
AbstractDelayed<T>::AbstractDelayed(xmmsc_result_t* res, const char* err) : errmsg(err) {
  ready = false;
  pmaker = NULL;

  xmmsc_result_notifier_set(res, &runDelayedMethod<T, &AbstractDelayed<T>::callback>, this);
  xmmsc_result_unref(res);
}

template <class T>
AbstractDelayed<T>::~AbstractDelayed() {
  if(pmaker != NULL) {
    delete pmaker;
  }

  // FIXME: Delete callbacks?
}

template <class T>
void
AbstractDelayed<T>::callback(xmmsc_result_t* res) {
  try {
    pmaker->setResult(res);
    pmaker->checkErrors(errmsg);
    createProduct();
    runCallbacks(res);

    // FIXME: TEMPORARY because delayed objects are never destroyed - yet!
    if(pmaker != NULL) {
      delete pmaker;
    }

  }
  catch(...) {
    // FIXME: What do we do? throw further?
  }

  unblock();
}

template <class T>
AbstractDelayed<T>*
AbstractDelayed<T>::wait() {
  // Pass IPC traffic until unblocked
  while(!ready) {
    async->waitForData(Asynchronizer::WAIT_XMMSIPC);
  }

  return this;
}

template <class T>
void
AbstractDelayed<T>::unblock() {
  ready = true;
}



template <class T>
Delayed<T>::Delayed(xmmsc_result_t* res, const char* err) : AbstractDelayed<T>(res, err) {
  // FIXME: actually working?
  this->pmaker = new ObjectProduct<T>();
}

template <class T>
Delayed<T>::Delayed(xmmsc_result_t* res, ProductMaker<T>* _pmaker, const char* err)
  : AbstractDelayed<T>(res, err) {
  this->pmaker = _pmaker;
}


template <class T>
T
Delayed<T>::getProduct() {
  this->wait();
  return product;
}

template <class T>
void
Delayed<T>::createProduct() {
  product = this->pmaker->create();
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

#endif  // __DELAYED_HH__
