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


#ifndef __MEDIALIBQUERY_HH__
#define __MEDIALIBQUERY_HH__


#include <string>
#include <sstream>
using namespace std;


#include "sequence.hh"
#include <xmmsclient/xmmsclient.h>


/**
 *
 */
class MedialibQuery {
public:

  MedialibQuery();
  ~MedialibQuery();

  void appendStartGroup();
  void appendEndGroup();

  void appendString(const string& str);
  void appendString(char* str);
  void appendString(unsigned int i);
  void appendProtectedString(const string& str);
  void appendProtectedString(char* str);

  void appendExactMatch(char* value);
  void appendApproxMatch(char* value);
  void appendExactMatch(char* field, char* value);
  void appendApproxMatch(char* field, char* value);

  void appendSequence(char* label, IdSequence* seq);

  void appendOrderField(char* field, bool asc);
  void appendOrderFunction(char* function);

  string getQuery();

private:
  // Lists all fields searched when no key selected
  list<string> anyFieldList;

  int aliasCount;
  int orderCount;
  stringstream joins;
  string conditions;
  string orderby;

  void appendOrderBy(char* field, bool asc = true);
  void appendThisOrderField(char* field);

  void appendAnyField();
  void appendThisField(const string& field);
  void appendThisField(char* field);
  void appendApproxValue(char* value);
  void appendExactValue(char* value);

  void appendCurrentKey();
  void appendCurrentValue();
  void appendCurrentField(char* field);
};

#endif  // __MEDIALIBQUERY_HH__
