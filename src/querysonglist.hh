#ifndef __QUERYSONGLIST_HH__
#define __QUERYSONGLIST_HH__

#include "dictlist.hh"


// Or do we actually get ids only?


class QuerySongList : public DictList {
public:
  QuerySongList(xmmsc_result_t* res, xmmsc_connection_t* conn);

  char* get(char* key);
  bool  next();

  // There is no song selected in a query!
  inline bool isSelected() { return false; }

private:
  /**
   * The connection to the server.
   */
  xmmsc_connection_t* conn;

  /**
   * Result referencing the dict storing the metadata of the current
   * song.
   */
  xmmsc_result_t* cache;

};


#endif  // __QUERYSONGLIST_HH__
