#include "resultlist.hh"

ResultList::ResultList(AbstractResult* _result) : result(_result) {
}

ResultList::~ResultList() {
  delete result;
}
