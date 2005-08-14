#include "playlistlist.hh"

PlaylistList::PlaylistList(xmmsc_result_t* _res, char* _playingPlaylist)
  : DictList(_res), playingPlaylist(_playingPlaylist) {
}

bool
PlaylistList::isSelected() {
  return (strcmp(get("name"), playingPlaylist) == 0);
}
