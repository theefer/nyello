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

  void appendString(char* str);
  void appendString(unsigned int i);
  void appendProtectedString(char* str);

  void appendExactMatch(char* value);
  void appendApproxMatch(char* value);
  void appendExactMatch(char* field, char* value);
  void appendApproxMatch(char* field, char* value);

  void appendSequence(char* label, IdSequence* seq);

  void appendOrderField(char* field, bool asc);
  void appendOrderFunction(char* function);

  char* getQuery();

private:
  char* ANY_FIELDS;

  int aliasCount;
  int orderCount;
  stringstream joins;
  string conditions;
  string orderby;

  void appendOrderBy(char* field, bool asc = true);
  void appendThisOrderField(char* field);

  void appendAnyField();
  void appendThisField(char* field);
  void appendApproxValue(char* value);
  void appendExactValue(char* value);

  void appendCurrentKey();
  void appendCurrentValue();
  void appendCurrentField(char* field);
};

#endif  // __MEDIALIBQUERY_HH__
