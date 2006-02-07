#include "medialibquery.hh"


MedialibQuery::MedialibQuery() {
  orderby = "";
  aliasCount = 0;
  orderCount = 0;
  conditions = "";

  anyFieldList.push_back("artist");
  anyFieldList.push_back("album");
  anyFieldList.push_back("title");
}

MedialibQuery::~MedialibQuery() {
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
MedialibQuery::appendString(const string& str) {
  conditions += str;
}

void
MedialibQuery::appendString(char* str) {
  conditions += str;
}
void
MedialibQuery::appendString(unsigned int i) {
  stringstream buffer;
  buffer << i;
  conditions += buffer.str();
}

void
MedialibQuery::appendProtectedString(const string& str) {
  char* prep_str = xmmsc_sqlite_prepare_string(str.c_str());
  conditions += prep_str;
  delete prep_str;
}
void
MedialibQuery::appendProtectedString(char* str) {
  char* prep_str = xmmsc_sqlite_prepare_string(str);
  conditions += prep_str;
  delete prep_str;
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

  // Empty sequence: append FALSE
  if(seq->empty()) {
    appendString("0");
    return;
  }


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


/**
 * Append an ordering field to the query.
 */
void
MedialibQuery::appendOrderField(char* field, bool asc) {
  appendThisOrderField(field);
  appendOrderBy(NULL, asc);
  ++orderCount;
}

/**
 * Append an ordering function to the query.
 */
void
MedialibQuery::appendOrderFunction(char* function) {
  appendOrderBy(function);  
}

/**
 * Append a field to the ORDER BY clause of the query.
 */
void
MedialibQuery::appendOrderBy(char* field, bool asc) {
  if(!orderby.empty()) {
    orderby += ", ";
  }

  // FIXME: Find a cleaner way to do this
  if(field != NULL) {
    orderby += field;
  }
  else {
    stringstream buffer;
    buffer << "j" << orderCount << ".value";
    orderby += buffer.str();
  }

  if(!asc) {
    orderby += " DESC";
  }
}


string
MedialibQuery::getQuery() {
  int i;
  stringstream query;

  // Append select
  query << "SELECT DISTINCT m0.id FROM Media as m0";
  for(i = 1; i < aliasCount; ++i) {
    query << ", Media as m" << i;
  }
  query << joins.str();

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
  if(!orderby.empty()) {
    query << " ORDER BY " << orderby;
  }

  return query.str();
}


void
MedialibQuery::appendAnyField() {
  list<string>::iterator fieldIt;

  appendStartGroup();
  for(fieldIt = anyFieldList.begin(); fieldIt != anyFieldList.end(); ++fieldIt) {
    if(fieldIt != anyFieldList.begin()) {
      appendString(" OR ");
    }
    appendThisField(*fieldIt);
  }
  appendEndGroup();
}

void
MedialibQuery::appendThisOrderField(char* field) {
  char* prep_field = xmmsc_sqlite_prepare_string(field);
  joins << " LEFT JOIN Media as j" << orderCount
        << " ON m0.id=j" << orderCount << ".id"
        << " AND j" << orderCount << ".key = " << prep_field;
  delete prep_field;
}

void
MedialibQuery::appendThisField(const string& field) {
  appendCurrentKey();
  appendString(" = ");
  appendProtectedString(field);
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
  appendCurrentField("key");
}

void
MedialibQuery::appendCurrentValue() {
  appendCurrentField("value");
}

void
MedialibQuery::appendCurrentField(char* field) {
  stringstream buffer;
  buffer << "m" << aliasCount << "." << field;
  conditions += buffer.str();
}
