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


#include "songresult.hh"


SongResult::SongResult(xmmsc_result_t* _res, xmmsc_connection_t* _conn)
  : AbstractResult(_res), conn(_conn) {
  cache = NULL;
}

SongResult::~SongResult() {
  flushCache();
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
  flushCache();
  ++counter;

  return xmmsc_result_list_next(res);
}

void
SongResult::fetchCache() {
  cache = xmmsc_medialib_get_info(conn, getId());
  delay = new Delayed<void>(cache);
  delay->wait();
}

void
SongResult::flushCache() {
  if(cache != NULL) {
    delete delay;
    delay = NULL;
    cache = NULL;
  }
}
