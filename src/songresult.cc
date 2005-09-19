#include "songresult.hh"


SongResult::SongResult(xmmsc_result_t* _res, xmmsc_connection_t* _conn)
  : AbstractResult(_res), conn(_conn) {
  cache = NULL;
}

SongResult::~SongResult() {
}

unsigned int
SongResult::getId() {
  unsigned int currentId;
  xmmsc_result_get_uint(res, &currentId);
  return currentId;
}

char*
SongResult::get(char* key) {
  // Setup the internal cache
  if(cache == NULL) {
    cache = xmmsc_medialib_get_info(conn, getId());
    xmmsc_result_wait(cache);
  }

  // Retrieve the value
  return getDictEntryAsStr(cache, key);
}

bool
SongResult::next() {
  // Update internal state
  if(cache != NULL) {
    xmmsc_result_unref(cache);
    cache = NULL;
  }
  ++counter;

  // Try to move forward in the list
  return xmmsc_result_list_next(res);
}
