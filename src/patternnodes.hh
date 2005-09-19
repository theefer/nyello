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
  PatternPlaylistSequence(char* plname, IdSequence* seq);
  ~PatternPlaylistSequence();

  virtual void appendToQuery(MedialibQuery* query);

private:
  char* plname;
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
