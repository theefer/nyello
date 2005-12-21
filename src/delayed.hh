#ifndef __DELAYED_HH__
#define __DELAYED_HH__

#include <list>
#include <iostream>

using namespace std;

#include <xmmsclient/xmmsclient.h>

#include "productmaker.hh"
#include "asynchronizer.hh"


// Simple forward definitions so the callback classes compile
template <typename T> class Delayed;


/**
 * Different types of wrappers for method callbacks.
 */
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


template <>
class TopDelayedCallback<void> {
public:
  virtual void run() = 0;
};

template <typename C>
class DelayedCallback<C,void> : public TopDelayedCallback<void> {

  typedef void (C::*CallbackVoidFnPtr)();

public:
  DelayedCallback(C* object, CallbackVoidFnPtr function);

  virtual inline void run() { (object->*function)(); }

private:
  C* object;
  CallbackVoidFnPtr function;
};

template <typename C>
class DelayedChainCallback<C,void> : public TopDelayedCallback<void> {

  typedef Delayed<void>* (C::*ChainCallbackVoidFnPtr)();

public:
  DelayedChainCallback(C* object, ChainCallbackVoidFnPtr function);

  virtual void run();

private:
  C* object;
  ChainCallbackVoidFnPtr function;
};



/**
 * Generic parent class for all the Delayed specializations, handling
 * all what a Delayed class should do.
 */
template <typename T>
class AbstractDelayed : public StaticAsynchronizer {
public:
  AbstractDelayed(xmmsc_result_t* res, string errmsg = "");
  AbstractDelayed(xmmsc_result_t* res, const char* errmsg);
  ~AbstractDelayed();

  void callback(xmmsc_result_t* res);

  AbstractDelayed<T>* wait();

protected:
  bool ready;

  string errmsg;
  ProductMaker<T>* pmaker;
  list<TopDelayedCallback<T>* > receivers;

  virtual void createProduct() = 0;
  virtual void runCallbacks() = 0;

  inline void unblock() { ready = true; }
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
  Delayed(xmmsc_result_t* res, string errmsg = "");
  Delayed(xmmsc_result_t* res, const char* errmsg);
  Delayed(xmmsc_result_t* res, ProductMaker<T>* pmaker, string errmsg = "");
  Delayed(xmmsc_result_t* res, ProductMaker<T>* pmaker, const char* errmsg);

  template<typename C>
  void addCallback(C* object, void (C::*function)(T));

  template<typename C>
  void addCallback(C* object, Delayed<void>* (C::*function)(T));

  T getProduct();

protected:
  T product;

  virtual void createProduct();
  virtual void runCallbacks();
};


/**
 * Template specialization for void type, obviously lacking all the
 * product-related stuff.
 */
template <>
class Delayed<void> : public AbstractDelayed<void> {
public:
  Delayed(xmmsc_result_t* res, string errmsg = "");
  Delayed(xmmsc_result_t* res, const char* errmsg);

  template<typename C>
  void addCallback(C* object, void (C::*function)());

  template<typename C>
  void addCallback(C* object, Delayed<void>* (C::*function)());

protected:
  virtual void createProduct();
  virtual void runCallbacks();
};




/*  == And here comes the implementation of template functions. == */

template <typename C>
DelayedCallback<C,void>::DelayedCallback(C* _object, CallbackVoidFnPtr _function) 
  : object(_object), function(_function) {
}

template <typename C>
DelayedChainCallback<C,void>::DelayedChainCallback(C* _object, ChainCallbackVoidFnPtr _function) 
  : object(_object), function(_function) {
}

template <typename C>
void
DelayedChainCallback<C,void>::run() {
  Delayed<void>* del = (object->*function)();

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
  Delayed<void>* del = (object->*function)(product);

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
AbstractDelayed<T>::AbstractDelayed(xmmsc_result_t* res, string err) : errmsg(err) {
  ready = false;
  pmaker = NULL;

  xmmsc_result_notifier_set(res, &runDelayedMethod<T, &AbstractDelayed<T>::callback>, this);
  xmmsc_result_unref(res);
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

  // Delete callback objects
  typename list<TopDelayedCallback<T>* >::iterator cbit;
  for(cbit = receivers.begin(); cbit != receivers.end(); ++cbit) {
    delete (*cbit);
  }
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
Delayed<T>::Delayed(xmmsc_result_t* res, string err)
  : AbstractDelayed<T>(res, err) {
  this->pmaker = new ObjectProduct<T>();
}

template <typename T>
Delayed<T>::Delayed(xmmsc_result_t* res, const char* err)
  : AbstractDelayed<T>(res, err) {
  this->pmaker = new ObjectProduct<T>();
}

template <typename T>
Delayed<T>::Delayed(xmmsc_result_t* res, ProductMaker<T>* _pmaker, string err)
  : AbstractDelayed<T>(res, err) {
  this->pmaker = _pmaker;
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
  this->receivers.push_back(new DelayedCallback<C, T>(object, function));
}

template <typename T>
template <typename C>
void
Delayed<T>::addCallback(C* object, Delayed<void>* (C::*function)(T)) {
  this->receivers.push_back(new DelayedChainCallback<C, T>(object, function));
}

template <typename T>
void
Delayed<T>::runCallbacks() {
  while(this->receivers.size() > 0) {
    this->receivers.front()->run(product);
    delete this->receivers.front();
    this->receivers.pop_front();
  }
}


template<typename C>
void
Delayed<void>::addCallback(C* object, void (C::*function)()) {
  receivers.push_back(new DelayedCallback<C,void>(object, function));
}

template<typename C>
void
Delayed<void>::addCallback(C* object, Delayed<void>* (C::*function)()) {
  receivers.push_back(new DelayedChainCallback<C,void>(object, function));
}

#endif  // __DELAYED_HH__
