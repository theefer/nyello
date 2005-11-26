#ifndef __DELAYED_HH__
#define __DELAYED_HH__

#include <time.h>
#include <list>
#include <iostream>

using namespace std;

#include <xmmsclient/xmmsclient.h>

#include "asynchronizer.hh"


class DelayedCallback { };



template <class T>
class ProductMaker {
public:
  virtual T create(xmmsc_result_t* res) = 0;
};


template <class T, int (*extractor)(xmmsc_result_t*, T*)>
class PrimitiveProduct : public ProductMaker<T> {
public:
  virtual T create(xmmsc_result_t* res);
};

template <class T, int (*extractor)(xmmsc_result_t*, T*)>
T
PrimitiveProduct<T, extractor>::create(xmmsc_result_t* res) {
  T product;
  (*extractor)(res, &product);
  return product;
}


template <class T, int (*extractor)(xmmsc_result_t*, T*)>
class ComparatorProduct : public ProductMaker<bool> {
public:
  ComparatorProduct(T value);

  bool ComparatorProduct::create(xmmsc_result_t* res);

private:
  int value;
};


template <class T, int (*extractor)(xmmsc_result_t*, T*)>
ComparatorProduct<T, extractor>::ComparatorProduct(T _value) : value(_value) {
}

template <class T, int (*extractor)(xmmsc_result_t*, T*)>
bool
ComparatorProduct<T, extractor>::create(xmmsc_result_t* res) {
  T product;
  (*extractor)(res, &product);
  return (product == value);
}




class StringMatcherProduct : public ProductMaker<bool> {
public:
  StringMatcherProduct(char* str);

  bool StringMatcherProduct::create(xmmsc_result_t* res);

private:
  char* str;
};


template <class T>
class ObjectProduct : public ProductMaker<T> {
public:
  virtual T create(xmmsc_result_t* res);
};

template <class T>
T
ObjectProduct<T>::create(xmmsc_result_t* res) {
  T product(res);
  return product;
}


template <class T, class X>
class ComplexObjectProduct : public ProductMaker<T> {
public:
  ComplexObjectProduct(X extraParam);

  virtual T create(xmmsc_result_t* res);

private:
  X extraParam;
};

template <class T, class X>
ComplexObjectProduct<T, X>::ComplexObjectProduct(X extra) : extraParam(extra) {
}

template <class T, class X>
T
ComplexObjectProduct<T, X>::create(xmmsc_result_t* res) {
  T product(res, extraParam);
  return product;
}



class DelayedVoid {
public:
  static inline void setAsynchronizer(Asynchronizer* _async) { async = _async; }

  DelayedVoid(xmmsc_result_t* res, const char* errmsg = NULL);
  ~DelayedVoid();

  void callback(xmmsc_result_t* res);
  virtual void runHooks(xmmsc_result_t* res);
  virtual void runCallbacks(xmmsc_result_t* res);

  DelayedVoid* wait();

protected:
  static Asynchronizer* async;

  const char* errmsg;

  bool ready;

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

  virtual void runHooks(xmmsc_result_t* res);
  virtual void runCallbacks(xmmsc_result_t* res);

  inline void addCallback(DelayedCallbackFnPtr fn) { receivers.push_back(fn); }

  T getProduct();

protected:
  T product;

  ProductMaker<T>* pmaker;
  list<DelayedCallbackFnPtr> receivers;
};



/*  == And here comes the implementation, in the header file due to
       boring GCC limitations...  == */


// Generic hack-function to use methods as callback functions
template <void (DelayedVoid::*func) (xmmsc_result_t*)>
void runDelayedMethod(xmmsc_result_t *res, void *del_ptr);


template <class T>
Delayed<T>::Delayed(xmmsc_result_t* res, const char* err) : DelayedVoid(res, err) {
  ObjectProduct<T> oprod;
  pmaker = &oprod;
}

template <class T>
Delayed<T>::Delayed(xmmsc_result_t* res, ProductMaker<T>* _pmaker, const char* err)
  : DelayedVoid(res, err), pmaker(_pmaker) {
}

template <class T>
Delayed<T>::~Delayed() {
  // FIXME: Delete callbacks?
  // FIXME: Delete product maker?
}


template <class T>
void
Delayed<T>::runHooks(xmmsc_result_t* res) {
  DelayedVoid::runHooks(res);
  product = pmaker->create(res);
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
