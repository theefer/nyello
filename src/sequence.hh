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


#ifndef __SEQUENCE_HH__
#define __SEQUENCE_HH__

#include <list>
#include <utility>
#include <string>
#include <sstream>

using namespace std;


template <typename T> class Sequence;

typedef Sequence<unsigned int> IdSequence;
typedef Sequence<unsigned int> PositionSequence;


template <typename T>
class Sequence {
public:

  Sequence();
  ~Sequence();

  void addValue(T val);
  void addRange(T from, T to);
  void addRangeFrom(T from);
  void addRangeTo(T to);

  bool parseAdd(char* seq);

  bool contains(T elem);

  bool empty();

  inline bool hasValues() { return !values.empty(); }
  inline list<T> getValues() { return values; }

  inline bool hasRanges() { return !ranges.empty(); }
  inline list<pair<T,T> > getRanges() { return ranges; }

  inline bool hasRangeToMax()   { return rangeToMaxIsset; }
  inline bool hasRangeFromMin() { return rangeFromMinIsset; }
  inline T getRangeToMax()   { return rangeToMax; }
  inline T getRangeFromMin() { return rangeFromMin; }

private:
  T rangeToMax;
  T rangeFromMin;
  list<T> values;
  list<pair<T,T> > ranges;

  bool rangeToMaxIsset;
  bool rangeFromMinIsset;

  T convToT(string s);
  
};



/*  == And here comes the implementation of template functions. == */

template <typename T>
Sequence<T>::Sequence() {
  rangeToMaxIsset   = false;
  rangeFromMinIsset = false;
}

template <typename T>
Sequence<T>::~Sequence() {

}

template <typename T>
void
Sequence<T>::addValue(T val) {
  if(!contains(val)) {
    // FIXME: Insert in a sorted order
    values.push_back(val);
  }
}

template <typename T>
void
Sequence<T>::addRange(T from, T to) {
  // Invalid range, stop now
  if(from > to) {
    return;
  }

  if(from == to) {
    addValue(from);
    return;
  }

  // Already contained in unbounded range
  if(rangeToMaxIsset && rangeFromMinIsset &&
     ((to <= rangeToMax) || (from >= rangeFromMin))) {
    return;
  }

  // Merge with unbounded range
  if(rangeToMaxIsset && (from <= rangeToMax)) {
    addRangeTo(to);
  }
  else if(rangeFromMinIsset && (to >= rangeFromMin)) {
    addRangeFrom(from);
  }

  // Just register the range
  else {
    pair<T,T> range(from, to);
    ranges.push_back(range);
  }

  // FIXME: Merge with other ranges?
  // FIXME: Remove values in range
}

template <typename T>
void
Sequence<T>::addRangeFrom(T from) {
  // Initialize
  if(!rangeFromMinIsset) {
    rangeFromMin = from;
    rangeFromMinIsset = true;
  }
  // Take minimum
  else if(from < rangeFromMin) {
    rangeFromMin = from;
    // FIXME: Update values and ranges
  }
}

template <typename T>
void
Sequence<T>::addRangeTo(T to) {
  // Initialize
  if(!rangeToMaxIsset) {
    rangeToMax = to;
    rangeToMaxIsset = true;
  }
  // Take maximum
  else if(to > rangeToMax) {
    rangeToMax = to;
    // FIXME: Update values and ranges
  }
}

template <typename T>
bool
Sequence<T>::parseAdd(char* char_seq) {
  // It's really too painful without C++ strings...
  string seq(char_seq);
  string token;
  int token_len;
  int pos_range;

  while(seq.length() > 0) {
    // Isolate token, update seq
    token_len = seq.find(',');
    if(token_len != string::npos) {
      token = seq.substr(0, token_len);
      seq.erase(0, token_len + 1);
    }
    else {
      token = seq;
      seq.erase();
    }

    // Find range or value
    pos_range = token.find('-');
    if(pos_range != string::npos) {
      string from = token.substr(0, pos_range);
      string to = token.substr(pos_range + 1, token.length() - pos_range - 1);
      
      if(!from.empty() && !to.empty())
        addRange(convToT(from), convToT(to));
      else if(!from.empty())
        addRangeFrom(convToT(from));
      else if(!to.empty())
        addRangeTo(convToT(to));
    }
    else {
      addValue(convToT(token));
    }
  }

  // FIXME: return bool success
  return true;
}

template <typename T>
bool
Sequence<T>::contains(T elem) {
  typename list<T>::iterator valIt;
  typename list<pair<T,T> >::iterator rangeIt;

  // In semi-bounded ranges?
  if((rangeToMaxIsset && elem <= rangeToMax) ||
     (rangeFromMinIsset && elem >= rangeFromMin)) {
    return true;
  }

  /* In value list?
  for(int i = 0; i < values.size(); ++i) {
    // FIXME: When list is sorted, shorten that
    if(elem == values[i])
      return true;
  }
  */

  // In value list?
  for(valIt = values.begin(); valIt != values.end(); ++valIt) {
    // FIXME: When list is sorted, shorten that
    if(elem == *valIt)
      return true;
  }

  /* In range list?
  for(int i = 0; i < ranges.size(); ++i) {
    // FIXME: When list is sorted, shorten that
    if((elem >= ranges[i].first) && (elem <= ranges[i].second))
      return true;
  }
  */

  // In range list?
  for(rangeIt = ranges.begin(); rangeIt != ranges.end(); ++rangeIt) {
    // FIXME: When list is sorted, shorten that
    if((elem >= (*rangeIt).first) && (elem <= (*rangeIt).second))
      return true;
  }

  return false;
}

template <typename T>
bool
Sequence<T>::empty() {
  return (!hasRangeToMax() && !hasRangeFromMin()
          && values.empty() && ranges.empty());
}

/**
 * A bit of a hack, but allows us to convert a string to type T.
 */
template <typename T>
T
Sequence<T>::convToT(string s) {
  T converted;
  stringstream sstream(s);
  sstream >> converted;
  return converted;
}


#endif  // __SEQUENCE_HH__
