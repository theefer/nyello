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
PatternQuery::saveResults(AbstractResult* songlist) {
  resultCache.erase(resultCache.begin(), resultCache.end());
  // FIXME: causes a segfault?
  songlist->rewind();
  while(songlist->isValid()) {
    resultCache.push_back(songlist->getId());
    songlist->next();
  }
}


char*
PatternQuery::getSql() {
  return queryCache->getQuery();
}


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
