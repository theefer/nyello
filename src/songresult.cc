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
  if(cache == NULL) {
    fetchCache();
  }

  return getDictEntryAsStr(cache, key);
}

int
SongResult::getInt(char* key) {
  int32_t bufferInt;

  if(cache == NULL) {
    fetchCache();
  }

  xmmsc_result_get_dict_entry_int32(cache, key, &bufferInt);
  return bufferInt;
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

void
SongResult::fetchCache() {
  cache = xmmsc_medialib_get_info(conn, getId());
  xmmsc_result_wait(cache);
}