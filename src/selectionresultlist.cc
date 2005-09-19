#include "selectionresultlist.hh"

SelectionResultList::SelectionResultList(AbstractResult* _result, unsigned int pos)
  : ResultList(_result), selectedPos(pos) {
}

SelectionResultList::~SelectionResultList() {
}
