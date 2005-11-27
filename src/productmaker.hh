#ifndef __PRODUCTMAKER_HH__
#define __PRODUCTMAKER_HH__

using namespace std;

#include <cstring>
#include <xmmsclient/xmmsclient.h>


template <class T>
class ProductMaker {
public:
  virtual T create(xmmsc_result_t* res) = 0;
};



template <class T, int (*extractor)(xmmsc_result_t*, T*)>
class ComparatorProduct : public ProductMaker<bool> {
public:
  ComparatorProduct(T value);

  bool ComparatorProduct::create(xmmsc_result_t* res);

private:
  int value;
};



class StringMatcherProduct : public ProductMaker<bool> {
public:
  StringMatcherProduct(char* str);

  bool StringMatcherProduct::create(xmmsc_result_t* res);

private:
  char* str;
};


template <class T>
class ObjectProduct : public ProductMaker<T*> {
public:
  virtual T* create(xmmsc_result_t* res);
};


template <class T, class X>
class ComplexObjectProduct : public ProductMaker<T> {
public:
  ComplexObjectProduct(X extraParam);

  virtual T create(xmmsc_result_t* res);

private:
  X extraParam;
};





/*  == And here comes the implementation, in the header file due to
       boring GCC limitations...  == */


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
  xmmsc_result_unref(res);
  return product;
}




template <class T, int (*extractor)(xmmsc_result_t*, T*)>
ComparatorProduct<T, extractor>::ComparatorProduct(T _value) : value(_value) {
}

template <class T, int (*extractor)(xmmsc_result_t*, T*)>
bool
ComparatorProduct<T, extractor>::create(xmmsc_result_t* res) {
  T product;
  (*extractor)(res, &product);
  xmmsc_result_unref(res);
  return (product == value);
}



template <class T>
T*
ObjectProduct<T>::create(xmmsc_result_t* res) {
  T* product = new T(res);
  return product;
}



template <class T, class X>
ComplexObjectProduct<T, X>::ComplexObjectProduct(X extra) : extraParam(extra) {
}

template <class T, class X>
T
ComplexObjectProduct<T, X>::create(xmmsc_result_t* res) {
  T product(res, extraParam);
  return product;
}


#endif  // __PRODUCTMAKER_HH__
