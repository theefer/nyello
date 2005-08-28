#ifndef __PATTERNNODES_HH__
#define __PATTERNNODES_HH__

#include <list>

using namespace std;

#include "patternnodes.hh"
#include "medialibquery.hh"


class PatternNode;
typedef list<PatternNode*> PatternNodeList;


class PatternNode {
public:

  virtual void appendToQuery(MedialibQuery* query) = 0;

private:
};


class PatternOperator : public PatternNode {
public:
  static const int OPERATOR_AND = 0;
  static const int OPERATOR_OR  = 1;
  static const int OPERATOR_XOR = 2;
  static const int OPERATOR_NOT = 3;

  virtual void appendToQuery(MedialibQuery* query);

  inline void setOperands(PatternNodeList* _oprds) { operands = _oprds; }
  inline int getOperatorId() { return operatorId; }

protected:
  PatternOperator(int operId);
  ~PatternOperator();

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

private:
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

#endif  // __PATTERNNODES_HH__
