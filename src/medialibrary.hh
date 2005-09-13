#ifndef __MEDIALIBRARY_HH__
#define __MEDIALIBRARY_HH__

#define MAX_PLAYLIST_NAME_LEN 64

#include <xmmsclient/xmmsclient.h>

#include <list>
#include <vector>
#include <iostream>

#include "playlistlist.hh"
#include "playlistsonglist.hh"
#include "querysonglist.hh"
#include "patternquery.hh"
#include "collection.hh"



using namespace std;


class MediaLibrary {

public:

  MediaLibrary(xmmsc_connection_t* connection);

  PlaylistSongList* getCurrentPlaylist();
  PlaylistSongList* getPlaylist(char* name);
  PlaylistList* getPlaylists();

  void saveCurrentPlaylistAs(char* name);
  void usePlaylist(char* name);
  void removePlaylist(char* name);

  void clearCurrentPlaylist();

  bool hasPlaylist(char* name);

  QuerySongList* searchSongs(PatternQuery* query);

  void enqueueSongs(PatternQuery* query);
  void insertSongs(PatternQuery* query, unsigned int position);
  void removeSongAt(unsigned int position);

  Collection* newCollection();
  Collection* getCollection(char* name);
  Collection* getCollections();

  const char* getCurrentPlaylistName();

  // FIXME: HACK: should be private
  void catchPlaylistChanged(xmmsc_result_t *res);
  void catchPlaylistLoaded(xmmsc_result_t *res);

private:
  xmmsc_connection_t* connection;
  xmmsc_result_t*     lastRes;

  char* currentPlaylistName;

  QuerySongList* performQuery(PatternQuery* query);

  bool validPlaylistName(char* name);

};


#endif  // __MEDIALIBRARY_HH__
