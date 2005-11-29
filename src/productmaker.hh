#ifndef __PRODUCTMAKER_HH__
#define __PRODUCTMAKER_HH__

#include <cstring>
#include <iostream>
#include <xmmsclient/xmmsclient.h>

using namespace std;


template <class T>
class ProductMaker {
public:
  ~ProductMaker();

  inline void setResult(xmmsc_result_t* result) { res = result; }

  virtual void checkErrors(const char* errmsg = NULL);
  virtual T create() = 0;

protected:
  xmmsc_result_t* res;

  // FIXME: Why can't we use a virtual function to determine that?
  bool unrefResult;
};


/**
 * Dumb concrete instance of ProductMaker that can be instantiated for
 * void products.
 */
class VoidProduct : public ProductMaker<void> {
public:
  inline virtual void create() {}
};


template <class T, int (*extractor)(xmmsc_result_t*, T*)>
class PrimitiveProduct : public ProductMaker<T> {
public:
  virtual T create();
};



template <class T>
class ObjectProduct : public ProductMaker<T*> {
public:
  virtual T* create();
};



template <class T, class X>
class ComplexObjectProduct : public ProductMaker<T> {
public:
  ComplexObjectProduct(X extraParam);

  virtual T create();

private:
  X extraParam;
};



template <class T, int (*extractor)(xmmsc_result_t*, T*)>
class ComparatorProduct : public ProductMaker<bool> {
public:
  ComparatorProduct(T value);

  virtual bool create();

private:
  T value;
};



class StringMatcherProduct : public ProductMaker<bool> {
public:
  StringMatcherProduct(char* str);

  virtual bool create();

private:
  char* str;
};



/*  == And here comes the implementation, in the header file due to
       boring GCC limitations...  == */


template <class T>
ProductMaker<T>::~ProductMaker() {
  if(unrefResult) {
    xmmsc_result_unref(res);
  }
}

template <class T>
void
ProductMaker<T>::checkErrors(const char* errmsg) {
  if(xmmsc_result_iserror(res)) {
    // FIXME: Show error?
    if(errmsg != NULL) {
      cerr << errmsg << xmmsc_result_get_error(res) << endl;
    }
  }
}



template <class T, int (*extractor)(xmmsc_result_t*, T*)>
T
PrimitiveProduct<T, extractor>::create() {
  this->unrefResult = true;

  T product;
  (*extractor)(this->res, &product);
  return product;
}


template <class T>
T*
ObjectProduct<T>::create() {
  this->unrefResult = false;

  T* product = new T(this->res);
  return product;
}



template <class T, class X>
ComplexObjectProduct<T, X>::ComplexObjectProduct(X extra) : extraParam(extra) {
}

template <class T, class X>
T
ComplexObjectProduct<T, X>::create() {
  this->unrefResult = false;

  T product(this->res, extraParam);
  return product;
}



template <class T, int (*extractor)(xmmsc_result_t*, T*)>
ComparatorProduct<T, extractor>::ComparatorProduct(T _value) : value(_value) {
}

template <class T, int (*extractor)(xmmsc_result_t*, T*)>
bool
ComparatorProduct<T, extractor>::create() {
  this->unrefResult = true;

  T product;
  (*extractor)(res, &product);
  return (product == value);
}


#endif  // __PRODUCTMAKER_HH__
