#include "querysonglist.hh"

QuerySongList::QuerySongList(xmmsc_result_t* _res, xmmsc_connection_t* _conn)
  : DictList(_res), conn(_conn) {
  cache = NULL;
}

char*
QuerySongList::get(char* key) {
  // Setup the internal cache
  if(cache == NULL) {
    getId();
    cache = xmmsc_medialib_get_info(conn, currentId);
    xmmsc_result_wait(cache);
  }

  // Retrieve the value
  return getDictEntryAsStr(cache, key);
}

bool
QuerySongList::next() {
  // Update internal state
  if(cache != NULL) {
    xmmsc_result_unref(cache);
    cache = NULL;
  }

  // Try to move forward in the list
  return xmmsc_result_list_next(res);
}
