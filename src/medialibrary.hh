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


#ifndef __MEDIALIBRARY_HH__
#define __MEDIALIBRARY_HH__

#define MAX_PLAYLIST_NAME_LEN 64

// FIXME: enable to show generated SQL queries
//#define MLIB_QUERY_DEBUG

#include <xmmsclient/xmmsclient.h>

#include <list>
#include <vector>
#include <iostream>
#include <string>

#include "abstractresult.hh"
#include "richresult.hh"
#include "songresult.hh"
#include "selectresult.hh"
#include "patternquery.hh"
#include "collection.hh"
#include "delayed.hh"
#include "uri.hh"

using namespace std;


/**
 * High-level representation of the XMMS2 MediaLib.
 *
 * Any operation related to the medialib should be done with this
 * class.  Results are returned as asynchroneous Delayed objects.
 */
class MediaLibrary {
public:

  MediaLibrary(xmmsc_connection_t* connection);

  Delayed<SongResult*>* getCurrentPlaylist();
  Delayed<SongResult*>* getPlaylist(const char* name);
  Delayed<RichResult*>* getPlaylists();

  Delayed<int>* getPlaylistSize(const char* name);
  Delayed<int>* getCurrentPlaylistSize();

  Delayed<void>* saveCurrentPlaylistAs(const char* name);
  Delayed<void>* usePlaylist(const char* name);
  Delayed<void>* removePlaylist(const char* name);

  Delayed<void>* clearCurrentPlaylist();
  Delayed<void>* shuffleCurrentPlaylist();

  Delayed<bool>* hasPlaylist(const char* name);

  Delayed<RichResult*>* getSongById(unsigned int id);
  Delayed<SelectResult*>* searchSongs(PatternQuery* query);

  void enqueueSongs(AbstractResult* songlist);
  void insertSongs(AbstractResult* songlist, unsigned int position);

  Delayed<void>* removeSongAt(unsigned int position);

  Delayed<void>* import(char* uri);

  Collection* newCollection();
  Collection* getCollection(char* name);
  Collection* getCollections();

  string getCurrentPlaylistName();

  // FIXME: HACK: should be private
  void catchPlaylistChanged(xmmsc_result_t *res);
  void catchPlaylistLoaded(xmmsc_result_t *res);
  void catchPlaylistPositionChanged(xmmsc_result_t *res);

private:
  xmmsc_connection_t* connection;
  xmmsc_result_t*     lastRes;

  string currentPlaylistName;
  string newPlaylistName;

  bool validPlaylistName(const char* name);

  Delayed<void>* loadNewPlaylist();
  Delayed<void>* loadPlaylist(const char* name);
  void updateCurrentPlaylistName();

};


#endif  // __MEDIALIBRARY_HH__
