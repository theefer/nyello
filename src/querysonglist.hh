#ifndef __QUERYSONGLIST_HH__
#define __QUERYSONGLIST_HH__

#include "dictlist.hh"


class QuerySongList : public DictList {
public:
  QuerySongList(xmmsc_result_t* res);

  // There is no song selected in a query!
  inline bool isSelected() { return false; }

};


#endif  // __QUERYSONGLIST_HH__
