#ifndef __MEDIALIBRARY_HH__
#define __MEDIALIBRARY_HH__


#include <xmmsclient/xmmsclient.h>

#include <list>
#include <vector>

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

  void saveCurrentPlaylistAs(const char* name);
  void usePlaylist(char* name);
  void removePlaylist(const char* name);

  bool hasPlaylist(char* name);

  QuerySongList* searchSongs(PatternQuery* query);

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

  vector<int>* getEntryVector(xmmsc_result_t* res);

};


#endif  // __MEDIALIBRARY_HH__
