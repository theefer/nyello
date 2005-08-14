#include "playlistsonglist.hh"


PlaylistSongList::PlaylistSongList(xmmsc_result_t* _res, xmmsc_connection_t* _conn, int _playingPos)
  : res(_res), conn(_conn), playingPos(_playingPos) {
  cache = NULL;
  currentPos = 0;
}

PlaylistSongList::~PlaylistSongList() {
  xmmsc_result_unref(res);
}

char*
PlaylistSongList::get(char* key) {
  // Setup the internal cache
  if(cache == NULL) {
    xmmsc_result_get_uint(res, &currentId);
    cache = xmmsc_medialib_get_info(conn, currentId);
    xmmsc_result_wait(cache);
  }

  // Retrieve the value
  return getDictEntryAsStr(cache, key);
}

bool
PlaylistSongList::next() {
  // Update internal state
  xmmsc_result_unref(cache);
  cache = NULL;
  ++currentPos;

  // Try to move forward in the list
  return xmmsc_result_list_next(res);
}

bool
PlaylistSongList::isValid() {
  return xmmsc_result_list_valid(res);
}
