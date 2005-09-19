#include "playlistresultlist.hh"

PlaylistResultList::PlaylistResultList(AbstractResult* _result, const char* _name)
  : ResultList(_result), playingPlaylist(_name) {
}

PlaylistResultList::~PlaylistResultList() {
}

bool
PlaylistResultList::isSelected() {
  return (strcmp(get("name"), playingPlaylist) == 0);
}
