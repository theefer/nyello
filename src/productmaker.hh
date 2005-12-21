#ifndef __PRODUCTMAKER_HH__
#define __PRODUCTMAKER_HH__

#include <cstring>
#include <iostream>
#include <xmmsclient/xmmsclient.h>

using namespace std;


template <typename T>
class ProductMaker {
public:
  ~ProductMaker();

  inline void setResult(xmmsc_result_t* result) { res = result; }

  virtual void checkErrors(string errmsg);
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
  virtual void create();
};


template <typename T, int (*extractor)(xmmsc_result_t*, T*)>
class PrimitiveProduct : public ProductMaker<T> {
public:
  virtual T create();
};


template <typename T, int (*extractor)(xmmsc_result_t*, const char*, T*)>
class PrimitiveDictProduct : public ProductMaker<T> {
public:
  PrimitiveDictProduct(const char* key);

  virtual T create();

private:
  const char* key;
};



template <typename T>
class ObjectProduct : public ProductMaker<T*> {
public:
  virtual T* create();
};



template <typename T, class X>
class ComplexObjectProduct : public ProductMaker<T*> {
public:
  ComplexObjectProduct(X extraParam);

  virtual T* create();

private:
  X extraParam;
};



template <typename T, int (*extractor)(xmmsc_result_t*, T*)>
class ComparatorProduct : public ProductMaker<bool> {
public:
  ComparatorProduct(T value);

  virtual bool create();

private:
  T value;
};



class StringMatcherProduct : public ProductMaker<bool> {
public:
  StringMatcherProduct(string str);

  virtual bool create();

private:
  string str;
};



/*  == And here comes the implementation of template functions. == */

template <typename T>
ProductMaker<T>::~ProductMaker() {
  if(unrefResult) {
    xmmsc_result_unref(res);
  }
}

template <typename T>
void
ProductMaker<T>::checkErrors(string errmsg) {
  if(xmmsc_result_iserror(res)) {
    // FIXME: Show error? Throw error?
    if(errmsg.size() > 0) {
      cerr << errmsg << xmmsc_result_get_error(res) << endl;
    }
  }
}



template <typename T, int (*extractor)(xmmsc_result_t*, T*)>
T
PrimitiveProduct<T, extractor>::create() {
  this->unrefResult = true;

  T product;
  (*extractor)(this->res, &product);
  return product;
}


template <typename T, int (*extractor)(xmmsc_result_t*, const char*, T*)>
PrimitiveDictProduct<T, extractor>::PrimitiveDictProduct(const char* _key) : key(_key) {
}

template <typename T, int (*extractor)(xmmsc_result_t*, const char*, T*)>
T
PrimitiveDictProduct<T, extractor>::create() {
  this->unrefResult = true;

  T product;
  (*extractor)(this->res, key, &product);
  return product;
}


template <typename T>
T*
ObjectProduct<T>::create() {
  this->unrefResult = false;

  T* product = new T(this->res);
  return product;
}



template <typename T, class X>
ComplexObjectProduct<T, X>::ComplexObjectProduct(X extra) : extraParam(extra) {
}

template <typename T, class X>
T*
ComplexObjectProduct<T, X>::create() {
  this->unrefResult = false;

  T* product = new T(this->res, extraParam);
  return product;
}



template <typename T, int (*extractor)(xmmsc_result_t*, T*)>
ComparatorProduct<T, extractor>::ComparatorProduct(T _value) : value(_value) {
}

template <typename T, int (*extractor)(xmmsc_result_t*, T*)>
bool
ComparatorProduct<T, extractor>::create() {
  this->unrefResult = true;

  T product;
  (*extractor)(res, &product);
  return (product == value);
}


#endif  // __PRODUCTMAKER_HH__
