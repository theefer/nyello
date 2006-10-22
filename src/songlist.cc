/* nyello - an advanced command-line client for XMMS2
 * Copyright (C) 2006  Sébastien Cevey
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */


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
