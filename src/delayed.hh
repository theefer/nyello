#ifndef __DELAYED_HH__
#define __DELAYED_HH__

#include <list>
#include <iostream>

using namespace std;

#include <xmmsclient/xmmsclient.h>

#include "productmaker.hh"
#include "asynchronizer.hh"


// Simple forward definitions so the callback classes compile
class DelayedVoid;
template <typename T> class Delayed;


class TopDelayedCallbackVoid {
public:
  virtual void run() = 0;
};

template <typename C>
class DelayedCallbackVoid : public TopDelayedCallbackVoid {

  typedef void (C::*CallbackVoidFnPtr)();

public:
  DelayedCallbackVoid(C* object, CallbackVoidFnPtr function);

  virtual inline void run() { (object->*function)(); }

private:
  C* object;
  CallbackVoidFnPtr function;
};

template <typename C>
class DelayedChainCallbackVoid : public TopDelayedCallbackVoid {

  typedef DelayedVoid* (C::*ChainCallbackVoidFnPtr)();

public:
  DelayedChainCallbackVoid(C* object, ChainCallbackVoidFnPtr function);

  virtual void run();

private:
  C* object;
  ChainCallbackVoidFnPtr function;
};


template <typename T>
class TopDelayedCallback {
public:
  virtual void run(T product) = 0;
};

template <typename C, typename T>
class DelayedCallback : public TopDelayedCallback<T> {

  typedef void (C::*CallbackFnPtr)(T);

public:
  DelayedCallback(C* object, CallbackFnPtr function);

  virtual inline void run(T product) { (object->*function)(product); }

private:
  C* object;
  CallbackFnPtr function;
};

template <typename C, typename T>
class DelayedChainCallback : public TopDelayedCallback<T> {

  typedef void (C::*ChainCallbackFnPtr)(T);

public:
  DelayedChainCallback(C* object, ChainCallbackFnPtr function);

  virtual void run(T product);

private:
  C* object;
  ChainCallbackFnPtr function;
};



template <typename T>
class AbstractDelayed : public StaticAsynchronizer {
public:
  AbstractDelayed(xmmsc_result_t* res, const char* errmsg = NULL);
  ~AbstractDelayed();

  void callback(xmmsc_result_t* res);

  AbstractDelayed<T>* wait();

protected:
  ProductMaker<T>* pmaker;
  const char* errmsg;

  bool ready;

  virtual void createProduct() = 0;
  virtual void runCallbacks() = 0;

  void unblock();
};


class DelayedVoid : public AbstractDelayed<void> {
public:
  DelayedVoid(xmmsc_result_t* res, const char* errmsg = NULL);

  template<typename C>
  void addCallback(C* object, void (C::*function)());

  template<typename C>
  void addCallback(C* object, DelayedVoid* (C::*function)());

protected:
  list<TopDelayedCallbackVoid*> receivers;

  virtual void createProduct();
  virtual void runCallbacks();
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
template <typename T>
class Delayed : public AbstractDelayed<T> {

public:
  Delayed(xmmsc_result_t* res, const char* errmsg = NULL);
  Delayed(xmmsc_result_t* res, ProductMaker<T>* pmaker, const char* errmsg = NULL);

  template<typename C>
  void addCallback(C* object, void (C::*function)(T));

  template<typename C>
  void addCallback(C* object, DelayedVoid* (C::*function)(T));

  T getProduct();

protected:
  T product;

  list<TopDelayedCallback<T>* > receivers;

