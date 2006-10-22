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


#ifndef __SONGLIST_HH__
#define __SONGLIST_HH__

#include <xmmsclient/xmmsclient.h>

#include "printable.hh"


class SongList : public Printable {
public:
  PlaylistSongList(xmmsc_result_t* res, xmmsc_connection_t* conn);
  ~PlaylistSongList();

  unsigned int getId();
  char* get(char* key);
  bool  next();
  bool  isSelected();
  void  rewind();
  bool  isValid();


private:
  /**
   * Pointer to the data result object.
   */
  SongResult* songres;

};


#endif  // __SONGLIST_HH__
