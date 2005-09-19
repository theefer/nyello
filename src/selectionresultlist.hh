#ifndef __SELECTIONRESULTLIST_HH__
#define __SELECTIONRESULTLIST_HH__

#include <xmmsclient/xmmsclient.h>

#include "resultlist.hh"


class SelectionResultList : public ResultList {
public:

  SelectionResultList(AbstractResult* result, unsigned int selectedPos);
  ~SelectionResultList();

  virtual bool isSelected() { return (result->getId() == selectedPos); }

private:
  unsigned int selectedPos;

};


#endif  // __SELECTIONRESULTLIST_HH__
