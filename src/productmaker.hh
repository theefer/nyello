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
  virtual void cleanup();

protected:
  xmmsc_result_t* res;
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
  virtual void cleanup();
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
  cleanup();
  cout << "cleanup done" << endl;
}

template <class T>
void
ProductMaker<T>::cleanup() {
  cout << "unref this" << endl;
  xmmsc_result_unref(res);
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
  cerr << "NO ERROR: " << errmsg << endl;
}



template <class T, int (*extractor)(xmmsc_result_t*, T*)>
T
PrimitiveProduct<T, extractor>::create() {
  T product;
  (*extractor)(this->res, &product);
  return product;
}



template <class T>
void
ObjectProduct<T>::cleanup() {
  // FIXME: How do we avoid the super-destructor to be called, so we do not
  // free the result?
}

template <class T>
T*
ObjectProduct<T>::create() {
  T* product = new T(this->res);
  return product;
}



template <class T, class X>
ComplexObjectProduct<T, X>::ComplexObjectProduct(X extra) : extraParam(extra) {
}

template <class T, class X>
T
ComplexObjectProduct<T, X>::create() {
  T product(this->res, extraParam);
  return product;
}



template <class T, int (*extractor)(xmmsc_result_t*, T*)>
ComparatorProduct<T, extractor>::ComparatorProduct(T _value) : value(_value) {
}

template <class T, int (*extractor)(xmmsc_result_t*, T*)>
bool
ComparatorProduct<T, extractor>::create() {
  T product;
  (*extractor)(res, &product);
  return (product == value);
}


#endif  // __PRODUCTMAKER_HH__
