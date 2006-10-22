/* nyello - an advanced command-line client for XMMS2
 * Copyright (C) 2006  Sébastien Cevey
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */


#ifndef __PRODUCTMAKER_HH__
#define __PRODUCTMAKER_HH__

#include <cstring>
#include <iostream>
#include <xmmsclient/xmmsclient.h>

using namespace std;


/**
 * Generic ProductMaker parent class implementing methods shared by
 * all ProductMaker specializations.
 */
template <typename T>
class AbstractProductMaker {
public:
  inline void setResult(xmmsc_result_t* result) { res = result; }

  virtual void checkErrors(string errmsg);
  virtual T create() = 0;

protected:
  xmmsc_result_t* res;
};

/**
 * Standard ProductMaker specialization class; frees the associated
 * result.
 */
template <typename T>
class ProductMaker : public AbstractProductMaker<T> {
public:
  ~ProductMaker();
};

/**
 * ProductMaker specialization class for pointer products; does not
 * free the associated result.
 */
template <typename T>
class ProductMaker<T*> : public AbstractProductMaker<T*> {
public:
  ~ProductMaker();
};



/**
 * Dumb concrete instance of ProductMaker that can be instantiated for
 * void products.
 */
class VoidProduct : public ProductMaker<void> {
public:
  inline virtual void create() { }
};


/**
 * Produces a value of a primitive type using the provided function to
 * extract it from the result.
 */
template <typename T, int (*extractor)(xmmsc_result_t*, T*)>
class PrimitiveProduct : public ProductMaker<T> {
public:
  virtual T create();
};

/**
 * Produces a value of a primitive type using the provided function to
 * extract it from the dict result (under the given key).
 */
template <typename T, int (*extractor)(xmmsc_result_t*, const char*, T*)>
class PrimitiveDictProduct : public ProductMaker<T> {
public:
  PrimitiveDictProduct(const char* key);

  virtual T create();

private:
  const char* key;
};


/**
 * Produces a value of a pointer to an object created by calling its
 * constructor on the result.
 */
template <typename T>
class ObjectProduct : public ProductMaker<T*> {
public:
  virtual T* create();
};


/**
 * Produces a value of a pointer to an object created by calling its
 * constructor on the result, with a second argument provided to this
 * object.
 */
template <typename T, class X>
class ComplexObjectProduct : public ProductMaker<T*> {
public:
  ComplexObjectProduct(X extraParam);

  virtual T* create();

private:
  X extraParam;
};


/**
 * Produces a boolean comparison between the given value and the value
 * extracted from the result using the provided function.
 */
template <typename T, int (*extractor)(xmmsc_result_t*, T*)>
class ComparatorProduct : public ProductMaker<bool> {
public:
  ComparatorProduct(T value);

  virtual bool create();

private:
  T value;
};


/**
 * Reads the list contained in the result and determines whether the
 * given string is in that list or not.
 */
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
  xmmsc_result_unref(this->res);
}

template <typename T>
ProductMaker<T*>::~ProductMaker() {
  // Do not unref the result when producing a pointer to an object.
}


template <typename T>
void
AbstractProductMaker<T>::checkErrors(string errmsg) {
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
  T product;
  (*extractor)(this->res, key, &product);
  return product;
}


template <typename T>
T*
ObjectProduct<T>::create() {
  T* product = new T(this->res);
  return product;
}



template <typename T, class X>
ComplexObjectProduct<T, X>::ComplexObjectProduct(X extra) : extraParam(extra) {
}

template <typename T, class X>
T*
ComplexObjectProduct<T, X>::create() {
  T* product = new T(this->res, extraParam);
  return product;
}



template <typename T, int (*extractor)(xmmsc_result_t*, T*)>
ComparatorProduct<T, extractor>::ComparatorProduct(T _value) : value(_value) {
}

template <typename T, int (*extractor)(xmmsc_result_t*, T*)>
bool
ComparatorProduct<T, extractor>::create() {
  T product;
  (*extractor)(res, &product);
  return (product == value);
}


#endif  // __PRODUCTMAKER_HH__
