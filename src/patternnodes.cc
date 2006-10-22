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


#include "patternnodes.hh"


/*  == CONSTRUCTORS == */

PatternOperator::PatternOperator(int operId) : operatorId(operId) {
  operands = NULL;
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

PatternPlaylistSequence::PatternPlaylistSequence(const string& _plname, IdSequence* _seq) 
  : plname(_plname), seq(_seq) {
}


PatternOrderBy::PatternOrderBy() {
  orderlist = new PatternOrderList();
}

PatternOrder::PatternOrder(char* _field) : field(_field) {
}

PatternOrderField::PatternOrderField(char* _field, bool _asc)
  : PatternOrder(_field), asc(_asc) {
}

PatternOrderFunction::PatternOrderFunction(char* _function)
  : PatternOrder(_function) {
}


/*  == DESTRUCTORS == */

PatternOperator::~PatternOperator() {
  // Delete operands
  if(operands != NULL) {
    PatternNodeList::iterator it;
    for(it = operands->begin(); it != operands->end(); ++it) {
      delete (*it);
    }
  }
}

PatternOperatorAnd::~PatternOperatorAnd() { }
PatternOperatorOr::~PatternOperatorOr() { }
PatternOperatorXor::~PatternOperatorXor() { }
PatternOperatorNot::~PatternOperatorNot() { }

PatternCondition::~PatternCondition() { }
PatternMatchCondition::~PatternMatchCondition() {
  delete value;
}
PatternMLibSequence::~PatternMLibSequence() {
  delete seq;
}
PatternPlaylistSequence::~PatternPlaylistSequence() {
  delete seq;
}


PatternOrderBy::~PatternOrderBy() {
  // Delete orderlist
  if(orderlist != NULL) {
    PatternOrderList::iterator it;
    for(it = orderlist->begin(); it != orderlist->end(); ++it) {
      delete (*it);
    }
  }
}

PatternOrder::~PatternOrder() { }
PatternOrderField::~PatternOrderField() { }
PatternOrderFunction::~PatternOrderFunction() { }


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
      default:  /* shouldn't ever happen! */  break;
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
  // FIXME: What of entries not starting with "mlib://" ?
  query->appendString("m0.id IN ("
                      "SELECT substr(entry, 8, 10) "
                      "FROM PlaylistEntries, Playlist "
                      "WHERE substr(entry, 0, 7)=\"mlib://\" AND "
                            "playlist_id=id AND "
                            "name=");
  query->appendProtectedString(plname);
  if(!seq->empty()) {
    query->appendString(" AND ");
    query->appendSequence("PlaylistEntries.pos", seq);
  }
  query->appendString(" ORDER BY Playlist.pos, PlaylistEntries.pos)");
}


void
PatternOrderBy::appendToQuery(MedialibQuery* query) {
  PatternOrderList::iterator it;
  for(it = orderlist->begin(); it != orderlist->end(); ++it) {
    (*it)->appendToQuery(query);
  }
}

void
PatternOrderField::appendToQuery(MedialibQuery* query) {
  query->appendOrderField(field, asc);
}

void
PatternOrderFunction::appendToQuery(MedialibQuery* query) {
  query->appendOrderFunction(field);
}
