#include "songlist.hh"


SongList::SongList(xmmsc_result_t* _res, xmmsc_connection_t* _conn) {
  songres = new SongResult(_res, _conn);
}

PlaylistSongList::~PlaylistSongList() {
  delete songres;
}

unsigned int
PlaylistSongList::getId() {
  return songres->getId();
}

char*
PlaylistSongList::get(char* key) {
  return songres->get(key);
}

bool
PlaylistSongList::next() {
  return songres->next();
}

void
PlaylistSongList::rewind() {
  songres->rewind();
}

bool
PlaylistSongList::isValid() {
  return songres->isValid();
}