  virtual void createProduct();
  virtual void runCallbacks();
};




/*  == And here comes the implementation, in the header file due to
       boring GCC limitations...  == */

template <typename C>
DelayedCallbackVoid<C>::DelayedCallbackVoid(C* _object, CallbackVoidFnPtr _function) 
  : object(_object), function(_function) {
}

template <typename C>
DelayedChainCallbackVoid<C>::DelayedChainCallbackVoid(C* _object, ChainCallbackVoidFnPtr _function) 
  : object(_object), function(_function) {
}

template <typename C>
void
DelayedChainCallbackVoid<C>::run() {
  DelayedVoid* del = (object->*function)();

  // FIXME: Could be bad for fast async; setup callback? doh, complex...
  del->wait();
  delete del;
}

template <typename C, typename T>
DelayedCallback<C, T>::DelayedCallback(C* _object, CallbackFnPtr _function)
  : object(_object), function(_function) {
}

template <typename C, typename T>
DelayedChainCallback<C, T>::DelayedChainCallback(C* _object, ChainCallbackFnPtr _function)
  : object(_object), function(_function) {
}

template <typename C, typename T>
void
DelayedChainCallback<C, T>::run(T product) {
  DelayedVoid* del = (object->*function)(product);

  // FIXME: Could be bad for fast async; setup callback? doh, complex...
  del->wait();
  delete del;
}


// Generic hack-function to use methods as callback functions
template <typename T, void (AbstractDelayed<T>::*func) (xmmsc_result_t*)>
void runDelayedMethod(xmmsc_result_t *res, void *del_ptr) {
  AbstractDelayed<T>* d = (AbstractDelayed<T>*)del_ptr;
  (d->*func)(res);
}



template <typename T>
AbstractDelayed<T>::AbstractDelayed(xmmsc_result_t* res, const char* err) : errmsg(err) {
  ready = false;
  pmaker = NULL;

  xmmsc_result_notifier_set(res, &runDelayedMethod<T, &AbstractDelayed<T>::callback>, this);
  xmmsc_result_unref(res);
}

template <typename T>
AbstractDelayed<T>::~AbstractDelayed() {
  if(pmaker != NULL) {
    delete pmaker;
  }

  // FIXME: Delete callbacks?
}

template <typename T>
void
AbstractDelayed<T>::callback(xmmsc_result_t* res) {
  try {
    pmaker->setResult(res);
    pmaker->checkErrors(errmsg);
    createProduct();
    runCallbacks();
  }
  catch(...) {
    // FIXME: What do we do? throw further?
  }

  unblock();
}

template <typename T>
AbstractDelayed<T>*
AbstractDelayed<T>::wait() {
  // Pass IPC traffic until unblocked
  while(!ready) {
    async->waitForData(Asynchronizer::WAIT_XMMSIPC);
  }

  return this;
}

template <typename T>
void
AbstractDelayed<T>::unblock() {
  ready = true;
}



template <typename T>
Delayed<T>::Delayed(xmmsc_result_t* res, const char* err) : AbstractDelayed<T>(res, err) {
  // FIXME: actually working?
  this->pmaker = new ObjectProduct<T>();
}

template <typename T>
Delayed<T>::Delayed(xmmsc_result_t* res, ProductMaker<T>* _pmaker, const char* err)
  : AbstractDelayed<T>(res, err) {
  this->pmaker = _pmaker;
}


template <typename T>
T
Delayed<T>::getProduct() {
  this->wait();
  return product;
}

template <typename T>
void
Delayed<T>::createProduct() {
  product = this->pmaker->create();
}


template <typename T>
template <typename C>
void
Delayed<T>::addCallback(C* object, void (C::*function)(T)) {
  receivers.push_back(new DelayedCallback<C, T>(object, function));
}

template <typename T>
template <typename C>
void
Delayed<T>::addCallback(C* object, DelayedVoid* (C::*function)(T)) {
  receivers.push_back(new DelayedChainCallback<C, T>(object, function));
}

template <typename T>
void
Delayed<T>::runCallbacks() {
  while(receivers.size() > 0) {
    receivers.front()->run(product);
    delete receivers.front();
    receivers.pop_front();
  }
}



template<typename C>
void
DelayedVoid::addCallback(C* object, void (C::*function)()) {
  receivers.push_back(new DelayedCallbackVoid<C>(object, function));
}

template<typename C>
void
DelayedVoid::addCallback(C* object, DelayedVoid* (C::*function)()) {
  receivers.push_back(new DelayedChainCallbackVoid<C>(object, function));
}


#endif  // __DELAYED_HH__
