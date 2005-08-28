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

  inline void setOrderBy(char* order) { orderby = order; }

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

  char* getQuery();

private:
  // FIXME: Add all
  char* ANY_FIELDS; // = "('artist','album')";

  int aliasCount;
  string conditions;
  char* orderby;

  void appendAnyField();
  void appendThisField(char* field);
  void appendApproxValue(char* value);
  void appendExactValue(char* value);

  void appendCurrentKey();
  void appendCurrentValue();
};

#endif  // __MEDIALIBQUERY_HH__
