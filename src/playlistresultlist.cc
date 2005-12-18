#include "playlistresultlist.hh"

PlaylistResultList::PlaylistResultList(AbstractResult* _result, string _name)
  : ResultList(_result), playingPlaylist(_name) {
}

PlaylistResultList::~PlaylistResultList() {
}

bool
PlaylistResultList::isSelected() {
  return (playingPlaylist == get("name"));
}
