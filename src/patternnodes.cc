#include "patternnodes.hh"



/* MLib id sequence

query->appendStartGroup();

// for each comma-separated group
  query->appendStartGroup();

    query->appendString("id <OP> <N>");
    query->appendString(" AND ");
    query->appendString("id <OP> <N>");

  query->appendEndGroup();

  query->appendString(" OR ");


query->appendEndGroup();



== Thoughts ==

- generate query AND tree with same methods?



== Type -> Query ==

[current song]
id=<playlist_current_id>

  id = xmmsc_playback_current_id()
  return new PatternMLibSequence(id)


[previous pattern]
insert previous generated query

=> Warning: beware of references?


[match: -aTn Foo]
m0.key = 'artist' AND LOWER(m0.value) LIKE LOWER('Foo')  AND
m1.key = 'title'  AND m1.value = 'Foo'                   AND
m2.key = 'track'  AND LOWER(m2.value) LIKE LOWER('Foo')  AND
m0.id = m1.id AND m1.id = m2.id

=> Warning: - mID must be aliased as Media
            - when flag is "any", what is the key?


[mlib sequence: seq]
( make-sequence("id", seq) )


[playlist sequence: plname/seq]
id IN (SELECT GETID(entry)
       FROM Playlists, PlaylistEntries
       WHERE Playlists.name=''
         AND make-sequence("pos", seq))


[history sequence: N#seq]
id IN ( <list> )  [must be filtered by hand from seq]


*/


/*  == CONSTRUCTORS == */

PatternOperator::PatternOperator(int operId) : operatorId(operId) {
}

PatternOperatorAnd::PatternOperatorAnd() : PatternOperator(OPERATOR_AND) {
}

PatternOperatorOr::PatternOperatorOr() : PatternOperator(OPERATOR_OR) {
}

PatternOperatorXor::PatternOperatorXor() : PatternOperator(OPERATOR_XOR) {
}

PatternOperatorNot::PatternOperatorNot() : PatternOperator(OPERATOR_NOT) {
}


PatternCondition::PatternCondition() {
}

PatternMatchCondition::PatternMatchCondition(char* _value, bool _exact)
  : value(_value), exact(_exact) {
  field = NULL;
}
PatternMatchCondition::PatternMatchCondition(char* _field, char* _value, bool _exact)
  : field(_field), value(_value), exact(_exact) {
}

PatternMLibSequence::PatternMLibSequence(IdSequence* _seq) : seq(_seq) {
}

PatternPlaylistSequence::PatternPlaylistSequence(char* _plname, IdSequence* _seq) 
  : plname(_plname), seq(_seq) {
}



/*  == DESTRUCTORS == */

PatternOperator::~PatternOperator() {
  // FIXME: Delete operands
}

PatternOperatorAnd::~PatternOperatorAnd() { }
PatternOperatorOr::~PatternOperatorOr() { }
PatternOperatorXor::~PatternOperatorXor() { }
PatternOperatorNot::~PatternOperatorNot() { }

PatternCondition::~PatternCondition() { }
PatternMatchCondition::~PatternMatchCondition() { }
PatternMLibSequence::~PatternMLibSequence() { }
PatternPlaylistSequence::~PatternPlaylistSequence() {
  delete plname;
}


/*  == APPEND TO QUERY == */

void
PatternOperator::appendToQuery(MedialibQuery* query) {
  PatternNodeList::iterator it;
  query->appendStartGroup();
  for(it = operands->begin(); it != operands->end(); ++it) {
    if(it != operands->begin()) {
      switch(operatorId) {
      case OPERATOR_AND: query->appendString(" AND "); break;
      case OPERATOR_OR:  query->appendString(" OR ");  break;
      case OPERATOR_XOR: query->appendString(" XOR "); break;
      default: /* FIXME: shouldn't happen! */ break;
      }
    }

    (*it)->appendToQuery(query);
  }
  query->appendEndGroup();
}

void
PatternOperatorNot::appendToQuery(MedialibQuery* query) {
  query->appendStartGroup();
  query->appendString("NOT ");
  (operands->front())->appendToQuery(query);
  query->appendEndGroup();
}

void
PatternMatchCondition::appendToQuery(MedialibQuery* query) {
  if(field == NULL) {
    if(exact) query->appendExactMatch(value);
    else      query->appendApproxMatch(value);
  }
  else {
    if(exact) query->appendExactMatch(field, value);
    else      query->appendApproxMatch(field, value);
  }
}

void
PatternMLibSequence::appendToQuery(MedialibQuery* query) {
  query->appendSequence("m0.id", seq);
}

void
PatternPlaylistSequence::appendToQuery(MedialibQuery* query) {
  query->appendString("m0.id IN ("
                      "SELECT substr(entry, 8, 10) "
                      "FROM PlaylistEntries, Playlist "
                      "WHERE substr(entry, 0, 7)=\"mlib://\" AND "
                            "playlist_id=id AND "
                            "name=");
  query->appendProtectedString(plname);
  if(!seq->empty()) {
    query->appendString(" AND ");
    query->appendSequence("pos", seq);
  }
  query->appendString(" ORDER BY pos)");
}
