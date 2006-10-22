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


#include "patternquery.hh"


PatternQuery::PatternQuery(PatternNode* top, PatternOrderBy* orderby)
  : topNode(top), orderNode(orderby) {
  queryCache = new MedialibQuery();
  topNode->appendToQuery(queryCache);
  orderNode->appendToQuery(queryCache);
}

PatternQuery::~PatternQuery() {
  // FIXME: Or unref until it's actually freed?
  delete topNode;

  // FIXME: buggy if we delete the default orderby
  // delete orderNode;

  if(queryCache != NULL) {
    delete queryCache;
  }
}


void
PatternQuery::saveResults(SelectResult* songlist) {
  resultCache.erase(resultCache.begin(), resultCache.end());
  // FIXME: causes a segfault?
  songlist->rewind();
  while(songlist->isValid()) {
    resultCache.push_back(songlist->getId());
    songlist->next();
  }
}


string
PatternQuery::getSql() {
  return queryCache->getQuery();
}


/**
 * Get the ids of all songs matched by the query, as a sequence.
 */
IdSequence*
PatternQuery::getIds() {
  IdSequence* res_seq = new IdSequence();

  vector<unsigned int>::iterator it;
  for(it = resultCache.begin(); it != resultCache.end(); ++it) {
    res_seq->addValue(*it);
  }

  return res_seq;
}

/**
 * Get the ids of all songs matched by the query and with position
 * within the given sequence, as a sequence.
 */
IdSequence*
PatternQuery::getIdsFromSequence(IdSequence* seq) {
  IdSequence* res_seq = new IdSequence();

  int c;
  vector<unsigned int>::iterator it;
  for(it = resultCache.begin(), c = 1; it != resultCache.end(); ++it, ++c) {
    if(seq->contains(c)) {
      res_seq->addValue(*it);
    }
  }

  return res_seq;
}
