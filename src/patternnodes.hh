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


#ifndef __PATTERNNODES_HH__
#define __PATTERNNODES_HH__

#include <list>

using namespace std;

#include "patternnodes.hh"
#include "medialibquery.hh"


class PatternNode;
class PatternOrder;
typedef list<PatternNode*> PatternNodeList;
typedef list<PatternOrder*> PatternOrderList;


class PatternNode {
public:

  virtual void appendToQuery(MedialibQuery* query) = 0;

};


class PatternOperator : public PatternNode {
public:
  PatternOperator(int operId);
  ~PatternOperator();

  static const int OPERATOR_AND = 0;
  static const int OPERATOR_OR  = 1;
  static const int OPERATOR_XOR = 2;
  static const int OPERATOR_NOT = 3;

  virtual void appendToQuery(MedialibQuery* query);

  inline void setOperands(PatternNodeList* _oprds) { operands = _oprds; }
  inline int getOperatorId() { return operatorId; }

protected:
  PatternNodeList* operands;

private:
  int operatorId;

};


class PatternOperatorAnd : public PatternOperator {
public:
  PatternOperatorAnd();
  ~PatternOperatorAnd();
};

class PatternOperatorOr : public PatternOperator {
public:
  PatternOperatorOr();
  ~PatternOperatorOr();
};

class PatternOperatorXor : public PatternOperator {
public:
  PatternOperatorXor();
  ~PatternOperatorXor();
};

class PatternOperatorNot : public PatternOperator {
public:
  PatternOperatorNot();
  ~PatternOperatorNot();

  virtual void appendToQuery(MedialibQuery* query);
};


class PatternCondition : public PatternNode {
public:
  PatternCondition();
  ~PatternCondition();
};


class PatternMatchCondition : public PatternCondition {
public:
  PatternMatchCondition(char* value, bool exact = false);
  PatternMatchCondition(char* field, char* value, bool exact = false);
  ~PatternMatchCondition();

  virtual void appendToQuery(MedialibQuery* query);

private:
  char* field;
  char* value;
  bool exact;
};


class PatternMLibSequence : public PatternCondition {
public:
  PatternMLibSequence(IdSequence* seq);
  ~PatternMLibSequence();

  virtual void appendToQuery(MedialibQuery* query);

private:
  IdSequence* seq;
};


class PatternPlaylistSequence : public PatternCondition {
public:
  PatternPlaylistSequence(const string& plname, IdSequence* seq);
  ~PatternPlaylistSequence();

  virtual void appendToQuery(MedialibQuery* query);

private:
  string plname;
  IdSequence* seq;
};



class PatternOrderBy : public PatternNode {
public:
  PatternOrderBy();
  ~PatternOrderBy();

  virtual void appendToQuery(MedialibQuery* query);

  inline void addOrder(PatternOrder* order) { orderlist->push_back(order); }

protected:
  PatternOrderList* orderlist;

};


class PatternOrder : public PatternNode {
public:
  PatternOrder(char* field);
  ~PatternOrder();

  virtual void appendToQuery(MedialibQuery* query) = 0;

protected:
  char* field;
};

class PatternOrderField : public PatternOrder {
public:
  PatternOrderField(char* field, bool asc);
  ~PatternOrderField();

  virtual void appendToQuery(MedialibQuery* query);

protected:
  bool asc;
};

class PatternOrderFunction : public PatternOrder {
public:
  PatternOrderFunction(char* function);
  ~PatternOrderFunction();

  virtual void appendToQuery(MedialibQuery* query);
};


#endif  // __PATTERNNODES_HH__
