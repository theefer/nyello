#include "medialibquery.hh"


MedialibQuery::MedialibQuery() {
  orderby = NULL;
  aliasCount = 0;
  conditions = "";

  // FIXME: Add all; and can't we do it in a clean way?
  ANY_FIELDS = "('artist','album','title')";
}

MedialibQuery::~MedialibQuery() {
  if(orderby != NULL) {
    delete orderby;
  }
}

void
MedialibQuery::appendStartGroup() {
  appendString("(");
}

void
MedialibQuery::appendEndGroup() {
  appendString(")");
}

void
MedialibQuery::appendString(char* str) {
  conditions += str;
}
void
MedialibQuery::appendString(unsigned int i) {
  // FIXME: Other way to append int?
  stringstream buffer;
  buffer << i;
  conditions += buffer.str();
}
void
MedialibQuery::appendProtectedString(char* str) {
  conditions += xmmsc_sqlite_prepare_string(str);
}

void
MedialibQuery::appendExactMatch(char* value) {
  appendStartGroup();
  appendAnyField();
  appendString(" AND ");
  appendExactValue(value);
  appendEndGroup();
  ++aliasCount;
}
void
MedialibQuery::appendApproxMatch(char* value) {
  appendStartGroup();
  appendAnyField();
  appendString(" AND ");
  appendApproxValue(value);
  appendEndGroup();
  ++aliasCount;
}
void
MedialibQuery::appendExactMatch(char* field, char* value) {
  appendStartGroup();
  appendThisField(field);
  appendString(" AND ");
  appendExactValue(value);
  appendEndGroup();
  ++aliasCount;
}
void
MedialibQuery::appendApproxMatch(char* field, char* value) {
  appendStartGroup();
  appendThisField(field);
  appendString(" AND ");
  appendApproxValue(value);
  appendEndGroup();
  ++aliasCount;
}


void
MedialibQuery::appendSequence(char* label, IdSequence* seq) {
  bool first = true;
  appendStartGroup();

  // Append individual values
  if(seq->hasValues()) {
    list<unsigned int>::iterator valIt;
    list<unsigned int> val = seq->getValues();
    appendString(label);
    appendString(" IN (");
    for(valIt = val.begin(); valIt != val.end(); ++valIt) {
      if(!first)
        appendString(",");
      appendString(*valIt);
      first = false;
    }
    appendString(")");
  }

  // Append ranges
  if(seq->hasRanges()) {
    list<pair<unsigned int,unsigned int> >::iterator rgIt;
    list<pair<unsigned int,unsigned int> > rg = seq->getRanges();
    for(rgIt = rg.begin(); rgIt != rg.end(); ++rgIt) {
      if(!first)
        appendString(" OR ");

      appendStartGroup();
      appendString(label);
      appendString(" >= ");
      appendString((*rgIt).first);
      appendString(" AND ");
      appendString(label);
      appendString(" <= ");
      appendString((*rgIt).second);
      appendEndGroup();
      first = false;
    }
  }

  // Append unbounded ranges
  if(seq->hasRangeToMax()) {
    if(!first)
      appendString(" OR ");

    appendString(label);
    appendString(" <= ");
    appendString(seq->getRangeToMax());
    first = false;
  }
  if(seq->hasRangeFromMin()) {
    if(!first)
      appendString(" OR ");

    appendString(label);
    appendString(" >= ");
    appendString(seq->getRangeFromMin());
    first = false;
  }

  appendEndGroup();
}

char*
MedialibQuery::getQuery() {
  int i;
  stringstream query;
  char* query_ret;

  // Append select
  query << "SELECT DISTINCT m0.id FROM Media as m0";
  for(i = 1; i < aliasCount; ++i) {
    query << ", Media as m" << i;
  }

  // Append conditions
  if((aliasCount > 0) || !conditions.empty()) {
    query << " WHERE ";
  }

  query << conditions;

  for(i = 1; i < aliasCount; ++i) {
    if((i > 1) || !conditions.empty()) {
      query << " AND ";
    }
    query << "m" << (i-1) << ".id = m" << i << ".id";
  }

  // Append ordering
  if(orderby != NULL) {
    query << " ORDER BY " << orderby;
  }

  // FIXME: Cleaner way?
  query_ret = new char[ query.str().length() + 1 ];
  strcpy(query_ret, query.str().c_str());
  return query_ret;
}


void
MedialibQuery::appendAnyField() {
  appendCurrentKey();
  appendString(" IN ");
  appendString(ANY_FIELDS);
}

void
MedialibQuery::appendThisField(char* field) {
  appendCurrentKey();
  appendString(" = ");
  appendProtectedString(field);
}

void
MedialibQuery::appendApproxValue(char* value) {
  // FIXME: Cleaner way to surround value with % ?
  char* percentvalue = new char[ strlen(value) + 3 ];
  strcpy(percentvalue+1, value);
  percentvalue[0] = '%';
  percentvalue[strlen(value) + 1] = '%';
  percentvalue[strlen(value) + 2] = '\0';

  appendString("LOWER(");
  appendCurrentValue();
  appendString(") LIKE LOWER(");
  appendProtectedString(percentvalue);
  appendString(")");
}

void
MedialibQuery::appendExactValue(char* value) {
  appendCurrentValue();
  appendString(" = ");
  appendProtectedString(value);
}

void
MedialibQuery::appendCurrentKey() {
  // FIXME: Other way to append int?
  stringstream buffer;
  buffer << "m" << aliasCount << ".key";
  conditions += buffer.str();
}

void
MedialibQuery::appendCurrentValue() {
  // FIXME: Other way to append int?
  stringstream buffer;
  buffer << "m" << aliasCount << ".value";
  conditions += buffer.str();
}
